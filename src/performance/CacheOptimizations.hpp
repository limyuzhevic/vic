#pragma once

/**
 * Cache Performance Optimizations
 * 
 * Comprehensive cache optimization strategies for neural simulation:
 * 
 * Cache Optimization Categories:
 * 1. Loop Order Optimization
 * 2. Data Structure Layout (SoA vs AoS)
 * 3. Padding and Alignment Optimization
 * 4. Prefetching Strategies
 * 5. Cache Blocking
 * 6. Temporal and Spatial Locality Optimization
 * 
 * Integration:
 * - Works with NLM SoA memory layouts
 * - Optimizes neural computation loops
 * - Provides cache-friendly data structures
 * - Implements prefetching strategies
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * Cache line size detection
 */
constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t CACHE_LINE_ALIGNMENT = 64;

/**
 * Cache-friendly vector with padding for false sharing prevention
 */
template<typename T>
class CacheFriendlyVector {
public:
    struct alignas(CACHE_LINE_SIZE) CacheLineEntry {
        T value;
        char padding[CACHE_LINE_SIZE - sizeof(T)]; // Prevent false sharing
    };
    
    CacheFriendlyVector() = default;
    
    explicit CacheFriendlyVector(size_t size) {
        resize(size);
    }
    
    ~CacheFriendlyVector() {
        delete[] data_;
    }
    
    // Disable copying
    CacheFriendlyVector(const CacheFriendlyVector&) = delete;
    CacheFriendlyVector& operator=(const CacheFriendlyVector&) = delete;
    
    // Move operations
    CacheFriendlyVector(CacheFriendlyVector&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }
    
