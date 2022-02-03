[bits 64]
section .data
jumpBack dq 0

section .text
global patch_deformtool_move_uwp
extern originalNormalX
extern originalNormalZ
extern modifiedNormalX
extern modifiedNormalZ
extern applyNewNormal
patch_deformtool_move_uwp:
	mov rdi, jumpBack
	mov [rdi], rax

	movss xmm1, [rbp+0x514] ; impactNormalX = hitResult.impactNormal.X
	movss [rsp+0x70], xmm1
	
	mov rdi, originalNormalX
	movss [rdi], xmm1

	movss xmm0, [rbp+0x518] ; impactNormalY = hitResult.impactNormal.Y
	movss [rsp+0x74], xmm0
	movss xmm1, [rbp+0x51c] ; impactNormalZ = hitResult.impactNormal.Z
	movss [rsp+0x78], xmm1

	mov rdi, originalNormalZ
	movss [rdi], xmm1

	mov rdi, applyNewNormal
	mov eax, [rdi]
	test eax, eax
	jz end

	mov rdi, modifiedNormalX
	movss xmm1, [rdi]
	movss [rbp+0x514], xmm1
	movss [rsp+0x70], xmm1

	mov rdi, modifiedNormalZ
	movss xmm1, [rdi]
	movss [rbp+0x51c], xmm1
	movss [rsp+0x78], xmm1

end:
	mov rdi, jumpBack
	jmp [rdi]