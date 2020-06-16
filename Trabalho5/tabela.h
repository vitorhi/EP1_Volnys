#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


// Inclui biblioteca de semaforos
#include <semaphore.h> 

#define MAXUSUARIOS 3

struct nome_address {
	char nome[11];
	struct sockaddr_in ender_socket;
	sem_t sem_mutex;
	sem_t sem_itens;
	sem_t sem_slots;
} ;

struct tabela{
	struct nome_address t[MAXUSUARIOS];
	int n_usuarios;

};

void InitTabela(struct tabela *T);
int TabelaVazia(struct tabela *T);
int TabelaCheia(struct tabela *T);
int InserirTabela(struct tabela *T, struct nome_address n);