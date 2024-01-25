"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""
import arguments
from clean_paths import clean_paths

def add_argument_commands():
    clean_parser = arguments.subparsers.add_parser(
        "clean", add_help=False, help="Clean build files"
    )
    clean_parser.set_defaults(func=clean)

def clean():
    clean_paths("clean_paths_edk2.json")
