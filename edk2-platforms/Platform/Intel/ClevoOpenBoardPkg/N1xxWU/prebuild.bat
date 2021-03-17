@REM @file
@REM  Pre build script.
@REM
@REM Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
@REM This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

cd ..

@REM
@REM Set build capsule flag with default being OFF
@REM

@set CAPSULE_BUILD=0

@if /I "%2" == "TRUE" (
  @set CAPSULE_BUILD=1
  goto StartCapsulePrep
)

:StartCapsulePrep
@REM
@REM Define platform specific environment variables.
@REM
if not defined WORKSPACE_PLATFORM set WORKSPACE_PLATFORM=%WORKSPACE%\edk2-platforms\Platform\Intel
if not defined WORKSPACE_SILICON set WORKSPACE_SILICON=%WORKSPACE%\edk2-platforms\Silicon\Intel
if not defined WORKSPACE_PLATFORM_BIN set WORKSPACE_PLATFORM_BIN=%WORKSPACE%\edk2-non-osi\Platform\Intel
if not defined WORKSPACE_SILICON_BIN set WORKSPACE_SILICON_BIN=%WORKSPACE%\edk2-non-osi\Silicon\Intel
if not defined WORKSPACE_FSP_BIN set WORKSPACE_FSP_BIN=%WORKSPACE%\FSP
if not defined WORKSPACE_CORE set WORKSPACE_CORE=%WORKSPACE%\edk2
if not defined PLATFORM_PACKAGE set PLATFORM_PACKAGE=MinPlatformPkg
if not defined PLATFORM_BOARD_PACKAGE set PLATFORM_BOARD_PACKAGE=ClevoOpenBoardPkg
if not defined BOARD set BOARD=N1xxWU
if not defined PROJECT set PROJECT=%PLATFORM_BOARD_PACKAGE%\%BOARD%

@set SCRIPT_ERROR=0

@set CATALOG_DEBUG=0

@REM Set basic environment.
@echo.
@echo Prebuild:  Run edksetup.bat batch file.
@echo.
@if %CATALOG_DEBUG% == 0 (
  @del Conf\build_rule.txt
)
cd %WORKSPACE_CORE%
@call edksetup.bat
cd %WORKSPACE%
@set EFI_SOURCE=%WORKSPACE_CORE%

@REM
@REM Setup Visual Studio environment. Order of precedence is 2012, 2013, 2010 and then 2008.
@REM
@REM NOTE: To override precedence set TOOL_CHAIN_TAG before calling prep.bat.
@REM       Example: set TOOL_CHAIN_TAG=VS2008
@REM

@REM Check if tool chain has not been selected and Visual Studio 2014 is installed.
@if not defined TOOL_CHAIN_TAG (
  if defined VS140COMNTOOLS (
    set TOOL_CHAIN_TAG=VS2015
  )
)

@REM If Visual Studio 2014 is selected by priority or by preference, setup the environment variables.
@if /I "%TOOL_CHAIN_TAG%"=="VS2015" (
  echo.
  echo Prebuild:  Set the VS2015 environment.
  echo.
  if not defined VSINSTALLDIR call "%VS140COMNTOOLS%\vsvars32.bat"
  if /I "%VS140COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 14.0\Common7\Tools\" (
    set TOOL_CHAIN_TAG=VS2015
  ) else (
    set TOOL_CHAIN_TAG=VS2015x86
  )
)

@REM Check if tool chain has not been selected and Visual Studio 2013 is installed.
@if not defined TOOL_CHAIN_TAG (
  if defined VS120COMNTOOLS (
    set TOOL_CHAIN_TAG=VS2013
  )
)

@REM If Visual Studio 2013 is selected by priority or by preference, setup the environment variables.
@if /I "%TOOL_CHAIN_TAG%"=="VS2013" (
  echo.
  echo Prebuild:  Set the VS2013 environment.
  echo.
  if not defined VSINSTALLDIR call "%VS120COMNTOOLS%\vsvars32.bat"
  if /I "%VS120COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 12.0\Common7\Tools\" (
    set TOOL_CHAIN_TAG=VS2013
  ) else (
    set TOOL_CHAIN_TAG=VS2013x86
  )
)

@REM If no supported version of Visual Studio was detected, return an error.
@if not defined TOOL_CHAIN_TAG (
  echo.
  echo !!! ERROR !!! Visual Studio not installed correctly!!!
  echo.
  set SCRIPT_ERROR=1
  goto :EndPreBuild
)

echo Show CL revision
cl

