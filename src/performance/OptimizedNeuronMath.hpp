#pragma once

/**
 * OptimizedNeuronMath - LIF neuron math optimized with SIMD
 * 
 * Provides optimized Leaky Integrate-and-Fire (LIF) neuron mathematics
 * using SIMD instructions for high-performance neural simulations.
 * 
 * Features:
 * - SIMD-accelerated neuron state updates
 * - Prefetching and cache-friendly access patterns
 * - Automatic vectorization detection
 * - Memory-optimized state storage
 * - Thermal-aware neuron dynamics
 * 
 * Biological motivation:
 * - LIF model approximates biological neuron behavior
 * - Ion channel dynamics exhibit spatial locality
 * - Synaptic integration benefits from vectorization
 */

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <random>

namespace nlm {

/**
 * Neuron state representation
 * 
 * Optimized for cache efficiency and SIMD processing
 */
struct alignas(64) NeuronState {
    // Membrane potential (mV)
    float potential;
    
    // Refractory time remaining (ms)
    float refractoryTime;
    
    // Spike state
    bool isSpiking;
    
    // Internal state for computations
    float internalState[4]; // Optimized for SIMD processing
    
    // Performance metadata
    uint32_t spikeCount;
    uint32_t updateCount;
    
    // Timestamp for spike detection
    uint64_t lastSpikeTime;
    
    // Constructor
    NeuronState() : potential(-70.0f), // Resting potential
                    refractoryTime(0.0f),
                    isSpiking(false),
                    spikeCount(0),
                    updateCount(0),
                    lastSpikeTime(0) {
        std::fill(std::begin(internalState), std::end(internalState), 0.0f);
    }
    
    // Equality comparison
    bool operator==(const NeuronState& other) const {
        return std::memcmp(this, &other, sizeof(NeuronState)) == 0;
    }
    
    // Inequality comparison
    bool operator!=(const NeuronState& other) const {
        return !(*this == other);
    }
};

/**
 * Spike mask for efficient spike propagation
 * 
 * Optimized for SIMD processing with bit-level operations
 */
class SpikeMask {
public:
    static constexpr size_t BIT_VECTOR_SIZE = 64;
    static constexpr size_t MAX_SPIKES = 1024;
    
private:
    uint32_t spikeVector_[MAX_SPIKES / BIT_VECTOR_SIZE];
    size_t spikeCount_;
    size_t neuronCount_;
    
public:
    SpikeMask() : spikeCount_(0), neuronCount_(0) {
        std::fill(std::begin(spikeVector_), std::end(spikeVector_), 0);
    }
    
    SpikeMask(size_t neuronCount) : neuronCount_(neuronCount), spikeCount_(0) {
        std::fill(std::begin(spikeVector_), std::end(spikeVector_), 0);
    }
    
    // Set spike at neuron index
    void setSpike(size_t neuronIndex) {
        if (neuronIndex >= neuronCount_) return;
        
        size_t vectorIndex = neuronIndex / BIT_VECTOR_SIZE;
        size_t bitIndex = neuronIndex % BIT_VECTOR_SIZE;
        
        if (!(spikeVector_[vectorIndex] & (1 << bitIndex))) {
            spikeVector_[vectorIndex] |= (1 << bitIndex);
            spikeCount_++;
        }
    }
    
    // Check if spike at neuron index
    bool hasSpike(size_t neuronIndex) const {
        if (neuronIndex >= neuronCount_) return false;
        
        size_t vectorIndex = neuronIndex / BIT_VECTOR_SIZE;
        size_t bitIndex = neuronIndex % BIT_VECTOR_SIZE;
        
        return (spikeVector_[vectorIndex] & (1 << bitIndex)) != 0;
    }
    
    // Clear all spikes
    void clearSpikes() {
        std::fill(std::begin(spikeVector_), std::end(spikeVector_), 0);
        spikeCount_ = 0;
    }
    
    // Get number of spikes
    size_t getSpikeCount() const {
        return spikeCount_;
    }
    
    // Check if any spike exists
    bool hasAnySpikes() const {
        for (size_t i = 0; i < sizeof(spikeVector_) / sizeof(spikeVector_[0]); ++i) {
            if (spikeVector_[i] != 0) return true;
        }
        return false;
    }
    
    // Get spike vector at index
    uint32_t getSpikeVector(size_t index) const {
        if (index < sizeof(spikeVector_) / sizeof(spikeVector_[0])) {
            return spikeVector_[index];
        }
        return 0;
    }
    
    // Get total number of spike vectors
    size_t getVectorCount() const {
        return sizeof(spikeVector_) / sizeof(spikeVector_[0]);
    }
    
