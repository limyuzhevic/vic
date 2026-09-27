#include "CacheManager.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

CacheManager::CacheManager(size_t capacity, size_t lineSize)
    : capacity_(capacity)
    , lineSize_(lineSize)
    , prefetchingEnabled_(true)
    , prefetchDistance_(8)  // Prefetch 8 cache lines ahead
    , nextTimestamp_(0)
{
    if (lineSize_ == 0) lineSize_ = 64;
    
    // Calculate cache parameters
    size_t cacheLines = capacity_ / lineSize_;
    if (cacheLines < 16) {
        // Minimum cache size
        capacity_ = 16 * lineSize_;
        cacheLines = 16;
    }
    
    // Determine number of sets based on power of 2
    size_t numSets = 1;
    while (numSets * 2 <= cacheLines && (cacheLines % (numSets * 2) == 0)) {
        numSets *= 2;
    }
    numSets_ = numSets;
    associativity_ = cacheLines / numSets_;
    
    cache_.resize(cacheLines);
    
    if (prefetchingEnabled_) {
        std::cout << "CacheManager: Initialized with " << cacheLines << " cache lines (" 
                  << capacity_ << " bytes)" << std::endl;
    }
}

CacheManager::~CacheManager() {
    // Clean up is automatic
}

CacheLine* CacheManager::getCacheLine(uintptr_t address) {
    updateAccessStats(findCacheLine(address) != nullptr);
    return findCacheLine(address);
}

template<typename T>
T* CacheManager::access(uintptr_t address, size_t size) {
    CacheLine* line = getCacheLine(address);
    if (line) {
        // Update access statistics
        line->accessCount++;
        line->lastAccess = nextTimestamp_.fetch_add(1);
        return reinterpret_cast<T*>(&line->data[0]);
    }
    return nullptr;
}

void CacheManager::prefetch(uintptr_t address, size_t size) {
    if (!prefetchingEnabled_) return;
    
    // Simple sequential prefetch
    uintptr_t alignedAddress = alignToCacheLine(address, lineSize_);
    size_t numLines = (size + lineSize_ - 1) / lineSize_;
    
    for (size_t i = 1; i <= prefetchDistance_ && i < numLines; ++i) {
        uintptr_t prefetchAddr = alignedAddress + i * lineSize_;
        if (prefetchAddr < address + size) {
            // Access the prefetched line to load it into cache
            findCacheLine(prefetchAddr);
        }
    }
}

void CacheManager::invalidate(uintptr_t address) {
    CacheLine* line = findCacheLine(address);
    if (line) {
        line->version++;
    }
}

void CacheManager::invalidateAll() {
    for (auto& line : cache_) {
        line.version++;
    }
}

CacheStats CacheManager::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void CacheManager::setCapacity(size_t newCapacity) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    capacity_ = newCapacity;
    size_t cacheLines = capacity_ / lineSize_;
    
    // Resize cache
    cache_.resize(cacheLines);
    
    // Recalculate parameters
    numSets_ = 1;
    while (numSets_ * 2 <= cacheLines && (cacheLines % (numSets_ * 2) == 0)) {
        numSets_ *= 2;
    }
    associativity_ = cacheLines / numSets_;
}

uintptr_t CacheManager::alignToCacheLine(uintptr_t address, size_t lineSize) {
    if (lineSize == 0) lineSize = 64;
    return (address + lineSize - 1) & ~(lineSize - 1);
}

size_t CacheManager::calculateCacheSet(uintptr_t address, size_t lineSize, size_t numSets) {
    uintptr_t alignedAddress = alignToCacheLine(address, lineSize);
    return alignedAddress % numSets;
}

bool CacheManager::isCacheLineAligned(uintptr_t address, size_t lineSize) {
    if (lineSize == 0) lineSize = 64;
    return (address % lineSize) == 0;
}

CacheLine* CacheManager::findCacheLine(uintptr_t address) {
    if (cache_.empty()) return nullptr;
    
    size_t setIndex = calculateCacheSet(address, lineSize_, numSets_);
    
    // Search through all ways in this set
    for (size_t i = 0; i < associativity_; ++i) {
        size_t lineIndex = setIndex * associativity_ + i;
        if (lineIndex < cache_.size()) {
            CacheLine& line = cache_[lineIndex];
            
            // Calculate the base address of this cache line
            uintptr_t lineAddress = reinterpret_cast<uintptr_t>(&line.data[0]);
            
            if (lineAddress == address) {
                return &line;
            }
        }
    }
    
    // Cache miss - evict LRU and insert new line
    evictLRU();
    
    // Add new line
    CacheLine& newLine = cache_[numSets_ * associativity_]; // Last line in last set
    memset(&newLine, 0, sizeof(CacheLine));
    
    return &newLine;
}

void CacheManager::evictLRU() {
    // Simple LRU implementation - evict from last set
    if (cache_.empty()) return;
    
    size_t setIndex = numSets_ - 1;
    if (setIndex >= numSets_) setIndex = 0;
    
    size_t minAccess = cache_[setIndex * associativity_].lastAccess;
    size_t minIndex = setIndex * associativity_;
    
    // Find LRU line in the set
    for (size_t i = setIndex * associativity_; i < (setIndex + 1) * associativity_; ++i) {
        if (i < cache_.size() && cache_[i].lastAccess < minAccess) {
            minAccess = cache_[i].lastAccess;
            minIndex = i;
        }
    }
    
    // Clear the evicted line
    memset(&cache_[minIndex], 0, sizeof(CacheLine));
}

void CacheManager::updateAccessStats(bool hit) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    stats_.totalAccesses++;
    if (hit) {
        stats_.cacheHits++;
    } else {
        stats_.cacheMisses++;
    }
}

void CacheManager::cleanup() {
    // Invalidate all lines
    invalidateAll();
    
    // Reset statistics
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_ = CacheStats();
}

size_t CacheManager::hashAddress(uintptr_t address) const {
    // Simple hash function
    return address % cache_.size();
}

} // namespace nlm
