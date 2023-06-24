.text
        .global outputb
outputb:
        movb 4(%esp), %al
        movw 8(%esp), %dx
        outb %al, %dx
        ret
