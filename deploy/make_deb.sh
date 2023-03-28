#!/bin/bash

version=1.5.6
deb_file="RoboGoTool_"$version"_arm64.deb"
debian="DEBIAN"

_dir="./deb/"
if [ ! -d "$_dir" ]; then
    echo "directory ./deb not found"
    exit 1
fi

cd $_dir

if [ -d "$debian" ]; then
    rm -rf $debian
fi

if [ -f "$deb_file" ]; then
    rm $deb_file
fi
mkdir $debian
cd $debian

cat << EOF > control
Package: RoboGoTool
Version: $version
Section: devel
Priority: required
Origin: Ubuntu
Architecture: arm64
Maintainer: RoboGo Developers <kun.yang@ubtrobot.com>
Description: IMU device tool for RoboGo
EOF

cat << EOF > postinst
# !/bin/sh
echo "RoboGoTool installed"
EOF

cat << EOF > postrm
# !/bin/sh
echo "RoboGoTool removed"
EOF

cat << EOF > prerm
# !/bin/sh
echo "RoboGoTool start to remove"
EOF

cd ..
chmod -R 755 $debian
cd ..

dpkg -b ${_dir} $deb_file
#cp $deb_file ../
#rm -rf $deb_file
rm -rf "${_dir}/"$debian
echo "make $deb_file success!"
