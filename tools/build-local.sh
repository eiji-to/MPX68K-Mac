#!/bin/bash
set -euo pipefail
PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_DIR"
CONFIGURATION="${1:-Release}"
case "$CONFIGURATION" in Debug|Release) ;; *) echo 'Use Debug or Release' >&2; exit 2 ;; esac
DERIVED_DATA="build/DerivedData"
if [[ "$CONFIGURATION" == Debug ]]; then DERIVED_DATA="build/DebugDerivedData"; fi
xcodebuild -project X68000.xcodeproj -scheme "X68000 macOS" \
    -configuration "$CONFIGURATION" -derivedDataPath "$DERIVED_DATA" \
    CODE_SIGNING_ALLOWED=NO DEVELOPMENT_TEAM= ARCHS=arm64 ONLY_ACTIVE_ARCH=YES build
APP="$DERIVED_DATA/Build/Products/$CONFIGURATION/MPX68K.app"
if [[ "$CONFIGURATION" == Release ]]; then
    mkdir -p build/dist
    ditto "$APP" build/dist/MPX68K.app
    LICENSE_DIR="build/dist/MPX68K.app/Contents/Resources/ThirdPartyLicenses"
    mkdir -p "$LICENSE_DIR"
    cp LICENSE ATTRIBUTION.md README.ja.md "$LICENSE_DIR/"
    ditto LICENSES "$LICENSE_DIR/LICENSES"
    xattr -cr build/dist/MPX68K.app
    codesign --force --deep --sign - build/dist/MPX68K.app
    codesign --verify --deep --strict build/dist/MPX68K.app
    ditto -c -k --sequesterRsrc --keepParent build/dist/MPX68K.app build/dist/MPX68K-AppleSilicon.zip
    echo "$PROJECT_DIR/build/dist/MPX68K.app"
fi
