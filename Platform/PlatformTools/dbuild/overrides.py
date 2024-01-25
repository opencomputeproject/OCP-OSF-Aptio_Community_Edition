"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import os
import shutil
import arguments


def _overrides(override=False):
    """!
    Override or restore Platfrom Override files

    @param overrides        True = override files
                            False = restore files
    @exception              varies by failure
    """
    if override:
        mode = "overrides"
    else:
        mode = "restores"
    print("\nProcessing {}".format(mode))
    if arguments.args.showonly:
        print("SHOW ONLY: overrides not executed")

    search_dir = os.path.join(
        arguments.args.workspace,
        "Platform",
        arguments.args.selected_build_attributes["dir"],
        arguments.args.selected_build_attributes["platform"],
        "Override",
    )
    # Get environment variables exception if not located
    for root, dirs, files in os.walk(search_dir):
        for file in files:
            src = os.path.join(root, file)
            dst = os.path.join(arguments.args.workspace, src[len(search_dir) + 1 :])
            back = "{}.back".format(dst)
            print('src:"{}"\n\tdst:"{}"\n\tback:"{}"'.format(src, dst, back))
            if arguments.args.showonly:
                continue
            if override:
                if os.path.exists(dst):
                    # Do not override back or dst if back already exists
                    # Leftover from failed build. A clean build will clean up
                    if not os.path.exists(back):
                        shutil.copy(dst, back)
                        # src must exist, no need to check
                        shutil.copy(src, dst)
                    else:
                        print('\tNo Override: Backup already exists: "{}"'.format(back))
                else:
                    print(
                        '\tNo Override: Override file does not exist:"{}"'.format(dst)
                    )
            else:
                if os.path.exists(back):
                    shutil.move(back, dst)
                else:
                    print('\tNo Restore: Backup file does not exist:"{}"'.format(back))


def overrides():
    """!
    Override Platfrom Override files
    """
    _overrides(override=True)


def restore_overrides():
    """!
    Restore Platfrom Override files
    """
    _overrides(override=False)
