#!/bin/sh

test -z "$1" && test -z "$2" && echo "usage: $0 <version> <identity>" && exit 1

IDENTITY=$2
VOLUME_NAME="Prismatik $1"
DMG_TEMP_NAME="temp.dmg"
DMG_NAME="Prismatik.$1.dmg"
SRC_FOLDER="contents"
LAYOUT_FILE="dsstore"
APP_PATH="$SRC_FOLDER/Prismatik.app/Contents/MacOS"
QT_PATH=~/Qt/5.2.1/clang_64

if [ ! -L "$SRC_FOLDER/Applications" ]; then
	echo "Creating link to /Applications folder"
	ln -s /Applications $SRC_FOLDER/Applications
fi

if [ ! -d "$SRC_FOLDER/Prismatik.app" ]; then
	echo "please copy Prismatik.app to $SRC_FOLDER directory"
	exit 1
fi

echo "collecting dependencies..."
$QT_PATH/bin/macdeployqt $SRC_FOLDER/Prismatik.app

echo "signing application..."

FRAMEWORKS=$SRC_FOLDER/Prismatik.app/Contents/Frameworks/*
for f in $FRAMEWORKS
do
    FW=$(basename "$f")
    echo "copying Info.plist for $FW"
    cp $QT_PATH/lib/$FW/Contents/Info.plist $f/Resources
done

#FRAMEWORKS=$SRC_FOLDER/Prismatik.app/Contents/Frameworks/*
#for f in $FRAMEWORKS
#do
#    for ff in "$f/Versions/5/*"
#    do
#    	if codesign --verify --verbose -s "$IDENTITY" $ff ; then
#    		echo "\t done"
#    	else
#    		exit 1
#    	fi
#    done 
#done
#
#PLUGINS=$SRC_FOLDER/Prismatik.app/Contents/PlugIns/*
#for f in $PLUGINS
#do
#    for ff in "$f/*"
#    do
#    	if codesign --verify --verbose -s "$IDENTITY" $ff ; then
#    	    echo "\t done"
#    	else
#    	    exit 1
#    	fi
#    done 
#done

codesign -vvv --deep -s "$IDENTITY" $SRC_FOLDER/Prismatik.app || exit 1
codesign -vvv --verify $SRC_FOLDER/Prismatik.app || exit 1

# Create the image
echo "Creating disk image..."
test -f "${DMG_TEMP_NAME}" && rm -f "${DMG_TEMP_NAME}"
ACTUAL_SIZE=`du -sm "$SRC_FOLDER" | sed -e 's/	.*//g'`
DISK_IMAGE_SIZE=$(expr $ACTUAL_SIZE + 10)
hdiutil create -srcfolder "$SRC_FOLDER" -volname "${VOLUME_NAME}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${DISK_IMAGE_SIZE}m "${DMG_TEMP_NAME}"

# mount it
echo "Mounting disk image..."
MOUNT_DIR="/Volumes/${VOLUME_NAME}"
echo "Mount directory: $MOUNT_DIR"
DEV_NAME=$(hdiutil attach -readwrite -noverify -noautoopen "${DMG_TEMP_NAME}" | egrep '^/dev/' | sed 1q | awk '{print $1}')
echo "Device name:     $DEV_NAME"

if ! test -z "$LAYOUT_FILE"; then
  echo "Copying layout file '$LAYOUT_FILE'..."
  cp -f "$LAYOUT_FILE" "$MOUNT_DIR/.DS_Store"
fi

# make sure it's not world writeable
echo "Fixing permissions..."
chmod -Rf go-w "${MOUNT_DIR}" &> /dev/null || true
echo "Done fixing permissions."

# make the top window open itself on mount:
echo "Blessing started"
bless --folder "${MOUNT_DIR}" --openfolder "${MOUNT_DIR}"
echo "Blessing finished"

if ! test -z "$VOLUME_ICON_FILE"; then
   # tell the volume that it has a special file attribute
   SetFile -a C "$MOUNT_DIR"
fi

# unmount
echo "Unmounting disk image..."
hdiutil detach "${DEV_NAME}"

test -f "${DMG_NAME}" && rm -f "${DMG_NAME}"

# compress image
echo "Compressing disk image..."
hdiutil convert "${DMG_TEMP_NAME}" -format UDZO -imagekey zlib-level=9 -o "${DMG_NAME}"
rm -f "${DMG_TEMP_NAME}"

echo "Disk image done"
exit 0
