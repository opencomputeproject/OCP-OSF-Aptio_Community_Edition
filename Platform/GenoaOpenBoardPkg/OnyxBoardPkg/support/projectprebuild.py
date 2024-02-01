"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os

def projectprebuild():
    # These will be set elsewhere with dbuild.py and can be removed when it
    # replaces dbuild.cmd
    os.environ["SOC_FAMILY"] = os.environ.get("SOC_FAMILY", "0x19")
    os.environ["SOC_SKU"] = os.environ.get("SOC_SKU", "Genoa")
    os.environ["SOC2"] = os.environ.get("SOC2", "Genoa")
    os.environ["SOCKET"] = os.environ.get("SOCKET", "SP5")
