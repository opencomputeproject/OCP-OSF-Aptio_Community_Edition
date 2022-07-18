# Aptio OpenEdition Firmware  

The Minimum Platform is a software architecture that guides uniform delivery of Intel platforms enabling firmware solutions for basic boot functionality with extensibility built-in.  This project incorporates support for the OCP derived Tioga Pass platform and Junction City Platform.

Package maintainers are listed in Maintainers.txt.

## Overview
The key elements of the architecture are organized into a staged boot approach where each stage has requirements and
functionality for specific use cases. The generic control flow through the boot process is implemented in the
[`MinPlatformPkg`](https://github.com/tianocore/edk2-platforms/tree/devel-MinPlatform/Platform/Intel/MinPlatformPkg).
The generic nature of the tasks performed in MinPlatformPkg lends to reuse across all Intel platforms with no
source modification. Details for any particular board are made accessible to the MinPlatformPkg through a well-defined
statically linked board API. A complete platform solution then consists of the MinPlatformPkg and a compatible board
package.

## Board Naming Convention
The board packages supported by Intel follow the naming convention \<xxx\>OpenBoardPkg where xxx refers to the
encompassing platform name for a particular platform generation. For example, the [`KabylakeOpenBoardPkg`](https://github.com/tianocore/edk2-platforms/tree/devel-MinPlatform/Platform/Intel/KabylakeOpenBoardPkg) contains the
board code for Intel Kaby Lake reference systems. Intel uses the moniker "OpenBoardPkg" to indicate that this package
is the open source board code. A closed source counterpart may exist which simply uses "BoardPkg". Both directly use
the MinPlatformPkg from edk2-platforms.

## Stage Selection
Stage selection is controlled via the PCD `gMinPlatformPkgTokenSpaceGuid.PcdBootStage` in [`MinPlatformPkg.dec`](https://github.com/tianocore/edk2-platforms/blob/devel-MinPlatform/Platform/Intel/MinPlatformPkg/MinPlatformPkg.dec).
The stage should be configured in the board package DSC file to the appropriate value. For example, a board may disable
all advanced features by setting this value to 4 instead of 6. This may be used to improve boot time for a particular
use case. Decrementing the stage can also be used for debug since only the actions required for that stage objective
should be executed. As an example, ACPI initialization is not required for a Stage 3 boot.

The stages are defined as follows:

| Stage  | Functional Objective         | Example Capabilities                                                                               |
| -------|------------------------------|----------------------------------------------------------------------------------------------------|
| I      | Minimal Debug                | Serial port output, source debug enabled, hardware debugger enabled                                |
| II     | Memory Functional            | Basic hardware initialization necessary to reach memory initialization, permanent memory available |
| III    | Boot to UI                   | Simple console input and output to a UI, UEFI shell                                                |
| IV     | Boot to OS                   | Boot an operating system with the minimally required features                                      |
| V      | Security Enable              | UEFI Secure Boot, TCG measured boot, DMA protections                                               |
| VI     | Advanced Feature Enable      | Firmware update, power management, non-essential I/O                                               |

## Minimum Platform Firmware Solution Stack
A UEFI firmware implementation using MinPlatformPkg is constructed using the following pieces.

|                                    |
|------------------------------------|
| [EDK II](https://github.com/tianocore/edk2)                                                                              |
| [Intel(r) FSP](https://github.com/IntelFsp/FSP)                                                                            |
| [Minimum Platform (`MinPlatformPkg`)](https://github.com/tianocore/edk2-platforms/tree/devel-MinPlatform/Platform/Intel/MinPlatformPkg)                        |
| [Board Support (\<xxx\>OpenBoardPkg)](https://github.com/tianocore/edk2-platforms/tree/devel-MinPlatform/Platform/Intel)  |


## Board Support
* The `PurleyOpenBoardPkg` contains board implementations for Purley systems.
* The `WhitleyOpenBoardPkg` contains board implementations for Whitley systems.

## Board Package Organization
The board package follows the standard EDK II package structure with the following additional elements and guidelines:
* Only code usable across more than one board at the root level.* Board-specific code in a directory. The directory name should match that of the board supported.
* Features not essential to achieve stage 5 or earlier boots are maintained in a Features folder at the appropriate
  level in the package hierarchy.

Shared resources in the package root directory can include interfaces described in header files, library instances,
firmware modules, binaries, etc. The UEFI firmware implementation is built using the process described below from the
board-specific directory.

A board package must implement the board APIs defined in the MinPlatformPkg even if a "NULL" implementation is used to
return back to the minimum platform caller.

## **Windows Build Instructions**

### Pre-requisites

* [GIT client](https://git-scm.com/downloads): Available from https://git-scm.com/downloads
* [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/vs/older-downloads/#visual-studio-2019-and-other-products) <br>
   Login with user credentials and refer the below to download the VS2019
   
   ![image](https://user-images.githubusercontent.com/80769446/170539401-dd2ba633-1e2f-4103-a137-49132a484c5f.png)


  - Visual Studio 2015 build tools from Visual Studio 2019.  See image below for recommended install configuration.
    - **Please note that the VS2015 Build Tools are not enabled by default in the Visual Studio 2019 install.**
  - Visual Studio 2015 can be used instead.
  ![Visual Studio 2019 Installation](/Readme_VisualStudioInstall.png)
* [ASL compiler](https://www.acpica.org/downloads/binary-tools): iasl.exe available from http://www.acpica.org
  - Install into ```C:\ASL``` to match default tools_def.txt configuration.
  - The validated version of iasl compiler that can build MinPurley is 20180629.
* [NASM assembler](https://www.nasm.us/):  nasm.exe available from: http://www.nasm.us/
  - NASM 2.15.05 is the recommended minimum version.
  - Install into ```C:\NASM``` to match default tools_def.txt configuration.
* [Python 3.8.10](https://www.python.org/downloads/release/python-3810/):  Available from: https://www.python.org/downloads/release/python-3810/
  - Install into ```C:\Python38``` to match default tools_def.txt configuration.
  - Add C:\Python38 to your path
  - Other versions of 3.8 may also work fine.

### **Supported Hardware**

| Machine Name                          | Supported Chipsets                         | BoardPkg                     | Board Name         |
----------------------------------------|--------------------------------------------|------------------------------|--------------------|
| [Junction City](#junction-city--build-information)                         | IceLake-SP (Xeon Scalable)                 | WhitleyOpenBoardPkg          | JunctionCity       |
| [Aowanda](#Aowanda-build-information) | IceLake-SP (Xeon Scalable)         | WhitleyOpenBoardPkg           | Aowanda     |
| [MtJade](#Aowanda-build-information) | Ampere Altra         | Jade         | MtJade     |

  
### Download the required components

 To download the project, clone the repository along with all the submodules and checkout required TAG using the following command:
 git clone --recurse-submodules https://github.com/opencomputeproject/Aptio-OE.git -b (need to be replaced with TAG name)

### Junction City Build Information

**Building with the python script**

1. Open command window, go to the workspace directory, e.g. c:\Edk2Workspace 
2. Type "cd edk2-platforms/Platform/Intel
3. Type "python build_bios.py -p JunctionCity"
4. On successful build, IFWI (Integrated Firmware Image) JUNCTIONCITY.bin and BIOS JUNCTIONCITY.fd rom files are created.

* build_bios.py arguments:

  | Argument              | Function                            |
  | ----------------------|-------------------------------------|
  | -h, --help            | show this help message and exit     |
  | --platform, -p        | the platform to build               |
  | --DEBUG, -d           | debug flag                          |
  | --RELEASE, -r         | release flag                        |
  | --cleanall            | cleans all                          |


### Aowanda Build Information

**Building with the python script**

1. Open command window, go to the workspace directory, e.g. c:\Edk2Workspace 
2. Type "cd edk2-platforms/Platform/Intel
3. Type "python build_bios.py -p Aowanda"
4. On successful build, IFWI (Integrated Firmware Image) AOWANDA.bin and BIOS AOWANDA.fd rom files are created.

* build_bios.py arguments:

  | Argument              | Function                            |
  | ----------------------|-------------------------------------|
  | -h, --help            | show this help message and exit     |
  | --platform, -p        | the platform to build               |
  | --DEBUG, -d           | debug flag                          |
  | --RELEASE, -r         | release flag                        |
  | --cleanall            | cleans all                          |


### MtJade Build Information

Refer to https://github.com/opencomputeproject/OSF-Aptio-OpenEdition/tree/OE-AMI-MtJade-202206 branch

### **Binary and Reference Code Details**

* [EDK2](https://github.com/tianocore/edk2) source based on edk2-stable202205
* [EDK2-Platforms](https://github.com/tianocore/edk2-platforms) source based on commit hash f653a22385f502a021dced5fe174ab03b7c2be29
* [EDK2-Non-OSI](https://github.com/tianocore/edk2-non-osi) source based on commit hash 61662e8596dd9a64e3372f9a3ba6622d2628607c
* [FSP](https://github.com/IntelFsp/FSP) source based on commit hash  6d184188ca4915197df84549b412c48dd381165a

### **Validation Details**

* All firmware projects can only build on Windows with the validated configuration below.

**WhitleyOpenBoardPkg**

**This firmware project has only been tested on the Junction City hardware**.
* This firmware project build has only been tested using the Microsoft Visual Studio 2015 build tools.
* Booted to UEFI shell.
* Booted to UEFI Windows Server 2019 on M.2 NVME Slot.
* Booted to UEFI Windows Server 2019 using SATA HDD.
* Booted to UEFI RHEL 8.3 using SATA HDD and U2 SSD.
* Booted to Ubuntu 18.04 on SATA slot and U2 SSD.
* Verified PCIE LAN card detection during POST and OS.
* Verified TPM offboard chip detection

**This firmware project has only been tested on the Aowanda AD1S01 hardware**.
* This firmware project build has only been tested using the Microsoft Visual Studio 2015 build tools.
* Booted to UEFI shell.
* Booted to UEFI Windows Server 2019 on M.2 NVME Slot.
* Booted to UEFI RHEL 8.3 using SATA on M.2 NVME Slot.
* Verified onboard PCIE LAN card detection in POST and OS.
* Verified TPM offboard chip detection in POST and OS.
* All the above testing is done using AMI MEGARAC SPX FW version 0.14.0 Remote KVM redirection
  
  
### **New Features**
* None

### **Planned Activities**
* Sync with latest EDKII and EDKII platforms

### **Additional Support and Customizations**
*	To get dedicated support or additional features or customizations for Aptio OpenEdition, feel free to email sales@ami.com
