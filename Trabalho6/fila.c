// arquivo: fila.c
//    Este arquivo deve ser compilado junto com o programa principal:
//        cc -o prog prog.c fila.c

#include "fila.h"

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

void InitFila(struct fila *F, int nMax) {
    F->inicio = 0;
    F->fim = 0;
    F->nitens = 0;
    F->tammax = nMax;
    sem_init(&F->sem_mutex, 0, 1);
    sem_init(&F->sem_itens, 0, 0);
    sem_init(&F->sem_slots, 0, nMax);
}

int FilaVazia(struct fila *F) {
    if (F->nitens == 0)
        return (TRUE);
    else
        return (FALSE);
}

int FilaCheia(struct fila *F) {
    if (F->nitens == F->tammax) {
        return (TRUE);
    } else
        return (FALSE);
}

void InserirFila(struct fila *F, int item) {
    sem_wait(&F->sem_slots);
    sem_wait(&F->sem_mutex);
    if (FilaCheia(F)) {
        printf("ERRO: tentando inserir em fila cheia. \n");
        exit(1);
    }
    F->buffer[F->fim] = item;
    F->fim = (F->fim + 1) % F->tammax;
    F->nitens = F->nitens + 1;
    sem_post(&F->sem_itens);
    sem_post(&F->sem_mutex);
}

int RetirarFila(struct fila *F) {
    int item;
    sem_wait(&F->sem_itens);
    sem_wait(&F->sem_mutex);
    if (FilaVazia(F)) {
        printf("ERRO: tentando retirar de fila vazia. \n");
        exit(1);
    }
    item = F->buffer[F->inicio];
    F->inicio = (F->inicio + 1) % F->tammax;
    F->nitens = F->nitens - 1;
    sem_post(&F->sem_slots);
    sem_post(&F->sem_mutex);
    return (item);
}

int RetirarFilaSemRemover(struct fila *F) {
    int item;
    if (FilaVazia(F)) {
        printf("ERRO: tentando retirar de fila vazia. \n");
        exit(1);
    }
    item = F->buffer[F->inicio];
    return (item);
}

void PrintaBuffer(struct fila *F) {
    int i;
    printf("\n");
    printf("N items: %d\n", F->nitens);
    for (i = 0; i < F->nitens; i++) {
        printf("Item %d: %d\n", i, F->buffer[i]);
    }
}