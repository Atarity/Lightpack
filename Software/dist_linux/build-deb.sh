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

if [ -e "deb/usr/bin/Prismatik" ];
then
	echo "Renaming 'deb/usr/bin/Prismatik' to 'deb/usr/bin/prismatik'."
	mv deb/usr/bin/Prismatik deb/usr/bin/prismatik
fi

if [ ! -e "deb/usr/bin/prismatik" ];
then
	echo "File 'deb/usr/bin/prismatik' not found."
	exit 2
fi

if [ ! -e "deb/usr/lib/prismatik" ];
then
	echo "File 'deb/usr/lib/prismatik' not found."
	exit 3
fi

if [ ! -e "deb/usr/lib/prismatik/libQt5SerialPort.so.5" ];
then
	echo "File 'deb/usr/lib/prismatik/libQt5SerialPort.so.5' not found."
	exit 4
fi



if [ -x "`which md5deep 2>/dev/null`" ]; 
then
	# Update MD5 sums
	md5deep -r deb/usr > deb/DEBIAN/md5sums
else
	echo "Please install 'md5deep' package first."
	exit 5
fi

# Build package in format "name_version_arch.deb"
fakeroot dpkg-deb --build deb/ .
