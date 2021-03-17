@REM @file
@REM
@REM Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
@REM This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

@echo off

pushd ..\..\..\..\..\

@REM Set WORKSPACE environment.
set WORKSPACE=%cd%
echo.
echo Set WORKSPACE as: %WORKSPACE%
echo.

@REM Check whether Git has been installed and been added to system path.
git --help >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo.
  echo The 'git' command is not recognized.
  echo Please make sure that Git is installed and has been added to system path.
  echo.
  goto :EOF
)

@REM Create the Conf directory under WORKSPACE
if not exist %WORKSPACE%\Conf (
  mkdir Conf
)

@REM Set other environments.
@REM Basic Rule:
@REM   Platform override Silicon override Core
@REM   Source override Binary

set PACKAGES_PATH=%WORKSPACE%\edk2-platforms\Platform\Intel;%WORKSPACE%\edk2-platforms\Silicon\Intel;%WORKSPACE%\edk2-non-osi\Silicon\Intel;%WORKSPACE%\FSP;%WORKSPACE%\edk2;%WORKSPACE%
set EDK_TOOLS_BIN=%WORKSPACE%\edk2-BaseTools-win32

@if not defined PYTHON_HOME (
  @if exist C:\Python27 (
    set PYTHON_HOME=C:\Python27
  )
)

set EDK_SETUP_OPTION=
@rem if python is installed, disable the binary base tools.
if defined PYTHON_HOME (
  set EDK_TOOLS_BIN=
  set EDK_SETUP_OPTION=--nt32
)
pushd %WORKSPACE%\edk2
call edksetup.bat %EDK_SETUP_OPTION%
popd
pushd %WORKSPACE%
@rem if python is installed, nmake BaseTools source and enable BaseTools source build
@if defined PYTHON_HOME (
  nmake -f %BASE_TOOLS_PATH%\Makefile
)
popd

set openssl_path=%WORKSPACE%

popd

goto :EOF

:Help
echo.
echo Usage:
echo GitEdk2.bat [-w Workspace_Directory] (optional) [-b Branch_Name] (optional)
echo.
echo -w    A absolute/relative path to be the workspace.
echo       Default value is the current directory.
echo.
echo -b    The branch name of the repository. Currently, only master, udk2015,
echo       trunk (same as master) and bp13 (same as udk2015) are supported.
echo       Default value is master.
echo.
