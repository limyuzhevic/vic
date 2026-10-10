// NLM Batch Processing Tool
// Enables running multiple simulations in batch mode

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

using namespace nlm;

void printBatchBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                   NLM Batch Processing Tool                     ║
║                                                               ║
║  Run multiple simulations efficiently with batch processing   ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

class NLMBatchProcessor {
public:
    struct BatchConfig {
        std::string name;
        std::string configFile;
        std::string outputDir;
        int numSimulations;
        int stepsPerSimulation;
        bool saveEach; // Save each simulation individually
        bool parallel;  // Run simulations in parallel
        std::vector<std::string> extraArgs;
    };

    struct BatchResult {
        int simulationIndex;
        std::string configName;
        double simulationTime; // Real time
        size_t stepsExecuted;
        size_t totalSpikes;
        size_t activeNeurons;
        double learningRate;
        std::string status; // "SUCCESS", "ERROR", "TIMEOUT"
        std::string errorMessage;
        std::string outputFile; // Where results were saved
    };

    NLMBatchProcessor() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    BatchResult runSimulation(int index, const BatchConfig& config) {
        BatchResult result;
        result.simulationIndex = index;
        result.configName = config.name;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Load configuration
            auto cfg = std::make_shared<Config>();
            if (!config.configFile.empty() && cfg->loadFromFile(config.configFile)) {
                NLM_LOG_INFO("Batch " + std::to_string(index) + ": Loaded config from " + config.configFile);
            } else {
                cfg = std::make_shared<Config>();
                NLM_LOG_INFO("Batch " + std::to_string(index) + ": Using default config");
            }
            
            // Set batch-specific parameters
            cfg->set("random_seed", static_cast<int64_t>(42 + index), ConfigSource::Default);
            cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
            
            // Create brain
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            // Create world if needed
            std::shared_ptr<SimpleWorld> world;
            std::shared_ptr<AgentBrain> agent;
            
            // Run simulation
            for (int step = 0; step < config.stepsPerSimulation; ++step) {
                brain->step(step);
                
                // If agent/world is configured, run that too
                // (In real implementation, would check if world was configured)
            }
            
            // Calculate results
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            result.simulationTime = duration.count() / 1000.0; // Convert to seconds
            result.stepsExecuted = config.stepsPerSimulation;
            result.totalSpikes = brain->getTotalSpikeCount();
            result.activeNeurons = brain->getActiveNeuronCount();
            result.learningRate = calculateLearningRate(brain);
            result.status = "SUCCESS";
            result.outputFile = config.outputDir + "/batch_" + std::to_string(index) + ".state";
            
            // Save if configured
            if (config.saveEach) {
                brain->save(result.outputFile);
            }
            
            NLM_LOG_INFO("Batch " + std::to_string(index) + ": Completed in " + 
                        std::to_string(result.simulationTime) + "s, " +
                        std::to_string(result.totalSpikes) + " spikes");
                
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            result.simulationTime = duration.count() / 1000.0;
            result.status = "ERROR";
            result.errorMessage = std::string("Exception: ") + e.what();
            
            NLM_LOG_ERROR("Batch " + std::to_string(index) + ": " + result.errorMessage);
        }
        
        return result;
    }

    std::vector<BatchResult> runBatch(const BatchConfig& config) {
        std::vector<BatchResult> results;
        
        NLM_LOG_INFO("Starting batch: " + config.name + " with " + 
                    std::to_string(config.numSimulations) + " simulations");
        
        if (config.parallel && config.numSimulations > 1) {
            // Run in parallel
            results.resize(config.numSimulations);
            std::vector<std::thread> threads;
            
            for (int i = 0; i < config.numSimulations; ++i) {
                threads.emplace_back([&, i]() {
                    results[i] = runSimulation(i, config);
                });
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
        } else {
            // Run sequentially
            for (int i = 0; i < config.numSimulations; ++i) {
                results.push_back(runSimulation(i, config));
            }
        }
        
        return results;
    }

    void exportResults(const std::vector<BatchResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Batch Processing Results" << std::endl;
        file << "===============================" << std::endl;
        file << std::endl;
        
        file << "Summary:" << std::endl;
        size_t successful = 0;
        size_t failed = 0;
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        
        for (const auto& result : results) {
            if (result.status == "SUCCESS") {
                successful++;
                totalTime += result.simulationTime;
                totalSpikes += result.totalSpikes;
            } else {
                failed++;
            }
        }
        
        file << "  Total Simulations: " << results.size() << std::endl;
        file << "  Successful: " << successful << std::endl;
        file << "  Failed: " << failed << std::endl;
        file << "  Average Time per Simulation: " << 
            (results.empty() ? 0.0 : totalTime / results.size()) << "s" << std::endl;
        file << "  Total Spikes: " << totalSpikes << std::endl;
        file << std::endl;
        
        file << "Detailed Results:" << std::endl;
        file << "Index\tConfig\t\tTime(s)\tSteps\tSpikes\tLearning\tStatus" << std::endl;
        file << "----\t------\t\t------\t------\t------\t-------\t------" << std::endl;
        
        for (const auto& result : results) {
            file << result.simulationIndex << "\t"
                 << result.configName << "\t\t"
                 << std::fixed << std::setprecision(3) << result.simulationTime << "\t"
                 << result.stepsExecuted << "\t"
                 << result.totalSpikes << "\t"
                 << std::fixed << std::setprecision(3) << result.learningRate << "\t\t"
                 << result.status << std::endl;
        }
        
        if (failed > 0) {
            file << std::endl;
            file << "Failed Simulations:" << std::endl;
            for (const auto& result : results) {
                if (result.status == "ERROR") {
                    file << "  Simulation " << result.simulationIndex << ": " 
                         << result.errorMessage << std::endl;
                }
            }
        }
        
        file.close();
        
        NLM_LOG_INFO("Batch results exported to " + filename);
    }

    double calculateLearningRate(std::shared_ptr<Brain> brain) {
        // Simplified learning rate calculation
        size_t synapses = brain->getTotalSynapseCount();
        double spikeRate = brain->getAverageFiringRate();
        
        return spikeRate * (synapses / 1000.0);
    }

    void printBatchReport(const std::vector<BatchResult>& results) {
        std::cout << "=== NLM Batch Processing Report ===" << std::endl;
        std::cout << std::endl;
        
        size_t successful = 0;
        size_t failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.status == "SUCCESS") {
                successful++;
                totalTime += result.simulationTime;
            } else {
                failed++;
            }
        }
        
        std::cout << "Batch Summary:" << std::endl;
        std::cout << "  Total Simulations: " << results.size() << std::endl;
        std::cout << "  Successful: " << successful << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Average Time: " << (results.empty() ? 0.0 : totalTime / results.size()) << "s" << std::endl;
        std::cout << std::endl;
        
        if (successful > 0) {
            std::cout << "Performance (Successful Runs):" << std::endl;
            double minTime = std::numeric_limits<double>::max();
            double maxTime = 0.0;
            size_t totalSpikes = 0;
            
            for (const auto& result : results) {
                if (result.status == "SUCCESS") {
                    minTime = std::min(minTime, result.simulationTime);
                    maxTime = std::max(maxTime, result.simulationTime);
                    totalSpikes += result.totalSpikes;
                }
            }
            
            std::cout << "  Time Range: " << minTime << "s - " << maxTime << "s" << std::endl;
            std::cout << "  Average Spikes: " << (successful > 0 ? totalSpikes / successful : 0) << std::endl;
        }
        
        if (failed > 0) {
            std::cout << std::endl;
            std::cout << "Failed Runs:" << std::endl;
            for (const auto& result : results) {
                if (result.status == "ERROR") {
                    std::cout << "  Simulation " << result.simulationIndex << ": " 
                              << result.errorMessage << std::endl;
                }
            }
        }
        
        std::cout << std::endl;
    }
};

