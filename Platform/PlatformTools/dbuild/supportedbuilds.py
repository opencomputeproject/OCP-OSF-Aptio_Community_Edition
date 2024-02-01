"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""
import os
import logging
import json
import arguments
from external import external_only
from platform_environment import get_platform_json_file_contents


def _verify_contents(supported_builds):
    required_attributes = [
        "platform",
        "sku",
        "soc",
        "soc_sku",
        "soc_family",
        "board",
        "build",
        "secure",
        "simnow",
        "emulation",
        "cbs",
        "dir",
    ]
    success = True
    for build in supported_builds:
        attributes = supported_builds[build]
        for attrib in required_attributes:
            if attrib not in attributes:
                success = False
                logging.error('Required item "{}" not in "{}"'.format(attrib, build))
    return success


def get_supported_builds():
    """!
    Collect a dictionary of supported build information

    Reads supportedbuilds_search_dirs.json for a list of directories to search
    for supported build JSON files.
    Example:
        ["search_dir_1", "search_dir_2"]

    The directories are searched for SupportedBuilds.json files to collect
    supported builds.
    Example:
        {
          "rs1oe": {
              "platform": "OnyxBoardPkg",   # BoardPkg dir with Project files
              "sku": "1O",
              "soc": "GENOA",
              "soc_family": "0x19",
              "soc_sku": "GENOA",
              "board": "GENOAONYX",
              "build": "EXTERNAL",
              "secure": "False",
              "simnow": "False",
              "emulation": "False",
              "cbs": "True",
              "dir": "GenoaOpenBoardPkg"    # Dir with Collection of BoardPkgs
            }
        }
    """
    all_supported_builds = {}
    supported_builds_file = "SupportedBuilds.json"

    # Get the list of directories to search for supported builds
    try:
        supported_builds_search_dir_list = get_platform_json_file_contents(
            "supportedbuilds_search_dirs.json"
        )
    except:
        raise

    for supported_build_search_dir in supported_builds_search_dir_list:
        for root, dirs, files in os.walk(supported_build_search_dir):
            for file in files:
                if file.lower() == supported_builds_file.lower():
                    fqn = os.path.join(root, file)
                    try:
                        with open(fqn, "r", encoding="utf-8") as json_file:
                            supported_builds = json.load(json_file)
                    except:
                        error_text = "Unable collect supported builds from {}".format(
                            fqn
                        )
                        logging.error(error_text)

                    if _verify_contents(supported_builds):
                        all_supported_builds = {
                            **all_supported_builds,
                            **supported_builds,
                        }

    if all_supported_builds == {}:
        error_text = "No supported build files found"
        logging.error(error_text)

    supported_builds = {}
    for supported_build in all_supported_builds:
        build_attributes = all_supported_builds[supported_build]
        if external_only():
            if build_attributes["build"] == "EXTERNAL":
                supported_builds[supported_build] = build_attributes
        else:
            supported_builds[supported_build] = build_attributes

    return supported_builds


def get_build_information_string(build_attributes):
    attributes_string = "{}, {}, secure={}".format(
        build_attributes["platform"],
        build_attributes["build"],
        build_attributes["secure"],
    )
    return attributes_string


def set_build_arguments():
    # Set default buildtarget
    if arguments.args.selected_build_attributes["build"] == "INTERNAL":
        setattr(arguments.args, "buildtarget", "DEBUG")
    else:
        setattr(arguments.args, "buildtarget", "RELEASE")

    # Set default platform
    platform_rel_path = "/".join(
        [
            "Platform",
            arguments.args.selected_build_attributes["dir"],
            arguments.args.selected_build_attributes["platform"],
        ]
    )
    platform = "/".join([platform_rel_path, "Project.dsc"])
    setattr(arguments.args, "platform", platform)

    # Set Platform location
    setattr(
        arguments.args,
        "amd_platform_dir",
        os.path.normpath(
            os.path.join(
                arguments.args.workspace,
                "Platform",
                arguments.args.selected_build_attributes["dir"],
                arguments.args.selected_build_attributes["platform"],
            )
        ),
    )
    setattr(
        arguments.args,
        "amd_common_platform_dir",
        os.path.normpath(
            os.path.join(
                arguments.args.workspace,
                "Platform",
                arguments.args.selected_build_attributes["dir"],
            )
        ),
    )


def set_build_env(env):
    env["SOC"] = arguments.args.selected_build_attributes["soc"]
    env["SOC2"] = arguments.args.selected_build_attributes["soc2"]
    env["SOC_FAMILY"] = arguments.args.selected_build_attributes["soc_family"]
    env["SOC_SKU"] = arguments.args.selected_build_attributes["soc_sku"]
    env["SOCKET"] = arguments.args.selected_build_attributes["socket"]
    env["AMD_PLATFORM_BUILD_TYPE"] = arguments.args.selected_build_attributes["build"]
    env["SIMNOW_SUPPORT"] = arguments.args.selected_build_attributes["simnow"]
    env["EMULATION"] = arguments.args.selected_build_attributes["emulation"]
    env["SIL_PLATFORM_NAME"] = arguments.args.selected_build_attributes["platform_name"]


def set_build_defines(command):
    command.extend(
        ["-D", "PLATFORM_CRB=" + arguments.args.selected_build_attributes["board"]]
    )
    command.extend(
        ["-D", "AMD_PROCESSOR=" + arguments.args.selected_build_attributes["soc"]]
    )
    command.extend(
        ["-D", "CBS_INCLUDE=" + arguments.args.selected_build_attributes["cbs"].upper()]
    )


