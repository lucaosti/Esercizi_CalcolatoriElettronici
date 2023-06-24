.global start,_start
.text
start:				// entry point
_start:
	call init_all
        call main
	call stop_all
	call reboot
	hlt
