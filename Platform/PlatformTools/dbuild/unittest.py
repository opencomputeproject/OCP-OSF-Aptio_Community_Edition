"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""
import arguments
import os
import subprocess
import sys
import argparse
from platform_environment import is_edkii_build
from clean_paths import clean_paths
from buildtools import verify_tools
from env_var_path import get_path_string_for_env_var
from platform_environment import get_platform_json_file_contents
from edk2build import execute_edk2_tools_build

def add_argument_commands():
    """!
    installs unit test component argparse commands

    Adds Unit test arguments to argument parser

    Only logging.error will work in this function
    """
    if not is_edkii_build:
        return

    # Add parser for build flags
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        prefix_chars="-/",
        add_help=False,
    )

    parser.add_argument(
        "--showonly",
        "/showonly",
        help="Show build configuration only",
        action="store_true",
    )

    parser.add_argument(
        "--rebuild",
        "/rebuild",
        help="Rebuild the firmware. Does clean and then builds",
        action="store_true",
    )

    parser.add_argument(
        "--quick",
        "/quick",
        help="Skip as much as possible while building EDK2, and final image",
        action="store_true",
    )

    parser.add_argument(
        "--clean", "/clean", help="Clean the build completely", action="store_true"
    )

    # Add parser to build the tests
    unit_parser = arguments.subparsers.add_parser(
        "unit", add_help=False, help="Build unit test", parents=[parser]
    )

    # Specify function to execute
    unit_parser.set_defaults(func=unit_test)

def set_build_attributes():
    # Set build config
    setattr(
        arguments.args,
        "buildtarget",
        "NOOPT"
    )

    setattr(
        arguments.args,
        "tagname",
        "VS2017"
    )

    setattr(
        arguments.args,
        "arch",
        "IA32"
    )

    # Set directories and file names
    setattr(
        arguments.args,
        "amd_platform_dir",
        os.path.join(
            arguments.args.workspace,
            "Platform",
            "AmdCommonPkg",
            "Test",
        )
    )

    setattr(
        arguments.args,
        "platform",
        os.path.join(
            arguments.args.amd_platform_dir,
            "AmdCommonPkgHostTest.dsc",
        )
    )

    # Set build_output
    setattr(
        arguments.args,
        "build_output",
        os.path.join(
            arguments.args.workspace,
            "Build",
            "AmdCommonPkg",
        )
    )

    setattr(
        arguments.args,
        "selected_build_attributes",
        {}
    )

    arguments.args.selected_build_attributes["dir"] = "AmdCommonPkg"
    arguments.args.selected_build_attributes["platform"] = "Test"

def set_packages_path():
    # Get default package path
    try:
        path_list = get_platform_json_file_contents(
            "edk2build_default_packages_path.json"
        )
    except:
        raise

    for index in range(len(path_list)):
        path_list[index] = os.path.join(arguments.args.workspace, path_list[index])

    setattr(
        arguments.args,
        "packages_path",
        ";".join(path_list)
    )

def set_unit_test_env():
    # Set required build environment variables
    unit_test_env = os.environ.copy()
    unit_test_env["WORKSPACE"] = get_path_string_for_env_var(arguments.args.workspace)
    unit_test_env["PYTHON_HOME"] = get_path_string_for_env_var(arguments.args.python_home)
    unit_test_env["PYTHON_COMMAND"] = get_path_string_for_env_var(
        arguments.args.python_command
    )
    unit_test_env["AMD_PLATFORM_DIR"] = get_path_string_for_env_var(
        arguments.args.amd_platform_dir
    )
    unit_test_env["BUILD_OUTPUT"] = get_path_string_for_env_var(arguments.args.build_output)
    unit_test_env["EDK_TOOLS_PATH"] = get_path_string_for_env_var(
        os.path.abspath("edk2/BaseTools")
    )
    unit_test_env["PACKAGES_PATH"] = os.path.join(arguments.args.packages_path)
    return unit_test_env

def execute_unit_test_build(unit_test_env):
    # Create build command
    command_build_edk2 = [os.path.join("edk2", "edksetup.bat"), "&&"]
    command_build_edk2.extend(["build"])
    command_build_edk2.extend(
        ["--log", os.path.join(arguments.args.build_output, "build.log")]
    )
    command_build_edk2.extend(
        ["--report-file", os.path.join(arguments.args.build_output, "report.log")]
    )
    command_build_edk2.extend(["-b", arguments.args.buildtarget])
    command_build_edk2.extend(["-t", arguments.args.tagname])
    command_build_edk2.extend(["-p", arguments.args.platform])
    command_build_edk2.extend(["-a", arguments.args.arch])

    # Execute command
    print("Executing:")
    print(" ".join(command_build_edk2))
    returncode = 0
    if not arguments.args.showonly:
        sys.stdout.flush()
        try:
            if sys.platform.startswith("linux"):
                command_build_edk2 = " ".join(command_build_edk2)
                completed_process = subprocess.run(
                    command_build_edk2,
                    env=unit_test_env,
                    cwd=".",
                    shell=True,
                    executable="/usr/bin/bash",
                )
            else:
                completed_process = subprocess.run(
                    command_build_edk2, env=unit_test_env, cwd="."
                )
        except:
            error_text = "EDK2 build failed with exception"
            logging.error(error_text)
            raise ValueError(error_text)
    returncode = completed_process.returncode
    return returncode

def unit_test():
    """!
    Do complete unit test build

    Clean build
    Verify tools
    Set environment variables
    Build unit tests

    @returns Returns build return code
    """

    print("Building {}".format(arguments.args.command))

    # Clean build
    if arguments.args.clean or arguments.args.rebuild:
        clean_paths("clean_paths_edk2.json")
        if arguments.args.clean:
            return

    # Verify Tools
    try:
        verify_tools()
    except:
        raise

    # Set all arguments to be inputted
    set_build_attributes()
    set_packages_path()

    # Set environment variables
    unit_test_env = set_unit_test_env()

    # Make sure build output directory exists
    os.makedirs(arguments.args.build_output, exist_ok=True)

    returncode = execute_edk2_tools_build(unit_test_env)

    # Build unit tests
    if returncode == 0:
        execute_unit_test_build(unit_test_env)

    return returncode
