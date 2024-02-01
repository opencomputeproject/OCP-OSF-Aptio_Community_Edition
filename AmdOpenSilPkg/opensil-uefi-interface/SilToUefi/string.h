/**
 * @file  string.h
 * @brief Standard type definitions and their limits.
 *
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#define _STRING


void * EFIAPI memset(void *str, int c, size_t n);
//#pragma intrinsic(memset)
void * EFIAPI memcpy(void *dest, const void *src, size_t n);
//#pragma intrinsic(memcpy)

