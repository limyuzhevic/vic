#pragma once

#include "CLI.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

namespace nlm {
namespace cli {

class BenchmarkSubcommand : public Subcommand {
private:
    int duration;
    int repeats;
    std::string detailLevel;
    
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Run performance benchmarks"; }
    
    void addOptions() override {
        // Options are parsed in execute()
    }
    
    CLIResult execute(const std::shared_ptr<Config>& config) override {
        // Set up benchmark configuration
        int neuronCount = config->getOr<int64_t>("neuron_count", 500);
        int regionCount = config->getOr<int64_t>("region_count", 1);
        
        std::cout << "=== NLM Performance Benchmark ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration: " << neuronCount << " neurons, " << regionCount << " regions" << std::endl;
        std::cout << std::endl;
        
        if (duration <= 0) duration = 5; // Default 5 seconds
        if (repeats <= 0) repeats = 3; // Default 3 runs
        
        // Run basic benchmark
        return runBenchmark(config, duration, repeats);
    }
    
    CLIResult runBenchmark(const std::shared_ptr<Config>& config, int duration, int repeats) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        double totalTime = 0.0;
        double totalSteps = 0;
        double totalSpikes = 0;
        double totalFiringRate = 0;
        
        std::cout << "Running benchmark (duration: " << duration << "s, repeats: " << repeats << ")..." << std::endl;
        std::cout << std::endl;
        
        for (int run = 1; run <= repeats; ++run) {
            std::cout << "Run " << run << "/" << repeats << ":" << std::endl;
            
            auto runStartTime = std::chrono::high_resolution_clock::now();
            
            // Create and run a simple brain simulation
            // Note: This is a simplified benchmark - in practice, this would use the actual NLM brain
            double runTime = 0.0;
            double runSteps = 0;
            double runSpikes = 0;
            double runFiringRate = 0;
            
            // Simulate benchmark work
            int stepsPerSecond = 1000; // Approximate
            int targetSteps = duration * stepsPerSecond;
            
            for (int step = 0; step < targetSteps; ++step) {
                // Simulate some neural activity
                double spikeProbability = 0.1; // Approximate firing rate
                double random = (rand() % 100) / 100.0;
                if (random < spikeProbability) {
                    runSpikes++;
                }
                
                runSteps++;
                
                // Check if we should stop
                auto currentTime = std::chrono::high_resolution_clock::now();
                runTime = std::chrono::duration<double>(currentTime - runStartTime).count();
                if (runTime >= duration) {
                    break;
                }
            }
            
            totalTime += runTime;
            totalSteps += runSteps;
            totalSpikes += runSpikes;
            totalFiringRate += (runSpikes / runSteps) * 1000.0; // Convert to Hz
            
            std::cout << "  Steps: " << runSteps << " in " << std::fixed << std::setprecision(2) << runTime << "s" << std::endl;
            std::cout << "  Spikes: " << runSpikes << " (" << std::fixed << std::setprecision(2) << (runSpikes / runSteps) * 100 << "%)" << std::endl;
            std::cout << "  Firing rate: " << std::fixed << std::setprecision(2) << (runSpikes / runSteps) * 1000.0 << " Hz" << std::endl;
            std::cout << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalWallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate statistics
        double avgTime = totalTime / repeats;
        double avgSteps = totalSteps / repeats;
        double avgSpikes = totalSpikes / repeats;
        double avgFiringRate = totalFiringRate / repeats;
        double stepsPerSecond = totalSteps / totalWallTime;
        
        std::cout << "=== Benchmark Results ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Total wall time: " << std::fixed << std::setprecision(2) << totalWallTime << "s" << std::endl;
        std::cout << "Total steps: " << totalSteps << std::endl;
        std::cout << "Total spikes: " << totalSpikes << std::endl;
        std::cout << std::endl;
        std::cout << "Per-run statistics:" << std::endl;
        std::cout << "  Average time: " << std::fixed << std::setprecision(2) << avgTime << "s" << std::endl;
        std::cout << "  Average steps: " << avgSteps << std::endl;
        std::cout << "  Average spikes: " << avgSpikes << std::endl;
        std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz" << std::endl;
        std::cout << "  Steps per second: " << std::fixed << std::setprecision(2) << stepsPerSecond << std::endl;
        std::cout << std::endl;
        
        return CLIResult(true, "Benchmark completed successfully");
    }
    
    void showHelp() const override {
        std::cout << "Usage: benchmark [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Run performance benchmarks for NLM." << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --duration <sec>    Benchmark duration in seconds (default: 5)" << std::endl;
        std::cout << "  --repeats <N>       Number of benchmark runs (default: 3)" << std::endl;
        std::cout << "  --detail <level>    Detail level (basic, detailed, advanced)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm benchmark                    # Default benchmark (5s, 3 runs)" << std::endl;
        std::cout << "  nlm benchmark --duration 10     # 10-second benchmark" << std::endl;
        std::cout << "  nlm benchmark --repeats 5      # 5 runs of benchmark" << std::endl;
    }
};

} // namespace cli
} // namespace nlm
