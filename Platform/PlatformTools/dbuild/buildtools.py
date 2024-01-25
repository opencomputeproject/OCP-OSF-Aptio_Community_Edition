"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import sys
import os
import logging
import arguments
from platform_environment import get_platform_json_file_contents


def verify_amd_tool_location(prefix, tool, parameters):
    tagname = parameters.get("tagname", None)
    variable_value_list = parameters.get("variable_value_list", None)
    if variable_value_list == None:
        return
    for variable, value in variable_value_list:
        # os.path functions remove trailing "/" so store if we ned to add it back
        if value.endswith("/"):
            append_separator = True
        else:
            append_separator = False
        # Convert to full path for current OS
        full_path = os.path.normpath(os.path.abspath(value))

        # Retrieve current variable value if any
        current_value = os.environ.get(variable, None)
        if not os.path.exists(full_path):
            error = "path does not exist for {}:{}={}".format(tool, variable, value)
            logging.debug(error)

        if current_value != None and not current_value in full_path:
            set_value = os.pathsep.join([full_path, current_value])
        else:
            set_value = full_path

        if append_separator == True:
            set_value = set_value + os.path.sep

        os.environ[variable] = set_value

    if tagname != None:
        setattr(arguments.args, "tagname", tagname)


def verify_tool(tool, parameters):
    prefix = parameters.get("prefix", None)
    prefix_startswith = parameters.get("prefix_startswith", None)
    prefix_endswith = parameters.get("prefix_endswith", None)
    # Ignore if environment variable set outside of dbuild
    if prefix != None:
        if os.environ.get(prefix, None) != None:
            return
    # Must use both startswith and endswith if either is used
    if prefix_startswith != None and prefix_endswith != None:
        for var in os.environ:
            if var.startswith(prefix_startswith) and var.endswith(prefix_endswith):
                return
    verify_amd_tool_location(prefix, tool, parameters)


def verify_tools():
    buildtoolsjson = "buildtools.json"
    try:
        buildtools_dict = get_platform_json_file_contents(buildtoolsjson)
    except:
        raise
    # Look for supported OS
    for supported_os, build_tools in buildtools_dict.items():
        if not sys.platform.startswith(supported_os):
            continue
        else:
            # Parse through all tools for this OS
            for tool, parameters in build_tools.items():
                if not os.path.exists(tool):
                    logging.debug(
                        "{} does not exist, assuming tool configured properly".format(
                            tool
                        )
                    )
                    continue
                verify_tool(tool, parameters)
            break
