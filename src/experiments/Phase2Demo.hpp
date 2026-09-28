#include "experiments/Phase2Config.hpp"
#include "experiments/Phase2Demo.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 2 Enhanced Demo ===" << std::endl;
    std::cout << "Testing Phase 2 neural computation with enhanced configuration..." << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    Phase2Config config;
    Phase2Results results;
    std::string configFile;
    std::string resultsFile;
    std::string experimentName;
    bool runAllTests = true;
    bool saveResults = false;
    
    // Simple command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printPhase2Help(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printPhase2Version();
            return 0;
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.find("--output=") == 0) {
            resultsFile = arg.substr(9);
            saveResults = true;
        } else if (arg == "--output" && i + 1 < argc) {
            resultsFile = argv[++i];
            saveResults = true;
        } else if (arg.find("--name=") == 0) {
            experimentName = arg.substr(7);
        } else if (arg == "--name" && i + 1 < argc) {
            experimentName = argv[++i];
        } else if (arg == "--quick") {
            runAllTests = false; // Just run integration tests
        } else if (arg == "--comprehensive") {
            runAllTests = true; // Run all tests
        }
    }
    
    // Load configuration if specified
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        if (!config.loadFromFile(configFile)) {
            std::cerr << "ERROR: Failed to load configuration file: " << configFile << std::endl;
            std::cerr << "Using default configuration." << std::endl;
        }
    } else {
        std::cout << "Using default Phase 2 configuration." << std::endl;
    }
    
    if (!experimentName.empty()) {
        config.experimentName = experimentName;
        results.observations.push_back("Experiment name set to: " + experimentName);
    }
    
    // Set start time
    auto startTime = std::chrono::system_clock::now();
    config.startTime = startTime;
    
    // Create experiment runner
    Phase2ExperimentRunner runner;
    
    std::cout << "\n=== Running Phase 2 Enhanced Experiment ===" << std::endl;
    std::cout << "Experiment: " << config.experimentName << std::endl;
    std::cout << "Neurons: " << config.neuronCount << ", Regions: " << config.regionCount << std::endl;
    std::cout << "Plasticity Steps: " << config.plasticityTestSteps << std::endl;
    std::cout << "STDP Trials: " << config.stdpTestTrials << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Run the experiment
    bool success = runner.run(config, results);
    
    // Set end time
    auto endTime = std::chrono::system_clock::now();
    config.endTime = endTime;
    
    // Calculate wall clock time
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    results.totalWallClockTime = static_cast<double>(duration.count());
    
    std::cout << "\n=== EXPERIMENT COMPLETED ===" << std::endl;
    std::cout << "Success: " << (success ? "YES" : "NO") << std::endl;
    std::cout << "Wall clock time: " << results.totalWallClockTime << "s" << std::endl;
    std::cout << "Integration successful: " << (results.integrationSuccessful ? "YES" : "NO") << std::endl;
    
    // Print detailed results
    std::cout << "\n=== DETAILED RESULTS ===" << std::endl;
    printPhase2Results(results);
    
    // Save results if requested
    if (saveResults) {
        if (resultsFile.empty()) {
            resultsFile = config.experimentName + "_results.json";
            if (resultsFile.empty()) {
                resultsFile = "phase2_results.json";
            }
        }
        
        std::cout << "\nSaving results to: " << resultsFile << std::endl;
        if (runner.saveResultsToFile(results, resultsFile)) {
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "WARNING: Failed to save results to file." << std::endl;
        }
    }
    
    // Print configuration summary
    std::cout << "\n=== CONFIGURATION SUMMARY ===" << std::endl;
    std::cout << config.summary() << std::endl;
    
    return success ? 0 : 1;
}

void printPhase2Help(const std::string& programName) {
    std::cout << "NLM Phase 2 Enhanced Demo" << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show program version" << std::endl;
    std::cout << "  --config <file>           Load configuration from file" << std::endl;
    std::cout << "  --output <file>           Save results to file" << std::endl;
    std::cout << "  --name <name>             Set experiment name" << std::endl;
    std::cout << "  --quick                   Run quick integration tests only" << std::endl;
    std::cout << "  --comprehensive           Run all tests (default)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --quick" << std::endl;
    std::cout << "  " << programName << " --config my_config.cfg --output results.json" << std::endl;
    std::cout << "  " << programName << " --name my_experiment --comprehensive" << std::endl;
}

