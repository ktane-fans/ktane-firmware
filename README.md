# KTANE ![CI Status](https://github.com/ktane-fans/ktane-firmware/workflows/CI/badge.svg) ![GitHub](https://img.shields.io/github/license/ktane-fans/ktane-firmware)

Video of it in action: https://www.youtube.com/watch?v=fBK7vhbQetI

```shell
git clone --recursive https://github.com/ktane-fans/ktane-firmware
```

### Docs
- [Particle OS API](https://docs.particle.io/reference/device-os/firmware/photon/)
- [Photon Datasheet](https://docs.particle.io/datasheets/wi-fi/photon-datasheet/)

## Compile
Prerequisites
- `particle-cli` (`npm i -g particle-cli`)
- GNU Make (`apt install build-essential`)

```shell
make compile
make flash # target devices are configured in the Makefile
```
