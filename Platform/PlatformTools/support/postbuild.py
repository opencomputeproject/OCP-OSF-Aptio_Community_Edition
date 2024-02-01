"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os
import sys

# Add selected platform support directory to beginning of python script search path
search_path = os.path.join(os.environ['AMD_PLATFORM_DIR'], 'support', '')
sys.path.insert(0, search_path)

# Import from platfrom selected above
from projectpostbuild import projectpostbuild
from apcb import build_apcb
from pspdirectory import insert_psp_directory
from call_bios_tar import tar_bios_image
from build_sanity_check import build_sanity_check
def postbuild():
    print('PostBuild')
    print('Launched Python Version: {}.{}.{}'.format(
        sys.version_info.major,
        sys.version_info.minor,
        sys.version_info.micro))
    # Execute first in postbuild
    projectpostbuild()
    # Execute APCB Build
    #build_apcb()
    # Execute PSP insert after APCB
    insert_psp_directory()
    # tar the BIOS FD image
    tar_bios_image()
    # sanity check for PI build
    build_sanity_check()

def main():
    """!
    Execute PostBuild items

    Execute anything that needs to be completed after the EDKII BUILD
    """
    postbuild()

if __name__ == '__main__':
    main()
