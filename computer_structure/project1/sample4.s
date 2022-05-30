        .data
data:   .word 4             0000
data2:  .word 0x00000000    0004
        .word 0x00000400    0008
        .word 0x00000001
        .text
main:
        addiu $2, $0, 0x4   0
        addiu $1, $2, -3    04
        la $3, data         08, 
        addu $4, $3, $2     0c
        la $5, data2        10 , 14
        lw $6, 4($4)        18
        srl $7, $6, 8       1c
        sw $7, 0($5)        20
        lb $8, 0($5)        24
        sll $9, $7, 20      28
        bne $4, $5, check   2c
        jal check           30

sub:
        addiu $11, $11, 5   34
        srl $11, $11, 1     38
        sb $11, 11($5)      3c
        j check             40

check:
        addiu $10, $10, 1   44
        lb $11, 11($4)      48
        sltu $20, $11, $2   4c
        sltu $21, $2, $11   50
        beq $20, $21, exit  54
        ori $9, $9, 0x30    58
        jr $31              5c

exit: