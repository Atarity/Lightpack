#
# lightpack/Software/build-deb.sh
#
#     Author: brunql
# Created on: 25.12.11
#

if [ -e "deb/usr/lib/lightpack/Lightpack" ];
then
	echo "Renaming 'deb/usr/lib/lightpack/Lightpack' to 'deb/usr/lib/lightpack/lightpack'."
	mv deb/usr/lib/lightpack/Lightpack deb/usr/lib/lightpack/lightpack
fi


if [ ! -e "deb/usr/lib/lightpack" ];
then
	echo "File 'deb/usr/lib/lightpack' not found."
	exit 1
fi

if [ ! -e "deb/usr/lib/lightpack/libPythonQt.so.1" ];
then
	echo "File 'deb/usr/lib/lightpack/libPythonQt.so.1' not found."
	exit 1
fi

if [ ! -e "deb/usr/lib/lightpack/libPythonQt_QtAll.so.1" ];
then
	echo "File 'deb/usr/lib/lightpack/libPythonQt_QtAll.so.1' not found."
	exit 1
fi



if [ -x "`which md5deep 2>/dev/null`" ]; 
then
	# Update MD5 sums
	md5deep -r deb/usr > deb/DEBIAN/md5sums
else
	echo "Please install 'md5deep' package first."
	exit 2
fi

# Build package in format "name_version_arch.deb"
fakeroot dpkg-deb --build deb/ .
