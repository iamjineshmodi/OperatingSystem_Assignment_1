# Operating System Assignment: Trie-Based Shared Memory and Message Queue Application

## Overview
This project demonstrates the use of shared memory, message queues, and a Trie data structure in a Linux environment. The program processes a matrix of words to perform diagonal traversal, applies a cipher to the words, and utilizes a Trie for word frequency analysis.

## Key Features
1. **Data Structures**:
   - Implements a Trie for efficient word insertion and lookup.
   - Cipher function for encoding words with a shift key.

2. **Inter-Process Communication**:
   - **Shared Memory**: Stores a matrix of words shared between processes.
   - **Message Queues**: Coordinates sum calculations between processes.

3. **File Handling**:
   - Reads matrix dimensions and words from input files.
   - Dynamically processes word lists for Trie operations.

4. **Core Functionality**:
   - Traverses the matrix diagonally.
   - Encrypts words with a cipher before Trie search.
   - Exchanges results between parent and child processes using message queues.

## Key Functions
- `createNewNode()`: Allocates a new Trie node.
- `insert()`: Adds words to the Trie.
- `searchTrie()`: Searches for words in the Trie.
- `cipher()`: Encrypts words with a Caesar cipher.

## System Requirements
- Linux OS
- GCC Compiler
- POSIX Shared Memory and IPC Support
