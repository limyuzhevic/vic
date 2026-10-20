// SIMD Vectorization Benchmark for NLM Neural Simulation
// Demonstrates performance improvements from SIMD optimizations

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/VectorizedLIF.hpp"
#include "performance/VectorizedSTDP.hpp"
#include "performance/VectorizedStats.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace nlm;

// Benchmark result structure
struct SIMDBenchmarkResult {
    std::string operation;
    size_t dataSize;
    double scalarTime;
    double vectorTime;
    double speedup;
    double efficiency;
    std::string simdType;
};

// Memory pool for efficient allocation
class BenchmarkMemoryPool {
public:
    static std::vector<float> allocateVector(size_t size) {
        return std::vector<float>(size);
    }
    
    static std::vector<uint32_t> allocateUInt(size_t size) {
        return std::vector<uint32_t>(size);
    }
    
    static std::vector<uint8_t> allocateByte(size_t size) {
        return std::vector<uint8_t>(size);
    }
    
    static void fillRandom(std::vector<float>& vec, float min = -1.0f, float max = 1.0f) {
        RandomGenerator rng;
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i] = rng.uniformReal(min, max);
        }
    }
    
    static void fillSequential(std::vector<float>& vec) {
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i] = static_cast<float>(i % 100) * 0.01f;
        }
    }
};

// Neuron operation benchmarks
SIMDBenchmarkResult benchmarkNeuronStepLIF(size_t neuronCount, size_t iterations) {
    std::cout << "  Benchmarking Neuron::stepLIF with " << neuronCount << " neurons, " << iterations << " iterations..." << std::flush;
    
    // Initialize benchmark data
    auto membranePotentials = BenchmarkMemoryPool::allocateVector(neuronCount);
    auto synapticInputs = BenchmarkMemoryPool::allocateVector(neuronCount);
    auto refractoryRemaining = BenchmarkMemoryPool::allocateUInt(neuronCount);
    auto fired = BenchmarkMemoryPool::allocateByte(neuronCount);
    
    BenchmarkMemoryPool::fillRandom(membranePotentials, -70.0f, -55.0f);
    BenchmarkMemoryPool::fillRandom(synapticInputs, -10.0f, 10.0f);
    std::fill(refractoryRemaining.begin(), refractoryRemaining.end(), 0);
    std::fill(fired.begin(), fired.end(), 0);
    
    // Benchmark scalar version
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < neuronCount; ++i) {
            // Simulate scalar LIF update
            float V = membranePotentials[i];
            float V_rest = -70.0f;
            float V_reset = -70.0f;
            float threshold = -55.0f;
            float tau = 20.0f;
            float C = 1.0f;
            float dt = 1.0f;
            
            float leak = (V_rest - V) / tau;
            float input = synapticInputs[i] / C;
            V += dt * 1000.0f * (leak + input);
            
            // Spike detection
            if (V >= threshold) {
                V = V_reset;
                refractoryRemaining[i] = 10;
                fired[i] = 1;
            } else {
                fired[i] = 0;
            }
            
            membranePotentials[i] = V;
            refractoryRemaining[i] = std::max(0u, refractoryRemaining[i] - 1);
            
            // Apply refractory
            if (refractoryRemaining[i] > 0) {
                membranePotentials[i] = V_rest;
            }
        }
    }
    auto scalarEnd = std::chrono::high_resolution_clock::now();
    double scalarTime = std::chrono::duration<double>(scalarEnd - start).count();
    
    // Benchmark vectorized version
    VectorizedLIF lif;
    lif.V_rest = -70.0f;
    lif.V_reset = -70.0f;
    lif.V_threshold = -55.0f;
    lif.tau = 20.0f;
    lif.C = 1.0f;
    lif.dt = 1.0f;
    lif.refractoryDecay = 10.0f;
    
    start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        VectorizedLIF::updateLIF(lif, membranePotentials, synapticInputs,
                                refractoryRemaining, fired, neuronCount);
    }
    auto vectorEnd = std::chrono::high_resolution_clock::now();
    double vectorTime = std::chrono::duration<double>(vectorEnd - start).count();
    
    // Calculate results
    SIMDBenchmarkResult result;
    result.operation = "Neuron::stepLIF";
    result.dataSize = neuronCount * iterations;
    result.scalarTime = scalarTime;
    result.vectorTime = vectorTime;
    result.speedup = scalarTime / (vectorTime > 0 ? vectorTime : 1e-9);
    result.efficiency = result.speedup / SIMDUtils::SIMD_WIDTH;
    result.simdType = SIMDUtils::getSIMDType();
    
    std::cout << " Done!" << std::endl;
    
    return result;
}

