/******************************************************************************
 * Copyright (C) 2022 - 2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/**
 * ClearEspiIOForlegacyIO - clear espi IO range if it is equal to legacy Uart IO range.
 *
 * @param[in]  VOID
 */
VOID
ClearEspiIOForlegacyIO (
  VOID
  );

/**
 * ClsEspiIOForlegacyIO - clear espi IO range if it is equal to legacy Uart IO range.
 *
 * @param[in]  LegacyIO      LegacyIO[x]  0 :  the terminal of this array pointer   ex: LegacyIO[0] = 0x3F8; LegacyIO[1] = 0x2F8;  LegacyIO[2] = 0;
 */
VOID
ClsEspiIOForlegacyIO (
  IN  UINT16  *LegacyIO
  );