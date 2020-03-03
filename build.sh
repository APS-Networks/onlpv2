#! /bin/sh
git clone https://github.com/opencomputeproject/OpenNetworkLinux.git -b ONLPv2
cd OpenNetworkLinux
#Best working hash of ONLv2 branch
git checkout d9730e34670429302377d277b429c3a0582ccdd4
patch -p2 < ../stordis_onlpv2.patch
cp -r ../stordis ./packages/platforms/
#Debian version
export VERSION=9
make docker
