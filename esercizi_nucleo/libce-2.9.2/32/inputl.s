.text
        .global inputl
inputl:
        movw 4(%esp), %dx
        inl %dx, %eax
        ret
