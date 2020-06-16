#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
extern int    errno;

#define IPMAXSTRSIZE  16

//##########################################################
// main
//##########################################################

#define  BUFFERSIZE  80
#define  MSGMAXSIZE     512 

int main()
{
    int                sd;                 // socket descriptor  
    int                status;             // estado
    int                size;
    char               buffer[BUFFERSIZE]; // buffer temporario
    char               msg[MSGMAXSIZE];    // buffer temporario
    char               command_str[5];     // comando recebido
    int                serverport;         // porta (formato host)
    struct sockaddr_in mysocketaddr;
    struct sockaddr_in fromaddr;
    struct sockaddr_in socket_a;
    struct hostent   * hostentryp;         // host entry (p/ traducao nome<->ip)
    struct servent   * serventryp;         // host entry (p/ traducao nome<->ip)

    mysocketaddr.sin_family = AF_INET;
    mysocketaddr.sin_addr.s_addr   = INADDR_ANY;

    //####################
    // servico/porta  
    //####################
    serverport=10000;
    mysocketaddr.sin_port = htons((unsigned short int) serverport);
    //####################
    // imprime IP e porta  
    //####################
    printf("Endereco IP do servidor = %s \n",
        inet_ntop(AF_INET,&mysocketaddr.sin_addr,buffer,BUFFERSIZE));
    printf("Porta do servidor = %hu \n",ntohs(mysocketaddr.sin_port));

    /****************************************************************/
    /* Socket(): Criacao do socket                                  */
    /****************************************************************/
    sd = socket(PF_INET,SOCK_DGRAM,0); 
    if (sd < 0)
    {
        printf("Erro na criacao do socket. \n");
        perror("Descricao do erro");
        exit(1);
    }

    /****************************************************************/
    /* Bind(): associa o socket a um IP e a uma porta               */
    /****************************************************************/
    status=bind(sd,(struct sockaddr *)&mysocketaddr,sizeof(struct sockaddr_in));
    if (status < 0)
    {
        perror("Erro na chamada bind()");
        exit(1);
    }

    while (1)
    {
        size = sizeof(fromaddr);
        status=recvfrom(sd,msg,MSGMAXSIZE,0,(struct sockaddr *)&fromaddr,&size);
        if (status < 0)
        {
            perror("ERRO no recebimento de datagramas UDP \n");
            exit(1); 
        }
        printf("Endereco de origem: %s\n",inet_ntop(AF_INET,&fromaddr.sin_addr,buffer,BUFFERSIZE));
        printf("Porta de origem = %hu \n",ntohs(fromaddr.sin_port));
        printf("<< %s\n",msg);
        fflush(stdout);
        
        // Divide mensagem em comando e conteudo
        strncpy(command_str, msg, 4);        command_str[5] = "\0";
        printf("comando: %s\n",command_str);

        // Compara comandos
        if (strcmp("USER",command_str) == 0){
            printf("conteudo: %s\n",msg+5);
            // TODO: armazenar usuario
            // Envia OKOK caso haja espaco
            // TODO: fazer condicao em que nao haja espaco
            status = sendto(sd,"OKOK:\0",strlen("OKOK:\0"),0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
            if (status < 0)
            {
                printf("ERRO no envio\n");
                exit(1); 
            }
        }

        else if (strcmp("TEST",command_str) == 0){
            // Envia OKOK
            status = sendto(sd,"OKOK:\0",strlen("OKOK:\0"),0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
            if (status < 0)
            {
                printf("ERRO no envio de OKOK \n");
                exit(1); 
            }
        }

        // status = sendto(sd,msg,strlen(msg)+1,0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
        // if (status < 0)
        // {
        //     printf("ERRO no envio de datagramas UDP \n");
        //     exit(1); 
        // }

    }        
}