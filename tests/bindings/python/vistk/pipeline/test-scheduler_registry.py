#!@PYTHON_EXECUTABLE@
#ckwg +4
# Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
# KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
# Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.


def test_import():
    try:
        from sprokit.pipeline import config
        import sprokit.pipeline.scheduler_registry
    except:
        test_error("Failed to import the scheduler_registry module")


def test_create():
    from sprokit.pipeline import config
    from sprokit.pipeline import scheduler_registry

    scheduler_registry.SchedulerRegistry.self()
    scheduler_registry.SchedulerType()
    scheduler_registry.SchedulerTypes()
    scheduler_registry.SchedulerDescription()
    scheduler_registry.SchedulerModule()


def test_api_calls():
    from sprokit.pipeline import config
    from sprokit.pipeline import modules
    from sprokit.pipeline import pipeline
    from sprokit.pipeline import scheduler_registry

    modules.load_known_modules()

    reg = scheduler_registry.SchedulerRegistry.self()

    sched_type = 'thread_per_process'
    c = config.empty_config()
    p = pipeline.Pipeline()

    reg.create_scheduler(sched_type, p)
    reg.create_scheduler(sched_type, p, c)
    reg.types()
    reg.description(sched_type)
    reg.default_type


def example_scheduler():
    from sprokit.pipeline import scheduler

    class PythonExample(scheduler.PythonScheduler):
        def __init__(self, pipe, conf):
            scheduler.PythonScheduler.__init__(self, pipe, conf)

            self.ran_start = False
            self.ran_wait = False
            self.ran_stop = False

        def start(self):
            self.ran_start = True

        def wait(self):
            self.ran_wait = True

        def stop(self):
            self.ran_stop = True

        def check(self):
            if not self.ran_start:
                test_error("start override was not called")
            if not self.ran_wait:
                test_error("wait override was not called")
            if not self.ran_stop:
                test_error("stop override was not called")

    return PythonExample


def test_register():
    from sprokit.pipeline import config
    from sprokit.pipeline import modules
    from sprokit.pipeline import pipeline
    from sprokit.pipeline import scheduler_registry

    modules.load_known_modules()

    reg = scheduler_registry.SchedulerRegistry.self()

    sched_type = 'python_example'
    sched_desc = 'simple description'

    reg.register_scheduler(sched_type, sched_desc, example_scheduler())

    if not sched_desc == reg.description(sched_type):
        test_error("Description was not preserved when registering")

    p = pipeline.Pipeline()

    try:
        s = reg.create_scheduler(sched_type, p)
        if s is None:
            raise Exception()
    except:
        test_error("Could not create newly registered scheduler type")


def test_wrapper_api():
    from sprokit.pipeline import config
    from sprokit.pipeline import modules
    from sprokit.pipeline import pipeline
    from sprokit.pipeline import scheduler_registry

    sched_type = 'python_example'
    sched_desc = 'simple description'

    reg = scheduler_registry.SchedulerRegistry.self()

    reg.register_scheduler(sched_type, sched_desc, example_scheduler())

    p = pipeline.Pipeline()

    def check_scheduler(s):
        if s is None:
            test_error("Got a 'None' scheduler")
            return

        s.start()
        s.wait()
        s.stop()

        s.check()

    s = reg.create_scheduler(sched_type, p)
    check_scheduler(s)


if __name__ == '__main__':
    import os
    import sys

    if not len(sys.argv) == 4:
        test_error("Expected three arguments")
        sys.exit(1)

    testname = sys.argv[1]

    os.chdir(sys.argv[2])

    sys.path.append(sys.argv[3])

    tests = \
        { 'import': test_import
        , 'create': test_create
        , 'api_calls': test_api_calls
        , 'register': test_register
        , 'wrapper_api': test_wrapper_api
        }

    from sprokit.test.test import *

    run_test(testname, tests)
