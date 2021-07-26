/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

linked_list_t*
ll_create(unsigned int data_size)
{
	linked_list_t *list = malloc(sizeof(linked_list_t));
	DIE(list == NULL, "Memory ups");
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;
	return list;
}

void
ll_add_nth_node(linked_list_t* list, int n, const void* new_data)
{
	DIE(list == NULL, "No list\n");
	ll_node_t *nod = malloc(sizeof(ll_node_t));
	DIE(nod == NULL, "Memory ups\n");
	nod->data = malloc(list->data_size);
	memcpy(nod->data, new_data, list->data_size);

	if (n >= list->size) {
		n = list->size;
	}
	// adding on the first position
	if (n == 0) {
		nod->next = list->head;
		list->head = nod;
		list->size++;

	} else {
		// adding on the n position
		ll_node_t *current;
		current = list->head;
		for (int i = 0; i < n - 1; i++) {
			current = current->next;
		}
		nod->next = current->next;
		current->next = nod;
		list->size++;
	}
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, int n)
{
	DIE(list == NULL, "No list\n");
	ll_node_t *nod;
	DIE(n < 0, "Error\n");
	// only one node in the list
	if (list->size == 1) {
		nod = list->head;
		list->head = NULL;
		list->size--;
		return nod;
	}
	if (n >= list->size -1) {
		n = list->size - 1;
	}
	// removing the first node
	if (n == 0) {
		nod = list->head;
		list->head = nod->next;
		list->size--;
		return nod;

	} else {
		// removing the nth node
		ll_node_t *prev;
		nod = list->head->next;
		prev = list->head;
		for (int i = 0; i < n - 1; i++) {
			prev = nod;
			nod = nod->next;
		}
		prev->next = nod ->next;
		list->size--;
		return nod;
	}
}

unsigned int
ll_get_size(linked_list_t* list)
{
	DIE(list == NULL, "No list\n");
	return list->size;
}

void
ll_free(linked_list_t** pp_list)
{
	DIE(*pp_list == NULL, "No list\n");
	if ((*pp_list)->head == NULL) {
		free(*pp_list);
		return;
	}
	ll_node_t *nod, *prev;
	nod = (*pp_list)->head->next;
	prev = (*pp_list)->head;
	while(nod != NULL) {
		free(prev->data);
		free(prev);
		prev = nod;
		nod = nod->next;
	}
	free(prev->data);
	free(prev);
	free(*pp_list);
	*pp_list = NULL;
}
