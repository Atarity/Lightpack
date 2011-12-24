# Update MD5 sums
md5deep -r deb/usr > deb/DEBIAN/md5sums

# Build package in format "name_version_arch.deb"
fakeroot dpkg-deb --build deb/ .
