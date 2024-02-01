"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""

import os
import shutil
import argparse

def _overrides(override=False):
    """!
    Override or restore Platfrom Override files

    @param overrides        True = override files
                            False = restore files
    @exception              varies by failure
    """
    if override:
        mode = 'overrides'
    else:
        mode = 'restores'
    print('\nProcessing {}'.format(mode))

    # Get environment variables exception if not located
    search_dir = os.path.join(os.environ['AMD_PLATFORM_DIR'], 'Override', '')
    workspace = os.environ['WORKSPACE']
    for root, dirs, files in os.walk(search_dir):
        for file in files:
            src = os.path.join(root, file)
            dst = os.path.join(workspace, src[len(search_dir):])
            back = '{}.back'.format(dst)
            print('src:"{}"\n\tdst:"{}"\n\tback:"{}"'.format(src, dst, back))
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
                    print('\tNo Override: Override file does not exist:"{}"'.format(dst))
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

def main():
    """!
    Standalone entry point to override and restore files
    """
    # Set up argument parser. Help text will show default value
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        prefix_chars='-/',
        description="""
            Build a platform Firmware image
            """
    )

    subparsers = parser.add_subparsers(
        title='Command',
        dest='command',
        required=True
    )

    override_parser = subparsers.add_parser(
        'override',
        help='Override Files'
    )
    override_parser.set_defaults(func=overrides)
    restore_parser = subparsers.add_parser(
        'restore',
        help='Restore Files'
    )
    restore_parser.set_defaults(func=restore_overrides)

    args = parser.parse_args()
    args.func()

if __name__ == '__main__':
    main()