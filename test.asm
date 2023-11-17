INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]
jp Start
ds $150 - @, 0

Start:
    ld  a, 0

Loop:
    inc a
    cp a, 100
    jp nz, Loop

    cp a, 100
    jp nz, Failure

    add a, 20
    sub a, 20
    cp a, 100
    jp nz, Failure

    ld bc, $dead
    ld de, $beef

    push bc
    push de

    ld bc, $ff00
    ld de, $00ff
    ld c, e
    ld d, b

    ld a, b
    cp a, $ff
    jp nz, Failure
    ld a, c
    cp a, $ff
    jp nz, Failure
    ld a, d
    cp a, $ff
    jp nz, Failure
    ld a, e
    cp a, $ff

    pop de
    pop bc

    ld a, b
    cp a, $de
    jp nz, Failure
    ld a, c
    cp a, $ad
    jp nz, Failure
    ld a, d
    cp a, $be
    jp nz, Failure
    ld a, e
    cp a, $ef

Success:
    ld a, 1
    halt

Failure:
    ld a, 0
    halt