/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "extra.h"
#define HMAX 100

// hashtable initialization
server_memory* init_server_memory()
{
	server_memory *ht_server = malloc(sizeof(server_memory));
	DIE(ht_server == NULL, "Malloc failed in server_memory!\n");

	ht_server->buckets = malloc(HMAX * sizeof(linked_list_t *));
	for (int i = 0; i < HMAX; i++) {
		ht_server->buckets[i] = ll_create(sizeof(str_info));
	}

	ht_server->size = 0;
	ht_server->hmax = HMAX;
	return ht_server;;
}

// checking if a key is already in the ht_server
int ht_server_has_key(server_memory *server, char *key)
{
	DIE(server == NULL, "No server in ht_server_has_key!\n");
	int index_key = hash_function_key(key) % server->hmax;
	ll_node_t *node = server->buckets[index_key]->head;
	while (node != NULL) {
		if (compare_strings(((str_info *)(node->data))->key, key) == 0) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

// adding a new entry (key, value) in the hashtable
void server_store(server_memory* server, char* key, char* value)
{
	DIE(server == NULL, "No server in server_store!\n");
	// searching for the position in the hashtable
	int index_key = hash_function_key(key) % server->hmax;
	linked_list_t *list = server->buckets[index_key];

	// if my key is already in bucket => update for value
	if (ht_server_has_key(server, key) == 1) {
		ll_node_t *node = list->head;
		while(node != NULL && compare_strings(((str_info *)
								(node->data))->key, key) != 0) {
			node = node->next;
		}
		memcpy(((str_info *)(node->data))->value, value, strlen(value));
	} else {
		// if I have't found the key, I add (key, value) to the list
		str_info new_info;
		new_info.key = malloc(strlen(key) + 1);
		new_info.value = malloc(strlen(value) + 1);
		DIE(new_info.key == NULL, "Malloc failed in server_store!\n");
		DIE(new_info.value == NULL, "Malloc failed in server_store!\n");
		memcpy(new_info.key, key, strlen(key) + 1);
		memcpy(new_info.value, value, strlen(value) + 1);
		ll_add_nth_node(list, list->size, &new_info);
		server->size++;
	}
}

// removing a (key, value) entry from the hashtable
void server_remove(server_memory* server, char* key)
{
	DIE(server == NULL, "No server in server_remove!\n");
	int index_key = hash_function_key(key) % server->hmax;
	if (ht_server_has_key(server, key) == 0) {
		return;
	}
	ll_node_t *rmw = server->buckets[index_key]->head;
	int poz_rmw = 0;
	while(rmw != NULL &&
				compare_strings(((str_info *)(rmw->data))->key, key) != 0) {
		rmw = rmw->next;
		poz_rmw++;
	}
	rmw = ll_remove_nth_node(server->buckets[index_key], poz_rmw);
	free(((str_info *)(rmw->data))->key);
	free(((str_info *)(rmw->data))->value);
	free(rmw->data);
	free(rmw);
	server->size--;
}

// getting the value from ht_server, associated with key
char* server_retrieve(server_memory* server, char* key)
{
	DIE(server == NULL, "No server in server_retrieve!\n");
	int index_key = hash_function_key(key) % server->hmax;
	ll_node_t *node = server->buckets[index_key]->head;
	if (node == NULL) {
		return NULL;
	} else {
		while (node != NULL &&
				compare_strings(((str_info *)(node->data))->key, key) != 0) {
			node = node->next;
		}
		if (node == NULL) {
			return NULL;
		}
		return ((str_info *)(node->data))->value;
	}
	return NULL;
}

void free_list_inside_data(linked_list_t *list)
{
	DIE(list == NULL, "No list!\n");
	ll_node_t *node = list->head;
	for(int i = 0; i < list->size; i++) {
		free(((str_info *)(node->data))->key);
		free(((str_info *)(node->data))->value);
		node = node->next;
	}
}
// relesae the memory for the hashtable
void free_server_memory(server_memory* server)
{
	DIE(server == NULL, "No server in free_server_memory");
	for (unsigned int i = 0; i < server->hmax; i++) {
		free_list_inside_data(server->buckets[i]);
		ll_free(&server->buckets[i]);
	}
	free(server->buckets);
	free(server);
}


