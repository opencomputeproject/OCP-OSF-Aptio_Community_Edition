"""
*******************************************************************************
 Copyright (C) 2022 Advanced Micro Devices, Inc. All rights reserved.

*******************************************************************************
"""
import os
from io import BytesIO
import tarfile
import argparse
import sys

def create_MANIFEST(FD_filename,version,ExtendedVersion,
        KeyType,HashType,MachineName):
    """!
    This function takes in the given arguments and returns a
    MANIFEST file object suitable for storing in the Tarball

    @param      FD_filename        name of FD file
    @param      version            inputed version of BIOS
    @param      ExtendedVersion    extended name of BIOS version
    @param      KeyType            Key Type
    @param      HashType           Hash Type
    @param      MachineName        Machine Name

    @returns    MANIFEST file object
    """
    MANIF = "purpose=xyz.openbmc_project.Software.Version.VersionPurpose.Host\n"

    if version == "":
        version, ext = os.path.splitext(FD_filename)
        MANIF += "version="+version+"\n"
    else:
        MANIF += "version="+version+"\n"
    if ExtendedVersion != "":
        MANIF += "ExtendedVersion="+ExtendedVersion+"\n"
    if KeyType != "":
         MANIF += "KeyType="+KeyType+"\n"
    if HashType != "":
        MANIF += "HashType="+HashType+"\n"
    if MachineName !="":
        MANIF += "MachineName="+MachineName+"\n"
    try:
        s = BytesIO()
        s.write(MANIF.encode('utf8'))
        s.seek(0)
    except:
        print("error: Creating MANIFEST file object")
        sys.exit(254)
    return s

def to_tar(FD_filename, user_MANIF='', version='', ExtendedVersion='', KeyType='',
    HashType='',MachineName=''):
    """!
    Turns the BIOS image and MANIFEST into a compressed tarball
    compression at default(9)

    @param      FD_filename        name of FD file
    @param      user_MANIF         user's inputed MANIFEST
    @param      version            inputed version of BIOS
    @param      ExtendedVersion    extended name of BIOS version
    @param      KeyType            Key Type
    @param      HashType           Hash Type
    @param      MachineName        Machine Name
    """
    head, tail = os.path.split(FD_filename)
    name, ext = os.path.splitext(tail)

    if user_MANIF != '':
        try:
            with tarfile.open(name+".tar.gz",'w:gz') as tar:
                tar.add(user_MANIF, arcname="MANIFEST")
                tar.add(FD_filename, arcname=tail)
        except Exception as e:
            print("error: Can't create Tarball with given files")
            sys.exit(255)
    else:
        MANIF = create_MANIFEST(tail,version,ExtendedVersion,KeyType,HashType,MachineName)
        tarinfo = tarfile.TarInfo(name="MANIFEST")
        tarinfo.size = len(MANIF.getvalue())
        try:
            with tarfile.open(name+".tar.gz",'w:gz') as tar:
                tar.addfile(tarinfo=tarinfo, fileobj=MANIF)
                tar.add(FD_filename, arcname=tail)
        except Exception as e:
            print("error: Can't write Tarball")
            sys.exit(253)
        MANIF.close()

def extract_tar(tar_file):
    """!
    Splits the given Tarball into a MANIFEST file and BIOS image

    @param    tar_file    name of .tar.gz file
    """
    try:
        tar = tarfile.open(tar_file)
        tar.extractall()
        tar.close()
    except Exception as e:
        print("error: Can't extract Tarball")
        sys.exit(252)

def file_extension(astring):
    """!
    Makes sure inputed file has a .FD or .tar.gz extention

    @param    astring    file argument
    """
    if astring.endswith(".FD") or astring.endswith(".tar.gz"):
        return astring
    else:
        raise ValueError

def parse_args():
    """!
    Uses argparse to take in arguments
    File (-f) is the only required argument, the rest is accounted for

    @returns    Parsed arguments
    """
    parser = argparse.ArgumentParser(
        description="Generate Tarball with Bios image and MANIFEST Script or split Tarball")
    parser.add_argument("file",default="", type=file_extension,
        help='Either a BIOS FD image or .tar.gz file')
    parser.add_argument("-a","--manifest",default="",
        help='OPTIONAL: Add externally created MANIFEST to Tarball')
    parser.add_argument("-v","--version",default="",
        help='OPTIONAL: BIOS version for generated MANIFEST')
    parser.add_argument("-ev","--ExtendedVersion", default="",
        help='OPTIONAL: Add extended version for generated MANIFEST')
    parser.add_argument("-kt","--KeyType",default="",
        help='OPTIONAL: Add key type for generated MANIFEST')
    parser.add_argument("-ht","--HashType",default="",
        help='OPTIONAL: Add hash type for generated MANIFEST')
    parser.add_argument("-mn","--MachineName",default="",
        help='OPTIONAL: Add machine name for generated MANIFEST')

    return parser.parse_args()


def main():
    """!
    Assumes this script is in the same folder as image

    If parsed file is .FD image, will create a manifest and create a tarball
    If parsed file is .tar.gz, will split tarball into image and manifest

    create_MANIFEST(version, fd file name, ExtendedVersion, KeyType, HashType, MachineName)
    """
    print("bios_tar.py\t Copyright 2021 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.")
    args = parse_args()

    if args.file.endswith(".FD"):
        to_tar(FD_filename=args.file,user_MANIF=args.manifest,version=args.version,
            ExtendedVersion=args.ExtendedVersion,KeyType=args.KeyType,
            HashType=args.HashType,MachineName=args.MachineName)
    elif args.file.endswith(".tar.gz"):
        extract_tar(args.file)

    print("complete")

if __name__ == "__main__":
    main()
