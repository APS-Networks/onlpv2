####To build 
cd stordis_onlpv2  
./build.sh
####Patch files
.patch files are patch files works against specific commits on ONLPv2 branch of OpenNetworkLinux.
####To install
Boot into onie (os install mode)
copy build image to switch.
login to switch
onie-nos install <image name>
####Post install
ln -s /usr/share/onl/packages/amd64/onl-kernel-4.14-lts-x86-64-all/mbuilds/ /lib/modules/4.14.49-OpenNetworkLinux/build
