#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tabela.h"
extern int    errno;

#define IPMAXSTRSIZE  16

//##########################################################
// main
//##########################################################

#define  BUFFERSIZE  80
#define  MSGMAXSIZE     96 

//pthread_t thread_timer;
pthread_t thread_rx;
//pthread_t thread_tx;

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

struct tabela Tabela_nomes;

//Funcao que envia a msg para 'toaddr'
void envia_DOWN(char msg[MSGMAXSIZE], struct sockaddr_in toaddr){
	int status;
	status = sendto(sd,msg,strlen(msg),0,(struct sockaddr *) &toaddr,sizeof(toaddr));
	if (status < 0){
	    printf("ERRO no envio da resposta DOWN");
		exit(1); 
    }
}


int main() {
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
    
    InitTabela(&Tabela_nomes);
    
    
    while (1) {
        size = sizeof(fromaddr);
        status=recvfrom(sd,msg,MSGMAXSIZE,0,(struct sockaddr *)&fromaddr,&size);
        if (status < 0)
        {
            perror("ERRO no recebimento de datagramas UDP \n");
            exit(1); 
        }
        printf("\nEndereco de origem: %s\n",inet_ntop(AF_INET,&fromaddr.sin_addr,buffer,BUFFERSIZE));
        printf("Porta de origem = %hu \n",ntohs(fromaddr.sin_port));
        printf("<< %s\n",msg);
        fflush(stdout);
        
        // Divide mensagem em comando e conteudo
        strncpy(command_str, msg, 4);        command_str[5] = "\0";
        printf("comando: %s\n",command_str);

        // Compara comandos
        if (strcmp("USER",command_str) == 0){
            printf("conteudo: %s\n",msg+5);
            printf("msg: %s\n", msg);
            
            if(TabelaCheia(&Tabela_nomes)){
            	status = sendto(sd,"BUSY:\0",strlen("BUSY:\0"),0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
            	
            	if (status < 0) {
		            printf("ERRO no envio BUSY\n");
		            exit(1); 
            	}
            } else {
               	//printf("\nINSERINDO user na tabela\n");
            	struct nome_address nm_adr;
            	struct sockaddr_in addr_rx;
            	memcpy((struct sockaddr*)&addr_rx,(struct sockaddr*)&fromaddr,sizeof(struct sockaddr_in));
            	
            	
            	strcpy(nm_adr.nome, msg+5);
            	nm_adr.ender_socket = addr_rx;

            	InserirTabela(&Tabela_nomes,nm_adr);
            	//printf(nm_adr.nome);
            	//printf("\nINSERIU user na tabela\n");
            	
            	status = sendto(sd,"OKOK:\0",strlen("OKOK:\0"),0,(struct sockaddr *) &addr_rx,sizeof(addr_rx));
            	
            	if (status < 0) {
                	printf("ERRO no envio OKOK\n");
                	exit(1); 
            	}
            }
            
            int n_usuarios;
            char name[11];
            int i;
            n_usuarios = Tabela_nomes.n_usuarios;
            
            printf("Usuários conectados: %d | ", n_usuarios); 
            for (i=0; i<n_usuarios; i++){
            	strcpy(name, GetNome(&Tabela_nomes, Tabela_nomes.t[i].ender_socket));
            	printf(name);
            	printf(" | ");
            }
            printf("\n");
                  
        }else if (strcmp("TEST",command_str) == 0){
            // Envia OKOK
            status = sendto(sd,"OKOK:\0",strlen("OKOK:\0"),0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
            if (status < 0)
            {
                printf("ERRO no envio de OKOK \n");
                exit(1); 
            }
        } else if (strcmp("UP  ",command_str) == 0){
        	char name_rx[11];
        	char name[11];
        	int n_usuarios;
        	int i;
        	n_usuarios = Tabela_nomes.n_usuarios;


   			//Obtem nome da pessoa que enviou a mensagem
        	strcpy(name_rx, GetNome(&Tabela_nomes, fromaddr));
        	
        	for (i=0; i<n_usuarios; i++){
        		//Cria mensagem de resposta
        		char resposta[MSGMAXSIZE] = "";
		    	strcat(resposta, "DOWN:");
		    	strcat(resposta, name_rx);
		    	strcat(resposta, ":");
		    	strcat(resposta, msg+5);
		    	strcat(resposta, "\0");
		    	
		    	        	
        		//Pega o nome dos usuarios conectados
		    	strcpy(name, GetNome(&Tabela_nomes, Tabela_nomes.t[i].ender_socket));
		    	
		    	//Envia a mensagem para o usuario conectado
		    	envia_DOWN(resposta, Tabela_nomes.t[i].ender_socket);
        	}
        } else if (strcmp("EXIT",command_str) == 0) {
        	char name_rx[11];
        	char name[11];
        	int n_usuarios;
        	int i;
        	int status_saiu;
        	
        	//Obtem nome da pessoa que enviou a mensagem
        	strcpy(name_rx, GetNome(&Tabela_nomes, fromaddr));
        
        
            // Envia BYE para o usuário que pediu saida
            status = sendto(sd,"BYE :\0",strlen("BYE :\0"),0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
            if (status < 0) {
                printf("ERRO no envio de BYE  \n");
                exit(1); 
            }
            
            //Remove usuario da tabela
            status_saiu = RetirarTabela(&Tabela_nomes, fromaddr);
            //printf("Status saiu: %d\n", status_saiu);
            
            n_usuarios = Tabela_nomes.n_usuarios;
        	
			//Envia informacao de saida aos outros usuarios
            for (i=0; i < n_usuarios; i++){
            	//Cria mensagem de envio
        		char resposta[MSGMAXSIZE] = "";
        		strcat(resposta, "DOWN:");
		    	strcat(resposta, name_rx);
		    	strcat(resposta, " Saiu");
		    	strcat(resposta, "\0");
            
            	//status = sendto(sd,resposta,strlen(resposta),0,(struct sockaddr *) &Tabela_nomes.t[i].ender_socket,sizeof(fromaddr));
            	envia_DOWN(resposta, Tabela_nomes.t[i].ender_socket);

        		/*if (status < 0){
		            printf("ERRO no envio da resposta do user [%d]  \n", i);
		            exit(1); 
            	}*/
            }
            
            //Printa informacoes dos usuarios restantes
            printf("Usuários conectados: %d | ", n_usuarios); 
            for (i=0; i<n_usuarios; i++){
            	strcpy(name, GetNome(&Tabela_nomes, Tabela_nomes.t[i].ender_socket));
            	printf(name);
            	printf(" | ");
            }
            printf("\n");
           
        }

        // status = sendto(sd,msg,strlen(msg)+1,0,(struct sockaddr *) &fromaddr,sizeof(fromaddr));
        // if (status < 0)
        // {
        //     printf("ERRO no envio de datagramas UDP \n");
        //     exit(1); 
        // }

    }
    
    
    //pthread_create(&thread_timer, NULL, (void*) timer, NULL);
    //pthread_create(&thread_rx, NULL, (void*) rx, NULL);
    //pthread_create(&thread_tx, NULL, (void*) tx, NULL);

    //pthread_join(thread_rx,NULL);
    //pthread_join(thread_tx,NULL);        
}
