// Test utilities and debugging tools
// Provides various utilities for test development and analysis
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <random>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <atomic>
#include <filesystem>

namespace nlm {
namespace test {

// Random test data generator
class TestDataGenerator {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> realDist;
    std::uniform_int_distribution<int> intDist;
    
public:
    TestDataGenerator(uint32_t seed = std::random_device{}()) : rng(seed) {}
    
    // Generate random numbers
    double randomDouble(double min = 0.0, double max = 1.0) {
        return realDist(rng) * (max - min) + min;
    }
    
    int randomInt(int min = 0, int max = 100) {
        return intDist(rng) % (max - min + 1) + min;
    }
    
    // Generate test data arrays
    std::vector<double> randomVector(size_t size, double min = 0.0, double max = 1.0) {
        std::vector<double> data(size);
        std::generate(data.begin(), data.end(), [this, min, max]() { 
            return randomDouble(min, max); 
        });
        return data;
    }
    
    std::vector<int> randomIntVector(size_t size, int min = 0, int max = 100) {
        std::vector<int> data(size);
        std::generate(data.begin(), data.end(), [this, min, max]() { 
            return randomInt(min, max); 
        });
        return data;
    }
    
    // Generate realistic neural network test data
    std::vector<double> generateNeuralActivity(size_t neuronCount, double baseActivity = 0.1, double noise = 0.05) {
        std::vector<double> activity(neuronCount);
        for (size_t i = 0; i < neuronCount; ++i) {
            double base = baseActivity * (1.0 + (i % 3) * 0.2); // Some variation
            activity[i] = base + (randomDouble(-noise, noise));
            activity[i] = std::max(0.0, std::min(1.0, activity[i])); // Clamp to [0, 1]
        }
        return activity;
    }
    
    std::vector<std::vector<double>> generateConnectivityMatrix(size_t rows, size_t cols, double sparsity = 0.3) {
        std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols, 0.0));
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (randomDouble() < sparsity) {
                    matrix[i][j] = randomDouble(0.0, 1.0);
                }
            }
        }
        
        return matrix;
    }
};

// Comparison utilities for test validation
class ComparisonUtils {
public:
    struct ComparisonResult {
        bool passed;
        double maxDifference;
        double relativeError;
        std::string message;
    };
    
    // Floating point comparison with tolerance
    static ComparisonResult compareValues(double expected, double actual, 
                                         double absoluteTolerance = 1e-6,
                                         double relativeTolerance = 1e-6,
                                         const std::string& testName = "") {
        ComparisonResult result;
        result.maxDifference = std::abs(expected - actual);
        result.relativeError = (std::abs(expected) > 1e-12) ? 
            result.maxDifference / std::abs(expected) : result.maxDifference;
        
        bool absPass = result.maxDifference <= absoluteTolerance;
        bool relPass = result.relativeError <= relativeTolerance;
        
        result.passed = absPass && relPass;
        
        std::stringstream ss;
        if (!result.passed) {
            ss << "Test '" << testName << "' failed: expected " << expected 
               << ", got " << actual << ", diff=" << result.maxDifference
               << ", rel_err=" << result.relativeError;
        }
        result.message = ss.str();
        
        return result;
    }
    
    // Vector comparison
    static ComparisonResult compareVectors(const std::vector<double>& expected,
                                           const std::vector<double>& actual,
                                           double tolerance = 1e-6,
                                           const std::string& testName = "") {
        ComparisonResult result;
        
        if (expected.size() != actual.size()) {
            result.passed = false;
            result.message = "Vector size mismatch: " + std::to_string(expected.size()) + 
                           " vs " + std::to_string(actual.size());
            return result;
        }
        
        double maxDiff = 0.0;
        for (size_t i = 0; i < expected.size(); ++i) {
            double diff = std::abs(expected[i] - actual[i]);
            maxDiff = std::max(maxDiff, diff);
        }
        
        result.maxDifference = maxDiff;
        result.relativeError = (expected.size() > 0) ? 
            maxDiff / expected.size() : 0.0;
        
        result.passed = maxDiff <= tolerance;
        
        std::stringstream ss;
        if (!result.passed) {
            ss << "Test '" << testName << "' failed: max difference " << maxDiff 
               << " exceeds tolerance " << tolerance;
        }
        result.message = ss.str();
        
        return result;
    }
    
    // Matrix comparison
    static ComparisonResult compareMatrices(const std::vector<std::vector<double>>& expected,
                                            const std::vector<std::vector<double>>& actual,
                                            double tolerance = 1e-6,
                                            const std::string& testName = "") {
        ComparisonResult result;
        
        if (expected.size() != actual.size()) {
            result.passed = false;
            result.message = "Matrix row count mismatch: " + std::to_string(expected.size()) + 
                           " vs " + std::to_string(actual.size());
            return result;
        }
        
        for (size_t i = 0; i < expected.size(); ++i) {
            if (expected[i].size() != actual[i].size()) {
                result.passed = false;
                result.message = "Matrix column count mismatch at row " + 
                               std::to_string(i) + ": " + 
                               std::to_string(expected[i].size()) + " vs " + 
                               std::to_string(actual[i].size());
                return result;
            }
        }
        
        double maxDiff = 0.0;
        for (size_t i = 0; i < expected.size(); ++i) {
            for (size_t j = 0; j < expected[i].size(); ++j) {
                double diff = std::abs(expected[i][j] - actual[i][j]);
                maxDiff = std::max(maxDiff, diff);
            }
        }
        
        result.maxDifference = maxDiff;
        result.relativeError = (expected.size() * expected[0].size() > 0) ? 
            maxDiff / (expected.size() * expected[0].size()) : 0.0;
        
        result.passed = maxDiff <= tolerance;
        
        std::stringstream ss;
        if (!result.passed) {
            ss << "Test '" << testName << "' failed: max difference " << maxDiff 
               << " exceeds tolerance " << tolerance;
        }
        result.message = ss.str();
        
        return result;
    }
};

