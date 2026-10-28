#!/usr/bin/env bash
# Build/install OCCT for CI. Honors CASROOT / MS3D_DEV_ROOT. Skips rebuild if install exists.
set -euo pipefail

DEV_ROOT="${MS3D_DEV_ROOT:-${HOME}/Dev}"
OCCT_TAG="${MS3D_OCCT_TAG:-V7_9_3}"
SRC="${DEV_ROOT}/OCCT/src"
BUILD="${DEV_ROOT}/OCCT/build"
PREFIX="${DEV_ROOT}/OCCT/install"

mkdir -p "${DEV_ROOT}/OCCT"

if [[ -f "${PREFIX}/lib/cmake/opencascade/OpenCASCADEConfig.cmake" ]] \
  || [[ -f "${PREFIX}/cmake/OpenCASCADEConfig.cmake" ]]; then
  echo "OCCT already installed at ${PREFIX}"
  echo "${PREFIX}" > "${DEV_ROOT}/OCCT/CASROOT.txt"
  exit 0
fi

if [[ ! -d "${SRC}/.git" ]]; then
  git clone --depth 1 --branch "${OCCT_TAG}" https://github.com/Open-Cascade-SAS/OCCT.git "${SRC}"
fi

cmake -S "${SRC}" -B "${BUILD}" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
  -DBUILD_LIBRARY_TYPE=Shared \
  -DBUILD_MODULE_Draw=OFF \
  -DBUILD_DOC_Overview=OFF \
  -DUSE_FREETYPE=OFF \
  -DUSE_FREEIMAGE=OFF \
  -DUSE_RAPIDJSON=OFF \
  -DUSE_TCL=OFF \
  -DUSE_TK=OFF \
  -DUSE_VTK=OFF \
  -DUSE_TBB=OFF \
  -DUSE_OPENGL=ON \
  -DUSE_GLES2=OFF

cmake --build "${BUILD}" --parallel
cmake --install "${BUILD}"
echo "${PREFIX}" > "${DEV_ROOT}/OCCT/CASROOT.txt"
echo "OCCT installed: ${PREFIX}"
