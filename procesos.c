#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

int main() {
    int pipe_padre_hijo1[2];
    int pipe_hijo1_padre[2];
    int pipe_padre_hijo2[2];
    int pipe_hijo2_padre[2];
    pid_t pid_hijo1, pid_hijo2;
    double x, resultado_hijo1, resultado_hijo2, resultado_final;

    if (pipe(pipe_padre_hijo1) == -1 || pipe(pipe_hijo1_padre) == -1 ||
        pipe(pipe_padre_hijo2) == -1 || pipe(pipe_hijo2_padre) == -1) {
        return 1;
    }

    printf("Introduce el valor de X: ");
    scanf("%lf", &x);

    pid_hijo1 = fork();
    if (pid_hijo1 == -1) {
        return 1;
    }

    if (pid_hijo1 == 0) {
        close(pipe_padre_hijo1[1]);
        close(pipe_hijo1_padre[0]);
        close(pipe_padre_hijo2[0]);
        close(pipe_padre_hijo2[1]);
        close(pipe_hijo2_padre[0]);
        close(pipe_hijo2_padre[1]);

        read(pipe_padre_hijo1[0], &x, sizeof(x));
        resultado_hijo1 = x - 5;
        printf("Hijo 1: X - 5 = %lf\n", resultado_hijo1);
        write(pipe_hijo1_padre[1], &resultado_hijo1, sizeof(resultado_hijo1));

        close(pipe_padre_hijo1[0]);
        close(pipe_hijo1_padre[1]);
        exit(0);
    }

    pid_hijo2 = fork();
    if (pid_hijo2 == -1) {
        return 1;
    }

    if (pid_hijo2 == 0) {
        close(pipe_padre_hijo2[1]);
        close(pipe_hijo2_padre[0]);
        close(pipe_padre_hijo1[0]);
        close(pipe_padre_hijo1[1]);
        close(pipe_hijo1_padre[0]);
        close(pipe_hijo1_padre[1]);

        read(pipe_padre_hijo2[0], &x, sizeof(x));
        resultado_hijo2 = 2 + sin(x);
        printf("Hijo 2: 2 + sin(X) = %lf\n", resultado_hijo2);
        write(pipe_hijo2_padre[1], &resultado_hijo2, sizeof(resultado_hijo2));

        close(pipe_padre_hijo2[0]);
        close(pipe_hijo2_padre[1]);
        exit(0);
    }

    close(pipe_padre_hijo1[0]);
    close(pipe_hijo1_padre[1]);
    close(pipe_padre_hijo2[0]);
    close(pipe_hijo2_padre[1]);

    write(pipe_padre_hijo1[1], &x, sizeof(x));
    write(pipe_padre_hijo2[1], &x, sizeof(x));

    read(pipe_hijo1_padre[0], &resultado_hijo1, sizeof(resultado_hijo1));
    read(pipe_hijo2_padre[0], &resultado_hijo2, sizeof(resultado_hijo2));

    resultado_final = fabs(resultado_hijo1 / resultado_hijo2);
    printf("Padre: |(%lf) / (%lf)| = %lf\n", resultado_hijo1, resultado_hijo2, resultado_final);
    printf("Resultado final = %lf\n", resultado_final);

    wait(NULL);
    wait(NULL);

    close(pipe_padre_hijo1[1]);
    close(pipe_hijo1_padre[0]);
    close(pipe_padre_hijo2[1]);
    close(pipe_hijo2_padre[0]);

    return 0;
}
