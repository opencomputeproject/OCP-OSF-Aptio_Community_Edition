ARM Trusted Firmware for Raspberry Pi 3
=======================================

The `bl1.bin` and `fip.bin` TF-A binaries found in this directory were built from the
[official TF-A 2.3 release](https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tag/?h=v2.3)
through an [AppVeyor build script](https://github.com/pbatard/pitf/blob/master/appveyor.yml)
that is designed to provide evidence that these binaries match the vanilla TF-A source.

As per the [AppVeyor build log](https://ci.appveyor.com/project/pbatard/pitf/builds/32330098),
the SHA-256 sums for the blobs can be validated to be as follows:
- `bl1.bin`: `28d70adc6e7041582264874d342bcad992adb8d34c9de5813e661029d0189b3b`
- `fip.bin`: `02a8c3ea9227fbe60ecfc20999db6bb755d0b32fd757a596353e068e1814e171`

For Raspberry Pi 3 usage, TF-A was built using the command:
```
make PLAT=rpi3 RPI3_PRELOADED_DTB_BASE=0x10000 PRELOADED_BL33_BASE=0x30000 SUPPORT_VFP=1 RPI3_USE_UEFI_MAP=1 DEBUG=0 fip all
```
which results in the following memory mapping:
```
    0x00000000 +-----------------+
               |       ROM       | BL1
    0x00010000 +-----------------+
               |     Nothing     |
    0x00020000 +-----------------+
               |       FIP       |
    0x00030000 +-----------------+
               |                 |
               |  UEFI PAYLOAD   |
               |                 |
    0x001f0000 +-----------------+
               |       DTB       | (Loaded by the VideoCore)
    0x00200000 +-----------------+
               |   Secure SRAM   | BL2, BL31
    0x00300000 +-----------------+
               |   Secure DRAM   | BL32 (Secure payload)
    0x00400000 +-----------------+
               |                 |
               |                 |
               | Non-secure DRAM | BL33
               |                 |
               |                 |
    0x01000000 +-----------------+
               |                 |
               |       ...       |
               |                 |
    0x3F000000 +-----------------+
               |       I/O       |
```
