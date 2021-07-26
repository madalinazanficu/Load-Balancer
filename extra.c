/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#include <stdlib.h>
#include <string.h>
#include "extra.h"
#include "utils.h"

unsigned int hash_function_servers(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a)
{
	unsigned char *puchar_a = (unsigned char *) a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
	hash = ((hash << 5u) + hash) + c;

	return hash;
}

int compare_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;
	return strcmp(str_a, str_b);
}

// liner searching in HR, to get the position where new_server will be stocked
int position_in_hash_ring(load_balancer *main, hr_element new_server)
{
	doubly_linked_list_t *hash_ring = main->hash_ring;
	dll_node_t *node = hash_ring->head;
	if (node == NULL)
		return 0;
	int i = 0;
	while(((hr_element *)(node->data))->hash < new_server.hash &&
								i < hash_ring->size) {
		++i;
		node = node->next;
	}
	if (((hr_element *)(node->data))->hash == new_server.hash) {
		while (((hr_element *)(node->data))->server_id < new_server.server_id &&
								i < hash_ring->size) {
		node = node->next;
		++i;
		}
	}
	return i;
}

// linear searching in the doubly linked list for the hash position
int searching_hash(load_balancer *main, unsigned int hash)
{
	doubly_linked_list_t *hash_ring = main->hash_ring;
	dll_node_t *node = hash_ring->head;
	int i = 0;
	while (i < hash_ring->size && ((hr_element *)(node->data))->hash < hash) {
		++i;
		node = node->next;
	}
	return i;
}

void redistribute_server_objects_add(load_balancer* main, int poz)
{
	DIE(main == NULL, "No load_balancer in redistrubute_add!\n");
	// getting the left neighbor of the new_server added
	dll_node_t *prev_server = dll_get_nth_node(main->hash_ring, poz - 1);
	unsigned int prev_server_hash = ((hr_element *)(prev_server->data))->hash;

	// getting the right neighbor of the new_server added
	dll_node_t *server = dll_get_nth_node(main->hash_ring, poz + 1);

	dll_node_t *new_server = dll_get_nth_node(main->hash_ring, poz);
	unsigned int new_server_hash = ((hr_element *)(new_server->data))->hash;

	// corner case
	if (((hr_element *)(server->data))->server_id ==
				((hr_element *)(new_server->data))->server_id)
		return;

	// getting the hashtable from inside his right neighbor
	server_memory *ht_server = ((hr_element *)(server->data))->pServer;

	for (unsigned int i = 0; i < ht_server->hmax; i++) {
		ll_node_t *ht_node = ht_server->buckets[i]->head;
		while (ht_node != NULL) {
			// hash for the object key
			unsigned int object_hash =
						hash_function_key(((str_info *)(ht_node->data))->key);
			char *object_key = ((str_info *)(ht_node->data))->key;
			char *object_value = ((str_info *)(ht_node->data))->value;

			// hashtable behind the new-server added and his right neighbor
			server_memory *p_store = ((hr_element *)(new_server->data))->pServer;
			server_memory *p_remove = ((hr_element *)(server->data))->pServer;

			// corner case, adding a new server on the first position in the hash_ring
			if (((hr_element *)(new_server->data))->server_tag ==
				((hr_element *)(main->hash_ring->head->data))->server_tag) {
				// this is happening because I am on 0 degrees of the imaginary circle
				if (object_hash < new_server_hash || object_hash >= prev_server_hash) {
					server_store(p_store, object_key, object_value);
					ht_node = ht_node->next;
					server_remove(p_remove, object_key);
				}
				else
					ht_node = ht_node->next;
			} else {
				// object_hash should be in the interval (new_server_hash, prev_server_hash)
				if (object_hash < new_server_hash && object_hash >= prev_server_hash) {
					server_store(p_store, object_key, object_value);
					ht_node = ht_node->next;
					server_remove(p_remove, object_key);
				}
				else
					ht_node = ht_node->next;
			}
		}
	}
}

// balancing the objects before removing a server
void redistribute_server_objects_remove(load_balancer *main,
						server_memory *ht_rmw_server)
{
	DIE(main == NULL, "No load_balancer in redistribute_remove\n!");
	for (unsigned int i = 0; i < ht_rmw_server->hmax; i++) {
		ll_node_t *ht_node = ht_rmw_server->buckets[i]->head;
		while (ht_node != NULL) {
			char *object_key = ((str_info *)(ht_node->data))->key;
			char *object_value = ((str_info *)(ht_node->data))->value;
			int server_id;
			loader_store(main, object_key, object_value, &server_id);
			ht_node = ht_node->next;
		}
	}
}
