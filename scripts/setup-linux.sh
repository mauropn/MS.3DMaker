#!/usr/bin/env bash
# MS3DMaker — Linux dependency installer (Ubuntu/Debian x86_64)
#
# Lessons from the verified Windows Phase 0 install:
# - Prefer official/pinned versions: Qt 6.8.3, OCCT V7_9_3, CMake 3.28+/Ninja
# - aqtinstall can fail (mirrors/SSL/nested Qt repo layout) → manual download.qt.io fallback
# - Build OCCT on a fast local disk (SSD); HDD header collect is extremely slow
# - Do not reuse FreeCAD/KiCad bundled OCCT DLLs (no headers)
# - REA needs Node ^22.23.4 || ^24.22.1 || >=26.12.1
# - Smoke-test Qt, OCCT, and Qt+OCCT before calling the environment "ready"
#
# Usage:
#   chmod +x scripts/setup-linux.sh
#   ./scripts/setup-linux.sh              # full install (asks sudo for apt)
#   ./scripts/setup-linux.sh --smoke-only
#   ./scripts/setup-linux.sh --skip-occt
#   MS3D_DEV_ROOT=/data/ms3d ./scripts/setup-linux.sh
#
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEV_ROOT="${MS3D_DEV_ROOT:-${HOME}/Dev}"
QT_VERSION="${MS3D_QT_VERSION:-6.8.3}"
QT_ARCH="${MS3D_QT_ARCH:-linux_gcc_64}"
OCCT_TAG="${MS3D_OCCT_TAG:-V7_9_3}"
NODE_MAJOR="${MS3D_NODE_MAJOR:-24}"

DO_APT=1
DO_NODE=1
DO_GH=1
DO_QT=1
DO_OCCT=1
DO_SMOKE=0
SMOKE_ONLY=0

log()  { printf '\n==> %s\n' "$*"; }
warn() { printf 'WARN: %s\n' "$*" >&2; }
die()  { printf 'ERROR: %s\n' "$*" >&2; exit 1; }

usage() {
  sed -n '2,20p' "$0"
  exit 0
}

for arg in "$@"; do
  case "$arg" in
    --help|-h) usage ;;
    --skip-apt) DO_APT=0 ;;
    --skip-node) DO_NODE=0 ;;
    --skip-gh) DO_GH=0 ;;
    --skip-qt) DO_QT=0 ;;
    --skip-occt) DO_OCCT=0 ;;
    --smoke) DO_SMOKE=1 ;;
    --smoke-only) SMOKE_ONLY=1; DO_APT=0; DO_NODE=0; DO_GH=0; DO_QT=0; DO_OCCT=0; DO_SMOKE=1 ;;
    *) die "Unknown argument: $arg (try --help)" ;;
  esac
done

need_cmd() { command -v "$1" >/dev/null 2>&1 || die "Missing required command: $1"; }

detect_os() {
  if [[ -f /etc/os-release ]]; then
    # shellcheck source=/dev/null
    . /etc/os-release
    echo "${ID:-unknown}"
  else
    echo unknown
  fi
}

install_apt() {
  log "Installing apt packages (build tools, cmake, ninja, git, python, OpenGL deps)"
  local os_id
  os_id="$(detect_os)"
  [[ "$os_id" == "ubuntu" || "$os_id" == "debian" || "$os_id" == "linuxmint" || "$os_id" == "pop" ]] \
    || warn "OS id=$os_id — script targets Ubuntu/Debian; continuing anyway"

  sudo apt-get update
  sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    git-lfs \
    curl \
    ca-certificates \
    gnupg \
    python3 \
    python3-pip \
    python3-venv \
    pkg-config \
    libgl1-mesa-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxcb-cursor0 \
    libxcb-xinerama0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-render-util0 \
    libdbus-1-dev \
    libfontconfig1-dev \
    libfreetype-dev \
    wget \
    p7zip-full \
    xz-utils
  git lfs install --skip-repo || true
}

