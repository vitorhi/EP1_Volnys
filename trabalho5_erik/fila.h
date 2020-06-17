// Arquivo: fila.h
//    Este arquivo deve ser incluido no programa principal da seguinte forma:
//    #include "fila.h"
//

// Inclui biblioteca de semaforos
#include <semaphore.h> 


// Declaracao da fila

#define TAMMAXFILA 4

struct fila {
	int buffer[TAMMAXFILA];
	int inicio;
	int fim;
	int nitens;
	int tammax;
	sem_t sem_mutex;
	sem_t sem_itens;
	sem_t sem_slots;
} ;

void InitFila(struct fila *F);
int  FilaVazia(struct fila *F);
int  FilaCheia(struct fila *F);
void InserirFila(struct fila *F, int item);
int  RetirarFila(struct fila *F);

	
