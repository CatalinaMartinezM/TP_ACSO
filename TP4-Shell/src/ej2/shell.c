#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64  // Comando + 63 argumentos

int debug_mode = 0;
int test_mode = 0;

// Funcion para validar la sintaxis y detectar errores
int validate_syntax(char *command) {
    char *p = command;
    int in_quotes = 0;
    char quote_char = 0;
    int pipe_count = 0;
    int consecutive_pipes = 0;
    int command_started = 0;
    int expecting_command = 0;
    
    // Salteo espacios en blanco iniciales
    while (*p && isspace(*p)) p++;
    
    // Verifico si hay un pipe al comienzo
    if (*p == '|') {
        fprintf(stderr, "shell: error de sintaxis cerca del token inesperado '|'\n");
        return -1;
    }
    
    while (*p) {
        if (!in_quotes && (*p == '"' || *p == '\'')) {
            in_quotes = 1;
            quote_char = *p;
            command_started = 1;
        } else if (in_quotes && *p == quote_char) {
            in_quotes = 0;
            quote_char = 0;
        } else if (!in_quotes && *p == '|') {
            pipe_count++;
            consecutive_pipes++;
            
            // Verifico si hay pipes consecutivos
            if (consecutive_pipes > 1) {
                fprintf(stderr, "shell: error de sintaxis cerca del token inesperado '|'\n");
                return -1;
            }
            
            // Verifico que haya un comando despues de un pipe
            if (expecting_command) {
                fprintf(stderr, "shell: error de sintaxis cerca del token inesperado '|'\n");
                return -1;
            }
            
            expecting_command = 1;
            command_started = 0;
        } else if (!isspace(*p)) {
            consecutive_pipes = 0;
            command_started = 1;
            expecting_command = 0;
        }
        p++;
    }
    
    // Verifico si hay comillas sin cerrar
    if (in_quotes) {
        fprintf(stderr, "shell: error de sintaxis: cadena entrecomillada sin cerrar\n");
        return -1;
    }
    
    // Verifico si hay un pipe al final
    if (expecting_command || (pipe_count > 0 && !command_started)) {
        // Saltep espacios en blanco finales para comprobar si hay un comando luego del ultimo pipe
        p--;
        while (p >= command && isspace(*p)) p--;
        if (p >= command && *p == '|') {
            fprintf(stderr, "shell: error de sintaxis cerca del token inesperado '|'\n");
            return -1;
        }
    }
    
    return 0;
}

// Funcion para parsear argumentos respetando comillas y validaciones
int parse_args(char *command, char **args, int max_args) {
    int arg_count = 0;
    char *p = command;
    
    while (*p && arg_count < max_args - 1) { // Dejo espacio para NULL
        // Salteo espacios en blanco iniciales
        while (*p && isspace(*p)) p++;
        if (!*p) break;
        
        char *start = p;
        
        if (*p == '"') {
            // Manejo comillas dobles
            p++; // Salteo comilla de apertura
            start = p;
            while (*p && *p != '"') {
                // Manejo caracteres // dentro de comillas
                if (*p == '\\' && *(p+1)) {
                    p += 2; // Salteo caracter
                } else {
                    p++;
                }
            }
            if (*p == '"') {
                *p = '\0';
                p++;
            } else {
                fprintf(stderr, "shell: error de sintaxis: cadena entrecomillada sin cerrar\n");
                return -1;
            }
        } else if (*p == '\'') {
            // Manejo comillas simples
            p++; // Salteo comilla de apertura
            start = p;
            while (*p && *p != '\'') p++;
            if (*p == '\'') {
                *p = '\0';
                p++;
            } else {
                fprintf(stderr, "shell: error de sintaxis: cadena entrecomillada sin cerrar\n");
                return -1;
            }
        } else {
            // Manejo argumento sin comillas
            while (*p && !isspace(*p)) p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        }
        
        args[arg_count++] = start;
    }
    
    // Verifico si se supero el limite de argumentos
    if (*p) {
        // Salteo espacios en blanco para ver si hay mas argumentos
        while (*p && isspace(*p)) p++;
        if (*p) {
            fprintf(stderr, "shell: demasiados argumentos\n");
            return -1;
        }
    }
    
    args[arg_count] = NULL;
    return arg_count;
}

