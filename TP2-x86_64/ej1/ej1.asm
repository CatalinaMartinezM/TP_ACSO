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

    test rax, rax ; rax == NULL
    je .end

    mov [rax], 0    ; list -> first = NULL
    mov [rax+8], 0  ; list -> last  = NULL

    .end
    pop rbp
    ret

string_proc_node_create_asm:

string_proc_list_add_node_asm:

string_proc_list_concat_asm:

