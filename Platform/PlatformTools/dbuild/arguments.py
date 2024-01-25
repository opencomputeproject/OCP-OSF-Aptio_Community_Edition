"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import argparse
import os
import sys
import logging
import importlib
from env_var_path import get_path_env_var
from platform_environment import get_platform_json_file_contents

parser = None
subparsers = None
args = None


def parse_args():
    """!
    Build argument parser from platform selected modules

    Sets args module global variable and creates global parser and subparsers
    to install commands into

    Calls add_argument_commands() from each python script listed in
    arguments_search_list.json

    Finally parses the arguments into the globally accessable args variable

    Only logging.error will work in this function
    """
    global parser
    global subparsers
    global args

    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        prefix_chars="-/",
        description="""
            Build a platform Firmware image
            """,
    )

    subparsers = parser.add_subparsers(
        title="Commands",
        dest="command",
        required=True,
    )

    parser.add_argument(
        "-v",
        "--verbosity",
        choices=["CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG"],
        default="ERROR",
        help="Logging level",
    )

    # Get the list of directories to search for supported builds
    # arguments_search_list.json format is a list of module names without ".py"
    # extension:
    #   ["module1", "module2", "module3"]
    try:
        python_scripts = get_platform_json_file_contents("arguments_search_list.json")
    except:
        raise

    # For each module, call module.add_argument_commands()
    for script in python_scripts:
        logging.debug("Checking for commands from {}.py".format(script))
        try:
            module = importlib.import_module(script)
        except:
            continue
        if "add_argument_commands" in dir(module):
            module.add_argument_commands()

    # Parse the arguments and store additional inforamation in args
    args = parser.parse_args()

    # Insert selected build attributes
    setattr(args, "workspace", os.getcwd())

    # Store PYTHON_HOME or infer from python executable
    python_home = get_path_env_var("PYTHON_HOME")
    if python_home == None:
        python_home = os.path.dirname(sys.executable)
    setattr(args, "python_home", python_home)
    if sys.platform.startswith("linux"):
        python_exe = "python3"
    elif sys.platform.startswith("win"):
        python_exe = "python.exe"
    setattr(args, "python_exe", os.path.join(python_home, python_exe))
    setattr(args, "python_command", os.path.join(python_home, python_exe))
