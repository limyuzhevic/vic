// Test Infrastructure Utilities
#pragma once

#include <chrono>
#include <functional>
#include <random>
#include <cassert>
#include <iostream>
#include <iomanip>

namespace nlm::test {

// Performance measurement framework
struct PerformanceTimer {
    std::chrono::high_resolution_clock::time_point startTime;
    
    PerformanceTimer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    double elapsedMs() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime);
        return duration.count() / 1000.0;
    }
    
    double elapsedSeconds() const {
        return elapsedMs() / 1000.0;
    }
};

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    double durationMs;
    std::string message;
};

// Benchmark utility
class Benchmark {
public:
    template<typename Func>
    static double measure(Func&& func) {
        PerformanceTimer timer;
        func();
        return timer.elapsedMs();
    }
    
    template<typename Func>
    static void runBenchmark(const std::string& name, Func&& func, int iterations = 100) {
        double totalTime = 0.0;
        for (int i = 0; i < iterations; ++i) {
            totalTime += measure(func);
        }
        double avgTime = totalTime / iterations;
        
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Benchmark: " << name << " (avg " << iterations << " runs): " << avgTime << " ms" << std::endl;
    }
};

// Test data generators
class TestDataGenerator {
public:
    // Generate random neuron IDs
    static std::vector<int64_t> generateNeuronIds(size_t count, int64_t startId = 1000) {
        std::vector<int64_t> ids;
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int64_t> dist(startId, startId + 10000);
        
        for (size_t i = 0; i < count; ++i) {
            ids.push_back(dist(gen));
        }
        return ids;
    }
    
    // Generate random synaptic weights
    static std::vector<double> generateWeights(size_t count, double min = 0.0, double max = 1.0) {
        std::vector<double> weights;
        std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<double> dist(min, max);
        
        for (size_t i = 0; i < count; ++i) {
            weights.push_back(dist(gen));
        }
        return weights;
    }
    
    // Generate test configuration values
    static std::vector<std::string> generateConfigKeys(size_t count, const std::vector<std::string>& prefixes = {"param", "setting", "option"}) {
        std::vector<std::string> keys;
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> prefixDist(0, prefixes.size() - 1);
        std::uniform_int_distribution<int> suffixDist(1, 1000);
        
        for (size_t i = 0; i < count; ++i) {
            size_t prefixIdx = prefixDist(gen);
            int suffix = suffixDist(gen);
            keys.push_back(prefixes[prefixIdx] + "_" + std::to_string(suffix));
        }
        return keys;
    }
};

// Test utilities
namespace test_utils {
    // Safe pointer checking
    template<typename T>
    bool isNull(const std::unique_ptr<T>& ptr) {
        return ptr == nullptr;
    }
    
    template<typename T>
    bool isNull(const std::shared_ptr<T>& ptr) {
        return ptr == nullptr;
    }
    
    // Container size checking
    template<typename Container>
    bool hasSize(const Container& container, size_t expectedSize) {
        return container.size() == expectedSize;
    }
    
    // Value range checking
    template<typename T>
    bool isInRange(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    // Configuration key validation
    std::vector<std::string> findDuplicateKeys(const std::vector<std::string>& keys) {
        std::vector<std::string> duplicates;
        std::unordered_map<std::string, int> keyCount;
        
        for (const auto& key : keys) {
            keyCount[key]++;
            if (keyCount[key] == 2) {
                duplicates.push_back(key);
            }
        }
        
        return duplicates;
    }
    
    // Safe value extraction with error handling
    template<typename T, typename Func>
    std::optional<T> safeExtract(const Func& extractor, T defaultValue = T{}) {
        try {
            T result = extractor();
            return result;
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
}

// Test result collector
class TestCollector {
private:
    std::vector<TestResult> results;
    
public:
    void addTest(const TestResult& result) {
        results.push_back(result);
    }
    
    void printSummary() const {
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
                std::cout << "FAILED: " << result.name << " - " << result.message << std::endl;
            }
        }
        
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;
    }
    
    double getAverageDuration() const {
        if (results.empty()) return 0.0;
        
        double total = 0.0;
        for (const auto& result : results) {
            total += result.durationMs;
        }
        return total / results.size();
    }
};

} // namespace nlm::test
