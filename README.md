AMI Aptio OE Genoa openSIL Project
===============

This is BIOS FW project based on AMI Aptio Open Edition framework. It implements the firmware for AMD Genoa platform. This implementation includes AMD openSIL library as a replacement of AGESA.

The platform used for development and validation: **Onyx** - AMD Genoa reference board.



Build Requirements
------------------

### Required Tools for Windows

  * **Git**

    Download URL: https://git-scm.com/

    Make sure any proxy requirements are set in the git config settings.

  * **Microsoft Visual Studio 2019** (tested)

    Make sure Visual Studio and the SDK are properly configured for your
    environment.

    The build will ultimately execute edksetup.bat from edk2 open source which
    should be able to detect properly installed Visual Studio components and
    SDKs.

    For inspiration on Visual Studio and SDK Environment Variables, please
    refer to:

    `edk2/Conf/tools_def.txt`

    `Platform/PlatformTools/BuildTools-env.cmd`

    If you do not have a Visual Studio install which can be located by
    edksetup.bat, you will need to configure all the proper PREFIX variables
    required for the build.

    *  **Microsoft SDK**

      Match chosen version of Microsoft Visual Studio.

  * **Python 3.x** (tested 3.7.4, 3.9 & 3.11.1)

    Download URL: https://www.python.org

    Environment Variable: PYTHON_HOME

    E.g., `PYTHON_HOME = C:\Python39`

  * **Perl** (tested 5.32.1.1)

    Download URL: https://strawberryperl.com (tested)

    Strawberry Perl might require separately installing XML::LibXML
    `cpan install XML::LibXML`

    Environment Variable: PERL_PATH
    E.g., `PERL_PATH=C:\Strawberry\perl\bin`

    Alternatively, ActiveState perl is available if there is trouble
    installing Strawberry Perl.

  * **NASM** (tested 2.15.05)

    Environment Variable: NASM_PREFIX

  * **ASL compiler** (tested 20200110)

    Environment Variable: ASL_PREFIX

### Required Tools for Linux

  * **Packages to install**

    build-essential

    uuid-dev

    python3

    python3-pip

    perl

    libperl-dev

    libxml-simple-perl

    libxml-parser-perl

    curl

    ca-certificates

    nasm

    iasl

### Required CRB support files

  * **edk2**

    version: edk2-stable202205

  * **edk2-platforms**

    version: b8ffb76b471dae5e24badcd9e04033e8c9439ce3

  * All CRBs are using the AST 2600 BMC chip.  For UEFI video support on the
    CRBs the GOP EFI driver must be obtained and placed in:
    `CrbSupportPkg\BmcGopDxe\X64\uefi_2600.efi>` Latest drivers can be downloaded from https://www.aspeedtech.com/support_driver/

Downloading and building the project
-----------------

### Download the project
`git clone [project git URL] --recursive -b OE-AMI-Genoa_openSIL`  
`git submodule update --init --checkout --recursive`  

*Note: **-b OE-AMGenoa_openSIL** option is needed to directly switch to OE-AMI-Genoa_openSIL branch. If not specified, git switches to main; the submodule set is different between main and OE-AMI-Genoa_openSIL and following submodule update might be ugly.*

### Building Platform BIOS (Windows CMD prompt)

  * From the workspace directory, run the dbuild.cmd script.

    ex: `dbuild.cmd genoa-onyx --edk2args="-t VS2019"`

    By default (without options), the script will display a usage message and
    list all available CRBs to build.

  * To receive more verbose information for the first Level commands run:

    `dbuild --help`

  * Commands can have additional options.  Example:

    `dbuild genoa-onyx --help`

  * If you are not configuring the proper PREFIX variables for Visual Studio,
    you will need to provide an edk2args override to supply the tagname.
    The build process will use this to configure the build output directory
    before the edk2 build commences

    `--edk2args="-t VS2019"`

  * The final BIOS will be placed in <workspace>\\*.FD

### Building Platform BIOS (Linux bash)

  * Make sure your build environment is configured as referenced in
    [Required Tools for Linux](#required-tools-for-linux)

  * From the workspace directory, run the dbuild.sh script.

    ex: `./dbuild.sh genoa-onyx`

    By default (without options), the script will display a usage message and
    list all available CRBs to build.

  * To receive more verbose information for the first Level commands run:

    `./dbuild.sh --help`

  * Commands can have additional options.  Example:
    `./dbuild.sh genoa-onyx --help`

  * The final BIOS will be placed in <workspace>\\*.FD

## Enable CRB Serial Debug output

  * The EDKII build defaults to a "RELEASE" type build, which does not include
    serial debug output.

    To build a "DEBUG" build, pass to dbuild `--edk2args="-b DEBUG"`.

    Make sure to surround the arguments passed via edk2args with double quotes.

    In the Project.dsc file,
    `gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel` can be modified
    to change the EDK2 debug output.

## Enable Secureboot

  * Place the secureboot keys inside the respective subfolders within
    Platform/SecurebootKeys folder.

  * Modify Project.fdf and add/modify the path of secureboot keys in
    [FV.FvSecurityLate] section.

  * Uncomment the line
    `gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE` in Project.dsc

  * On the first boot, the keys can be enrolled by selecting "Reset Secure Boot
    Keys" option in the BIOS setup menu.

## Current Level of Functionality

  * Windows Datacenter 2019 and Linux Ubuntu 22.04LTS installation and boot support on Onyx CRB Hardware

  * SOL Serial console: 115,200/8/1/N

  * USB ports (Keyboard, Storage, etc.)

  * On-board VGA (BMC) console

  * Support for boot from USB and NVMe

  * Shell command-line extension for "AcpiView" utility:
    https://github.com/tianocore/edk2/tree/master/ShellPkg/Library/UefiShellAcpiViewCommandLib

## Current known Issues and Limitations

  * Platform-specific ACPI and SMBIOS tables minimally implemented.
