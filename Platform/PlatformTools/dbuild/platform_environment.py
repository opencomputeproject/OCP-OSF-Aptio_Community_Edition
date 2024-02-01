"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import os
import logging
import json

AMI_PLATFORM_TOOLS_DIR = os.path.normpath(
    os.path.abspath("Platform/AmdPlat/Tools/dbuild_support")
)
EDKII_PLATFORM_TOOLS_DIR = os.path.normpath(
    os.path.abspath("Platform/PlatformTools/dbuild_support")
)
platform_dbuild_support_dir = ""


def is_ami_build():
    global platform_dbuild_support_dir
    if platform_dbuild_support_dir == AMI_PLATFORM_TOOLS_DIR:
        return True
    elif os.path.isdir(AMI_PLATFORM_TOOLS_DIR):
        platform_dbuild_support_dir = AMI_PLATFORM_TOOLS_DIR
        return True
    else:
        return False


def is_edkii_build():
    global platform_dbuild_support_dir
    if platform_dbuild_support_dir == EDKII_PLATFORM_TOOLS_DIR:
        return True
    elif os.path.isdir(EDKII_PLATFORM_TOOLS_DIR):
        platform_dbuild_support_dir = EDKII_PLATFORM_TOOLS_DIR
        return True
    else:
        return False


def get_platform_dbuild_support_dir():
    global platform_dbuild_support_dir
    if platform_dbuild_support_dir != "":
        return platform_dbuild_support_dir
    if is_ami_build():
        return platform_dbuild_support_dir
    elif is_edkii_build():
        return platform_dbuild_support_dir
    else:
        error_text = "Could not locate Platform dbuild support directory"
        logging.debug(error_text)
        raise ValueError(error_text)


def get_platform_json_file_contents(filename):
    path = get_platform_dbuild_support_dir()
    json_file = os.path.join(path, filename)

    try:
        with open(json_file, "r", encoding="utf-8") as file:
            return json.load(file)
    except:
        error_text = "Unable to parse {}".format(json_file)
        logging.error(error_text)
        raise ValueError(error_text)
