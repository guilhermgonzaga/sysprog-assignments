.text	                            # Code segment
.code16                           
.globl _start                       # The entry point must be global

  .equ START, 0x1000
  .equ BOOT_SECTOR_SIZE, 0x200
  .equ PARAGRAPH, 0x10

  .equ TESTDATA_SEGMENT, 0x14c0
  .equ TESTDATA_OFFSET, 0
#
# The first instruction to execute in a program is called the entry
# point. The linker expects to find the entry point in the "symbol" _start
# (with underscore).
#
_start:
  mov $okay1,%si
  call print_string

  mov $TESTDATA_SEGMENT,%bx
  mov %bx, %es
  movb %es:(0), %al
  cmp $'T',%al
  jnz fail

  mov $okay2,%si
  call print_string

  movb %es:(1), %al
  cmp $'h',%al
  jnz fail

  mov $0x0301, %ax
  mov $0x0001, %cx
  mov $TESTDATA_OFFSET, %bx
  mov $0x0001, %dx
  int $0x13

  mov $okay3,%si
  call print_string

shutdown:
  # shut down the computer
  movw $0x5307, %ax
  movw $0x01, %bx
  movw $0x03, %cx
#  int $0x15

  # in case that didn't work
halt:
  hlt
  jmp halt

fail:
  mov $bad, %si
  call print_string
  jmp shutdown

okay1: .asciz "Passed test 1\n\r"
okay2: .asciz "Passed test 2\n\r"
okay3: .asciz "Passed test 3\n\r"
bad: .asciz "Bad!\n\r"

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


  .org 150 * BOOT_SECTOR_SIZE + START

testdata:
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "

  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "

  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "

  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
  .string "This is sixteen "
end:



