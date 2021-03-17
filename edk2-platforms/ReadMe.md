# THIS BRANCH IS DEPRECATED!

**All Minimum Platform development has moved to edk2-platforms/master.** No more commits should be made to this branch.

Send all patches to the corresponding area on edk2-platforms/master.

# **EDK II Minimum Platform Firmware for Intel(R) Platforms**

The Minimum Platform is a software architecture that guides uniform delivery of Intel platforms enabling firmware
solutions for basic boot functionality with extensibility built-in.

Package maintainers for the Minimum Platform projects are listed in Maintainers.txt.

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
* The `KabylakeOpenBoardPkg` contains board implementations for Kaby Lake systems.
* The `PurleyOpenBoardPkg` contains board implementations for Purley systems.
* The `ClevoOpenBoardPkg` contains board implementations for Clevo systems.

## Board Package Organization
The board package follows the standard EDK II package structure with the following additional elements and guidelines:
* Only code usable across more than one board at the root level.
* Board-specific code in a directory. The directory name should match that of the board supported.
* Features not essential to achieve stage 5 or earlier boots are maintained in a Features folder at the appropriate
  level in the package hierarchy.

Shared resources in the package root directory can include interfaces described in header files, library instances,
firmware modules, binaries, etc. The UEFI firmware implementation is built using the process described below from the
board-specific directory.

A board package must implement the board APIs defined in the MinPlatformPkg even if a "NULL" implementation is used to
return back to the minimum platform caller.

## **Windows Build Instructions**

### Pre-requisites

* GIT client: Available from https://git-scm.com/downloads
* Microsoft Visual Studio.
  - Visual Studio 2015 recommended and is used in the examples below.
* ASL compiler: Available from http://www.acpica.org
  - Install into ```C:\ASL``` to match default tools_def.txt configuration.
* NASM assembler:  Available from: http://www.nasm.us/
  - Install into ```C:\NASM``` to match default tools_def.txt configuration.
* Python 2.7.6:  Available from: https://www.python.org/download/releases/2.7.6/
  - Install into ```C:\Python27``` to match default tools_def.txt configuration.
  - Add C:\Python27 to your path
  - Other versions of 2.7 may also work fine.

### Download the required components

1. Create a new directory for the EDK II WORKSPACE.

2. Download below repository to this WORKSPACE:

* edk2 repository
  * ``git clone https://github.com/tianocore/edk2.git``

* edk2-platforms repository
  * ``git clone https://github.com/tianocore/edk2-platforms.git -b devel-MinPlatform``

* edk2-non-osi repository
  * ``git clone https://github.com/tianocore/edk2-non-osi.git -b devel-MinPlatform``

* FSP repository
  * ``git clone https://github.com/IntelFsp/FSP.git -b Kabylake``

### Build

**Building with the python script**

1. Open command window, go to the workspace directory, e.g. c:\Kabylake.
2. Type "cd edk2-platforms\Platform\Intel
3. Type "python build_bios.py -p REPLACE_WITH_BOARD_NAME"

* build_bios.py arguments:

  | Argument              | Function                            |
  | ----------------------|-------------------------------------|
  | -h, --help            | show this help message and exit     |
  | --platform, -p        | the platform to build               |
  | --toolchain, -t       | tool Chain to use in build process  |
  | --DEBUG, -d           | debug flag                          |
  | --RELEASE, -r         | release flag                        |
  | --TEST_RELEASE, -tr   | test Release flag                   |
  | --RELEASE_PDB, -rp    | release flag                        |
  | --list, -l            | lists available platforms           |
  | --cleanall            | cleans all                          |
  | --clean               | cleans specified platform           |
  | --capsule             | capsule build enabled               |
  | --silent              | silent build enabled                |
  | --performance         | performance build enabled           |
  | --fsp                 | fsp build enabled                   |
  |                                                             |

* For more information on build options
  * ``Type "python build_bios.py -h"``

