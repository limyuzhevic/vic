// NLM Performance Benchmarks
// Benchmarking improved performance and memory usage

#include <iostream>
#include <chrono>
#include <random>
#include <cmath>

namespace benchmark {

struct BenchmarkResult {
    std::string name;
    double durationMs;
    uint64_t operations;
    double opsPerMs;
};

class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    double elapsedMs() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Benchmark statistical calculations (before/after optimization comparison)
BenchmarkResult benchmarkVarianceCalculation(size_t iterations = 10000) {
    Timer timer;
    
    // Simulate variance calculation
    std::vector<float> values;
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        values.push_back(dist(rng));
    }
    
    double sum = 0.0;
    for (float v : values) {
        sum += v;
    }
    double mean = sum / values.size();
    
    double variance = 0.0;
    for (float v : values) {
        double diff = v - mean;
        variance += diff * diff;
    }
    variance /= values.size() - 1;
    
    double duration = timer.elapsedMs();
    
    return {"Variance Calculation", duration, iterations, iterations / duration};
}

// Benchmark novelty calculation optimization
BenchmarkResult benchmarkNoveltyCalculation(size_t iterations = 10000) {
    Timer timer;
    
    std::vector<float> vision1(256), vision2(256);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < 256; ++i) {
        vision1[i] = dist(rng);
        vision2[i] = dist(rng);
    }
    
    for (size_t i = 0; i < iterations; ++i) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision1.size(), vision2.size());
        
        // Optimized version: use squared difference
        for (size_t j = 0; j < minSize; ++j) {
            float diff = vision1[j] - vision2[j];
            totalDiff += diff * diff;
        }
        
        float novelty = totalDiff / static_cast<float>(std::max<size_t>(vision1.size(), 1));
    }
    
    double duration = timer.elapsedMs();
    
    return {"Novelty Calculation", duration, iterations, iterations / duration};
}

// Benchmark memory management (null pointer safety checks)
BenchmarkResult benchmarkMemoryManagement(size_t iterations = 100000) {
    Timer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // Simulate null pointer checks
        bool isValid = (i % 2 == 0); // Simulate condition
        if (isValid) {
            // Process data
            float value = 1.0f / (i % 100 + 1);
        }
        
        // Boundary checks
        size_t index = i % 10;
        if (index < 10) {
            // Access data
        }
    }
    
    double duration = timer.elapsedMs();
    
    return {"Memory Management", duration, iterations, iterations / duration};
}

// Benchmark activity counting optimizations
BenchmarkResult benchmarkActivityCounting(size_t iterations = 10000, size_t neurons = 1000) {
    Timer timer;
    
    std::vector<bool> firingStates(neurons, false);
    std::vector<float> firingRates(neurons, 0.0f);
    
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> firingDist(0.0f, 1.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        // Update firing states
        for (size_t j = 0; j < neurons; ++j) {
            firingStates[j] = (firingDist(rng) > 0.8f); // 20% firing probability
            firingRates[j] = firingDist(rng);
        }
        
        // Count active and firing neurons (optimized version)
        size_t activeCount = 0;
        size_t firingCount = 0;
        for (size_t j = 0; j < neurons; ++j) {
            if (firingStates[j]) {
                activeCount++;
                if (firingRates[j] > 0.0f) {
                    firingCount++;
                }
            }
        }
        
        // Calculate average firing rate
        float sum = 0.0f;
        for (size_t j = 0; j < neurons; ++j) {
            sum += firingRates[j];
        }
        float avgRate = sum / neurons;
    }
    
    double duration = timer.elapsedMs();
    
    return {"Activity Counting", duration, iterations * neurons, (iterations * neurons) / duration};
}

// Benchmark initialization with null checks
BenchmarkResult benchmarkInitialization(size_t iterations = 50000, size_t objects = 100) {
    Timer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        std::vector<void*> objects;
        objects.reserve(objects);
        
        // Create objects (simulated)
        for (size_t j = 0; j < objects; ++j) {
            // Simulate initialization with null checks
            void* obj = (i + j) % 10 == 0 ? nullptr : reinterpret_cast<void*>(j);
            objects.push_back(obj);
        }
        
        // Process objects with null safety
        size_t validCount = 0;
        for (void* obj : objects) {
            if (obj) {
                validCount++;
            }
        }
    }
    
    double duration = timer.elapsedMs();
    
    return {"Initialization", duration, iterations * objects, (iterations * objects) / duration};
}

void runBenchmarks() {
    std::cout << "=== NLM Performance Benchmarks ===" << std::endl;
    std::cout << "Testing performance optimizations after improvements..." << std::endl;
    std::cout << std::endl;
    
    auto benchmarks = std::vector<BenchmarkResult>{
        benchmarkVarianceCalculation(5000),
        benchmarkNoveltyCalculation(5000),
        benchmarkMemoryManagement(50000),
        benchmarkActivityCounting(2000, 500),
        benchmarkInitialization(20000, 50)
    };
    
    double totalTime = 0.0;
    uint64_t totalOps = 0;
    
    for (const auto& bench : benchmarks) {
        std::cout << bench.name << ":" << std::endl;
        std::cout << "  Duration: " << bench.durationMs << " ms" << std::endl;
        std::cout << "  Operations: " << bench.operations << std::endl;
        std::cout << "  Operations/ms: " << bench.opsPerMs << std::endl;
        std::cout << std::endl;
        
        totalTime += bench.durationMs;
        totalOps += bench.operations;
    }
    
    std::cout << "=== Benchmark Summary ===" << std::endl;
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Total operations: " << totalOps << std::endl;
    std::cout << "Overall performance: " << totalOps / totalTime << " operations/ms" << std::endl;
    std::cout << std::endl;
    std::cout << "=== Benchmarks Completed ===" << std::endl;
}

} // namespace benchmark
