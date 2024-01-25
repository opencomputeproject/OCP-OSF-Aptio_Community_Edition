"""
*******************************************************************************
 Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os

def projectpostbuild():
    # These will be set elsewhere with dbuild.py and can be removed when it
    # replaces dbuild.cmd
    os.environ["SOC_FAMILY"] = os.environ.get("SOC_FAMILY", "0x19")
    os.environ["SOC_SKU"] = os.environ.get("SOC_SKU", "Genoa")
    os.environ["SOC2"] = os.environ.get("SOC2", "Genoa")
    os.environ["SOCKET"] = os.environ.get("SOCKET", "SP5")

    workspace = os.environ['WORKSPACE']
    build_output = os.environ['BUILD_OUTPUT']

    os.environ['APCB_TOOL_TEMP_PATH'] = os.path.normpath(os.path.join(
        workspace,
        'Platform/Apcb/GenoaSp5Rdimm'
    ))
    os.environ['APCB_MULTI_BOARD_SUPPORT'] = '1'
    os.environ['APCB_DATA_BOARD_DIR_LIST'] = 'GenoaCommon Onyx'
    os.environ['CUSTOM_APCB_PATH'] = os.path.normpath(os.path.join(
        build_output,
        'Apcb'
    ))