void printPhase2Version() {
    std::cout << "NLM Phase 2 Enhanced Demo v2.0" << std::endl;
    std::cout << "Enhanced neural computation with full configuration management" << std::endl;
    std::cout << "Supports experiment persistence and result storage" << std::endl;
}

void printPhase2Results(const Phase2Results& results) {
    std::cout << "\n=== Learning Metrics ===" << std::endl;
    std::cout << "Learning Score: " << results.learningScore << std::endl;
    std::cout << "Plasticity Score: " << results.plasticityScore << std::endl;
    std::cout << "STDP Score: " << results.stdpScore << std::endl;
    std::cout << "Memory Score: " << results.memoryScore << std::endl;
    std::cout << "Prediction Score: " << results.predictionScore << std::endl;
    
    std::cout << "\n=== System Integration ===" << std::endl;
    std::cout << "Integration Score: " << results.integrationScore << std::endl;
    std::cout << "Neuromodulation Integration: " << (results.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Development Integration: " << (results.developmentIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Cognition Integration: " << (results.cognitionScore > 0.0 ? "YES" : "NO") << std::endl;
    
    std::cout << "\n=== Memory System Performance ===" << std::endl;
    std::cout << "Working Memory Used: " << results.memoryWorkingMemoryUsed << "/" << 100 << std::endl;
    std::cout << "Episodic Memory Used: " << results.memoryEpisodicMemoryUsed << "/" << 1000 << std::endl;
    std::cout << "Associative Memory Used: " << results.memoryAssociativeMemoryUsed << "/" << 1000 << std::endl;
    std::cout << "Procedural Memory Used: " << results.memoryProceduralMemoryUsed << "/" << 100 << std::endl;
    
    std::cout << "\n=== Neuromodulation Levels ===" << std::endl;
    std::cout << "Dopamine Level: " << results.dopamineLevel << std::endl;
    std::cout << "Curiosity Level: " << results.curiosityLevel << std::endl;
    std::cout << "Novelty Level: " << results.noveltyLevel << std::endl;
    std::cout << "Prediction Error Level: " << results.predictionErrorLevel << std::endl;
    
    std::cout << "\n=== Cognitive Performance ===" << std::endl;
    std::cout << "Episodes Stored: " << results.episodesStored << std::endl;
    std::cout << "Concepts Formed: " << results.conceptsFormed << std::endl;
    std::cout << "Plans Generated: " << results.plansGenerated << std::endl;
    std::cout << "Actions Selected: " << results.actionsSelected << std::endl;
    
    std::cout << "\n=== Reward and Reinforcement ===" << std::endl;
    std::cout << "Total Reward: " << results.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << results.avgFiringRate << " Hz" << std::endl;
    std::cout << "Total Spikes: " << results.totalSpikes << std::endl;
    std::cout << "Firing Neuron Count: " << results.firingNeuronCount << "/" << results.totalNeurons << std::endl;
    
    std::cout << "\n=== System Health ===" << std::endl;
    std::cout << "Integration Successful: " << (results.integrationSuccessful ? "YES" : "NO") << std::endl;
    std::cout << "Working Memory Integrated: " << (results.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Episodic Memory Integrated: " << (results.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Associative Memory Integrated: " << (results.memoryAssociativeMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Neuromodulation Integrated: " << (results.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Prediction Integrated: " << (results.predictionIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Development Integrated: " << (results.developmentIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Checkpointing Works: " << (results.checkpointingWorks ? "YES" : "NO") << std::endl;
    std::cout << "Replay Works: " << (results.replayWorks ? "YES" : "NO") << std::endl;
    
    // Print warnings and errors
    if (!results.warnings.empty()) {
        std::cout << "\n=== WARNINGS ===" << std::endl;
        for (const auto& warning : results.warnings) {
            std::cout << "WARNING: " << warning << std::endl;
        }
    }
    
    if (!results.errors.empty()) {
        std::cout << "\n=== ERRORS ===" << std::endl;
        for (const auto& error : results.errors) {
            std::cout << "ERROR: " << error << std::endl;
        }
    }
}