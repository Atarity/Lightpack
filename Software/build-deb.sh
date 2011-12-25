if [ -x "`which md5deep 2>/dev/null`" ]; 
then
	# Update MD5 sums
	md5deep -r deb/usr > deb/DEBIAN/md5sums
else
	echo "Please install 'md5deep' package first."
	exit 1
fi

# Build package in format "name_version_arch.deb"
fakeroot dpkg-deb --build deb/ .
