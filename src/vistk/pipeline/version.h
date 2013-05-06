/*ckwg +5
 * Copyright 2012 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef SPROKIT_PIPELINE_VERSION_H
#define SPROKIT_PIPELINE_VERSION_H

#include "pipeline-config.h"

#include <boost/cstdint.hpp>

#include <string>

/**
 * \file version.h
 *
 * \brief Runtime version checks.
 */

namespace sprokit
{

/**
 * \class version "version.h" <sprokit/pipeline/version.h>
 *
 * \brief Runtime version information.
 */
class SPROKIT_PIPELINE_EXPORT version
{
  public:
    /// The type of version components.
    typedef uint64_t version_t;

    /// The major version number.
    static version_t const major;
    /// The minor version number.
    static version_t const minor;
    /// The patch version number.
    static version_t const patch;
    /// The version string.
    static std::string const version_string;

    /// True if information from the git repository is available, false otherwise.
    static bool const git_build;
    /// The full git hash of the build.
    static std::string const git_hash;
    /// An abbreviated git hash of the build.
    static std::string const git_hash_short;
    /// Empty if the git repository was clean at the time of the build.
    static std::string const git_dirty;

    /**
     * \brief Check whether the runtime version is at least some value.
     *
     * \param major_ The major version to test.
     * \param minor_ The minor version to test.
     * \param patch_ The patch version to test.
     *
     * \returns True if the runtime version is at least the given version.
     */
    static bool check(version_t major_, version_t minor_, version_t patch_);
};

}

#endif // SPROKIT_PIPELINE_VERSION_H
