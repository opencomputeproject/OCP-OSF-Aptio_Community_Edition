@REM @file
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

@echo OFF
@set PrepRELEASE=DEBUG
@set SILENT_MODE=FALSE
@set CapsuleBuild=FALSE

@set EXT_CONFIG_CLEAR=
@set EXT_BUILD_FLAGS=

:CmdLineParse
if "" == "%1" (
  goto Continue
) else if "r" == "%1" (
  set PrepRELEASE=RELEASE
) else if "tr" == "%1" (
  set PrepRELEASE=TEST_RELEASE
) else if "rp" == "%1" (
  set PrepRELEASE=RELEASE_PDB
) else if "s" == "%1" (
  set SILENT_MODE=TRUE
) else if "help" == "%1" (
  goto PrepHelp
) else (
  echo Invalid input arguments: %1
  echo.
  goto PrepHelp
)
SHIFT
goto CmdLineParse

:PrepHelp
@echo Preparation for BIOS build.
@echo.
@echo prep [r][rp][s][help]
@echo.
@echo   r         To do release build. Default is debug build. See note 1
@echo   rp        To do release build with Symbols - For source level debugging. See note 1
@echo   s         To build in silent mode. . See note 1
@echo.
@echo 1) Re-running prep without these arguments cannot be used for
@echo    incremental build. Hence, these inputs must be supplied each time
@echo    prep are desired to be re-run.
@echo.
goto PrepDone

:Continue
@echo ==============================================

if exist %WORKSPACE%\Prep.log del %WORKSPACE%\Prep.log

:PrepReleaseCheck

@if %SILENT_MODE% EQU TRUE goto BldSilent

call prebuild.bat %PrepRelease% %CapsuleBuild%
goto PrePrepDone

:BldSilent
@echo ************************************************************************ >> %WORKSPACE%\Prep.log
@echo ***********           Prebuild.bat is launched here          *********** >> %WORKSPACE%\Prep.log
@echo ************************************************************************ >> %WORKSPACE%\Prep.log
call prebuild.bat %PrepRelease% %CapsuleBuild% 1>>%WORKSPACE%\Prep.log 2>&1

:PrePrepDone
@If %SCRIPT_ERROR% EQU 1 goto PrepFail
@goto PrepDone

:PrepFail
@echo.
@echo !! The EDKII BIOS build has failed in prep!
@echo.
@exit /b 1

:PrepDone