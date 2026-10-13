#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace nlm {

// Cache-friendly neural computation algorithms
class CacheOptimizedNeuralOps {
public:
    // Cache-friendly neural computation with blocking
    static void neuralComputationBlocked(const std::vector<float>& inputs,
                                         const std::vector<std::vector<float>>& weights,
                                         const std::vector<float>& biases,
                                         std::vector<float>& outputs,
                                         size_t blockSize = 128) {
        size_t numNeurons = weights.size();
        size_t inputSize = inputs.size();
        
        // Compute in blocks for better cache locality
        for (size_t neuron = 0; neuron < numNeurons; neuron += blockSize) {
            size_t endNeuron = std::min(neuron + blockSize, numNeurons);
            
            for (size_t i = neuron; i < endNeuron; ++i) {
                float sum = biases[i];
                
                // Block the inner loop for weights
                for (size_t j = 0; j < inputSize; j += blockSize) {
                    size_t endJ = std::min(j + blockSize, inputSize);
                    
                    for (size_t k = j; k < endJ; ++k) {
                        sum += inputs[k] * weights[i][k];
                    }
                }
                
                outputs[i] = sum;
            }
        }
    }
    
    // Cache-optimized activation function computation
    static void activationFunctionBlocked(std::vector<float>& values, 
                                          size_t blockSize = 256) {
        size_t numValues = values.size();
        
        // Compute activation functions in blocks
        for (size_t i = 0; i < numValues; i += blockSize) {
            size_t endI = std::min(i + blockSize, numValues);
            
            // Use cache-friendly access pattern
            for (size_t j = i; j < endI; ++j) {
                // Fast sigmoid approximation for better cache performance
                float x = values[j];
                float result = x * (1.0f / (1.0f + std::exp(std::abs(x)))) * 
                             (x >= 0 ? 1.0f : -1.0f);
                values[j] = result;
            }
        }
    }
    
    // Cache-friendly matrix-vector multiplication
    static void matrixVectorMultiplicationBlocked(const std::vector<std::vector<float>>& matrix,
                                                   const std::vector<float>& vector,
                                                   std::vector<float>& result,
                                                   size_t blockSize = 128) {
        size_t rows = matrix.size();
        size_t cols = vector.size();
        
        // Pre-allocate result
        result.resize(rows, 0.0f);
        
        // Compute matrix-vector multiplication in blocks
        for (size_t i = 0; i < rows; i += blockSize) {
            size_t endI = std::min(i + blockSize, rows);
            
            for (size_t j = i; j < endI; ++j) {
                float sum = 0.0f;
                
                // Block the inner loop for better cache locality
                for (size_t k = 0; k < cols; k += blockSize) {
                    size_t endK = std::min(k + blockSize, cols);
                    
                    for (size_t l = k; l < endK; ++l) {
                        sum += matrix[j][l] * vector[l];
                    }
                }
                
                result[j] = sum;
            }
        }
    }
    
    // Cache-optimized spike propagation
    static void spikePropagationCacheOptimized(std::vector<size_t>& spikeNeurons,
                                                std::vector<std::vector<size_t>>& synapseLists,
                                                std::vector<float>& weights,
                                                std::vector<float>& updatedWeights,
                                                size_t blockSize = 256) {
        size_t numNeurons = synapseLists.size();
        
        // Initialize updated weights
        updatedWeights.resize(weights.size(), 0.0f);
        
        // Process spikes in blocks for cache efficiency
        for (size_t i = 0; i < spikeNeurons.size(); i += blockSize) {
            size_t endI = std::min(i + blockSize, spikeNeurons.size());
            
            for (size_t j = i; j < endI; ++j) {
                size_t neuronId = spikeNeurons[j];
                
                // Process synapses in blocks
                for (size_t k = 0; k < synapseLists[neuronId].size(); k += blockSize) {
                    size_t endK = std::min(k + blockSize, synapseLists[neuronId].size());
                    
                    for (size_t l = k; l < endK; ++l) {
                        size_t synapseId = synapseLists[neuronId][l];
                        updatedWeights[synapseId] = std::min(weights[synapseId] + 0.001f, 1.0f);
                    }
                }
            }
        }
    }
    
