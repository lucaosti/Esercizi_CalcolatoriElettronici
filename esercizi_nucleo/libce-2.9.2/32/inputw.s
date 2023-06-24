.text
        .global inputw
inputw:
        movw 4(%esp), %dx
        inw %dx, %ax
        ret
