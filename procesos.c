#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>


int main() {
    double x;
    printf("Ingrese el valor de X: ");
    scanf("%lf", &x);
    
    // Tuberías para enviar x a los hijos
    int pipe1[2], pipe2[2]; 
    // Tuberías para recibir resultados de los hijos
    int pipe1_resultado[2], pipe2_resultado[2];
    
    // Crear todas las tuberías necesarias
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || 
        pipe(pipe1_resultado) == -1 || pipe(pipe2_resultado) == -1) {
        perror("Error al crear pipes");
        exit(1);
    }
    
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error al crear el primer hijo");
        exit(1);
    }
    
    if (pid1 == 0) {
        // Primer hijo: Recibe x, calcula (x - 5) y lo envía de vuelta
        close(pipe1[1]); // Cierra escritura entrada
        close(pipe1_resultado[0]); // Cierra lectura resultado
        close(pipe2[0]); // Cierra descriptores no usados
        close(pipe2[1]);
        close(pipe2_resultado[0]);
        close(pipe2_resultado[1]);
        
        double x_hijo;
        read(pipe1[0], &x_hijo, sizeof(x_hijo)); // Lee x enviado por el padre
        close(pipe1[0]); // Cierra lectura
        
        double resultado = x_hijo - 5; // Realiza la operación
        
        // Envía el resultado al padre
        write(pipe1_resultado[1], &resultado, sizeof(resultado));
        close(pipe1_resultado[1]); // Cierra escritura
        
        exit(0);
    }
    
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Error al crear el segundo hijo");
        exit(1);
    }
    
    if (pid2 == 0) {
        // Segundo hijo: Recibe x, calcula (2 + sin(x)) y lo envía de vuelta
        close(pipe2[1]); // Cierra escritura entrada
        close(pipe2_resultado[0]); // Cierra lectura resultado
        close(pipe1[0]); // Cierra descriptores no usados
        close(pipe1[1]);
        close(pipe1_resultado[0]);
        close(pipe1_resultado[1]);
        
        double x_hijo;
        read(pipe2[0], &x_hijo, sizeof(x_hijo)); // Lee x enviado por el padre
        close(pipe2[0]); // Cierra lectura
        
        double resultado = 2 + sin(x_hijo); // Requiere compilar con -lm
        
        // Envía el resultado al padre
        write(pipe2_resultado[1], &resultado, sizeof(resultado));
        close(pipe2_resultado[1]); // Cierra escritura
        
        exit(0);
    }
    
    // Proceso padre
    // Cierra los extremos de las tuberías que no se usarán
    close(pipe1[0]); // Cierra lectura de entrada
    close(pipe2[0]); // Cierra lectura de entrada
    close(pipe1_resultado[1]); // Cierra escritura de resultado
    close(pipe2_resultado[1]); // Cierra escritura de resultado
    
    // Envía x a los hijos
    write(pipe1[1], &x, sizeof(x)); // Envía x al primer hijo
    write(pipe2[1], &x, sizeof(x)); // Envía x al segundo hijo
    
    // Cierra escritura después de enviar los datos
    close(pipe1[1]);
    close(pipe2[1]);
    
    // Espera a que los hijos terminen
    wait(NULL);
    wait(NULL);
    
    // Recibe los resultados
    double numerador, denominador;
    read(pipe1_resultado[0], &numerador, sizeof(numerador));
    read(pipe2_resultado[0], &denominador, sizeof(denominador));
    
    // Cierra lectura después de recibir los datos
    close(pipe1_resultado[0]);
    close(pipe2_resultado[0]);
    
    // Verifica que el denominador no sea 0 antes de dividir
    if (denominador != 0) {
        double resultado_final = fabs(numerador / denominador);
        printf("Resultado final |(x - 5) / (2 + sin(x))| = %lf\n", resultado_final);
    } else {
        printf("Error: División por cero\n");
    }
    
    return 0;
}