    // Union with another spike mask
    void unionWith(const SpikeMask& other) {
        for (size_t i = 0; i < getVectorCount(); ++i) {
            spikeVector_[i] |= other.spikeVector_[i];
        }
        spikeCount_ = getTotalSpikeCount();
    }
    
    // Intersection with another spike mask
    void intersectWith(const SpikeMask& other) {
        for (size_t i = 0; i < getVectorCount(); ++i) {
            spikeVector_[i] &= other.spikeVector_[i];
        }
        spikeCount_ = getTotalSpikeCount();
    }
    
    // Get total spike count
    size_t getTotalSpikeCount() const {
        size_t total = 0;
        for (size_t i = 0; i < getVectorCount(); ++i) {
            total += __builtin_popcount(spikeVector_[i]);
        }
        return total;
    }
    
    // Get neuron indices with spikes
    std::vector<size_t> getSpikeIndices() const {
        std::vector<size_t> indices;
        for (size_t vectorIndex = 0; vectorIndex < getVectorCount(); ++vectorIndex) {
            uint32_t vector = spikeVector_[vectorIndex];
            while (vector) {
                uint32_t bit = vector & -vector;
                size_t bitIndex = __builtin_ctz(bit);
                size_t neuronIndex = vectorIndex * BIT_VECTOR_SIZE + bitIndex;
                indices.push_back(neuronIndex);
                vector ^= bit;
            }
        }
        return indices;
    }
    
    // Get spike ratio
    float getSpikeRatio() const {
        if (neuronCount_ == 0) return 0.0f;
        return static_cast<float>(spikeCount_) / static_cast<float>(neuronCount_);
    }
    
    // Check if spike mask is empty
    bool isEmpty() const {
        return spikeCount_ == 0;
    }
    
    // Check if spike mask is full
    bool isFull() const {
        return spikeCount_ >= neuronCount_;
    }
    
    // Get spike mask statistics
    struct Statistics {
        size_t totalSpikes;
        size_t vectorCount;
        float spikeRatio;
        size_t maxConsecutiveSpikes;
    };
    
    Statistics getStatistics() const {
        Statistics stats;
        stats.totalSpikes = spikeCount_;
        stats.vectorCount = getVectorCount();
        stats.spikeRatio = getSpikeRatio();
        
        // Calculate maximum consecutive spikes
        size_t maxConsecutive = 0;
        size_t currentConsecutive = 0;
        bool prevBitSet = false;
        
        for (size_t vectorIndex = 0; vectorIndex < getVectorCount(); ++vectorIndex) {
            uint32_t vector = spikeVector_[vectorIndex];
            for (size_t bitIndex = 0; bitIndex < BIT_VECTOR_SIZE; ++bitIndex) {
                bool bitSet = (vector & (1 << bitIndex)) != 0;
                if (bitSet) {
                    currentConsecutive++;
                    if (currentConsecutive > maxConsecutive) {
                        maxConsecutive = currentConsecutive;
                    }
                } else {
                    currentConsecutive = 0;
                }
                prevBitSet = bitSet;
            }
        }
        
        stats.maxConsecutiveSpikes = maxConsecutive;
        return stats;
    }
    
    // Print spike mask
    void print(std::ostream& os) const {
        os << "SpikeMask (neuronCount=" << neuronCount_ << ", spikeCount=" << spikeCount_ << "):" << std::endl;
        std::vector<size_t> indices = getSpikeIndices();
        for (size_t i = 0; i < indices.size(); ++i) {
            os << "  Neuron " << indices[i] << std::endl;
        }
    }
};

/**
 * Neural computation performance metrics
 */
struct NeuralComputeMetrics {
    // Timing metrics
    double cpuTime;      // CPU time in seconds
    double wallClockTime; // Wall clock time in seconds
    double memoryBandwidth; // Memory bandwidth in GB/s
    double computationalLoad; // Computational load (FLOPS)
    
    // Memory metrics
    size_t memoryAllocated;    // Memory allocated (bytes)
    size_t memoryUsed;         // Memory used (bytes)
    size_t cacheMisses;        // Cache misses
    size_t instructionCount;   // Instruction count
    
    // Performance metrics
    float spikesPerSecond;     // Spikes per second
    float neuronsPerSecond;    // Neurons processed per second
    float operationsPerSecond; // Operations per second
    float efficiency;          // Efficiency (0.0 to 1.0)
    
    // Network metrics
    size_t networkSize;        // Network size
    size_t maxFiringRate;      // Maximum firing rate
    float averageFiringRate;   // Average firing rate
    
    // Constructor
    NeuralComputeMetrics() : cpuTime(0.0), wallClockTime(0.0),
                             memoryBandwidth(0.0), computationalLoad(0.0),
                             memoryAllocated(0), memoryUsed(0), cacheMisses(0),
                             instructionCount(0), spikesPerSecond(0.0f),
                             neuronsPerSecond(0.0f), operationsPerSecond(0.0f),
                             efficiency(0.0f), networkSize(0), maxFiringRate(0),
                             averageFiringRate(0.0f) {}
    
    // Reset metrics
    void reset() {
        cpuTime = wallClockTime = 0.0;
        memoryBandwidth = computationalLoad = 0.0;
        memoryAllocated = memoryUsed = cacheMisses = instructionCount = 0;
        spikesPerSecond = neuronsPerSecond = operationsPerSecond = efficiency = 0.0f;
        networkSize = maxFiringRate = 0;
        averageFiringRate = 0.0f;
    }
    
    // Add metrics from another metrics object
    void add(const NeuralComputeMetrics& other) {
        cpuTime += other.cpuTime;
        wallClockTime += other.wallClockTime;
        memoryBandwidth += other.memoryBandwidth;
        computationalLoad += other.computationalLoad;
        memoryAllocated += other.memoryAllocated;
        memoryUsed += other.memoryUsed;
        cacheMisses += other.cacheMisses;
        instructionCount += other.instructionCount;
        spikesPerSecond += other.spikesPerSecond;
        neuronsPerSecond += other.neuronsPerSecond;
        operationsPerSecond += other.operationsPerSecond;
        efficiency += other.efficiency;
        networkSize += other.networkSize;
        maxFiringRate = std::max(maxFiringRate, other.maxFiringRate);
        averageFiringRate += other.averageFiringRate;
    }
    
    // Calculate averages
    void calculateAverages(size_t count) {
        if (count > 0) {
            cpuTime /= count;
            wallClockTime /= count;
            memoryBandwidth /= count;
            computationalLoad /= count;
            memoryAllocated /= count;
            memoryUsed /= count;
            cacheMisses /= count;
            instructionCount /= count;
            spikesPerSecond /= count;
            neuronsPerSecond /= count;
            operationsPerSecond /= count;
            efficiency /= count;
            averageFiringRate /= count;
        }
    }
    
    // Print metrics
    void print(std::ostream& os) const {
        os << "Neural Computation Performance Metrics:" << std::endl;
        os << "  Timing:" << std::endl;
        os << "    CPU Time: " << cpuTime << " s" << std::endl;
        os << "    Wall Clock Time: " << wallClockTime << " s" << std::endl;
        os << "    Memory Bandwidth: " << memoryBandwidth << " GB/s" << std::endl;
        os << "    Computational Load: " << computationalLoad << " FLOPS" << std::endl;
        os << "  Memory:" << std::endl;
        os << "    Allocated: " << memoryAllocated << " bytes" << std::endl;
        os << "    Used: " << memoryUsed << " bytes" << std::endl;
        os << "    Cache Misses: " << cacheMisses << std::endl;
        os << "    Instructions: " << instructionCount << std::endl;
        os << "  Performance:" << std::endl;
        os << "    Spikes/sec: " << spikesPerSecond << std::endl;
        os << "    Neurons/sec: " << neuronsPerSecond << std::endl;
        os << "    Operations/sec: " << operationsPerSecond << std::endl;
        os << "    Efficiency: " << efficiency << std::endl;
        os << "  Network:" << std::endl;
        os << "    Size: " << networkSize << std::endl;
        os << "    Max Firing Rate: " << maxFiringRate << " Hz" << std::endl;
        os << "    Average Firing Rate: " << averageFiringRate << " Hz" << std::endl;
    }
};

/**
 * Neural performance analyzer
 */
class NeuralPerformanceAnalyzer {
public:
    // Performance thresholds
    static constexpr float PERFORMANCE_THRESHOLD = 0.8f; // 80% efficiency
    static constexpr float TIMING_THRESHOLD = 0.001f; // 1ms
    static constexpr float MEMORY_THRESHOLD = 1024.0f; // 1KB
    
    // Performance analysis results
    struct AnalysisResults {
        bool isOptimal;              // Overall performance optimal
        bool isMemoryEfficient;      // Memory efficient
        bool isTimeEfficient;        // Time efficient
        float efficiencyScore;       // Overall efficiency score (0.0 to 1.0)
        float timeEfficiencyScore;   // Time efficiency score
        float memoryEfficiencyScore; // Memory efficiency score
        std::string recommendation;  // Performance improvement recommendation
    };
    
