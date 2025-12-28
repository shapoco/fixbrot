#!/bin/bash

set -e

ARG_DEVICE="picopad20"
ARG_OUTDIR="bin"
ARG_GRPDIR=""
ARG_TARGET=""
while getopts d:o:g:t: opt; do
  case ${opt} in
    d)
      ARG_DEVICE="${OPTARG}"
      ;;
    o)
      ARG_OUTDIR="${OPTARG}"
      ;;
    g)
      ARG_GRPDIR="${OPTARG}"
      ;;
    t)
      ARG_TARGET="${OPTARG}"
      ;;
    *)
      echo "Usage: $0 [-d device]"
      exit 1
      ;;
  esac
done

if [ -z "${ARG_GRPDIR}" ]; then
  echo "*Error: -g GRPDIR is required."
  exit 1
fi

if [ -z "${ARG_TARGET}" ]; then
  echo "*Error: -t TARGET is required."
  exit 1
fi

set -u

function pushdir() {
  pushd "${1}" > /dev/null
}

function popdir() {
  popd > /dev/null
}

APP_DIR=$(pwd)
REPO_DIR=$(cd ../.. && pwd)
SUBMODULES_DIR="${REPO_DIR}/submodules"
ADAFRUIT_GFX_DIR="${SUBMODULES_DIR}/Adafruit-GFX-Library"

SRC_TARGET_DIR="${APP_DIR}/${ARG_GRPDIR}/${ARG_TARGET}"
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
  mkdir -p "./${ARG_GRPDIR}/${ARG_TARGET}/src"
  pushdir "./${ARG_GRPDIR}/${ARG_TARGET}"
    cp -rp ${LIB_DIR}/include/* "."
    cp -rp ${SRC_TARGET_DIR}/* "."
    cp -p "${ADAFRUIT_GFX_DIR}/gfxfont.h" "."
    touch src/*.cpp
    ./c.sh "${ARG_DEVICE}"
    mkdir -p "${ARG_OUTDIR}"
    cp *.hex *.bin *.lst *.sym *.siz *.uf2 "${ARG_OUTDIR}/."
  popdir
popdir

