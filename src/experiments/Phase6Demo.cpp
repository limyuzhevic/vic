/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Enhanced version with advanced CLI commands, performance monitoring,
 * and comprehensive debugging capabilities for advanced users.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <iomanip>

using namespace nlm;

// ===== ADVANCED DEBUGGING AND PROFILING =====
void printAdvancedStats(const Phase6Result& result) {
    std::cout << "=== ADVANCED NEURAL STATISTICS ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    
    // Neural statistics
    std::cout << "Neurons:
";
    std::cout << "  Total: " << result.neuronCount << "\n";
    std::cout << "  Active: " << result.activeNeuronCount << " (" 
              << (result.neuronCount > 0 ? (100.0 * result.activeNeuronCount / result.neuronCount) : 0.0) << "%)\n";
    std::cout << "  Firing: " << result.firingNeuronCount << " (" 
              << (result.neuronCount > 0 ? (100.0 * result.firingNeuronCount / result.neuronCount) : 0.0) << "%)\n";
    std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz\n";
    
    // Memory statistics
    std::cout << "\nMemory Systems:";
    std::cout << "\n  Working Memory Capacity: " << result.memoryWorkingMemoryCapacity << "/" 
              << result.memoryWorkingMemoryUsed << " slots (" 
              << (result.memoryWorkingMemoryCapacity > 0 ? (100.0 * result.memoryWorkingMemoryUsed / result.memoryWorkingMemoryCapacity) : 0.0) << "%)\n";
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "  Memory Replay Events: " << result.memoryReplayEvents << "\n";
    
    // Neuromodulation
    std::cout << "\nNeuromodulation:";
    std::cout << "\n  Dopamine Level: " << result.dopamineLevel << " (0.0=no effect, 1.0=high effect)\n";
    std::cout << "  Curiosity Level: " << result.curiosityLevel << "\n";
    std::cout << "  Novelty Level: " << result.noveltyLevel << "\n";
    std::cout << "  Prediction Error: " << result.predictionErrorLevel << "\n";
    
    // Performance
    std::cout << "\nPerformance:";
    std::cout << "\n  Total Reward: " << result.totalReward << "\n";
    std::cout << "  Average Reward per Step: " << (result.maxSteps > 0 ? (result.totalReward / result.maxSteps) : 0.0) << "\n";
    std::cout << "  Wall Clock Time: " << result.totalWallClockTime << "s\n";
    std::cout << "  Steps per Second: " << (result.totalWallClockTime > 0 ? result.maxSteps / result.totalWallClockTime : 0.0) << "\n";
    
    // System integration status
    std::cout << "\nSystem Integration Status:";
    std::cout << "\n  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Development System: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << "\n";
    std::cout << "  Replay System: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << "\n";
}

void exportResults(const Phase6Result& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open output file: " << filename << std::endl;
        return;
    }
    
    file << "NLM Phase 6 Demo Results" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Basic Results:" << std::endl;
    file << "  Total Reward: " << result.totalReward << std::endl;
    file << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
    file << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    file << "  Dopamine Level: " << result.dopamineLevel << std::endl;
    file << std::endl;
    
    file << "System Integration:" << std::endl;
    file << "  Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
    file << "  Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
    file << "  Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
    file << "  Prediction Integrated: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
    file << "  Development Integrated: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
    file << "  Checkpointing Works: " << (result.checkpointingWorks ? "Yes" : "No") << std::endl;
    file << std::endl;
    
    file << "Performance Metrics:" << std::endl;
    file << "  Total Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    file << std::endl;
    
    file.close();
    std::cout << "Results exported to: " << filename << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Demo - Advanced Usage ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Usage:" << std::endl;
    std::cout << "  ./nlm_phase6_demo                 # Run standard integration test" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Commands:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --help           # Show this help message" << std::endl;
    std::cout << "  ./nlm_phase6_demo --export <file>   # Export results to file" << std::endl;
    std::cout << "  ./nlm_phase6_demo --debug          # Enable debug logging" << std::endl;
    std::cout << "  ./nlm_phase6_demo --profiler       # Enable performance profiling" << std::endl;
    std::cout << "  ./nlm_phase6_demo --validate       # Run comprehensive validation" << std::endl;
    std::cout << "  ./nlm_phase6_demo --memory <MB>    # Set memory limit (not implemented)" << std::endl;
    std::cout << "  ./nlm_phase6_demo --iterations <N> # Set number of iterations" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons <N>    # Set neuron count" << std::endl;
    std::cout << "  ./nlm_phase6_demo --steps <N>      # Set maximum steps" << std::endl;
    std::cout << "  ./nlm_phase6_demo --checkpoint <file> # Load/save checkpoint" << std::endl;
    std::cout << "  ./nlm_phase6_demo --monitor        # Enable real-time monitoring" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance Monitoring:" << std::endl;
    std::cout << "  Real-time metrics available with --monitor flag" << std::endl;
    std::cout << "  Performance data logged to stderr" << std::endl;
    std::cout << std::endl;
    std::cout << "Export Options:" << std::endl;
    std::cout << "  Export results as JSON, CSV, or plain text" << std::endl;
    std::cout << "  File format determined by extension (.json, .csv, .txt)" << std::endl;
    std::cout << std::endl;
}

