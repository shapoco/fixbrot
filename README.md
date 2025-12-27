# FixBrot

Mandelbrot Viewer for [PicoPad](https://github.com/pajenicko/picopad) / [PicoSystem](https://shop.pimoroni.com/products/picosystem) / [M5Stack CoreS3](https://docs.m5stack.com/en/core/CoreS3) with Border Tracing Algorithm.

https://github.com/user-attachments/assets/eaa0953f-67d0-4af1-865a-db988a986c22

## Binary Download

See [Releases](https://github.com/shapoco/fixbrot/releases).

## Hou to Use

### PicoPad/PicoSystem

|PicoPad|PicoSystem|Funcion|
|:--|:--|:--|
|←→↑↓|←→↑↓|Scroll|
|A|B|Zoom in|
|B|A|Zoom out|
|Hold X + ↑|Hold Y + ↑|Increse iteration count|
|Hold X + ↓|Hold Y + ↓|Decrese iteration count|
|Hold X + ←|Hold Y + ←|Change palette|
|Hold X + →|Hold Y + →|Shift palette|
|X|Y|Open/Close menu|
|Y|-|Quit|

### M5Stack CoreS3

|Operation|Funcion|
|:--|:--|
|Swipe screen|Scroll|
|Tap screen|Zoom in|
|Double-Tap screen|Zoom in|
|Bottom center button|Open/Close menu|
|Tap menu item|Select option|
|Bottom left/right button|Change option value|

## How to Build

### PicoPad (Linux/WSL2)

```sh
git clone https://github.com/Panda381/PicoLibSDK.git
export PICOLIBSDK_PATH=${PWD}/PicoLibSDK
git clone https://github.com/shapoco/fixbrot.git
cd fixbrot/app/picopad
make DEVICE=picopad10 clean all
make DEVICE=picopad20 clean all
```

binary generated in `bin/picopad10/` or `bin/picopad20/`.

### PicoSystem (Linux/WSL2)

1. Setup [Pico SDK](https://github.com/raspberrypi/pico-sdk) and [PicoSystem SDK](https://github.com/pimoroni/picosystem).
2. Set `PICO_SDK_PATH` and `PICOSYSTEM_DIR` environment variable.
3. Checkout or download this repository and run `make` in `app/picosystem`

    ```sh
    git clone https://github.com/shapoco/fixbrot.git
    cd fixbrot/app/picosystem
    make
    ```

binary generated in `bin/picosystem/`.

### M5Stack CoreS3 (Arduino IDE)

1. Install [Arduino IDE and Board/Library for M5Stack](https://docs.m5stack.com/en/arduino/arduino_ide).
    - Fixbrot requires `M5Unified` library.
2. Checkout or download this repository.
3. Open the Arduino sketch in `app/m5/Fixbrot/` with Arduino IDE.
4. Connect your M5Stack CoreS3 to the PC, and specify board.
5. Open `Tools` in menu bar, and click `PSRAM: ...` --&gt; `Disabled` (to improve performance).
6. Upload the sketch to the CoreS3.
