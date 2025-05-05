# C File Indexer & Search Engine

This project implements a full-text file indexer and search engine in pure C. 
Given one or more index files (produced by a recursive crawler and tokenizer), 
the tool supports ranked search queries using an interactive shell. All indexing and searching logic is built 
from scratch, including custom on-disk data structures, hash table serialization, and inverted index management.

> Originally developed for CSE 333: Systems Programming at the University of Washington.

---

## Project Structure
 
-Indexing Pipeline
`CrawlFileTree.c`: Recursively traverses a directory to gather documents.

`FileParser.c`, `DocTable.c`, `MemIndex.c`: Tokenize files, assign document IDs, and construct an in-memory inverted index.

`WriteIndex.c`: Serializes the in-memory index to a compact, binary format with a CRC checksum.

Search Engine
`QueryProcessor.`: Loads one or more index files and performs ranked multi-word queries.

`searchshell.c`: Interactive shell for CLI-based search.

HTTP Search Server
`http333d.cc`, `HttpServer.cc`, `HttpConnection.cc`: Implements a basic HTTP server that supports GET queries.

`HttpUtils.cc`, `ServerSocket.cc`: Handle socket setup and HTTP parsing.

Supports multithreaded processing and dynamic content responses.

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
