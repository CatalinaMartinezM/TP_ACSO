#include "ej1.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

/**
*	crea y destruye a una lista vacía
*/
void test_create_destroy_list(){
	string_proc_list * list	= string_proc_list_create_asm();
	string_proc_list_destroy(list);
}

/**
*	crea y destruye un nodo
*/
void test_create_destroy_node(){
	string_proc_node* node	= string_proc_node_create_asm(0, "hash");
	string_proc_node_destroy(node);
}

/**
 * 	crea una lista y le agrega nodos
*/
void test_create_list_add_nodes()
{	
	string_proc_list * list	= string_proc_list_create_asm();
	string_proc_list_add_node_asm(list, 0, "hola");
	string_proc_list_add_node_asm(list, 0, "a");
	string_proc_list_add_node_asm(list, 0, "todos!");
	string_proc_list_destroy(list);
}

/**
 * 	crea una lista y le agrega nodos. Luego aplica la lista a un hash.
*/

void test_list_concat()
{
	string_proc_list * list	= string_proc_list_create();
	string_proc_list_add_node(list, 0, "hola");
	string_proc_list_add_node(list, 0, "a");
	string_proc_list_add_node(list, 0, "todos!");	
	char* new_hash = string_proc_list_concat(list, 0, "hash");
	string_proc_list_destroy(list);
	free(new_hash);
}

/**
 *  Crea una lista vacia y verifica que 'first' y 'last' sean NULL
 */
void test_create_empty_list() {
    string_proc_list* list = string_proc_list_create();
    
    if (!list) {
        printf("Fallo al crear la lista\n");
        return;
    }
    if (list->first != NULL) {
        printf("Error: list -> first no es NULL\n");
    }
    if (list->last != NULL) {
        printf("Error: list -> last no es NULL\n");
    }

	if (list->first == NULL && list->last == NULL) {
        printf("[OK] test_create_empty_list\n");
    }

    string_proc_list_destroy(list);
}

/**
 *  Crea un nodo. Verifica que:
 *  - 'next' y 'previous' sean NULL
 *  - 'type' coincida con el parametro indicado
 *  - 'hash' apunte al mismo.
 */
void test_create_node() {
    char* hash = "hash";
    uint8_t type = 12;
    string_proc_node* node = string_proc_node_create(type, hash);

    if (!node) {
        printf("Fallo al crear el nodo\n");
        return;
    }
    if (node -> next != NULL) {
        printf("Error: node -> next no es NULL\n");
    }
    if (node -> previous != NULL) {
        printf("Error: node -> previous no es NULL\n");
    }
    if (node -> type != type) {
        printf("Error: node -> type incorrecto (esperado %d, recibido %d)\n", type, node -> type);
    }
    if (node -> hash != hash) {
        printf("Error: node -> hash no apunta al string original\n");
    }

    if (node -> next == NULL && node -> previous == NULL && node -> type == type && node -> hash == hash) {
        printf("[OK] test_create_node\n");
    }

    string_proc_node_destroy(node);
}

/**
 *  Crea una lista vacia y agrega un unico nodo. Verifica que:
 *  - Los punteros 'first' y 'last' de la lista no sean NULL.
 *  - 'first' y 'last' apunten al mismo nodo.
 *  - Los punteros 'next' y 'previous' del nodo sean NULL, ya que es el unico nodo en la lista.
 */
void test_add_single_node() {
    string_proc_list* list = string_proc_list_create();
    string_proc_list_add_node(list, 1, "nodo1");

    if (!list -> first || !list -> last) {
        printf("Error: list -> first o list -> last son NULL\n");
    } else if (list -> first != list -> last) {
        printf("Error: list -> first y list -> last no son iguales (con el mismo nodo)\n");
    } else if (list -> first -> next != NULL || list -> first -> previous != NULL) {
        printf("Error: punteros next o previous del nodo no son NULL\n");
    } else {
        printf("[OK] test_add_single_node\n");
    }

    string_proc_list_destroy(list);
}

/**
 *  Crea una lista vacia y agrega tres nodos. Verifica que:
 *  - La lista tenga tres nodos con 'first' apuntando al primer nodo y 'last' al ultimo.
 *  - Los punteros 'next' y 'previous' de los nodos intermedios sean correctos.
 *  - El primer nodo de la lista tenga su puntero 'previous' como NULL.
 *  - El ultimo nodo de la lista tenga su puntero 'next' como NULL.
 */
void test_add_multiple_nodes() {
    string_proc_list* list = string_proc_list_create();
    string_proc_list_add_node(list, 1, "nodo1");
    string_proc_list_add_node(list, 2, "nodo2");
    string_proc_list_add_node(list, 3, "nodo3");

    string_proc_node* first = list -> first;
    string_proc_node* second = first ? first->next : NULL;
    string_proc_node* third = second ? second->next : NULL;

    if (!first || !second || !third || third -> next != NULL) {
        printf("Error: estructura de la lista incorrecta\n");
    } else if (second -> previous != first || third -> previous != second) {
        printf("Error: punteros previous incorrectos\n");
    } else if (list -> last != third) {
        printf("Error: list -> last no apunta al último nodo\n");
    } else if (first -> previous != NULL) {
        printf("Error: el 'previous' del primer nodo debe apuntar a NULL\n");
    } else if (third -> next != NULL) {
        printf("Error: el 'next' del último nodo debe apuntar a NULL\n");
    } else {
        printf("[OK] test_add_multiple_nodes\n");
    }

    string_proc_list_destroy(list);
}



/**
* Corre los test a se escritos por lxs alumnxs	
*/
void run_tests(){

	/* Aqui pueden comenzar a probar su codigo */
	test_create_empty_list();

	test_create_node();

	test_add_single_node();

	test_add_multiple_nodes();

	// test_create_destroy_list();

	// test_create_destroy_node();

	// test_create_list_add_nodes();

	// test_list_concat();
}

int main (void){
	run_tests();
	return 0;    
}