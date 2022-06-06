# Aptio OpenEdition Firmware for Mt Jade Platform

## Downloading source
git clone --recurse-submodules https://github.com/opencomputeproject/Aptio-OE.git -b OE-AMI-MtJade-YYYYMM
(replace the YYYYMM with tag version)

## Prerequisites to Build
* X86 Linux host machines running Ubuntu 18.04 and later.
* The Cross Compiler has to be downloaded from the ampere and configure it in Jade/Config.yml
* sudo apt-get install make gcc build-essential libssl-dev gawk uuid-dev
* sudo apt-get install python3.8
* sudo apt-get m4 bison flex
* To build UEFI image only :
    * Set “UEFI_BUILD_ONLY” to "yes" in Jade/Config.yml
* To build Full SPI NOR image :
    * Set “UEFI_BUILD_ONLY” to "no" in Jade/Config.yml
    * Download the ATF binary from SOC vendor and set the ATF_PATH in config.yml
    * Board setting file needs to be placed under edk2-platforms/Platform/Ampere/JadePkg

## BUILD Commands

* Only Linux build is supported
* JadeBuild.py is the main script file to build source.
* Build options:
   * -b : Build Mode(DEBUG,RELEASE)
   * -c : Clean the source, Build & BUILDS directories, and BaseTools
* Example : use  the command python3 JadeBuild.py -b release
* On successful build, BIOS rom file is created in BUILDS directory

## Validation 
<pre>
Configuration:
   Platform       : Ampere Mt Jade
   Processer      : Ampere Altra
   BMC Firmware   : AMI SPX
</pre>

1. This firmware project has been tested booting to UEFI shell
2. Installed and booted to CentOS 8.4 using M2 NVME Disk
3. Installed and booted to Windows 2022 using M2 NVME Disk
4. Connected PCIE Network card and made sure PCIE card detected in POST and in OS
5. Verified Basic BIOS - BMC communication

   
 
### **New Features**
* None

### **Planned Activities**
* Sync with latest EDKII and EDKII platforms

### **Additional Support and Customizations**
*	To get dedicated support or additional features or customizations for Aptio OpenEdition, feel free to email sales@ami.com
