/*ckwg +5
 * Copyright 2011-2012 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef SPROKIT_PYTHON_HELPERS_PYTHON_WRAP_CONST_SHARED_PTR_H
#define SPROKIT_PYTHON_HELPERS_PYTHON_WRAP_CONST_SHARED_PTR_H

#include <boost/python/pointee.hpp>
#include <boost/get_pointer.hpp>
#include <boost/shared_ptr.hpp>

// Retrieved from http://mail.python.org/pipermail/cplusplus-sig/2006-November/011329.html
namespace boost
{

namespace python
{

template <typename T>
inline
T*
get_pointer(boost::shared_ptr<T const> const& p)
{
  return const_cast<T*>(p.get());
}

template <typename T>
struct pointee<boost::shared_ptr<T const> >
{
  typedef T type;
};

// Don't hide other get_pointer instances.
using boost::python::get_pointer;
using boost::get_pointer;

}

}

#endif // SPROKIT_PYTHON_HELPERS_PYTHON_WRAP_CONST_SHARED_PTR_H
