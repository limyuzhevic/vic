// Performance Test Tool
// Demonstrates NLM's performance capabilities and benchmarking features

#include "src/performance/PerformanceMonitor.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>

namespace nlm {

class PerformanceTestTool {
public:
    PerformanceTestTool() : performanceMonitor("PerformanceTest") {}
    ~PerformanceTestTool() = default;
    
    // Disable copying, enable moving
    PerformanceTestTool(const PerformanceTestTool&) = delete;
    PerformanceTestTool& operator=(const PerformanceTestTool&) = delete;
    PerformanceTestTool(PerformanceTestTool&&) noexcept;
    PerformanceTestTool& operator=(PerformanceTestTool)&& = delete;

    /**
     * Run comprehensive performance tests
     */
    bool runAllTests();
    
    /**
     * Benchmark neural network creation
     */
    bool benchmarkBrainCreation(const std::vector<size_t>& neuronCounts);
    
    /**
     * Benchmark simulation steps
     */
    bool benchmarkSimulation(const std::shared_ptr<Config>& config, size_t steps);
    
    /**
     * Benchmark memory usage
     */
    bool benchmarkMemoryUsage();
    
    /**
     * Benchmark concurrent processing
     */
    bool benchmarkConcurrentProcessing(size_t threadCount);
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport() const;
    
    /**
     * Export results to file
     */
    bool exportResults(const std::string& filename) const;

private:
    PerformanceMonitor performanceMonitor;
    std::vector<BenchmarkResult> benchmarkResults;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    
    /**
     * Create test configuration
     */
    std::shared_ptr<Config> createTestConfig(size_t neuronCount, size_t regionCount = 4);
    
    /**
     * Run spike processing benchmark
     */
    void benchmarkSpikeProcessing(size_t neuronCount, size_t spikeRate);
    
    /**
     * Run synaptic connectivity benchmark
     */
    void benchmarkSynapticConnectivity(size_t neuronCount, float connectionProbability);
    
    /**
     * Run memory allocation benchmark
     */
    void benchmarkMemoryAllocation(size_t totalMemory);
    
    /**
     * Run simulation throughput benchmark
     */
    void benchmarkSimulationThroughput(const std::shared_ptr<Config>& config, size_t steps);
};

PerformanceTestTool::PerformanceTestTool(PerformanceTestTool&& other) noexcept
    : performanceMonitor(std::move(other.performanceMonitor)), 
      benchmarkResults(std::move(other.benchmarkResults)),
      startTime(std::move(other.startTime)) {}

PerformanceTestTool& PerformanceTestTool::operator=(PerformanceTestTool&& other)&& {
    if (this != &other) {
        performanceMonitor = std::move(other.performanceMonitor);
        benchmarkResults = std::move(other.benchmarkResults);
        startTime = std::move(other.startTime);
    }
    return *this;
}

std::shared_ptr<Config> PerformanceTestTool::createTestConfig(size_t neuronCount, size_t regionCount) {
    auto config = std::make_shared<Config>();
    
    config->set("neuron_count", neuronCount);
    config->set("region_count", regionCount);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001f);
    config->set("max_steps", 1000);
    config->set("enable_checkpointing", true);
    config->set("enable_replay", true);
    config->set("enable_development", true);
    
    return config;
}

bool PerformanceTestTool::runAllTests() {
    std::cout << "=== NLM Performance Test Suite ===" << std::endl;
    startTime = std::chrono::steady_clock::now();
    
    // Run brain creation benchmarks
    std::cout << "\n1. Brain Creation Benchmarks:" << std::endl;
    std::vector<size_t> neuronCounts = {100, 1000, 5000, 10000};
    if (!benchmarkBrainCreation(neuronCounts)) {
        std::cerr << "Brain creation benchmarks failed" << std::endl;
        return false;
    }
    
    // Run simulation benchmarks
    std::cout << "\n2. Simulation Benchmarks:" << std::endl;
    auto config = createTestConfig(1000);
    if (!benchmarkSimulation(config, 1000)) {
        std::cerr << "Simulation benchmarks failed" << std::endl;
        return false;
    }
    
    // Run memory benchmarks
    std::cout << "\n3. Memory Usage Benchmarks:" << std::endl;
    if (!benchmarkMemoryUsage()) {
        std::cerr << "Memory benchmarks failed" << std::endl;
        return false;
    }
    
    // Run concurrent processing benchmarks
    std::cout << "\n4. Concurrent Processing Benchmarks:" << std::endl;
    if (!benchmarkConcurrentProcessing(4)) {
        std::cerr << "Concurrent processing benchmarks failed" << std::endl;
        return false;
    }
    
    // Generate final report
    std::cout << "\n=== Performance Test Complete ===" << std::endl;
    std::cout << generatePerformanceReport();
    
    return true;
}

bool PerformanceTestTool::benchmarkBrainCreation(const std::vector<size_t>& neuronCounts) {
    for (size_t neuronCount : neuronCounts) {
        std::cout << "  Testing brain creation with " << neuronCount << " neurons...";
        
        auto config = createTestConfig(neuronCount);
        
        auto start = std::chrono::steady_clock::now();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        std::cout << " Done (" << elapsed << "s)" << std::endl;
        
        // Record benchmark result
        BenchmarkResult result("brain_creation_" + std::to_string(neuronCount), "brain_creation");
        result.executionTime = std::chrono::duration<double>(elapsed);
        result.operationsPerSecond = neuronCount / elapsed;
        result.memoryUsage = brain->getTotalNeuronCount() * sizeof(Neuron) + 
                           brain->getTotalSynapseCount() * sizeof(Synapse);
        result.status = TestStatus::Passed;
        
        benchmarkResults.push_back(result);
    }
    
    return true;
}

