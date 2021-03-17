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

@REM #
@REM #  Module Name:
@REM #
@REM #    postbuild.bat
@REM #
@REM #  Abstract:
@REM #
@REM #    Post build script.
@REM #
@REM #--*/

@set SCRIPT_ERROR=0

@if /I not "%0" == "%WORKSPACE_PLATFORM%\%PROJECT%\postbuild.bat" (
  if /I not "%0" == "%WORKSPACE_PLATFORM%\%PROJECT%\postbuild" (
    echo.
    echo !!! ERROR !!! This postbuild.bat must run under workspace root using "%WORKSPACE_PLATFORM%\%PROJECT%\postbuild.bat" !!!
    echo.
    set SCRIPT_ERROR=1
    goto :EOF
  )
)

@cd %WORKSPACE_PLATFORM%

@cd %WORKSPACE%

@if %FSP_WRAPPER_BUILD% EQU TRUE (
  del /f %WORKSPACE_FSP_BIN%\KabylakeFspBinPkg\Fsp_Rebased*.fd
)

@if %FSP_WRAPPER_BUILD% EQU TRUE exit /b

