// Servidor TCP multi-conexoes - Trabalho7
// Para compilar: cc -o trabalho6 trabalho6.c fila.c sensor.c gera_html.c
// listdir2.h -lpthread transferfile.c

#include <netdb.h>   //connect()
#include <pthread.h> //Biblioteca para multi-thread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //socket()
#include <unistd.h>     //read
// #include <fcntl.h>

#include "fila.h"
#include "gera_html.h"
#include "listdir2.h"
#include "sensor.h"

#define QLEN 100 // Quantidade limite de conexoes pendentes
#define TRUE 1
#define FALSE 0

#define BUFFERSIZE 1024
#define LINESIZE 80
#define PATHSIZE 80
#define AUTOMATICO 0
#define MANUAL 1

struct fila F;

int sd; // Socket descriptor
int status;
int myport;
int n_conex; // numero limite de conexoes
const char dir_path[] = "diretorio";

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
    printf("A fila possui %d elementos\n", F.nitens);

    if (FilaCheia(&F)) {
      printf("\nNumero maximo de conexoes atingidas\n");
      // TODO: Mandar BAD REQUEST

      // status = write(newsd, "Maximo de conexoes atigindo\0", sizeof("Maximo
      // de conexoes atigindo\0")+1); if (status == -1) perror("Erro na chamada
      // read");
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
  int intensidade = 50;
  int HTML_CUSTOMIZADO=0;

  while (1) {
    newsd = RetirarFila(&F);

    // processar item
    printf("Thread de tratamento %d consumiu conexao sd = %d\n", id, newsd);
    printf("Tratando conexao...\n\n");
    while (conectado == 1) {
      aguardar = FALSE;

      // Recebendo dados de newsd
      char rxbuffer[80];

      status = read(newsd, rxbuffer, sizeof(rxbuffer));
      if (status == -1)
        perror("Erro na chamada read");

      // Separa a string nos caracteres "espaco"
      char *ptr = strtok(rxbuffer, " ");

      printf("\n\nRecebido (de sd = %d):\n\n  %s", newsd, ptr);
      if (strcmp(ptr, "GET") == 0) {
        ptr = strtok(NULL, " ");
        printf("\nptr: %s\n", ptr);
        if (ptr != NULL) {
          // Copia diretorio para path_file
          path_file[0] = '\0';
          strcpy(path_file, dir_path);

          // Cria buffer com o caminho
          char buffer_lum[30];
          strcpy(buffer_lum,ptr);
          // Separa a string nos caracteres "=" e pega a intensidade          
          char *ptr_lum= strtok(buffer_lum, "=");
          ptr_lum = strtok(NULL, "=");
          printf("\nptr_lum:%s\n", ptr_lum);
          
          // Salva a intensidade
          if (ptr_lum!= NULL){
            intensidade=atoi(ptr_lum);
            HTML_CUSTOMIZADO=1;
          }

          // printf("dir_file: %s\n", dir_path);
          // printf("path_file: %s\n", path_file);

          // Concatena com o caminho do arquivo
          if (ptr[strlen(ptr) - 1] == '/') {
            char line[LINESIZE];
            char path[PATHSIZE];
            char string[LINESIZE];
            char listbuffer[LISTBUFFERSIZE];
            char listdir[PATHSIZE];

            // Criacao do HTML
            char html[10000];
            int luminosidade;
            
            int modo = MANUAL;
            luminosidade = valorsensor();
            gera_html(html, dir_path, modo, intensidade, luminosidade);

            strcpy(listdir, strcat(path_file, ptr));
            lista_diretorio(listdir, listbuffer, LISTBUFFERSIZE);

            // Procura index.html na lista
            if (strstr(listbuffer, "index.html") !=
                NULL) { // Achou index.html na lista
              strcat(path_file, "index.html");
            } else {
              // TODO: CRIAR ARQUIVO HTML com hyperlinks para os arquivos
              printf("Nao encontrei o arquivo 'index.html\n\n");
              // printf("Criando arquivo...");
            }
          } else {
            strcat(path_file, ptr);
            // printf("Nao e diretorio... \n");
          }

          printf("path_file_2: %s\n", path_file);

          // Envia header com mensagem OK
          status = write(newsd, "HTTP/1.0 200 OK\r\nServer: WEB\r\n",
                         strlen("HTTP/1.0 200 OK\r\nServer: WEB\r\n"));
          if (status == -1)
            perror("Erro na chamada write");

          status = transferfile(path_file, newsd);
          if (status == -1)
            perror("Erro na chamada transferfile");

          // Retornar ao html normal
          HTML_CUSTOMIZADO=0;
          // Final da mensagem, enviar quebras de linha
          status = write(newsd, "\r\n", strlen("\r\n"));
        } else {
          status = write(
              newsd,
              "HTTP/1.0 505 HTTP Version Not Supported\r\nServer: WEB\r\n",
              strlen("HTTP/1.0 505 HTTP Version Not Supported\r\nServer: "
                     "WEB\r\n"));
          if (status == -1)
            perror("Erro na chamada write");
        }

        conectado = 0;
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
  myport = 8080;

  printf("Porta de escuta: %d\n", myport);

  mylocal_addr.sin_family = AF_INET;
  mylocal_addr.sin_addr.s_addr = INADDR_ANY;
  mylocal_addr.sin_port = htons(myport);

  status =
      bind(sd, (struct sockaddr *)&mylocal_addr, sizeof(struct sockaddr_in));
  if (status == -1)
    perror("Erro na chamada bind");

  status = listen(sd, QLEN);
  if (status == -1)
    perror("Erro na chamada listen");

  // Define o numero maximo de conexoes simultaneas
  printf("Digite a quantidade maxima de conexoes simultaneas: ");
  scanf("%d", &n_conex);

  InitFila(&F, n_conex);
  initsensor();

  pthread_t prod1;
  pthread_t cons[QLEN];
  pthread_t threademulador;

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

  pthread_join(threademulador, NULL);

  printf("Terminado processo Produtor-Consumidor.\n\n");
}