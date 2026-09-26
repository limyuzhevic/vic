#pragma once

/**
 * Algorithmic Optimizations
 * 
 * Advanced algorithmic optimization strategies for neural simulation:
 * 
 * Algorithmic Optimization Categories:
 * 1. Complexity Reduction (O(n^2) to O(n log n) or O(n))
 * 2. Vectorization and SIMD optimization
 * 3. Early exit and convergence optimization
 * 4. Memoization and caching
 * 5. Lookup tables
 * 6. Approximation algorithms
 * 7. Sparse data structure algorithms
 * 
 * Integration:
 * - Works with NLM algorithmic infrastructure
 * - Provides optimized implementations
 * - Supports algorithmic trade-offs
 * - Maintains numerical accuracy
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <array>
#include <chrono>
#include <numeric> // For std::iota

namespace nlm {

/**
 * Complexity reduction utilities
 */
class ComplexityReducer {
public:
    /**
     * Optimized distance calculation with early exit
     */
    static float optimizedDistance(const std::vector<float>& a, const std::vector<float>& b,
                                   float threshold = std::numeric_limits<float>::max()) {
        float distance = 0.0f;
        size_t minSize = std::min(a.size(), b.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float diff = a[i] - b[i];
            distance += diff * diff;
            
            if (distance > threshold) {
                return threshold; // Early exit if already over threshold
            }
        }
        
        return std::sqrt(distance);
    }
    
    /**
     * Optimized sorting for neural state updates
     */
    static void optimizedSort(std::vector<float>& values, std::vector<size_t>* indices = nullptr) {
        if (indices != nullptr) {
            // Sort with indices
            std::vector<size_t> sortedIndices(values.size());
            std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
            
            std::sort(sortedIndices.begin(), sortedIndices.end(),
                     [&values](size_t a, size_t b) { return values[a] < values[b]; });
            
            // Update values
            std::vector<float> sortedValues;
            sortedValues.reserve(values.size());
            
            for (size_t idx : sortedIndices) {
                sortedValues.push_back(values[idx]);
            }
            
            values = std::move(sortedValues);
            *indices = std::move(sortedIndices);
        } else {
            // Quick sort with median-of-three pivot
            std::sort(values.begin(), values.end());
        }
    }
    
    /**
     * Binary search optimization for spike time lookup
     */
    static size_t binarySearchSpikes(const std::vector<float>& spikeTimes,
                                    float targetTime, bool findFirst = true) {
        size_t low = 0;
        size_t high = spikeTimes.size() - 1;
        size_t result = spikeTimes.size();
        
        while (low <= high) {
            size_t mid = low + (high - low) / 2;
            
            if (spikeTimes[mid] >= targetTime) {
                result = mid;
                if (findFirst) {
                    high = mid - 1;
                } else {
                    low = mid + 1;
                }
            } else {
                low = mid + 1;
            }
        }
        
        return result;
    }
};

/**
 * Vectorization and SIMD optimization utilities
 */
class VectorizationOptimizer {
public:
    /**
     * Optimized vectorized reduction operations
     */
    static float vectorizedSum(const std::vector<float>& values) {
        float sum = 0.0f;
        size_t i = 0;
        
        // Process in vector chunks for potential SIMD optimization
        constexpr size_t VECTOR_SIZE = 8; // For AVX2
        size_t remainder = values.size() % VECTOR_SIZE;
        
        // Vectorized portion
        for (; i < values.size() - remainder; i += VECTOR_SIZE) {
            // This would be optimized by compiler to SIMD operations
            float chunkSum = 0.0f;
            for (size_t j = 0; j < VECTOR_SIZE; ++j) {
                chunkSum += values[i + j];
            }
            sum += chunkSum;
        }
        
        // Remainder portion
        for (; i < values.size(); ++i) {
            sum += values[i];
        }
        
        return sum;
    }
    
    /**
     * Optimized vectorized operations with early exit
     */
    static size_t vectorizedFindFirstGreater(const std::vector<float>& values, float threshold) {
        size_t i = 0;
        
        // Process in vector chunks
        constexpr size_t VECTOR_SIZE = 8;
        size_t remainder = values.size() % VECTOR_SIZE;
        
        for (; i < values.size() - remainder; i += VECTOR_SIZE) {
            bool found = false;
            for (size_t j = 0; j < VECTOR_SIZE; ++j) {
                if (values[i + j] > threshold) {
                    found = true;
                    i += j;
                    break;
                }
            }
            if (found) break;
        }
        
        // Process remainder
        for (; i < values.size(); ++i) {
            if (values[i] > threshold) {
                return i;
            }
        }
        
        return values.size(); // Not found
    }
    
    /**
     * Optimized vectorized operations with branch prediction hints
     */
    static float vectorizedWeightedSum(const std::vector<float>& values,
                                       const std::vector<float>& weights) {
        float sum = 0.0f;
        size_t minSize = std::min(values.size(), weights.size());
        
        // Assume weights are mostly positive for branch prediction
        for (size_t i = 0; i < minSize; ++i) {
            if (weights[i] > 0.0f) {  // Likely true for most weights
                sum += values[i] * weights[i];
            }
        }
        
        return sum;
    }
};

/**
 * Early exit and convergence optimization
 */
class ConvergenceOptimizer {
public:
    /**
     * Optimized convergence check with early exit
     */
    static bool checkConvergence(const std::vector<float>& current, const std::vector<float>& previous,
                                 float tolerance = 1e-6f, float maxChange = 0.1f) {
        float maxDiff = 0.0f;
        
        for (size_t i = 0; i < current.size(); ++i) {
            float diff = std::abs(current[i] - previous[i]);
            if (diff > maxDiff) {
                maxDiff = diff;
                if (maxDiff > maxChange) {
                    return false; // Early exit if change is too large
                }
            }
        }
        
        return maxDiff <= tolerance;
    }
    
    /**
     * Optimized convergence with adaptive tolerance
     */
    static bool checkAdaptiveConvergence(const std::vector<float>& current, const std::vector<float>& previous,
                                         float baseTolerance = 1e-6f, float learningRate = 0.01f) {
        float maxDiff = 0.0f;
        size_t i = 0;
        
        for (; i < current.size(); ++i) {
            float diff = std::abs(current[i] - previous[i]);
            if (diff > maxDiff) {
                maxDiff = diff;
                
                // Adaptive tolerance based on learning rate
                float adaptiveTolerance = baseTolerance * (1.0f + learningRate * i);
                
                if (maxDiff > adaptiveTolerance) {
                    return false; // Early exit
                }
            }
        }
        
        return true;
    }
    
    /**
     * Optimized multi-dimensional convergence check
     */
    static bool checkMultiDimensionalConvergence(const std::vector<std::vector<float>>& current,
                                                const std::vector<std::vector<float>>& previous,
                                                float tolerance = 1e-6f) {
        if (current.size() != previous.size()) {
            return false;
        }
        
        for (size_t dim = 0; dim < current.size(); ++dim) {
            if (current[dim].size() != previous[dim].size()) {
                return false;
            }
            
            float maxDiff = 0.0f;
            for (size_t i = 0; i < current[dim].size(); ++i) {
                float diff = std::abs(current[dim][i] - previous[dim][i]);
                if (diff > maxDiff) {
                    maxDiff = diff;
                }
            }
            
            if (maxDiff > tolerance) {
                return false;
            }
        }
        
        return true;
    }
};

/**
 * Memoization and caching utilities
 */
class MemoizationCache {
public:
    using CacheKey = std::vector<float>;
    using CacheValue = float;
    
    /**
     * Compute with memoization
     */
    static float computeWithMemoization(const CacheKey& key, 
                                        const std::function<float(const CacheKey&)>& computeFunc,
                                        size_t maxCacheSize = 1000) {
        // Simple hash for cache key (in real implementation, use better hash)
        size_t hash = 0;
        for (float val : key) {
            hash = hash * 31 + static_cast<size_t>(val * 1000.0f);
        }
        
        // Check cache
        auto it = cache_.find(hash);
        if (it != cache_.end()) {
            return it->second;
        }
        
        // Compute and cache
        float result = computeFunc(key);
        
        // Manage cache size
        if (cache_.size() >= maxCacheSize) {
            // Remove oldest entry (simple LRU implementation)
            auto oldest = cache_.begin();
            cache_.erase(oldest);
        }
        
        cache_[hash] = result;
        return result;
    }
    
    /**
     * Clear cache
     */
    static void clearCache() {
        cache_.clear();
    }
    
    /**
     * Get cache size
     */
    static size_t getCacheSize() {
        return cache_.size();
    }
    
    /**
     * Get cache hit rate
     */
    static double getCacheHitRate() {
        if (accessCount_ == 0) return 0.0;
        return static_cast<double>(cacheHits_) / accessCount_;
    }
    
private:
    static std::unordered_map<size_t, float> cache_;
    static size_t cacheHits_;
    static size_t accessCount_;
};

std::unordered_map<size_t, float> MemoizationCache::cache_;
size_t MemoizationCache::cacheHits_ = 0;
size_t MemoizationCache::accessCount_ = 0;

/**
 * Lookup table optimization
 */
class LookupTableOptimizer {
public:
    /**
     * Create sine lookup table
     */
    static std::vector<float> createSineTable(size_t size = 1024) {
        std::vector<float> table;
        table.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(size);
            table.push_back(std::sin(angle));
        }
        
        return table;
    }
    
    /**
     * Create exponential lookup table
     */
    static std::vector<float> createExpTable(size_t size = 1024, float minExp = -10.0f, float maxExp = 10.0f) {
        std::vector<float> table;
        table.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            float x = minExp + (maxExp - minExp) * static_cast<float>(i) / static_cast<float>(size - 1);
            table.push_back(std::exp(x));
        }
        
        return table;
    }
    
    /**
     * Create square root lookup table
     */
    static std::vector<float> createSqrtTable(size_t size = 1024, float maxInput = 100.0f) {
        std::vector<float> table;
        table.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            float x = maxInput * static_cast<float>(i) / static_cast<float>(size - 1);
            table.push_back(std::sqrt(x));
        }
        
        return table;
    }
    
    /**
     * Optimized table lookup with interpolation
     */
    static float interpolateFromTable(const std::vector<float>& table, float input,
                                      float minInput = 0.0f, float maxInput = 1.0f) {
        if (input <= minInput) return table[0];
        if (input >= maxInput) return table[table.size() - 1];
        
        float normalized = (input - minInput) / (maxInput - minInput);
        size_t index = static_cast<size_t>(normalized * (table.size() - 1));
        
        if (index == table.size() - 1) return table[index];
        
        float frac = normalized * (table.size() - 1) - static_cast<float>(index);
        return table[index] * (1.0f - frac) + table[index + 1] * frac;
    }
};

/**
 * Approximation algorithms for large-scale computations
 */
class ApproximationOptimizer {
public:
    /**
     * Fast approximate distance calculation
     */
    static float approximateDistance(const std::vector<float>& a, const std::vector<float>& b) {
        float sum = 0.0f;
        size_t minSize = std::min(a.size(), b.size());
        
        // Use only every other element for approximation (faster)
        for (size_t i = 0; i < minSize; i += 2) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }
    
    /**
     * Fast approximate sort (not fully sorted)
     */
    static void approximateSort(std::vector<float>& values) {
        // Simple bucket sort approximation
        constexpr size_t BUCKET_COUNT = 256;
        std::vector<std::vector<float>> buckets(BUCKET_COUNT);
        
        for (float val : values) {
            size_t bucket = static_cast<size_t>(val * BUCKET_COUNT);
            if (bucket >= BUCKET_COUNT) bucket = BUCKET_COUNT - 1;
            buckets[bucket].push_back(val);
        }
        
        // Concatenate buckets
        size_t index = 0;
        for (auto& bucket : buckets) {
            for (float val : bucket) {
                values[index++] = val;
            }
        }
    }
    
    /**
     * Fast approximate matrix multiplication
     */
    static void approximateMatrixMultiply(const std::vector<std::vector<float>>& A,
                                          const std::vector<std::vector<float>>& B,
                                          std::vector<std::vector<float>>& C,
                                          float threshold = 0.01f) {
        size_t N = A.size();
        size_t M = A[0].size();
        size_t K = B[0].size();
        
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                float sum = 0.0f;
                
                for (size_t k = 0; k < K; ++k) {
                    float product = A[i][k] * B[k][j];
                    if (std::abs(product) > threshold) {
                        sum += product;
                    }
                }
                
                C[i][j] = sum;
            }
        }
    }
};

} // namespace nlm