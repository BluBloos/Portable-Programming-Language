global main
main:
  push ebp
  mov ebp, esp
  mov eax, 1
  mov ebx, 2
  add eax, ebx
  push eax
  mov eax, 5
  test eax, eax
  xor eax, eax
  sete eax
  push eax
  mov eax, [ebp - 4]
  mov ebx, [ebp - 8]
  sub eax, ebx
  push eax
  mov eax, [ebp - 12]
  add esp, 12
  pop ebp
  ret
