/*Code by Asier Zubia*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_RANDOM 11

/*Declaracion de las funciones, para que quede mas limpio el codigo*/
void imprimir_matriz(int *ma,int na);
int *inicializar_matrices(int ta);
void rellenar_matriz(int *matriz,int tam);


int *P_uno;
int *P_dos;
int *Columna;
int *Resultado;

int main(int argc, char *argv[])
{
  int from, to, i, j, k, N, M, world_size, world_rank;
  /*Esto es para probar, pero acordarme de que me lo tienen que pasar como parametro*/
  N = 2;
  /*Inicializacion de las matrices de manera dinamica y de manera contigua para que no$
    con el broadcast y con el scatter. Lo he hecho como se propuso en clase*/
   /*Como el enunciado no especifica entre que numeros, yo voy a rellenar
  las matrices con numero aleatorios entre el 0 y 10*/
  MPI_Init (&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  /*Esto es porque tengo que jugar con las matrices cuadradas y con el numero de procesadores*/
  /*Me queda contemplar que haago con las matrices no cuadradas*/
/*1--> dividirlo entre el numero de procesadores, y el que quede que lo haga el procesador 0*/

  if (N%world_size!=0) {
    if(world_rank==0) printf("El tamaño de la matriz tiene que ser multiplo de los procesadores utilizados\n");
    MPI_Finalize();
    exit(-1);
  }
  MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
  P_uno = inicializar_matrices(N);
  P_dos = inicializar_matrices(N);
  Resultado = inicializar_matrices(N);

  if(world_rank == 0){
        /*Simplificar en una sola funcion?*/
        rellenar_matriz(P_uno,N);
        rellenar_matriz(P_dos,N);
  }

  /*Dos formas*/
  /*1 --> Con la propia matriz*/
  /*2 --> Con un array que sea la columna*/
  /*Ahora estoy haciendo uso de la primera opcion, pero de esta menera envio mas paquetes por la red, tenerlo en cuenta*/
  MPI_Scatter (P_uno, N*N/world_size, MPI_INT, &P_uno[(world_rank * N)* N/world_size], N*N/world_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (P_dos, N*N, MPI_INT, 0, MPI_COMM_WORLD);
  /*Esto es solo para saber si se hace bien el envio de las columnas a los correspondientes procesadores*/
  for (i=(world_rank * N/world_size); i<((world_rank + 1)*N/world_size) ; i++){
    for (j=0; j<N; j++) {
      for (k=0; k<N; k++){
       Resultado[i*N + j] += P_uno[(N*i) + k] * P_dos[(N*k) + j];
      }
    }
  }
  /*Hacemos el gather para recuperar los resultados*/
  MPI_Gather(&Resultado[(world_rank * N) * N/world_size], N*N/world_size, MPI_INT, Resultado, N*N/world_size, MPI_INT,  0, MPI_COMM_WORLD);
  //printf("Despues del gather\n");
  /*El procesador 0 se encarga de hacer de imprimir las matrices*/
  if (world_rank==0) {
    for(i=0; i<N; i++) {
      printf("\n\t| ");
      for(j=0; j<N; j++){
        printf(" %2d ",P_uno[i*N + j]);
        printf("|");
     }
    }
  printf("\n\n");

    for(i=0; i<N; i++) {
      printf("\n\t| ");
      for(j=0; j<N; j++){
        printf(" %2d ",P_dos[i*N + j]);
        printf("|");
     }
    }
  printf("\n\n");


    for(i=0; i<N; i++) {
      printf("\n\t| ");
      for(j=0; j<N; j++){
        printf(" %2d ",Resultado[i*N + j]);
        printf("|");
     }
    }
  printf("\n\n");
  }
  /*Faltan los frees*/
  free(P_uno);
  free(P_dos);
  free(Resultado);

  MPI_Finalize();
  return 0;
}
void imprimir_matriz(int *matriz,int tam)
{
  int i,j;
  for(i=0; i<tam; i++) {
      printf("\n\t| ");
      for(j=0; j<tam; j++){
        printf(" %2d \n",matriz[i*tam + j]);
        printf("|");
     }
    }
  printf("\n\n");
  printf("Fin De imprimir Matriz\n");

}

int *inicializar_matrices(int tam)
{
  int i;
  /*Las inicializo de esta manera para que sean contiguas*/
  int *result;
  //result=(int **)malloc(tam*sizeof(int *));
  /*Dos formas vistas en clase*/
  //1--- |
  result=(int *)calloc(tam*tam,sizeof(int));
  return result;
}

void rellenar_matriz(int *matriz,int tam){

  int j,k = 0;
  srand(time(NULL));
  for(j = 0;j < tam; j++ ){
      for(k = 0; k< tam;k++){
        /*Lo tengo que tratar como un array*/
        matriz[(j*tam)+k] = rand() % MAX_RANDOM;
        /*Podria aumentar el numero a 50 o a 100, pero es mas de lo mismo*/
        /*Probar con floats*/
      }
  }
}

