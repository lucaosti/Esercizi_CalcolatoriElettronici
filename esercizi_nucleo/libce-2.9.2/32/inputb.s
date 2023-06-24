.text
        .global inputb
inputb:
        movw 4(%esp), %dx
        inb %dx, %al
        ret