// Statistics operation benchmarks
SIMDBenchmarkResult benchmarkStatistics(size_t dataSize, size_t iterations) {
    std::cout << "  Benchmarking statistics operations with " << dataSize << " data points, " << iterations << " iterations..." << std::flush;
    
    auto data = BenchmarkMemoryPool::allocateVector(dataSize);
    BenchmarkMemoryPool::fillSequential(data);
    
    // Benchmark scalar statistics
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        float sum = 0.0f;
        float min = std::numeric_limits<float>::max();
        float max = -std::numeric_limits<float>::max();
        
        for (size_t i = 0; i < dataSize; ++i) {
            sum += data[i];
            min = std::min(min, data[i]);
            max = std::max(max, data[i]);
        }
        
        float mean = sum / static_cast<float>(dataSize);
        float variance = 0.0f;
        for (size_t i = 0; i < dataSize; ++i) {
            float diff = data[i] - mean;
            variance += diff * diff;
        }
        variance /= static_cast<float>(dataSize - 1);
    }
    auto scalarEnd = std::chrono::high_resolution_clock::now();
    double scalarTime = std::chrono::duration<double>(scalarEnd - start).count();
    
    // Benchmark vectorized statistics
    start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        float sum = VectorizedStats::sum(data, dataSize);
        float mean = VectorizedStats::mean(data, dataSize);
        float variance = VectorizedStats::variance(data, dataSize);
        float max = VectorizedStats::max(data, dataSize);
        float min = VectorizedStats::min(data, dataSize);
    }
    auto vectorEnd = std::chrono::high_resolution_clock::now();
    double vectorTime = std::chrono::duration<double>(vectorEnd - start).count();
    
    // Calculate results
    SIMDBenchmarkResult result;
    result.operation = "Statistics";
    result.dataSize = dataSize * iterations;
    result.scalarTime = scalarTime;
    result.vectorTime = vectorTime;
    result.speedup = scalarTime / (vectorTime > 0 ? vectorTime : 1e-9);
    result.efficiency = result.speedup / SIMDUtils::SIMD_WIDTH;
    result.simdType = SIMDUtils::getSIMDType();
    
    std::cout << " Done!" << std::endl;
    
    return result;
}

// Plasticity operation benchmarks
SIMDBenchmarkResult benchmarkPlasticity(size_t synapseCount, size_t iterations) {
    std::cout << "  Benchmarking plasticity operations with " << synapseCount << " synapses, " << iterations << " iterations..." << std::flush;
    
    auto weights = BenchmarkMemoryPool::allocateVector(synapseCount);
    auto preSpikes = BenchmarkMemoryPool::allocateVector(synapseCount);
    auto postSpikes = BenchmarkMemoryPool::allocateVector(synapseCount);
    
    BenchmarkMemoryPool::fillRandom(weights, -1.0f, 1.0f);
    BenchmarkMemoryPool::fillRandom(preSpikes, -1.0f, 1.0f);
    BenchmarkMemoryPool::fillRandom(postSpikes, -1.0f, 1.0f);
    
    // Benchmark scalar STDP
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < synapseCount; ++i) {
            // Simulate scalar STDP update
            float dt = postSpikes[i] - preSpikes[i];
            
            float delta;
            if (dt > 0) {
                delta = 0.01f * std::exp(-dt / 20.0f);  // LTP
            } else if (dt < 0) {
                delta = -0.012f * std::exp(dt / 20.0f);  // LTD
            } else {
                delta = 0.0f;
            }
            
            weights[i] = std::clamp(weights[i] + delta, -1.0f, 1.0f);
        }
    }
    auto scalarEnd = std::chrono::high_resolution_clock::now();
    double scalarTime = std::chrono::duration<double>(scalarEnd - start).count();
    
    // Benchmark vectorized STDP
    VectorizedSTDP::EnhancedVectorizedSTDP stdp(VectorizedSTDP::RuleType::STDP);
    start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        VectorizedSTDP::updateWeights(stdp, weights, preSpikes, postSpikes, synapseCount, synapseCount);
    }
    auto vectorEnd = std::chrono::high_resolution_clock::now();
    double vectorTime = std::chrono::duration<double>(vectorEnd - start).count();
    
    // Calculate results
    SIMDBenchmarkResult result;
    result.operation = "Plasticity (STDP)";
    result.dataSize = synapseCount * iterations;
    result.scalarTime = scalarTime;
    result.vectorTime = vectorTime;
    result.speedup = scalarTime / (vectorTime > 0 ? vectorTime : 1e-9);
    result.efficiency = result.speedup / SIMDUtils::SIMD_WIDTH;
    result.simdType = SIMDUtils::getSIMDType();
    
    std::cout << " Done!" << std::endl;
    
    return result;
}