// Funcion para dividir comandos por pipes respetando comillas
int split_by_pipes(char *command, char **commands, int max_commands) {
    int command_count = 0;
    char *p = command;
    char *start = command;
    int in_quotes = 0;
    char quote_char = 0;
    
    while (*p && command_count < max_commands) {
        if (!in_quotes && (*p == '"' || *p == '\'')) {
            in_quotes = 1;
            quote_char = *p;
        } else if (in_quotes && *p == quote_char) {
            in_quotes = 0;
            quote_char = 0;
        } else if (!in_quotes && *p == '|') {
            // Se encontro un pipe fuera de comillas
            *p = '\0';
            
            // Trim espacios del comando
            char *cmd_start = start;
            char *cmd_end = p - 1;
            
            while (cmd_start <= cmd_end && isspace(*cmd_start)) cmd_start++;
            while (cmd_end >= cmd_start && isspace(*cmd_end)) *cmd_end-- = '\0';
            
            if (cmd_start <= cmd_end && strlen(cmd_start) > 0) {
                commands[command_count++] = cmd_start;
            } else {
                // Comando vacio entre pipes
                commands[command_count++] = "";
            }
            
            start = p + 1;
        }
        p++;
    }
    
    // Manejo el ultimo comando
    if (start <= p && command_count < max_commands) {
        char *cmd_start = start;
        char *cmd_end = p - 1;
        
        while (cmd_start <= cmd_end && isspace(*cmd_start)) cmd_start++;
        while (cmd_end >= cmd_start && isspace(*cmd_end)) *cmd_end-- = '\0';
        
        if (strlen(cmd_start) > 0) {
            commands[command_count++] = cmd_start;
        } else if (command_count > 0) {
            // Comando vacio al final despues de un pipe
            commands[command_count++] = "";
        }
    }
    
    return command_count;
}

int main() {
    char command[4096]; // Tamaño de buffer aumentado para comandos largos
    char *commands[MAX_COMMANDS];
    
    // Verifico variables depuracion y testeo
    if (getenv("SHELL_DEBUG") && strcmp(getenv("SHELL_DEBUG"), "1") == 0) {
        debug_mode = 1;
    }
    if (getenv("SHELL_TEST_MODE") && strcmp(getenv("SHELL_TEST_MODE"), "1") == 0) {
        test_mode = 1;
    }
    
    if (test_mode) {
        printf("Shell iniciada en modo de prueba\n");
    }

    while (1) {
        if (isatty(STDIN_FILENO) || test_mode){
            printf("Shell> ");
        }

        // Leo entrada
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // EOF
        }

        // Elimino \n
        command[strcspn(command, "\n")] = '\0';

        // Salteo comandos vacios
        if (strlen(command) == 0) {
            continue;
        }
        
        // Verifico comando "exit"
        if (strcmp(command, "exit") == 0) {
            if (test_mode) {
                printf("Shell terminada correctamente\n");
            }
            break;
        }

        // Valido sintaxis
        if (validate_syntax(command) != 0) {
            continue; // Salteo procesamiento si hay error de sintaxis
        }

        // Divido por pipes respetando comillas
        int command_count = split_by_pipes(command, commands, MAX_COMMANDS);

        if (command_count == 0) {
            continue;
        }
        
        // Verifico si hay comandos vacios entre pipes
        int syntax_error = 0;
        for (int i = 0; i < command_count; i++) {
            if (strlen(commands[i]) == 0) {
                fprintf(stderr, "shell: error de sintaxis cerca del token inesperado '|'\n");
                syntax_error = 1;
                break;
            }
        }
        
        if (syntax_error) {
            continue; // Salto al siguiente comando
        }
        
        // Salida de depuracion
        if (debug_mode) {
            for (int i = 0; i < command_count; i++) {
                printf("Comando %d: %s\n", i, commands[i]);
            }
        }

        // Creo todas las pipes a utilizar
        int pipes[command_count - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) < 0){
                perror("error de pipe");
                exit(EXIT_FAILURE);
            }
        }

        // Creo procesos hijos
        pid_t pids[command_count];
        for (int i = 0; i < command_count; i++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("error de fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                // Proceso hijo
                // Configuro redireccion de entrada excepto primer comando
                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }

                // Configuro redireccion de salida excepto ultimo comando
                if (i < command_count - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                // Cierro todos los pipes
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Parseo comando y argumentos con manejo de comillas
                char *args[MAX_ARGS + 1]; // MAX_ARGS + NULL
                char *cmd_copy = strdup(commands[i]);
                int arg_count = parse_args(cmd_copy, args, MAX_ARGS + 1);

                // Verifico errores de parseo de argumentos
                if (arg_count < 0) {
                    exit(EXIT_FAILURE);
                }

                // Ejecuto comando
                if (arg_count > 0 && execvp(args[0], args) < 0) {
                    perror("error en execvp");
                    exit(EXIT_FAILURE);
                }
            } else {
                pids[i] = pid;
            }
        }

        // Cierro todos los extremos de pipe en el proceso padre
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Espero a que todos mis hijos terminen
        for (int i = 0; i < command_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}