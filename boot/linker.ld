mode freestanding
format raw
entry 0x0
size 512
output "rom.exe"
sections {
    .text
    .data
}
symbols {
    _start=0x0
}
