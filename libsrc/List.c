
#include <stdio.h>
#include <stdlib.h>
#include "List.h"

//! A Doubly-linked List Class
/*!
    Contains functions to manipulate a doubly-linked list.

    Author: nickbender

*/

static void print(const NodePtr node, char * (*toString)(const void *));

//! createList
/*!
  Creates a new list and returns a pointer to the list.
  \param compareTo a function pointer used for comparison of nodes.
  \param toString a function pointer used for String converstion.
  \param freeObject a function pointer used for deallocate generic object types.
*/
ListPtr createList(int(*compareTo)(const void *,const void *),
                   char * (*toString)(const void *),
                   void (*freeObject)(const void *)) {
  ListPtr list;
  list = (ListPtr) malloc(sizeof(List));
  list->size = 0;
  list->head = NULL;
  list->tail = NULL;
  list->compareTo = compareTo;
  list->toString = toString;
  list->freeObject = freeObject;
  return list;
}

//! freeList
/*!
  Frees each individual node and then frees the list's allocated memory.
  \param list a pointer to the list to be deallocated
*/
void freeList(const ListPtr list) {
  NodePtr node = list->head;

  while(node) {
    NodePtr next = node->next;
    freeNode(node, list->freeObject);
    node = next;
  }

  free(list);
}

//! getSize
/*!
  Returns the current size of the list
  \param list a pointer to the list being checked for size.
*/
int getSize(const ListPtr list) {
  return list->size;
}

//! isEmpty
/*!
  Returns the status of whether a list has elements.
  \param list a pointer to the list being checked if empty.
*/
Boolean isEmpty(const ListPtr list) {
  if (list->size == 0)
    return TRUE;
  else
    return FALSE;
}

//! addAtFront
/*!
  Adds a node to the front of a list
  \param list a pointer to the list being added too
  \param node a pointer to the node being added
*/
void addAtFront(ListPtr list, NodePtr node) {
  if ((list == NULL) || (node == NULL)) return;

  node->next = list->head;
  node->prev = NULL;

  if (list->head == NULL)
  {
    list->head = node;
    list->tail = node;
  } else {
    list->head->prev = node;
    list->head = node;
  }

  list->size++;
}

//! addAtRear
/*!
  Adds a node to the rear of a list
  \param list a pointer to the list being added too
  \param node a pointer to the node being added
*/
void addAtRear(ListPtr list, NodePtr node) {
  if ((list == NULL) || (node == NULL)) return;

  if (list->head == NULL) {
    list->head = node;
    list->tail = node;
  } else {
    node->prev = list->tail;
    node->next = NULL;

    list->tail->next = node;
    list->tail = node;
  }

  list->size++;
}

//! removeFront
/*!
  Removes the front node of a list
  \param list a pointer to the list being removed from
  \param node a pointer to the node being removed
*/
NodePtr removeFront(ListPtr list) {
  if (list == NULL) return NULL;
  if (list->head == NULL) return NULL;

  list->size--;

  NodePtr node = list->head;
  list->head = node->next;

  if (list->head)
    list->head->prev = NULL;
  if (list->tail == node)
    list->tail = NULL;

  node->next = NULL;
  node->prev = NULL;

  return node;
}

//! removeRear
/*!
  Removes the rear node of a list
  \param list a pointer to the list being removed from
  \param node a pointer to the node being removed
*/
NodePtr removeRear(ListPtr list) {
  if (list == NULL) return NULL;
  if (list->head == NULL) return NULL;

  list->size--;

  NodePtr node = list->tail;
  list->tail = node->prev;

  if (list->tail)
    list->tail->next = NULL;

  if (list->head == node)
    list->head = NULL;

  return node;
}

//! removeNode
/*!
  Removes a node from a given list
  \param list a pointer to the list being removed from
  \param node a pointer to the node being removed
*/
NodePtr removeNode(ListPtr list, NodePtr node) {
  if ((list == NULL) || (node == NULL) || (list->size == 0)) return NULL;

  if (list->tail == node)
    return removeRear(list);

  if (list->head == node)
    return removeFront(list);

  list->size--;

  node->prev->next = node->next;
  node->next->prev = node->prev;

  return node;
}
//! search
/*!
  searches a given list for a node containing a given object
  \param list a pointer to the list being searched
  \param obj a pointer to an object being searched for
*/
NodePtr search(const ListPtr list, const void *obj) {
  if ((list == NULL) || (obj == NULL) || (list->head == NULL)) return NULL;

  NodePtr node = list->head;

  while(node) {
    if (list->compareTo(node->obj, obj))
      return node;

    node = node->next;
  }

  return NULL;
}

//! reverseList
/*!
  Reverses the order of a given list
  \param list a pointer to the list being searched
*/
void reverseList(ListPtr list) {
  if ((list == NULL) || (list->size == 0) || (list->head == NULL)) return;

  NodePtr current = list->head;
  NodePtr next;

  list->head = list->tail;
  list->tail = current;

  while(current) {
    next = current->next;
    current->next = current->prev;
    current->prev = next;
    current = next;
  }
}

//! printList
/*!
  Prints a human readable list of a list and it's contents
  \param list a pointer to the list being printed
*/
void printList(const ListPtr list) {
  if (list) print(list->head, list->toString);
}

//! print
/*!
  Prints a human readable representation of a given node's object
  \param list a pointer to the node being printed
  \param toString a function pointer to the toString method in use
*/
static void print(const NodePtr node, char * (*toString)(const void *)) {
  int count = 0;
  char *output;
  NodePtr temp = node;

  while (temp) {
    output = (*toString)(temp->obj);
    printf(" %s -->",output);
    free(output);
    temp = temp->next;
    count++;

    if ((count % 6) == 0)
      printf("\n");
  }

  printf(" NULL \n");
}
