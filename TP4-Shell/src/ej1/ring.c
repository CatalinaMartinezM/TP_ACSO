#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define PREV(i, n) ((i - 1 + n) % n)

int main(int argc, char **argv){	
	int start, pid, n;
	int buffer[1];

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    /* Parsing of arguments */
	start = atoi(argv[3]); // Mantener indexado desde 0
	buffer[0] = atoi(argv[2]);
	n = atoi(argv[1]);

	if (start < 0 || start >= n || n <= 0){
		fprintf(stderr, "Error: Argumentos ingresados inválidos \n");
		exit(EXIT_FAILURE);
	}

    // Si stderr no está redirigido
    if (isatty(STDERR_FILENO)) {
        printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    }
    
	// Creo los pipes a utilizar
	int pipes[n][2];
	for (int i = 0; i < n; i++){
		if (pipe(pipes[i]) < 0){
			fprintf(stderr, "Error en pipe \n");
			exit(EXIT_FAILURE);
		}
	}

	pid_t hijos[n]; // Para guardar el PID de los hijos

	for (int i = 0; i < n; i++){
		pid = fork();
		if (pid < 0){
			fprintf(stderr, "Error en fork \n");
			exit(EXIT_FAILURE);
		}

		// Proceso hijo
		if (pid == 0){
			// Cierro todos los pipes que no voy a usar
			for(int j = 0; j < n; j++){
				if (j != PREV(i, n)) close(pipes[j][0]); // No voy a leer de estos pipes
				if (j != i) 		 close(pipes[j][1]); // No voy a escribir en estos pipes
			}

			if(i != start){
				if (read(pipes[PREV(i, n)][0], buffer, sizeof(buffer)) == -1){
					perror("Error al leer en hijo");
					exit(EXIT_FAILURE);
				}
				close(pipes[PREV(i, n)][0]);
			}

			buffer[0]++;

			if (write(pipes[i][1], buffer, sizeof(buffer)) == -1) {
				perror("Error al escribir en hijo");
				exit(EXIT_FAILURE);
			}
			close(pipes[i][1]);
			exit(EXIT_SUCCESS);
		} else {
			// Guardo PID en el proceso padre
			hijos[i] = pid;
		}
	}

	// Proceso padre
	// Cierro pipes que no voy a usar
	for(int i = 0; i < n; i++){
		if (i != PREV(start, n)) close(pipes[i][0]);
		close(pipes[i][1]);
	}

	// Espero a todos mis hijos
	for (int k = 0; k < n; k++){
		waitpid(hijos[k], NULL, 0);
	}

	if (read(pipes[PREV(start, n)][0], buffer, sizeof(buffer)) == -1) {
		perror("Error al leer en padre");
		exit(EXIT_FAILURE);
	}
	close(pipes[PREV(start, n)][0]);

    // Verifico si stderr esta redirigido
    if (!isatty(STDERR_FILENO)) {
        printf("%i\n", buffer[0]);
    } else {
        printf("Caracter recibido por el padre: %i\n", buffer[0]);
    }
	return EXIT_SUCCESS;
}
