# C File Indexer & Search Engine

This project implements a full-text file indexer and search engine in pure C/C++. 
Given one or more index files (produced by a recursive crawler and tokenizer), 
the tool supports ranked search queries using an interactive shell. All indexing and searching logic is built 
from scratch, including custom on-disk data structures, hash table serialization, and inverted index management.

> Originally developed for CSE 333: Systems Programming at the University of Washington.

---

## Project Structure
 
Indexing Pipeline:

`CrawlFileTree.c`: Recursively traverses a directory to gather documents.

`FileParser.c`, `DocTable.c`, `MemIndex.c`: Tokenize files, assign document IDs, and construct an in-memory inverted index.

`WriteIndex.c`: Serializes the in-memory index to a compact, binary format with a CRC checksum.

Search Engine:
`QueryProcessor.`: Loads one or more index files and performs ranked multi-word queries.

`searchshell.c`: Interactive shell for CLI-based search.

HTTP Search Server:

`http333d.cc`, `HttpServer.cc`, `HttpConnection.cc`: Implements a basic HTTP server that supports GET queries.

`HttpUtils.cc`, `ServerSocket.cc`: Handle socket setup and HTTP parsing.

Reader Infrastructure
`FileIndexReader.c`, `IndexTableReader.c`, `DocIDTableReader.c`: Low-level parsing and validation of on-disk index data via direct FILE* access.

Supports multithreaded processing and dynamic content responses.

On-Disk Index Format
Document Table: Maps document IDs to filenames.

Word-to-Postings Index: Maps words to documents and their position offsets.

Stored using nested hash tables with position-aware postings lists for efficient lookups.

Index File Format
Encodes:

Document table: maps doc IDs to filenames.

Word-to-postings index: maps words to doc IDs and positions.

Uses nested hash tables with position-aware postings lists.

Reader Infrastructure

`FileIndexReader.c`, `IndexTableReader.c`, `DocIDTableReader.c`: Parse and validate on-disk index format using low-level FILE* I/O.

Design Highlights:
Binary Layout: Compact, network byte order structures ensure fast reads and portability.

Concurrency Safety: Duplicated file handles allow simultaneous reads from multiple index files.

Robustness: Defensive programming via Verify333 and strict offset validation.

Memory Safety: All dynamic allocations are cleaned up. Verified under Valgrind to have zero memory leaks.

---

## Design Decisions

- **Data Layout**: Compact binary structures minimize memory usage and disk reads. Network byte order ensures cross-platform consistency.
- **Concurrency-Safe Readers**: All file handles are duplicated to avoid contention during multi-index queries.
- **Error Safety**: All operations are validated with defensive checks (`Verify333`) and fallback recovery to avoid corrupt reads.
- **Memory Discipline**: All heap allocations are matched with deallocations; verified under Valgrind to ensure zero leaks.

## Test Coverage & Validation
This project has been tested using an autograder tests covering:

LinkedList and HashTable modules

100% test pass rate for operations such as insertion, deletion, slicing, iteration, and sorting

Verified memory safety using Valgrind (no leaks or invalid accesses)

File Parsing and Indexing Engine

All internal indexing components (DocTableReader, HashTableReader, QueryProcessor, etc.) passed correctness tests

Integration with .idx files validated via unit and functional tests

Multithreaded Web Server (http333d)

All core components (ThreadPool, HttpConnection, ServerSocket, FileReader) tested for correctness and stability

Path safety and HTML escaping functionality verified

✅ Memory safety confirmed via Valgrind in all modules
✅ Compiled successfully with -Wall -Wpedantic -std=c++17 and no warnings during test runs
❌ Known issue: filesearchshell CLI currently prints results in incorrect order — functional but not fully formatted


