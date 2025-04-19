; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat

; string_proc_list* string_proc_list_create(void)
string_proc_list_create_asm:
    push rbp
    mov rbp, rsp

    mov rdi, 16 ; guardo en rdi el tamaño de la lista
    call malloc ; la direccion de memoria donde empieza la lista se guarda en rax

    test rax, rax ; rax == NULL?
    je .end

    mov qword [rax], 0      ; list -> first = NULL
    mov qword [rax + 8], 0  ; list -> last  = NULL

    .end
    pop rbp
    ret

; string_proc_node* string_proc_node_create(uint8_t type, char* hash)
string_proc_node_create_asm:
    push rbp
    mov rbp, rsp
    mov r8b, dil ; guardo type (lo recibi en rdi) para no perderlo (dps voy a reescribir rdi)

    mov rdi, 32
    call malloc

    test rax, rax
    je .end

    mov qword [rax], 0
    mov qword [rax + 8], 0
    mov byte [rax + 16], r8b
    mov qword [rax + 24], rsi

    .end
    pop rbp
    ret

; void string_proc_list_add_node_asm(string_proc_list* list, uint8_t type, char* hash)
string_proc_list_add_node_asm:
    push rbp
    mov rbp, rsp
    push rbx
    
    mov rbx, rdi    ; list
    
    movzx rdi, sil  ; type
    mov rsi, rdx    ; hash
    call string_proc_node_create_asm
    
    test rax, rax  ; rax = node
    je .end
    
    cmp qword [rbx], 0  ; list -> first == NULL?
    je .isEmpty
    
    mov rcx, [rbx + 8]  ; rcx = list -> last
    mov [rax + 8], rcx  ; node -> previous = list -> last

    mov [rcx], rax    ; list -> last -> next = node
    mov [rbx + 8], rax  ; list -> last = node
    jmp .end

    .isEmpty
    mov [rbx], rax    ; list -> first = node
    mov [rbx + 8], rax  ; list -> last  = node
    jmp .end
    
    .end:
    pop rbx
    pop rbp
    ret

string_proc_list_concat_asm:

