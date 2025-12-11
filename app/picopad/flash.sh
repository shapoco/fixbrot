#!/bin/bash

if [ -z "${PICO_DRIVE}" ]; then
  PICO_DRIVE=e
fi

BIN=./bin/picopad20/FIXBROT.uf2

set -eux

sudo mkdir -p /mnt/${PICO_DRIVE}
sudo mount -t drvfs ${PICO_DRIVE}: /mnt/${PICO_DRIVE}
cp "${BIN}" /mnt/${PICO_DRIVE}/.
