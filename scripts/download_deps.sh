#!/usr/bin/env bash
# Download dependencies to dependencies/ (same layout as premake dependency.lua).
# Run from repo root: ./scripts/download_deps.sh

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RIVE_RUNTIME="$(cd "$SCRIPT_DIR/.." && pwd)"
DEPS="${DEPS:-$RIVE_RUNTIME/dependencies}"
mkdir -p "$DEPS"
cd "$DEPS"

clone() {
  local project="$1"
  local tag="$2"
  local dirname="${project//\//_}_${tag}"
  if [[ -d "$dirname" ]]; then
    echo "Already present: $dirname"
    return
  fi
  echo "Fetching $project @ $tag..."
  git clone --depth 1 --branch "$tag" "https://github.com/${project}.git" "$dirname"
}

# Core deps (required by rive)
clone "rive-app/harfbuzz" "rive_13.1.1"
clone "Tehreer/SheenBidi" "v2.6"
clone "rive-app/yoga" "rive_changes_v2_0_1_2"
clone "rive-app/miniaudio" "rive_changes_5"

# Decoders
clone "madler/zlib" "v1.3.1"
clone "glennrp/libpng" "libpng16"
clone "rive-app/libjpeg" "v9f"
clone "webmproject/libwebp" "v1.4.0"

# GLFW (for examples)
clone "glfw/glfw" "3.4"

echo "Dependencies ready in $DEPS"
