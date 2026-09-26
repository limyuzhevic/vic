#pragma once

/**
 * Computational Optimizations
 * 
 * Advanced computational optimization strategies for neural simulation:
 * 
 * Computational Optimization Categories:
 * 1. Neural Computation Optimization
 * 2. Matrix Operations Optimization
 * 3. Sorting/Searching Optimization
 * 4. Distance/Similarity Optimization
 * 5. Parallel Computing Optimization
 * 6. Numerical Stability Optimization
 * 7. Precision Management
 * 
 * Integration:
 * - Works with NLM computational infrastructure
 * - Provides optimized mathematical operations
 * - Supports precision trade-offs
 * - Maintains numerical accuracy
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <limits>

namespace nlm {

/**
 * Neural computation optimization utilities
 */
class NeuralComputationOptimizer {
public:
    /**
     * Optimized neuron state update with SIMD-friendly operations
     */
    static void optimizedNeuronUpdate(std::vector<float>& V, const std::vector<float>& I,
                                     const std::vector<float>& V_rest, const std::vector<float>& V_th,
                                     const std::vector<float>& V_reset, const std::vector<float>& tau,
                                     const std::vector<float>& C, float dt, float thresholdDecay,
                                     std::vector<uint32_t>& refractoryRemaining,
                                     std::vector<uint8_t>& fired) {
        size_t size = V.size();
        
        // Batch process with potential for SIMD optimization
        for (size_t i = 0; i < size; ++i) {
            if (refractoryRemaining[i] > 0) {
                // In refractory period
                refractoryRemaining[i]--;
                V[i] = V_rest[i];
                fired[i] = 0;
            } else {
                // LIF update: V_new = V + dt * ((V_rest - V)/tau + I/C)
                float leak = (V_rest[i] - V[i]) / tau[i];
                float input = I[i] / C[i];
                V[i] += dt * 1000.0f * (leak + input); // 1000 factor for ms to s conversion
                
                if (V[i] >= V_th[i]) {
                    // Spike detected
                    V[i] = V_reset[i];
                    refractoryRemaining[i] = static_cast<uint32_t>(thresholdDecay);
                    fired[i] = 1;
                } else {
                    fired[i] = 0;
                }
            }
        }
    }
    
    /**
     * Optimized synaptic integration with early exit
     */
    static void optimizedSynapticIntegration(std::vector<float>& synapticInputs,
                                             const std::vector<float>& weights,
                                             const std::vector<float>& preSpikeTimes,
                                             const std::vector<float>& postSpikeTimes,
                                             float currentTime, float learningRate,
                                             bool enableSTDP = false) {
        size_t size = synapticInputs.size();
        
        for (size_t i = 0; i < size; ++i) {
            float effectiveWeight = weights[i];
            
            if (enableSTDP) {
                // STDP update
                float deltaW = calculateSTDPUpdate(preSpikeTimes[i], postSpikeTimes[i], currentTime);
                effectiveWeight += learningRate * deltaW;
            }
            
            synapticInputs[i] += effectiveWeight;
        }
    }
    
    /**
     * Optimized spike detection with threshold crossing
     */
    static void optimizedSpikeDetection(const std::vector<float>& V, const std::vector<float>& V_th,
                                        std::vector<uint8_t>& fired, std::vector<float>& spikeTimes,
                                        float currentTime) {
        size_t size = V.size();
        
        for (size_t i = 0; i < size; ++i) {
            if (V[i] >= V_th[i]) {
                fired[i] = 1;
                spikeTimes[i] = currentTime;
            } else {
                fired[i] = 0;
                spikeTimes[i] = -1.0f; // No spike
            }
        }
    }
    
private:
    static float calculateSTDPUpdate(float preSpikeTime, float postSpikeTime, float currentTime) {
        if (preSpikeTime < 0.0f || postSpikeTime < 0.0f) {
            return 0.0f;
        }
        
        float dt = currentTime - postSpikeTime;
        if (dt > 0.0f) {
            // Post after pre: LTD
            return -0.012f * std::exp(-dt / 20.0f);
        } else {
            // Pre after post: LTP
            return 0.01f * std::exp(dt / 20.0f);
        }
    }
};

/**
 * Matrix operations optimization
 */
