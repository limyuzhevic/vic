/**
 * Matrix4x4 - SIMD-optimized 4x4 matrix implementation
 * 
 * Provides optimized 4x4 matrix operations using SIMD instructions
 * for high-performance neural computations.
 */

#include "Matrix4x4.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

// Template implementations for Matrix4x4
template<typename T>
Matrix4x4<T>::Matrix4x4(size_t blockSize, size_t initialBlocks)
    : blockSize(blockSize), 
      freeList(nullptr),
      allocatedCount(0),
      availableCount(blockSize * initialBlocks),
      firstBlock(nullptr) {
    
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock();
    }
}

template<typename T>
Matrix4x4<T>::~Matrix4x4() {
    Block* block = firstBlock;
    while (block) {
        Block* nextBlock = block->next;
        delete block;
        block = nextBlock;
    }
}

template<typename T>
T* Matrix4x4<T>::allocate() {
    // First try to get from free list (lock-free)
    MemPoolNode* oldHead = nullptr;
    do {
        oldHead = freeList;
        if (!oldHead) {
            Block* block = addBlock();
            if (!block || block->used >= blockSize) {
                return nullptr;
            }
            return reinterpret_cast<T*>(&block->data[block->used++]);
        }
    } while (!compare_exchange_weak(freeList, oldHead->next, oldHead->next, oldHead));
    
    T* obj = reinterpret_cast<T*>(oldHead);
    allocatedCount++;
    availableCount--;
    return obj;
}

template<typename T>
void Matrix4x4<T>::deallocate(T* obj) {
    if (!obj) return;
    
    MemPoolNode* node = reinterpret_cast<MemPoolNode*>(obj);
    
    MemPoolNode* oldHead = nullptr;
    do {
        oldHead = freeList;
        node->next = oldHead;
    } while (!compare_exchange_weak(freeList, oldHead->next, node, oldHead));
    
    allocatedCount--;
    availableCount++;
}

template<typename T>
size_t Matrix4x4<T>::memoryUsage() const {
    size_t total = 0;
    Block* block = firstBlock;
    while (block) {
        total += sizeof(Block) + blockSize * sizeof(T);
        block = block->next;
    }
    return total;
}

template<typename T>
void Matrix4x4<T>::reserve(size_t count) {
    size_t needed = (count + blockSize - 1) / blockSize;
    size_t current = 0;
    
    Block* block = firstBlock;
    while (block) {
        current++;
        block = block->next;
    }
    
    while (current < needed) {
        if (!addBlock()) break;
        current++;
    }
}

template<typename T>
Matrix4x4<T>::Block* Matrix4x4<T>::addBlock() {
    Block* newBlock = new Block();
    if (!newBlock) return nullptr;
    
    newBlock->next = firstBlock;
    firstBlock = newBlock;
    
    availableCount += blockSize;
    return newBlock;
}

// Explicit instantiations
template class Matrix4x4<float>;
template class Matrix4x4<double>;

// Additional specializations
template class Matrix4x4<std::vector<float>>;
template class Matrix4x4<std::vector<double>>;

template class Matrix4x4<MemPoolNode>;

// Type aliases
typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix4x4d;

} // namespace nlm

// Include template implementations
#include "Matrix4x4.cpp"
