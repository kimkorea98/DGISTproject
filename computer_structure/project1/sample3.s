
array:  .word   3
        .word   123
        .word   4346
array2: .word   0x12345678
        .word   0xFFFFFFFF
        .text
main:
        addiu   $2, $0, 1024    00
        addu    $3, $2, $2      04
        or      $4, $3, $2      08
        addiu   $5, $0, 1234    0c
        sll     $6, $5, 16      10
        addiu   $7, $6, 9999    14
        subu    $8, $7, $2      18
        nor     $9, $4, $3      1c
        ori     $10, $2, 255    20
        srl     $11, $6, 5      24
        srl     $12, $6, 4      28
        la      $4, array2      2c, 30
        lb      $2, 1($4)       34
        sb      $2, 6($4)       38
        and     $13, $11, $5    3c
        andi    $14, $4, 100    40
        subu    $15, $0, $10    44
        lui     $17, 100        48
        addiu   $2, $0, 0xa     4c
           