install_node() {
  log "Ensuring Node.js ${NODE_MAJOR}.x (>=24.22.1 or compatible for REA)"
  node_ok() {
    python3 - "$1" <<'PY'
import sys
parts = [int(p) for p in sys.argv[1].split(".")[:3]]
while len(parts) < 3:
    parts.append(0)
maj, minor, patch = parts
ok = (
    (maj == 22 and (minor, patch) >= (23, 4))
    or (maj == 24 and (minor, patch) >= (22, 1))
    or (maj > 26)
    or (maj == 26 and (minor, patch) >= (12, 1))
)
sys.exit(0 if ok else 1)
PY
  }

  if command -v node >/dev/null 2>&1; then
    local ver
    ver="$(node -v | sed 's/^v//')"
    if node_ok "$ver"; then
      log "Node $(node -v) already satisfies REA engines"
      return 0
    fi
    warn "Node $(node -v) is too old for REA; installing NodeSource ${NODE_MAJOR}.x"
  fi

  curl -fsSL "https://deb.nodesource.com/setup_${NODE_MAJOR}.x" | sudo -E bash -
  sudo DEBIAN_FRONTEND=noninteractive apt-get install -y nodejs
  node -v
  npm -v
}

install_gh() {
  if command -v gh >/dev/null 2>&1; then
    log "GitHub CLI already present: $(gh --version | head -n1)"
    return 0
  fi
  log "Installing GitHub CLI"
  (type -p wget >/dev/null || sudo apt-get install -y wget)
  sudo mkdir -p -m 755 /etc/apt/keyrings
  wget -qO- https://cli.github.com/packages/githubcli-archive-keyring.gpg \
    | sudo tee /etc/apt/keyrings/githubcli-archive-keyring.gpg >/dev/null
  sudo chmod go+r /etc/apt/keyrings/githubcli-archive-keyring.gpg
  echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" \
    | sudo tee /etc/apt/sources.list.d/github-cli.list >/dev/null
  sudo apt-get update
  sudo DEBIAN_FRONTEND=noninteractive apt-get install -y gh
  gh --version | head -n1
}

install_qt_aqt() {
  log "Trying Qt ${QT_VERSION} via aqtinstall → ${DEV_ROOT}/Qt"
  mkdir -p "${DEV_ROOT}/Qt"
  python3 -m pip install --user --upgrade aqtinstall py7zr
  export PATH="${HOME}/.local/bin:${PATH}"

  local cfg="${ROOT}/scripts/aqt-settings.ini"
  local aqt=(python3 -m aqt)
  if [[ -f "$cfg" ]]; then
    aqt+=(-c "$cfg")
  fi

  # Prefer download.qt.io; longer timeout; Linux GCC kit
  if "${aqt[@]}" install-qt linux desktop "${QT_VERSION}" "${QT_ARCH}" \
      -O "${DEV_ROOT}/Qt" \
      -b https://download.qt.io \
      --timeout 120 \
      -m qttools; then
    return 0
  fi
  warn "aqtinstall failed (common: mirrors/SSL/repo layout) — falling back to manual download"
  return 1
}

