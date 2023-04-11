#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}
//transformar claves en posiciones
long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}
// para comparar claves
int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


//2.- Implemente la función void insertMap(HashMap * map, char * key, void * value). 
//Esta función inserta un nuevo dato (key,value) en el mapa y actualiza el índice current a esa posición.
// Recuerde que para insertar un par (clave,valor) debe:
//a - Aplicar la función hash a la clave para obtener la posición donde debería insertar el nuevo par
//b - Si la casilla se encuentra ocupada, avance hasta una casilla disponible (método de resolución de colisiones). Una casilla disponible es una casilla nula, pero también una que tenga un par inválido (key==NULL).
//c - Ingrese el par en la casilla que encontró.
//No inserte claves repetidas. Recuerde que el arreglo es circular. Recuerde actualizar la variable size.


void insertMap(HashMap * map, char * key, void * value) {

    Pair * par = createPair(key,value);
    long posicion = hash(key,map->capacity);
    //no se ecuentra posicion valida
    while (map->buckets[posicion]!=NULL)
    {
        if (is_equal(map->buckets[posicion]->key,key))
        {
            return; //no insertar claves repetidas
        }

        if (map->buckets[posicion]->key==NULL)
        {
            break;  //se ecuentra posicicion valida
        }

        posicion++; // avanzar posicion
        
        if (posicion==map->capacity)    
        {
            posicion=0; //recorrer arreglo desde el inicio
        }   
    }

    map->buckets[posicion]=par; // insertar par
    map->size++; // actualizar size
    map->current=posicion; // actualizar current


}
//6.- Implemente la función void enlarge(HashMap * map). 
//Esta función agranda la capacidad del arreglo buckets y reubica todos sus elementos. 
//Para hacerlo es recomendable mantener referenciado el arreglo actual/antiguo de la tabla con un puntero auxiliar. 
//Luego, los valores de la tabla se reinicializan con un nuevo arreglo con el doble de capacidad. 
//Por último los elementos del arreglo antiguo se insertan en el mapa vacío con el método insertMap. 
//Puede seguir los siguientes pasos:
//a - Cree una variable auxiliar de tipo Pair** para matener el arreglo map->buckets (old_buckets);
//b - Duplique el valor de la variable capacity.
//c - Asigne a map->buckets un nuevo arreglo con la nueva capacidad.
//d - Inicialice size a 0.
//e - Inserte los elementos del arreglo old_buckets en el mapa (use la función insertMap que ya implementó).
void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    //Para hacerlo es recomendable mantener referenciado el arreglo actual/antiguo de la tabla con un puntero auxiliar.
    Pair ** old_buckets = map->buckets;
    //Luego, los valores de la tabla se reinicializan con un nuevo arreglo con el doble de capacidad.
    map->capacity=map->capacity*2;
    map->buckets = (Pair **)malloc(sizeof(Pair *)*map->capacity);
    //Por último los elementos del arreglo antiguo se insertan en el mapa vacío con el método insertMap.
    inicializarnull(map);
    for (int i = 0; i < map->capacity/2; i++)
    {
        if (old_buckets[i]!=NULL && old_buckets[i]->key!=NULL)
        {
            insertMap(map,old_buckets[i]->key,old_buckets[i]->value);
        }
    }
    free(old_buckets);
    

}

 //1.- Implemente la función createMap en el archivo hashmap.c. 
 //Esta función crea una variable de tipo HashMap, 
 //inicializa el arreglo de buckets con casillas nulas,
 // inicializa el resto de variables y retorna el mapa.
 // Inicialice el índice current a -1.
void inicializarnull(HashMap * mapa){
    for (int i = 0; i < mapa->capacity; i++)
    {
        mapa->buckets[i]=NULL;
    }
    mapa->size=0;
    mapa->current=-1;
}

HashMap * createMap(long capacity) {
    HashMap * mapa = (HashMap * ) malloc(sizeof(HashMap));
    mapa->buckets = (Pair **)malloc(sizeof(Pair *)*capacity);
    inicializarnull(mapa);
    mapa->capacity=capacity;
    
    return mapa;
}
//4.- Implemente la función void eraseMap(HashMap * map, char * key).
// Está función elimina el dato correspondiente a la clave key. 
//Para hacerlo debe buscar el dato y luego marcarlo para que no sea válido.
// No elimine el par, sólo invalídelo asignando NULL a la clave (pair->key=NULL).
// Recuerde actualizar la variable size.
void eraseMap(HashMap * map,  char * key){
    long posicion = hash(key,map->capacity);
    //no se ecuentra posicion valida
    while (map->buckets[posicion]!=NULL){
        if (is_equal(map->buckets[posicion]->key,key)){
            map->buckets[posicion]->key=NULL;
            map->size--;
            return;
        }
        posicion++; // avanzar posicion
        if (posicion==map->capacity){
            posicion=0; //recorrer arreglo desde el inicio
        }
    }
}


//3.- Implemente la función Pair * searchMap(HashMap * map, char * key), 
//la cual retorna el Pair asociado a la clave ingresada.
//Recuerde que para buscar el par debe:
//a - Usar la función hash para obtener la posición donde puede encontrarse el par con la clave
//b - Si la clave no se encuentra avance hasta encontrarla (método de resolución de colisiones)
//c - Si llega a una casilla nula, retorne NULL inmediatamente (no siga avanzando, la clave no está)
//Recuerde actualizar el índice current a la posición encontrada. Recuerde que el arreglo es circular.


Pair * searchMap(HashMap * map,  char * key) {   
    long posicion = hash(key,map->capacity);
    //no se ecuentra posicion valida
    while (map->buckets[posicion]!=NULL){
        if (is_equal(map->buckets[posicion]->key,key)){
            map->current=posicion;
            return map->buckets[posicion];
        }

        posicion++; // avanzar posicion
        if (posicion==map->capacity){
            posicion=0; //recorrer arreglo desde el inicio
        }
    }
    return NULL;
}
//5.- Implemente las funciones para recorrer la estructura: Pair * firstMap(HashMap * map)
// retorna el primer Pair válido del arreglo buckets. 

Pair * firstMap(HashMap * map) {
    for (int i = 0; i < map->capacity; i++)
    {
        if (map->buckets[i]!=NULL && map->buckets[i]->key!=NULL)
        {
            map->current=i;
            return map->buckets[i];
        }
    }
    return  NULL ;
}
//Pair * nextMap(HashMap * map) retorna el siguiente Pair del arreglo buckets a partir índice current.
// Recuerde actualizar el índice.
Pair * nextMap(HashMap * map) {
    for (int i = map->current+1; i < map->capacity; i++)
    {
        if (map->buckets[i]!=NULL && map->buckets[i]->key!=NULL)
        {
            map->current=i;
            return map->buckets[i];
        }
    }
    return NULL;

}
