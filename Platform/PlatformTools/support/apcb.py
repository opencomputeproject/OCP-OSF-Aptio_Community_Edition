"""
*******************************************************************************
 Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

from buildsupport import build_module_only, build_show_only, build_quick
from external import external_only

import sys
import os
import subprocess
import shutil

def build_apcb():
    """!
    build the APCB

    exception   various
    """
    if build_quick() or build_show_only() or build_module_only():
        print('\nSkipping APCB build')
        return

    print('\nExecuting APCB build')

    # Get environment variables exception if not located
    workspace = os.environ['WORKSPACE']
    soc_sku = os.environ['SOC_SKU']
    build_type = os.environ['AMD_PLATFORM_BUILD_TYPE']
    custom_apcb_path = os.environ['CUSTOM_APCB_PATH']

    if not os.path.exists(custom_apcb_path):
        os.makedirs(custom_apcb_path)

    apcb_tool_temp_path = os.environ['APCB_TOOL_TEMP_PATH']
    apcb_script = 'ApcbCreate.py'

    # If external_only, AcpbAutoGen.h file should already be in place
    force_cbs = os.environ.get("CBS_FORCE_BUILD", 'FALSE')
    if not external_only() or force_cbs == "TRUE":
        # Copy proper header file
        src = os.path.join(
            workspace,
            'AmdCbsPkg/Build/Resource{}'.format(soc_sku),
            'ApcbAutoGen{}.h'.format(soc_sku)
        )
        dst = os.path.join(
            apcb_tool_temp_path,
            'Include/ApcbAutoGen.h'
        )
        shutil.copy(src, dst)
    else:
        print("External PI package, No need to copy ApcbAutoGen.h")

    cmd_env = os.environ.copy()
    if build_type == "INTERNAL":
        cmd_env["APCB_INTERNAL_BUILD"] = "1"
    else:
        cmd_env["APCB_EXTERNAL_BUILD"] = "1"

    # Check OS to avoid Linux from defaulting to python 2.xx
    if sys.platform.startswith("linux"):
      cmd = ["python3"]
    elif sys.platform.startswith("win"):
      cmd = ["python"]

    # path to APCB script
    cmd.append(os.path.join(apcb_tool_temp_path, apcb_script))
    cmd.append("CLEAN")
    cmd.append("BUILD")

    # Execute the APCB build
    sys.stdout.flush()
    completed_process = subprocess.run(cmd, env=cmd_env, cwd=apcb_tool_temp_path)
    if completed_process.returncode != 0:
        error_text = 'Return code = {}'.format(completed_process.returncode)
        print(error_text)
        raise ValueError(error_text)

    # Copy built files
    src = os.path.join(apcb_tool_temp_path,'Release')
    dst = custom_apcb_path
    files = (
        'APCB_{}_D4_DefaultRecovery.bin'.format(soc_sku),
        'APCB_{}_D4_Updatable.bin'.format(soc_sku),
        'APCB_{}_D4_EventLog.bin'.format(soc_sku)
    )
    for file in files:
        src_file = os.path.join(src, file)
        dst_file = os.path.join(dst, file)
        print('Copying "{}"->"{}"'.format(src_file, dst_file))
        shutil.copy(src_file, dst_file)

    # Delete build time folders
    for dir in ('Release', 'Build', 'Log'):
        shutil.rmtree(
            os.path.join(apcb_tool_temp_path, dir),
            ignore_errors=True
        )
