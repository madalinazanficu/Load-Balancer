/* Copyright 2021 <Madalina-Valentina Zanficu 313CA> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CircularDoublyLinkedList.h"
#include "utils.h"


doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
	DIE(list == NULL, "Malloc failed\n");
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;
	return list;
}


dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n)
{
	DIE(list == NULL, "No list \n");
	DIE(list->size == 0, "No elemnts in the list\n");
	if (n < 0)
		return list->head->prev;
	// finding out the position of the element
	int poz = n % list->size;
	// choosing the most efficient serach method
	int d1 = list->size - poz - 1;
	int d2 = poz;
	int p;
	dll_node_t *curr;
	 if (d2 < d1) {
		// from the head to the end
		curr = list->head;
		p = 0;
		while (p != poz) {
			curr = curr->next;
			p++;
		}
	} else {
		// from the end to the head
		curr = list->head->prev;
		p = list->size - 1;
		while (p != poz) {
			curr = curr->prev;
			p--;
		}
	}
	return curr;
}


void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* data)
{
	DIE(list == NULL, "No list\n");
	DIE(n < 0, "Error\n");
	dll_node_t *new = malloc(sizeof(dll_node_t));
	DIE(new == NULL, "Malloc failed\n");
	new->data = malloc(list->data_size);
	memcpy(new->data, data, list->data_size);
	if (n >= list->size) {
		n = list->size;
	}
	// corner case, empty list
	if (list->size == 0) {
		new->next = new;
		new->prev = new;
		list->head = new;
		list->size++;
		return;
	}
	// adding on the first position
	if (n == 0) {
		dll_node_t *prim_nod = list->head;
		dll_node_t *ultim_nod = list->head->prev;
		new->next = prim_nod;
		new->prev = ultim_nod;
		ultim_nod->next = new;
		prim_nod->prev = new;
		list->head = new;
	} else {
		// adding on the nth position
		dll_node_t *curr = list->head;
		int p = 0;
		int poz = n - 1;
		while(p != poz) {
			curr = curr->next;
			p++;
		}
		new->next = curr->next;
		new->prev = curr;
		curr->next->prev = new;
		curr->next = new;
	}
	list->size++;
}


dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
	DIE(list == NULL, "No list\n");
	DIE(n < 0, "Error\n");
	DIE(list->size == 0, "No nodes in list\n");
	if (n >= list->size - 1) {
		n = list->size - 1;
	}
	// only one element in the list
	if (list->size == 1) {
		dll_node_t *first = list->head;
		list->head = NULL;
		list->size--;
		return first;
	}
	// removing first element
	if (n == 0) {
		dll_node_t *first, *second;
		dll_node_t *last;
		first = list->head;
		second = list->head->next;
		last = list->head->prev;
		second->prev = last;
		last->next = second;
		list->head = second;
		list->size--;
		return first;
	} else {
		// removing any other element
		dll_node_t *curr = list->head;
		int p = 0;
		int poz = n;
		while(p != poz) {
			curr = curr->next;
			p++;
		}
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
		list->size--;
		return curr;
	}
}

unsigned int
dll_get_size(doubly_linked_list_t* list)
{
	DIE(list == NULL, "No list\n");
	return list->size;
}
// release the memory for the doubly linked list
void
dll_free(doubly_linked_list_t** pp_list)
{
	DIE(*pp_list == NULL, "No list\n");
	if ((*pp_list)->head == NULL) {
		free(*pp_list);
		return;
	}
	dll_node_t *first, *second;
	first = (*pp_list)->head->next;
	second = (*pp_list)->head;
	int p = 0;
	int poz = (*pp_list)->size - 1;
	while(p != poz) {
		free(second->data);
		free(second);
		second = first;
		first = first->next;
		p++;
	}
	free(second->data);
	free(second);
	free(*pp_list);
}
