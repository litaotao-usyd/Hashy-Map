#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashmap.h"

struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*)) {
         if(hash == NULL || cmp == NULL || key_destruct == NULL || value_destruct == NULL) {
		return NULL;
		}//if nothing exist already past all null test

        struct hash_map *map = (struct hash_map *)malloc(sizeof(struct hash_map));
        map->size = 32; // size need to be reset later
	map->arr = (struct node **)malloc(sizeof(struct node *) * 16); //create an array to store element for the board
	map->hash = hash;
	map->cmp = cmp;
	map->key_destruct = key_destruct;
	map->value_destruct = value_destruct;//call each variable in the function 
        for(int i = 0; i < map->size; i++) {
	  		map->arr[i] = NULL;
	  			}
	return map;
}

void hash_map_put_entry_move(struct hash_map* map, void* k, void* v) {
       //lets assume both k and v are not zero in order to process further
       assert(k!=NULL);
       assert(v!=NULL);

       size_t hash_value = map->hash(k) % map->size; //get hash value, if k equals to zero error will occur
       struct node *arr = map->arr[hash_value];
       struct node * temp = arr;

       while(temp != NULL && map->cmp(k,temp->key)!= 0){
       if(map->cmp(k,temp->key) == 1){
          
	   hash_map_remove_entry(map,k);
	   if(temp!= NULL){
	   temp = NULL;
	   }
	  }
            temp = temp->next;
       }
       //set a node to put value in 
       struct node *node = (struct node *)malloc(sizeof(struct node));
       node->key = k;
       node->value = v;
       node->next = arr;
       map->arr[hash_value] = node;

}

void hash_map_remove_entry(struct hash_map* map, void* k) {
	 assert(k!=NULL);
	 size_t hash_value = map->hash(k) % map->size;

	 struct node *arr = map->arr[hash_value];
	 struct node * temp = arr;
	 struct node *next_n = temp->next;

	    while(temp != NULL && map->cmp(k,temp->key)!= 0){
		 if(map->cmp(k,temp->key) == 1){
			if(temp == arr){ //head node
		         pthread_mutex_lock(&(temp->lock));
			 map->key_destruct(temp->key);
			 map->value_destruct(temp->value);
			 pthread_mutex_unlock(&(temp->lock));
			 free(temp);
			 temp = NULL;
			 map->arr[hash_value] = NULL;
			 return;
			}else{ //if next exist
			pthread_mutex_lock(&(next_n->lock));
			pthread_mutex_lock(&(temp->lock));
			map->key_destruct(temp->key);
			map->value_destruct(temp->value);
			pthread_mutex_unlock(&(temp->lock));
			free(temp);
			temp = NULL;
			map->arr[hash_value] = next_n;
			pthread_mutex_unlock(&(next_n->lock));
			return;
			}   
			map->key_destruct(temp->key);
			map->value_destruct(temp->value);
			free(temp);
			temp = NULL;
			return;
			   }
	    temp = temp->next;
	    }
}

//not done yet keycmp() method need to be done before this function work
void* hash_map_get_value_ref(struct hash_map* map, void* k) {
         assert(k!=NULL);
	 size_t hash_value = map->hash(k) % map->size; //get hash value
	 struct node *arr = map->arr[hash_value];
	 struct node *temp = arr;
	       while(temp != NULL && map->cmp(temp->key,k) != 0){
		       if(map->cmp(temp->key,k)== 1){ //if key exist
		            return temp->value; //return its value
		       } 
		       temp = temp -> next;//check the next node
					             }
	 return NULL;
}

//did not work need to come back later
void hash_map_destroy(struct hash_map* map) {
    if(map != NULL){
	for(int i = 0; i < map->size; i++){ //loop thr all position   
     	struct node *arr = map->arr[i];
	struct node *temp = arr;//set temp value to delete
        struct node *value = value;
        struct node *key = key;	
             while(temp != NULL){ //if we can delete
		struct node *delete = temp; 
		temp = temp->next;
		map->key_destruct(delete->key);
		map->value_destruct(delete->value);
		free(delete);
		delete = NULL;
		}
		arr = NULL;
		map->arr[i] = arr;//set current position to NULL to delete
	}
		free(map->arr);//after every position in the hash array is removrd free the array
	
    }	
    free(map);

}