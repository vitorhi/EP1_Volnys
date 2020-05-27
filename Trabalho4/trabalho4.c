//Trabalho4

#include <string.h>
#include <stdio.h>
#include <netdb.h>

char stringURL[50];
char servico[50];
char endereco[50];
int porta;
char file[50];

int main(){
	struct servent 		*serventp;
	struct protoent 	*protoentp;
	struct hostent    	*hostentp;

	printf("Programa decodificador simplificado  de URL\n\n");
	printf("Entre com a URL (esquema://dominio:porta/caminho): ");
	scanf("%s",stringURL);
	printf("\n");
	sscanf(stringURL, "%99[^://]://%99[^:]:%99d/%99[^\n]", servico, endereco, &porta, file);

	serventp = getservbyname(servico, "udp");
	if (serventp == NULL)
    	perror("Erro na chamada getservbyname()");

	protoentp = getprotobyname(serventp->s_proto);
  	if (protoentp == NULL)
    	perror("Erro na chamada getprotobyname()");

	hostentp = gethostbyname(endereco);
	if (hostentp == NULL)
    	perror("Erro na chamada gethostbyname()");  

	printf("Protocolo: %s  (%d) Servico: %s\n", protoentp->p_name, protoentp->p_proto, serventp->s_name);
	printf("Endereco: %s (%d.%d.%d.%d)\n", endereco,((unsigned char *) hostentp->h_addr)[0],
       ((unsigned char *) hostentp->h_addr)[1],
       ((unsigned char *) hostentp->h_addr)[2],
       ((unsigned char *) hostentp->h_addr)[3]);
	printf("Porta: %d\n", porta);
	printf("Caminho: %s\n\n", file);
}

//http://www.usp.br:80/teste
