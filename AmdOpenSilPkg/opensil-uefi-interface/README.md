# opensil-uefi-interface environment for AMD-OpenSIL™


## Overview

This repository contains the files and the references needed for adding AMD OpenSIL™ to a UEFI project.


## Parts of opensil-uefi-interface
- OpenSIL submodule reference contains location of OpenSIL source files
- SilToUefi folder contains UEFI modules added to PEI execution; they contain the calls to OpenSIL for various silicon IP initialization
- AmdOpenSilPkg.dec is UEFI package declaration file. The name of the file implies the name of the directory for OpenSIL package, that is *AmdOpenSilPkg*. Note that  the match between the DEC file name and package directory is optional so that the content of this repo can be placed in any directory within UEFI project
- INF files have the list of source files for xSIM, xUSL, and xPRF libraries. These INF files should be included in the target DSC file of the UEFI project


## Adding OpenSIL UEFI package to the project

- **Step1:** Create a folder for UEFI OpenSIL package in UEFI project, for example $(PROJECT_DIR)/AmdOpenSilPkg

- **Step2:** From the package folder clone the repository:

  `git clone git@github.com:AMD-OpenSIL/opensil-uefi-interface.git --recursive`

- **Step3:** Add the reference to the new INF files to the project DSC file

Alternatively the reference to the opensil-uefi-interface repository can be presented as a submodule in a parent UEFI project repository. In this case cloning this parent project with `--recursive` option would perform the steps 1 through 3 automatically.

## Memory Allocation for openSIL

The opensil-uefi-interface will allocate a single block of contiguous memory through the creation of a GUIDed HOB (hand-off-block).  The HOB contains the contents of the openSIL ip block data with the data being populated as part of the SIL initialization prior to openSIL time point 1 execution.  It is the responsibility of the host to relocate this data, if necessary, and provide the base address to each openSIL time point.

opensil-uefi-interface has defined the following GUID which represents the name of this openSIL HOB:

gPeiOpenSilDataHobGuid = { 0xbc6ef377, 0x4190, 0x47d1, { 0x84, 0xe3, 0xe3, 0xc7, 0xa0, 0x8f, 0x35, 0xb8 }}
