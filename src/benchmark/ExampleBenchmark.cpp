#include "benchmark/BenchmarkSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace nlm;

/**
 * Simple benchmark demonstrating NLM performance measurement
 */
class SimpleBenchmark {
public:
    SimpleBenchmark() {
        // Initialize benchmark system
        benchmarkSystem_ = ProfilerFactory::createBenchmarkSystem();
        benchmarkSystem_->initialize();
        
        // Initialize profiler
        profiler_ = ProfilerFactory::createProfiler();
    }
    
    void runConfigurationBenchmark(const std::string& name, 
                                   const std::unordered_map<std::string, std::string>& config) {
        std::cout << "Running benchmark: " << name << std::endl;
        
        // Create configuration
        auto brainConfig = std::make_shared<Config>();
        for (const auto& pair : config) {
            // Convert string value to appropriate type based on key
            if (pair.first.find("neuron") != std::string::npos) {
                try {
                    int intValue = std::stoi(pair.second);
                    brainConfig->set(pair.first, intValue);
                } catch (...) {
                    double doubleValue = std::stod(pair.second);
                    brainConfig->set(pair.first, doubleValue);
                }
            } else if (pair.first.find("rate") != std::string::npos || 
                       pair.first.find("weight") != std::string::npos) {
                float floatValue = std::stof(pair.second);
                brainConfig->set(pair.first, floatValue);
            } else if (pair.first.find("enable") != std::string::npos) {
                bool boolValue = (pair.second == "true" || pair.second == "1");
                brainConfig->set(pair.first, boolValue);
            } else {
                brainConfig->set(pair.first, pair.second);
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(brainConfig);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for benchmark: " << name << std::endl;
            return;
        }
        
        // Start profiler
        profiler_->start(name);
        
        // Run benchmark simulation
        size_t neuronsProcessed = 0;
        size_t steps = 1000;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            neuronsProcessed += brain->getFiringNeuronCount();
            
            // Simulate some work
            if (step % 100 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Stop profiler
        profiler_->stop(name, neuronsProcessed, brain->getTotalSynapseCount());
        
        double throughput = neuronsProcessed / (duration.count() / 1000.0); // neurons/ms
        
        std::cout << "  Duration: " << duration.count() << " ms" << std::endl;
        std::cout << "  Neurons processed: " << neuronsProcessed << std::endl;
        std::cout << "  Throughput: " << throughput << " neurons/ms" << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        
        // Add benchmark result
        benchmarkSystem_->addCustomBenchmark(name, [this, name, neuronsProcessed, duration]() {
            // Simulate benchmark function
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return true;
        });
    }
    
    void exportResults(const std::string& filepath) {
        std::cout << "Exporting results to: " << filepath << std::endl;
        
        // Export profiler results
        if (!profiler_->exportResults(filepath + "_profiler.json")) {
            std::cerr << "Failed to export profiler results" << std::endl;
        }
        
        // Export benchmark results
        if (!benchmarkSystem_->exportResults(filepath + "_benchmark.json")) {
            std::cerr << "Failed to export benchmark results" << std::endl;
        }
        
        std::cout << "Export completed." << std::endl;
    }
    
    void printSummary() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "BENCHMARK SUMMARY" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        auto results = profiler_->getResults();
        if (results.empty()) {
            std::cout << "No benchmarks completed." << std::endl;
            return;
        }
        
        double totalDuration = 0.0;
        double totalNeurons = 0.0;
        
        std::cout << "Benchmarks completed: " << results.size() << std::endl;
        std::cout << std::endl;
        
        for (const auto& result : results) {
            totalDuration += result.duration;
            totalNeurons += result.neurons;
            
            std::cout << "  " << result.name << ":" << std::endl;
            std::cout << "    Duration: " << result.duration << " ms" << std::endl;
            std::cout << "    Neurons: " << result.neurons << std::endl;
            std::cout << "    Synapses: " << result.synapses << std::endl;
            std::cout << "    Throughput: " << result.throughput << " neurons/ms" << std::endl;
            std::cout << "    Memory: " << result.memoryUsage << " MB" << std::endl;
        }
        
        if (!results.empty()) {
            double avgDuration = totalDuration / results.size();
            double avgNeurons = totalNeurons / results.size();
            
            std::cout << std::endl;
            std::cout << "OVERALL STATISTICS:" << std::endl;
            std::cout << "  Average Duration: " << avgDuration << " ms" << std::endl;
            std::cout << "  Average Neurons: " << avgNeurons << std::endl;
            std::cout << "  Total Throughput: " << (totalNeurons / (totalDuration / 1000.0)) << " neurons/s" << std::endl;
            
            benchmarkSystem_->getPerformanceSummary();
        }
    }

private:
    std::unique_ptr<Profiler> profiler_;
    std::unique_ptr<BenchmarkSystem> benchmarkSystem_;
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Benchmark Example ===" << std::endl;
    
    SimpleBenchmark benchmark;
    
    // Define different configurations to benchmark
    std::vector<std::pair<std::string, std::unordered_map<std::string, std::string>>> configs = {
        {"Basic Configuration", {
            {"neuron_count", "1000"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"simulation_timestep", "0.001"}
        }},
        {"Research Configuration", {
            {"neuron_count", "5000"},
            {"region_count", "4"},
            {"connection_probability", "0.15"},
            {"simulation_timestep", "0.001"},
            {"enable_checkpoints", "true"},
            {"enable_developmental", "true"}
        }},
        {"Production Configuration", {
            {"neuron_count", "10000"},
            {"region_count", "6"},
            {"connection_probability", "0.2"},
            {"simulation_timestep", "0.001"},
            {"enable_checkpoints", "true"},
            {"enable_parallel", "true"},
            {"memory_limit", "2048"}
        }}
    };
    
    // Run benchmarks
    for (const auto& config : configs) {
        benchmark.runConfigurationBenchmark(config.first, config.second);
        std::cout << std::endl;
    }
    
    // Export results
    benchmark.exportResults("./results/benchmark_");
    
    // Print summary
    benchmark.printSummary();
    
    std::cout << std::endl;
    std::cout << "Benchmark example completed successfully!" << std::endl;
    std::cout << "Results saved to: ./results/" << std::endl;
    
    return 0;
}

