#!/bin/bash

cat << EOF
/*
 * Install RoboGo Tool
 */
EOF

ROOT_PATH=$(cd "$(dirname "$0")";pwd)

pushd $ROOT_PATH &> /dev/null

echo "Uninstall Old RoboGo Tool"

apt -y remove robogotool

echo "Install RoboGo Tool"

apt install ./RoboGoTool*.deb 

echo "done"

popd &> /dev/null
