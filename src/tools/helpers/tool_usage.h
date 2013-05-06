/*ckwg +5
 * Copyright 2012-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#ifndef SPROKIT_TOOL_HELPERS_TOOL_USAGE_H
#define SPROKIT_TOOL_HELPERS_TOOL_USAGE_H

#include <sprokit/config.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

void SPROKIT_NO_RETURN tool_usage(int ret, boost::program_options::options_description const& options);
void tool_version_message();

boost::program_options::options_description tool_common_options();

boost::program_options::variables_map tool_parse(int argc, char* argv[], boost::program_options::options_description const& desc);

#endif // SPROKIT_TOOL_HELPERS_TOOL_USAGE_H
