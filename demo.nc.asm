main:
  mov eax, 100
  neg eax
  test eax, eax
  xor eax, eax
  sete al
  neg eax
  ret
