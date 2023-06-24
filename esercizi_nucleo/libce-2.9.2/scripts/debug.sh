#!/bin/bash

source @ETC@/libce.conf

if ! [ -e gdb-socket ]; then
	echo "connessione non trovata: directory errata, oppure 'boot' lanciato senza '-g'" >&2
	exit 1
fi

GDBINIT=$(mktemp)
trap "rm '$GDBINIT'" exit
if [ -r .gdbinit ]; then
	sed 's/^target remote.*$/target remote gdb-socket/' .gdbinit > "$GDBINIT"
else
	EXE=${1:-a.out}

	cat <<-EOF > "$GDBINIT"
	set pagination off
	set arch i386:x86-64
	file ${QEMU_BOOT}
	target remote gdb-socket
	set {int}&wait_for_gdb=0
	file $EXE
	source ${LIB64}/libce-debug.py
	break *main
	continue
EOF
fi

$CE_GDB ${GDBINIT:+-x "$GDBINIT"}