@REM Set build TARGET.
@if /I "%1" == "" (
  set TARGET=DEBUG
  set TARGET_SHORT=D
) else if /I "%1" == "DEBUG" (
  set TARGET=DEBUG
  set TARGET_SHORT=D
) else if /I "%1" == "TEST_RELEASE" (
  set TARGET=RELEASE
  set TARGET_SHORT=R
) else if /I "%1" == "RELEASE" (
  set TARGET=RELEASE
  set TARGET_SHORT=R
) else if /I "%1" == "RELEASE_PDB" (
  set TARGET=RELEASE
  set TARGET_SHORT=R
) else (
  echo.
  echo !!! ERROR !!! Incorrect TARGET option for prebuild.bat. !!!
  echo.
  set SCRIPT_ERROR=1
  goto :EndPreBuild
)

@set BUILD_DIR_PATH=%WORKSPACE%\Build\%PROJECT%\%TARGET%_%TOOL_CHAIN_TAG%
@set BUILD_DIR=Build\%PROJECT%\%TARGET%_%TOOL_CHAIN_TAG%
@set BUILD_X64=%BUILD_DIR_PATH%\X64
@set BUILD_IA32=%BUILD_DIR_PATH%\IA32


@echo.
@echo Prebuild:  Set build environment.
@echo.
@if not exist %BUILD_DIR_PATH% (
  mkdir %BUILD_DIR_PATH%
)

@findstr /V "ACTIVE_PLATFORM TARGET TARGET_ARCH TOOL_CHAIN_TAG BUILD_RULE_CONF" %WORKSPACE%\Conf\target.txt > %BUILD_DIR_PATH%\target.txt
@echo ACTIVE_PLATFORM = %WORKSPACE_PLATFORM%/%PLATFORM_BOARD_PACKAGE%/%BOARD%/OpenBoardPkg.dsc        >> %BUILD_DIR_PATH%\target.txt
@echo TARGET          = %TARGET%                                  >> %BUILD_DIR_PATH%\target.txt
@echo TARGET_ARCH     = IA32 X64                                  >> %BUILD_DIR_PATH%\target.txt
@echo TOOL_CHAIN_TAG  = %TOOL_CHAIN_TAG%                          >> %BUILD_DIR_PATH%\target.txt
@echo BUILD_RULE_CONF = Conf/build_rule.txt                       >> %BUILD_DIR_PATH%\target.txt
@move /Y %BUILD_DIR_PATH%\target.txt Conf

@if %CAPSULE_BUILD% == 1 (
  goto EndCapsulePrep
)

@REM
@REM Set %FSP_WRAPPER_BUILD%
@REM
@set FSP_WRAPPER_BUILD=TRUE

@if %FSP_WRAPPER_BUILD% EQU TRUE (
  @REM Create dummy Fsp_Rebased_S_padded.fd to build the BiosInfo.inf if it is wrapper build, due to the SECTION inclusion
  echo "" > %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased_S_padded.fd
  attrib -r %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased_S_padded.fd
)

@REM
@REM Set %PERFORMANCE_BUILD%
@REM
@set PERFORMANCE_BUILD=FALSE

@REM
@REM Set %FSP_BINARY_BUILD% and %FSP_TEST_RELEASE%
@REM
@set FSP_BINARY_BUILD=FALSE
@set FSP_TEST_RELEASE=FALSE

@if "FSP_BINARY_BUILD"=="TRUE" (
  @if %FSP_WRAPPER_BUILD% EQU FALSE goto :EndPreBuild
)

@if not exist %BUILD_X64% (
  mkdir %BUILD_X64%
)

@set SECURE_BOOT_ENABLE=FALSE

@REM
@REM Skip BIOS_SIZE_OPTION if it is predefined
@REM
@if NOT "%BIOS_SIZE_OPTION%" == "" goto BiosSizeDone

@set BIOS_SIZE_OPTION=

@REM default size option is 6M
@set BIOS_SIZE_OPTION=-DBIOS_SIZE_OPTION=SIZE_60

:BiosSizeDone
@echo BIOS_SIZE_OPTION=%BIOS_SIZE_OPTION%

@echo   EFI_SOURCE           = %EFI_SOURCE%
@echo   TARGET               = %TARGET%
@echo   TARGET_ARCH          = IA32 X64
@echo   TOOL_CHAIN_TAG       = %TOOL_CHAIN_TAG%
@echo   WORKSPACE            = %WORKSPACE%
@echo   WORKSPACE_CORE       = %WORKSPACE_CORE%
@echo   EXT_BUILD_FLAGS      = %EXT_BUILD_FLAGS%
@echo.
:EndPreBuild
cd %WORKSPACE_PLATFORM%\%PROJECT%
