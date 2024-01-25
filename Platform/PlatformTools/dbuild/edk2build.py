"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import subprocess
import argparse
import sys
import os
import logging
import arguments
import string
from datetime import datetime
from env_var_path import get_path_string_for_env_var
from overrides import overrides, restore_overrides
from supportedbuilds import get_supported_builds
from supportedbuilds import get_build_information_string
from supportedbuilds import set_build_env
from supportedbuilds import set_build_arguments
from supportedbuilds import set_build_defines
from buildtools import verify_tools
from clean_paths import clean_paths
from platform_environment import get_platform_json_file_contents
from platform_environment import is_edkii_build


def date_parser(datestring):
    """!
    Validate entered date. Exit if date string not mm/dd/YYYY

    argparse help message indicates format required.

    @param  datestring      Datestring to parse

    @returns    date string as mm/dd/YYYY
    """
    try:
        entered_date = datetime.strptime(datestring, "%m/%d/%Y")
    except ValueError:
        error_text = "Exiting: Invalid date entered: {}.".format(datestring)
        logging.error(error_text)
        raise ValueError(error_text)

    return entered_date.strftime("%m/%d/%Y")


def add_argument_commands():
    """!
    Insert build tip commands into the argument parser

    Only logging.error will work in this function
    """
    # Only supports EDKII build
    if not is_edkii_build():
        return

    # Get current date/time to default date/time
    now = datetime.now()

    # Crate Default options to add to all subparsers
    edk2_common_parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        prefix_chars="-/",
        add_help=False,
    )
    edk2_common_parser.add_argument(
        "--showonly",
        "/showonly",
        help="Show build configuration only",
        action="store_true",
    )
    edk2_common_parser.add_argument(
        "--rebuild",
        "/rebuild",
        help="Rebuild the firmware. Does clean and then builds",
        action="store_true",
    )
    edk2_common_parser.add_argument(
        "--clean", "/clean", help="Clean the build completely", action="store_true"
    )
    edk2_common_parser.add_argument(
        "--quick",
        "/quick",
        help="Skip as much as possible while building EDK2, and final image",
        action="store_true",
    )
    edk2_common_parser.add_argument(
        "--tag", "/tag", help='Append "label" to firmware name'
    )
    edk2_common_parser.add_argument(
        "--edk2args",
        "/edk2args",
        default="",
        help="Arguments to pass to EDK2 build surrounded by double quotes",
    )
    edk2_common_parser.add_argument(
        "--date",
        "/date",
        type=date_parser,
        default=now.strftime("%m/%d/%Y"),
        help="Set the date for the BIOS mm/dd/yyyy",
    )
    edk2_common_parser.add_argument(
        "--build-cbs",
        "/build-cbs",
        help="On released PI re-generate CBS files",
        action="store_true",
    )

    # Add Commands for all supported builds
    supported_builds = get_supported_builds()
    for build in supported_builds:
        build_attributes = supported_builds[build]
        build_tip = arguments.subparsers.add_parser(
            build,
            formatter_class=argparse.ArgumentDefaultsHelpFormatter,
            prefix_chars="-/",
            help=get_build_information_string(build_attributes),
            parents=[edk2_common_parser],
        )

        # Specify function to execute
        build_tip.set_defaults(func=edk2build)
        # set build_attribute details
        build_tip.set_defaults(selected_build_attributes=build_attributes)

        # Add packages_path list
        build_tip.add_argument(
            "--packages_path",
            "/packages_path",
            nargs="*",
            default=get_default_packages_path(build_attributes),
            help="Package Paths to search",
        )

        # Build default file name: (T|R)(2-digit sku)00000000(secure-S|N).FD
        default_name = ""
        if build_attributes["build"] == "INTERNAL":
            default_name = "T"
        else:
            default_name = "R"
        if build_attributes["secure"] == "True":
            secure = "S"
        else:
            secure = "N"
        default_name += build_attributes["sku"] + "00000000" + secure

        build_tip.add_argument(
            "--name", "/name", default=default_name, help="Set the name for the BIOS"
        )


