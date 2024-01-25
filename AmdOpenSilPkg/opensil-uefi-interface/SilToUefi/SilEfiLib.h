/**
 * @file  SilEfiLib.h
 * @brief SIL to UEFI interface function prototypes
 *
 * Copyright 2021 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#pragma once

#define memset(s, c, n) SetMem(s, n, c)
