@REM @file
@REM
@REM Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
@REM This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

@REM ***********************************************************************
@REM *                                                                     *
@REM *   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
@REM *                                                                     *
@REM *      All rights reserved.                                           *
@REM *                                                                     *
@REM *      This program and the accompanying materials are licensed and   *
@REM *      made available under the terms and conditions of the BSD       *
@REM *      License that accompanies this distribution.  The full text of  *
@REM *      the license may be found at:                                   *
@REM *      http://opensource.org/licenses/bsd-license.php.                *
@REM *                                                                     *
@REM *      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
@REM *      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
@REM *      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
@REM *                                                                     *
@REM ***********************************************************************

@echo off

REM Run setlocal to take a snapshot of the environment variables.  endlocal is called to restore the environment.
setlocal
set SCRIPT_ERROR=0

REM ---- Do NOT use :: for comments Inside of code blocks() ----

::**********************************************************************
:: Initial Setup
::**********************************************************************

:parseCmdLine
if "%1"=="" goto :argumentCheck

if /I "%1"=="debug"          set TARGET=DEBUG
if /I "%1"=="release"        set TARGET=RELEASE

if /I "%1"=="clean" (
  set BUILD_TYPE=cleantree
  call :cleantree
  goto :EOF
)

shift
GOTO :parseCmdLine

:argumentCheck:

if /I "%TARGET%" == "" (
  echo Info: debug/release argument is empty, use DEBUG as default
  set TARGET=DEBUG
)

REM Art to notify which board you're working on
echo.
type logo.txt
echo.

::
:: Build configuration
::
set BUILD_REPORT_FLAGS=
set BUILD_CMD_LINE=
set BUILD_LOG=%WORKSPACE%\Build\build.log
set BUILD_REPORT=%WORKSPACE%\Build\BuildReport.txt

del %BUILD_LOG% *.efi *.log 2>NUL

echo --------------------------------------------------------------------------------------------
echo.
echo                                Purley Build Start
echo.
echo --------------------------------------------------------------------------------------------


:doPreBuild
echo.
echo --------------------------------------------------------------------
echo.
echo                          Prebuild Start
echo.
echo --------------------------------------------------------------------
call prebuild.bat
if %SCRIPT_ERROR% NEQ 0 EXIT /b %ERRORLEVEL%

echo --------------------------------------------------------------------
echo.
echo                          Prebuild End
echo.
echo --------------------------------------------------------------------
if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
timeout 1

:buildBios
set BUILD_CMD_LINE=%BUILD_CMD_LINE% -D MAX_SOCKET=%MAX_SOCKET% -y %BUILD_REPORT%
echo --------------------------------------------------------------------
echo.
echo                          Build Start
echo.
echo --------------------------------------------------------------------
echo.
echo build %BUILD_CMD_LINE% --log=%BUILD_LOG% %BUILD_REPORT_FLAGS%
call build %BUILD_CMD_LINE% --log=%BUILD_LOG% %BUILD_REPORT_FLAGS%
echo --------------------------------------------------------------------
echo.
echo                          Build End
echo.
echo --------------------------------------------------------------------
if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
timeout 1

:postBuild

echo --------------------------------------------------------------------
echo.
echo                          PostBuild Start
echo.
echo --------------------------------------------------------------------
echo.
call postbuild.bat
if %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
timeout 1
echo --------------------------------------------------------------------
echo.
echo                          PostBuild End
echo.
echo --------------------------------------------------------------------

echo %date%  %time%
echo.

echo --------------------------------------------------------------------------------------------
echo.
echo                                Purley Build End
echo.
echo --------------------------------------------------------------------------------------------

:done
endlocal & EXIT /b %SCRIPT_ERROR%

::--------------------------------------------------------  
::-- Function section starts below here  
::-------------------------------------------------------- 
:cleantree
choice /t 3 /d y /m "Confirm: clean tree of intermediate files created in tree during build"
if %ERRORLEVEL% EQU 2 goto :EOF
goto :EOF


:ErrorHandler:
echo Error handler