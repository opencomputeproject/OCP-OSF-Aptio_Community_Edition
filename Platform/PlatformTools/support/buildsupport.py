"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os

def _common_build_check(variable):
    """!
    Return Python boolean equivalent of variable

    Return Environment variable boolean state
    "TRUE" = True
    "FALSE" = False
    not set = False

    @param variable     environment variable name
    @returns            True or False
    """
    try:
        variable = os.environ[variable]
    except:
        return False
    else:
        if variable.upper() == 'TRUE':
            return True
        else:
            return False

def build_quick():
    """!
    Return Python boolean equivalent of BUILD_QUICK environment variable

    @returns            True or False
    """
    return _common_build_check('BUILD_QUICK')

def build_show_only():
    """!
    Return Python boolean equivalent of BUILD_SHOW_ONLY environment variable

    @returns            True or False
    """
    return _common_build_check('BUILD_SHOW_ONLY')

def build_module_only():
    """!
    Return Python boolean equivalent of BUILD_MODULE_ONLY environment variable

    @returns            True or False
    """
    return _common_build_check('BUILD_MODULE_ONLY')
