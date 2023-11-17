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

    ld a, $ff
    bit 0, a
    bit 1, a
    bit 2, a
    bit 3, a
    bit 4, a
    bit 5, a
    bit 6, a
    bit 7, a

    ld b, 18
    ld c, 7
    call Mult

    cp $0a
    jp nz, Failure

Success:
    ld a, 1
    halt

Failure:
    ld a, 0
    halt

Mult:
    push bc
    ld a, b
    cp a, 0         ; if (b == 0)
    jp z, MultRet0  ;   goto MultRet0;
    dec b
    call Mult       
    add c
    pop bc
    ret             ; return c + Mult(b--, c);
MultRet0:
    ld a, 0         ; MultRet0:
    pop bc
    ret             ;   return 0;

