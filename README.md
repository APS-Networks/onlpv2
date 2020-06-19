# How Tos

## Recommendation  

Recommended is to use debian 8 which compiles stordis package with kernel-3.16.
ONL support file from BF to be used Deb8.d91e20140ef9efe7a54a79439c4c205472a69841.tar , No need to apply patch provided that ia already catered in the .patch file available in this repository.
Hence latest  SDE-9.0 can be used.

## To build

cd stordis_onlpv2  
./build.sh  

### Then execute following steps afterwards, change debian version 8 (recommended) or 9 in below command  

cd OpenNetworkLinux  
sudo docker/tools/onlbuilder -8  
apt-cacher-ng  
source setup.env  
sudo apt-get install ca-certificates  
make amd64  

Installer is available in OpenNetworkLinux/RELEASE/jessie/amd64/ONL-<branch>_ONL-OS_<build identifier>_AMD64_INSTALLED_INSTALLER

## Patch files

.patch files are patch files works against specific commits on ONLPv2 branch of OpenNetworkLinux.
best working hash is mentioned in the file build.sh and patch file is named after that hash.

## To install

Boot into onie (os install mode)
copy build image to switch.
login to switch
onie-nos install <image name>

## Post install

ln -s /usr/share/onl/packages/amd64/onl-kernel-<kernel-version>-lts-x86-64-all/mbuilds/ /lib/modules/<kernel-version as shown in uname -a>-OpenNetworkLinux/build