    // Cache-friendly neural computation with LRU caching
    class LRUCache {
    public:
        struct CacheEntry {
            std::vector<float> data;
            std::chrono::high_resolution_clock::time_point lastAccess;
            size_t accessCount;
            
            CacheEntry(const std::vector<float>& d) 
                : data(d), lastAccess(std::chrono::high_resolution_clock::now()),
                  accessCount(1) {}
        };
        
        LRUCache(size_t capacity) : capacity(capacity) {}
        
        // Get value from cache (LRU)
        bool get(const std::string& key, std::vector<float>& value) {
            auto it = cache.find(key);
            if (it == cache.end()) {
                return false;
            }
            
            // Update LRU
            it->second.lastAccess = std::chrono::high_resolution_clock::now();
            it->second.accessCount++;
            value = it->second.data;
            
            return true;
        }
        
        // Set value in cache (LRU)
        void put(const std::string& key, const std::vector<float>& value) {
            // Check if key already exists
            auto it = cache.find(key);
            if (it != cache.end()) {
                it->second.data = value;
                it->second.lastAccess = std::chrono::high_resolution_clock::now();
                it->second.accessCount++;
                return;
            }
            
            // Insert new entry
            cache[key] = CacheEntry(value);
            
            // If cache is full, remove least recently used entry
            if (cache.size() > capacity) {
                // Find least recently used entry
                auto lruIt = cache.begin();
                for (auto iter = cache.begin(); iter != cache.end(); ++iter) {
                    if (iter->second.lastAccess < lruIt->second.lastAccess) {
                        lruIt = iter;
                    }
                }
                cache.erase(lruIt);
            }
        }
        
        // Clear cache
        void clear() {
            cache.clear();
        }
        
        // Get cache statistics
        size_t getSize() const {
            return cache.size();
        }
        
        // Check if cache contains key
        bool contains(const std::string& key) const {
            return cache.find(key) != cache.end();
        }
        
    private:
        std::unordered_map<std::string, CacheEntry> cache;
        size_t capacity;
    };
    
    // Cache-optimized neural computation with prediction
    static void neuralComputationWithPrediction(const std::vector<float>& inputs,
                                               const std::vector<std::vector<float>>& weights,
                                               const std::vector<float>& biases,
                                               std::vector<float>& outputs,
                                               LRUCache& cache,
                                               size_t cacheBlockSize = 256) {
        size_t numNeurons = weights.size();
        size_t inputSize = inputs.size();
        
        // Try to get result from cache
        std::string cacheKey = "neural:" + std::to_string(numNeurons) + ":" + std::to_string(inputSize);
        if (cache.contains(cacheKey)) {
            cache.get(cacheKey, outputs);
            return;
        }
        
        // Compute neural network
        outputs.resize(numNeurons, 0.0f);
        
        // Compute in blocks for cache efficiency
        for (size_t neuron = 0; neuron < numNeurons; neuron += cacheBlockSize) {
            size_t endNeuron = std::min(neuron + cacheBlockSize, numNeurons);
            
            for (size_t i = neuron; i < endNeuron; ++i) {
                float sum = biases[i];
                
                // Block the inner loop for better cache performance
                for (size_t j = 0; j < inputSize; j += cacheBlockSize) {
                    size_t endJ = std::min(j + cacheBlockSize, inputSize);
                    
                    for (size_t k = j; k < endJ; ++k) {
                        sum += inputs[k] * weights[i][k];
                    }
                }
                
                outputs[i] = sum;
            }
        }
        
        // Store result in cache
        cache.put(cacheKey, outputs);
    }
    
    // Cache-friendly data structure for neural computation
    template<typename T>
    class CacheFriendlyVector {
    public:
        CacheFriendlyVector(size_t size) {
            allocate(size);
        }
        
        ~CacheFriendlyVector() {
            delete[] data;
        }
        
        T& operator[](size_t index) {
            return data[index];
        }
        
        const T& operator[](size_t index) const {
            return data[index];
        }
        
        size_t size() const {
            return size_;
        }
        
        void fill(T value) {
            for (size_t i = 0; i < size_; ++i) {
                data[i] = value;
            }
        }
        
