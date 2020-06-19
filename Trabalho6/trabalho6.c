#include <netdb.h>  //connect()
#include <stdio.h>
#include <sys/socket.h>  //socket()
#include <unistd.h>      //read

#define QLEN 10  // Quantidade maxima de conexoes pendentes

int main() {
    int sd;  // Socket descriptor
    int status;
    int myport;

    struct sockaddr_in mylocal_addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Erro na chamada socket");
    }

    printf("Servidor TCP\n");
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

    // Aceita chamada de um client
    int newsd;
    int size;
    struct sockaddr_in clientaddr;

    newsd = accept(sd, (struct sockaddr *)&clientaddr, (socklen_t *)&size);
    if (newsd < 0) {
        perror("Erro na chamada acept");
    }

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

    //fecha conexão
    status = close(newsd);
    if (status == -1) perror("Erro na chamada close");
}