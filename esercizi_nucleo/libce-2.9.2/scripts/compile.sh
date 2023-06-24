#!/bin/bash

source @ETC@/libce.conf

if [ -r Makefile ]; then
	export LIBCECONF=@ETC@/libce.conf
	make reset
	exec make
fi

OUTPUT=${1:-a.out}

case "$OUTPUT" in
	*.cpp|*.s)
		if [ -f "$OUTPUT" ]; then
			echo "`basename $0`: evito sovrascrittura di '$OUTPUT'" > /dev/stderr
			exit 1
		fi
		;;
	*)
		;;
esac

COMPILER_OPTIONS="
	-g
	-fno-exceptions
	-fno-rtti
	-fno-pic
	-no-pie
	-fno-stack-protector
	-mno-red-zone
	-gdwarf-2
	-I${INCLUDE}
	-Wno-main
	-Wno-return-type
	-ffreestanding
	-m64
	-std=c++17
	$CFLAGS
	"

if [ -n "$AUTOCORR" ]; then
	COMPILER_OPTIONS+=" -DAUTOCORR"
fi

ASSEMBLER_OPTIONS="
	-g
	-m64
	-I${INCLUDE}
	-x assembler-with-cpp
	$ASFLAGS
	"

LINKER_OPTIONS="
	-melf_x86_64
	-nostdlib
	-Ttext 0x200000
	-z noexecstack
	$LDFLAGS
	-L${LIB64}
	${LIB64}/start64.o
	"

set -e

shopt -s nullglob
for f in *.cpp; do
	$CE_CXX64 $COMPILER_OPTIONS -c $f
done

for f in *.s; do
	$CE_AS64 $ASSEMBLER_OPTIONS -c -o ${f%.s}_s.o $f
done
shopt -u nullglob

$CE_LD64 $LINKER_OPTIONS *.o -lce64 -o "$OUTPUT"

