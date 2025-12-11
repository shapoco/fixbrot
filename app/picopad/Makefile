.PHONY: all build flash clean

SRC_DIR := $(shell pwd)

GRPDIR := DEMO
TARGET := FIXBROT
DEVICE := picopad20

PICO_DRIVE := e
BIN_DIR := $(SRC_DIR)/bin/$(DEVICE)
BIN := $(BIN_DIR)/$(TARGET).uf2

WORK_DIR := $(PICOLIBSDK_PATH)/PicoPad/$(GRPDIR)/$(TARGET)

all: build

build:
	./build.sh -d "$(DEVICE)" -o "$(BIN_DIR)"

flash:
	sudo mkdir -p /mnt/$(PICO_DRIVE)
	sudo mount -t drvfs $(PICO_DRIVE): /mnt/$(PICO_DRIVE)
	cp "$(BIN)" /mnt/$(PICO_DRIVE)/.

clean:
	rm -rf "$(BIN_DIR)"
	cd $(WORK_DIR) && ./d.sh || true
