#!/bin/bash
# Builds the Mac version of MovieMetaM: release build (universal: arm64 + x86_64), Qt frameworks
# into the bundle (macdeployqt), ad hoc signature (no Developer ID: on the first start the user
# allows the app via right click -> Open, or in System Settings -> Privacy & Security) and a DMG
# with the app and a link to "Applications".
#
# Call from anywhere:  tools/build-mac.sh
# Qt elsewhere:        QT_DIR=/path/to/Qt/6.x/macos tools/build-mac.sh
# Result:              build/MovieMetaM-<version>.dmg

set -euo pipefail

APP_NAME="MovieMetaM"
SOURCE_DIR="$(cd "$(dirname "$0")/.." && pwd)"
QT_DIR="${QT_DIR:-$HOME/Qt/6.12.0/macos}"
BUILD_DIR="$SOURCE_DIR/build/release"
DMG_STAGING="$SOURCE_DIR/build/dmg"

# CMake and Ninja of the Qt installation, if they are not in the PATH
export PATH="$HOME/Qt/Tools/CMake/CMake.app/Contents/bin:$HOME/Qt/Tools/Ninja:$PATH"

VERSION="$(sed -n 's/^set(PROJECT_VERSION "\(.*\)")/\1/p' "$SOURCE_DIR/CMakeLists.txt")"
APP_BUNDLE="$BUILD_DIR/$APP_NAME.app"
DMG_FILE="$SOURCE_DIR/build/$APP_NAME-$VERSION.dmg"

if [ ! -f "$SOURCE_DIR/tmdb_token.txt" ]; then
    echo "Warning: tmdb_token.txt is missing – the movie search of this build will not work." >&2
fi

echo "== 1. Release build $VERSION (arm64 + x86_64)"
cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QT_DIR" \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build "$BUILD_DIR"

echo "== 2. Qt frameworks and plugins into the bundle"
"$QT_DIR/bin/macdeployqt" "$APP_BUNDLE" -always-overwrite

echo "== 3. Ad hoc signature"
codesign --force --deep --sign - "$APP_BUNDLE"
codesign --verify --deep --strict "$APP_BUNDLE"

echo "== 4. DMG"
rm -rf "$DMG_STAGING" "$DMG_FILE"
mkdir -p "$DMG_STAGING"
cp -R "$APP_BUNDLE" "$DMG_STAGING/"
ln -s /Applications "$DMG_STAGING/Applications"
hdiutil create -volname "$APP_NAME $VERSION" -srcfolder "$DMG_STAGING" -ov -format UDZO "$DMG_FILE"
rm -rf "$DMG_STAGING"

echo
echo "Done: $DMG_FILE"
lipo -info "$APP_BUNDLE/Contents/MacOS/$APP_NAME"