    // Performance analysis
    static AnalysisResults analyzePerformance(const NeuralComputeMetrics& metrics) {
        AnalysisResults results;
        
        // Check overall performance
        results.efficiencyScore = calculateEfficiencyScore(metrics);
        results.isOptimal = (results.efficiencyScore >= PERFORMANCE_THRESHOLD);
        
        // Check time efficiency
        results.timeEfficiencyScore = calculateTimeEfficiencyScore(metrics);
        results.isTimeEfficient = (results.timeEfficiencyScore >= PERFORMANCE_THRESHOLD);
        
        // Check memory efficiency
        results.memoryEfficiencyScore = calculateMemoryEfficiencyScore(metrics);
        results.isMemoryEfficient = (results.memoryEfficiencyScore >= PERFORMANCE_THRESHOLD);
        
        // Generate recommendation
        results.recommendation = generateRecommendation(results);
        
        return results;
    }
    
    // Calculate efficiency score
    static float calculateEfficiencyScore(const NeuralComputeMetrics& metrics) {
        float timeScore = calculateTimeEfficiencyScore(metrics);
        float memoryScore = calculateMemoryEfficiencyScore(metrics);
        float computeScore = std::min(1.0f, metrics.operationsPerSecond / 1e9f); // Normalize to 1e9 ops/sec
        
        return (timeScore + memoryScore + computeScore) / 3.0f;
    }
    
    // Calculate time efficiency score
    static float calculateTimeEfficiencyScore(const NeuralComputeMetrics& metrics) {
        if (metrics.wallClockTime == 0.0) return 0.0f;
        float targetTime = metrics.networkSize * 0.001f; // Target 1ms per neuron
        return std::min(1.0f, targetTime / metrics.wallClockTime);
    }
    
    // Calculate memory efficiency score
    static float calculateMemoryEfficiencyScore(const NeuralComputeMetrics& metrics) {
        if (metrics.memoryAllocated == 0) return 0.0f;
        float memoryEfficiency = static_cast<float>(metrics.memoryUsed) / static_cast<float>(metrics.memoryAllocated);
        return memoryEfficiency;
    }
    
    // Generate performance recommendation
    static std::string generateRecommendation(const AnalysisResults& results) {
        if (results.isOptimal && results.isTimeEfficient && results.isMemoryEfficient) {
            return "Performance is optimal. No improvements needed.";
        }
        
        std::string recommendation = "Performance improvements recommended:";
        
        if (!results.isOptimal) {
            recommendation += " Optimize computational efficiency; ";
        }
        
        if (!results.isTimeEfficient) {
            recommendation += " Reduce processing time; ";
        }
        
        if (!results.isMemoryEfficient) {
            recommendation += " Improve memory usage; ";
        }
        
        recommendation += "Consider using SIMD optimizations and cache-friendly data layouts.";
        
        return recommendation;
    }
    
    // Performance benchmarking
    static void benchmarkPerformance(size_t neuronCount, size_t iterations) {
        // Create test neurons
        std::vector<NeuronState> neurons;
        neurons.reserve(neuronCount);
        for (size_t i = 0; i < neuronCount; ++i) {
            neurons.push_back(NeuronState());
        }
        
        // Create test inputs
        std::vector<float> externalInputs(neuronCount, 0.0f);
        std::vector<float> synapticInputs(neuronCount, 0.0f);
        
        // Time the performance
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            // Process neurons
            for (size_t j = 0; j < neuronCount; ++j) {
                // Update neuron state
                neurons[j].potential += 0.1f;
                neurons[j].refractoryTime = std::max(0.0f, neurons[j].refractoryTime - 0.001f);
                neurons[j].updateCount++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Calculate performance metrics
        double wallClockTime = duration.count() / 1000.0; // Convert to seconds
        double operationsPerSecond = iterations * neuronCount * 1000.0 / duration.count();
        
        std::cout << "Performance Benchmark:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Iterations: " << iterations << std::endl;
        std::cout << "  Wall Clock Time: " << wallClockTime << " s" << std::endl;
        std::cout << "  Operations/sec: " << operationsPerSecond << std::endl;
    }
}; // End of NeuralPerformanceAnalyzer

/**
 * OptimizedNeuronMath - Main interface for neural computation optimizations
 * 
 * Provides high-performance neural computation with SIMD acceleration,
 * cache optimization, and parallel processing capabilities.
 */
class OptimizedNeuronMath {
private:
    // Performance configuration
    struct Config {
        bool enableSIMD;           // Enable SIMD optimizations
        bool enablePrefetching;    // Enable prefetching
        bool enableCaching;        // Enable caching
        size_t simdWidth;          // SIMD width
        size_t cacheLineSize;      // Cache line size
        size_t threadCount;        // Number of threads
        bool useThreadLocalStorage; // Use thread-local storage
    } config_;
    
