/*
 * Copyright ©2024 Hannah C. Tang.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2024 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  ll->head = NULL;
  ll->tail = NULL;
  ll->num_elements = 0;

  // Return our newly minted linked list.
  
  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves.

  LinkedListNode *current = list->head;
  LinkedListNode *next;
  
  while (current != NULL) {
    next = current->next;
    payload_free_function(current->payload);
    free(current);
    current = next;
  }
  
  // free the LinkedList
  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

  // Allocate space for the new node.
void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
  } else {
    // STEP 3: typical case; list has >=1 elements
    ln->prev = NULL;
    ln->next = list->head;
    list->head->prev = ln;
    list->head = ln;
  }
  list->num_elements++;
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().
  if (list->num_elements == 0) {
    return false;
  }

  *payload_ptr = list->head->payload;
  LinkedListNode *node_to_remove = list->head;
  if (list->num_elements == 1) {
    list->head = list->tail = NULL;
  } else {
    list->head = node_to_remove->next;
    list->head->prev = NULL;
  }
  free(node_to_remove);
  list->num_elements -= 1;

  return true; 
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.
  LinkedListNode *new_node = (LinkedListNode *)malloc(sizeof(LinkedListNode));
  Verify333(new_node != NULL);

  new_node->payload = payload;
  new_node->next = NULL;

  if (list->num_elements == 0) {
    new_node->prev = NULL;
    list->head = list->tail = new_node;
  } else {
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
  }
  list->num_elements++;
}

void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return (iter->node != NULL);
}

bool LLIterator_Next(LLIterator *iter) {
    Verify333(iter != NULL);
    Verify333(iter->list != NULL);
    Verify333(iter->node != NULL);

    // STEP 6: try to advance iterator to the next node
    // and return true if you succeed, false otherwise.
    // If the iterator is already at the last node,
    // move the iterator past the end of the list.

    if (iter->node->next != NULL) {
        iter->node = iter->node->next;
        return true;
    } else {
        iter->node = NULL;
        return false;
    }
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter, LLPayloadFreeFnPtr payload_free_function) {
    Verify333(iter != NULL);
    Verify333(iter->list != NULL);
    Verify333(iter->node != NULL);

    // STEP 7: implement LLIterator_Remove.
    LinkedList *list = iter->list;
    LinkedListNode *nodeToRemove = iter->node;
    bool nodeRemovalTail = nodeToRemove == list->tail;

    if (list->num_elements == 1) {
        list->head = list->tail = NULL;
        iter->node = NULL;
    } else if (nodeToRemove == list->head) {
        list->head = nodeToRemove->next;
        list->head->prev = NULL;
        iter->node = list->head; 
    } else {
        nodeToRemove->prev->next = nodeToRemove->next;
        if (!nodeRemovalTail) {
            nodeToRemove->next->prev = nodeToRemove->prev;
            iter->node = nodeToRemove->next;
        }
    }
    if (nodeRemovalTail) {
        iter->node = nodeToRemove->prev;
        list->tail = iter->node;
        if (list->tail) list->tail->next = NULL;
    }
    payload_free_function(nodeToRemove->payload);
    free(nodeToRemove);
    list->num_elements -= 1;

    return iter->node != NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LLSlice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LLSlice.
  if (list->num_elements == 0) {
        return false;
  }

  LinkedListNode *nodeToRemove = list->tail;
  *payload_ptr = nodeToRemove->payload;

    // list->head = list->head->next;
  if (list->num_elements == 1) {
        list->head = list->tail = NULL;
  } else {
      list->tail = nodeToRemove->prev;
      list->tail->next = NULL;
  }
  free(nodeToRemove);

  list->num_elements--;

  return true;
}

void LLIteratorRewind(LLIterator *iter) {
  iter->node = iter->list->head;
}

