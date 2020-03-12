#! /bin/sh
git clone https://github.com/opencomputeproject/OpenNetworkLinux.git
cd OpenNetworkLinux
#Best working version of ONLPv2
commit=d9730e34670429302377d277b429c3a0582ccdd4
git checkout $commit
patch -p2 < ../"$commit.patch"
cp -r ../stordis ./packages/platforms/
#Debian version, 8 or 9
export VERSION=9
make docker
