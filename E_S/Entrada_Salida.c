  GNU nano 2.3.1                                                                                                                                               File: Optativo/e_s_c.c                                                                                                                                                                                                                                                                                                    

/*Code by Asier Zubia*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#define MAX_RANDOM 11
#define PATH "../Ficheros/matrices.txt"

/*Declaracion de las funciones, para que quede mas limpio el codigo*/
void imprimir_matriz(int *ma,int na);
int *inicializar_matrices(int ta);
int rellenar_matriz(int *matriz_uno,int *matriz_dos,int tam);
int getTamano();

int *P_uno;
int *P_dos;
int *Columna;
int *Resultado;

int main(int argc, char *argv[])
{
  int i, j,yes, k, N,world_size, world_rank;
  /*Esto es para probar, pero acordarme de que me lo tienen que pasar como parametro*/
  /*Inicializacion de las matrices de manera dinamica y de manera contigua para que no haya problemas. Se ha hecho con 
    con el broadcast y con el scatter. Lo he hecho como se propuso en clase*/

  MPI_Init (&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  /*Esto es porque tengo que jugar con las matrices cuadradas y con el numero de procesadores*/
  /*Me queda contemplar que haago con las matrices no cuadradas*/
/*1--> dividirlo entre el numero de procesadores, y el que quede que lo haga el procesador 0*/
  if(world_rank == 0){
    N = getTamano();
    if( N == -1) {
      printf("No se ha podido leer correctamente los datos del fichero");
      MPI_Finalize();
      exit(-1);
    }
  }
  MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
  P_uno = inicializar_matrices(N);
  P_dos = inicializar_matrices(N);
  Resultado = inicializar_matrices(N);

  if (N%world_size!=0) {
    if(world_rank==0) printf("El tamaño de la matriz tiene que ser multiplo de los procesadores utilizados\n");
    MPI_Finalize();
    exit(-1);
  }
  printf("Ahora va entrar en rellenar la matriz\n");
  if(world_rank == 0){
    yes = rellenar_matriz(P_uno,P_dos,N);
    printf("DEVULEVE RELLENAR ==> %d\n",yes);
    if( yes == -1) {
      printf("No se ha podido leer correctamente los datos del fichero");
      MPI_Finalize();
      exit(-1);
    }
  }
  
  printf("SCATTER y Boradcast\n");
  /*Dos formas*/
  /*Ahora estoy haciendo uso de la primera opcion, pero de esta menera envio mas paquetes por la red, tenerlo en cuenta*/
  MPI_Scatter (P_uno, N*N/world_size, MPI_INT, &P_uno[(world_rank * N)* N/world_size], N*N/world_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (P_dos, N*N, MPI_INT, 0, MPI_COMM_WORLD);
  for (i=(world_rank * N/world_size); i<((world_rank + 1)*N/world_size) ; i++){
    for (j=0; j<N; j++) {
      for (k=0; k<N; k++){
       Resultado[i*N + j] += P_uno[(N*i) + k] * P_dos[(N*k) + j];
      }
    }
  }
  /*Hacemos el gather para recuperar los resultados*/
  MPI_Gather(&Resultado[(world_rank * N) * N/world_size], N*N/world_size, MPI_INT, Resultado, N*N/world_size, MPI_INT,  0, MPI_COMM_WORLD);
  /*El procesador 0 se encarga de hacer de imprimir las matrices*/
  if (world_rank==0) {
    imprimir_matriz(P_uno,N);
    imprimir_matriz(P_dos,N);
    imprimir_matriz(Resultado,N);
  }
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
  /*Las inicializo de esta manera para que sean contiguas*/
  int i;
  int *result;
  result=(int *)calloc(tam*tam,sizeof(int));
  return result;
}

int rellenar_matriz(int *matriz_uno,int *matriz_dos,int tam){
  FILE *fp;
  int i = 0 ;
  int tam_uno = 0;
  int tam_dos = 0;
  int numero = 0;
  int primera = 0;
  char *path = "matrices.txt";
  fp = fopen(path,"r");
  if(fp!=NULL){
      char text_to_read[50];
      while(1){
        fgets(text_to_read,50,fp);
        if(i == 1){
          text_to_read[strcspn (text_to_read, "\n" )] = '\0';
          printf("TEXTO ES --> %d\n",strcmp(text_to_read,"begin"));
          if(strcmp(text_to_read,"begin") == 0){
            i++;
            printf("Lee el begin\n");
            primera = 1;
            printf("Valor de primera --> %d\n",primera);
          }else{
            printf("Lo siento, la estructura del fichero no se corresponde con la necesaria. Revisa la estrucutra del fichero\n");
            break;
          }
	}else if(i>1){
           if(!feof(fp)){ /*Para controlar que he llegado al final del archivo*/
              text_to_read[strcspn (text_to_read, "\n" )] = '\0';
              if(strcmp(text_to_read,"segunda") == 0){
                 primera = 0;
              }else{

                numero = atoi(text_to_read);
                printf("EL NUMERO ES --> %d\n", primera);
                if(primera == 1){
                        printf("Numero anadido a la MATRIZ UNO --> %d\n",numero);
                        matriz_uno[tam_uno] = numero;
                        tam_uno++;
                }
                else{
                     	printf("Numero anadido a la MATRIZ DOS --> %d\n",numero);
                        matriz_dos[tam_dos] = numero;
                        tam_dos++;
                }
             }
            }else break;
        }else{
          printf("Aumento el valor de i\n");
          i++;
	}
      }
    }
    fclose(fp);
    printf("Tamano P1 --> %d\n",tam_uno);
    printf("Tamano P2 --> %d\n",tam_dos);
    if(tam_uno>0 && tam_dos> 0){
      printf("Los tamanos son mayoes que 0\n");
      if(tam_uno == tam_dos){
        return tam_uno;
        }
    }else{
      return -1;
    }
}

int getTamano(){
    FILE *fp;
    char *path = "matrices.txt";
    char text_to_read[50];
    fp = fopen(path,"r");
    if(fp == NULL){
        printf("Es NULL\n");
    }else{
	fgets(text_to_read,50,fp);

       	int numero = atoi(text_to_read);
        //Control de errores. Por si a caso lo que lee no es un numero
        if(numero == 0 && text_to_read != "0"){

                 return -1;
        }else{

               	fclose(fp);
                return numero;
        }
   }
   return -1;
}