install_qt_manual() {
  # Mirror the Windows nested online-repo layout discovered in Phase 0.
  # Package dir naming varies slightly by Qt revision; probe known candidates.
  log "Manual Qt ${QT_VERSION} download from download.qt.io"
  need_cmd curl
  need_cmd python3

  python3 -m pip install --user --upgrade py7zr
  export PATH="${HOME}/.local/bin:${PATH}"

  local ver_prefix="${QT_VERSION}-0-"
  local base_root="https://download.qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_683/qt6_683"
  local pkg_dirs=(
    "qt.qt6.683.gcc_64"
    "qt.qt6.683.linux_gcc_64"
  )
  local pkg_dir=""
  local d
  for d in "${pkg_dirs[@]}"; do
    if curl -fsI "${base_root}/${d}/" >/dev/null 2>&1; then
      pkg_dir="$d"
      break
    fi
  done
  [[ -n "$pkg_dir" ]] || die "Could not find Qt ${QT_VERSION} linux package directory under ${base_root}"

  # Discover exact version stamp + archive names from directory listing
  local listing
  listing="$(curl -fsSL "${base_root}/${pkg_dir}/")"
  local stamp
  stamp="$(printf '%s\n' "$listing" | grep -oE "${QT_VERSION}-0-[0-9]+qtbase[^\"<]*\.7z" | head -n1 | sed -E "s/(${QT_VERSION}-0-[0-9]+)qtbase.*/\1/")"
  [[ -n "$stamp" ]] || die "Could not parse Qt version stamp from ${base_root}/${pkg_dir}/"

  local archive_dir="${DEV_ROOT}/Qt/_archives"
  local extract_root="${DEV_ROOT}/Qt/${QT_VERSION}/gcc_64"
  mkdir -p "$archive_dir" "$extract_root"

  local names=(qtbase qttools qtsvg)
  local n file url dest
  for n in "${names[@]}"; do
    file="$(printf '%s\n' "$listing" | grep -oE "${stamp}${n}-[^\"]+\.7z" | head -n1)"
    [[ -n "$file" ]] || die "Missing archive for module ${n} under ${pkg_dir}"
    dest="${archive_dir}/${file}"
    url="${base_root}/${pkg_dir}/${file}"
    if [[ -f "$dest" && $(stat -c%s "$dest") -gt 1000 ]]; then
      log "SKIP download (exists): ${file}"
    else
      log "DOWNLOAD ${url}"
      curl -fL --retry 5 --retry-delay 2 -o "$dest" "$url"
    fi
  done

  python3 - <<PY
from pathlib import Path
import py7zr
archive_dir = Path(r"${archive_dir}")
extract_root = Path(r"${extract_root}")
extract_root.mkdir(parents=True, exist_ok=True)
for z in sorted(archive_dir.glob("${stamp}*.7z")):
    if not any(m in z.name for m in ("qtbase", "qttools", "qtsvg")):
        continue
    print("EXTRACT", z.name)
    with py7zr.SevenZipFile(z, mode="r") as archive:
        archive.extractall(path=extract_root)
print("DONE")
PY

  [[ -x "${extract_root}/bin/qmake" || -x "${extract_root}/bin/qmake6" ]] \
    || die "qmake not found after Qt extract in ${extract_root}/bin"
}

resolve_qt_root() {
  local candidates=(
    "${DEV_ROOT}/Qt/${QT_VERSION}/gcc_64"
    "${DEV_ROOT}/Qt/${QT_VERSION}/linux_gcc_64"
  )
  local c
  for c in "${candidates[@]}"; do
    if [[ -x "${c}/bin/qmake" || -x "${c}/bin/qmake6" ]]; then
      echo "$c"
      return 0
    fi
  done
  # Fallback: newest qmake under DEV_ROOT/Qt
  local found
  found="$(find "${DEV_ROOT}/Qt" -type f \( -name qmake -o -name qmake6 \) 2>/dev/null | head -n1 || true)"
  if [[ -n "$found" ]]; then
    dirname "$(dirname "$found")"
    return 0
  fi
  return 1
}

install_qt() {
  if resolve_qt_root >/dev/null 2>&1; then
    log "Qt already present at $(resolve_qt_root)"
    return 0
  fi
  if ! install_qt_aqt; then
    install_qt_manual
  fi
  local qt_root
  qt_root="$(resolve_qt_root)" || die "Qt install finished but kit not found under ${DEV_ROOT}/Qt"
  log "Qt OK: ${qt_root} ($("${qt_root}/bin/qmake" -query QT_VERSION 2>/dev/null || "${qt_root}/bin/qmake6" -query QT_VERSION))"
  [[ -x "${qt_root}/bin/lupdate" && -x "${qt_root}/bin/lrelease" ]] \
    || warn "lupdate/lrelease missing — install qttools module"
}

