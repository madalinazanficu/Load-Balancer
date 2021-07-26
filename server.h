/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#ifndef SERVER_H_
#define SERVER_H_
#include "LinkedList.h"
#include "utils.h"

typedef struct str_info str_info;
struct str_info {
	void *key;
	void *value;
};
typedef struct str_info str_info;

struct server_memory {
	// array of linked list
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
};
typedef struct server_memory server_memory;

server_memory* init_server_memory();

void free_list_inside_data(linked_list_t *list);

void free_server_memory(server_memory* server);

/**
 * server_store() - Stores a key-value pair to the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */

int ht_server_has_key(server_memory *server, char *key);

void server_store(server_memory* server, char* key, char* value);

/**
 * server_remove() - Removes a key-pair value from the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory* server, char* key);

/**
 * server_retrieve() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
// returneaza valoarea din hashtable de la key respectiva,
char* server_retrieve(server_memory* server, char* key);

int compare_strings(void *a, void *b);

unsigned int hash_function_string(void *a);

#endif  // SERVER_H_
