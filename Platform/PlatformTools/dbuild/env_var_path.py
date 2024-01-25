"""
*******************************************************************************
 Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
*******************************************************************************
"""

import os


def get_path_env_var(path_env_var):
    """!
    Get path type environment variable

    Python has problems with paths containing spaces which are stored in Windows
    environment variables.  The environment variable needs to have quotes around
    the string, but python can't handle the quotes so they must be stripped off.
    This is the only way found to work.

    @param      path_env_var        String containing the name of the
                                    environment variable containing a path
                                    potentially with spaces.

    @returns    path                The path string from the environment
                                    variable properly formatted for use in
                                    python
    """
    # get the environment variable or an empty string and strip off quotes
    var = os.environ.get(path_env_var.upper(), None)
    if var != None:
        var.strip('"').strip("'")
        return os.path.normpath(var)  # Normalize the path
    else:
        return None


def get_path_string_for_env_var(path_string):
    """!
    Return a path string to be stored in an environment variable at some point

    Python has problems with paths containing spaces which are stored in Windows
    environment variables.  The environment variable needs to have quotes around
    the string, but python can't handle the quotes so they must be stripped off.
    This is the only way found to work.

    @param      path_string         String containing the path to format for
                                    storage in an environment variable

    @returns    path                The path string formated for environment
                                    variable
    """
    # if it already has quotes, leave them
    string = path_string
    path = os.path.normpath(string)
    if " " in path:
        if not (
            (path.startswith('"') and path.endswith('"'))
            or (path.startswith("'") and path.endswith("'"))
        ):
            path = '"{}"'.format(path)

    return path


def set_path_env_var(path_env_var, path_string):
    """!
    Set path type environment variable

    Python has problems with paths containing spaces which are stored in Windows
    environment variables.  The environment variable needs to have quotes around
    the string, but python can't handle the quotes so they must be stripped off.
    This is the only way found to work.

    @param      path_env_var        String containing the name of the
                                    environment variable containing a path
                                    potentially with spaces.
    @param      path_string         String containing the path to store in the
                                    environment variable
    """
    string = get_path_string_for_env_var(path_string)
    os.environ[path_env_var] = os.path.normpath(string)