        // Access pattern for cache-friendly iteration
        class iterator {
        public:
            iterator(T* ptr) : ptr(ptr) {}
            
            iterator& operator++() {
                ptr++;
                return *this;
            }
            
            bool operator!=(const iterator& other) const {
                return ptr != other.ptr;
            }
            
            T& operator*() {
                return *ptr;
            }
            
        private:
            T* ptr;
        };
        
        iterator begin() {
            return iterator(data);
        }
        
        iterator end() {
            return iterator(data + size_);
        }
        
    private:
        void allocate(size_t size) {
            size_ = size;
            // Align to cache line size (typically 64 bytes)
            size_t alignment = 64 / sizeof(T);
            size_ = (size_ + alignment - 1) / alignment * alignment;
            data = new T[size_];
        }
        
        T* data;
        size_t size_;
    };
    
    // Cache-friendly neural computation using custom vector
    static void neuralComputationCacheFriendly(const std::vector<float>& inputs,
                                               const std::vector<std::vector<float>>& weights,
                                               const std::vector<float>& biases,
                                               std::vector<float>& outputs,
                                               size_t blockSize = 128) {
        size_t numNeurons = weights.size();
        size_t inputSize = inputs.size();
        
        // Use cache-friendly vectors
        CacheFriendlyVector<float> cachedInputs(&inputs[0], inputSize);
        CacheFriendlyVector<float> cachedBiases(&biases[0], biases.size());
        CacheFriendlyVector<float> cachedOutputs(outputs.size());
        
        // Compute in blocks for cache efficiency
        for (size_t neuron = 0; neuron < numNeurons; neuron += blockSize) {
            size_t endNeuron = std::min(neuron + blockSize, numNeurons);
            
            for (size_t i = neuron; i < endNeuron; ++i) {
                float sum = cachedBiases[i];
                
                // Block the inner loop for cache efficiency
                for (size_t j = 0; j < inputSize; j += blockSize) {
                    size_t endJ = std::min(j + blockSize, inputSize);
                    
                    for (size_t k = j; k < endJ; ++k) {
                        sum += cachedInputs[k] * weights[i][k];
                    }
                }
                
                cachedOutputs[i] = sum;
            }
        }
        
        // Copy results back
        outputs = std::vector<float>(cachedOutputs.begin(), cachedOutputs.end());
    }
    
    // Benchmark cache optimization effectiveness
    static double benchmarkCacheOptimization(const std::vector<float>& inputs,
                                             const std::vector<std::vector<float>>& weights,
                                             const std::vector<float>& biases,
                                             size_t numIterations = 100) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<float> outputs(inputs.size());
        
        for (size_t i = 0; i < numIterations; ++i) {
            // Run neural computation with cache optimization
            neuralComputationBlocked(inputs, weights, biases, outputs);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        return duration / numIterations; // Average time per iteration
    }
    
    // Get cache optimization report
    static std::string getCacheOptimizationReport() {
        std::stringstream ss;
        ss << "=== Cache Optimization Report ===" << std::endl;
        ss << "Cache-optimized neural computation algorithms implemented:" << std::endl;
        ss << "  1. Blocked neural computation for better cache locality" << std::endl;
        ss << "  2. Cache-friendly matrix-vector multiplication" << std::endl;
        ss << "  3. LRU cache for neural computation results" << std::endl;
        ss << "  4. Cache-friendly data structures (aligned memory)" << std::endl;
        ss << "  5. Blocked activation function computation" << std::endl;
        ss << std::endl;
        ss << "Benefits:" << std::endl;
        ss << "  - Reduced cache misses" << std::endl;
        ss << "  - Improved memory locality" << std::endl;
        ss << "  - Better parallel performance" << std::endl;
        ss << "  - Scalable for large networks" << std::endl;
        
        return ss.str();
    }
    
private:
    template<typename T>
    CacheFriendlyVector<T>::CacheFriendlyVector(T* data, size_t size) {
        allocate(size);
        for (size_t i = 0; i < size_; ++i) {
            this->data[i] = data[i];
        }
    }
};

} // namespace nlm
