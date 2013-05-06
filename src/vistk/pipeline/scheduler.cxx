/*ckwg +5
 * Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "scheduler.h"
#include "scheduler_exception.h"

#include "pipeline.h"

#include <boost/thread/locks.hpp>
#ifndef BOOST_NO_HAVE_REVERSE_LOCK
#include <boost/thread/reverse_lock.hpp>
#endif
#include <boost/thread/shared_mutex.hpp>

/**
 * \file scheduler.cxx
 *
 * \brief Implementation of the base class for \link sprokit::scheduler schedulers\endlink.
 */

namespace sprokit
{

class scheduler::priv
{
  public:
    priv(scheduler* sched, pipeline_t const& pipe);
    ~priv();

    void stop();

    scheduler* const q;
    pipeline_t const p;
    bool paused;
    bool running;

    typedef boost::shared_mutex mutex_t;
    typedef boost::upgrade_lock<mutex_t> upgrade_lock_t;
    typedef boost::unique_lock<mutex_t> unique_lock_t;
#ifndef BOOST_NO_HAVE_REVERSE_LOCK
    typedef boost::reverse_lock<unique_lock_t> reverse_unique_lock_t;
#endif
    typedef boost::upgrade_to_unique_lock<mutex_t> upgrade_to_unique_lock_t;

    mutex_t mut;
};

scheduler
::~scheduler()
{
}

scheduler
::scheduler(pipeline_t const& pipe, config_t const& config)
  : d()
{
  if (!config)
  {
    throw null_scheduler_config_exception();
  }

  if (!pipe)
  {
    throw null_scheduler_pipeline_exception();
  }

  d.reset(new priv(this, pipe));
}

void
scheduler
::start()
{
  priv::unique_lock_t const lock(d->mut);

  (void)lock;

  if (d->running)
  {
    throw restart_scheduler_exception();
  }

  d->p->start();

  _start();

  d->running = true;
}

void
scheduler
::wait()
{
  priv::unique_lock_t lock(d->mut);

  if (!d->running)
  {
    throw wait_before_start_exception();
  }

  // Allow many threads to wait on the scheduler.
  {
#ifndef BOOST_NO_HAVE_REVERSE_LOCK
    priv::reverse_unique_lock_t const rev_lock(lock);

    (void)rev_lock;
#else
    lock.unlock();
#endif

    _wait();

#ifdef BOOST_NO_HAVE_REVERSE_LOCK
    lock.lock();
#endif
  }

  // After each thread, only one should call stop. Let threads through
  // one-at-a-time to see if the pipeline needs to be stopped yet.
  if (d->running)
  {
    d->stop();
  }
}

void
scheduler
::pause()
{
  priv::upgrade_lock_t lock(d->mut);

  if (!d->running)
  {
    throw pause_before_start_exception();
  }

  priv::upgrade_to_unique_lock_t const write_lock(lock);

  (void)write_lock;

  if (d->paused)
  {
    throw repause_scheduler_exception();
  }

  _pause();

  d->paused = true;
}

void
scheduler
::resume()
{
  priv::unique_lock_t const lock(d->mut);

  (void)lock;

  if (!d->running)
  {
    throw resume_before_start_exception();
  }

  if (!d->paused)
  {
    throw resume_unpaused_scheduler_exception();
  }

  _resume();

  d->paused = false;
}

void
scheduler
::stop()
{
  priv::unique_lock_t const lock(d->mut);

  (void)lock;

  if (!d->running)
  {
    throw stop_before_start_exception();
  }

  d->stop();
}

pipeline_t
scheduler
::pipeline() const
{
  return d->p;
}

scheduler::priv
::priv(scheduler* sched, pipeline_t const& pipe)
  : q(sched)
  , p(pipe)
  , paused(false)
  , running(false)
  , mut()
{
}

scheduler::priv
::~priv()
{
}

void
scheduler::priv
::stop()
{
  // Tell the subclass that we want to stop.
  q->_stop();

  // Unpause the pipeline.
  if (paused)
  {
    q->_resume();

    paused = false;
  }

  p->stop();
  running = false;
}

}
