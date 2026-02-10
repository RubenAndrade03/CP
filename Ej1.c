//INTEGRANTES DEL GRUPO
//Rubén Andrade Abeijón - ruben.andrade@udc.es
//Manuel Taibo González - manuel.taibo2@udc.es

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define N_THREADS_A 4
#define ITERS 10000
#define ESPERA 1000

typedef struct {
    volatile int *a;
    volatile int *b;
    int iters;
    int espera;
} args;

void *thread_tipo_a(void *arg) {

    args *p = (args *)arg;
    int i;

    for (i = 0; i < p->iters; i++) {
        (*p->a)++;
        (*p->b)--;
        usleep(p->espera / 1000);
    };

    free(p);
    return NULL;
}

int main() {

    pthread_t threads[N_THREADS_A];
    volatile int a = 0;
    volatile int b = 0;
    int i;

    for (i = 0; i < N_THREADS_A; i++) {
        
        args *p = malloc(sizeof(args));
        p->a = &a;
        p->b = &b;
        p->iters = ITERS;
        p->espera = ESPERA;

        pthread_create(&threads[i], NULL, thread_tipo_a, p);
    }

    for (i = 0; i < N_THREADS_A; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Valor final de a: %d\n", a);
    printf("Valor final de b: %d\n", b);

    return 0;
}