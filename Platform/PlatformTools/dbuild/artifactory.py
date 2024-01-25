"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""
import shutil
import sys
import os
import subprocess
import urllib.request
import logging
import arguments
import stat
from external import external_only
from platform_environment import get_platform_json_file_contents


def add_argument_commands():
    """!
    installs Artifactory component argparse commands

    Adds artifactory commands to argument parser if this BIOS has the internal
    components remaining in the tree.

    Parses platform artifactory_manifests.json file contains named commands to
    indicate which XML files to process or groups of commands to to execute.
    Also the location of jfrog utility to download.
    Example:
        {
          "win": {
            "jfrog": {
              "name": "location",
              "src": "path/to/file",
              "dst": "path/jfrog.exe"
            },
            "firmwares": {"artifactory": ["path/to/firmwares.xml"]},
            "tools": {"artifactory": ["path/to/tools.xml"]},
            "platbins": {"artifactory": ["path/to/PlatformBinaries.xml"]},
            "bins": {"group": ["firmwares", "platbins", "tools"]}
          },
        }

    Only logging.error will work in this function
    """
    if not external_only():
        try:
            manifests_dict = get_platform_json_file_contents(
                "artifactory_manifests.json"
            )
        except:
            raise
        # Parse all entries
        for os, manifests in manifests_dict.items():
            if not sys.platform.startswith(os):
                continue
            for key, value in manifests.items():
                artifactory_list = value.get("artifactory", None)
                if artifactory_list != None:
                    parser = arguments.subparsers.add_parser(
                        key, help="Download {} from Artifactory".format(key)
                    )
                    # Specify function to execute
                    parser.set_defaults(func=artifactory)
                    parser.set_defaults(artifactory_list=artifactory_list)
                    parser.set_defaults(artifactory_manifest=manifests)
                    continue

                bins_list = value.get("group", None)
                if bins_list != None:
                    parser = arguments.subparsers.add_parser(
                        key, help="Download {} from Artifactory".format(key)
                    )
                    # Specify function to execute
                    parser.set_defaults(func=collect)
                    parser.set_defaults(collect_list=bins_list)
                    parser.set_defaults(artifactory_manifest=manifests)
                    continue


def artifactory():
    """!
    Download Artifactory components via relic and manifest files
    """

    relic = os.path.normpath(os.path.abspath("AmdCommonTools/relic/relic.py"))

    art_env = os.environ.copy()

    # Process list of manifest files through relic
    for manifest in arguments.args.artifactory_list:
        print("Processing: {}".format(manifest))
        sys.stdout.flush()
        completed_process = subprocess.run(
            [arguments.args.python_exe, relic, "-x", manifest], env=art_env
        )
        if completed_process.returncode != 0:
            error_text = "Processing Failed: {}".format(manifest)
            logging.error(error_text)
            raise ValueError(error_text)
    return completed_process.returncode


def collect():
    """!
    Collect a group of artifactory components in a single command
    """
    art_manifest = arguments.args.artifactory_manifest
    for component in arguments.args.collect_list:
        art_dict = art_manifest.get(component, None)
        if art_dict == None:
            error_text = "Component group entry {} does not exist".format(component)
            logging.error(error_text)
            raise ValueError(error_text)

        art_list = art_dict.get("artifactory", None)
        if art_list == None:
            error_text = (
                'Component group entry "{}" does not have ' + "artfactory element"
            ).format(component)
            logging.error(error_text)
            raise ValueError(error_text)
        else:
            setattr(arguments.args, "artifactory_list", art_dict["artifactory"])
            return_code = artifactory()
            if return_code != 0:
                return return_code
