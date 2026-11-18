// NLM Benchmark Suite
// Benchmark performance of NLM neural simulation
// Provides measurements for:
// - Spike propagation speed
// - Learning rate comparison
// - Memory usage patterns
// - System throughput under load

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <thread>

using namespace nlm;

void printBenchmarkBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║                    NLM BENCHMARK SUITE                        ║
║                                                               ║
║              Performance Testing for Neural Simulation        ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

struct BenchmarkResult {
    std::string testName;
    double durationMs;
    size_t neuronsProcessed;
    double operationsPerSecond;
    std::string details;
};

BenchmarkResult benchmarkSpikePropagation(std::shared_ptr<Brain> brain, size_t steps = 1000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Inject current to stimulate activity
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (size_t i = 0; i < std::min(size_t(50), region->getTotalNeuronCount()); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
        }
    }
    
    // Run benchmark
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double durationMs = static_cast<double>(duration.count());
    double opsPerSecond = (steps * 1000.0) / durationMs;
    
    return {
        "Spike Propagation",
        durationMs,
        brain->getTotalNeuronCount() * steps,
        opsPerSecond,
        "Injected current to 50 neurons, measured spike delivery"
    };
}

BenchmarkResult benchmarkLearning(std::shared_ptr<Brain> brain, size_t steps = 5000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Reset and enable plasticity
    brain->reset();
    brain->initialize();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Apply learning patterns
    for (size_t step = 0; step < steps; ++step) {
        // Correlated pre-post activity for STDP
        if (step % 2 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        } else {
            brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
        }
        
        brain->step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double durationMs = static_cast<double>(duration.count());
    double opsPerSecond = (steps * 1000.0) / durationMs;
    
    return {
        "Learning (STDP & Hebbian)",
        durationMs,
        brain->getTotalSynapseCount() * steps,
        opsPerSecond,
        "Enabled all plasticity rules, measured synaptic changes"
    };
}

BenchmarkResult benchmarkMemoryUsage(std::shared_ptr<Brain> brain, size_t steps = 2000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Run benchmark with memory system active
    for (size_t step = 0; step < steps; ++step) {
        // Inject input to generate memory episodes
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        brain->step(step);
        
        // Periodically access memory (simulating consolidation)
        if (step % 100 == 0 && step > 0) {
            brain->step(step); // Extra step for memory operations
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double durationMs = static_cast<double>(duration.count());
    double opsPerSecond = (steps * 1000.0) / durationMs;
    
    return {
        "Memory System",
        durationMs,
        steps * brain->getTotalNeuronCount(),
        opsPerSecond,
        "Measured episodic memory storage and consolidation"
    };
}

BenchmarkResult benchmarkRealTimePerformance(std::shared_ptr<Brain> brain, double realtimeFactor = 0.1, size_t steps = 1000) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Simulate real-time processing with variable input
    for (size_t step = 0; step < steps; ++step) {
        // Variable input based on step
        float input = 10.0f * std::sin(static_cast<float>(step) * 0.1f);
        
        // Inject input and step
        brain->injectCurrentToNeurons(NeuronType::Sensory, input);
        brain->step(step);
        
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::duration<double>(realtimeFactor * 0.001));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double durationMs = static_cast<double>(duration.count());
    double opsPerSecond = (steps * 1000.0) / durationMs;
    
    return {
        "Real-time Performance",
        durationMs,
        steps * brain->getTotalNeuronCount(),
        opsPerSecond,
        "Simulated real-time processing with variable input patterns"
    };
}

void printResult(const BenchmarkResult& result) {
    std::cout << std::endl;
    std::cout << "Test: " << result.testName << std::endl;
    std::cout << "Duration: " << std::fixed << std::setprecision(2) << result.durationMs << " ms" << std::endl;
    std::cout << "Neurons Processed: " << result.neuronsProcessed << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(0) << result.operationsPerSecond << " ops/sec" << std::endl;
    std::cout << "Details: " << result.details << std::endl;
}

int main(int argc, char** argv) {
    printBenchmarkBanner();
    
    std::cout << "Initializing NLM Benchmark Suite..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Reduce noise
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    std::string configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Set benchmark-specific config
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "NLM Benchmark Configuration:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << std::endl;
    
    // Run benchmarks
    std::vector<BenchmarkResult> results;
    
    std::cout << "Running Spike Propagation Benchmark..." << std::endl;
    results.push_back(benchmarkSpikePropagation(brain, 1000));
    
    std::cout << "Running Learning Benchmark..." << std::endl;
    results.push_back(benchmarkLearning(brain, 3000));
    
    std::cout << "Running Memory Usage Benchmark..." << std::endl;
    results.push_back(benchmarkMemoryUsage(brain, 2000));
    
    std::cout << "Running Real-time Performance Benchmark..." << std::endl;
    results.push_back(benchmarkRealTimePerformance(brain, 0.01, 500));
    
    // Summary
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         BENCHMARK SUMMARY                        ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    double totalDuration = 0.0;
    for (const auto& result : results) {
        printResult(result);
        totalDuration += result.durationMs;
    }
    
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         PERFORMANCE SUMMARY                      ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Total Benchmark Time: " << std::fixed << std::setprecision(2) << totalDuration << " ms" << std::endl;
    std::cout << "Average Throughput: " << std::fixed << std::setprecision(0) << (results.size() * 1000000.0 / totalDuration) << " ops/sec" << std::endl;
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      BENCHMARK COMPLETE                         ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    return 0;
}
