#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define min(x,y)   (((x)<(y))?(x):(y))

#define LINESIZE       80
#define PATHSIZE       80
#define LISTBUFFERSIZE 500

void append(char *dest,int buffersize, char *src);
void lista_diretorio(char *path,char *buffer,int buffersize);