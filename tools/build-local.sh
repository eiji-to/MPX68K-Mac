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
    # File Provider can re-add FinderInfo inside Documents immediately after
    # xattr -c. Sign outside synced folders, then copy the sealed artifact.
    STAGING_DIR="$(mktemp -d "${TMPDIR:-/tmp}/mpx68k-package.XXXXXX")"
    trap 'rm -rf "$STAGING_DIR"' EXIT
    STAGED_APP="$STAGING_DIR/MPX68K.app"
    ditto --norsrc --noextattr "$APP" "$STAGED_APP"
    LICENSE_DIR="$STAGED_APP/Contents/Resources/ThirdPartyLicenses"
    mkdir -p "$LICENSE_DIR"
    cp LICENSE ATTRIBUTION.md README.ja.md BUILD-NOTES.md "$LICENSE_DIR/"
    ditto --norsrc --noextattr LICENSES "$LICENSE_DIR/LICENSES"
    xattr -cr "$STAGED_APP"
    codesign --force --deep --sign - "$STAGED_APP"
    codesign --verify --deep --strict "$STAGED_APP"
    ditto -c -k --sequesterRsrc --keepParent "$STAGED_APP" build/dist/MPX68K-AppleSilicon.zip
    ditto --norsrc --noextattr "$STAGED_APP" build/dist/MPX68K.app
    # Preserve File Provider metadata; remove only signature-incompatible Finder data.
    xattr -dr com.apple.FinderInfo build/dist/MPX68K.app
    xattr -dr com.apple.ResourceFork build/dist/MPX68K.app
    codesign --verify --deep --strict build/dist/MPX68K.app
    echo "$PROJECT_DIR/build/dist/MPX68K.app"
fi
