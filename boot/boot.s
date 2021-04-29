stack := 0x0180
_start := 0x0

HDD_IO := 0x02
HDD_CMD_GET_ID := 0x08
HDD_CMD_READ_BY_NAME := 0x14

.section .text

.globl _start
.type _start, function
_start:
    ; Setup a memory layout.
    ; 0x0000-0x01FF (512B) BIOS data
    ;  0x0000-0x017F (384B) ROM
    ;   0x0000-0x015F (352B) Code (.text)
    ;   0x0160-0x017F (32B) IVT (.ivt)
    ;  0x0180-0x01FF (128B) RAM
    ;   0x0180-0x01FF (128B) Stack

    ; Setup IVT
    livt 0x0160

    ; Find a file on disk called "/sys/krnl.EXE".

    ; Goto /sys (it has a hardcoded ID 0x0001).
    out16 HDD_IO, 0x0102 ; Write Register: User Address
    out16 HDD_IO, 0x0001 ;

    ; Read /krnl.
    out8 HDD_IO, HDD_CMD_READ_BY_NAME
    out16 HDD_IO, 0x6b72 ; "kr"
    out16 HDD_IO, 0x6e6c ; "nl"
    out16 HDD_IO, 0x00f1 ; stop reading + EXE extension
    out16 HDD_IO, 0x0400 ; address
    out16 HDD_IO, 0x0000 ; offset
    out16 HDD_IO, 0x0200 ; size

    ; Wait for IRQ. The IRQ handler sets DX to 1.
    mov dx, 0x0
    _fenbl IF ; Flag Enable: Interrupt Flag
_1:
    hlt
    ; Got an IRQ.
    cmp dx, 0x0
    ; If jumped, the IRQ was unrelated to HDD.
    ; TODO: Crash when other IRQs will be masked and only CPU exceptions / NMIs will be allowed.
    je _1
    _fdsbl IF ; Disable IRQs.

    ; Check image magic (0x50 - P) and arch (0x80 - cx16)
    mov bx, 0x0400
    mov dx, [bx]
    cmp dx, 0x5080
    je valid_boot_image
    _emu_dbg "Invalid boot image"
    hlt

valid_boot_image:
    mov bx, 0x0402
    mov dx, [bx] ; dx contains OS entry point

    mov ax, dx
    jmp ax
    
    ; We shouldn't go here - hang forever.
    _emu_dbg "Cheats??"
    _fdsbl IF
    hlt

hdd_entry:
    in8 HDD_IO, al ; To not hang the HDD
    mov dx, 0x0 ; Note that we cannot use global variables here, so we use 'dx'.
    iret

.section .ivt
    data 0x0B
    skip 22
    data hdd_entry
