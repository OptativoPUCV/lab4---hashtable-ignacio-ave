#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"



int enlarge_called=0;



Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

//2.- Implemente la función void insertMap(HashMap * map, char * key, void * value). Esta función inserta un nuevo dato (key,value) en el mapa y actualiza el índice current a esa posición. Recuerde que para insertar un par (clave,valor) debe:

//a - Aplicar la función hash a la clave para obtener la posición donde debería insertar el nuevo par

//b - Si la casilla se encuentra ocupada, avance hasta una casilla disponible (método de resolución de colisiones). Una casilla disponible es una casilla nula, pero también una que tenga un par inválido (key==NULL).

//c - Ingrese el par en la casilla que encontró.

//No inserte claves repetidas. Recuerde que el arreglo es circular. Recuerde actualizar la variable size.
void insertMap(HashMap * map, char * key, void * value) {
    if (map == NULL|| key == NULL) return;

    if (map->size >= map->capacity * 0.7) enlarge(map);

    long index = hash(key, map->capacity);
    while (map->buckets[index] != NULL && map->buckets[index]->key != NULL) {
        index = (index + 1) % map->capacity;
    }

    if (map->buckets[index] == NULL|| map->buckets[index]->key == NULL) {
        map->buckets[index] = createPair(key, value);
        map->size++;
    } else {
        map->buckets[index]->value = value;
    }
    map->current = index;
}
//6.- Implemente la función void enlarge(HashMap * map). Esta función agranda la capacidad del arreglo buckets y reubica todos sus elementos. Para hacerlo es recomendable mantener referenciado el arreglo actual/antiguo de la tabla con un puntero auxiliar. Luego, los valores de la tabla se reinicializan con un nuevo arreglo con el doble de capacidad. Por último los elementos del arreglo antiguo se insertan en el mapa vacío con el método insertMap. Puede seguir los siguientes pasos:

//a - Cree una variable auxiliar de tipo Pair** para matener el arreglo map->buckets (old_buckets);

//b - Duplique el valor de la variable capacity.

//c - Asigne a map->buckets un nuevo arreglo con la nueva capacidad.

//d - Inicialice size a 0.

//e - Inserte los elementos del arreglo old_buckets en el mapa (use la función insertMap que ya implementó).
void enlarge(HashMap * map) {
    if(map == NULL) return;

    Pair ** old_buckets = map->buckets;
    long old_capacity = map->capacity;
    map->capacity *= 2;

    enlarge_called = 1; //no borrar (testing purposes)


    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
    if(map->buckets == NULL) {
        map->buckets = old_buckets;
        map->capacity = old_capacity;
        return;
    }

    map->size = 0;
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]);
    
        }
    }
    free(old_buckets);

}

//1.- Implemente la función createMap en el archivo hashmap.c. Esta función crea una variable de tipo HashMap, inicializa el arreglo de buckets con casillas nulas, inicializa el resto de variables y retorna el mapa. Inicialice el índice current a -1.
HashMap * createMap(long capacity) {
    HashMap * mapa = (HashMap *)malloc(sizeof(HashMap));
    if (mapa == NULL) return NULL;

    mapa->buckets = (Pair **)calloc(capacity, sizeof(Pair *));

    if (mapa->buckets == NULL) {
        free(mapa);
        return NULL;
    }

    mapa->size = 0;
    mapa->capacity = capacity;
    mapa->current = -1;
    return mapa;
}

//4.- Implemente la función void eraseMap(HashMap * map, char * key). Está función elimina el dato correspondiente a la clave key. Para hacerlo debe buscar el dato y luego marcarlo para que no sea válido. No elimine el par, sólo invalídelo asignando NULL a la clave (pair->key=NULL). Recuerde actualizar la variable size.
void eraseMap(HashMap * map,  char * key) {   
    if (map == NULL || key == NULL) return;
    Pair * pair = searchMap(map, key);

    if (pair != NULL) {
        pair->key = NULL;
        map->size--;
    }


}


//3.- Implemente la función Pair * searchMap(HashMap * map, char * key), la cual retorna el Pair asociado a la clave ingresada. Recuerde que para buscar el par debe:

//a - Usar la función hash para obtener la posición donde puede encontrarse el par con la clave

//b - Si la clave no se encuentra avance hasta encontrarla (método de resolución de colisiones)

//c - Si llega a una casilla nula, retorne NULL inmediatamente (no siga avanzando, la clave no está)

//Recuerde actualizar el índice current a la posición encontrada. Recuerde que el arreglo es circular.

Pair * searchMap(HashMap * map,  char * key) {   
    if(map == NULL || key == NULL) return NULL;

    long index = hash(key, map->capacity);
    while(map->buckets[index] != NULL && map->buckets[index]->key != NULL) {
        if(is_equal(map->buckets[index]->key, key)) {
            map->current = index;
            return map->buckets[index];
        }
        index = (index + 1) % map->capacity;
    }

    return NULL;
}
//5.- Implemente las funciones para recorrer la estructura: Pair * firstMap(HashMap * map) retorna el primer Pair válido del arreglo buckets. Pair * nextMap(HashMap * map) retorna el siguiente Pair del arreglo buckets a partir índice current. Recuerde actualizar el índice.

Pair * firstMap(HashMap * map) {
    if (map == NULL) return NULL;

    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }

    return NULL;
}

Pair * nextMap(HashMap * map) {
    if (map == NULL) return NULL;

    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }


    return NULL;

}
