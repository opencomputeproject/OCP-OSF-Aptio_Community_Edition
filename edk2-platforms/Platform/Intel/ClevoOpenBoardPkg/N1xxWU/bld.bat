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

:: Useage: bld [/s] [/f <FEATURE_PCD_NAME> <FALSE or TRUE>] [/r]
::
:: For a given build command, 3 options may be passed into this batch file via command prompt:
:: 1) /s = Redirects all output to a file called EDK2.log(Prep.log must be existed), which will be located at the root.
:: 2) /f = Defines the passing in of a single override to a feature PCD that is used in the platform
::    DSC file.  If this parameter is used, it is to be followed immediately after by both the feature
::    pcd name and value. FeaturePcd is the full PCD name, like gMinPlatformPkgTokenSpaceGuid.PcdOptimizeCompilerEnable
:: 3) /r = Useful for faster rebuilds when no changes have been made to .inf files. Passes -u to
::    build.exe to skip the generation of makefiles.
:: 4) rom = Build Bios.rom only and building SPIs will be skipped.
::

@echo on

cd %WORKSPACE%

@REM
@REM Build FSP Binary
@REM
@if not defined FSP_BINARY_BUILD goto :SkipFspBinaryBuild
@if %FSP_BINARY_BUILD% EQU FALSE goto :SkipFspBinaryBuild
@set FSP_BUILD_PARAMETER=/d
@set FSP_PKG_NAME=KabylakeFspPkg
@if /I "%TARGET%" == "RELEASE" (
  @if "%FSP_TEST_RELEASE%"=="TRUE" (
    set FSP_BUILD_PARAMETER=/tr
  ) else (
    set FSP_BUILD_PARAMETER=/r
  )
)

@if %FSP_WRAPPER_BUILD% EQU FALSE goto :BldEnd
:SkipFspBinaryBuild

@if %FSP_WRAPPER_BUILD% EQU FALSE goto :SkipPatchFspBinFvsBaseAddress
del /f %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased*.fd

cd %WORKSPACE%

if exist %WORKSPACE_PLATFORM%\%PROJECT%\OpenBoardPkgPcd.dsc attrib -r %WORKSPACE_PLATFORM%\%PROJECT%\OpenBoardPkgPcd.dsc
@call %PYTHON_HOME%\python.exe %WORKSPACE_PLATFORM%\%PLATFORM_PACKAGE%\Tools\Fsp\RebaseAndPatchFspBinBaseAddress.py %WORKSPACE_PLATFORM%\%PROJECT%\Include\Fdf\FlashMapInclude.fdf %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg Fsp.fd %WORKSPACE_PLATFORM%\%PROJECT%\OpenBoardPkgPcd.dsc 0x0

@if %ERRORLEVEL% NEQ 0 (
  @echo !!! ERROR:RebaseAndPatchFspBinBaseAddress failed!!!
  set SCRIPT_ERROR=1
  goto :BldFail
)

cd %WORKSPACE%

copy /y /b %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased_S.fd+%WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased_M.fd+%WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased_T.fd %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased.fd
:SkipPatchFspBinFvsBaseAddress


@SET SILENT_MODE=FALSE
@SET REBUILD_MODE=
@SET BUILD_ROM_ONLY=

:: Loop through arguements until all are processed

:BUILD_FLAGS_LOOP

@if "%~1" == "" goto BUILD_FLAGS_LOOP_DONE

@if "%~1" == "/f" (
  shift
  goto BUILD_FLAGS_LOOP
)
@if "%~1" == "/s" (
  SET SILENT_MODE=TRUE
  shift
  goto BUILD_FLAGS_LOOP
)
@if "%~1" == "/r" (
  SET REBUILD_MODE=-u
  shift
  goto BUILD_FLAGS_LOOP
)
@if "%~1" == "rom" (
  SET BUILD_ROM_ONLY=rom
  shift
  goto BUILD_FLAGS_LOOP
)
:: Unknown build flag.
shift
goto BUILD_FLAGS_LOOP
:BUILD_FLAGS_LOOP_DONE

:: Output the build variables the user has selected.

@echo.
@echo  User Selected build options:
@echo    SILENT_MODE = %SILENT_MODE%
@echo    REBUILD_MODE = %REBUILD_MODE%
@echo    BUILD_ROM_ONLY = %BUILD_ROM_ONLY%
@echo.

@if %SILENT_MODE% EQU TRUE goto BldSilent

call build -n %NUMBER_OF_PROCESSORS% %REBUILD_MODE% %EXT_BUILD_FLAGS%

@if %ERRORLEVEL% NEQ 0 goto BldFail
@echo.
@echo Running postbuild.bat to complete the build process.
@echo.
call %WORKSPACE_PLATFORM%\%PROJECT%\postbuild.bat %BUILD_ROM_ONLY%
@if %SCRIPT_ERROR% EQU 1 goto BldFail
@goto BldSuccess

:BldSilent
@if exist Build.log del Build.log

@echo. > Build.log
@echo ************************************************************************ >> Build.log
@echo ***********             Build.bat is launched here           *********** >> Build.log
@echo ************************************************************************ >> Build.log
@echo. >> Build.log

call build -n %NUMBER_OF_PROCESSORS% %REBUILD_MODE% %EXT_BUILD_FLAGS% 1>>Build.log 2>&1

@if %ERRORLEVEL% NEQ 0 goto BldFail
@echo. >> Build.log
@echo Running postbuild.bat to complete the build process. >> Build.log
@echo. >> Build.log
@call %WORKSPACE_PLATFORM%\%PROJECT%\postbuild.bat %BUILD_ROM_ONLY% 1>>Build.log 2>&1
@If %SCRIPT_ERROR% EQU 1 goto BldFail

:BldSuccess
@echo.
@echo TARGET:               %TARGET%
@echo TOOL_CHAIN_TAG:       %TOOL_CHAIN_TAG%
@echo BIOS location:        %BUILD_DIR%\FV
@echo.
@echo The EDKII BIOS build has successfully completed!
@echo.
@REM

@goto BldEnd

:BldFail
cd %WORKSPACE_PLATFORM%\%PROJECT%
@echo.
@echo The EDKII BIOS Build has failed!
@echo.
@exit /b 1

:BldEnd
@if %SILENT_MODE% EQU TRUE (
  @if exist EDK2.log del EDK2.log
  @if exist Prep.log if exist Build.log copy Prep.log+Build.log EDK2.log
)

cd %WORKSPACE_PLATFORM%\%PROJECT%