// Test data persistence
class TestDataPersistence {
public:
    static bool saveVector(const std::string& filename, const std::vector<double>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return false;
        
        size_t size = data.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(data.data()), size * sizeof(double));
        return file.good();
    }
    
    static bool loadVector(const std::string& filename, std::vector<double>& data) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;
        
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size * sizeof(double));
        return file.good();
    }
    
    static bool saveTextResults(const std::string& filename, 
                               const std::vector<TestResult>& results) {
        std::ofstream file(filename);
        if (!file) return false;
        
        for (const auto& result : results) {
            file << "Test: " << result.name << "\n"
                 << "Status: " << static_cast<int>(result.status) << "\n"
                 << "Category: " << static_cast<int>(result.category) << "\n"
                 << "Execution Time: " << result.executionTime.count() << "ns\n"
                 << "Message: " << result.message << "\n"
                 << "---\n";
        }
        
        return file.good();
    }
    
    static bool saveCSVResults(const std::string& filename,
                              const std::vector<TestResult>& results) {
        std::ofstream file(filename);
        if (!file) return false;
        
        file << "TestName,Status,Category,ExecutionTime(ns),SetupTime(ns),TeardownTime(ns),MemoryDelta,Message\n";
        
        for (const auto& result : results) {
            file << "\"" << result.name << "\"," 
                 << static_cast<int>(result.status) << ","
                 << static_cast<int>(result.category) << ","
                 << result.executionTime.count() << ","
                 << result.setupTime.count() << ","
                 << result.teardownTime.count() << ","
                 << result.memoryDelta << ","
                 << "\"" << result.message << "\"" << "\n";
        }
        
        return file.good();
    }
};

// Test timing utilities
class TestTimer {
public:
    struct BenchmarkResult {
        std::string name;
        std::chrono::nanoseconds duration;
        std::chrono::nanoseconds setupTime;
        std::chrono::nanoseconds teardownTime;
        size_t iterations;
        double operationsPerSecond;
    };
    
    static BenchmarkResult benchmark(std::function<void()> func, 
                                     size_t iterations = 1000,
                                     const std::string& benchmarkName = "Benchmark") {
        BenchmarkResult result;
        result.name = benchmarkName;
        result.iterations = iterations;
        
        // Warm-up
        for (size_t i = 0; i < 100; ++i) {
            func();
        }
        
        // Actual benchmarking
        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::system_clock::now();
        
        result.duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        result.operationsPerSecond = (result.duration.count() > 0) ? 
            (iterations * 1.0e9) / result.duration.count() : 0.0;
        
        return result;
    }
    
    static void printBenchmarkResult(const BenchmarkResult& result) {
        std::cout << "Benchmark: " << result.name << std::endl;
        std::cout << "  Iterations: " << result.iterations << std::endl;
        std::cout << "  Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(result.duration).count() << "ms" << std::endl;
        std::cout << "  Ops/sec: " << std::fixed << std::setprecision(2) << result.operationsPerSecond << std::endl;
        std::cout << "  Per iteration: " << std::chrono::duration_cast<std::chrono::microseconds>(result.duration).count() / result.iterations << "μs" << std::endl;
    }
};

// Resource monitoring utilities
class ResourceMonitor {
public:
    struct ResourceUsage {
        std::chrono::nanoseconds timestamp;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        std::thread::id threadId;
        double cpuUsage;
    };
    
    static std::vector<ResourceUsage> monitor(std::function<void()> task, 
                                              std::chrono::nanoseconds duration,
                                              size_t sampleInterval = 10000000) { // 10ms in nanoseconds
        std::vector<ResourceUsage> usage;
        std::atomic<bool> running{true};
        std::thread monitorThread([&]() {
            while (running) {
                ResourceUsage current;
                current.timestamp = std::chrono::system_clock::now();
                current.threadId = std::this_thread::get_id();
                current.memoryUsage = getMemoryUsage(); // Implementation needed
                current.peakMemoryUsage = getPeakMemoryUsage(); // Implementation needed
                current.cpuUsage = getCPUUsage(); // Implementation needed
                
                usage.push_back(current);
                std::this_thread::sleep_for(std::chrono::nanoseconds(sampleInterval));
            }
        });
        
        task();
        running = false;
        
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
        
        return usage;
    }
    
private:
    static size_t getMemoryUsage() { return 0; } // TODO: Implement platform-specific memory measurement
    static size_t getPeakMemoryUsage() { return 0; } // TODO: Implement platform-specific peak memory measurement
    static double getCPUUsage() { return 0.0; } // TODO: Implement platform-specific CPU usage measurement
};

}}} // namespace nlm::test