def get_default_packages_path(build_attributes):
    """!
    Retreive default value for PACKAGES_PATH environment variable

    Base settings will be in default_packages_path.json file.
    Insert Index 1 as *OpenBoardPkg as described in gathered supported_builds.
    Append current working directory which is equivalent to workspace.

    edk2build_default_packages_path.json is a list of directory names.
    Example:
        [
          "Platform",
          "edk2",
          "edk2-platforms/Platform/Intel",
          "edk2-platforms/Features/Intel"
        ]

    @params     build_attributes        Parameters for each supported build

    @returns    packages_path_list      A python list containing PACKAGES_PATH
                                        entries
    """
    try:
        packages_path_list = get_platform_json_file_contents(
            "edk2build_default_packages_path.json"
        )
    except:
        raise
    # Change each one to the absolute path
    for index in range(len(packages_path_list)):
        packages_path_list[index] = os.path.normpath(
            os.path.abspath(packages_path_list[index])
        )
    # Insert at index 1 location of *BoardPkg(s) as an extension of index 0
    packages_path_list.insert(
        1,
        os.path.normpath(os.path.join(packages_path_list[0], build_attributes["dir"])),
    )
    # append current working directory which better be workspace
    packages_path_list.append(os.path.normpath(os.getcwd()))
    return packages_path_list


def parse_edk2_override_args():
    """!
    Parse --edk2args="<options>"

    Parse overrides for which a selected build will generate defaults

    Override args values generated for the selected build.
    """
    argparser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        usage="EDK2 Overrides [options]",
        description="""
            Handle options passed via --edk2args="<options>".  Support
            overriding options for which a selected build will create defaults
            """,
        add_help=False,
    )
    argparser.add_argument(
        "-p",
        "--platform",
        help="Build the platform specified by the DSC file name argument.",
    )
    argparser.add_argument(
        "-m",
        "--module",
        help="Build the module specified by the INF file name argument.",
    )
    argparser.add_argument(
        "-b",
        "--buildtarget",
        choices=["RELEASE", "DEBUG", "NOOPT"],
        help="Using the TARGET to build the platform.",
    )
    argparser.add_argument(
        "-t", "--tagname", help="Using the Tool Chain Tagname to build the platform."
    )
    argparser.add_argument(
        "-a",
        "--arch",
        choices=["IA32", "X64"],
        default=["IA32", "X64"],
        nargs="+",
        help="ARCHS is one of list: IA32, X64. To specify more archs, "
        + "please repeat this option.",
    )

    # Set default values at args level defined by selected platform

    if arguments.args.selected_build_attributes != None:
        set_build_arguments()
    else:
        # Default settings to None
        setattr(arguments.args, "buildtarget", None)
        setattr(arguments.args, "platform", None)
        setattr(arguments.args, "module", None)

    if arguments.args.edk2args != None:
        edk2_override_args, edk2_extra_args = argparser.parse_known_args(
            arguments.args.edk2args.split()
        )
        setattr(arguments.args, "edk2_override_args", edk2_override_args)
        setattr(arguments.args, "edk2_extra_args", edk2_extra_args)
    else:
        setattr(arguments.args, "edk2_override_args", None)
        setattr(arguments.args, "edk2_extra_args", None)
    if arguments.args.edk2_override_args.platform != None:
        setattr(arguments.args, "platform", arguments.args.edk2_override_args.platform)
    if arguments.args.edk2_override_args.buildtarget != None:
        setattr(
            arguments.args, "buildtarget", arguments.args.edk2_override_args.buildtarget
        )
    if arguments.args.edk2_override_args.module != None:
        setattr(arguments.args, "module", arguments.args.edk2_override_args.module)
    if arguments.args.edk2_override_args.tagname != None:
        setattr(arguments.args, "tagname", arguments.args.edk2_override_args.tagname)