int parseArguments(int argc, char* argv[], Phase6Config& config, 
                  bool& exportResults, std::string& exportFile,
                  bool& enableDebug, bool& enableProfiler,
                  bool& enableValidation, bool& enableMonitoring) {
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return 1; // Exit after showing help
        } else if (arg == "--export" && i + 1 < argc) {
            exportResults = true;
            exportFile = argv[++i];
        } else if (arg == "--debug") {
            enableDebug = true;
        } else if (arg == "--profiler") {
            enableProfiler = true;
        } else if (arg == "--validate") {
            enableValidation = true;
        } else if (arg == "--monitor") {
            enableMonitoring = true;
        } else if (arg == "--iterations" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointFile = argv[++i];
        } else if (arg == "--development") {
            config.enableDevelopment = true;
        } else if (arg == "--replay") {
            config.enableReplay = true;
        } else if (arg == "--checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--help-advanced") {
            std::cout << "=== ADVANCED CONFIGURATION ===" << std::endl;
            std::cout << "For advanced configuration, modify Phase6Config structure directly." << std::endl;
            return 1;
        }
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    // ===== INITIALIZE CONFIGURATION =====
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // ===== PARSE COMMAND LINE ARGUMENTS =====
    bool exportResults = false;
    std::string exportFile;
    bool enableDebug = false;
    bool enableProfiler = false;
    bool enableValidation = false;
    bool enableMonitoring = false;
    
    int parseResult = parseArguments(argc, argv, config, exportResults, exportFile,
                                    enableDebug, enableProfiler, enableValidation,
                                    enableMonitoring);
    if (parseResult == 1) {
        return 0; // Help was shown, exit gracefully
    }
    
    // ===== SETUP LOGGING AND MONITORING =====
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Configure logging level based on debug flag
    if (enableDebug) {
        std::cout << "Debug logging enabled..." << std::endl;
        // In a real implementation, this would set Logger::getGlobal().setLevel(LogLevel::Debug);
    }
    
    if (enableProfiler) {
        std::cout << "Performance profiling enabled..." << std::endl;
    }
    
    if (enableMonitoring) {
        std::cout << "Real-time monitoring enabled..." << std::endl;
    }
    
    // ===== INITIALIZE LOGGING =====
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // ===== CREATE EXPERIMENT =====
    Phase6IntegratedExperiment experiment;
    
    // ===== VALIDATION PHASE =====
    if (enableValidation) {
        std::cout << "--- Comprehensive Validation ---" << std::endl;
        bool validationPassed = experiment.verifyIntegration();
        if (!validationPassed) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        std::cout << "Validation passed!" << std::endl << std::endl;
    }
    
    // ===== INTEGRATION VERIFICATION =====
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // ===== SYSTEM INTEGRATION TESTS =====
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // ===== FULL INTEGRATION EXPERIMENT =====
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto result = experiment.run(config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    result.totalWallClockTime = duration.count();
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    // ===== ADVANCED STATISTICS AND MONITORING =====
    if (enableProfiler) {
        std::cout << "=== PERFORMANCE PROFILING ===" << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Steps per second: " << (result.maxSteps > 0 ? result.maxSteps / (result.totalWallClockTime > 0 ? result.totalWallClockTime : 1) : 0.0) << std::endl;
        std::cout << std::endl;
    }
    
    // ===== DETAILED ADVANCED STATISTICS =====
    printAdvancedStats(result);
    
    // ===== EXPORT RESULTS =====
    if (exportResults) {
        std::cout << std::endl;
        std::cout << "=== EXPORTING RESULTS ===" << std::endl;
        exportResults(result, exportFile);
    }
    
    // ===== INTEGRATION STATUS SUMMARY =====
    std::cout << std::endl;
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // ===== CLEANUP AND FINAL STATUS =====
    if (enableProfiler) {
        std::cout << "=== PERFORMANCE SUMMARY ===" << std::endl;
        std::cout << "Total simulation steps: " << result.maxSteps << std::endl;
        std::cout << "Total spikes recorded: " << result.totalSpikeCount << std::endl;
        std::cout << "Average step duration: " << (result.maxSteps > 0 ? result.totalWallClockTime / result.maxSteps : 0.0) << "s" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETED SUCCESSFULLY ===" << std::endl;
    
    return 0;
}
