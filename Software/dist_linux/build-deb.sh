#
# lightpack/Software/build-deb.sh
#
#     Author: brunql
# Created on: 25.12.11
#


if [ -z $2 ];
then
    echo "usage: $0 <version> <arch>"
    exit 1
fi

perl prepare_deb.pl $1 $2 || exit 1;

chmod a+x deb/DEBIAN/control || exit 1;

if [ -e "deb/usr/lib/prismatik/Prismatik" ];
then
	echo "Renaming 'deb/usr/lib/prismatik/Prismatik' to 'deb/usr/lib/prismatik/prismatik'."
	mv deb/usr/lib/prismatik/Prismatik deb/usr/lib/prismatik/prismatik
fi


if [ ! -e "deb/usr/lib/prismatik" ];
then
	echo "File 'deb/usr/lib/prismatik' not found."
	exit 1
fi

if [ ! -e "deb/usr/lib/prismatik/libPythonQt.so.1" ];
then
	echo "File 'deb/usr/lib/prismatik/libPythonQt.so.1' not found."
	exit 1
fi

if [ ! -e "deb/usr/lib/prismatik/libPythonQt_QtAll.so.1" ];
then
	echo "File 'deb/usr/lib/prismatik/libPythonQt_QtAll.so.1' not found."
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
