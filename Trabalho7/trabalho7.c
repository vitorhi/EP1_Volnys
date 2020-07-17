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

// Declara numero da porta e nome da pasta diretorio
int PORTA=8080;
char BASE[]="diretorio";

struct fila F;

int sd;  // Socket descriptor
int status;

int n_conex; //numero limite de conexoes


struct sockaddr_in mylocal_addr;

int transferfile(char *path, int output_fd);

void produtor(int id) {
    int newsd;
    int size;
    struct sockaddr_in clientaddr;
    printf("Inicio Thread principal %d \n", id);

    while (1) {
        newsd = accept(sd, (struct sockaddr *)&clientaddr, (socklen_t *)&size);
        printf("\nRecebi uma conexao: sd = %d\n", newsd);
        printf("A fila possui %d elementos\n",F.nitens );

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
    char path_file[PATHSIZE];

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

            // Separa a string nos caracteres "espaco"
            char *ptr = strtok(rxbuffer," ");

            printf("\n\nRecebido (de sd = %d):\n\n  %s",newsd,ptr);
            if (strcmp(ptr,"GET")==0)
            {   
                // Separa o caminho do arquivo do resto do header
                // e coloca em ptr
                ptr = strtok(NULL, " ");
                printf("\nptr: %s\n",ptr );

                if(ptr!=NULL)
                {
                    // Copia diretorio para path_file
                    strcpy(path_file, BASE);
                    printf("path_file: %s\n",path_file );
                   
                    // Concatena com o caminho do arquivo
                    if(strcmp(ptr,"/")==0)
                    {
                        strcat(path_file,"/index.html");
                    }
                    else 
                    {
                        strcat(path_file,ptr);
                    }
                   
                    printf("path_file_2: %s\n",path_file );

                    // Envia header com mensagem OK
                    status = write(newsd, "HTTP/1.0 200 OK\r\nServer: WEB\r\n", strlen("HTTP/1.0 200 OK\r\nServer: WEB\r\n"));
                    if (status == -1) perror("Erro na chamada write");

                    // Transfere arquivo html
                    status=transferfile(path_file,newsd);
                    if (status == -1) perror("Erro na chamada transferfile");

                    // Final da mensagem, enviar quebras de linha
                    status = write(newsd, "\r\n", strlen("\r\n"));   
                }

                
                conectado=0;
            }
            
            
        }
        // status = close(newsd);
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
    
    
    printf("Porta de escuta: %d\n",PORTA);

    mylocal_addr.sin_family = AF_INET;
    mylocal_addr.sin_addr.s_addr = INADDR_ANY;
    mylocal_addr.sin_port = htons(PORTA);

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