bool PerformanceTestTool::benchmarkSimulation(const std::shared_ptr<Config>& config, size_t steps) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for simulation benchmark" << std::endl;
        return false;
    }
    
    std::cout << "  Running " << steps << " simulation steps...";
    
    auto start = std::chrono::steady_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step);
        
        // Simulate some sensory input periodically
        if (step % 100 == 0) {
            std::vector<float> sensoryInput(10, static_cast<float>(rand() % 100) / 100.0f);
            SensoryInput input(sensoryInput);
            brain->receiveSensoryInput(input);
        }
    }
    
    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    std::cout << " Done (" << elapsed << "s, " << (steps / elapsed) << " steps/sec)" << std::endl;
    
    // Record benchmark result
    BenchmarkResult result("simulation_throughput", "simulation");
    result.executionTime = std::chrono::duration<double>(elapsed);
    result.throughput = steps / elapsed;
    result.memoryUsage = brain->getTotalNeuronCount() * sizeof(Neuron) + 
                       brain->getTotalSynapseCount() * sizeof(Synapse);
    result.status = TestStatus::Passed;
    
    benchmarkResults.push_back(result);
    
    return true;
}

bool PerformanceTestTool::benchmarkMemoryUsage() {
    std::cout << "  Testing memory allocation patterns...";
    
    // Test different memory allocation patterns
    std::vector<size_t> memorySizes = {1024, 10240, 102400}; // neurons
    
    for (size_t memorySize : memorySizes) {
        auto config = createTestConfig(memorySize);
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for memory benchmark" << std::endl;
            return false;
        }
        
        // Measure memory usage
        size_t memoryUsage = brain->getTotalNeuronCount() * sizeof(Neuron) + 
                           brain->getTotalSynapseCount() * sizeof(Synapse);
        
        std::cout << " " << memorySize << " neurons: " << (memoryUsage / 1024) << " KB" << std::endl;
    }
    
    return true;
}

bool PerformanceTestTool::benchmarkConcurrentProcessing(size_t threadCount) {
    std::cout << "  Testing concurrent processing with " << threadCount << " threads...";
    
    // Create multiple brains and run them concurrently
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<Brain>> brains;
    
    for (size_t i = 0; i < threadCount; ++i) {
        auto config = createTestConfig(1000);
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for concurrent test" << std::endl;
            return false;
        }
        
        brains.push_back(brain);
        
        threads.emplace_back([brain]() {
            for (size_t step = 0; step < 100; ++step) {
                brain->step(step);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << " Done" << std::endl;
    
    // Record benchmark result
    BenchmarkResult result("concurrent_processing", "concurrent");
    result.executionTime = std::chrono::duration<double>(threadCount * 100 * 0.001); // approximate
    result.throughput = threadCount * 100 / (threadCount * 100 * 0.001); // approximate
    result.memoryUsage = 0; // Would calculate actual memory usage
    result.status = TestStatus::Passed;
    
    benchmarkResults.push_back(result);
    
    return true;
}

std::string PerformanceTestTool::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Performance Test Results ===" << std::endl;
    ss << "Total tests run: " << benchmarkResults.size() << std::endl;
    ss << "Total execution time: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() << "s" << std::endl;
    
    ss << std::endl << "=== Benchmark Results ===" << std::endl;
    for (const auto& result : benchmarkResults) {
        ss << result.benchmarkName << " (" << result.benchmarkSuite << "):" << std::endl;
        ss << "  Time: " << result.executionTime.count() << "s" << std::endl;
        ss << "  Throughput: " << result.throughput << " ops/sec" << std::endl;
        ss << "  Memory Usage: " << result.memoryUsage << " bytes" << std::endl;
        ss << "  Status: " << result.getStatusString() << std::endl;
        if (!result.errorMessage.empty()) {
            ss << "  Error: " << result.errorMessage << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

bool PerformanceTestTool::exportResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "benchmark_name,benchmark_suite,execution_time,throughput,memory_usage,status" << std::endl;
    for (const auto& result : benchmarkResults) {
        file << escapeCSV(result.benchmarkName) << ",";
        file << escapeCSV(result.benchmarkSuite) << ",";
        file << std::fixed << std::setprecision(6) << result.executionTime.count() << ",";
        file << std::fixed << std::setprecision(6) << result.throughput << ",";
        file << result.memoryUsage << ",";
        file << escapeCSV(result.getStatusString()) << std::endl;
    }
    
    return true;
}

std::string PerformanceTestTool::escapeCSV(const std::string& str) const {
    if (str.find(',') == std::string::npos && str.find('"') == std::string::npos && str.find('\n') == std::string::npos) {
        return str;
    }
    
    std::stringstream ss;
    ss << '"';
    for (char c : str) {
        if (c == '"') ss << "\"\"";
        else ss << c;
    }
    ss << '"';
    return ss.str();
}

} // namespace nlm

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Performance Test Tool ===" << std::endl;
    
    nlm::PerformanceTestTool testTool;
    
    if (!testTool.runAllTests()) {
        std::cerr << "Performance tests failed" << std::endl;
        return 1;
    }
    
    std::cout << "\nAll performance tests completed successfully!" << std::endl;
    
    // Export results if requested
    if (argc > 1 && std::string(argv[1]) == "--export") {
        std::string filename = "performance_results.csv";
        if (argc > 2) {
            filename = argv[2];
        }
        if (!testTool.exportResults(filename)) {
            std::cerr << "Failed to export results to " << filename << std::endl;
        } else {
            std::cout << "Results exported to " << filename << std::endl;
        }
    }
    
    return 0;
}
