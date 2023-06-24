#!/bin/bash

source @ETC@/libce.conf

[ -n "$QEMU" ] && {
	[ -x "$QEMU" ] || { 
		[ "$REPORT" != 0 ] && echo "$QEMU non esistente o non eseguibile" >&2
		QEMU=
		REPORT=${REPORT:-1}
	}
}
[ -z "$QEMU" ] && QEMU=${BIN}/qemu-system-x86_64
[ -x "$QEMU" ] || QEMU=$(which qemu-system-x86_64)
[ -x "$QEMU" ] || { echo "qemu non trovato" >&2; exit 1; }

[ "$REPORT" = 1 ] && echo "uso $QEMU" >&2

serial=stdio
cmd="\"$QEMU\" \
	-audiodev alsa,id=alsa \
	-machine pcspk-audiodev=alsa \
	-no-reboot \
	-net none \
	-m ${MEM:-16}"

boot="boot.bin"
exe="a.out"
append=""
drive="-drive file=\"${HD_PATH}\",index=0,media=disk,cache=writeback,format=raw"

debug=
kvm=
curses=
for opt
do
	case $opt in
	-g)
		debug="-append \"-s\" -chardev socket,path=gdb-socket,server=on,wait=off,id=gdb0 -gdb chardev:gdb0"
		cmd="$cmd $debug"
		;;
	-n)
		drive=
		;;
	-c)
		CECURSES=1
		curses="-display curses"
		serial=file:serial.txt
		cmd="$cmd $curses"
		;;
	-k)
		kmv="-enable-kvm"
		cmd="$cmd $kvm"
		;;
	*)
		exe=$opt
	esac
done
if [ -n "$AUTOCORR" ]; then
	cmd="$cmd $drive -nographic"
else
	cmd="$cmd $drive -serial $serial"
fi

if [ -x run ]; then
	export LIBCECONF=/home/giuseppe/CE/etc/libce.conf
	export CECURSES
	exec ./run "$debug" "$kvm"
fi

if [ -d "$exe" ]; then
	exe="$exe/a.out"
fi

cmd="$cmd -kernel \"${QEMU_BOOT}\" -initrd \"$exe\""

trap 'stty sane 2>/dev/null' exit

echo Eseguo: $cmd
eval $cmd
