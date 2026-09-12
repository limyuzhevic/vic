/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected. Enhanced with
 * command-line utilities for exploration, testing, and analysis.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Phase 6 Integration Demo and Analysis Tool" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --quick, -q           Run quick integration verification only" << std::endl;
    std::cout << "  --full, -f            Run full integration experiment (default)" << std::endl;
    std::cout << "  --neurons N           Set neuron count (default: 500)" << std::endl;
    std::cout << "  --steps S             Set simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint PATH     Enable checkpointing with path" << std::endl;
    std::cout << "  --no-checkpoint       Disable checkpointing" << std::endl;
    std::cout << "  --no-replay           Disable replay system" << std::endl;
    std::cout << "  --no-development      Disable development" << std::endl;
    std::cout << "  --benchmark           Run benchmark comparison" << std::endl;
    std::cout << "  --output FILE         Save results to file" << std::endl;
    std::cout << "  --verbose, -v         Enable verbose output" << std::endl;
    std::cout << "  --help, -h            Show this help message" << std::endl;
    std::cout << std::endl;
}

void printExperimentBanner() {
    std::cout << "================================================================================\n";
    std::cout << "                       NLM Phase 6 INTEGRATION DEMONSTRATION                        \n";
    std::cout << "================================================================================\n";
    std::cout << "\nThis demo demonstrates the integration of all NLM brain systems:\n";
    std::cout << "  • Working Memory integration with neural processing\n";
    std::cout << "  • Episodic Memory consolidation and retrieval\n";
    std::cout << "  • Neuromodulation (dopamine, curiosity, novelty)\n";
    std::cout << "  • Prediction system integration\n";
    std::cout << "  • Developmental plasticity mechanisms\n";
    std::cout << "  • Checkpoint save/load functionality\n";
    std::cout << "  • Replay and memory consolidation\n";
    std::cout << "================================================================================\n\n";
}

void printPerformanceMetrics(const Phase6IntegrationResult& result) {
    std::cout << "\n--- Performance Metrics ---\n";
    std::cout << "System Integration Score: ";
    int integrationScore = 0;
    if (result.memoryWorkingMemoryIntegrated) integrationScore++;
    if (result.memoryEpisodicMemoryIntegrated) integrationScore++;
    if (result.neuromodulationIntegrated) integrationScore++;
    if (result.predictionIntegrated) integrationScore++;
    if (result.developmentIntegrated) integrationScore++;
    if (result.checkpointingWorks) integrationScore++;
    std::cout << integrationScore << "/7 systems integrated\n";
    
    std::cout << "Activity Metrics:" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  • Total Reward: " << result.totalReward << std::endl;
    std::cout << "  • Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  • Memory Episodes: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  • Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "  • Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "  • Curiosity Level: " << result.curiosityLevel << std::endl;
}

void saveResultsToFile(const Phase6IntegrationResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "Phase 6 Integration Results\n";
    file << "==========================\n\n";
    
    file << "System Integration:\n";
    file << "  Memory Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << "\n";
    file << "  Memory Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << "\n";
    file << "  Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "true" : "false") << "\n";
    file << "  Prediction Integrated: " << (result.predictionIntegrated ? "true" : "false") << "\n";
    file << "  Development Integrated: " << (result.developmentIntegrated ? "true" : "false") << "\n";
    file << "  Checkpointing Works: " << (result.checkpointingWorks ? "true" : "false") << "\n";
    file << "  Replay Works: " << (result.replayWorks ? "true" : "false") << "\n\n";
    
    file << "Performance Metrics:\n";
    file << "  Total Reward: " << result.totalReward << "\n";
    file << "  Average Firing Rate: " << result.avgFiringRate << "\n";
    file << "  Average Synaptic Weight: " << result.avgSynapticWeight << "\n";
    file << "  Memory Episodes Stored: " << result.memoryEpisodesStored << "\n";
    file << "  Novelty Level: " << result.noveltyLevel << "\n";
    file << "  Curiosity Level: " << result.curiosityLevel << "\n";
    file << "  Dopamine Level: " << result.dopamineLevel << "\n\n";
    
    file << "Timing:\n";
    file << "  Start Time: " << result.startTime << "\n";
    file << "  End Time: " << result.endTime << "\n";
    file << "  Total Wall Clock Time: " << result.totalWallClockTime << "s\n";
    
    file.close();
    std::cout << "Results saved to: " << filename << std::endl;
}

