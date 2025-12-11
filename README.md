# FixBrot

Mandelbrot Viewer for [PicoPad](https://github.com/pajenicko/picopad).

## How to Build (Linux/WSL2)

```sh
git clone https://github.com/Panda381/PicoLibSDK.git
export PICOLIBSDK_PATH=${PWD}/PicoLibSDK
git clone https://github.com/shapoco/fixbrot.git
cd fixbrot/app/picopad
make DEVICE=picopad20 all
```

binary generated in `fixbrot/app/picopad/bin/`.
