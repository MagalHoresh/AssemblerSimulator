; file ps.as

.entry LIST
.extern W
.define sz=2
MAIN:       movit   r3, LIST[sz]
LOOP:       jmp     W
            prn     #TTT
            mov     STR[5], STR[2]
            sub     r1, r4
            cmp     K, #sz
            bne     W
L1:         inc     L3
.entry LOOP
            bne     LOOP
END:        stop
.define len    = 4
STR:        .string "abcdef"
LIST:       .data   6, -9, len
K:          .data   22

.define sz=7
.extern L3