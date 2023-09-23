#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    int fdx[2];
    int status;
    int nread;
    char buffer[100];
    char msg[100];
    int intento;
    int x;

    srandom(getpid());
    x = (int)(256.0*random()/RAND_MAX);

    sprintf(buffer, "%d", x);

    if (pipe(fd) == -1 || pipe(fdx) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    printf("Pipe padre-hijo OK!\n");
    printf("Pipe hijo-padre OK!\n");

    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            close(fd[1]);
            close(fdx[0]);
            while (1) {
                switch(nread = read(fd[0], buffer, sizeof(buffer))) {
                    case -1:
                        perror("read");
                        break;
                    case 0:
                        perror("EOF");
                        break;
                    default:
                        buffer[nread]='\0';
                        intento = atoi(buffer);
                        if (intento == x) {
                            sprintf(msg, "Acierto!");
                            write(fdx[1], msg, sizeof(msg));
                            close(fd[0]);
                            close(fdx[1]);
                            exit(EXIT_SUCCESS);
                        }
                        else if (intento < x) {
                            sprintf(msg, "Mayor!");
                            write(fdx[1], msg, sizeof(msg));
                        }
                        else {
                            sprintf(msg, "Menor!");
                            write(fdx[1], msg, sizeof(msg));
                        }
                        break;
                }
            }
        default:
            close(fd[0]);
            close(fdx[1]);

            while (1) {
                printf("Introduce un número: ");
                scanf("%d", &intento);
                sprintf(msg, "%d", intento);
                write(fd[1], msg, sizeof(msg));

                switch(nread = read(fdx[0], buffer, sizeof(buffer))) {
                    case -1:
                        perror("read");
                        break;
                    case 0:
                        perror("EOF");
                        break;
                    default:
                        buffer[nread] = '\0';
                        printf("Hijo dice: %s\n", buffer);
                        if (strcmp(buffer, "Acierto!") == 0) {
                            close(fd[1]);
                            close(fdx[0]);
                            wait(&status);
                            exit(EXIT_SUCCESS);
                        }
                        break;
                }
            }
    }
}