def set_edk2_env():
    # Set required build environment variables
    edk2_env = os.environ.copy()
    edk2_env["WORKSPACE"] = get_path_string_for_env_var(arguments.args.workspace)
    edk2_env["PYTHON_HOME"] = get_path_string_for_env_var(arguments.args.python_home)
    edk2_env["PYTHON_COMMAND"] = get_path_string_for_env_var(
        arguments.args.python_command
    )
    edk2_env["AMD_PLATFORM_DIR"] = get_path_string_for_env_var(
        arguments.args.amd_platform_dir
    )
    edk2_env["AMD_COMMON_PLATFORM_DIR"] = get_path_string_for_env_var(
        arguments.args.amd_common_platform_dir
    )
    edk2_env["BUILD_OUTPUT"] = get_path_string_for_env_var(arguments.args.build_output)

    set_build_env(edk2_env)

    edk2_env["EDK_TOOLS_PATH"] = get_path_string_for_env_var(
        os.path.abspath("edk2/BaseTools")
    )
    edk2_env["FIRMWARE_VERSION_STR"] = arguments.args.name
    edk2_env["PACKAGES_PATH"] = os.pathsep.join(arguments.args.packages_path)
    if arguments.args.quick:
        edk2_env["BUILD_QUICK"] = "TRUE"
    if arguments.args.showonly:
        edk2_env["BUILD_SHOW_ONLY"] = "TRUE"
    if getattr(arguments.args, "module", None) != None:
        edk2_env["BUILD_MODULE_ONLY"] = "TRUE"
    if arguments.args.build_cbs == True:
        edk2_env["CBS_FORCE_BUILD"] = "TRUE"

    # If show only and debug
    logging.debug("Printing environment variables")
    for var in edk2_env:
        logging.debug("{}={}".format(var, edk2_env[var]))
    return edk2_env


def execute_edk2_tools_build(edk2_env):
    """!
    Excute the EDK2 tools build

    @param edk2_env     Environment to pass to command shell

    @returns            Return code from edk2 tools build
    """
    # Create command for building BaseTools
    if sys.platform.startswith("linux"):
        # Generate command to build tools under Linux
        command_build_tools = [""]
        if arguments.args.rebuild:
            command_build_tools.extend(
                ["make", "-C", os.path.join("edk2", "BaseTools"), "clean"]
            )
            command_build_tools.extend(["&&"])
        command_build_tools.extend(["make", "-C", os.path.join("edk2", "BaseTools")])
    else:
        # Generate command to build tools under Windows
        command_build_tools = [os.path.join("edk2", "edksetup.bat")]
        if arguments.args.rebuild:
            command_build_tools.extend(["ForceRebuild"])
        else:
            command_build_tools.extend(["Rebuild"])

    # Build tools
    returncode = 0
    print("Executing:")
    print(" ".join(command_build_tools))
    #    if not arguments.args.showonly and not arguments.args.quick:
    print(os.getcwd())
    if not arguments.args.showonly and not arguments.args.quick:
        sys.stdout.flush()
        if sys.platform.startswith("linux"):
            command_build_tools = " ".join(command_build_tools)
            try:
                completed_process = subprocess.run(
                    command_build_tools,
                    env=edk2_env,
                    cwd=".",
                    shell=True,
                    executable="/usr/bin/bash",
                )
            except:
                error_text = "EDK2 build of tools failed with exception"
                logging.error(error_text)
                raise ValueError(error_text)
        else:
            try:
                completed_process = subprocess.run(
                    command_build_tools, env=edk2_env, cwd="."
                )
            except:
                error_text = "EDK2 build of tools failed with exception"
                logging.error(error_text)
                raise ValueError(error_text)

        returncode = completed_process.returncode
    else:
        print("Skipping tool build")
    return returncode


def calculate_revision_num(name):
    # Skip first three characters (T|R)(SOC_SKU)
    start = 3
    # Skip last character
    end = len(name) - 1
    # strip off any preceding character that are not hex digits
    for index in range(start, end):
        if name[index : index + 1] not in string.hexdigits:
            start = index + 1
    if start == end:
        revision_number_string = "0"
    else:
        revision_number_string = name[start:end]
    # Now have some string. Convert to int first, only keep 32-bits, then
    # convert back to string
    try:
        revision_number = int(revision_number_string, 10)
        revision_number &= 0xFFFFFFFF
        output_revision_number_string = str(revision_number)
    except:
        try:
            revision_number = int(revision_number_string, 16)
            revision_number &= 0xFFFFFFFF
            output_revision_number_string = hex(revision_number)
        except:
            error_text = "{} was not an integer or hexidecimal string".format(
                revision_number_string
            )
            logging.error(error_text)
            raise ValueError(error_text)

    return output_revision_number_string