int main(int argc, char** argv) {
    printBatchBanner();
    
    if (argc < 2) {
        std::cout << "Usage: nlm_batch <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run <config_file> <name> <num_sim> <steps> [options]" << std::endl;
        std::cout << "  quick <num_sim> <steps>                      Quick batch test" << std::endl;
        std::cout << "  parallel <config_file> <name> <num_sim> <steps> Run in parallel" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --output DIR      Output directory" << std::endl;
        std::cout << "  --save-each        Save each simulation individually" << std::endl;
        std::cout << "  --seed SEED        Random seed base" << std::endl;
        std::cout << std::endl;
        return 0;
    }

    std::string command(argv[1]);
    
    if (command == "run" || command == "quick") {
        if (argc < 5) {
            std::cout << "Error: Missing arguments for batch run" << std::endl;
            return 1;
        }
        
        std::string configFile = (command == "run" && argc > 5) ? argv[5] : "";
        std::string name = argv[2];
        int numSim = std::stoi(argv[3]);
        int steps = std::stoi(argv[4]);
        
        // Parse options
        std::string outputDir = "results";
        bool saveEach = false;
        int seedBase = 42;
        
        for (int i = 5; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--output" && i + 1 < argc) {
                outputDir = argv[++i];
            } else if (arg == "--save-each") {
                saveEach = true;
            } else if (arg == "--seed" && i + 1 < argc) {
                seedBase = std::stoi(argv[++i]);
            }
        }
        
        NLM_LOG_INFO("Starting batch: " + name + ", " + std::to_string(numSim) + " simulations, " + 
                    std::to_string(steps) + " steps each");
        
        // Create batch config
        NLMBatchProcessor::BatchConfig config;
        config.name = name;
        config.configFile = configFile;
        config.outputDir = outputDir;
        config.numSimulations = numSim;
        config.stepsPerSimulation = steps;
        config.saveEach = saveEach;
        config.parallel = false;
        
        // Run batch
        NLMBatchProcessor processor;
        auto results = processor.runBatch(config);
        
        // Export results
        std::string resultsFile = outputDir + "/batch_results.txt";
        processor.exportResults(results, resultsFile);
        
        // Print report
        processor.printBatchReport(results);
        
    } else if (command == "parallel") {
        if (argc < 6) {
            std::cout << "Error: Missing arguments for parallel batch" << std::endl;
            return 1;
        }
        
        std::string configFile = argv[5];
        std::string name = argv[2];
        int numSim = std::stoi(argv[3]);
        int steps = std::stoi(argv[4]);
        
        // Parse options (same as above)
        std::string outputDir = "results";
        bool saveEach = false;
        
        for (int i = 5; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--output" && i + 1 < argc) {
                outputDir = argv[++i];
            } else if (arg == "--save-each") {
                saveEach = true;
            }
        }
        
        NLM_LOG_INFO("Starting parallel batch: " + name + ", " + std::to_string(numSim) + " simulations");
        
        // Create batch config
        NLMBatchProcessor::BatchConfig config;
        config.name = name;
        config.configFile = configFile;
        config.outputDir = outputDir;
        config.numSimulations = numSim;
        config.stepsPerSimulation = steps;
        config.saveEach = saveEach;
        config.parallel = true;
        
        // Run batch
        NLMBatchProcessor processor;
        auto results = processor.runBatch(config);
        
        // Export results
        std::string resultsFile = outputDir + "/parallel_results.txt";
        processor.exportResults(results, resultsFile);
        
        // Print report
        processor.printBatchReport(results);
        
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }
    
    return 0;
}
