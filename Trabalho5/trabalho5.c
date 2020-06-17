#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "tabela.h"
extern int    errno;

#define IPMAXSTRSIZE  16

//##########################################################
// main
//##########################################################

#define  BUFFERSIZE  80
#define  MSGMAXSIZE     512 


pthread_t thread_timer;
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

struct tabela tabela_usuarios;
struct nome_address usuario;

int Send_OKOK(struct sockaddr_in f_addr)
    {
    int    status;

    status = sendto(sd,"OKOK:\0",strlen("OKOK:\0")+1,0,(struct sockaddr *) &f_addr,sizeof(f_addr));
    if (status < 0)
        {
        perror("Erro na chamada send() na função Send_OKOK");
        return(-1);
        }
    return 0;
    }
int Send_TEST(struct sockaddr_in f_addr)
    {
    int    status;

    status = sendto(sd,"TEST:\0",strlen("TEST:\0")+1,0,(struct sockaddr *) &f_addr,sizeof(f_addr));
    if (status < 0)
        {
        perror("Erro na chamada send() na função Send_TEST");
        return(-1);
        }
    return 0;
    }

int Send_BUSY(struct sockaddr_in f_addr)
    {
    int    status;

    status = sendto(sd,"BUSY:\0",strlen("BUSY:\0")+1,0,(struct sockaddr *) &f_addr,sizeof(f_addr));
    if (status < 0)
        {
        perror("Erro na chamada send() na função Send_BUSY");
        return(-1);
        }
    return 0;
    }

int Send_BYE(struct sockaddr_in f_addr)
    {
    int    status;

    status = sendto(sd,"BYE :\0",strlen("BYE  :\0"),0,(struct sockaddr *) &f_addr,sizeof(f_addr));
    if (status < 0)
        {
        perror("Erro na chamada send() na função Send_BYE");
        return(-1);
        }
    return 0;
    }
int Send_DOWN(char* msg, char* user_name)
{
    int    status;
    char msg_to_send[95];
    struct sockaddr_in f_addr;

    printf("Enviando mensagem para usuarios\n");
    fflush(stdout);
    strcpy(msg_to_send, "DOWN:");
    strcat(msg_to_send, user_name);
    strcat(msg_to_send, ":");
    strcat(msg_to_send, msg);
    // strcat(msg_to_send, "\0");

    for (int i = 0; i < tabela_usuarios.n_usuarios; i++)
    {   
        // Compara usuario que enviou a mensagem com a tabela
        if(strcmp(user_name,tabela_usuarios.t[i].nome) != 0)
        {
            // Obtem endereco da tabela
            f_addr=tabela_usuarios.t[i].ender_socket;

            status = sendto(sd,msg_to_send,strlen(msg_to_send)+1,0,(struct sockaddr *) &f_addr,sizeof(f_addr));
            if (status < 0)
                {
                perror("Erro na chamada send() na função Send_DOWN");
                return(-1);
                }
            
        }
        
    }
    printf("Mensagem enviada\n");
    fflush(stdout);

    return 0;
    
}
void timer(){

    int    status;
    char   timerbuffer[10];    // buffer para mensagem "TEST:"

    while (1){
        sleep(30);
        // printf("30s se passaram\n");       
    }
}

int main()
{
    char nomeUP[11];
    mysocketaddr.sin_family = AF_INET;
    mysocketaddr.sin_addr.s_addr   = INADDR_ANY;

    usuario.ender_socket.sin_family = AF_INET;
    usuario.ender_socket.sin_addr.s_addr   = INADDR_ANY;

    InitTabela(&tabela_usuarios);
    
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

    // Inicia Thread de timer
    pthread_create(&thread_timer, NULL, (void*) timer, NULL);

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
        strncpy(command_str, msg, 4);        
        command_str[5] = "\0";
        printf("comando: %s\n",command_str);

        // Compara comandos
        if (strcmp("USER",command_str) == 0){
            printf("conteudo: %s\n",msg+5);

            // Armazena usuario em uma struct
            usuario.ender_socket=fromaddr;
            strncpy(usuario.nome,msg+5,10);

            status=InserirTabela(&tabela_usuarios,usuario);
            printf("numero de usuarios na tabela: %d\n",tabela_usuarios.n_usuarios );
            if (status==1)
            {
                // Envia OKOK caso haja espaco
                Send_OKOK(fromaddr);
            }
            else
            {
                // Envia BUSY caso não haja espaco
                Send_BUSY(fromaddr);
            }

            
        }

        else if (strcmp("TEST",command_str) == 0){
            // Envia OKOK
            Send_OKOK(fromaddr);
        }

        else if (strcmp("UP  ",command_str) == 0){

            strncpy(nomeUP,GetNome(&tabela_usuarios, fromaddr) , 10);
            printf("Mensagem de %s recebida:%s\n", nomeUP,msg+5);
            Send_DOWN(msg+5,nomeUP);
        }

        else if (strcmp("EXIT",command_str) == 0){

            strncpy(nomeUP,GetNome(&tabela_usuarios, fromaddr) , 10);
            if(RetirarTabela(&tabela_usuarios,fromaddr)>0)
            {
                printf("Usuario %s saiu\n", nomeUP);
                Send_DOWN("saiu\0",nomeUP);
                Send_BYE(fromaddr);
            }
            else
            {
                printf("Exclusão de usuário %s não feita\n", nomeUP);
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