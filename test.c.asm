global main
main:
  push ebp
  mov ebp, esp
  mov eax, 1
  mov ebx, 2
  add eax, ebx
  push eax
  mov eax, 1
  mov ebx, 1
  add eax, ebx
  push eax
  mov eax, [ebp - id:8]
  mov ebx, *
  mul ebx
  push eax
  mov eax, [ebp - id:12]
  add esp, 12
  pop ebp
  ret