* Note
  * ``Python 2.7.16 and Python 3.7.3 compatible``
  * ``These python build scripts have been tested on Windows due to`` [cross-platform limitations](#Known-limitations)

* Configuration Files
  * ``The edk2-platforms\Platform\Intel\build.cfg file contains the default settings used by build_bios.py``
  * ``The default settings are under the DEFAULT_CONFIG section``
  * ``Each board can have a settings file that will override the edk2-platforms\Platform\Intel\build.cfg settings``
  * ``An example of a board specific settings:``
    * ``edk2-platforms\Platform\Intel\KabylakeOpenBoardPkg\KabylakeRvp3\build_config.cfg``

* Workspace view of the build scripts
  * <pre>
    WORKSPACE
          |------edk2
          |------edk2-non-osi
          |------edk2-platforms
          |       |---Platform
          |       |    |--Intel
          |       |        |------build.cfg: Default build settings. These are overridden by
          |       |        |                 platform specific settings (build_config.cfg) and
          |       |        |                 then command-line settings.
          |       |        |
          |       |        |------build_bios.py: Main build script. Generic pre-build, build,
          |       |        |                     post-build, and clean functions.
          |       |        |
          |       |        |------ClevoOpenBoardPkg
          |       |        |        |------N1xxWU
          |       |        |                |---build_config.cfg: N1xxWU specific build
          |       |        |                                      settings environment variables.
          |       |        |
          |       |        |------KabylakeOpenBoardPkg
          |       |        |        |------KabylakeRvp3
          |       |        |                  |---build_config.cfg: KabylakeRvp3 specific
          |       |        |                  |                     build settings, environment variables.
          |       |        |                  |---build_board.py: Optional board-specific pre-build, build
          |       |        |                                      and clean post-build functions.
          |       |        |------PurleyOpenBoardPkg
          |       |        |       |------BoardMtOlympus
          |       |        |                |---build_config.cfg: BoardMtOlympus specific
          |       |        |                |                     build settings, environment variables.
          |       |        |                |---build_board.py: Optional board-specific pre-build,
          |       |        |                |                   build, post-build and clean functions.
          |------FSP
  </pre>

**Building with the batch scripts**
For KabylakeOpenBoardPkg
1. Open command window, go to the workspace directory, e.g. c:\Kabylake.
2. Type "cd edk2-platforms\Platform\Intel\KabylakeOpenBoardPkg\KabylakeRvp3".
3. Type "GitEdk2MinKabylake.bat" to setup GIT environment.
4. Type "prep" and make prebuild finish for debug build, "prep r" for release build.
5. Type "bld" to build Kaby Lake reference platform UEFI firmware image.

For PurleyOpenBoardPkg
1. Open command window, go to the workspace directory, e.g. c:\Purley.
2. Type "cd edk2-platforms\Platform\Intel\PurleyOpenBoardPkg\BoardMtOlympus".
3. Type "GitEdk2MinMtOlympus.bat" to setup GIT environment.
4. Type "bld" to build Purley Mt Olympus board UEFI firmware image, "bld release" for release build, "bld clean" to
   remove intermediate files.

The validated version of iasl compiler that can build MinPurley is 20180629. Older version may generate ACPI build
errors.

For ClevoOpenBoardPkg
1. Open command window, go to the workspace directory, e.g. c:\Clevo.
2. Type "cd edk2-platforms\Platform\Intel\ClevoOpenBoardPkg\N1xxWU".
3. Type "GitEdk2Clevo.bat" to setup GIT environment.
4. Type "bld" to build Clevo UEFI firmware image, "bld release" for release build, "bld clean" to remove intermediate
files.

Users with access to the Intel proprietary FITC tool and ME ingredients can build full images for flash  (BIOS + ME +
DESC).

Users can also flash the UEFI firmware image to the highest area of the flash region directly.

### **Known limitations**

* All firmware projects can only build on Windows with the validated configuration below.
  * Cross-platform build support is work-in-progress.

**KabylakeOpenBoardPkg**
1. This firmware project has only been tested on the Intel KabylakeRvp3 board.
2. This firmware project has only been tested booting to Microsoft Windows 10 x64 with AHCI mode and Integrated Graphic
  Device.
3. This firmware project build has only been tested using the Microsoft Visual Studio 2015 compiler.

**PurleyOpenBoardPkg**
1. This firmware project has only been tested on the Microsoft MtOlympus board.
2. This firmware project has only been tested booting to Microsoft Windows Server 2016 with NVME on M.2 slot.
3. This firmware project build has only been tested using the Microsoft Visual Studio 2015 compiler.

**ClevoOpenBoardPkg**
1. Currently, support is only being added for the N1xxWU series of boards.
2. The firmware project build has only been tested using the Microsoft Visual Studio 2015 compiler.
3. The firmware project has not been tested on an actual board, it *should not* be expected to boot.
4. The firmware project applies to all Clevo supported board configurations but is only being tested on System 76 Galago
  Pro devices.

### **Planned Activities**
* Replace the batch build scripts with cross-platform Python build scripts.
* Publish a Minimum Platform specification to describe the architecture and interfaces in more detail.

### **Ideas**
If you would like to help but are not sure where to start some areas currently identified for improvement include:
 * Adding board ports for more motherboards and systems
 * Adding Clang support
 * Adding GCC support

Please feel free to contact Michael Kubacki (michael.a.kubacki at intel.com) and Isaac Oram (isaac.w.oram at intel.com)
if you would like to discuss contribution ideas.