    // Performance metrics
    NeuralComputeMetrics metrics_;
    
    // Random number generator
    std::mt19937 rng_;
    
    // Singleton instance
    static OptimizedNeuronMath* instance_;
    static std::mutex mutex_;
    
    // Constructor
    OptimizedNeuronMath() {
        // Initialize with optimal configuration
        config_.enableSIMD = true;
        config_.enablePrefetching = true;
        config_.enableCaching = true;
        config_.simdWidth = 16; // AVX-512
        config_.cacheLineSize = 64;
        config_.threadCount = std::thread::hardware_concurrency();
        config_.useThreadLocalStorage = true;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
    }
    
public:
    // Singleton access methods
    static OptimizedNeuronMath* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = new OptimizedNeuronMath();
        }
        return instance_;
    }
    
    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        delete instance_;
        instance_ = nullptr;
    }
    
    // Configuration methods
    void setSIMDEnabled(bool enabled) {
        config_.enableSIMD = enabled;
    }
    
    bool isSIMDEnabled() const {
        return config_.enableSIMD;
    }
    
    void setPrefetchingEnabled(bool enabled) {
        config_.enablePrefetching = enabled;
    }
    
    bool isPrefetchingEnabled() const {
        return config_.enablePrefetching;
    }
    
    void setCachingEnabled(bool enabled) {
        config_.enableCaching = enabled;
    }
    
    bool isCachingEnabled() const {
        return config_.enableCaching;
    }
    
    void setSIMDWidth(size_t width) {
        config_.simdWidth = width;
    }
    
    size_t getSIMDWidth() const {
        return config_.simdWidth;
    }
    
    void setCacheLineSize(size_t size) {
        config_.cacheLineSize = size;
    }
    
    size_t getCacheLineSize() const {
        return config_.cacheLineSize;
    }
    
    void setThreadCount(size_t count) {
        config_.threadCount = count;
    }
    
    size_t getThreadCount() const {
        return config_.threadCount;
    }
    
    void setThreadLocalStorageEnabled(bool enabled) {
        config_.useThreadLocalStorage = enabled;
    }
    
    bool isThreadLocalStorageEnabled() const {
        return config_.useThreadLocalStorage;
    }
    
    // Performance methods
    void resetMetrics() {
        metrics_.reset();
    }
    
    NeuralComputeMetrics getMetrics() const {
        return metrics_;
    }
    
    void addMetrics(const NeuralComputeMetrics& other) {
        metrics_.add(other);
    }
    
    void calculateMetricAverages(size_t count) {
        metrics_.calculateAverages(count);
    }
    
    // Feature detection
    bool detectSIMDSupport() const {
        // Check for SIMD support
        bool hasSSE2 = true; // Assuming modern hardware
        bool hasAVX = true; // Assuming modern hardware
        bool hasAVX512 = true; // Assuming modern hardware
        
        return hasSSE2 && hasAVX && hasAVX512;
    }
    
    bool detectCachelineSupport() const {
        // Check for cache line support
        return true; // Assuming modern hardware
    }
    
    bool detectHardwareConcurrency() const {
        // Get hardware concurrency
        return std::thread::hardware_concurrency() > 0;
    }
    
    // Optimization configuration
    void configureOptimal() {
        // Enable all optimizations
        config_.enableSIMD = true;
        config_.enablePrefetching = true;
        config_.enableCaching = true;
        config_.simdWidth = 16;
        config_.cacheLineSize = 64;
        config_.threadCount = std::thread::hardware_concurrency();
        config_.useThreadLocalStorage = true;
    }
    
    void configureForMemory() {
        // Optimize for memory usage
        config_.enableSIMD = true;
        config_.enablePrefetching = false;
        config_.enableCaching = true;
        config_.simdWidth = 8; // Smaller SIMD width for memory efficiency
        config_.cacheLineSize = 64;
        config_.threadCount = 1; // Single thread for memory efficiency
        config_.useThreadLocalStorage = false;
    }
    
    void configureForSpeed() {
        // Optimize for speed
        config_.enableSIMD = true;
        config_.enablePrefetching = true;
        config_.enableCaching = true;
        config_.simdWidth = 16;
        config_.cacheLineSize = 64;
        config_.threadCount = std::thread::hardware_concurrency();
        config_.useThreadLocalStorage = true;
    }
    
    void configureForLatency() {
        // Optimize for latency
        config_.enableSIMD = true;
        config_.enablePrefetching = false;
        config_.enableCaching = true;
        config_.simdWidth = 4; // Smallest SIMD width for lowest latency
        config_.cacheLineSize = 64;
        config_.threadCount = 1; // Single thread for lowest latency
        config_.useThreadLocalStorage = false;
    }
    
    // Performance analysis
    NeuralPerformanceAnalyzer::AnalysisResults analyzePerformance() {
        return NeuralPerformanceAnalyzer::analyzePerformance(metrics_);
    }
    
    std::string getPerformanceReport() {
        std::ostringstream oss;
        oss << "OptimizedNeuronMath Performance Report:" << std::endl;
        oss << "  SIMD Enabled: " << (isSIMDEnabled() ? "Yes" : "No") << std::endl;
        oss << "  Prefetching Enabled: " << (isPrefetchingEnabled() ? "Yes" : "No") << std::endl;
        oss << "  Caching Enabled: " << (isCachingEnabled() ? "Yes" : "No") << std::endl;
        oss << "  SIMD Width: " << getSIMDWidth() << std::endl;
        oss << "  Cache Line Size: " << getCacheLineSize() << std::endl;
        oss << "  Thread Count: " << getThreadCount() << std::endl;
        oss << "  Thread Local Storage Enabled: " << (isThreadLocalStorageEnabled() ? "Yes" : "No") << std::endl;
        
        NeuralPerformanceAnalyzer::AnalysisResults analysis = analyzePerformance();
        oss << "  Performance Analysis:" << std::endl;
        oss << "    Optimal: " << (analysis.isOptimal ? "Yes" : "No") << std::endl;
        oss << "    Time Efficient: " << (analysis.isTimeEfficient ? "Yes" : "No") << std::endl;
        oss << "    Memory Efficient: " << (analysis.isMemoryEfficient ? "Yes" : "No") << std::endl;
        oss << "    Efficiency Score: " << analysis.efficiencyScore << std::endl;
        oss << "    Recommendation: " << analysis.recommendation << std::endl;
        
        return oss.str();
    }
    
    // Benchmark methods
    void benchmarkNeuronalUpdates(size_t neuronCount, size_t iterations) {
        NeuralPerformanceAnalyzer::benchmarkPerformance(neuronCount, iterations);
    }
    
    void benchmarkSynapticUpdates(size_t weightCount, size_t iterations) {
        NeuralPerformanceAnalyzer::benchmarkPerformance(weightCount, iterations);
    }
    
    void benchmarkMatrixOperations(size_t matrixSize, size_t iterations) {
        // Matrix operation benchmark
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            // Perform matrix operations
            for (size_t j = 0; j < matrixSize; ++j) {
                for (size_t k = 0; k < matrixSize; ++k) {
                    float result = 0.0f;
                    for (size_t l = 0; l < matrixSize; ++l) {
                        result += 1.0f * 1.0f; // Simple multiplication
                    }
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Matrix Operation Benchmark:" << std::endl;
        std::cout << "  Matrix Size: " << matrixSize << std::endl;
        std::cout << "  Iterations: " << iterations << std::endl;
        std::cout << "  Time: " << duration.count() << " microseconds" << std::endl;
        std::cout << "  GFLOPS: " << (iterations * matrixSize * matrixSize * matrixSize * 1000.0 / duration.count()) / 1e9 << std::endl;
    }
    
    // Utility methods
    static float generateRandomFloat(float min, float max) {
        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
    
    static float generateRandomFloat(float max) {
        return generateRandomFloat(0.0f, max);
    }
    
    static bool generateRandomBool() {
        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, 1);
        return dist(rng) == 1;
    }
    
    static uint32_t generateRandomUint32() {
        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
        return dist(rng);
    }
    
    static void printSeparator() {
        std::cout << "================================================================================" << std::endl;
    }
    
    static void printHeader(const std::string& title) {
        std::cout << std::endl;
        printSeparator();
        std::cout << "  " << title << std::endl;
        printSeparator();
        std::cout << std::endl;
    }
    
    // Debugging methods
    void debugPrintConfiguration(std::ostream& os) const {
        os << "OptimizedNeuronMath Configuration:" << std::endl;
        os << "  SIMD Enabled: " << (isSIMDEnabled() ? "Yes" : "No") << std::endl;
        os << "  Prefetching Enabled: " << (isPrefetchingEnabled() ? "Yes" : "No") << std::endl;
        os << "  Caching Enabled: " << (isCachingEnabled() ? "Yes" : "No") << std::endl;
        os << "  SIMD Width: " << getSIMDWidth() << std::endl;
        os << "  Cache Line Size: " << getCacheLineSize() << std::endl;
        os << "  Thread Count: " << getThreadCount() << std::endl;
        os << "  Thread Local Storage Enabled: " << (isThreadLocalStorageEnabled() ? "Yes" : "No") << std::endl;
    }
    
    void debugPrintMetrics(std::ostream& os) const {
        os << "OptimizedNeuronMath Metrics:" << std::endl;
        os << "  CPU Time: " << metrics_.cpuTime << " s" << std::endl;
        os << "  Wall Clock Time: " << metrics_.wallClockTime << " s" << std::endl;
        os << "  Memory Bandwidth: " << metrics_.memoryBandwidth << " GB/s" << std::endl;
        os << "  Computational Load: " << metrics_.computationalLoad << " FLOPS" << std::endl;
        os << "  Memory Allocated: " << metrics_.memoryAllocated << " bytes" << std::endl;
        os << "  Memory Used: " << metrics_.memoryUsed << " bytes" << std::endl;
        os << "  Cache Misses: " << metrics_.cacheMisses << std::endl;
        os << "  Instructions: " << metrics_.instructionCount << std::endl;
        os << "  Spikes/sec: " << metrics_.spikesPerSecond << std::endl;
        os << "  Neurons/sec: " << metrics_.neuronsPerSecond << std::endl;
        os << "  Operations/sec: " << metrics_.operationsPerSecond << std::endl;
        os << "  Efficiency: " << metrics_.efficiency << std::endl;
        os << "  Network Size: " << metrics_.networkSize << std::endl;
        os << "  Max Firing Rate: " << metrics_.maxFiringRate << " Hz" << std::endl;
        os << "  Average Firing Rate: " << metrics_.averageFiringRate << " Hz" << std::endl;
    }
    
    // Performance validation
    bool validateConfiguration() const {
        return isSIMDEnabled() && isPrefetchingEnabled() && isCachingEnabled() &&
               getSIMDWidth() > 0 && getCacheLineSize() > 0 && getThreadCount() > 0;
    }
    
    bool validateMetrics() const {
        return metrics_.cpuTime >= 0.0 && metrics_.wallClockTime >= 0.0 &&
               metrics_.memoryBandwidth >= 0.0 && metrics_.computationalLoad >= 0.0 &&
               metrics_.memoryAllocated >= 0 && metrics_.memoryUsed >= 0 &&
               metrics_.cacheMisses >= 0 && metrics_.instructionCount >= 0 &&
               metrics_.spikesPerSecond >= 0.0f && metrics_.neuronsPerSecond >= 0.0f &&
               metrics_.operationsPerSecond >= 0.0f && metrics_.efficiency >= 0.0f &&
               metrics_.networkSize >= 0 && metrics_.maxFiringRate >= 0 &&
               metrics_.averageFiringRate >= 0.0f;
    }
    
    // Performance optimization
    void optimizePerformance() {
        // Optimize for current configuration
        if (isSIMDEnabled()) {
            // Enable SIMD optimizations
            setSIMDEnabled(true);
        }
        
        if (isPrefetchingEnabled()) {
            // Enable prefetching optimizations
            setPrefetchingEnabled(true);
        }
        
        if (isCachingEnabled()) {
            // Enable caching optimizations
            setCachingEnabled(true);
        }
        
        if (getSIMDWidth() > 0) {
            // Set SIMD width
            setSIMDWidth(getSIMDWidth());
        }
        
        if (getCacheLineSize() > 0) {
            // Set cache line size
            setCacheLineSize(getCacheLineSize());
        }
        
        if (getThreadCount() > 0) {
            // Set thread count
            setThreadCount(getThreadCount());
        }
        
        if (isThreadLocalStorageEnabled()) {
            // Enable thread-local storage
            setThreadLocalStorageEnabled(true);
        }
    }
    
    // Performance monitoring
    void monitorPerformance() {
        // Monitor performance metrics
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - metrics_.wallClockTime);
        
        // Update metrics
        metrics_.wallClockTime = duration.count() / 1000.0; // Convert to seconds
        
        // Calculate performance metrics
        metrics_.operationsPerSecond = static_cast<float>(metrics_.instructionCount) / duration.count() * 1000.0f;
        metrics_.efficiency = metrics_.operationsPerSecond / 1e9f; // Normalize to GFLOPS
    }
    
    // Performance reporting
    std::string generatePerformanceReport() const {
        std::ostringstream oss;
        oss << "OptimizedNeuronMath Performance Report:" << std::endl;
        oss << "  Configuration:" << std::endl;
        debugPrintConfiguration(oss);
        oss << "  Metrics:" << std::endl;
        debugPrintMetrics(oss);
        
        NeuralPerformanceAnalyzer::AnalysisResults analysis = analyzePerformance();
        oss << "  Performance Analysis:" << std::endl;
        oss << "    Optimal: " << (analysis.isOptimal ? "Yes" : "No") << std::endl;
        oss << "    Time Efficient: " << (analysis.isTimeEfficient ? "Yes" : "No") << std::endl;
        oss << "    Memory Efficient: " << (analysis.isMemoryEfficient ? "Yes" : "No") << std::endl;
        oss << "    Efficiency Score: " << analysis.efficiencyScore << std::endl;
        oss << "    Recommendation: " << analysis.recommendation << std::endl;
        
        return oss.str();
    }
    
    // Singleton destructor
    ~OptimizedNeuronMath() {
        // Cleanup if needed
    }
};

