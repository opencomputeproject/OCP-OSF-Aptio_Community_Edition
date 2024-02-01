/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include <Library/SmbiosMiscLib.h>

SMBIOS_PORT_CONNECTOR_RECORD gPortConnectorRecord[] = {
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeUsb,  //0x12, ExternalConnectorType.
      PortTypeUsb //0x10, PortType.
    },
    {
      "J11",
      "USB3-R"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeUsb,  //0x12, ExternalConnectorType.
      PortTypeUsb //0x10, PortType.
    },
    {
      "J20",
      "USB3-R"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeUsb,  //0x12, ExternalConnectorType.
      PortTypeUsb //0x10, PortType.
    },
    {
      "J1F",
      "USB3-F"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeUsb,  //0x12, ExternalConnectorType.
      PortTypeUsb //0x10, PortType.
    },
    {
      "J2F",
      "USB3-F"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeDB15Female, //0x07, ExternalConnectorType.
      PortTypeVideoPort //0x1C, PortType.
    },
    {
      "J2",
      "VGA-R"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeDB15Female, //0x07, ExternalConnectorType.
      PortTypeVideoPort //0x1C, PortType.
    },
    {
      "J3-F",
      "VGA-F"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeDB9Female, //0x09, ExternalConnectorType.
      PortTypeSerial16550ACompatible //0x09, PortType.
    },
    {
      "J1",
      "Serial Port Header"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeNone, //0x00, InternalConnectorTypr.
      0x02, //ExternalReferenceDesignator.
      PortConnectorTypeRJ45, //0x0B, ExternalConnectorType.
      PortTypeNetworkPort //0x1F, PortType.
    },
    {
      "J15",
      "MGMT RJ45 Port"
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeOther , //0xFF, InternalConnectorTypr.
      0x00, //ExternalReferenceDesignator.
      PortConnectorTypeNone, //ExternalConnectorType.
      PortTypeOther //0xFF, PortType.
    },
    {
      "J75 M2_0",
      ""
    }
  },
  {
    { {0},
      0x01, //InternalReferenceDesignator.
      PortConnectorTypeOther , //0xFF, InternalConnectorTypr.
      0x00, //ExternalReferenceDesignator.
      PortConnectorTypeNone, //ExternalConnectorType.
      PortTypeOther //0xFF, PortType.
    },
    {
      "J77 M2_1",
      ""
    }
  }
};

/**
  This function outputs total number of port connectors present on board.

  @param[out] NumPortConnector   Pointer to output variable.

  @retval EFI_SUCCESS            Function successfully executed.
  @retval EFI_INVALID_PARAMETER  If NumPortConnector == NULL.

**/
EFI_STATUS
EFIAPI
GetNumberOfPortConnectors (
  OUT UINT8  *NumPortConnector
  )
{
  UINT8                     NumberOfPortConnector;

  if (NumPortConnector == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Calculate total number of port connectors.
  NumberOfPortConnector =
          sizeof (gPortConnectorRecord)/sizeof (SMBIOS_PORT_CONNECTOR_RECORD);

  *NumPortConnector = NumberOfPortConnector;

  return EFI_SUCCESS;
}

/**
  Output the Type8 Smbios related information for indexed port connector.

  @param[in]  PortConnectorIdNum  Port connector identifier number.
  @param[out] PortConnRecord      Pointer to output type8 port connector record.

  @retval EFI_SUCCESS             Function successfully executed.
  @retval EFI_INVALID_PARAMETER   If PortConnRecord == NULL.

**/
EFI_STATUS
EFIAPI
GetType8PortConnectorInfo (
  IN  UINT8                         PortConnectorIdNum,
  OUT SMBIOS_PORT_CONNECTOR_RECORD  *PortConnRecord
  )
{
  UINT8                     NumberOfPortConnector;

  if (PortConnRecord == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Calculate total number of port connectors.
  NumberOfPortConnector =
          sizeof (gPortConnectorRecord)/sizeof (SMBIOS_PORT_CONNECTOR_RECORD);

  if (PortConnectorIdNum >= NumberOfPortConnector) {
    return EFI_INVALID_PARAMETER;
  }

  *PortConnRecord =  gPortConnectorRecord[PortConnectorIdNum];

  return EFI_SUCCESS;
}
