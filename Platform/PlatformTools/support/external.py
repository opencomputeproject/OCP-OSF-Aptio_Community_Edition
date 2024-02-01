"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os


def external_only():
    """!
    Determine if the PI component supports external only

    @returns        True    - Only External build supported
                    False   - Internal and External build supported
    """
    # Check for a file that will not be available on an external build
    checkfile = "Platform/Library/PspIdsHookLib/Dxe/Internal"
    return not os.path.isdir(checkfile)
