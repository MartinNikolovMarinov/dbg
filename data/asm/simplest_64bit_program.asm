section .text
    global _start
_start:
    xor edi, edi        ; exit code 0
    mov eax, 60         ; syscall number for exit
    syscall             ; invoke syscall

section .note.GNU-stack
    ; This section is required to indicate no executable stack
