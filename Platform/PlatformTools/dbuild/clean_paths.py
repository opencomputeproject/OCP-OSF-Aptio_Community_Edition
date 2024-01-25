"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import shutil
import os
import logging
from pathlib import Path
from platform_environment import get_platform_json_file_contents
from external import external_only


def clean_directories(dir_list):
    """!
    Clean a list of directories

    @param      dir_list        A list of directories to delete
    """
    for directory in dir_list:
        if type(directory) != str:
            error_text = '"{}" must be a string'.format(directory)
            logging.error(error_text)
            raise ValueError(error_text)

        print("Removing: {}".format(directory))
        shutil.rmtree(directory, ignore_errors=True)


def clean_files(glob_lists):
    """!
    Clean a list of file type globs from given directories

    @param      glob_list        A list of directories and file globs to delete
    """
    for glob_entry in glob_lists:
        if type(glob_entry) != list or len(glob_entry) != 2:
            error_text = '"{}" must be a two element list'.format(glob_entry)
            logging.error(error_text)
            raise ValueError(error_text)
        glob_dir, glob = glob_entry
        if type(glob_dir) != str or type(glob) != str:
            error_text = '"{}" and "{}" must be strings'.format(glob_dir, glob)
            logging.error(error_text)
            raise ValueError(error_text)

        print('Removing "{}" files from "{}"'.format(glob, glob_dir))
        for path in Path(glob_dir).glob(glob):
            os.remove(path)


def clean_paths(clean_paths_json_file):
    """!
    Clean based on json dictionary file

    Parse clean_paths_json_file and either delete "directory" or remove files
    indicated by "glob"
    Example json content (list of single entry dictionaries)

    external = Only process if PI released components. (likely will not be used)
    internal = Only process if AMD internal capable source.
    always = always perform action.

      {
        "directory": {
          "always": [
            "dir1",
            "dir2"
          ],
          "internal": [],
          "external": []
        },
        "glob": {
          "always": [
            ["dir3", "*.obj"],
            ["dir4", "*.FD"]
          ],
          "internal": [],
          "external": []
        }
      }

    @param      clean_paths_json_file       File name of properly formated json
                                            for cleaning up directories
    """
    print("\nCleaning Paths")

    logging.debug('Processing "{}"'.format(clean_paths_json_file))
    try:
        clean_types = get_platform_json_file_contents(clean_paths_json_file)
    except:
        raise
    supported_scopes = ["always", "internal", "external"]
    supported_clean_types = ["directory", "glob"]

    if type(clean_types) != dict:
        error_text = '"{}" must be a dictionary'.format(clean_paths_json_file)
        logging.error(error_text)
        raise ValueError(error_text)

    for clean_type, clean_scopes in clean_types.items():
        if clean_type not in supported_clean_types:
            error_text = '"{}" must be one of {}'.format(
                clean_type, supported_clean_types
            )
            logging.error(error_text)
            raise ValueError(error_text)
        if type(clean_scopes) != dict:
            error_text = '"{}" must contain a dictionary'.format(clean_type)
            logging.error(error_text)
            raise ValueError(error_text)

        for scope, scope_list in clean_scopes.items():
            if scope not in supported_scopes:
                error_text = '"{}:{}" must be one of {}'.format(
                    clean_type, scope, supported_scopes
                )
                logging.error(error_text)
                raise ValueError(error_text)
            if type(scope_list) != list:
                error_text = '"{}:{}" must contain a list'.format(clean_type, scope)
                logging.error(error_text)
                raise ValueError(error_text)
            if scope == "internal" and external_only():
                continue
            if scope == "external" and not external_only():
                continue
            if clean_type == "directory":
                clean_directories(scope_list)
            elif clean_type == "glob":
                clean_files(scope_list)
