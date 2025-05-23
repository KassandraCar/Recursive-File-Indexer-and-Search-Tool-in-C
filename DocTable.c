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
#include "./DocTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhw1/CSE333.h"
#include "libhw1/HashTable.h"

#define HASHTABLE_INITIAL_NUM_BUCKETS 2

// This structure represents a DocTable; it contains two hash tables, one
// mapping from document id to document name, and one mapping from
// document name to document id.
struct doctable_st {
  HashTable* id_to_name;  // mapping document id to document name
  HashTable* name_to_id;  // mapping document name to document id
  DocID_t    max_id;      // max doc ID allocated so far
};

DocTable* DocTable_Allocate(void) {
  DocTable* dt = (DocTable*) malloc(sizeof(DocTable));
  Verify333(dt != NULL);

  dt->id_to_name = HashTable_Allocate(HASHTABLE_INITIAL_NUM_BUCKETS);
  dt->name_to_id = HashTable_Allocate(HASHTABLE_INITIAL_NUM_BUCKETS);
  dt->max_id = 1;  // we reserve max_id = 0 for the invalid docID

  return dt;
}

void DocTable_Free(DocTable* table) {
  Verify333(table != NULL);

  // STEP 1.
    HashTable_Free(table->id_to_name, &free);
    HashTable_Free(table->name_to_id, &free);
    free(table);
}

int DocTable_NumDocs(DocTable* table) {
  Verify333(table != NULL);
  return HashTable_NumElements(table->id_to_name);
}

char* custom_strdup(const char* s) {
    if (s == NULL) {
        return NULL;
    }
    char* copy = malloc(strlen(s) + 1);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}

DocID_t DocTable_Add(DocTable* table, char* doc_name) {
  Verify333(table != NULL);
  HTKey_t key = FNVHash64((unsigned char*)doc_name, strlen(doc_name));

  HTKeyValue_t kv, old_kv;

  // STEP 2.
  // Check to see if the document already exists.  Then make a copy of the
  // doc_name and allocate space for the new ID.

  if (HashTable_Find(table->name_to_id, key, &kv)) {
    return *(DocID_t*)kv.value;
  }

  char* doc_copy = custom_strdup(doc_name);
  if (doc_copy == NULL) {
      return INVALID_DOCID;
  }

    // STEP 3.
    // Set up the key/value for the id->name mapping, and do the insert.

  DocID_t new_id = table->max_id++;
  DocID_t* p_new_id = malloc(sizeof(DocID_t));
  *p_new_id = new_id;

  kv.key = new_id;
  kv.value = doc_copy;
  HashTable_Insert(table->id_to_name, kv, &old_kv);

  // STEP 4.
  // Set up the key/value for the name->id, and/ do the insert.
  // Be careful about how you calculate the key for this mapping.
  // You want to be sure that how you do this is consistent with
  // the provided code.
  kv.key = key;
  kv.value = p_new_id;
  HashTable_Insert(table->name_to_id, kv, &old_kv);

  return new_id;
}

DocID_t DocTable_GetDocID(DocTable* table, char* doc_name) {
  Verify333(table != NULL);
  Verify333(doc_name != NULL);

  HTKey_t key = FNVHash64((char*)doc_name, strlen(doc_name));

  HTKeyValue_t kv;
  //DocID_t res;

  // STEP 5.
  // Try to find the passed-in doc in name_to_id table.
  if (HashTable_Find(table->name_to_id, key, &kv)) {
      return *(DocID_t*)kv.value;
  }
  return INVALID_DOCID;
}

char* DocTable_GetDocName(DocTable* table, DocID_t doc_id) {
  Verify333(table != NULL);
  Verify333(doc_id != INVALID_DOCID);

  HTKeyValue_t kv;

  // STEP 6.
  // Lookup the doc_id in the id_to_name table,
  // and either return the string (i.e., the (char *)
  // saved in the value field for that key) or
  // NULL if the key isn't in the table.
  if (HashTable_Find(table->id_to_name, (HTKey_t)doc_id, &kv)) {
    return (char*)kv.value;
  }
  return NULL;
}

HashTable* DT_GetIDToNameTable(DocTable* table) {
  Verify333(table != NULL);
  return table->id_to_name;
}

HashTable* DT_GetNameToIDTable(DocTable* table) {
  Verify333(table != NULL);
  return table->name_to_id;
}