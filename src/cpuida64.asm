;  nasm -f elf64 -g -F stabs cpuida64.asm   for cpuida64.o
;  gcc cpuidc64.c -m64 -c          for cpuidc64.o
;  gcc test.c cpuidc64.o cpuida64.o -m64 -lc -lm -o test
;  ./test


section .text

    global _cpuida   

 _cpuida:

    push rax
    push rbx
    push rcx
    push rdx
    push rdi

    mov  eax, 1
    CPUID
    mov [eaxCode1], eax  ; Features Code
    mov [edxCode1], edx  ; family/model/stepping
    mov [ebxCode1], ebx
    mov [ecxCode1], ecx

    mov  rdi, idString1
    mov  eax, 0
    CPUID
    mov[rdi],   ebx
    mov[rdi+4], edx
    mov[rdi+8], ecx
    mov ecx, 0
    mov[rdi+12], ecx
    mov [intel1amd2], ebx


    ; get specials
    mov eax,80000000h
    CPUID
    cmp eax,80000000h
    jc nomore

    mov eax, [intel1amd2]
    cmp eax, 756E6547h    ; uneG
    jz intel
    cmp eax, 68747541h    ; htuA
    jnz nomore
    mov eax,80000001h
    CPUID

    mov [ext81edx], edx

    mov eax,80000000h
    CPUID
    cmp eax,80000004h
    jc nomore                
    mov  rdi, idString2
    mov eax,80000002h
    CPUID
    mov[rdi],    eax
    mov[rdi+4],  ebx
    mov[rdi+8],  ecx
    mov[rdi+12], edx
    mov eax,80000003h
    CPUID
    mov[rdi+16], eax
    mov[rdi+20], ebx
    mov[rdi+24], ecx
    mov[rdi+28], edx
    mov eax,80000004h
    CPUID
    mov[rdi+32], eax
    mov[rdi+36], ebx
    mov[rdi+40], ecx
    mov[rdi+44], edx
    mov ecx, 0
    mov[rdi+48], ecx
    jmp nomore
   intel:
    mov eax,80000000h
    CPUID
    cmp eax,80000004h
    jc nomore                
    mov  rdi, idString2
    mov eax,80000002h
    CPUID
    mov[rdi],    eax
    mov[rdi+4],  ebx
    mov[rdi+8],  ecx
    mov[rdi+12], edx
    mov eax,80000003h
    CPUID
    mov[rdi+16], eax
    mov[rdi+20], ebx
    mov[rdi+24], ecx
    mov[rdi+28], edx
    mov eax,80000004h
    CPUID
    mov[rdi+32], eax
    mov[rdi+36], ebx
    mov[rdi+40], ecx
    mov[rdi+44], edx
    mov ecx, 0
    mov[rdi+48], ecx
   nomore:

    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax


section .data

 extern  idString1
 extern  idString2
 extern  eaxCode1
 extern  ebxCode1
 extern  ecxCode1
 extern  edxCode1
 extern  intel1amd2
 extern  ext81edx


section .text

    global _calculateMHz   

 _calculateMHz:

        push rax
        push rdx
        push rbx 
        RDTSC 
        mov [startCount], eax 
        mov ebx, 25000
  outerloop:
        mov edx, 25000
  innerloop:    
        dec     edx
        jne     innerloop
        dec     ebx
        jne     outerloop
        RDTSC
        mov     [endCount], eax
        sub     eax, [startCount]
        mov     [cycleCount], eax        
        pop rbx
        pop rdx
        pop rax

section .data

 extern  startCount
 extern  endCount
 extern  cycleCount


