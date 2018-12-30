global _WinMain@16
_WinMain@16:
  push ebp
  mov ebp, esp
  ; declaration:
  push dword 4
  ; return:
  mov eax, [ebp - 4]
  add esp, 4
  pop ebp
  ret 16
