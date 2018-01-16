#!/bin/bash

declare -r HARDWAREPLATFORM=`uname -i`
printf "Checking nn_makedata($HARDWAREPLATFORM) output files md5 checksums... "
if [ ! -x "`which nn_makedata`" ]; then
        printf "FAILED\n"
        printf "Error: nn_makedata not found\n"
        exit 0
fi

rm -f grid10.hab
rm -f grid10.usc
rm -f grid10.vsc
rm -f grid10.sst
rm -f grid10.frg
rm -f habdata.log
rm -f gmon.out

nn_makedata &> /dev/null
if [ ! -e "grid10.frg" ]; then
        printf "FAILED\n"
        printf "Error: grid10.frg not created\n"
        exit 0
fi
if [ ! -e "grid10.hab" ]; then
        printf "FAILED\n"
        printf "Error: grid10.hab not created\n"
        exit 0
fi
if [ ! -e "grid10.sst" ]; then
        printf "FAILED\n"
        printf "Error: grid10.sst not created\n"
        exit 0
fi
if [ ! -e "grid10.usc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.usc not created\n"
        exit 0
fi
if [ ! -e "grid10.vsc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.vsc not created\n"
        exit 0
fi

if [ "$HARDWAREPLATFORM" = "x86_64" ]; then
#    if [ "`md5sum grid10.frg`" != "be302ef32fedd383e41d8f8223666298  grid10.frg" ]; then
#        printf "FAILED\n"
#        printf "Error: grid10.frg md5 checksums differ\n"
#        exit 0
#    fi
    if [ "`md5sum grid10.hab`" != "5e4d3b51b460797aa0f92a5377c04ce5  grid10.hab" ]; then
        printf "FAILED\n"
        printf "Error: grid10.hab md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.sst`" != "b72cfba69b115607922d004704164a5d  grid10.sst" ]; then
        printf "FAILED\n"
        printf "Error: grid10.sst md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.usc`" != "40ae4cc1f1aa33bff11d1406c88a37ab  grid10.usc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.usc md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.vsc`" != "9198ab71902747a63c745af677e4090e  grid10.vsc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.vsc md5 checksums differ\n"
        exit 0
    fi
    printf "OK\n";
    exit 0
fi

if [ "$HARDWAREPLATFORM" = "i386" ]; then
    if [ "`md5sum grid10.frg`" != "f5bdbc65822567950d99a959da1381bc  grid10.frg" ]; then
        printf "FAILED\n"
        printf "Error: grid10.frg md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.hab`" != "75783fec900ef14e70caa301ada7d0e3  grid10.hab" ]; then
        printf "FAILED\n"
        printf "Error: grid10.hab md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.sst`" != "d7f7901838a28456d15709bf5ca84fec  grid10.sst" ]; then
        printf "FAILED\n"
        printf "Error: grid10.sst md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.usc`" != "7cc2abb5e60fb6f2804687b621a64976  grid10.usc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.usc md5 checksums differ\n"
        exit 0
    fi
    if [ "`md5sum grid10.vsc`" != "a459cd3d06471d6449fa95acee404d78  grid10.vsc" ]; then
        printf "FAILED\n"
        printf "Error: grid10.vsc md5 checksums differ\n"
        exit 0
    fi
    printf "OK\n";
    exit 0
fi

printf "FAILED\n"
printf "Error: unknown hardware platform\n"
