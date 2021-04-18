#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char** argv)
{
    int world_rank, world_size;
	int *matriz1, *matriz2, *msca, *resultado, *matlag;
    int tam, i, a, j, x, suma;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Reservamos la memoria de las matrices
    tam = atoi(argv[1]);
    matriz1=malloc((tam*tam) * sizeof(int));
	matriz2=malloc((tam*tam) * sizeof(int));
	resultado=malloc((tam*tam) * sizeof(int));

	//variables para calcular el tiempo de ejecucion (no se utilizan)
	//clock_t t_ini, t_fin;
  	//double secs;

  	

	//control de parametros y errores
	//asegurar el numero de argumentos que se pasan al programa
	if(argc!=2){
	   printf("El numero de argumentos no es correcto \n");
           return(-1);
    }
    //para que solo lo llene el proceso 0
    //t_ini = clock();
    if(world_rank==0){
    	for(i=0; i<tam*tam; i++){
            matriz1[i]=rand()%50;
            //matriz1[i]=1;
    	}
    	for(j=0; j<tam*tam; j++){
            matriz2[j]=rand()%50;
            //matriz2[j]=1;
    	}
    }
    
    msca=malloc(((tam*tam)/world_size)*sizeof(int));

    //Scatter
    //Metemos los datos de la matriz 1 en una matriz de tamaño reducido que sera el que urara cada procesador. El tamaño sera, el tamaño de la matriz(tam*tam) entre el numero de procesadores (word_size)
    MPI_Scatter(&matriz1[0], (tam*tam)/world_size, MPI_INT, msca, (tam*tam)/world_size, MPI_INT, 0, MPI_COMM_WORLD);
    //En el caso de la segunda matriz la mandaremos entera mediante bcast
    MPI_Bcast(&matriz2[0], tam*tam, MPI_INT, 0, MPI_COMM_WORLD);
	
    //Multiplicar la matriz
    //Crearemos una matriz de ayuda(matlag) que nos servira para meter los resultados que mas tarde agruparemos.
    matlag=malloc(((tam*tam)/world_size)*sizeof(int));
    
    for(i=0; i<tam/world_size; i++){

        for (j=0; j<tam; j++){
            suma = 0;
            for (a=0; a<tam; a++){
                suma += msca[a+(i*tam)] * matriz2[a+(j*tam)];
            }
            matlag[j+(i*tam)] = suma;
        }
    }

    //Mediante el gather agruparemos los resultados y los meteremos en la matriz resultado.
    MPI_Gather(&matlag[0], (tam*tam)/world_size, MPI_INT, resultado, (tam*tam)/world_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    //calculo final del tiempo de ejecucion
    //t_fin = clock();
  	//secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    //para imprimir los resultados. Solo lo hara el proceso 0
    if(world_rank==0){
        printf("Resultado \n");
            for(i=0; i<tam*tam; i++)
            {           
                    printf("[%d]", resultado[i]);
                    
            }
            //printf("El tiempo de ejecucion ha sido: %f", secs);
            printf("\n");
		
	}
   
	free(matriz1);
	free(matriz2);
	free(resultado);
    MPI_Finalize();
    return (0);

}
