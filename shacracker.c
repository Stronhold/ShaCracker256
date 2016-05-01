#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ALPHABET "12345"
#define MIN 2
#define MAX 4

//obtenemos la potencia para un numero (funciona!)
int getPow(int size, int pos){
	int i;
	int result = 1;
	printf("Resultado para posicion: %i \r\n", pos);
	for(i = 1; i <= pos;i++){
		result *= size;
	}
	printf("Result: %u \r\n", result);
	printf("--------------------------------- \r\n");
	return result;
}

//obtenemos el keyspace (works!)
int getKeySpace(int min, int max, int size){
	int i;
	int result = 0;
	//printf("Min recibido: %u", )
	for(i = min; i <= max; i++){
		result += getPow(size, i);
	}
	return result;
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
	printf("Tamaño del array: %u \r\n", size);
	
	char *caracters = malloc(size+1);
	int i = 0;

	//inicializacion del array
	for(i = 0; i < size; i++){
		caracters[i] = alph[0];
	}
	caracters[size] = '\0';
	printf("%s \n", caracters);
	i = 0;

	//cambio de base
	while(indexs%longitudAlfabeto != 0){
		caracters[i] = ALPHABET[indexs%longitudAlfabeto];
		i++;
		indexs /= longitudAlfabeto;
	}
	return caracters;

}

//haciendo pruebas aun con el getkey
int main(int argc, char *argv[]){
	int size = strlen(ALPHABET);
	printf("Size: %u \r\n", size);
	int keyspace = getKeySpace(MIN, MAX, size);
	printf("KeySpace %u \r\n", keyspace);
	char* key = getKey(0, MIN,MAX, ALPHABET);
	//char* clave = getKey(0,0,0,"");
	printf("Clave: %s \r\n", key);
	free(key);
	return 0;
}