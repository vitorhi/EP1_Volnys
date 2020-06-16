#include "tabela.h"
#include <stddef.h>

#define  FALSE 0
#define  TRUE  1

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

int InserirTabela(struct tabela *T, struct nome_address n){
	if (TabelaCheia(T)== TRUE)
		return FALSE;
	else{
		T->t[T->n_usuarios]=n;
		T->n_usuarios++;
		return TRUE;

	}

}

int GetName(struct tabela *T, struct sockaddr_in socket){
	int i=0;
	for (i = 0; i < T->n_usuarios; i++)
	{
		if (T->t[i].ender_socket.sin_addr.s_addr==socket.sin_addr.s_addr)
		{
			if (T->t[i].ender_socket.sin_port==socket.sin_port)
			{
				printf("%s\n",T->t[i].nome );
			}
		}
		else{

		}
	}
}

