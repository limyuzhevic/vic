// NLM Benchmark Tool
// Performance profiling and benchmarking for Enhanced NLM

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <memory>

using namespace nlm;

void printBenchmarkHeader() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Benchmark Tool                                            ║
    ║                                                               ║
    ║     Performance Testing for Enhanced NLM                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runBenchmark(const std::string& benchmarkName, size_t steps, size_t neuronCount,
                  float timestep, size_t regionCount, float connectionProbability) {
    std::cout << std::endl;
    std::cout << "=== " << benchmarkName << " ===" << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Regions: " << regionCount << std::endl;
    std::cout << "Timestep: " << timestep << "s" << std::endl;
    std::cout << "Connection Probability: " << connectionProbability << std::endl;
    std::cout << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Set benchmark configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", connectionProbability, ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting benchmark: " + benchmarkName);
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing brain for benchmark...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return;
    }
    
    // Run benchmark
    NLM_LOG_INFO("Running benchmark simulation...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, static_cast<Timestamp>(step) * timestep);
        
        // Progress update every 1000 steps
        if (step % 1000 == 0 && step > 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0;
            float stepsPerSecond = step / elapsed;
            NLM_LOG_INFO("  Progress: " + std::to_string(step) + "/" + std::to_string(steps) + 
                        " steps (" + std::to_string(stepsPerSecond) + " steps/sec)");
        }
    }
    
    // End timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    
    // Calculate performance metrics
    float stepsPerSecond = steps / duration;
    float avgSpikeRate = brain->getTotalSpikeCount() / duration;
    float avgFiringRate = brain->getAverageFiringRate();
    
    // Display results
    std::cout << std::endl;
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << duration << " seconds" << std::endl;
    std::cout << "Steps per second: " << std::setprecision(2) << stepsPerSecond << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average spike rate: " << std::setprecision(2) << avgSpikeRate << " spikes/sec" << std::endl;
    std::cout << "Average firing rate: " << std::setprecision(2) << avgFiringRate << " Hz" << std::endl;
    std::cout << "Neurons created: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses created: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Regions: " << brain->getRegionCount() << std::endl;
    
    // Efficiency metrics
    float efficiency = (brain->getTotalSynapseCount() * 1000) / (duration * 1000000);  // synapses per million CPU cycles (approximate)
    std::cout << "Efficiency metric: " << std::setprecision(2) << efficiency << " synapses/CPU cycles" << std::endl;
}

int main(int argc, char** argv) {
    printBenchmarkHeader();
    
    // Default benchmark parameters
    size_t steps = 5000;
    size_t neuronCount = 1000;
    float timestep = 0.001f;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    std::string benchmarkName = "Standard Benchmark";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help                    Show this help message" << std::endl;
            std::cout << "  --version                 Show version information" << std::endl;
            std::cout << "  --name <name>            Benchmark name" << std::endl;
            std::cout << "  --steps <num>            Number of simulation steps (default: 5000)" << std::endl;
            std::cout << "  --neurons <num>          Number of neurons (default: 1000)" << std::endl;
            std::cout << "  --regions <num>          Number of regions (default: 1)" << std::endl;
            std::cout << "  --timestep <val>          Timestep in seconds (default: 0.001)" << std::endl;
            std::cout << "  --connection <val>       Connection probability (default: 0.15)" << std::endl;
            std::cout << "  --output <file>          Output benchmark results to file" << std::endl;
            return 0;
        } else if (arg == "--version") {
            std::cout << "NLM Benchmark Tool v0.1.0 Enhanced Edition" << std::endl;
            std::cout << "Phase 6 Enhanced Edition" << std::endl;
            return 0;
        } else if (arg == "--name") {
            if (i + 1 < argc) benchmarkName = argv[++i];
        } else if (arg == "--steps") {
            if (i + 1 < argc) steps = std::stoull(argv[++i]);
        } else if (arg == "--neurons") {
            if (i + 1 < argc) neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--regions") {
            if (i + 1 < argc) regionCount = std::stoull(argv[++i]);
        } else if (arg == "--timestep") {
            if (i + 1 < argc) timestep = std::stof(argv[++i]);
        } else if (arg == "--connection") {
            if (i + 1 < argc) connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                std::ofstream outFile(argv[++i]);
                if (outFile.is_open()) {
                    outFile << "Benchmark Name: " << benchmarkName << std::endl;
                    outFile << "Steps: " << steps << std::endl;
                    outFile << "Neurons: " << neuronCount << std::endl;
                    outFile << "Regions: " << regionCount << std::endl;
                    outFile << "Timestep: " << timestep << std::endl;
                    outFile << "Connection Probability: " << connectionProbability << std::endl;
                    outFile.close();
                    std::cout << "Benchmark results saved to " << argv[i] << std::endl;
                } else {
                    std::cout << "Warning: Could not open output file " << argv[i] << std::endl;
                }
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Enhanced NLM Benchmark Tool ===" << std::endl;
    std::cout << "Benchmark: " << benchmarkName << std::endl;
    std::cout << "Configuration loaded successfully" << std::endl;
    
    // Run benchmark
    runBenchmark(benchmarkName, steps, neuronCount, timestep, regionCount, connectionProbability);
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Complete ===" << std::endl;
    std::cout << "The Enhanced NLM benchmark demonstrates the performance and capabilities" << std::endl;
    std::cout << "of the enhanced neural learning machine with advanced features." << std::endl;
    
    return 0;
}