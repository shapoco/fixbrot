#!/bin/bash

set -eu

function pushdir() {
  pushd "${1}" > /dev/null
}

function popdir() {
  popd > /dev/null
}

APP_DIR=$(pwd)
REPO_DIR=$(cd ../.. && pwd)
GRPDIR=DEMO
TARGET=FIXBROT

set +u
if [ -z "${DEVICE_LIST}" ]; then
  DEVICE_LIST=(picopad10 picopad20)
fi
set -u

SRC_TARGET_DIR="${APP_DIR}/${GRPDIR}/${TARGET}"
LIB_DIR="${REPO_DIR}/lib"

for device in "${DEVICE_LIST[@]}"; do
  dest_bin_dir="${APP_DIR}/bin/${device}"

  pushdir "${PICOLIBSDK_PATH}/_tools"
    pushdir elf2uf2
      g++ -o elf2uf2 *.cpp
    popdir
    pushdir PicoPadLoaderCrc
      g++ -o LoaderCrc *.cpp
    popdir
  popdir

  pushdir "${PICOLIBSDK_PATH}/PicoPad"
    mkdir -p "./${GRPDIR}/${TARGET}/src"
    pushdir "./${GRPDIR}/${TARGET}"
      cp -rp ${LIB_DIR}/include/* "."
      cp -rp ${SRC_TARGET_DIR}/* "."
      #set +e
      #./d.sh
      #set -e
      #touch src/*.cpp
      ./c.sh "${device}"
      mkdir -p "${dest_bin_dir}"
      cp *.hex *.bin *.lst *.sym *.siz *.uf2 "${dest_bin_dir}/."
    popdir
  popdir

done
