// arquivo: prodcons.c
// descricao: Programa produtor-consumidor com mutex
// Utiliza a biblioteca pthreads.
// para compilar:  cc -o phtread pthread.c -lpthread

#include <pthread.h>
#include <semaphore.h>


//incluir fila
#include "fila.h"

#define  FALSE 0
#define  TRUE  1


// Declaracao das variaveis de condicao:
// pthread_mutex_t  mutex;


// Declaração da variável de condição “mycondv” pré inicializada
// pthread_cond_t mycondv = PTHREAD_COND_INITIALIZER;


// Declara a fila
struct fila *F;

sem_t slots;
sem_t mutex;
sem_t items;


void produtor(int id) {
	int i=0;
	int item;
    int aguardar;	
    int *val;

	printf("Inicio produtor %d \n",id);
	while (i < 10) {
     	//produzir item
		item = i + (id*1000);
	
    	// pthread_mutex_lock(&mutex);
		// aguardar = FALSE;
		// while(FilaCheia(&F)) {
		//     	aguardar = TRUE;
		// 	pthread_cond_wait(&mycondv, &mutex); 
		//     	//pthread_mutex_unlock(&mutex);
		// }
	    sem_wait(&slots);
	    sem_wait(&mutex);
	    
	    //inserir item
		printf("Produtor %d inserindo item %d\n", id, item); 
	 	InserirFila(&F,item);
	 	sem_post(&mutex);
	 	sem_post(&items);
	 	
		// pthread_cond_signal(&mycondv);  
		// pthread_mutex_unlock(&mutex);
		i++;
		sleep(1);
	}
	printf("Produtor %d terminado \n", id); 
}

void consumidor(int id) {
	int item;
	int aguardar;
	int *val;

	printf("Inicio consumidor %d \n",id);
	while (1) {
	// 	// retirar item da fila
 //   		pthread_mutex_lock(&mutex);
 //        aguardar = FALSE;
 //        while (FilaVazia(&F)){
 //        	aguardar = TRUE;
	// 	//pthread_mutex_unlock(&mutex);
	// 	pthread_cond_wait(&mycondv, &mutex); 
	// }
		sem_wait(&items);
		sem_wait(&mutex);
		
		item = RetirarFila(&F);
		printf("Consumidor %d consumiu item %d\n", id, item);
		// pthread_cond_signal(&mycondv);  
		// pthread_mutex_unlock(&mutex);
		sem_post(&mutex);
		sem_post(&slots);
		
		// processar item
		sleep(1);
	}
	printf("Consumidor %d terminado \n", id); 
}

int main() { 
	pthread_t prod1;
	pthread_t prod2;
	pthread_t prod3;
	pthread_t cons1;
	pthread_t cons2;
	pthread_t cons3;
	InitFila(&F);
	int status_m=0;
	int status_i=0;
	int status_s=0;
	printf("Fila Vazia init = %d\n", FilaVazia(&F));

	printf("Programa Produtor-Consumidor\n");

	printf("Iniciando variaveis de sincronizacao.\n");
	// pthread_mutex_init(&mutex,NULL);
	// pthread_cond_init(&mycondv,NULL);
	status_m = sem_init(&mutex,0,1);
	status_i = sem_init(&items,0,0);
	status_s = sem_init(&slots,0,4);

	
    printf("Disparando threads produtores\n");

	pthread_create(&prod1, NULL, (void*) produtor,1);
	pthread_create(&prod2, NULL, (void*) produtor,2);
	pthread_create(&prod3, NULL, (void*) produtor,3);

    printf("Disparando threads consumidores\n");
	pthread_create(&cons1, NULL, (void*) consumidor,1);
	//pthread_create(&cons2, NULL, (void*) consumidor,2);
	//pthread_create(&cons2, NULL, (void*) consumidor,3);

	pthread_join(prod1,NULL);
	pthread_join(prod2,NULL);
	pthread_join(prod3,NULL);
	pthread_join(cons1,NULL);
	//pthread_join(cons2,NULL);
	//pthread_join(cons3,NULL);
	
    	printf("Terminado processo Produtor-Consumidor.\n\n");
}
