#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


//malloc check
void *safe_malloc(size_t size){
    void *ptr = malloc(size);

    if (!ptr && (size > 0)) {
      perror("malloc failed!");
      exit(EXIT_FAILURE);
    }
    return ptr;
}



//Node
typedef struct sNode {
	char* key;
	char* value;
	struct sNode* next;
} sNode;

/*typedef struct node1 {
    int value;
    struct node1 *link;
    pthread_mutex_t lock;
} node1_t;

node1_t ListHead; */

//LL
typedef struct sevLl{ //queue struct
	sNode* head; 
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready; //might not need
} sevLl;




//init
int ll_init(sevLl *L){
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
	pthread_cond_init(&L->read_ready, NULL);
	pthread_cond_init(&L->read_ready, NULL);
	return 0;
}

//delete list
int ll_destroy(sevLl *L){
	//delete all keys
	sNode* cur;
	sNode* after;
	cur = L->head;
	pthread_mutex_lock(&L->lock);
	
	while (cur != NULL) {
		after = cur->next;
		free(cur);
		cur = after;		
	}
	L->head = NULL;
	
	pthread_mutex_unlock(&L->lock);	
	//delete mutex
	pthread_mutex_destroy(&L->lock);
	pthread_cond_destroy(&L->read_ready);
	pthread_cond_destroy(&L->write_ready);
	return 0;
}

//alpha ins
int ll_ins(sevLl *L, char* k, char* v){
	//create node
	sNode* new = safe_malloc(sizeof(sNode));
	new->key = safe_malloc(strlen(k) + 1);
	new->value = safe_malloc(strlen(v) + 1);
	//if (new == NULL || new->key == NULL || v1 == NULL) return 1 //malloc failed
	strcpy(new->key, k);
	strcpy(new->value, v);		
	new->next = NULL;	
	
	sNode* cur = L->head;
	sNode* prev;
	
	//lock mutex
	pthread_mutex_lock(&L->lock);
	
	//if empty
	if (L->head == NULL) {
		L->head = new;
	}
	
	else if (strcmp(new->key, cur->key) < 0) {
		new->next = cur;
		L->head = new;			
	}
	
	else {
		while (cur != NULL) {
			if (strcmp(new->key, cur->key) <= 0) {
				if (strcmp(new->key, cur->key) == 0) { //reseting value
					cur->value = safe_malloc(strlen(v) + 1);
					strcpy(cur->value, v);
					free(new->value);
					free(new->key);
					free(new);
					pthread_cond_signal(&L->read_ready);
					pthread_cond_signal(&L->write_ready);
					pthread_mutex_unlock(&L->lock);
					return 0;
				}
				
				else {
					new->next = cur;
					prev->next = new;
					break;
				}				
			}			
			prev = cur;
			cur = cur->next;			
		}
		//end
		prev->next = new;		
	}	
	//unlock mutex signal ready
	pthread_cond_signal(&L->read_ready);
	pthread_cond_signal(&L->write_ready);
	pthread_mutex_unlock(&L->lock);
	return 0;
}



//read
char *ll_read(sevLl *L, char *k) {
	sNode *cur = L->head;
	char* val;
	while (cur != NULL) {
		if (strcmp(k, cur->key) == 0) {
			val = cur->value;			
			return val;
		}
		cur = cur->next;
	}
	return NULL;
}



//remove
char *ll_del(sevLl *L, char *k) {
	sNode* cur;
	sNode* prev;
	cur = L->head;
	prev = NULL;
	char *val;
	
	//lock mutex
	pthread_mutex_lock(&L->lock);
	
	while (cur != NULL) {
		if (strcmp(k, cur->key) == 0) {			
			if (prev == NULL){ //del head
				val = cur->value;
				L->head = cur->next;
			}
			
			else { 
				val = cur->value;
				prev->next = cur->next;				
			}
			free(cur->value);
			free(cur->key);
			free(cur);
			pthread_cond_signal(&L->read_ready);
			pthread_cond_signal(&L->write_ready);
			pthread_mutex_unlock(&L->lock);
			return val;			
		}
		prev = cur;
		cur = cur->next;
	}
	
	//key DNE
	pthread_cond_signal(&L->read_ready);
	pthread_cond_signal(&L->write_ready);
	pthread_mutex_unlock(&L->lock);
	
	return NULL;	
}

//print list
void ll_print (sevLl* L) {
	sNode *cur = L->head;
	while (cur != NULL) {
		printf("key: %s, value: %s\n", cur->key, cur->value);
		cur = cur->next;		
	}
}









//MAIN
int main(int argc, char * const argv[]) {
	sevLl L;
	ll_init(&L);
	
	ll_ins(&L, "dog", "glenn");
	ll_ins(&L, "cat", "tabby");
	ll_ins(&L, "gf", "Jan");
	ll_ins(&L, "team", "pats");
	ll_ins(&L, "cat", "Gin");
	
	ll_del(&L, "dog");
	ll_del(&L, "gf");
	
	ll_ins(&L, "home", "apt");
	
	ll_print(&L);
	
	
	ll_destroy(&L);
}