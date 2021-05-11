ARM Trusted Firmware for Raspberry Pi 4
=======================================

The `bl31.bin` TF-A binary found in this directory was built from the
[official TF-A 2.3 release](https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tag/?h=v2.3)
through an [AppVeyor build script](https://github.com/pbatard/pitf/blob/master/appveyor.yml)
that is designed to provide evidence that these binaries match the vanilla TF-A source.

As per the [AppVeyor build log](https://ci.appveyor.com/project/pbatard/pitf/builds/32330098),
the SHA-256 sum for the bin can be validated to be as follows:
- `bl31.bin`: `b868ef51cead73ab96b2af778334eb063f6bc8009736c1a16080edc21796dc6a`

For Raspberry Pi 4 usage, TF-A was built using the command:
```
make PLAT=rpi4 RPI3_PRELOADED_DTB_BASE=0x1F0000 PRELOADED_BL33_BASE=0x20000 SUPPORT_VFP=1 DEBUG=0 all
```