void runBenchmark(const Phase6Config& config, const Phase6Config& benchmarkConfig) {
    std::cout << "\n--- Benchmarking ---\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Phase6IntegratedExperiment experiment;
    auto result = experiment.run(config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "Baseline configuration performance:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Time: " << elapsed << "s" << std::endl;
    std::cout << "  Reward per step: " << (result.totalReward / config.maxSteps) << std::endl;
    
    // Compare with benchmark configuration
    auto benchStart = std::chrono::high_resolution_clock::now();
    auto benchResult = experiment.run(benchmarkConfig);
    auto benchEnd = std::chrono::high_resolution_clock::now();
    double benchElapsed = std::chrono::duration<double>(benchEnd - benchStart).count();
    
    std::cout << "\nBenchmark configuration performance:" << std::endl;
    std::cout << "  Neurons: " << benchmarkConfig.neuronCount << std::endl;
    std::cout << "  Steps: " << benchmarkConfig.maxSteps << std::endl;
    std::cout << "  Time: " << benchElapsed << "s" << std::endl;
    std::cout << "  Reward per step: " << (benchResult.totalReward / benchmarkConfig.maxSteps) << std::endl;
    
    double speedup = elapsed / benchElapsed;
    double efficiency = (benchResult.totalReward / benchmarkConfig.maxSteps) / (result.totalReward / config.maxSteps);
    
    std::cout << "\nBenchmark Comparison:" << std::endl;
    std::cout << "  Speedup: " << speedup << "x" << std::endl;
    std::cout << "  Efficiency: " << efficiency << "x (reward per step)" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string outputFile;
    bool quickMode = false;
    bool fullMode = true;
    bool benchmarkMode = false;
    bool verboseMode = false;
    
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--quick" || arg == "-q") {
            quickMode = true;
            fullMode = false;
            maxSteps = 100;
        }
        else if (arg == "--full" || arg == "-f") {
            quickMode = false;
            fullMode = true;
        }
        else if (arg == "--benchmark") {
            benchmarkMode = true;
        }
        else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        }
        else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoul(argv[++i]);
        }
        else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = std::stoul(argv[++i]);
        }
        else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath = argv[++i];
            enableCheckpointing = true;
        }
        else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        }
        else if (arg == "--no-replay") {
            enableReplay = false;
        }
        else if (arg == "--no-development") {
            enableDevelopment = false;
        }
        else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(verboseMode ? Logger::Level::Debug : Logger::Level::Info);
    
    // Print banner
    printExperimentBanner();
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Configure experiment
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.checkpointPath = checkpointPath;
    
    if (verboseMode) {
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Max Steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    }
    
    if (quickMode) {
        std::cout << "=== QUICK INTEGRATION TEST ===\n";
        bool integrationOK = experiment.verifyIntegration();
        
        if (integrationOK) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
            return 1;
        }
    } else {
        if (fullMode) {
            std::cout << "=== FULL INTEGRATION EXPERIMENT ===\n";
            
            // Test individual systems
            if (verboseMode) std::cout << "\n--- Testing Individual Systems ---" << std::endl;
            
            experiment.testMemoryIntegration();
            experiment.testNeuromodulationIntegration();
            experiment.testCheckpointing();
            experiment.testReplay();
            
            // Run full experiment
            if (verboseMode) std::cout << "\n--- Running Full Integration Experiment ---" << std::endl;
            auto result = experiment.run(config);
            
            // Display results
            std::cout << "\n=== FINAL RESULTS ===" << std::endl;
            std::cout << "Total Reward: " << result.totalReward << std::endl;
            std::cout << "Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
            std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
            
            // Display integration status
            std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
            
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            
            // Print performance metrics
            printPerformanceMetrics(result);
            
            // Save results to file if requested
            if (!outputFile.empty()) {
                saveResultsToFile(result, outputFile);
            }
        }
    }
    
    // Run benchmark if requested
    if (benchmarkMode) {
        Phase6Config benchmarkConfig;
        benchmarkConfig.neuronCount = 1000;
        benchmarkConfig.maxSteps = 1000;
        benchmarkConfig.enableCheckpointing = false;
        benchmarkConfig.enableReplay = false;
        benchmarkConfig.enableDevelopment = false;
        
        runBenchmark(config, benchmarkConfig);
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Phase 6 Demonstration Complete!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
