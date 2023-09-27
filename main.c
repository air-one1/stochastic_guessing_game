#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    // variables utilizada
    
    int fd[2];
    int fdx[2];
    int status;
    int nread;
    char buffer[100];
    char msg[100];
    int intento;
    int x;
    int j = 0; // Variable para contar el número del inteto.

    srandom(getpid()); //initiliza la semilla para los numeros
    x = (int)(256.0*random()/RAND_MAX); //x sera un numero entre 0 y 256

    sprintf(buffer, "%d", x);
    // creacion de los dos pipes (padre-hijo y hijo-padre)
    if (pipe(fd) == -1 || pipe(fdx) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    printf("Pipe padre-hijo OK!\n");
    printf("Pipe hijo-padre OK!\n");
    // creacion de un proceso hijo
    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            close(fd[1]);
            close(fdx[0]);
            while (1) {
            	// lectura de la informacion contenida en la pipe
                switch(nread = read(fd[0], buffer, sizeof(buffer))) {
                    case -1:
                        perror("read");
                        break;
                    case 0:
                        exit(EXIT_SUCCESS);
                    default:
                        buffer[nread]='\0';
                        // transforma la cadena en numero
                        intento = atoi(buffer);
                        //respuestas del hijo
                        if (intento == x) {
                            printf("Acierto !");
                            close(fd[0]);
                            close(fdx[1]);
                            exit(EXIT_SUCCESS);
                        }
                        else if (intento < x) {
                            sprintf(msg, "Mayor!\n");
                            write(fdx[1], msg, sizeof(msg));
                        }
                        else {
                            sprintf(msg, "Menor!\n");
                            write(fdx[1], msg, sizeof(msg));
                        }
                        break;
                }
            }
       default:
    	   close(fd[0]);
    	   close(fdx[1]);
           while (1) {
           // intento maximum de 100
           	j = j+1;
        	if (j==101){
            	break;
           }
           printf("Intento %d\n", j);
           printf("Introduce un número: ");
           switch(scanf("%d", &intento)){
           // el padre debera advinar el numero
            	case -1:
                	perror("read");
                	break;
            	case 0:
            	// Sólo puedes ingresar un número entero, de lo contrario es el final del juego.
                	printf("No es un entero! END OF GAME\n");
                	close(fd[1]);
                	close(fdx[0]);
                	// el proceso padre espara la finalizacion del proceso hijo
                	wait(&status);
                	exit(EXIT_SUCCESS);
            	default:        
                	sprintf(msg, "%d", intento);
                	write(fd[1], msg, sizeof(msg));
                	// lectura de la informacion contenida en la pipe
                	switch(nread = read(fdx[0], buffer, sizeof(buffer))) {
                    		case -1:
                        		perror("read");
                        		break;
                    		case 0:
                        		exit(EXIT_SUCCESS);
                    		default:
                        		buffer[nread] = '\0';
                        		printf("Hijo dice: %s\n", buffer);
                        		if (strcmp(buffer, "Acierto!") == 0) {
                            			close(fd[1]);
                            			close(fdx[0]);
                            			// el proceso padre espara la finalizacion del proceso hijo
                            			wait(&status);
                            			exit(EXIT_SUCCESS);
                        		}
                        	break;
                	}
            }
          } 
        }
    }
