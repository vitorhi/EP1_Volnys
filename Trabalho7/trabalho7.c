// Servidor TCP multi-conexoes - Trabalho7
// Para compilar: cc -o trabalho6 trabalho6.c fila.c -lpthread transferfile.c

#include <netdb.h>    //connect()
#include <pthread.h>  //Biblioteca para multi-thread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  //socket()
#include <unistd.h>      //read
// #include <fcntl.h>

#include "fila.h"

#define QLEN 100  // Quantidade limite de conexoes pendentes
#define TRUE 1
#define FALSE 0

#define BUFFERSIZE 1024
#define LINESIZE 80
#define PATHSIZE 80

struct fila F;

int sd;  // Socket descriptor
int status;
int myport;
int n_conex; //numero limite de conexoes

struct sockaddr_in mylocal_addr;

int transferfile(char *path, int output_fd);

int SendEND(int newsd) {
    status = write(newsd, "END:\0", strlen("END:\0") + 1);
    if (status == -1)
        perror("Erro na chamada write");
    else
        printf("END enviado\n");
}

void produtor(int id) {
    int newsd;
    int size;
    struct sockaddr_in clientaddr;
    printf("Inicio Thread principal %d \n", id);

    while (1) {
        newsd = accept(sd, (struct sockaddr *)&clientaddr, (socklen_t *)&size);
        printf("\nRecebi uma conexao: sd = %d\n", newsd);

        if (FilaCheia(&F)) {
            printf("\nNumero maximo de conexoes atingidas\n");
            // TODO: Mandar BAD REQUEST

            // status = write(newsd, "Maximo de conexoes atigindo\0", sizeof("Maximo de conexoes atigindo\0")+1);
            // if (status == -1) perror("Erro na chamada read");
        } else {
            InserirFila(&F, newsd);
        }

        if (newsd < 0) {
            perror("Erro na chamada accept");
            exit(1);
        }
    }
}

void consumidor(int id) {
    int newsd = 0;
    int aguardar;
    int conectado = 1;
    FILE *fp;
    printf("Inicio Thread de tratamento %d \n", id);

    while(1){
        // Pega o item da fila, mas não retira
        newsd = RetirarFila(&F);
 
        // processar item
        printf("Thread de tratamento %d consumiu conexao sd = %d\n", id, newsd);
        printf("Tratando conexao...\n\n");
        while (conectado == 1) {
            aguardar = FALSE;

            // Recebendo dados de newsd
            char rxbuffer[80];
            status = read(newsd, rxbuffer, sizeof(rxbuffer));
            if (status == -1) perror("Erro na chamada read");

            
            
            printf("Recebido (de sd = %d):\n\n  %s",newsd,rxbuffer);
            if (strncmp(rxbuffer,"GET",3))
            {
                // Envia header com mensagem OK
                status = write(newsd, "HTTP/1.0 200 OK\r\nServer: WEB\r\n", strlen("HTTP/1.0 200 OK\r\nServer: WEB\r\n"));
                if (status == -1) perror("Erro na chamada write");

                // Transfere arquivo html
                transferfile("diretorio/index.html",newsd);

                // Final da mensagem, enviar quebras de linha
                status = write(newsd, "\r\n\r\n", strlen("\r\n\r\n"));   
            }
            
            
        }
        status = close(newsd);
        printf("Thread de tratamento %d terminado \n", id);
        printf("Aguardando novas conexoes... \n");
        conectado = 1;
    }
}

int main() {
    printf("Servidor TCP multi-thread\n");

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Erro na chamada socket");
    }
    myport=8080;
    
    printf("Porta de escuta: %d\n",myport);

    mylocal_addr.sin_family = AF_INET;
    mylocal_addr.sin_addr.s_addr = INADDR_ANY;
    mylocal_addr.sin_port = htons(myport);

    status =
        bind(sd, (struct sockaddr *)&mylocal_addr, sizeof(struct sockaddr_in));
    if (status == -1) perror("Erro na chamada bind");

    status = listen(sd, QLEN);
    if (status == -1) perror("Erro na chamada listen");

    // Define o numero maximo de conexoes simultaneas
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