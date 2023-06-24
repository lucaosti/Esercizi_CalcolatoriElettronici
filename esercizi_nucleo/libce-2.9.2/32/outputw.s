.text
        .global outputw
outputw:
        movw 4(%esp), %ax
        movw 8(%esp), %dx
        outw %ax, %dx
        ret
