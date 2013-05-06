/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef SPROKIT_PIPELINE_CONFIG_H_
#define SPROKIT_PIPELINE_CONFIG_H_

#include <sprokit/config.h>

/**
 * \file pipeline-config.h
 *
 * \brief Defines for symbol visibility in pipeline.
 */

#ifndef SPROKIT_PIPELINE_EXPORT
#ifdef MAKE_SPROKIT_PIPELINE_LIB
/// Export the symbol if building the library.
#define SPROKIT_PIPELINE_EXPORT SPROKIT_EXPORT
#else
/// Import the symbol if including the library.
#define SPROKIT_PIPELINE_EXPORT SPROKIT_IMPORT
#endif // MAKE_SPROKIT_PIPELINE_LIB
/// Hide the symbol from the library interface.
#define SPROKIT_PIPELINE_NO_EXPORT SPROKIT_NO_EXPORT
#endif // SPROKIT_PIPELINE_EXPORT

#ifndef SPROKIT_PIPELINE_EXPORT_DEPRECATED
/// Mark as deprecated.
#define SPROKIT_PIPELINE_EXPORT_DEPRECATED SPROKIT_DEPRECATED SPROKIT_PIPELINE_EXPORT
#endif // SPROKIT_PIPELINE_EXPORT_DEPRECATED

#endif // SPROKIT_PIPELINE_CONFIG_H_
