# FixBrot

Mandelbrot Viewer for [PicoPad](https://github.com/pajenicko/picopad) with Border Tracing Algorithm.

https://github.com/user-attachments/assets/04b31c8e-c4cc-4e2c-9aa6-9185a7eabbc5

## Key Assign

|Key|Funcion|
|:--|:--|
|←→↑↓|Scroll|
|A|Zoom In|
|B|Zoom Out|
|Hold X + ↑|Increse Iteration Count|
|Hold X + ↓|Decrese Iteration Count|
|Hold X + ←|Change Palette|
|Hold X + →|Shift Palette|
|X|Open/Close Menu|
|Y|Quit|

## How to Build (Linux/WSL2)

```sh
git clone https://github.com/Panda381/PicoLibSDK.git
export PICOLIBSDK_PATH=${PWD}/PicoLibSDK
git clone https://github.com/shapoco/fixbrot.git
cd fixbrot/app/picopad
make DEVICE=picopad20 all
```

binary generated in `fixbrot/app/picopad/bin/`.
