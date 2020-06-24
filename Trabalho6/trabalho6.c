// Servidor TCP multi-conexoes - Trabalho6
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

// Recebe o command e executa 'command > file'
// Gerando um arquivo 'file' no diretorio
int terminalcommand(char *command) {
    char comandoTerminal[100] = "";

    strcat(comandoTerminal, command);
    strcat(comandoTerminal, " > ");
    strcat(comandoTerminal, "file");

    status = system(comandoTerminal);

    if (status < 0) {
        perror("ERRO chamada system(): ");
        return (-1);
    } else {
        return (1);
    }
}

/*int transfercommand(char *command, int output_fd) {
    FILE *input_fp;  // input file descriptor
    int status;
    int n;
    char buffer[BUFFERSIZE];
    char str[10];
    // struct stat  statp;

    input_fp = popen(command, "r");
    // TODO: usar fd = fileno(fp) para pegar o file descriptor

    // input_fd = open(path,O_RDONLY);
    if (input_fp < 0) {
        perror("ERRO chamada open(): Erro na abertura do arquivo: ");
        return (-1);
    }

    // // obtem tamanho do arquivo
    // status = fstat(input_fd,&statp);
    // if (status != 0)
    // {
    //     perror("ERRO chamada stat(): Erro no acesso ao arquivo: ");
    //     status = close(input_fd);
    //     return(-1);
    // }
    // sprintf(str,"SIZE=%d\n",statp.st_size);
    // write(output_fd,str,strlen(str));

    // le arquivo , por partes

    while (fgets(buffer, sizeof(buffer), input_fp) != 0) {
        status = write(output_fd, buffer, strlen(buffer) + 1);
    }
    // printf("Foram enviados %d caracteres\n", status);

    status = pclose(input_fp);
    if (status == -1) {
        perror("ERRO: chamada close(): Erro no fechamento do arquivo: ");
        return (-1);
    }
    return (0);
}*/

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
            
            status = write(newsd, "Maximo de conexoes atigindo\0", sizeof("Maximo de conexoes atigindo\0")+1);
            if (status == -1) perror("Erro na chamada read");
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
        //newsd = 0;
        // Pega o item da fila, mas não retira
        newsd = RetirarFila(&F);
 
        //newsd = RetirarFila(&F);
        // processar item
        printf("Thread de tratamento %d consumiu conexao sd = %d\n", id, newsd);
        printf("Tratando conexao...\n\n");
        while (conectado == 1) {
            aguardar = FALSE;

            // Recebendo dados de newsd
            char rxbuffer[80];
            status = read(newsd, rxbuffer, sizeof(rxbuffer));
            if (status == -1) perror("Erro na chamada read");

            printf("Comando Recebido: %s (de sd = %d)\n", rxbuffer, newsd);

            if (strcmp("exit", rxbuffer) == 0) {
                // fecha conexão
                status = write(newsd, "Fechando conexao\0",
                            strlen("Fechando conexao\0") + 1);
                if (status == -1) perror("Erro na chamada write");
                printf("Fechando conexao com sd = %d\n", newsd);
                //SendEND(newsd);
                //printf("\n");
                //if (status == -1) perror("Erro na chamada close");

                conectado = 0;
            } else {
                if (strcmp("date", rxbuffer) == 0) {
                    printf("Enviando data...\n");
                    status = terminalcommand("date");
                    // status = transfercommand("date",newsd);
                    status = transferfile("file", newsd);

                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Data enviada !\n");

                } else if (strcmp("version", rxbuffer) == 0) {
                    printf("Enviando versão...\n");
                    status = transferfile("/proc/version", newsd);
                    // status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Versão enviada !\n");

                } else if (strcmp("cpu", rxbuffer) == 0) {
                    printf("Enviando dados do CPU...\n");
                    status = transferfile("/proc/cpuinfo", newsd);
                    // status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Dados do CPU enviados !\n");

                } else if (strcmp("memory", rxbuffer) == 0) {
                    printf("Enviando dados da memoria...\n");
                    status = transferfile("/proc/meminfo", newsd);
                    // status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Dados da memoria enviadas !\n");

                } else if (strcmp("partitions", rxbuffer) == 0) {
                    printf("Enviando dados das particoes dos discos...\n");
                    status = transferfile("/proc/partitions", newsd);
                    // status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Dados das particoes dos discos enviados !\n");

                } else if (strcmp("interfaces", rxbuffer) == 0) {
                    printf("Enviando interdace da rede...\n");

                    status = terminalcommand("/sbin/ifconfig");
                    status = transferfile("file", newsd);
                    // status = transfercommand("/sbin/ifconfig",newsd);

                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Interface enviada !\n");

                } else if (strcmp("route", rxbuffer) == 0) {
                    printf("Enviando tabela de roteamento...\n");
                    status = transferfile("/proc/route", newsd);
                    // status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1)
                        perror("Erro na chamada write");
                    else
                        printf("Tabela de roteamento enviada !\n");
                } else {
                    status = write(newsd, rxbuffer, strlen(rxbuffer) + 1);
                    if (status == -1) perror("Erro na chamada write");
                }
            }
            //Envia END para client saber que acabou os dados
            SendEND(newsd);
            printf("\n");
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