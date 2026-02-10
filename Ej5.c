#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define N_THREADS_A 4
#define N_THREADS_B 3
#define ITERS 10000
#define ESPERA 10000
#define ESPERA_MONITOR 100000

volatile bool fin = false;

typedef struct {
    volatile int *a;
    volatile int *b;
    int iters;
    int espera;
    int liberar;
    pthread_mutex_t *m_a;
    pthread_mutex_t *m_b;
} args;

void *threads_fun(void *arg) {

    args *p = (args *)arg;
    int i;

    for (i = 0; i < p->iters; i++) {
        pthread_mutex_lock(p->m_a);
        pthread_mutex_lock(p->m_b);
        (*p->a)++;
        (*p->b)--;
        pthread_mutex_unlock(p->m_a);
        pthread_mutex_unlock(p->m_b);
        usleep(p->espera / 1000);
    };

    if (p->liberar) free(p);
    return NULL;
}

void *thread_monitorizacion(void *arg) {

    args *p = (args *)arg;

    while (!fin) {
        pthread_mutex_lock(p->m_a);
        pthread_mutex_lock(p->m_b);
        printf("a = %d\n", *p->a);
        printf("b = %d\n", *p->b);
        printf("Suma = %d\n", *p->a + *p->b);
        pthread_mutex_unlock(p->m_a);
        pthread_mutex_unlock(p->m_b);
        usleep(ESPERA_MONITOR);
    }

    free(p);
    return NULL;
}

int main() {

    pthread_t threadM;
    pthread_t threadsA[N_THREADS_A];
    pthread_t threadsB[N_THREADS_B];
    volatile int a = 0;
    volatile int b = 0;
    int i;
    pthread_mutex_t mutex_a;
    pthread_mutex_t mutex_b;
    pthread_mutex_init(&mutex_a, NULL);
    pthread_mutex_init(&mutex_b, NULL);

    args *pM = malloc(sizeof(args));
    pM->a = &a;
    pM->b = &b;
    pM->iters = ITERS;
    pM->espera = ESPERA;
    pM->liberar = 1;
    pM->m_a = &mutex_a;
    pM->m_b = &mutex_b;

    pthread_create(&threadM, NULL, thread_monitorizacion, pM);

    for (i = 0; i < N_THREADS_A; i++) {
        
        args *p = malloc(sizeof(args));
        p->a = &a;
        p->b = &b;
        p->iters = ITERS;
        p->espera = ESPERA;
        p->liberar = 1;
        p->m_a = &mutex_a;
        p->m_b = &mutex_b;

        pthread_create(&threadsA[i], NULL, threads_fun, p);
    }
    
    args argsB[N_THREADS_B];
    for (i = 0; i < N_THREADS_B; i++) {
        
        argsB[i].a = &a;
        argsB[i].b = &b;
        argsB[i].iters = ITERS;
        argsB[i].espera = ESPERA;
        argsB[i].liberar = 0;
        argsB[i].m_a = &mutex_a;
        argsB[i].m_b = &mutex_b;

        pthread_create(&threadsB[i], NULL, threads_fun, &argsB[i]);
    }

    for (i = 0; i < N_THREADS_A; i++) {
        pthread_join(threadsA[i], NULL);
    }

    for (i = 0; i < N_THREADS_B; i++) {
        pthread_join(threadsB[i], NULL);
    }

    fin = true;
    pthread_join(threadM, NULL);

    printf("Valor final de a: %d\n", a);
    printf("Valor final de b: %d\n", b);

    return 0;
}