def execute_edk2_build(edk2_env):
    """!
    Excute the EDK2 build

    @param edk2_env     Environment to pass to command shell

    @returns            Return code from edk2 build
    """
    if sys.platform.startswith("linux"):
        # Start command to build EDK2
        command_build_edk2 = ["source", os.path.join("edk2", "edksetup.sh")]
        command_build_edk2.extend(["BaseTools"])
        command_build_edk2.extend(["&&"])
    else:
        # Start command to build EDK2
        command_build_edk2 = [os.path.join("edk2", "edksetup.bat"), "&&"]

    # Create command for edk2 main build
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
    module = getattr(arguments.args, "module", None)
    if module != None:
        command_build_edk2.extend(["-m", arguments.args.module])
    for arch in arguments.args.edk2_override_args.arch:
        command_build_edk2.extend(["-a", arch])

    command_build_edk2.extend(["-D", "RELEASE_DATE=" + arguments.args.date])

    command_build_edk2.extend(
        ["-D", "FIRMWARE_REVISION_NUM=" + calculate_revision_num(arguments.args.name)]
    )

    command_build_edk2.extend(["-D", "FIRMWARE_VERSION_STR=" + arguments.args.name])

    # Set selected build defines
    set_build_defines(command_build_edk2)

    extra_args = getattr(arguments.args, "edk2_extra_args", None)
    if extra_args != None:
        command_build_edk2.extend(extra_args)

    print("Executing:")
    print(" ".join(command_build_edk2))
    returncode = 0
    if not arguments.args.showonly:
        sys.stdout.flush()
        if sys.platform.startswith("linux"):
            command_build_edk2 = " ".join(command_build_edk2)
            try:
                completed_process = subprocess.run(
                    command_build_edk2,
                    env=edk2_env,
                    cwd=".",
                    shell=True,
                    executable="/usr/bin/bash",
                )
            except:
                error_text = "EDK2 build failed with exception"
                logging.error(error_text)
                raise ValueError(error_text)
        else:
            try:
                completed_process = subprocess.run(
                    command_build_edk2, env=edk2_env, cwd="."
                )
            except:
                error_text = "EDK2 build failed with exception"
                logging.error(error_text)
                raise ValueError(error_text)
        returncode = completed_process.returncode
    else:
        print("Skipping edk2 build")
    return returncode


def edk2build():
    """!
    Do complete EDK2 build

    Clean the build if requested.
    Verify the tools.
    Override any files.
    Build the EDK2 tools
    Build EDK2
    Restore the overrides

    @returns                Returns edk2 build return code
    """
    print("\nBuilding {}".format(arguments.args.command))
    # Parse and configure edk2 override args
    parse_edk2_override_args()
    if arguments.args.clean or arguments.args.rebuild:
        clean_paths("clean_paths_edk2.json")
        if arguments.args.clean:
            return

    try:
        verify_tools()
    except:
        raise

    tagname = getattr(arguments.args, "tagname", None)
    if tagname == None:
        error_text = (
            "Edk2 build tagname not defined.  Specify a "
            + "tagname for a properly installed toolchain.\n"
            + "Use the edk2args override parameter if needed."
        )
        logging.error(error_text)
        raise ValueError(error_text)

    # Set final build output location
    setattr(
        arguments.args,
        "build_output",
        os.path.normpath(
            os.path.join(
                arguments.args.workspace,
                "Build",
                "{}_{}_{}".format(
                    arguments.args.selected_build_attributes["platform"],
                    arguments.args.selected_build_attributes["soc"],
                    arguments.args.selected_build_attributes["build"],
                ),
                "{}_{}".format(arguments.args.buildtarget, arguments.args.tagname),
            )
        ),
    )
    # Make the output directory so build.log can be created.
    os.makedirs(arguments.args.build_output, exist_ok=True)

    # Configure environment variables
    edk2_env = set_edk2_env()

    # Only print
    if arguments.args.showonly:
        print("SHOW ONLY: display what commands would be")

    print(
        "\nRemoving previously built BIOS images for {} if any".format(
            arguments.args.command
        )
    )
    if os.path.exists(arguments.args.name + ".FD"):
        os.remove(os.path.join(arguments.args.workspace, arguments.args.name + ".FD"))
    if os.path.exists(arguments.args.name + ".tar.gz"):
        os.remove(
            os.path.join(arguments.args.workspace, arguments.args.name + ".tar.gz")
        )

    # Override files
    overrides()

    returncode = execute_edk2_tools_build(edk2_env)

    # Build BIOS
    if returncode == 0:
        returncode = execute_edk2_build(edk2_env)

    # Restore overridden files
    restore_overrides()
    return returncode
