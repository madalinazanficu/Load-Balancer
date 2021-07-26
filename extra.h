/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#ifndef EXTRA_H_
#define EXTRA_H_
#include "server.h"
#include "load_balancer.h"

unsigned int hash_function_servers(void *a);

unsigned int hash_function_key(void *a);

int compare_strings(void *a, void *b);

int position_in_hash_ring(load_balancer *main, hr_element new_server);

int searching_hash(load_balancer *main, unsigned int hash);

void redistribute_server_objects_add(load_balancer* main, int poz);

void redistribute_server_objects_remove(load_balancer *main,
						server_memory *ht_rmw_server);

#endif  // EXTRA_H_
