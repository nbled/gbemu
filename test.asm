INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]
jp Start
ds $150 - @, 0

Start:
    ld  a, 99
    inc a
    halt