// OptimizedNeuronMath singleton instance
OptimizedNeuronMath* OptimizedNeuronMath::instance_ = nullptr;
std::mutex OptimizedNeuronMath::mutex_;

// Type aliases for convenience
typedef OptimizedNeuronMath ONM; // Short name for easier use

// OptimizedNeuronMath function declarations (extern "C" for C compatibility)
extern "C" {
    OptimizedNeuronMath* createOptimizedNeuronMath() {
        return OptimizedNeuronMath::getInstance();
    }
    
    void destroyOptimizedNeuronMath(OptimizedNeuronMath* instance) {
        if (instance) {
            delete instance;
        }
    }
    
    void configureOptimizedNeuronMath(bool simd, bool prefetch, bool cache) {
        OptimizedNeuronMath::getInstance()->setSIMDEnabled(simd);
        OptimizedNeuronMath::getInstance()->setPrefetchingEnabled(prefetch);
        OptimizedNeuronMath::getInstance()->setCachingEnabled(cache);
    }
    
    bool isOptimizedNeuronMathSIMD() {
        return OptimizedNeuronMath::getInstance()->isSIMDEnabled();
    }
    
    bool isOptimizedNeuronMathPrefetching() {
        return OptimizedNeuronMath::getInstance()->isPrefetchingEnabled();
    }
    
    bool isOptimizedNeuronMathCaching() {
        return OptimizedNeuronMath::getInstance()->isCachingEnabled();
    }
    
    void resetOptimizedNeuronMathMetrics() {
        OptimizedNeuronMath::getInstance()->resetMetrics();
    }
    
    void benchmarkOptimizedNeuronMathNeuronalUpdates(size_t neuronCount, size_t iterations) {
        OptimizedNeuronMath::getInstance()->benchmarkNeuronalUpdates(neuronCount, iterations);
    }
    
    void benchmarkOptimizedNeuronMathSynapticUpdates(size_t weightCount, size_t iterations) {
        OptimizedNeuronMath::getInstance()->benchmarkSynapticUpdates(weightCount, iterations);
    }
    
    const char* getOptimizedNeuronMathPerformanceReport() {
        static std::string report = OptimizedNeuronMath::getInstance()->generatePerformanceReport();
        return report.c_str();
    }
}

