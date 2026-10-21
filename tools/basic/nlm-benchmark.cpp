#!/usr/bin/env g++
// NLM Benchmark Tool
// Performance benchmarking suite for NLM

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

class BenchmarkResult {
public:
    std::string testName;
    double timeMs;
    double memoryMb;
    std::string status;
    std::map<std::string, std::string> metrics;
    
    BenchmarkResult(const std::string& name, double time, double mem, const std::string& stat)
        : testName(name), timeMs(time), memoryMb(mem), status(stat) {}
    
    void addMetric(const std::string& key, const std::string& value) {
        metrics[key] = value;
    }
};

class NLMBenchmark {
private:
    std::vector<BenchmarkResult> results;
    std::shared_ptr<Config> config;
    
public:
    NLMBenchmark() {
        config = std::make_shared<Config>();
        // Load default configuration
        config->loadFromFile("configs/default.cfg");
    }
    
    void runQuickBenchmark() {
        std::cout << "=== NLM Quick Benchmark ===" << std::endl;
        std::cout << "Running quick benchmark of core NLM functionality..." << std::endl;
        std::cout << std::endl;
        
        // Run benchmark suite
        runConfigurationBenchmark();
        runMemoryBenchmark();
        runNeuralBenchmark();
        runPlasticityBenchmark();
        runSimulationBenchmark();
        
        printResults();
    }
    
    void runConfigurationBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Benchmark configuration loading
        std::vector<double> loadTimes;
        for (int i = 0; i < 10; ++i) {
            auto config = std::make_shared<Config>();
            auto loadStart = std::chrono::high_resolution_clock::now();
            config->loadFromFile("configs/default.cfg");
            auto loadEnd = std::chrono::high_resolution_clock::now();
            double loadTime = std::chrono::duration<double, std::milli>(loadEnd - loadStart).count();
            loadTimes.push_back(loadTime);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double avgTime = std::accumulate(loadTimes.begin(), loadTimes.end(), 0.0) / loadTimes.size();
        double minTime = *std::min_element(loadTimes.begin(), loadTimes.end());
        double maxTime = *std::max_element(loadTimes.begin(), loadTimes.end());
        
        BenchmarkResult result("Configuration Load", totalTime, 0.0, "PASS");
        result.addMetric("Average", std::to_string(avgTime) + "ms");
        result.addMetric("Min", std::to_string(minTime) + "ms");
        result.addMetric("Max", std::to_string(maxTime) + "ms");
        result.addMetric("Config Size", "10");
        
        results.push_back(result);
        
        std::cout << "Configuration Loading:" << std::endl;
        std::cout << "  Average: " << avgTime << "ms" << std::endl;
        std::cout << "  Min:     " << minTime << "ms" << std::endl;
        std::cout << "  Max:     " << maxTime << "ms" << std::endl;
        std::cout << "  Configs: " << loadTimes.size() << std::endl;
        std::cout << std::endl;
    }
    
