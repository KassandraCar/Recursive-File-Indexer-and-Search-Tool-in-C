# C File Indexer & Search Engine

This project implements a full-text file indexer and search engine in pure C. 
Given one or more index files (produced by a recursive crawler and tokenizer), 
the tool supports ranked search queries using an interactive shell. All indexing and searching logic is built 
from scratch, including custom on-disk data structures, hash table serialization, and inverted index management.

> Originally developed for CSE 333: Systems Programming at the University of Washington.

---

## Project Structure

- **Indexing Pipeline**
  - `CrawlFileTree.c`: Recursively walks the file system and collects documents.
  - `FileParser.c`, `DocTable.c`, `MemIndex.c`: Tokenize text, assign document IDs, and build the in-memory inverted index.
  - `WriteIndex.c`: Serializes all index structures into a binary file format, including a CRC checksum.

- **On-Disk Index File Format**
  - Encodes a document table and a hash table of word-to-postings mappings.
  - Uses `WriteHashTable()` with nested buckets and position lists for scalable lookups.

- **Search Engine**
  - `QueryProcessor.*`: Loads and queries one or more index files using memory-mapped data structures.
  - `searchshell.c`: Interactive query interface that accepts arbitrary word queries and returns ranked document matches.

- **Reader Infrastructure**
  - `FileIndexReader.c`, `DocIDTableReader.c`, `IndexTableReader.c`: Low-level access to index file components using FILE* I/O with strict format validation.

---

## Design Decisions

- **Data Layout**: Compact binary structures minimize memory usage and disk reads. Network byte order ensures cross-platform consistency.
- **Concurrency-Safe Readers**: All file handles are duplicated to avoid contention during multi-index queries.
- **Error Safety**: All operations are validated with defensive checks (`Verify333`) and fallback recovery to avoid corrupt reads.
- **Memory Discipline**: All heap allocations are matched with deallocations; verified under Valgrind to ensure zero leaks.

---

## Example Use

```bash
# Build and run the indexer
./filecrawler ./documents/ index.idx

# Launch the search shell with one or more index files
./filesearchshell index.idx