// Print benchmark results
void printBenchmarkResults(const std::vector<SIMDBenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                    NLM SIMD Vectorization Benchmark Results                                ║" << std::endl;
    std::cout << "╠════════════════════╦═════════════════════════════╦════════════╦════════════╦════════════╦════════════════════════╣" << std::endl;
    std::cout << "║     Operation     ║       SIMD Type         ║ Speedup    ║   Scalar   ║  Vector    ║     Efficiency     ║" << std::endl;
    std::cout << "╠════════════════════╬═════════════════════════════╬════════════╬════════════╬════════════╬════════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(18) << r.operation
                  << " ║ " << std::setw(27) << r.simdType
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.speedup
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(6) << r.scalarTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(6) << r.vectorTime
                  << " ║ " << std::setw(18) << std::fixed << std::setprecision(2) << r.efficiency
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════════════╩═════════════════════════════╩════════════╩════════════╩════════════╩════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Global profiling benchmark
void benchmarkGlobalProfiling() {
    std::cout << "Running global profiling benchmark..." << std::endl;
    
    SIMDProfiler::getInstance().reset();
    
    // Run various operations to generate profiling data
    size_t testSize = 1000;
    auto data = BenchmarkMemoryPool::allocateVector(testSize);
    BenchmarkMemoryPool::fillSequential(data);
    
    // Profile statistics operations
    for (size_t i = 0; i < 10; ++i) {
        SIMDProfiler::getInstance().startProfiling();
        float sum = VectorizedStats::sum(data, testSize);
        float mean = VectorizedStats::mean(data, testSize);
        SIMDProfiler::getInstance().stopProfiling();
    }
    
    // Profile vectorized LIF operations
    VectorizedLIF lif;
    auto V = BenchmarkMemoryPool::allocateVector(testSize);
    auto I = BenchmarkMemoryPool::allocateVector(testSize);
    auto R = BenchmarkMemoryPool::allocateUInt(testSize);
    auto F = BenchmarkMemoryPool::allocateByte(testSize);
    
    BenchmarkMemoryPool::fillRandom(V, -70.0f, -55.0f);
    BenchmarkMemoryPool::fillRandom(I, -10.0f, 10.0f);
    std::fill(R.begin(), R.end(), 0);
    
    for (size_t i = 0; i < 5; ++i) {
        SIMDProfiler::getInstance().startProfiling();
        VectorizedLIF::updateLIF(lif, V, I, R, F, testSize);
        SIMDProfiler::getInstance().stopProfiling();
    }
    
    // Print profiling results
    SIMDProfiler::getInstance().printResults();
}

int main() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                                                ║
    ║            NLM SIMD Vectorization Performance Benchmark                           ║
    ║                                                                                ║
    ║              Demonstrating Neural Simulation Optimizations                       ║
    ║                                                                                ║
    ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "Initializing SIMD vectorization benchmark..." << std::endl;
    std::cout << "SIMD Type: " << SIMDUtils::getSIMDType() << std::endl;
    std::cout << "SIMD Width: " << SIMDUtils::SIMD_WIDTH << std::endl;
    std::cout << "Memory Alignment: " << SIMDUtils::alignment() << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::vector<SIMDBenchmarkResult> results;
    
    // Run benchmarks with different data sizes
    std::vector<std::pair<size_t, size_t>> benchmarkConfigs = {
        {1000, 100},    // Small: 1000 neurons, 100 iterations
        {5000, 50},     // Medium: 5000 neurons, 50 iterations
        {10000, 20},    // Large: 10000 neurons, 20 iterations
    };
    
    std::cout << "Running SIMD vectorization benchmarks..." << std::endl;
    std::cout << std::endl;
    
    for (const auto& config : benchmarkConfigs) {
        size_t size = config.first;
        size_t iterations = config.second;
        
        // Neuron operations
        auto neuronResult = benchmarkNeuronStepLIF(size, iterations);
        results.push_back(neuronResult);
        
        // Statistics operations
        auto statsResult = benchmarkStatistics(size, iterations);
        results.push_back(statsResult);
        
        // Plasticity operations
        auto plasticityResult = benchmarkPlasticity(size, iterations);
        results.push_back(plasticityResult);
        
        std::cout << std::endl;
    }
    
    // Print summary results
    printBenchmarkResults(results);
    
    // Run global profiling benchmark
    benchmarkGlobalProfiling();
    
    std::cout << "Benchmark complete!" << std::endl;
    
    // Calculate and print overall summary
    double totalSpeedup = 0.0;
    double totalEfficiency = 0.0;
    size_t count = 0;
    
    for (const auto& r : results) {
        totalSpeedup += r.speedup;
        totalEfficiency += r.efficiency;
        count++;
    }
    
    if (count > 0) {
        totalSpeedup /= count;
        totalEfficiency /= count;
        
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                 Overall Benchmark Summary                          ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Average Speedup: " << std::setw(34) << std::fixed << std::setprecision(2) << totalSpeedup << " ║" << std::endl;
        std::cout << "║ Average Efficiency: " << std::setw(32) << std::fixed << std::setprecision(2) << totalEfficiency << " ║" << std::endl;
        std::cout << "║ SIMD Type Used: " << std::setw(35) << SIMDUtils::getSIMDType() << " ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}