@REM @file
@REM
@REM Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
@REM SPDX-License-Identifier: BSD-2-Clause-Patent
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

