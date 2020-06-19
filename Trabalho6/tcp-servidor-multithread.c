// Servidor TCP multi-conexoes - Trabalho6
// Para compilar: cc -o trabalho6 trabalho6.c fila.c -lpthread

#include <netdb.h>    //connect()
#include <pthread.h>  //Biblioteca para multi-thread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  //socket()
#include <unistd.h>      //read

#include "fila.h"

#define QLEN 100  // Quantidade limite de conexoes pendentes
#define TRUE 1
#define FALSE 0

struct fila F;

int sd;  // Socket descriptor
int status;
int myport;

struct sockaddr_in mylocal_addr;

void produtor(int id) {
    int newsd;
    int size;
    struct sockaddr_in clientaddr;
    printf("Inicio Thread principal %d \n", id);

    while (1) {
        newsd = accept(sd, (struct sockaddr *)&clientaddr, (socklen_t *)&size);
        printf("\nRecebi uma conexao: sd = %d\n", newsd);

        if (FilaCheia(&F)) {
            printf("Numero maximo de conexoes atingidas\n");
        } else {
            InserirFila(&F, newsd);
        }

        if (newsd < 0) {
            perror("Erro na chamada acept");
            exit(1);
        }
    }
}

void consumidor(int id) {
    int newsd;
    int aguardar;
    int conectado = 1;
    printf("Inicio Thread de tratamento %d \n", id);
    while (1) {
        // retirar item da fila
        aguardar = FALSE;
        newsd = RetirarFila(&F);

        // processar item
        printf("Thread de tratamento %d consumiu conexao %d\n", id, newsd);
        printf("Tratando conexao...\n");

        // Recebendo dados de newsd
        char rxbuffer[80];

        status = read(newsd, rxbuffer, sizeof(rxbuffer));
        if (status == -1) perror("Erro na chamada read");

        printf("Mensagem Recebida: %s\n", rxbuffer);

        // Enviando mensagem para newsd
        char txbuffer[80];
        printf("Digite a mensagem a ser enviada para client: ");
        scanf("%s", txbuffer);

        status = write(newsd, txbuffer, strlen(txbuffer) + 1);

        if (status == -1) perror("Erro na chamada write");

        // fecha conexão
        status = close(newsd);
        if (status == -1) perror("Erro na chamada close");

        conectado = 0;
    }
    printf("Consumidor %d terminado \n", id);
}

int main() {
    printf("Servidor TCP multi-thread\n");

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Erro na chamada socket");
    }

    printf("Digite a porta de escuta: ");
    scanf("%d", &myport);

    mylocal_addr.sin_family = AF_INET;
    mylocal_addr.sin_addr.s_addr = INADDR_ANY;
    mylocal_addr.sin_port = htons(myport);

    status =
        bind(sd, (struct sockaddr *)&mylocal_addr, sizeof(struct sockaddr_in));
    if (status == -1) perror("Erro na chamada bind");

    status = listen(sd, QLEN);
    if (status == -1) perror("Erro na chamada listen");

    // Define o numero maximo de conexoes simultaneas
    int n_conex;
    printf("Digite a quantidade maxima de conexoes simultaneas: ");
    scanf("%d", &n_conex);

    InitFila(&F, n_conex);

    pthread_t prod1;
    pthread_t cons[QLEN];

    printf("\nDisparando thread principal de recepcao\n");
    int prod_n = 1;
    pthread_create((pthread_t *)&prod1, NULL, (void *)produtor, (int *)1);

    printf("Disparando threads de tratamento de conexao\n");
    int i;
    for (i = 0; i < n_conex; i++) {
        pthread_create((pthread_t *)&cons[i], NULL, (void *)consumidor, i + 1);
    }

    pthread_join(prod1, NULL);
    for (i = 0; i < n_conex; i++) {
        pthread_join((pthread_t)&cons[i], NULL);
    }

    printf("Terminado processo Produtor-Consumidor.\n\n");
}

/*/ Aceita chamada de um client
    int newsd;
    int size;
    struct sockaddr_in clientaddr;
    printf("Esperando conexao...\n");
    newsd = accept(sd, (struct sockaddr *)&clientaddr, (socklen_t *)&size);
    if (newsd < 0) {
        perror("Erro na chamada acept");
    }
    printf("Conexao realizada...%d\n", newsd);
    // Recebendo dados de newsd
    char rxbuffer[80];

    status = read(newsd, rxbuffer, sizeof(rxbuffer));
    if (status == -1) perror("Erro na chamada read");

    printf("Mensagem Recebida: %s\n", rxbuffer);

    // Enviando mensagem para newsd
    char txbuffer[80];
    printf("Digite a mensagem a ser enviada para client: ");
    scanf("%s", txbuffer);

    status = write(newsd, txbuffer, strlen(txbuffer) + 1);

    if (status == -1) perror("Erro na chamada write");

    // fecha conexão
    status = close(newsd);
    if (status == -1) perror("Erro na chamada close");*/