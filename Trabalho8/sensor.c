// Arquivo: sensor.c
// Utiliza a biblioteca pthreads.
// Incluir arquivo na compilação do programa
// Para compilar:  cc -o <executavel> <modulo.c> sensor-luz.c -lpthread

#include "sensor.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TAMVETOR 30

// Vetor de niveis de luminosidade
int valores[TAMVETOR] = {0,  10, 20,  40, 60, 80, 90, 100, 100, 90,
                         80, 60, 40,  30, 20, 30, 40, 50,  40,  60,
                         80, 90, 100, 90, 80, 60, 40, 20,  10,  0};
int nivelsensor = 0;
pthread_t threademulador;

// thread de emulacao do sensor de luminosidade
void emuladorsensor(void) {
  int i = 0;

  while (1) {
    nivelsensor = valores[i];
    sleep(PERIODO);
    i = (i + 1) % TAMVETOR;
  }
}

int valorsensor(void) {
  return (nivelsensor);
}

void initsensor(void) {
  pthread_create(&threademulador, NULL, (void *)emuladorsensor, NULL);
}

// Descomentar para testar
//
// int main()
//    {
//    printf("Programa teste emulador do sensor \n");
//    initsensor();
//    while (1)
//      {
//      sleep (1);
//      printf("%d\n",valorsensor());
//      }
//    pthread_join(threademulador,NULL);
//    printf("Terminado processo Produtor-Consumidor.\n\n");
//    }