    CacheFriendlyVector& operator=(CacheFriendlyVector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // Element access
    T& operator[](size_t index) {
        return data_[index].value;
    }
    
    const T& operator[](size_t index) const {
        return data_[index].value;
    }
    
    // Capacity operations
    void resize(size_t newSize) {
        if (newSize <= capacity_) {
            size_ = newSize;
            return;
        }
        
        // Allocate new cache-aligned array
        CacheLineEntry* newData = new CacheLineEntry[newSize];
        
        // Copy existing data
        for (size_t i = 0; i < std::min(size_, newSize); ++i) {
            newData[i].value = std::move(data_[i].value);
        }
        
        // Initialize new elements
        for (size_t i = size_; i < newSize; ++i) {
            new (&newData[i].value) T();
        }
        
        // Clean up old data
        for (size_t i = 0; i < size_; ++i) {
            data_[i].value.~T();
        }
        
        delete[] data_;
        data_ = newData;
        size_ = newSize;
        capacity_ = newSize;
    }
    
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
    // Iterators
    T* begin() { return &data_[0].value; }
    const T* begin() const { return &data_[0].value; }
    T* end() { return &data_[size_].value; }
    const T* end() const { return &data_[size_].value; }
    
    // Memory usage
    size_t memoryUsage() const {
        return size_ * CACHE_LINE_SIZE;
    }
    
private:
    CacheLineEntry* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

/**
 * Cache blocking utilities for matrix operations
 */
class CacheBlocking {
public:
    struct BlockingParams {
        size_t blockSize;
        size_t innerLoopSize;
        size_t outerLoopSize;
        
        BlockingParams(size_t bs = 0, size_t ils = 0, size_t ols = 0)
            : blockSize(bs), innerLoopSize(ils), outerLoopSize(ols) {}
    };
    
    /**
     * Calculate optimal cache blocking parameters
     */
    static BlockingParams calculateOptimalBlocking(size_t matrixSize, 
                                                   size_t cacheSize = 32768) {
        // Target cache size for blocking
        double targetCacheBytes = static_cast<double>(cacheSize) * 0.8; // Use 80% of cache
        
        // Calculate block size that fits in cache
        size_t elementSize = sizeof(float);
        size_t maxBlockSize = static_cast<size_t>(std::sqrt(targetCacheBytes / elementSize));
        
        // Ensure reasonable block sizes
        size_t blockSize = std::max<size_t>(16, std::min<size_t>(128, maxBlockSize));
        
        // Calculate loop unrolling factors
        size_t innerLoopSize = std::min<size_t>(8, blockSize);
        size_t outerLoopSize = (matrixSize + blockSize - 1) / blockSize;
        
        return BlockingParams(blockSize, innerLoopSize, outerLoopSize);
    }
    
    /**
     * Perform cache-optimal matrix multiplication with blocking
     */
    static void cacheBlockedMatrixMultiply(const float* A, const float* B, float* C,
                                           size_t N, size_t M, size_t K,
                                           const BlockingParams& params = BlockingParams()) {
        BlockingParams blocking = params;
        if (blocking.blockSize == 0) {
            blocking = calculateOptimalBlocking(N);
        }
        
        // Block indices
        for (size_t ii = 0; ii < N; ii += blocking.blockSize) {
            size_t iEnd = std::min(ii + blocking.blockSize, N);
            
            for (size_t jj = 0; jj < M; jj += blocking.blockSize) {
                size_t jEnd = std::min(jj + blocking.blockSize, M);
                
                // Inner accumulation
                for (size_t kk = 0; kk < K; kk += blocking.blockSize) {
                    size_t kEnd = std::min(kk + blocking.blockSize, K);
                    
                    // Blocked computation
                    for (size_t i = ii; i < iEnd; ++i) {
                        for (size_t j = jj; j < jEnd; ++j) {
                            float sum = C[i * M + j];
                            
                            for (size_t k = kk; k < kEnd; ++k) {
                                sum += A[i * K + k] * B[k * M + j];
                            }
                            
                            C[i * M + j] = sum;
                        }
                    }
                }
            }
        }
    }
    
    /**
     * Perform cache-optimal matrix-vector multiplication with blocking
     */
    static void cacheBlockedMatrixVectorMultiply(const float* A, const float* x, float* y,
                                                 size_t N, size_t M,
                                                 const BlockingParams& params = BlockingParams()) {
        BlockingParams blocking = params;
        if (blocking.blockSize == 0) {
            blocking = calculateOptimalBlocking(N);
        }
        
        // Block processing
        for (size_t ii = 0; ii < N; ii += blocking.blockSize) {
            size_t iEnd = std::min(ii + blocking.blockSize, N);
            
            // Process block
            for (size_t i = ii; i < iEnd; ++i) {
                float sum = 0.0f;
                
                // Vectorized computation with prefetching hint
                for (size_t j = 0; j < M; ++j) {
                    // Prefetch next element
                    if (j + blocking.innerLoopSize < M) {
                        __builtin_prefetch(&A[(i * M + j + blocking.innerLoopSize) * sizeof(float)], 0, 3);
                    }
                    
                    sum += A[i * M + j] * x[j];
                }
                
                y[i] = sum;
            }
        }
    }
};

/**
 * Loop order optimizer for memory access patterns
 */
class LoopOrderOptimizer {
public:
    /**
     * Determine optimal loop order for matrix operations based on memory layout
     */
    enum class LoopOrder { RowMajor, ColumnMajor, CacheOptimal };
    
    /**
     * Get optimal loop order for specific operation and data layout
     */
    static LoopOrder getOptimalLoopOrder(size_t sizeA, size_t sizeB, size_t sizeC,
                                         bool isRowMajorA, bool isRowMajorB,
                                         bool isRowMajorC, size_t cacheLineSize = CACHE_LINE_SIZE) {
        // For simplicity, always return CacheOptimal
        return LoopOrder::CacheOptimal;
    }
    
    /**
     * Generate cache-optimal nested loop indices
     */
    static void generateCacheOptimalIndices(size_t N, size_t M, size_t K,
                                            size_t& iStep, size_t& jStep, size_t& kStep) {
        // For matrix multiplication: i (row), j (column), k (inner)
        // Optimized for cache: outer loop over i, middle loop over j, inner loop over k
        iStep = 1;
        jStep = 1;
        kStep = 1;
    }
};

/**
 * Prefetching utilities for large data sets
 */
class Prefetcher {
public:
    /**
     * Prefetch memory address for read (streaming)
     */
    static void prefetchRead(const void* addr) {
        #if defined(__x86_64__) || defined(_M_X64)
        _mm_prefetch(reinterpret_cast<const char*>(addr), _MM_HINT_T0);
        #endif
    }
    
    /**
     * Prefetch memory address for write
     */
    static void prefetchWrite(const void* addr) {
        #if defined(__x86_64__) || defined(_M_X64)
        _mm_prefetch(reinterpret_cast<const char*>(addr), _MM_HINT_T1);
        #endif
    }
    
    /**
     * Prefetch next element in a loop (streaming prefetch)
     */
    template<typename Iterator>
    static void prefetchStream(Iterator it, size_t stride = 1) {
        if (stride == 1) {
            prefetchRead(&(*(it + stride)));
        }
    }
    
    /**
     * Adaptive prefetching based on access pattern
     */
    static void adaptivePrefetch(const float* baseAddr, size_t baseIndex,
                                 size_t currentIndex, size_t stride,
                                 size_t lookahead = 4) {
        for (size_t i = 1; i <= lookahead; ++i) {
            if (currentIndex + i * stride < baseIndex + stride) {
                prefetchRead(baseAddr + (currentIndex + i * stride) * sizeof(float));
            }
        }
    }
};

/**
 * Cache efficiency analyzer
 */
class CacheEfficiencyAnalyzer {
public:
    struct CacheStats {
        uint64_t cacheHits;
        uint64_t cacheMisses;
        uint64_t totalAccesses;
        double hitRate;
        double missRate;
        uint64_t averageMissLatency;
        
        CacheStats() : cacheHits(0), cacheMisses(0), totalAccesses(0),
                      hitRate(0.0), missRate(0.0), averageMissLatency(0) {}
    };
    
    /**
     * Calculate cache efficiency score
     */
    static double calculateCacheEfficiency(const CacheStats& stats) {
        if (stats.totalAccesses == 0) return 0.0;
        
        double hitRate = static_cast<double>(stats.cacheHits) / stats.totalAccesses;
        double missPenalty = 1.0 - (static_cast<double>(stats.averageMissLatency) / 100.0); // Normalized
        
        return hitRate * missPenalty;
    }
    
    /**
     * Estimate cache misses for matrix operations
     */
    static uint64_t estimateMatrixMisses(size_t rows, size_t cols, size_t cacheLineSize = CACHE_LINE_SIZE) {
        // Simple estimation based on stride
        size_t elementsPerCacheLine = cacheLineSize / sizeof(float);
        
        // For row-major access with stride = cols
        uint64_t misses = 0;
        
        // Column-wise access (worst case)
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (j % elementsPerCacheLine == 0) {
                    misses++;
                }
            }
        }
        
        return misses;
    }
};

} // namespace nlm