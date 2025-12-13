#!/bin/bash

set -e

ARG_DEVICE="picopad20"
ARG_OUTDIR="bin"
while getopts d:o: opt; do
  case ${opt} in
    d)
      ARG_DEVICE="${OPTARG}"
      ;;
    o)
      ARG_OUTDIR="${OPTARG}"
      ;;
    *)
      echo "Usage: $0 [-d device]"
      exit 1
      ;;
  esac
done

set -u

function pushdir() {
  pushd "${1}" > /dev/null
}

function popdir() {
  popd > /dev/null
}

APP_DIR=$(pwd)
REPO_DIR=$(cd ../.. && pwd)
GRPDIR=PROG
TARGET=FIXBROT

SRC_TARGET_DIR="${APP_DIR}/${GRPDIR}/${TARGET}"
LIB_DIR="${REPO_DIR}/lib"

DEST_BIN_DIR="${APP_DIR}/bin/${ARG_DEVICE}"

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
    touch src/*.cpp
    ./c.sh "${ARG_DEVICE}"
    mkdir -p "${ARG_OUTDIR}"
    cp *.hex *.bin *.lst *.sym *.siz *.uf2 "${ARG_OUTDIR}/."
  popdir
popdir

