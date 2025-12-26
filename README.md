# FixBrot

Mandelbrot Viewer for [M5CoreS3](https://docs.m5stack.com/en/core/CoreS3)/[PicoPad](https://github.com/pajenicko/picopad) with Border Tracing Algorithm.

https://github.com/user-attachments/assets/04b31c8e-c4cc-4e2c-9aa6-9185a7eabbc5

## Hou to Use

### M5Stack CoreS3

|Operation|Funcion|
|:--|:--|
|Swipe screen|Scroll|
|Tap screen|Zoom in|
|Double-Tap screen|Zoom in|
|Bottom center button|Open/Close menu|
|Tap menu item|Select option|
|Bottom left/right button|Change option value|

### PicoPad

|Key|Funcion|
|:--|:--|
|←→↑↓|Scroll|
|A|Zoom in|
|B|Zoom out|
|Hold X + ↑|Increse iteration count|
|Hold X + ↓|Decrese iteration count|
|Hold X + ←|Change palette|
|Hold X + →|Shift palette|
|X|Open/Close menu|
|Y|Quit|

## How to Build

### M5Stack CoreS3 (Arduino IDE)

1. Install [Arduino IDE and Board/Library for M5Stack](https://docs.m5stack.com/en/arduino/arduino_ide).
    - Fixbrot requires `M5Unified` library.
2. Checkout or download this repository.
3. Open the Arduino sketch in `app/m5/Fixbrot/` with Arduino IDE.
4. Connect your M5Stack CoreS3 to the PC, and specify board.
5. Open `Tools` in menu bar, and click `PSRAM: ...` --&gt; `Disabled` (to improve performance).
6. Upload the sketch to the CoreS3.

### PicoPad (Linux/WSL2)

```sh
git clone https://github.com/Panda381/PicoLibSDK.git
export PICOLIBSDK_PATH=${PWD}/PicoLibSDK
git clone https://github.com/shapoco/fixbrot.git
cd fixbrot/app/picopad
make DEVICE=picopad20 all
```

binary generated in `fixbrot/app/picopad/bin/`.