class MatrixOperationsOptimizer {
public:
    /**
     * Optimized matrix multiplication with cache blocking
     */
    static void optimizedMatrixMultiply(const float* A, const float* B, float* C,
                                        size_t N, size_t M, size_t K,
                                        bool useBlocking = true) {
        if (useBlocking) {
            // Use cache blocking
            size_t blockSize = 32;
            
            for (size_t ii = 0; ii < N; ii += blockSize) {
                size_t iEnd = std::min(ii + blockSize, N);
                
                for (size_t jj = 0; jj < M; jj += blockSize) {
                    size_t jEnd = std::min(jj + blockSize, M);
                    
                    // Initialize C block
                    for (size_t i = ii; i < iEnd; ++i) {
                        for (size_t j = jj; j < jEnd; ++j) {
                            C[i * M + j] = 0.0f;
                        }
                    }
                    
                    for (size_t kk = 0; kk < K; kk += blockSize) {
                        size_t kEnd = std::min(kk + blockSize, K);
                        
                        // Blocked multiplication
                        for (size_t i = ii; i < iEnd; ++i) {
                            for (size_t k = kk; k < kEnd; ++k) {
                                float a = A[i * K + k];
                                
                                for (size_t j = jj; j < jEnd; ++j) {
                                    C[i * M + j] += a * B[k * M + j];
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // Naive multiplication (for comparison)
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    float sum = 0.0f;
                    
                    for (size_t k = 0; k < K; ++k) {
                        sum += A[i * K + k] * B[k * M + j];
                    }
                    
                    C[i * M + j] = sum;
                }
            }
        }
    }
    
    /**
     * Optimized matrix-vector multiplication
     */
    static void optimizedMatrixVectorMultiply(const float* A, const float* x, float* y,
                                              size_t N, size_t M, bool useVectorization = true) {
        if (useVectorization) {
            // Process in blocks for vectorization
            size_t blockSize = 8;
            size_t remainder = N % blockSize;
            
            // Vectorized portion
            for (size_t i = 0; i < N - remainder; i += blockSize) {
                float y0 = 0.0f, y1 = 0.0f, y2 = 0.0f, y3 = 0.0f, y4 = 0.0f, y5 = 0.0f, y6 = 0.0f, y7 = 0.0f;
                
                for (size_t j = 0; j < M; ++j) {
                    float xVal = x[j];
                    y0 += A[i * M + j] * xVal;
                    y1 += A[(i + 1) * M + j] * xVal;
                    y2 += A[(i + 2) * M + j] * xVal;
                    y3 += A[(i + 3) * M + j] * xVal;
                    y4 += A[(i + 4) * M + j] * xVal;
                    y5 += A[(i + 5) * M + j] * xVal;
                    y6 += A[(i + 6) * M + j] * xVal;
                    y7 += A[(i + 7) * M + j] * xVal;
                }
                
                y[i] = y0;
                y[i + 1] = y1;
                y[i + 2] = y2;
                y[i + 3] = y3;
                y[i + 4] = y4;
                y[i + 5] = y5;
                y[i + 6] = y6;
                y[i + 7] = y7;
            }
            
            // Remainder portion
            for (size_t i = N - remainder; i < N; ++i) {
                float sum = 0.0f;
                
                for (size_t j = 0; j < M; ++j) {
                    sum += A[i * M + j] * x[j];
                }
                
                y[i] = sum;
            }
        } else {
            // Naive multiplication
            for (size_t i = 0; i < N; ++i) {
                float sum = 0.0f;
                
                for (size_t j = 0; j < M; ++j) {
                    sum += A[i * M + j] * x[j];
                }
                
                y[i] = sum;
            }
        }
    }
    
    /**
     * Optimized matrix transpose
     */
    static void optimizedTranspose(const float* A, float* AT, size_t N, size_t M, bool useBlocking = true) {
        if (useBlocking) {
            size_t blockSize = 32;
            
            for (size_t ii = 0; ii < N; ii += blockSize) {
                size_t iEnd = std::min(ii + blockSize, N);
                
                for (size_t jj = 0; jj < M; jj += blockSize) {
                    size_t jEnd = std::min(jj + blockSize, M);
                    
                    // Blocked transpose
                    for (size_t i = ii; i < iEnd; ++i) {
                        for (size_t j = jj; j < jEnd; ++j) {
                            AT[j * N + i] = A[i * M + j];
                        }
                    }
                }
            }
        } else {
            // Naive transpose
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    AT[j * N + i] = A[i * M + j];
                }
            }
        }
    }
};

/**
 * Sorting and searching optimization
 */
class SortingSearchingOptimizer {
public:
    /**
     * Optimized sorting with partial sorting
     */
    static void optimizedSort(std::vector<float>& values, size_t k = 0) {
        if (k > 0 && k < values.size()) {
            // Partial sort (only first k elements)
            std::nth_element(values.begin(), values.begin() + k, values.end());
            std::partial_sort(values.begin(), values.begin() + k, values.end());
        } else {
            // Full sort
            std::sort(values.begin(), values.end());
        }
    }
    
    /**
     * Optimized binary search with branchless operations
     */
    static size_t branchlessBinarySearch(const std::vector<float>& arr, float target) {
        size_t low = 0;
        size_t high = arr.size() - 1;
        size_t result = arr.size();
        
        while (low <= high) {
            size_t mid = low + ((high - low) >> 1); // Branchless division
            
            // Branchless comparison
            bool greater = arr[mid] >= target;
            bool less = arr[mid] < target;
            
            result = greater ? mid : result;
            high = less ? high : mid - 1;
            low = less ? low : mid + 1;
        }
        
        return result;
    }
    
    /**
     * Optimized linear search with early exit
     */
    static size_t optimizedLinearSearch(const std::vector<float>& arr, float target, size_t start = 0) {
        // Linear search with potential for SIMD optimization
        for (size_t i = start; i < arr.size(); ++i) {
            if (arr[i] == target) {
                return i;
            }
        }
        
        return arr.size(); // Not found
    }
};

/**
 * Distance and similarity optimization
 */
class DistanceSimilarityOptimizer {
public:
    /**
     * Optimized Euclidean distance calculation
     */
    static float optimizedEuclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
        size_t size = std::min(a.size(), b.size());
        float sum = 0.0f;
        
        // Process in blocks for potential SIMD optimization
        size_t blockSize = 8;
        size_t remainder = size % blockSize;
        
        // Vectorized portion
        for (size_t i = 0; i < size - remainder; i += blockSize) {
            float diff0 = a[i] - b[i];
            float diff1 = a[i + 1] - b[i + 1];
            float diff2 = a[i + 2] - b[i + 2];
            float diff3 = a[i + 3] - b[i + 3];
            float diff4 = a[i + 4] - b[i + 4];
            float diff5 = a[i + 5] - b[i + 5];
            float diff6 = a[i + 6] - b[i + 6];
            float diff7 = a[i + 7] - b[i + 7];
            
            sum += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3 +
                   diff4 * diff4 + diff5 * diff5 + diff6 * diff6 + diff7 * diff7;
        }
        
        // Remainder portion
        for (size_t i = size - remainder; i < size; ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }
    
    /**
     * Optimized Manhattan distance calculation
     */
    static float optimizedManhattanDistance(const std::vector<float>& a, const std::vector<float>& b) {
        size_t size = std::min(a.size(), b.size());
        float sum = 0.0f;
        
        for (size_t i = 0; i < size; ++i) {
            sum += std::abs(a[i] - b[i]);
        }
        
        return sum;
    }
    
    /**
     * Optimized cosine similarity calculation
     */
    static float optimizedCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float dotProduct = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;
        size_t minSize = std::min(a.size(), b.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        float normProduct = std::sqrt(normA) * std::sqrt(normB);
        if (normProduct > 0.0f) {
            return dotProduct / normProduct;
        }
        
        return 0.0f;
    }
    
    /**
     * Optimized nearest neighbor search
     */
    static size_t optimizedNearestNeighbor(const std::vector<std::vector<float>>& database,
                                           const std::vector<float>& query,
                                           float& bestDistance) {
        size_t bestIndex = 0;
        bestDistance = std::numeric_limits<float>::max();
        
        for (size_t i = 0; i < database.size(); ++i) {
            float distance = optimizedEuclideanDistance(database[i], query);
            
            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = i;
            }
        }
        
        return bestIndex;
    }
};

/**
 * Parallel computing optimization
 */
class ParallelComputingOptimizer {
public:
    /**
     * Optimized parallel reduction
     */
    static float parallelReduce(const std::vector<float>& values, size_t threadCount = 4) {
        if (values.empty()) return 0.0f;
        
        size_t chunkSize = values.size() / threadCount;
        std::vector<float> partialSums(threadCount, 0.0f);
        
        // Parallel computation (in real implementation, use threading)
        for (size_t t = 0; t < threadCount; ++t) {
            size_t start = t * chunkSize;
            size_t end = (t == threadCount - 1) ? values.size() : start + chunkSize;
            
            float sum = 0.0f;
            for (size_t i = start; i < end; ++i) {
                sum += values[i];
            }
            
            partialSums[t] = sum;
        }
        
        // Combine partial sums
        float totalSum = 0.0f;
        for (float partial : partialSums) {
            totalSum += partial;
        }
        
        return totalSum;
    }
    
