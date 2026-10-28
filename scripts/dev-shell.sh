#!/usr/bin/env bash
# MS3DMaker Linux developer shell — loads Qt + OCCT into the environment.
# Usage:
#   source scripts/dev-shell.sh
#   source scripts/dev-shell.sh && cmake --version
#
# Optional: MS3D_DEV_ROOT (default: $HOME/Dev)

_MS3D_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
_DEV_ROOT="${MS3D_DEV_ROOT:-${HOME}/Dev}"

if [[ -f "${_DEV_ROOT}/ms3d-env.sh" ]]; then
  # shellcheck source=/dev/null
  source "${_DEV_ROOT}/ms3d-env.sh"
else
  export MS3D_DEV_ROOT="${_DEV_ROOT}"
  if [[ -z "${QT_ROOT:-}" ]]; then
    for _c in \
      "${_DEV_ROOT}/Qt/6.8.3/gcc_64" \
      "${_DEV_ROOT}/Qt/6.8.3/linux_gcc_64"; do
      if [[ -x "${_c}/bin/qmake" || -x "${_c}/bin/qmake6" ]]; then
        export QT_ROOT="${_c}"
        break
      fi
    done
  fi
  if [[ -z "${CASROOT:-}" ]]; then
    if [[ -f "${_DEV_ROOT}/OCCT/CASROOT.txt" ]]; then
      CASROOT="$(tr -d '\r\n' < "${_DEV_ROOT}/OCCT/CASROOT.txt")"
      export CASROOT
    elif [[ -d "${_DEV_ROOT}/OCCT/install" ]]; then
      export CASROOT="${_DEV_ROOT}/OCCT/install"
    fi
  fi
  if [[ -n "${QT_ROOT:-}" ]]; then
    export CMAKE_PREFIX_PATH="${QT_ROOT}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
    export PATH="${QT_ROOT}/bin:${PATH}"
    export LD_LIBRARY_PATH="${QT_ROOT}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
  fi
  if [[ -n "${CASROOT:-}" ]]; then
    export PATH="${CASROOT}/bin:${PATH}"
    export LD_LIBRARY_PATH="${CASROOT}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
  fi
fi

echo "MS3DMaker Linux dev shell"
echo "  repo:    ${_MS3D_ROOT}"
[[ -n "${QT_ROOT:-}" ]] && echo "  QT_ROOT: ${QT_ROOT}"
[[ -n "${CASROOT:-}" ]] && echo "  CASROOT: ${CASROOT}"

unset _MS3D_ROOT _DEV_ROOT _c
