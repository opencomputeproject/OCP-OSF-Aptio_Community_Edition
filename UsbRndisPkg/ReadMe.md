# UsbRndisPkg

​	The package provides support for UsbRndis feature which is mainly used for REDFISH BIOS-BMC communication. 

  

## How to include UsbRndisPkg to the project?

- `PcdUsbRndisFeatureEnable` is defined in UsbRndisPkg.dec file. This PCD is used to enable or disable the feature. 

- Add UsbRndisPkg.dec in project DSC file under `Packages` section.

  ```
  UsbRndisPkg/UsbRndisPkg.dec
  ```

- Add the PCD in project DSC file under `PcdsFeatureFlag` section and enable it as shown below.

  ```
  gUsbRndisPkgTokenSpaceGuid.PcdUsbRndisFeatureEnable|TRUE
  ```
  
- Add UsbRndisPkg modules based on `PcdUsbRndisFeatureEnable` under `Components.X64` section in project DSC file as shown below. 

  ```
  !if gUsbRndisPkgTokenSpaceGuid.PcdUsbRndisFeatureEnable == TRUE
    UsbRndisPkg/UsbRndis/UsbRndis.inf
    UsbRndisPkg/UsbLan/UsbLan.inf
  !endif
  ```
  
- Similarly, add UsbRndisPkg modules based on `PcdUsbRndisFeatureEnable` under a FV in project FDF file as shown below

  ```
  !if gUsbRndisPkgTokenSpaceGuid.PcdUsbRndisFeatureEnable == TRUE
    INF UsbRndisPkg/UsbRndis/UsbRndis.inf
    INF UsbRndisPkg/UsbLan/UsbLan.inf
  !endif
  ```



## Disabling UsbRndis feature

​	To disable UsbRndis feature, set `PcdUsbRndisFeatureEnable` to FALSE in project DSC file.
