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

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;
using std::cerr;
using std::endl;

namespace hw3 {

QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int     rank;    // The rank of the result so far.
} IdxQueryResult;

static vector<IdxQueryResult> ProcessSingleIndex(IndexTableReader* const 
  idx_reader_arr[], int i, const vector<string>& query);

static void ProcessQueryWord(const DocIDTableReader* dtr, 
  vector<IdxQueryResult>* index_query_res);

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;

  for (int i = 0; i < array_len_; i++) {
    if (!itr_array_[i]) {
      cerr << "IndexTableReader is null for index " << i << endl;
      continue;
    }

    vector<IdxQueryResult> q_query;
    q_query = ProcessSingleIndex(itr_array_, i, query);

    for (const IdxQueryResult & res : q_query) {
      string filename;

      Verify333(dtr_array_[i]->LookupDocID(res.doc_id, &filename));

      bool found = false;
      for (const auto& curr_res : final_result) {
        if (curr_res.document_name == filename) {
          found = true;
          break;
        }
      }
      if (!found) {
        final_result.push_back({filename, res.rank});
      }
    }
  }
  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

static vector<IdxQueryResult> ProcessSingleIndex(IndexTableReader* const 
  idx_reader_arr[], int i, const vector<string>& query) {

  std::cout << "Processing index: " << i << " for word: " << query[0] << 
    std::endl;

  const DocIDTableReader* doc_id_reader = idx_reader_arr[i]->LookupWord
  (query[0]);

  vector<IdxQueryResult> idx_reader_list;

  if (!doc_id_reader) {
    std::cout << "Word not found in index: " << query[0] << std::endl;
    return idx_reader_list;
  }

list<DocIDElementHeader> doc_header_list = doc_id_reader->GetDocIDList();

for (const DocIDElementHeader& doc_header : doc_header_list) {
  idx_reader_list.push_back({doc_header.doc_id, doc_header.num_positions});
}

delete doc_id_reader;

if (query.size() <= 1) {
  return idx_reader_list;
}

size_t idx = 1;

while(idx < query.size()) {
  DocIDTableReader* doc_id_reader = idx_reader_arr[i]->LookupWord(query[idx]);

  if (!doc_id_reader) {
    idx_reader_list.clear();
    break;
  }

  ProcessQueryWord(doc_id_reader, &idx_reader_list);

  delete doc_id_reader;
  idx++;
  }
  return idx_reader_list;
}

static void ProcessQueryWord(const DocIDTableReader* doc_table_reader, 
                    vector<IdxQueryResult>* index_query_res) {
  list<DocPositionOffset_t> doc_pos_offsest;
  auto iter = index_query_res->begin();
  while (iter != index_query_res->end()) {
    if (doc_table_reader->LookupDocID(iter->doc_id, &doc_pos_offsest)) {
      iter->rank += doc_pos_offsest.size();
      ++iter;
    } else {
      iter = index_query_res->erase(iter);
    }
  }
}


}  // namespace hw3