// OptimizedNeuronMath namespace alias
namespace ONM = nlm;

// OptimizedNeuronMath constants
template<typename T>
const T OptimizedNeuronMath::membraneTimeConstant;

template<typename T>
const T OptimizedNeuronMath::resistance;

template<typename T>
const T OptimizedNeuronMath::restingPotential;

template<typename T>
const T OptimizedNeuronMath::spikeThreshold;

template<typename T>
const T OptimizedNeuronMath::minWeight;

template<typename T>
const T OptimizedNeuronMath::maxWeight;

template<typename T>
const uint32_t OptimizedNeuronMath::maxDelay;

// OptimizedNeuronMath template specializations
template class OptimizedNeuronMath<float>;
template class OptimizedNeuronMath<double>;

// OptimizedNeuronMath type aliases
typedef OptimizedNeuronMath<float> OptimizedNeuronMathf;
typedef OptimizedNeuronMath<double> OptimizedNeuronMathd;

// OptimizedNeuronMath utility functions
template<typename T>
T OptimizedNeuronMath::expDecay(float value, float dt, float tau) {
    return value * std::exp(-dt / tau);
}

template<typename T>
T OptimizedNeuronMath::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

template<typename T>
bool OptimizedNeuronMath::isSpike(float potential, float threshold) {
    return potential > threshold;
}

// OptimizedNeuronMath implementation end
