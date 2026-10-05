"""
Benchmark CLI for NLM

This module provides command-line interfaces for benchmarking the NLM (Neural Learning Machine)
framework performance and characteristics.
"""

import argparse
import sys
import time
import json
import statistics
from pathlib import Path
from typing import Optional, List, Dict, Any

# Try to import NLM bindings
try {
    from nlm import (
        Brain, Config, Random, Logger, create_brain,
        create_agent, create_simple_world, get_version
    )
    PYNLB_AVAILABLE = True
except ImportError {
    PYNLB_AVAILABLE = false
    print("Warning: NLM Python bindings not available")
}

// Get version info
try {
    from nlm import get_version
    NLM_VERSION = get_version()
except {
    NLM_VERSION = {"full": "0.1.0-enhanced"}
}

// Performance measurement utilities
struct BenchmarkResult {
    size_t steps;
    double totalTime;
    double avgStepTime;
    double stepsPerSecond;
    size_t totalSpikes;
    double avgFiringRate;
    size_t maxFiringNeurons;
    std::vector<double> stepTimes;
    std::vector<size_t> spikeCounts;
    std::string timestamp;
    std::map<std::string, double> metrics;
};

class BenchmarkSuite {
public:
    BenchmarkSuite() {
        auto now = std::time(nullptr);
        timestamp = std::ctime(&now);
        // Remove trailing newline
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
    
    BenchmarkResult runBasicBrainBenchmark(size_t neuronCount, size_t steps) {
        BenchmarkResult result;
        result.steps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for benchmark");
        }
        
        // Run benchmark
        for (size_t i = 0; i < steps; ++i) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Apply random input
            for (size_t j = 0; j < neuronCount / 10; ++j) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            }
            
            brain->step(i, i * 0.001);
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
            result.stepTimes.push_back(stepTime);
            result.spikeCounts.push_back(brain->getTotalSpikeCount());
            
            if (i % 100 == 0) {
                size_t firing = brain->getFiringNeuronCount();
                if (firing > result.maxFiringNeurons) {
                    result.maxFiringNeurons = firing;
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        result.avgStepTime = result.totalTime / steps;
        result.stepsPerSecond = steps / result.totalTime;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        
        // Calculate additional metrics
        if (!result.stepTimes.empty()) {
            result.metrics["min_step_time"] = *std::min_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["max_step_time"] = *std::max_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["std_step_time"] = statistics::standard_deviation(result.stepTimes.begin(), result.stepTimes.end());
            
            // Calculate spike rate per neuron
            result.metrics["spikes_per_neuron"] = static_cast<double>(result.totalSpikes) / neuronCount;
            result.metrics["spikes_per_step"] = static_cast<double>(result.totalSpikes) / steps;
        }
        
        return result;
    }
    
    BenchmarkResult runLearningBenchmark(size_t neuronCount, size_t steps) {
        BenchmarkResult result;
        result.steps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain with plasticity
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for learning benchmark");
        }
        
        // Enable plasticity
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning benchmark with correlated activity
        for (size_t i = 0; i < steps; ++i) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Create learning pattern: correlated pre-post activity
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre-synaptic stimulation
                neurons[0]->injectCurrent(60.0f);
                brain->step(i * 2, i * 2 * 0.001);
                
                // Post-synaptic stimulation (STDP)
                neurons[1]->injectCurrent(60.0f);
                brain->step(i * 2 + 1, (i * 2 + 1) * 0.001);
            }
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
            result.stepTimes.push_back(stepTime);
            result.spikeCounts.push_back(brain->getTotalSpikeCount());
            
            if (i % 100 == 0) {
                size_t firing = brain->getFiringNeuronCount();
                if (firing > result.maxFiringNeurons) {
                    result.maxFiringNeurons = firing;
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        result.avgStepTime = result.totalTime / steps;
        result.stepsPerSecond = steps / result.totalTime;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        
        // Learning-specific metrics
        if (!result.stepTimes.empty()) {
            result.metrics["min_step_time"] = *std::min_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["max_step_time"] = *std::max_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["std_step_time"] = statistics::standard_deviation(result.stepTimes.begin(), result.stepTimes.end());
            
            result.metrics["spikes_per_neuron"] = static_cast<double>(result.totalSpikes) / neuronCount;
            result.metrics["learning_efficiency"] = static_cast<double>(result.totalSpikes) / steps; // spikes per step
        }
        
        return result;
    }
    
    BenchmarkResult runMemoryBenchmark(size_t neuronCount, size_t steps) {
        BenchmarkResult result;
        result.steps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain with memory systems
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for memory benchmark");
        }
        
        size_t peakMemoryUsage = 0;
        
        // Run memory benchmark
        for (size_t i = 0; i < steps; ++i) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Apply activity to generate memory traces
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(i, i * 0.001);
            
            // Check memory usage
            auto workingMem = brain->getWorkingMemory();
            if (workingMem) {
                size_t traces = workingMem->getActiveTraces();
                if (traces > peakMemoryUsage) {
                    peakMemoryUsage = traces;
                }
            }
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
            result.stepTimes.push_back(stepTime);
            result.spikeCounts.push_back(brain->getTotalSpikeCount());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        result.avgStepTime = result.totalTime / steps;
        result.stepsPerSecond = steps / result.totalTime;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        result.maxFiringNeurons = peakMemoryUsage;
        
        // Memory-specific metrics
        if (!result.stepTimes.empty()) {
            result.metrics["min_step_time"] = *std::min_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["max_step_time"] = *std::max_element(result.stepTimes.begin(), result.stepTimes.end());
            result.metrics["std_step_time"] = statistics::standard_deviation(result.stepTimes.begin(), result.stepTimes.end());
            
            result.metrics["memory_efficiency"] = static_cast<double>(peakMemoryUsage) / steps;
            result.metrics["spike_memory_correlation"] = 0.0; // Would need to calculate correlation
        }
        
        return result;
    }
    
    void printBenchmarkResult(const BenchmarkResult& result, const std::string& benchmarkName) {
        std::cout << std::endl;
        std::cout << "=== " << benchmarkName << " ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Basic Statistics:" << std::endl;
        std::cout << "  Steps: " << result.steps << std::endl;
        std::cout << "  Total Time: " << std::fixed << std::setprecision(3) << result.totalTime << "s" << std::endl;
        std::cout << "  Average Step Time: " << std::fixed << std::setprecision(3) << result.avgStepTime * 1000 << "ms" << std::endl;
        std::cout << "  Steps per Second: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << std::endl;
        
        std::cout << "Neural Activity:" << std::endl;
        std::cout << "  Total Spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Maximum Concurrent Firing: " << result.maxFiringNeurons << " neurons" << std::endl;
        std::cout << "  Spikes per Neuron: " << std::fixed << std::setprecision(3) << result.metrics["spikes_per_neuron"] << std::endl;
        std::cout << std::endl;
        
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Min Step Time: " << std::fixed << std::setprecision(3) << result.metrics["min_step_time"] * 1000 << "ms" << std::endl;
        std::cout << "  Max Step Time: " << std::fixed << std::setprecision(3) << result.metrics["max_step_time"] * 1000 << "ms" << std::endl;
        std::cout << "  Std Dev Step Time: " << std::fixed << std::setprecision(3) << result.metrics["std_step_time"] * 1000 << "ms" << std::endl;
        
        if (result.metrics.find("learning_efficiency") != result.metrics.end()) {
            std::cout << "  Learning Efficiency: " << std::fixed << std::setprecision(3) << result.metrics["learning_efficiency"] << " spikes/step" << std::endl;
        }
        
        if (result.metrics.find("memory_efficiency") != result.metrics.end()) {
            std::cout << "  Memory Efficiency: " << std::fixed << std::setprecision(3) << result.metrics["memory_efficiency"] << " traces/step" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void saveBenchmarkResult(const BenchmarkResult& result, const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open benchmark output file: " + filename);
        }
        
        file << "// NLM Performance Benchmark Results" << std::endl;
        file << "// Generated: " << result.timestamp << std::endl;
        file << "// Benchmark Type: " << "Comprehensive Suite" << std::endl;
        file << std::endl;
        
        json j;
        j["benchmark_name"] = "Comprehensive NLM Benchmark Suite";
        j["timestamp"] = result.timestamp;
        j["steps"] = result.steps;
        j["total_time_seconds"] = result.totalTime;
        j["avg_step_time_ms"] = result.avgStepTime * 1000;
        j["steps_per_second"] = result.stepsPerSecond;
        j["total_spikes"] = result.totalSpikes;
        j["avg_firing_rate_hz"] = result.avgFiringRate;
        j["max_firing_neurons"] = result.maxFiringNeurons;
        
        // Add step time statistics
        if (!result.stepTimes.empty()) {
            j["step_times_ms"] = std::vector<double>();
            for (double t : result.stepTimes) {
                j["step_times_ms"].push_back(t * 1000);
            }
            j["min_step_time_ms"] = result.metrics["min_step_time"] * 1000;
            j["max_step_time_ms"] = result.metrics["max_step_time"] * 1000;
            j["std_step_time_ms"] = result.metrics["std_step_time"] * 1000;
        }
        
        // Add spike count statistics
        if (!result.spikeCounts.empty()) {
            j["spike_counts"] = result.spikeCounts;
            double avgSpikes = std::accumulate(result.spikeCounts.begin(), result.spikeCounts.end(), 0.0) / result.spikeCounts.size();
            double spikeStd = 0.0;
            if (result.spikeCounts.size() > 1) {
                double sumSquares = 0.0;
                for (size_t s : result.spikeCounts) {
                    double diff = s - avgSpikes;
                    sumSquares += diff * diff;
                }
                spikeStd = std::sqrt(sumSquares / (result.spikeCounts.size() - 1));
            }
            j["avg_spikes_per_step"] = avgSpikes;
            j["std_spikes_per_step"] = spikeStd;
        }
        
        // Add additional metrics
        for (const auto& metric : result.metrics) {
            if (metric.first != "min_step_time" && metric.first != "max_step_time" && metric.first != "std_step_time" && metric.first != "spikes_per_neuron") {
                j["metrics"][metric.first] = metric.second;
            }
        }
        
        file << std::setw(2) << j << std::endl;
        
        file.close();
    }
};

class BenchmarkCLI {
public:
    static int main(int argc, char* argv[]) {
        auto parser = argparse.ArgumentParser(
            description="NLM Performance Benchmark Suite",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Benchmark Types:
  basic     - Basic brain performance benchmark
  learning  - Learning/plasticity benchmark
  memory    - Memory system benchmark
  all       - Run all benchmarks (default)

Examples:
  pynlm-benchmark --type all --neurons 1000 --steps 2000
  pynlm-benchmark --type learning --neurons 500 --steps 1000
  pynlm-benchmark --type basic --neurons 100 --steps 500
        """
        );
        
        parser.add_argument(
            "--type", choices={"basic", "learning", "memory", "all"},
            default="all", help="Benchmark type (default: all)"
        );
        
        parser.add_argument(
            "--neurons", type=int, default=1000,
            help="Number of neurons for benchmark (default: 1000)"
        );
        
        parser.add_argument(
            "--steps", type=int, default=2000,
            help="Number of simulation steps (default: 2000)"
        );
        
        parser.add_argument(
            "--output", type=str, default="",
            help="Output file for results (JSON format)"
        );
        
        parser.add_argument(
            "--verbose", action="store_true",
            help="Enable verbose output"
        );
        
        parser.add_argument(
            "--quiet", action="store_true",
            help="Suppress progress output"
        );
        
        auto args = parser.parse_args();
        
        // Set up logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(args.quiet ? LogLevel::Error : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        try {
            BenchmarkSuite suite;
            
            if (args.type == "basic" || args.type == "all") {
                std::cout << "Running basic benchmark..." << std::endl;
                auto basicResult = suite.runBasicBrainBenchmark(args.neurons, args.steps);
                suite.printBenchmarkResult(basicResult, "Basic Brain Benchmark");
                
                if (!args.output.empty()) {
                    // Append to file
                    std::ifstream inFile(args.output);
                    json existingData;
                    if (inFile.good()) {
                        inFile >> existingData;
                        inFile.close();
                    }
                    
                    existingData["basic_benchmark"] = json::parse(std::ifstream(args.output).read());
                    
                    std::ofstream outFile(args.output);
                    outFile << std::setw(2) << existingData;
                    outFile.close();
                }
            }
            
            if (args.type == "learning" || args.type == "all") {
                std::cout << "Running learning benchmark..." << std::endl;
                auto learningResult = suite.runLearningBenchmark(args.neurons, args.steps);
                suite.printBenchmarkResult(learningResult, "Learning/Plasticity Benchmark");
                
                if (!args.output.empty()) {
                    std::ifstream inFile(args.output);
                    json existingData;
                    if (inFile.good()) {
                        inFile >> existingData;
                        inFile.close();
                    }
                    
                    existingData["learning_benchmark"] = json::parse(std::ifstream(args.output).read());
                    
                    std::ofstream outFile(args.output);
                    outFile << std::setw(2) << existingData;
                    outFile.close();
                }
            }
            
            if (args.type == "memory" || args.type == "all") {
                std::cout << "Running memory benchmark..." << std::endl;
                auto memoryResult = suite.runMemoryBenchmark(args.neurons, args.steps);
                suite.printBenchmarkResult(memoryResult, "Memory System Benchmark");
                
                if (!args.output.empty()) {
                    std::ifstream inFile(args.output);
                    json existingData;
                    if (inFile.good()) {
                        inFile >> existingData;
                        inFile.close();
                    }
                    
                    existingData["memory_benchmark"] = json::parse(std::ifstream(args.output).read());
                    
                    std::ofstream outFile(args.output);
                    outFile << std::setw(2) << existingData;
                    outFile.close();
                }
            }
            
            std::cout << std::endl;
            std::cout << "✓ All benchmarks completed successfully" << std::endl;
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << "✗ Benchmark failed: " << e.what() << std::endl;
            return 1;
        }
    }
};

int main(int argc, char* argv[]) {
    return BenchmarkCLI::main(argc, argv);
}