    /**
     * Optimized parallel map operation
     */
    static void parallelMap(std::vector<float>& output, const std::vector<float>& input,
                           const std::function<float(float)>& func, size_t threadCount = 4) {
        if (output.size() != input.size()) {
            output.resize(input.size());
        }
        
        size_t chunkSize = input.size() / threadCount;
        
        // Parallel computation (in real implementation, use threading)
        for (size_t t = 0; t < threadCount; ++t) {
            size_t start = t * chunkSize;
            size_t end = (t == threadCount - 1) ? input.size() : start + chunkSize;
            
            for (size_t i = start; i < end; ++i) {
                output[i] = func(input[i]);
            }
        }
    }
    
    /**
     * Optimized parallel filter operation
     */
    static void parallelFilter(std::vector<float>& output, const std::vector<float>& input,
                               const std::function<bool(float)>& predicate, size_t threadCount = 4) {
        output.clear();
        output.reserve(input.size());
        
        size_t chunkSize = input.size() / threadCount;
        std::vector<std::vector<float>> threadOutputs(threadCount);
        
        // Parallel computation (in real implementation, use threading)
        for (size_t t = 0; t < threadCount; ++t) {
            size_t start = t * chunkSize;
            size_t end = (t == threadCount - 1) ? input.size() : start + chunkSize;
            
            for (size_t i = start; i < end; ++i) {
                if (predicate(input[i])) {
                    threadOutputs[t].push_back(input[i]);
                }
            }
        }
        
        // Combine thread outputs
        for (auto& threadOutput : threadOutputs) {
            output.insert(output.end(), threadOutput.begin(), threadOutput.end());
        }
    }
};

/**
 * Numerical stability optimization
 */
class NumericalStabilityOptimizer {
public:
    /**
     * Optimized exponential with numerical stability
     */
    static float stableExp(float x) {
        if (x > 30.0f) return std::exp(30.0f);
        if (x < -30.0f) return 0.0f;
        return std::exp(x);
    }
    
