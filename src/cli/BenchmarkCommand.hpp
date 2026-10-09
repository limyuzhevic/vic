// Benchmark Command - Performance testing

#pragma once

#include "CommandBase.hpp"
#include "CLIParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>

class BenchmarkCommand : public CommandBase {
public:
    BenchmarkCommand() : CommandBase("benchmark", "Performance testing and benchmarking") {}
    
    int execute(std::shared_ptr<Config> config, const CLIParser& parser) override {
        std::cout << "NLM Performance Benchmark" << std::endl;
        std::cout << "==========================" << std::endl;
        
        // Get benchmark configuration
        std::vector<std::pair<size_t, size_t>> benchmarks;
        
        // Get custom benchmarks or use defaults
        std::vector<std::string> args = parser.getPositionalArgs();
        if (!args.empty()) {
            // Parse custom benchmark configurations
            for (const auto& arg : args) {
                // Parse format like "neurons,steps"
                size_t commaPos = arg.find(',');
                if (commaPos != std::string::npos) {
                    try {
                        size_t neurons = std::stoull(arg.substr(0, commaPos));
                        size_t steps = std::stoull(arg.substr(commaPos + 1));
                        benchmarks.emplace_back(neurons, steps);
                    } catch (const std::exception&) {
                        printError("Invalid benchmark specification: " + arg);
                        printUsage("benchmark", "<neurons,steps> [<neurons,steps> ...]");
                        return 1;
                    }
                }
            }
        }
        
        // Use default benchmarks if none provided
        if (benchmarks.empty()) {
            benchmarks = {
                {100, 1000},      // 100 neurons, 1000 steps
                {500, 1000},      // 500 neurons, 1000 steps
                {1000, 500},      // 1000 neurons, 500 steps
                {2000, 200},      // 2000 neurons, 200 steps
            };
        }
        
        std::cout << "Running " << benchmarks.size() << " benchmark configurations:" << std::endl;
        for (size_t i = 0; i < benchmarks.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << benchmarks[i].first 
                      << " neurons, " << benchmarks[i].second << " steps" << std::endl;
        }
        std::cout << std::endl;
        
        // Configure logging for benchmark
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Quiet benchmark output
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Run benchmarks
        std::vector<BenchmarkResult> results;
        
        for (const auto& bench : benchmarks) {
            BenchmarkResult result = runBenchmark(bench.first, bench.second, parser);
            results.push_back(result);
        }
        
        // Print results
        printBenchmarkResults(results, parser);
        
        // Print summary
        printBenchmarkSummary(results, parser);
        
        return 0;
    }
    
private:
    struct BenchmarkResult {
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;      // seconds
        double stepsPerSecond;
        double spikesPerStep;
        double synapsesPerNeuron;
        double memoryUsage;    // MB (placeholder)
    };
    
    BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, const CLIParser& parser) {
        std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        // Create configuration
        auto benchConfig = std::make_shared<Config>();
        
        // Get configuration from main config with overrides
        benchConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        benchConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        benchConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        benchConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        benchConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Apply benchmark-specific config values
        auto simTimestep = config->get<double>("simulation_timestep");
        if (simTimestep) {
            benchConfig->set("simulation_timestep", *simTimestep, ConfigSource::Default);
        }
        
        // Get additional benchmark options
        bool warmup = parser.getBool("--warmup", false);
        bool detailed = parser.getBool("--detailed", false);
        
        if (warmup) {
            std::cout << " (with warmup), " << std::flush;
        }
        
        // TODO: In real implementation, create actual brain and run simulation
        // For now, simulate benchmark results
        
        // Simulate benchmark timing
        auto startTime = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(50 + neuronCount / 10)); // Simulate work
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Simulate results based on neuron count
        BenchmarkResult result;
        result.neurons = neuronCount;
        result.synapses = static_cast<size_t>(neuronCount * neuronCount * 0.1); // Approximate
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = 0.1 + (neuronCount / 1000.0) * 0.9; // Varying spikes
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        result.memoryUsage = (neuronCount * 4.0) / 1024.0; // Approximate MB
        
        std::cout << " Done!" << std::endl;
        
        if (detailed) {
            std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
            std::cout << "    Memory: " << std::fixed << std::setprecision(1) << result.memoryUsage << " MB" << std::endl;
        }
        
        return result;
    }
    
    void printBenchmarkResults(const std::vector<BenchmarkResult>& results, const CLIParser& parser) {
        bool detailed = parser.getBool("--detailed", false);
        
        std::cout << std::endl;
        if (detailed) {
            std::cout << "Detailed Benchmark Results" << std::endl;
            std::cout << "==========================" << std::endl;
            
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& r = results[i];
                std::cout << std::endl;
                std::cout << "Configuration " << (i + 1) << ":" << std::endl;
                std::cout << "  Neurons:       " << r.neurons << std::endl;
                std::cout << "  Synapses:      " << r.synapses << std::endl;
                std::cout << "  Steps:         " << r.steps << std::endl;
                std::cout << "  Total Time:    " << std::fixed << std::setprecision(3) << r.totalTime << "s" << std::endl;
                std::cout << "  Steps/sec:     " << std::fixed << std::setprecision(0) << r.stepsPerSecond << std::endl;
                std::cout << "  Spikes/step:   " << std::fixed << std::setprecision(2) << r.spikesPerStep << std::endl;
                std::cout << "  Synapses/Neuron:" << std::fixed << std::setprecision(1) << r.synapsesPerNeuron << std::endl;
                std::cout << "  Memory Usage:  " << std::fixed << std::setprecision(1) << r.memoryUsage << " MB" << std::endl;
            }
        } else {
            // Compact table
            std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
            std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
            std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
            std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
            
            for (const auto& r : results) {
                std::cout << "║ " << std::setw(13) << r.neurons 
                          << " ║ " << std::setw(13) << r.synapses
                          << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                          << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                          << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                          << " ║" << std::endl;
            }
            
            std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void printBenchmarkSummary(const std::vector<BenchmarkResult>& results, const CLIParser& parser) {
        bool summary = parser.getBool("--summary", false);
        
        if (summary || results.size() > 1) {
            std::cout << "Benchmark Summary" << std::endl;
            std::cout << "==================" << std::endl;
            
            double totalTime = 0.0;
            double totalSteps = 0.0;
            double avgStepsPerSec = 0.0;
            
            for (const auto& r : results) {
                totalTime += r.totalTime;
                totalSteps += r.steps;
            }
            
            if (!results.empty()) {
                avgStepsPerSec = totalSteps / totalTime;
            }
            
            std::cout << "Total benchmarks: " << results.size() << std::endl;
            std::cout << "Total time:       " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
            std::cout << "Total steps:      " << std::fixed << std::setprecision(0) << totalSteps << std::endl;
            std::cout << "Avg steps/sec:    " << std::fixed << std::setprecision(0) << avgStepsPerSec << std::endl;
            
            // Find best performance
            if (!results.empty()) {
                auto bestIt = std::max_element(results.begin(), results.end(),
                    [](const BenchmarkResult& a, const BenchmarkResult& b) {
                        return a.stepsPerSecond < b.stepsPerSecond;
                    });
                
                std::cout << "Best performance:  " << bestIt->neurons << " neurons, "
                          << std::fixed << std::setprecision(0) << bestIt->stepsPerSecond 
                          << " steps/sec" << std::endl;
            }
        }
    }
};
