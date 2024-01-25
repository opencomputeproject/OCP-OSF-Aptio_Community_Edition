import os
import re


def build_sanity_check():
    """!
    Build sanity check with PI exclude list

    exception   various
    """
    excludefile = (
        "AmdCommonTools/Server/PiExcludeList/{}_ExcludeList.txt".format(os.environ["SOC"]))
    buildfile = os.path.join(os.environ["BUILD_OUTPUT"], "build.log")
    excludefile = os.path.join(os.environ["WORKSPACE"], excludefile)
    if os.environ['AMD_PLATFORM_BUILD_TYPE'] == "INTERNAL":
        return
    if os.path.exists(buildfile) and os.path.exists(excludefile):
        print("Build PI sanity check ...")
        with open(buildfile, "r") as build_file:
            data = build_file.read().replace('\\', '/')
            with open(excludefile, "r") as exclude_list:
                for line in exclude_list:
                    searchline = line.lstrip().strip().replace('\\', '/')
                    if "*" in searchline:
                        # contains '*' hence need to regular expression
                        if searchline.startswith('*'):
                            # first character * means none or any character, trim it
                            searchline = searchline[1:]
                        if searchline.startswith('Internal'):
                            # Special case for Internal, look only in PI delivered packages
                            searchline = "(?:Platform)/*.*/" + searchline
                        foundre = set(re.findall(searchline, data))
                        if foundre:
                            print("!!! WARNING !!!: The build has a file or directory which is excluded.: {}".format(
                                foundre))
                    else:
                        if "." not in searchline:
                            if not searchline.endswith('//'):
                                searchline = "".join((searchline, '/'))
                        if searchline in data:
                            print("!!! WARNING !!!: The build has a file or directory which is excluded.: {}".format(
                                searchline))
        print("Done.")
