#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "sha256.h"
#define ALPHABET "12345"
#define MIN 2
#define MAX 4
#define HASHLENGTH 32

//obtenemos la potencia para un numero (funciona!)
int getPow(int size, int pos){
	int i;
	int result = 1;
	for(i = 1; i <= pos;i++){
		result *= size;
	}
	return result;
}

//obtenemos el keyspace (works!)
int getKeySpace(int min, int max, int size){
	int i;
	int result = 0;

	for(i = min; i <= max; i++){
		result += getPow(size, i);
	}
	return result;
}

void getHash(char *str, char hash[HASHLENGTH+1] ) {
   unsigned char hashAux[32];
   SHA256_CTX ctx;
   sha256_init(&ctx);
   sha256_update(&ctx,str,strlen(str));
   sha256_final(&ctx,hash);
}


char* getKey(int indice, int min, int max, char* alph){
	int size = min;
	int indexs = indice;
	int longitudAlfabeto = strlen(alph);
	//caracteres necesarios para representarlo
	while(getPow(longitudAlfabeto, size)<indexs){
		indexs -= getPow(longitudAlfabeto, size);
		size++;
	}
	
	char *caracters = malloc(size+1);
	int i = 0;

	//inicializacion del array
	for(i = 0; i < size; i++){
		caracters[i] = alph[0];
	}
	caracters[size] = '\0';
	i = 0;

	//cambio de base
	while(indexs%longitudAlfabeto != 0){
		caracters[i] = ALPHABET[indexs%longitudAlfabeto];
		i++;
		indexs /= longitudAlfabeto;
	}
	return caracters;

}

void print_hash(unsigned char hash[])
{
   int idx;
   for (idx=0; idx < 32; idx++)
      printf("%02x",hash[idx]);
   printf("\n");
}

//bool does not exist in c pre c99
int main(int argc, char *argv[]){
	int rank, sizew;
	MPI_Status status;
	int buf = 1;

	auto password = "53";
	unsigned char passwordHash[HASHLENGTH+1];
	getHash(password, passwordHash);

	unsigned char hash[HASHLENGTH+1]; 
	int size = strlen(ALPHABET);
	printf("Size: %u \r\n", size);
	int keyspace = getKeySpace(MIN, MAX, size);
	printf("KeySpace %u \r\n", keyspace);
	int same = 5;
	int result;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	MPI_Comm_size(MPI_COMM_WORLD,&sizew);
  	int next = 1;

  	printf("Rank: %i, Size: %i\n", rank, sizew);

  	if(rank == 0){
		for(int i = 0; i < keyspace && same != 0; i++){
			MPI_Send(&buf, 1, MPI_INT, next, i, MPI_COMM_WORLD);
			if(next == 3)
				next = 0;
			next++;
		}
	}

	else{
		while(same != 0){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			if(status.MPI_SOURCE == 0){
				char* key = getKey(status.MPI_TAG, MIN,MAX, ALPHABET);
				getHash(key, hash);
				same = memcmp(passwordHash, hash, HASHLENGTH);

				printf("Clave: %s I: %i Rank: %i\n ", key, status.MPI_TAG, rank);
				printf("Iguales? %i \r\n", same);
				if(same == 0){
					result = atoi(key);
					for(int i=1; i<sizew; i++){
						if(i != rank)
							MPI_Send(&buf, 1, MPI_INT, i, result, MPI_COMM_WORLD);
					}
				}
				free(key);
			}
			else{
				same = 0;
				result = status.MPI_TAG;
			}
		}
	}

	MPI_Finalize();
	if(rank != 0)
		printf("Rank %i Result: %i\n", rank, result);
	return 0;
}