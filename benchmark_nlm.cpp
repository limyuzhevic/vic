// Performance benchmarking tool for NLM
// Measures memory usage, CPU performance, and simulation speed

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <thread>

using namespace nlm;

class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string testName;
        double timeSeconds;
        size_t operations;
        double operationsPerSecond;
        size_t memoryBytes;
    };
    
    static void runBenchmarks() {
        std::cout << "Running NLM Performance Benchmarks..." << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error); // Only errors
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        std::vector<BenchmarkResult> results;
        
        // Benchmark 1: Config class performance
        results.push_back(benchmarkConfigPerformance());
        
        // Benchmark 2: Random class performance
        results.push_back(benchmarkRandomPerformance());
        
        // Benchmark 3: Brain initialization performance
        results.push_back(benchmarkBrainInitPerformance());
        
        // Benchmark 4: Simulation performance
        results.push_back(benchmarkSimulationPerformance());
        
        // Benchmark 5: Memory usage
        results.push_back(benchmarkMemoryUsage());
        
        // Print results
        printBenchmarkResults(results);
        
        // Save results to file
        saveBenchmarkResults(results, "benchmark_results.csv");
    }
    
private:
    static BenchmarkResult benchmarkConfigPerformance() {
        std::cout << "Benchmarking Config class performance..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t operations = 0;
        
        // Test setting many values
        Config config;
        for (int i = 0; i < 10000; ++i) {
            config.set(std::to_string(i), i);
            config.has(std::to_string(i));
            config.get<int64_t>(std::to_string(i));
            operations += 3;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.testName = "Config Class Performance";
        result.timeSeconds = duration.count() / 1000000.0;
        result.operations = operations;
        result.operationsPerSecond = operations / result.timeSeconds;
        result.memoryBytes = sizeof(Config);
        
        return result;
    }
    
    static BenchmarkResult benchmarkRandomPerformance() {
        std::cout << "Benchmarking Random class performance..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t operations = 0;
        
        RandomGenerator rng(42);
        
        // Test various random operations
        for (int i = 0; i < 100000; ++i) {
            uint32_t int1 = rng.uniformInt(1, 100);
            double real1 = rng.uniformReal(0.0, 1.0);
            bool bool1 = rng.bernoulli(0.5);
            double normal1 = rng.normal(0.0, 1.0);
            
            operations += 5;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.testName = "Random Class Performance";
        result.timeSeconds = duration.count() / 1000000.0;
        result.operations = operations;
        result.operationsPerSecond = operations / result.timeSeconds;
        result.memoryBytes = sizeof(RandomGenerator);
        
        return result;
    }
    
    static BenchmarkResult benchmarkBrainInitPerformance() {
        std::cout << "Benchmarking Brain initialization performance..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create config
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 1000);
        config->set("region_count", 5);
        config->set("connection_probability", 0.1);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        bool initSuccess = brain->initialize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.testName = "Brain Initialization Performance";
        result.timeSeconds = duration.count() / 1000000.0;
        result.operations = initSuccess ? 1 : 0;
        result.operationsPerSecond = result.operations / result.timeSeconds;
        result.memoryBytes = 0; // Complex, would need memory measurement
        
        return result;
    }
    
    static BenchmarkResult benchmarkSimulationPerformance() {
        std::cout << "Benchmarking simulation performance..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create config for smaller simulation (for speed)
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Warning: Brain initialization failed for simulation benchmark" << std::endl;
            BenchmarkResult result;
            result.testName = "Simulation Performance";
            result.timeSeconds = 0.0;
            result.operations = 0;
            result.operationsPerSecond = 0.0;
            result.memoryBytes = 0;
            return result;
        }
        
        // Run simulation
        size_t steps = 1000;
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.testName = "Simulation Performance";
        result.timeSeconds = duration.count() / 1000000.0;
        result.operations = steps;
        result.operationsPerSecond = steps / result.timeSeconds;
        result.memoryBytes = 0; // Complex, would need memory measurement
        
        return result;
    }
    
    static BenchmarkResult benchmarkMemoryUsage() {
        std::cout << "Benchmarking memory usage..." << std::endl;
        
        // This is a simplified memory benchmark
        // In a real implementation, we would use tools like valgrind or memory_profiler
        
        // Allocate various objects and measure memory
        std::vector<size_t> memorySamples;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create various objects
        std::vector<std::shared_ptr<Config>> configs;
        for (int i = 0; i < 100; ++i) {
            configs.push_back(std::make_shared<Config>());
        }
        
        std::vector<std::shared_ptr<RandomGenerator>> rngs;
        for (int i = 0; i < 100; ++i) {
            rngs.push_back(std::make_shared<RandomGenerator>(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        // Estimate memory usage (very rough approximation)
        size_t estimatedMemory = 
            configs.size() * sizeof(Config) +
            rngs.size() * sizeof(RandomGenerator) +
            configs.size() * 10000 * sizeof(int64_t) +  // Rough estimate for config entries
            rngs.size() * 5 * sizeof(double);  // Rough estimate for random operations
        
        BenchmarkResult result;
        result.testName = "Memory Usage";
        result.timeSeconds = duration.count() / 1000000.0;
        result.operations = estimatedMemory / 1024;  // Estimate in KB
        result.operationsPerSecond = result.operations / result.timeSeconds;
        result.memoryBytes = estimatedMemory;
        
        return result;
    }
    
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
        std::cout << "" << std::endl;
        std::cout << "Benchmark Results Summary:" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(30) << result.testName;
            std::cout << std::right << std::setw(12) << result.timeSeconds << " s";
            std::cout << std::right << std::setw(12) << result.operations << " ops";
            std::cout << std::right << std::setw(12) << result.operationsPerSecond << " ops/s";
            std::cout << std::right << std::setw(12) << result.memoryBytes / 1024 << " KB";
            std::cout << std::endl;
        }
        
        std::cout << "" << std::endl;
        std::cout << "Performance Summary:" << std::endl;
        std::cout << "- Config class: " << getFastestTest(results, "Config") << " fastest" << std::endl;
        std::cout << "- Random class: " << getFastestTest(results, "Random") << " fastest" << std::endl;
        std::cout << "- Brain initialization: " << getFastestTest(results, "Brain") << " fastest" << std::endl;
        std::cout << "- Simulation: " << getFastestTest(results, "Simulation") << " fastest" << std::endl;
        std::cout << "- Memory usage: " << getFastestTest(results, "Memory") << " most efficient" << std::endl;
    }
    
    static std::string getFastestTest(const std::vector<BenchmarkResult>& results, const std::string& type) {
        double minTime = std::numeric_limits<double>::max();
        std::string fastestTest;
        
        for (const auto& result : results) {
            if (result.testName.find(type) != std::string::npos && result.timeSeconds < minTime) {
                minTime = result.timeSeconds;
                fastestTest = result.testName;
            }
        }
        
        return fastestTest.empty() ? "N/A" : fastestTest;
    }
    
    static void saveBenchmarkResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not save benchmark results to " << filename << std::endl;
            return;
        }
        
        // Write CSV header
        file << "Test Name,Time (s),Operations,Operations per Second,Memory (KB)" << std::endl;
        
        // Write results
        for (const auto& result : results) {
            file << result.testName << ",";
            file << std::fixed << std::setprecision(6) << result.timeSeconds << ",";
            file << result.operations << ",";
            file << std::fixed << std::setprecision(2) << result.operationsPerSecond << ",";
            file << result.memoryBytes / 1024 << std::endl;
        }
        
        file.close();
        std::cout << "Benchmark results saved to " << filename << std::endl;
    }
};

int main() {
    try {
        PerformanceBenchmark::runBenchmarks();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Benchmark failed: " << e.what() << std::endl;
        return 1;
    }
}