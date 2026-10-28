#!/usr/bin/env bash
# Build and run Phase 0 smoke tests on Linux (or any Unix with bash).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=/dev/null
source "${ROOT}/scripts/dev-shell.sh"

[[ -n "${QT_ROOT:-}" ]] || { echo "QT_ROOT not set — run setup-linux.sh or set MS3D_DEV_ROOT"; exit 1; }
[[ -n "${CASROOT:-}" ]] || { echo "CASROOT not set — build/install OCCT first"; exit 1; }

export CMAKE_PREFIX_PATH="${QT_ROOT}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
export PATH="${QT_ROOT}/bin:${CASROOT}/bin:${PATH}"
export LD_LIBRARY_PATH="${CASROOT}/lib:${QT_ROOT}/lib:${LD_LIBRARY_PATH:-}"

# Prefer OCCT CMake package dir
OPENCASCADE_DIR=""
for d in \
  "${CASROOT}/cmake" \
  "${CASROOT}/lib/cmake/opencascade" \
  "${CASROOT}/lib/cmake/OpenCASCADE"; do
  if [[ -f "${d}/OpenCASCADEConfig.cmake" ]]; then
    OPENCASCADE_DIR="$d"
    break
  fi
done
[[ -n "$OPENCASCADE_DIR" ]] || { echo "OpenCASCADEConfig.cmake not found under ${CASROOT}"; exit 1; }

LOG_DIR="${ROOT}/docs/development/smoke-logs"
mkdir -p "$LOG_DIR"

run_one() {
  local name="$1"
  local dir="${ROOT}/examples/smoke/${name}"
  local build="${dir}/build-linux"
  echo "=== Building ${name} ==="
  cmake -S "$dir" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DOpenCASCADE_DIR="${OPENCASCADE_DIR}"
  cmake --build "$build"
  echo "=== Running ${name} ==="
  # Qt GUI smokes auto-quit; allow headless with offscreen if no DISPLAY
  if [[ -z "${DISPLAY:-}" && "$name" != "occt_box" ]]; then
    export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
  fi
  "${build}/${name}" >"${LOG_DIR}/${name}.linux.out.txt" 2>"${LOG_DIR}/${name}.linux.err.txt"
  echo "${name} OK"
}

run_one qt_hello
run_one occt_box
run_one qt_occt
echo "All smoke tests passed."
