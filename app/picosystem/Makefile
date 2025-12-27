.PHONY: all build clean flash

REPO_DIR := $(shell cd ../.. && pwd)

APP_NAME := fixbrot
APP_BUILD_DIR := build
APP_SRC_DIR := src
APP_BIN_DIR := $(REPO_DIR)/bin/picosystem
APP_BIN := $(APP_BIN_DIR)/$(APP_NAME).uf2

PICO_DRIVE := e

LIBFIXBROT_DIR := ../../lib

DEPENDENCIES := \
	Makefile \
	CMakeLists.txt \
	$(wildcard $(APP_SRC_DIR)/*.cpp) \
	$(wildcard $(LIBFIXBROT_DIR)/include/*.hpp)

all: build

build: $(APP_BIN)

$(APP_BIN): $(DEPENDENCIES) $(APP_BUILD_DIR)
	@mkdir -p $(APP_BUILD_DIR)
	cd $(APP_BUILD_DIR) && cmake ../ && make -j
	@mkdir -p $(APP_BIN_DIR)
	cp $(APP_BUILD_DIR)/$(APP_NAME).uf2 $(APP_BIN_DIR)/

clean:
	rm -f $(APP_BIN)
	rm -rf $(APP_BUILD_DIR)

flash: $(APP_BIN)
	sudo mkdir -p /mnt/$(PICO_DRIVE)
	sudo mount -t drvfs $(PICO_DRIVE): /mnt/$(PICO_DRIVE)
	cp $(APP_BIN) /mnt/$(PICO_DRIVE)/.