install_occt() {
  local src="${DEV_ROOT}/OCCT/src"
  local build="${DEV_ROOT}/OCCT/build"
  local install_prefix="${DEV_ROOT}/OCCT/install"

  log "OCCT ${OCCT_TAG} → build/install under ${DEV_ROOT}/OCCT (use SSD/local disk)"
  mkdir -p "${DEV_ROOT}/OCCT"

  if [[ ! -d "${src}/.git" ]]; then
    git clone --depth 1 --branch "${OCCT_TAG}" https://github.com/Open-Cascade-SAS/OCCT.git "$src"
  else
    log "OCCT sources already present: ${src}"
  fi

  if [[ -f "${install_prefix}/lib/cmake/opencascade/OpenCASCADEConfig.cmake" ]] \
     || [[ -f "${install_prefix}/cmake/OpenCASCADEConfig.cmake" ]]; then
    log "OCCT already installed at ${install_prefix}"
    echo "$install_prefix" > "${DEV_ROOT}/OCCT/CASROOT.txt"
    return 0
  fi

  need_cmd cmake
  need_cmd ninja

  cmake -S "$src" -B "$build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${install_prefix}" \
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

  cmake --build "$build" --parallel
  cmake --install "$build"
  echo "$install_prefix" > "${DEV_ROOT}/OCCT/CASROOT.txt"
  log "OCCT installed: ${install_prefix}"
}

write_env_file() {
  local qt_root casroot
  qt_root="$(resolve_qt_root 2>/dev/null || true)"
  if [[ -f "${DEV_ROOT}/OCCT/CASROOT.txt" ]]; then
    casroot="$(tr -d '\r\n' < "${DEV_ROOT}/OCCT/CASROOT.txt")"
  else
    casroot="${DEV_ROOT}/OCCT/install"
  fi

  local envf="${DEV_ROOT}/ms3d-env.sh"
  cat > "$envf" <<EOF
# Generated by scripts/setup-linux.sh — source before building MS3DMaker
export MS3D_DEV_ROOT="${DEV_ROOT}"
export QT_ROOT="${qt_root}"
export CASROOT="${casroot}"
export CMAKE_PREFIX_PATH="\${QT_ROOT}\${CMAKE_PREFIX_PATH:+:\${CMAKE_PREFIX_PATH}}"
export PATH="\${QT_ROOT}/bin:\${CASROOT}/bin:\${PATH}"
export LD_LIBRARY_PATH="\${CASROOT}/lib:\${QT_ROOT}/lib:\${LD_LIBRARY_PATH:-}"
EOF
  log "Wrote ${envf}"
  echo "  source ${envf}"
}

run_smokes() {
  log "Running smoke tests"
  need_cmd bash
  bash "${ROOT}/scripts/run_smokes.sh"
}

main() {
  log "MS3DMaker Linux setup"
  echo "  repo:     ${ROOT}"
  echo "  DEV_ROOT: ${DEV_ROOT}  (override with MS3D_DEV_ROOT)"
  mkdir -p "$DEV_ROOT"

  if [[ "$SMOKE_ONLY" -eq 0 ]]; then
    [[ "$DO_APT"  -eq 1 ]] && install_apt
    [[ "$DO_NODE" -eq 1 ]] && install_node
    [[ "$DO_GH"   -eq 1 ]] && install_gh
    [[ "$DO_QT"   -eq 1 ]] && install_qt
    [[ "$DO_OCCT" -eq 1 ]] && install_occt
    write_env_file
  fi

  if [[ "$DO_SMOKE" -eq 1 ]]; then
    # shellcheck source=/dev/null
    source "${DEV_ROOT}/ms3d-env.sh"
    run_smokes
  fi

  log "Done"
  cat <<EOF

Next:
  source ${DEV_ROOT}/ms3d-env.sh
  # or:  source ${ROOT}/scripts/dev-shell.sh
  ./scripts/run_smokes.sh

Verify:
  cmake --version && ninja --version && g++ --version
  qmake -query QT_VERSION   # or qmake6
  test -f "\$CASROOT/cmake/OpenCASCADEConfig.cmake" -o -f "\$CASROOT/lib/cmake/opencascade/OpenCASCADEConfig.cmake"
  node -v   # need ^22.23.4 || ^24.22.1 || >=26.12.1 for REA

Optional REA (after Node is OK):
  npx rea-agents@latest setup --client cursor

Docs: docs/development/setup-guide.md
EOF
}

main
