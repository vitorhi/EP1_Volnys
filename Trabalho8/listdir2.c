#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define min(x,y)   (((x)<(y))?(x):(y))

#define LINESIZE       80
#define PATHSIZE       80
#define LISTBUFFERSIZE 500

void append(char *dest,int buffersize, char *src)
    {
    int d;
    int i;

    d = strlen(dest);
    for (i=0; i<min(strlen(src),buffersize-1-d); i++)
       dest[d+i] = src[i];
    dest[d+i] = '\0';
    }

// Lista diretorio
//    path: diretorio a ser listado
//    buffer: buffer que contera' a string com a sequencia ASCII
//            resultado da listagem do diretorio (finalizada por '\0'
//    bufffersize: tamanho do buffer

void lista_diretorio(char *path,char *buffer,int buffersize)
    {
    DIR           * dirp;
    struct dirent * direntry;
    char            linha[80];
    int             i = 0;

    dirp = opendir(path);
    if (dirp ==NULL)
       {
       perror("ERRO: chamada opendir(): Erro na abertura do diretorio: ");
       snprintf(buffer,buffersize,"Erro na listagem diretorio!\n");
       return;
       }
    buffer[0]='\0';
    while (dirp) 
        {
	// Enquanto nao chegar ao fim do diretorio, leia cada entrada
        direntry = readdir(dirp);
	if (direntry == NULL)
           // chegou ao fim
           break;
        else
           {
           // ler entrada
           append(buffer,buffersize,direntry->d_name);
           append(buffer,buffersize,"\n");
           }
        }
    closedir(dirp);
    }
