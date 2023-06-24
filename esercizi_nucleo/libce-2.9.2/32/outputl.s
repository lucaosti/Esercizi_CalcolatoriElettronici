.text
        .global outputl
outputl:
        movl 4(%esp), %eax
        movw 8(%esp), %dx
        outl %eax, %dx
        ret
