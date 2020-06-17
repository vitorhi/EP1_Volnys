#include "tabela.h"
#include <stddef.h>

#define  FALSE 0
#define  TRUE  1

// Construtor da tabela
void InitTabela(struct tabela *T){
	T->n_usuarios=0;
	
}

int TabelaVazia(struct tabela *T)
        {
	if (T->n_usuarios == 0)
		return(TRUE);
	else
		return(FALSE);
        }
int TabelaCheia(struct tabela *T){
	if (T->n_usuarios >= MAXUSUARIOS)
		return(TRUE);
	else
		return(FALSE);
}

// Insere nome e socket address do usuário na tabela T
// Retorna: 1 se bem sucedido
//			0 se falha 
int InserirTabela(struct tabela *T, struct nome_address n){
	if (TabelaCheia(T)== TRUE)
		return FALSE;
	else{
		T->t[T->n_usuarios]=n;
		T->n_usuarios++;
		return TRUE;

	}

}

// Retira Usuario da tabela T com socket address socket
// Retorna: 1 se bem sucedido
//			0 se falha 
int RetirarTabela(struct tabela *T, struct sockaddr_in socket){
	
	int i=0;
	struct tabela nova_tabela;
	InitTabela(&nova_tabela);
	int isInTable=TRUE;
	int vezesRemovido=0;

	for (i = 0; i < T->n_usuarios; i++)
	{
		if (T->t[i].ender_socket.sin_addr.s_addr==socket.sin_addr.s_addr)
		{
			if (T->t[i].ender_socket.sin_port==socket.sin_port)
			{
				// Socket address a ser removido encontrado
				isInTable=FALSE;
				// Atualiza o número de vezes que o endereço foi removido
				vezesRemovido++;				
			}
		}
		if (isInTable==TRUE)
		{
			InserirTabela(&nova_tabela,T->t[i]);
		}
		isInTable=TRUE;
	}

	// Atualiza a tabela T com a nova tabela
	memcpy(T,&nova_tabela,sizeof(nova_tabela));
	if (vezesRemovido>=1){
		// Socket Address encontrado e removido
		return vezesRemovido;
	}
	// Socket address não encontrado
	return FALSE;
}

// Obtem o nome do usuario da tabela T com socket address socket
// Retorna: o nome se bem sucedido
//			char "0" se falha 
char *GetNome(struct tabela *T, struct sockaddr_in socket){
	int i=0;
	for (i = 0; i < T->n_usuarios; i++)
	{
		if (T->t[i].ender_socket.sin_addr.s_addr==socket.sin_addr.s_addr)
		{
			if (T->t[i].ender_socket.sin_port==socket.sin_port)
			{
				return T->t[i].nome;
			}
		}
	}
	return "0";
}


