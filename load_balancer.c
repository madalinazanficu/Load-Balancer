/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#include <stdlib.h>
#include <string.h>
#include "load_balancer.h"
#include "extra.h"
#define MAX_SIZE 1e+5


// load_balancer's initialization, creating my dll
load_balancer* init_load_balancer()
{
	load_balancer *load_balancer_main = malloc(sizeof(load_balancer));
	DIE(load_balancer_main == NULL, "Malloc failed in init_load_balancer!\n");
	load_balancer_main->hash_ring = dll_create(sizeof(hr_element));
	return load_balancer_main;
}


// searching the server (after his hash) to add a new entry in ht
void loader_store(load_balancer* main, char* key, char* value, int* server_id)
{
	DIE(main == NULL, "No load_balancer in loader_store!\n");
	unsigned int search_hash = hash_function_key(key);
	int poz = searching_hash(main, search_hash);

	dll_node_t *server = dll_get_nth_node(main->hash_ring, poz);
	*server_id = ((hr_element*)(server->data))->server_id;
	server_store(((hr_element*)(server->data))->pServer, key, value);
}

// searching in the hash_ring the server which contain the desired key
char* loader_retrieve(load_balancer* main, char* key, int* server_id)
{
	DIE(main == NULL, "No load_balancer in loader_retreieve");
	unsigned search_hash = hash_function_key(key);
	int poz = searching_hash(main, search_hash);

	dll_node_t *server = dll_get_nth_node(main->hash_ring, poz);
	*server_id = ((hr_element*)(server->data))->server_id;

	server_memory *ht_server = ((hr_element *)(server->data))->pServer;
	return server_retrieve(ht_server, key);
}


// adding a new server in the system
void loader_add_server(load_balancer* main, int server_id)
{
	DIE(main == NULL, "No load_balancer in add_server!\n");
	// initializating a hr_element (hash_ring element)
	hr_element new_server;
	unsigned int tag = 0 * MAX_SIZE + server_id;
	new_server.server_id = server_id;
	new_server.server_tag = tag;
	new_server.hash = hash_function_servers(&tag);
	new_server.pServer = init_server_memory();
	int poz = position_in_hash_ring(main, new_server);
	dll_add_nth_node(main->hash_ring, poz, &new_server);

	// checking his neighbors in order to balance the objects from his hashtable
	redistribute_server_objects_add(main, poz);


	// initializating the new_server duplicates
	hr_element copy1_new_server;
	tag = 1 * MAX_SIZE + server_id;
	copy1_new_server.server_id = server_id;
	copy1_new_server.server_tag = tag;
	copy1_new_server.hash = hash_function_servers(&tag);
	copy1_new_server.pServer = new_server.pServer;
	poz = position_in_hash_ring(main, copy1_new_server);
	dll_add_nth_node(main->hash_ring, poz, &copy1_new_server);

	// checking his neighbors in order to balance the objects from his hashtable
	redistribute_server_objects_add(main, poz);


	hr_element copy2_new_server;
	tag = 2 * MAX_SIZE + server_id;
	copy2_new_server.server_id = server_id;
	copy2_new_server.server_tag = tag;
	copy2_new_server.hash = hash_function_servers(&tag);
	copy2_new_server.pServer = new_server.pServer;
	poz = position_in_hash_ring(main, copy2_new_server);
	dll_add_nth_node(main->hash_ring, poz, &copy2_new_server);

	// checking his neighbors in order to pass the objects from his hashtable
	redistribute_server_objects_add(main, poz);
}

// removing a server and his duplicates from the system
void loader_remove_server(load_balancer* main, int server_id)
{
	DIE(main == NULL, "No load_balancer in remove_server!\n");
	doubly_linked_list_t *hash_ring = main->hash_ring;
	dll_node_t *server = hash_ring->head;
	dll_node_t *rmw_server;
	unsigned int i = 0, ok = 0, j;
	server_memory *save_pServer = NULL;
	for (j = 0; j < 3; j++) {
		server = hash_ring->head;
		i = 0;
		ok = 1;
		while (ok) {
			if (((hr_element *)(server->data))->server_id == server_id) {
				save_pServer = ((hr_element *)(server->data))->pServer;
				ok = 0;
				rmw_server = dll_remove_nth_node(hash_ring, i);
				free(rmw_server->data);
				free(rmw_server);
			} else {
				server = server->next;
				i++;
			}
		}
	}
	// balancing the server's objects
	redistribute_server_objects_remove(main, save_pServer);
	free_server_memory(save_pServer);
}

// memeory release for the load_balancer
void free_load_balancer(load_balancer* main)
{
	DIE(main == NULL, "No load_balancer in free_load_balancer");
	doubly_linked_list_t *hash_ring = main->hash_ring;
	dll_node_t *server = hash_ring->head;
	dll_node_t *rmw_server;
	server_memory *save_pServer = NULL;

	int i, ok, j;
	while (hash_ring->size > 0) {
		server = hash_ring->head;
		int server_id = ((hr_element *)(server->data))->server_id;
		// memory realease for the original server, and his copies
		for (j = 0; j < 3; j++) {
			server = hash_ring->head;
			i = 0;
			ok = 1;
			while (ok) {
				if (((hr_element *)(server->data))->server_id == server_id) {
					save_pServer = ((hr_element *)(server->data))->pServer;
					ok = 0;
					rmw_server = dll_remove_nth_node(hash_ring, i);
					free(rmw_server->data);
					free(rmw_server);
				} else {
					server = server->next;
					i++;
				}
			}
		}
		// memory realease for the hashtable behind the original server
		free_server_memory(save_pServer);
	}
	// memory realease for  the hash_ring
	dll_free(&main->hash_ring);
	free(main);
}

