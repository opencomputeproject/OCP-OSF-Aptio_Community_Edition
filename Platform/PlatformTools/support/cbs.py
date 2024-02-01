"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

from buildsupport import build_module_only, build_show_only, build_quick

import sys
import os
import errno
import subprocess
import shutil
import filecmp
from glob import glob


def build_cbs_ids():
    """!
    build the CBS IDS header files

    exception   various
    """
    if build_quick() or build_show_only() or build_module_only():
        print("\nSkipping CBS IDS build")
        return
    print("\nExecuting CBS IDS Build")

    # Get environment variables
    cbs_env = os.environ.copy()

    tools_folder = os.path.join(cbs_env["WORKSPACE"], "AmdCbsPkg", "Tools")

    cbs_cmd = [os.path.join(cbs_env["PERL_PATH"], "perl")]
    perl_parent, dir = os.path.split(cbs_env["PERL_PATH"].rstrip("/"))
    cbs_cmd.extend(
        ["-I{}".format(tools_folder),
         "-I{}".format(os.path.join(perl_parent, "lib"))]
    )
    cbs_cmd.extend(
        [os.path.join(cbs_env["WORKSPACE"], "AmdCbsPkg",
                      "Tools", "IdsIdGen.pl")]
    )
    output_folder = os.path.join(cbs_env["BUILD_OUTPUT"], "AmdCbsPkg")
    cbs_cmd.extend(
        [
            "-i",
            os.path.join(cbs_env["WORKSPACE"], "AmdCbsPkg", "Library", "Family", os.environ["SOC_FAMILY"],
                         os.environ["SOC_SKU"], os.environ["AMD_PLATFORM_BUILD_TYPE"].title()),
            "-o",
            output_folder,
        ]
    )

    print("Executing:")
    print(" ".join(cbs_cmd))
    sys.stdout.flush()
    try:
        completed_process = subprocess.run(cbs_cmd, env=cbs_env)
    except:
        error_text = "Failure executing CBS IDS header generation"
        print(error_text)
        raise ValueError(error_text)
    if completed_process.returncode != 0:
        error_text = "Return code = {}".format(completed_process.returncode)
        print(error_text)
        raise ValueError(error_text)

    agesa_include = os.path.join(
        cbs_env["WORKSPACE"], "Platform", "Include"
    )
    for src in glob(os.path.join(output_folder, "*.h")):
        dst = os.path.join(agesa_include, os.path.basename(src))
        print("Checking: src: {}\n\tdst: {}".format(src, dst))
        if not os.path.exists(dst) or not filecmp.cmp(src, dst):
            print("Copying: src: {}\n\tdst: {}".format(src, dst))
            shutil.copyfile(src, dst)


def build_cbs_from_xml():
    """!
    build the CBS UEFI EDK2 code from XML

    exception   various
    """
    if build_quick() or build_show_only() or build_module_only():
        print("\nSkipping CBS build from XML")
        return
    print("\nExecuting CBS Build from XML")

    # Get environment variables
    cbs_env = os.environ.copy()

    tools_folder = os.path.join(cbs_env["WORKSPACE"], "AmdCbsPkg", "Tools")

    cbs_cmd = [os.path.join(cbs_env["PERL_PATH"], "perl")]
    perl_parent, dir = os.path.split(cbs_env["PERL_PATH"].rstrip("/"))
    cbs_cmd.extend(
        ["-I{}".format(tools_folder),
         "-I{}".format(os.path.join(perl_parent, "lib"))]
    )
    cbs_cmd.extend(
        [os.path.join(cbs_env["WORKSPACE"], "AmdCbsPkg",
                      "Tools", "CBSgenerate.pl")]
    )
    cbs_lib_family_path = os.path.join(
        "Library",
        "Family",
        cbs_env["SOC_FAMILY"],
        cbs_env["SOC_SKU"],
        cbs_env["AMD_PLATFORM_BUILD_TYPE"].capitalize(),
    )
    # Input XML file to parse
    soc2 = cbs_env["SOC2"].capitalize()
    xml_type = "i" if cbs_env["AMD_PLATFORM_BUILD_TYPE"] == "INTERNAL" else "e"
    cbs_cmd.extend(
        [
            "-i",
            os.path.join(
                cbs_env["WORKSPACE"],
                "AmdCbsPkg",
                cbs_lib_family_path,
                soc2,
                "{}Setup{}.xml".format(xml_type, soc2),
            ),
        ]
    )

    # Input GBS file to parse
    cbs_cmd.extend(
        [
            "-s",
            os.path.join(
                cbs_env["WORKSPACE"],
                "AmdCbsPkg",
                cbs_lib_family_path,
                soc2,
                "{}{}Setting.xml".format(xml_type, soc2),
            ),
        ]
    )

    # Output location
    build_output = os.path.join(
        cbs_env["WORKSPACE"],
        "AmdCbsPkg",
        "Build",
        "Resource{}".format(cbs_env["SOC_SKU"]),
    )
    try:
        os.makedirs(build_output)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    cbs_cmd.extend(["-o", build_output])

    # Internal vs External
    cbs_cmd.extend(["--version", cbs_env["AMD_PLATFORM_BUILD_TYPE"].lower()])

    # Not sure what this does
    cbs_cmd.extend(["-b", "enable"])

    # -t PROMONTORY_SUPPORT=0|1 -x BIXBY_SUPPORT=0|1 -r PROMONTORY_PLUS_SUPPORT=0|1
    cbs_cmd.extend(["-t", "0", "-x", "0", "-r", "0"])

    # Vendor String
    cbs_cmd.extend(["-y", cbs_env.get("CBS_VENDOR_STRING", "NULL")])

    print("Executing:")
    print(" ".join(cbs_cmd))
    sys.stdout.flush()
    try:
        completed_process = subprocess.run(cbs_cmd, env=cbs_env)
    except:
        error_text = "Failure executing CBS code generation"
        print(error_text)
        raise ValueError(error_text)
    if completed_process.returncode != 0:
        error_text = "Return code = {}".format(completed_process.returncode)
        print(error_text)
        raise ValueError(error_text)

    agesa_include = os.path.join(
        cbs_env["WORKSPACE"], "Platform", "Include"
    )
    copy_file_list = [
        "IdsNvDef{}.h".format(cbs_env["SOC_SKU"]),
        "IdsNvIntDef{}.h".format(cbs_env["SOC_SKU"]),
    ]
    for src_file in copy_file_list:
        src = os.path.join(build_output, src_file)
        dst = os.path.join(agesa_include, os.path.basename(src))
        if os.path.exists(src):
            print("Checking: src: {}\n\tdst: {}".format(src, dst))
            if not os.path.exists(dst) or not filecmp.cmp(src, dst):
                print("Copying: src: {}\n\tdst: {}".format(src, dst))
                shutil.copyfile(src, dst)