    /**
     * Optimized square root with numerical stability
     */
    static float stableSqrt(float x) {
        if (x <= 0.0f) return 0.0f;
        if (x > 1e12f) return std::sqrt(1e12f);
        return std::sqrt(x);
    }
    
    /**
     * Optimized log with numerical stability
     */
    static float stableLog(float x) {
        if (x <= 0.0f) return -std::numeric_limits<float>::max();
        if (x > 1e12f) return std::log(1e12f);
        return std::log(x);
    }
    
    /**
     * Optimized normalization with numerical stability
     */
    static void stableNormalize(std::vector<float>& values) {
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        if (sum > 0.0f) {
            float maxVal = *std::max_element(values.begin(), values.end());
            if (maxVal > 1e12f) {
                // Scale down to prevent overflow
                float scale = 1e12f / maxVal;
                for (float& val : values) {
                    val *= scale;
                }
                sum = std::accumulate(values.begin(), values.end(), 0.0f);
            }
            
            float invSum = 1.0f / sum;
            for (float& val : values) {
                val *= invSum;
            }
        }
    }
};

/**
 * Precision management
 */
class PrecisionManager {
public:
    enum class Precision { Single, Double, Half };
    
    /**
     * Convert precision with optimization
     */
    static void convertPrecision(std::vector<float>& data, Precision targetPrecision) {
        if (targetPrecision == Precision::Single) {
            // Already in single precision
            return;
        } else if (targetPrecision == Precision::Double) {
            // Convert to double (expensive)
            std::vector<double> doubleData(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                doubleData[i] = static_cast<double>(data[i]);
            }
            // Would require caller to handle
        } else if (targetPrecision == Precision::Half) {
            // Convert to half precision (would require FP16 support)
            // For now, keep as single precision
        }
    }
    
    /**
     * Optimized precision conversion for neural computations
     */
    static void optimizedPrecisionConversion(std::vector<float>& data, bool useHalfPrecision = false) {
        if (useHalfPrecision) {
            // Half precision optimization (simplified)
            for (float& val : data) {
                // Clip to half precision range [-65504, 65504]
                if (val > 65504.0f) val = 65504.0f;
                else if (val < -65504.0f) val = -65504.0f;
            }
        }
    }
};

} // namespace nlm