global Sum
Sum:
  push ebp
  mov ebp, esp
  ; return:
  ; [ebp + 4] + [ebp + 8]
  mov eax, [ebp + 4]
  mov ecx, [ebp + 8]
  add eax, ecx
  push eax
  mov eax, [ebp - 4]
  add esp, 4
  pop ebp
  ret
global _WinMain@16
_WinMain@16:
  push ebp
  mov ebp, esp
  ; declaration:
  push dword 5
  push dword 4
  call Sum
  add esp, 8
  push eax
  ; return:
  mov eax, [ebp - 4]
  add esp, 4
  pop ebp
  ret 16
