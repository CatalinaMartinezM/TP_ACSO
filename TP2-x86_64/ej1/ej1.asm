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

; string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash)
string_proc_list_add_node_asm:
    push rbp
    mov rbp, rsp
    mov r8, rdi   ; list
    mov r9b, sil  ; type
    mov r10, rdx  ; hash
    
    movzx rdi, sil ; type
    mov rsi, rdx   ; hash
    call string_proc_node_create_asm

    test rax, rax
    je .end

    mov r9, rax

    test r8, r8 ; list == NULL?
    je .end

    ;mov r11, [r8 + 8] ; r11 = list -> last
    ;test r11, r11           ; list -> last == NULL?
    cmp qword [r8], 0
    je .isEmpty

    mov r11, [r8 + 8] ; r11 = list -> last
    mov qword [rax + 8], r11 ; node -> previous = list -> last
    mov qword [r11], rax     ; list -> last -> next = node;

    mov qword [r11], rax ; list -> last  = node
    jmp .end

    .isEmpty
    mov [r8], r9  ; list -> first = node
    mov [r8 + 8], r9 ; list -> last  = node

    .end
    pop rbp
    ret


string_proc_list_concat_asm:

