#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <immintrin.h> // For SIMD intrinsics

namespace nlm {
namespace performance {

/**
 * SIMD-optimized vector operations
 */
class SIMDUtils {
public:
    /**
     * Compute dot product of two vectors using SIMD
     */
    static float dotProductSIMD(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) {
            return 0.0f;
        }
        
        size_t size = a.size();
        size_t i = 0;
        
        __m128 sum = _mm_setzero_ps();
        
        // Process 4 elements at a time
        for (; i + 3 < size; i += 4) {
            __m128 va = _mm_loadu_ps(&a[i]);
            __m128 vb = _mm_loadu_ps(&b[i]);
            sum = _mm_add_ps(sum, _mm_mul_ps(va, vb));
        }
        
        // Horizontally add the remaining elements
        float result[4];
        _mm_storeu_ps(result, sum);
        float total = result[0] + result[1] + result[2] + result[3];
        
        // Process any remaining elements (less than 4)
        for (; i < size; ++i) {
            total += a[i] * b[i];
        }
        
        return total;
    }
    
    /**
     * Compute Euclidean distance between two vectors using SIMD
     */
    static float euclideanDistanceSIMD(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) {
            return 0.0f;
        }
        
        size_t size = a.size();
        size_t i = 0;
        
        __m128 sum = _mm_setzero_ps();
        
        // Process 4 elements at a time
        for (; i + 3 < size; i += 4) {
            __m128 va = _mm_loadu_ps(&a[i]);
            __m128 vb = _mm_loadu_ps(&b[i]);
            __m128 diff = _mm_sub_ps(va, vb);
            sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));
        }
        
        // Horizontally add the remaining elements
        float result[4];
        _mm_storeu_ps(result, sum);
        float total = result[0] + result[1] + result[2] + result[3];
        
        // Process any remaining elements
        for (; i < size; ++i) {
            float diff = a[i] - b[i];
            total += diff * diff;
        }
        
        return std::sqrt(total);
    }
    
    /**
     * Normalize a vector using SIMD
     */
    static void normalizeVectorSIMD(std::vector<float>& vec) {
        float norm = std::sqrt(dotProductSIMD(vec, vec));
        if (norm > 0.0f) {
            float invNorm = 1.0f / norm;
            size_t i = 0;
            
            for (; i + 3 < vec.size(); i += 4) {
                __m128 v = _mm_loadu_ps(&vec[i]);
                v = _mm_mul_ps(v, _mm_set1_ps(invNorm));
                _mm_storeu_ps(&vec[i], v);
            }
            
            for (; i < vec.size(); ++i) {
                vec[i] *= invNorm;
            }
        }
    }
    
    /**
     * Compute cosine similarity between two vectors using SIMD
     */
    static float cosineSimilaritySIMD(const std::vector<float>& a, const std::vector<float>& b) {
        float dot = dotProductSIMD(a, b);
        float normA = std::sqrt(dotProductSIMD(a, a));
        float normB = std::sqrt(dotProductSIMD(b, b));
        
        if (normA > 0.0f && normB > 0.0f) {
            return dot / (normA * normB);
        }
        return 0.0f;
    }
};

/**
 * Parallel processing utilities for neural computations
 */
class ParallelProcessor {
public:
    /**
     * Process neurons in parallel using threads (placeholder for future implementation)
     */
    static void parallelNeuronProcessing(std::vector<Neuron*>& neurons, 
                                         std::function<void(Neuron*)> processor,
                                         size_t numThreads = 4) {
        // Placeholder for thread-based parallel processing
        // In a real implementation, this would use std::thread or similar
        
        if (neurons.empty() || numThreads <= 1) {
            // Sequential processing
            for (auto* neuron : neurons) {
                processor(neuron);
            }
            return;
        }
        
        size_t chunkSize = neurons.size() / numThreads;
        std::vector<std::vector<Neuron*>> chunks(numThreads);
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            chunks[i % numThreads].push_back(neurons[i]);
        }
        
        // Process chunks in parallel (placeholder)
        for (auto& chunk : chunks) {
            for (auto* neuron : chunk) {
                processor(neuron);
            }
        }
    }
    
    /**
     * Parallel synapse processing
     */
    static void parallelSynapseProcessing(std::vector<Synapse*>& synapses,
                                          std::function<void(Synapse*)> processor,
                                          size_t numThreads = 4) {
        // Similar to neuron processing but for synapses
        if (synapses.empty() || numThreads <= 1) {
            for (auto* synapse : synapses) {
                processor(synapse);
            }
            return;
        }
        
        size_t chunkSize = synapses.size() / numThreads;
        std::vector<std::vector<Synapse*>> chunks(numThreads);
        
        for (size_t i = 0; i < synapses.size(); ++i) {
            chunks[i % numThreads].push_back(synapses[i]);
        }
        
        for (auto& chunk : chunks) {
            for (auto* synapse : chunk) {
                processor(synapse);
            }
        }
    }
};

/**
 * Memory pool optimizations for better cache locality
 */
class MemoryPoolOptimizations {
public:
    /**
     * Allocate aligned memory for better SIMD performance
     */
    static void* alignedAlloc(size_t size, size_t alignment = 32) {
        return _mm_malloc(size, alignment);
    }
    
    /**
     * Free aligned memory
     */
    static void alignedFree(void* ptr) {
        _mm_free(ptr);
    }
    
    /**
     * Prefetch memory for better cache performance
     */
    static void prefetchMemory(const void* ptr, int locality = 3) {
        _mm_prefetch(ptr, locality);
    }
    
    /**
     * Process vector with optimized memory access pattern
     */
    static void processVectorOptimized(const std::vector<float>& input,
                                       std::vector<float>& output,
                                       std::function<float(float)> processor) {
        size_t size = input.size();
        
        // Prefetch first cache line
        if (size >= 64) {
            prefetchMemory(input.data());
        }
        
        // Process with SIMD where possible
        size_t i = 0;
        for (; i + 3 < size; i += 4) {
            __m128 v = _mm_loadu_ps(&input[i]);
            __m128 result = _mm_set1_ps(processor(_mm_cvtss_f32(v))); // Simplified processor
            _mm_storeu_ps(&output[i], result);
        }
        
        // Process remaining elements
        for (; i < size; ++i) {
            output[i] = processor(input[i]);
        }
    }
};

/**
 * Algorithmic optimizations for neural network operations
 */
class AlgorithmicOptimizations {
public:
    /**
     * Fast normalization for many vectors
     */
    static void fastNormalizeVectors(std::vector<std::vector<float>>& vectors) {
        // Use a more efficient approach for batch normalization
        for (auto& vec : vectors) {
            float sum = 0.0f;
            for (float val : vec) {
                sum += val * val;
            }
            float norm = std::sqrt(sum);
            if (norm > 0.0f) {
                float invNorm = 1.0f / norm;
                for (float& val : vec) {
                    val *= invNorm;
                }
            }
        }
    }
    
    /**
     * Compute similarity matrix efficiently
     */
    static void computeSimilarityMatrix(const std::vector<std::vector<float>>& vectors,
                                       std::vector<std::vector<float>>& similarityMatrix) {
        size_t n = vectors.size();
        similarityMatrix.resize(n, std::vector<float>(n, 0.0f));
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i; j < n; ++j) {
                float similarity = SIMDUtils::cosineSimilaritySIMD(vectors[i], vectors[j]);
                similarityMatrix[i][j] = similarityMatrix[j][i] = similarity;
            }
        }
    }
    
    /**
     * Find top-k elements efficiently
     */
    static std::vector<size_t> findTopKElements(const std::vector<float>& values, size_t k) {
        if (k >= values.size()) {
            // Return all indices
            std::vector<size_t> allIndices(values.size());
            std::iota(allIndices.begin(), allIndices.end(), 0);
            return allIndices;
        }
        
        // Use partial_sort for efficiency
        std::vector<size_t> indices(values.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        auto compareFunc = [&values](size_t a, size_t b) {
            return values[a] > values[b];
        };
        
        std::partial_sort(indices.begin(), indices.begin() + k, indices.end(), compareFunc);
        
        return std::vector<size_t>(indices.begin(), indices.begin() + k);
    }
};

} // namespace performance

} // namespace nlm
