#include "ej1.h"

string_proc_list* string_proc_list_create(void){
	string_proc_list* list = (string_proc_list*)malloc(sizeof(string_proc_list));
	if (!list) { 
		return NULL; 
	}

	list -> first = NULL;
    list -> last  = NULL;
	
    return list;
}

string_proc_node* string_proc_node_create(uint8_t type, char* hash){
	string_proc_node* node = (string_proc_node*)malloc(sizeof(string_proc_node));
	if (!node) { return NULL; }

	node -> next     = NULL;
    node -> previous = NULL;
    node -> type = type;
	node -> hash = hash;

    return node;
}

void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
	string_proc_node* node = string_proc_node_create(type, hash);
	if (!node){ return; }

	node -> previous = list -> last;

	if (list -> last){
		list -> last -> next = node;
	} else{
		list -> first = node;
	}

	list -> last = node;
}

char* string_proc_list_concat(string_proc_list* list, uint8_t type , char* hash){
	char* result = malloc(strlen(hash) + 1);
	if (!result){ return NULL; }
	strcpy(result, hash);

    string_proc_node* currentNode = list -> first;
    while (currentNode) {
        if (currentNode -> type == type) {
            char* newResult = str_concat(result, currentNode -> hash);
            free(result);
            result = newResult;
        }
        currentNode = currentNode -> next;
    }

    return result;
}


/** AUX FUNCTIONS **/

size_t my_strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* my_strcpy(char* dest, const char* src) {
    char* original_dest = dest;

    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0'; // No olvidarse de terminar el string

    return original_dest;
}

void string_proc_list_destroy(string_proc_list* list){

	/* borro los nodos: */
	string_proc_node* current_node	= list->first;
	string_proc_node* next_node		= NULL;
	while(current_node != NULL){
		next_node = current_node->next;
		string_proc_node_destroy(current_node);
		current_node	= next_node;
	}
	/*borro la lista:*/
	list->first = NULL;
	list->last  = NULL;
	free(list);
}
void string_proc_node_destroy(string_proc_node* node){
	node->next      = NULL;
	node->previous	= NULL;
	node->hash		= NULL;
	node->type      = 0;			
	free(node);
}


char* str_concat(char* a, char* b) {
	int len1 = strlen(a);
    int len2 = strlen(b);
	int totalLength = len1 + len2;
    char *result = (char *)malloc(totalLength + 1); 
    strcpy(result, a);
    strcat(result, b);
    return result;  
}

void string_proc_list_print(string_proc_list* list, FILE* file){
        uint32_t length = 0;
        string_proc_node* current_node  = list->first;
        while(current_node != NULL){
                length++;
                current_node = current_node->next;
        }
        fprintf( file, "List length: %d\n", length );
		current_node    = list->first;
        while(current_node != NULL){
                fprintf(file, "\tnode hash: %s | type: %d\n", current_node->hash, current_node->type);
                current_node = current_node->next;
        }
}