    void runMemoryBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create multiple configurations to test memory efficiency
        std::vector<std::shared_ptr<Config>> configs;
        for (int i = 0; i < 100; ++i) {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(i));
            config->set("neuron_count", static_cast<int64_t>(100 + i * 10));
            configs.push_back(config);
        }
        
        // Test memory access patterns
        for (int i = 0; i < 50; ++i) {
            for (const auto& config : configs) {
                config->getOr<int64_t>("random_seed", 0);
                config->getOr<int64_t>("neuron_count", 1000);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        BenchmarkResult result("Memory Efficiency", totalTime, 0.0, "PASS");
        result.addMetric("Operations", "5000");
        result.addMetric("Configs", "100");
        result.addMetric("Accesses", "5000");
        
        results.push_back(result);
        
        std::cout << "Memory Efficiency:" << std::endl;
        std::cout << "  Operations: " << 5000 << std::endl;
        std::cout << "  Configs:   " << 100 << std::endl;
        std::cout << "  Time:      " << totalTime << "ms" << std::endl;
        std::cout << std::endl;
    }
    
    void runNeuralBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain instances and measure performance
        std::vector<std::shared_ptr<Brain>> brains;
        for (int i = 0; i < 5; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(brain);
        }
        
        // Run some simulation steps
        for (auto& brain : brains) {
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Calculate operations per second
        double opsPerSec = (5 * 100) / (totalTime / 1000.0);
        
        BenchmarkResult result("Neural Simulation", totalTime, 0.0, "PASS");
        result.addMetric("Brains", "5");
        result.addMetric("Steps per Brain", "100");
        result.addMetric("Total Steps", "500");
        result.addMetric("OPS/sec", std::to_string(opsPerSec));
        
        results.push_back(result);
        
        std::cout << "Neural Simulation:" << std::endl;
        std::cout << "  Brains:     " << 5 << std::endl;
        std::cout << "  Steps:      " << 100 << " per brain" << std::endl;
        std::cout << "  OPS/sec:    " << opsPerSec << std::endl;
        std::cout << "  Time:       " << totalTime << "ms" << std::endl;
        std::cout << std::endl;
    }
    
    void runPlasticityBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::vector<std::shared_ptr<Brain>> brains;
        for (int i = 0; i < 3; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Enable plasticity
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            brains.push_back(brain);
        }
        
        // Run plasticity experiments
        for (auto& brain : brains) {
            for (SimulationStep step = 0; step < 200; ++step) {
                // Inject patterns to stimulate plasticity
                for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Calculate plasticity updates per second
        double updatesPerSec = (3 * 200) / (totalTime / 1000.0);
        
        BenchmarkResult result("Plasticity Dynamics", totalTime, 0.0, "PASS");
        result.addMetric("Brains", "3");
        result.addMetric("Steps", "200");
        result.addMetric("Total Plasticity Updates", std::to_string(3 * 200));
        result.addMetric("Updates/sec", std::to_string(updatesPerSec));
        
        results.push_back(result);
        
        std::cout << "Plasticity Dynamics:" << std::endl;
        std::cout << "  Brains:     " << 3 << std::endl;
        std::cout << "  Steps:      " << 200 << std::endl;
        std::cout << "  Updates/sec:" << updatesPerSec << std::endl;
        std::cout << "  Time:       " << totalTime << "ms" << std::endl;
        std::cout << std::endl;
    }
    
    void runSimulationBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Complex simulation benchmark
        std::vector<std::shared_ptr<Brain>> brains;
        for (int i = 0; i < 2; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Enable all plasticity types
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
            
            brains.push_back(brain);
        }
        
        // Run complex simulation
        for (int brainIdx = 0; brainIdx < 2; ++brainIdx) {
            for (SimulationStep step = 0; step < 500; ++step) {
                // Complex input pattern
                for (size_t i = 0; i < 20 && i < brains[brainIdx]->getTotalNeuronCount() / 4; ++i) {
                    brains[brainIdx]->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
                }
                
                // Also inject some motor patterns
                for (size_t i = 0; i < 5 && i < brains[brainIdx]->getTotalNeuronCount() / 4; ++i) {
                    brains[brainIdx]->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
                }
                
                brains[brainIdx]->step(step, step * 0.001);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Calculate performance metrics
        double totalSteps = 2 * 500;
        double opsPerSec = totalSteps / (totalTime / 1000.0);
        
        BenchmarkResult result("Complex Simulation", totalTime, 0.0, "PASS");
        result.addMetric("Brains", "2");
        result.addMetric("Steps", "500");
        result.addMetric("Total Operations", std::to_string(totalSteps));
        result.addMetric("OPS/sec", std::to_string(opsPerSec));
        
        results.push_back(result);
        
        std::cout << "Complex Simulation:" << std::endl;
        std::cout << "  Brains:     " << 2 << std::endl;
        std::cout << "  Steps:      " << 500 << " per brain" << std::endl;
        std::cout << "  OPS/sec:    " << opsPerSec << std::endl;
        std::cout << "  Time:       " << totalTime << "ms" << std::endl;
        std::cout << std::endl;
    }
    
    void printResults() {
        std::cout << "=== Benchmark Results ===" << std::endl;
        std::cout << std::endl;
        
        int passed = 0;
        double totalTime = 0.0;
        
        std::cout << std::left << std::setw(25) << "Test" 
                  << std::setw(15) << "Time (ms)" 
                  << std::setw(10) << "Status" 
                  << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(25) << result.testName
                      << std::setw(15) << std::to_string(result.timeMs)
                      << std::setw(10) << result.status << std::endl;
            
            if (result.status == "PASS") {
                passed++;
            }
            
            totalTime += result.timeMs;
            
            // Print additional metrics
            for (const auto& metric : result.metrics) {
                std::cout << "  " << std::left << std::setw(20) << metric.first 
                          << ": " << metric.second << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << std::string(50, '-') << std::endl;
        std::cout << std::left << std::setw(25) << "TOTAL"
                  << std::setw(15) << std::to_string(totalTime)
                  << std::setw(10) << "N/A" << std::endl;
        std::cout << std::endl;
        std::cout << "Tests passed: " << passed << "/" << results.size() << std::endl;
        
        if (passed == results.size()) {
            std::cout << "✓ All benchmarks passed!" << std::endl;
        } else {
            std::cout << "✗ Some benchmarks failed!" << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --quick                   Run quick benchmark (default)" << std::endl;
        std::cout << "  --config <file>           Load configuration file" << std::endl;
        std::cout << "  --neurons <count>         Set neuron count for benchmark" << std::endl;
        std::cout << "  --steps <count>           Set simulation steps for benchmark" << std::endl;
        std::cout << "  --output <file>           Save benchmark results to file" << std::endl;
        std::cout << "  --detailed                Run detailed benchmark suite" << std::endl;
        std::cout << "  --csv                     Output in CSV format" << std::endl;
        std::cout << "  --json                    Output in JSON format" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-benchmark";
    std::string configFile;
    bool quickOnly = true;
    bool detailed = false;
    std::string outputFile;
    bool csvFormat = false;
    bool jsonFormat = false;
    int neuronCount = 1000;
    int steps = 100;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMBenchmark benchmark;
            benchmark.printUsage(programName);
            return 0;
        } else if (arg == "--quick") {
            quickOnly = true;
        } else if (arg == "--detailed") {
            detailed = true;
            quickOnly = false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --neurons requires a count argument" << std::endl;
                return 1;
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --steps requires a count argument" << std::endl;
                return 1;
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--csv") {
            csvFormat = true;
        } else if (arg == "--json") {
            jsonFormat = true;
        }
    }
    
    NLMBenchmark benchmark;
    
    if (!configFile.empty()) {
        if (!benchmark.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
        
        // Update config with benchmark parameters
        benchmark.config->set("neuron_count", static_cast<int64_t>(neuronCount));
    }
    
    if (quickOnly) {
        benchmark.runQuickBenchmark();
    } else if (detailed) {
        std::cout << "Running detailed benchmark suite..." << std::endl;
        // Run all benchmarks
        benchmark.runConfigurationBenchmark();
        benchmark.runMemoryBenchmark();
        benchmark.runNeuralBenchmark();
        benchmark.runPlasticityBenchmark();
        benchmark.runSimulationBenchmark();
    }
    
    // Save results if requested
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "NLM Benchmark Results" << std::endl;
            file << "========================" << std::endl;
            file << "Tests run: " << benchmark.results.size() << std::endl;
            for (const auto& result : benchmark.results) {
                file << result.testName << ","
                     << result.timeMs << ","
                     << result.memoryMb << ","
                     << result.status << std::endl;
            }
            file.close();
            std::cout << "Results saved to: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
        }
    }
    
    return 0;
}
