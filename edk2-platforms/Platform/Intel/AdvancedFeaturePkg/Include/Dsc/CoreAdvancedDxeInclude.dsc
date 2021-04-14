## @file
#  Platform description.
#
# Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##


  #
  # UEFI network modules
  #
!if gAdvancedFeaturePkgTokenSpaceGuid.PcdNetworkEnable == TRUE
  NetworkPkg/DpcDxe/DpcDxe.inf
  NetworkPkg/SnpDxe/SnpDxe.inf
  NetworkPkg/MnpDxe/MnpDxe.inf
  NetworkPkg/VlanConfigDxe/VlanConfigDxe.inf
  NetworkPkg/ArpDxe/ArpDxe.inf
  NetworkPkg/Dhcp4Dxe/Dhcp4Dxe.inf
  NetworkPkg/Ip4Dxe/Ip4Dxe.inf
  NetworkPkg/Mtftp4Dxe/Mtftp4Dxe.inf
  NetworkPkg/Udp4Dxe/Udp4Dxe.inf

  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf

  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  NetworkPkg/HttpBootDxe/HttpBootDxe.inf

  NetworkPkg/IScsiDxe/IScsiDxe.inf
  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
!endif

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable == TRUE
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
!endif

