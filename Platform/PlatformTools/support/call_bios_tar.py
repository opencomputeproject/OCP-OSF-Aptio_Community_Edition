"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""
import os
import sys
from buildsupport import build_module_only

# Make sure python environment knows where to find bios_tar.py
search_path = os.path.join(
                os.environ['WORKSPACE'],
                'Platform',
                'PlatformTools',
                'bios_tar',
                ''
                )
sys.path.insert(0, search_path)

from bios_tar import to_tar

def tar_bios_image():
    """!
    Build the BIOS tar image from FD image for OpenBMC flashing

    exception   various
    """

    # Get environment variables exception if not located
    firmware_version = os.environ['FIRMWARE_VERSION_STR']
    print('\nCreating OpenBMC tarball {}'.format(firmware_version))
    if build_module_only():
        print('Skip creating tarball')
        return

    input_image = '{}.FD'.format(firmware_version)
    machinename = os.environ["SOCKET"]
    to_tar(input_image, MachineName=machinename.lower())
