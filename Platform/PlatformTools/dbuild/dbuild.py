"""
*******************************************************************************
 Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""
import sys

# Verify Minimum Python version when loading the script.
MIN_PYTHON = (3, 7, 4)
if sys.version_info < MIN_PYTHON:
    print(
        "Exiting dbuild.py: Minimum of Python {}.{}.{} required.".format(*MIN_PYTHON)
    )
    exit(1)

import logging

import arguments


def main():
    """!
    Execute requested command
    """
    arguments.parse_args()

    logging.basicConfig(
        format="%(levelname)s: %(message)s", level=arguments.args.verbosity
    )

    logging.debug(arguments.args)

    try:
        return_code = arguments.args.func()  # Call requested function
    except:
        logging.error(
            'Command: "{}" failed exiting with error'.format(arguments.args.command)
        )
        raise

    return return_code


if __name__ == "__main__":
    exit(main())
