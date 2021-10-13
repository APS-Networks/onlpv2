#!/usr/bin/env bash
delete="n"
if [ -d "./OpenNetworkLinux" ]
  then
    read -p "Found OpenNetworkLinux. This will be deleted first, OK? [y]/n:" delete
    if [ -z "$delete" ]
      then
        delete="y"
    fi
fi

if [ $delete == "y" ]
  then
  sudo rm -rf ./OpenNetworkLinux
fi

git clone https://github.com/opencomputeproject/OpenNetworkLinux.git
cd OpenNetworkLinux
#Best working version of ONLPv2
commit=d9730e34670429302377d277b429c3a0582ccdd4
git checkout $commit
patch -p2 < ../"$commit.patch"
cp -r ../apsn ./packages/platforms/
#Debian version, 8 or 9
#export VERSION=8
#make docker



