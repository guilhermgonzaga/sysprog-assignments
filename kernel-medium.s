.text	                            # Code segment
.code16                             # Real mode
.globl _start                       # The entry point must be global

  .equ START, 0x1000
  .equ SECTOR_SIZE, 0x200

#
# The first instruction to execute in a program is called the entry
# point. The linker expects to find the entry point in the "symbol" _start
# (with underscore).
#
_start:
  mov $message1, %si
  call print_string

  mov $message2, %si
  call print_string

  mov $message3, %si
  call print_string

  mov $explanation, %si
  call print_string

  # in case that didn't work
halt:
  hlt
  jmp halt

print_string:
  lodsb
  cmp $0, %al
  jz print_string_done
  call print_char
  jmp print_string
print_string_done:
  ret

print_char:
  movb $0xe, %ah
  movw $0x0002, %bx
  int $0x10
  ret

explanation: .asciz "If you saw three lines, it probably works.\r\n"


  .org 14 * SECTOR_SIZE + START
message1: .asciz "Expect two more lines...\r\n"

  .org 51 * SECTOR_SIZE + START
message2: .asciz "Expect one more line...\r\n"

  .org 75 * SECTOR_SIZE + START
message3: .asciz "This is the third line.!\r\n"

end:



