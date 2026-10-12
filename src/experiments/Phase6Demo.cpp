/**
 * Enhanced Phase 6 Demo - Integration Test with Command-Line Interface
 * 
 * This demo runs the Phase 6 integration experiment with comprehensive
 * command-line arguments, progress reporting, and flexible configuration.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace nlm;

/**
 * Parse command-line arguments for Phase 6 demo
 */
struct DemoConfig {
    // Core configuration
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Features to enable/disable
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableLogging = true;
    bool verboseOutput = false;
    
    // Output and timing
    std::string checkpointPath = "phase6_checkpoint.bin";
    std::string outputFile = "phase6_results.txt";
    uint64_t logInterval = 100;
    uint64_t progressInterval = 500;
    
    // Display options
    bool showMemoryStats = true;
    bool showNeuromodulationStats = true;
    bool showDevelopmentStats = true;
    bool showPredictionStats = false;
    
    // Execution mode
    std::string mode = "full"; // "verify", "quick", "full"
    
    static DemoConfig parseCommandLine(int argc, char* argv[]);
};

DemoConfig DemoConfig::parseCommandLine(int argc, char* argv[]) {
    DemoConfig config;
    
    // Simple argument parsing (full implementation would use a proper library)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelpAndExit();
        }
        else if (arg == "--quick") {
            config.mode = "quick";
            config.maxSteps = 500;
            config.neuronCount = 200;
        }
        else if (arg == "--verify-only") {
            config.mode = "verify";
        }
        else if (arg == "--full") {
            config.mode = "full";
        }
        else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        }
        else if (arg == "--no-replay") {
            config.enableReplay = false;
        }
        else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
        else if (arg == "--no-logging") {
            config.enableLogging = false;
        }
        else if (arg == "--verbose" || arg == "-v") {
            config.verboseOutput = true;
            config.logInterval = 10;
        }
        else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        }
        else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoul(argv[++i]);
        }
        else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        }
        else if (arg == "--log-interval" && i + 1 < argc) {
            config.logInterval = std::stoul(argv[++i]);
        }
        else if (arg == "--progress-interval" && i + 1 < argc) {
            config.progressInterval = std::stoul(argv[++i]);
        }
        else if (arg.find("--") == 0) {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelpAndExit();
        }
    }
    
    return config;
}

void printHelpAndExit() {
    std::cout << "=== NLM Phase 6 Enhanced Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Mode options:" << std::endl;
    std::cout << "  --quick            Quick test (500 steps, 200 neurons)" << std::endl;
    std::cout << "  --verify-only      Only run integration verification" << std::endl;
    std::cout << "  --full             Full integration test (default)" << std::endl;
    std::cout << std::endl;
    std::cout << "Feature toggles:" << std::endl;
    std::cout << "  --no-checkpoint    Disable checkpointing" << std::endl;
    std::cout << "  --no-replay        Disable replay system" << std::endl;
    std::cout << "  --no-development   Disable development system" << std::endl;
    std::cout << "  --no-logging       Disable detailed logging" << std::endl;
    std::cout << "  --verbose, -v      Enable verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration options:" << std::endl;
    std::cout << "  --steps N          Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --neurons N        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions N        Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --checkpoint PATH   Checkpoint file path (default: phase6_checkpoint.bin)" << std::endl;
    std::cout << "  --output FILE      Output results file (default: phase6_results.txt)" << std::endl;
    std::cout << std::endl;
    std::cout << "Reporting options:" << std::endl;
    std::cout << "  --log-interval N   Log every N steps (default: 100)" << std::endl;
    std::cout << "  --progress-interval N  Progress update every N steps (default: 500)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick                    # Quick test" << std::endl;
    std::cout << "  ./nlm_phase6_demo --steps 5000 --neurons 1000  # Large simulation" << std::endl;
    std::cout << "  ./nlm_phase6_demo --verify-only             # Integration test only" << std::endl;
    std::cout << std::endl;
    exit(0);
}

/**
 * Format elapsed time for display
 */
std::string formatElapsedTime(double seconds) {
    std::ostringstream oss;
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
    int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    if (hours > 0) {
        oss << hours << "h " << minutes << "m " << secs << "s " << millisecs << "ms";
    } else if (minutes > 0) {
        oss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
    } else {
        oss << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
    }
    
    return oss.str();
}

/**
 * Print progress update
 */
void printProgress(const std::string& message, double progress, int total, uint64_t currentStep) {
    if (progress < 0.0 || progress > 1.0) return;
    
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r";
    std::cout << "[PROGRESS] ";
    
    if (currentStep > 0 && total > 0) {
        std::cout << "Step " << currentStep << "/" << total << " (" 
                  << static_cast<int>(progress * 100.0) << "%)";
    }
    
    std::cout << " |";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "| " << message;
    
    if (progress >= 1.0) {
        std::cout << std::endl;
    }
}

/**
 * Print system statistics
 */
void printSystemStats(const Phase6IntegrationResult& result, uint64_t totalSteps) {
    std::cout << std::endl;
    std::cout << "=== SYSTEM STATISTICS ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total Wall Clock Time: " << formatElapsedTime(result.totalWallClockTime) << std::endl;
    std::cout << "  Average Steps per Second: " << (totalSteps > 0 ? (totalSteps / std::max(1.0, result.totalWallClockTime)) : 0) << std::endl;
    std::cout << "  Total Reward: " << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Levels:" << std::endl;
    std::cout << "  Dopamine: " << result.dopamineLevel << std::endl;
    std::cout << "  Curiosity: " << result.curiosityLevel << std::endl;
    std::cout << "  Novelty: " << result.noveltyLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

/**
 * Save results to file
 */
void saveResults(const Phase6IntegrationResult& result, uint64_t totalSteps, const std::string& outputFile) {
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Warning: Could not open output file: " << outputFile << std::endl;
        return;
    }
    
    outFile << "NLM Phase 6 Integration Results" << std::endl;
    outFile << "===============================" << std::endl;
    outFile << std::endl;
    outFile << "Total Steps: " << totalSteps << std::endl;
    outFile << "Total Reward: " << result.totalReward << std::endl;
    outFile << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    outFile << "Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    outFile << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    outFile << "Dopamine Level: " << result.dopamineLevel << std::endl;
    outFile << "Curiosity Level: " << result.curiosityLevel << std::endl;
    outFile << "Novelty Level: " << result.noveltyLevel << std::endl;
    outFile << std::endl;
    outFile << "Integration Status:" << std::endl;
    outFile << "Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
    outFile << "Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
    outFile << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    outFile << "Prediction Integrated: " << (result.predictionIntegrated ? "YES" : "NO") << std::endl;
    outFile << "Development Integrated: " << (result.developmentIntegrated ? "YES" : "NO") << std::endl;
    outFile << "Checkpointing Works: " << (result.checkpointingWorks ? "YES" : "NO") << std::endl;
    outFile << "Replay Works: " << (result.replayWorks ? "YES" : "NO") << std::endl;
    outFile << std::endl;
    outFile << "End Time: " << ctime(&result.endTime);
    
    outFile.close();
    std::cout << "Results saved to: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        DemoConfig config = DemoConfig::parseCommandLine(argc, argv);
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Print startup message
        std::cout << "=== Enhanced NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << std::endl;
        
        if (config.mode == "quick") {
            std::cout << "Quick Test Mode - Lightweight Integration Test" << std::endl;
        } else if (config.mode == "verify") {
            std::cout << "Integration Verification Mode - Only System Connectivity Test" << std::endl;
        } else {
            std::cout << "Full Integration Test Mode - Complete Brain Simulation" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Steps: " << config.maxSteps << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Regions: " << config.regionCount << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Logging: " << (config.enableLogging ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
        
        // Create experiment
        std::cout << "Creating Phase 6 integration experiment..." << std::endl;
        Phase6IntegratedExperiment experiment;
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Phase 1: Integration verification (always run)
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl;
        std::cout << std::endl;
        
        // Phase 2: Optional system-specific tests
        if (config.mode != "verify") {
            std::cout << "--- System-Specific Tests ---" << std::endl;
            
            if (config.showMemoryStats) {
                std::cout << "Running memory integration tests..." << std::endl;
                if (config.verboseOutput) {
                    printProgress("Memory testing", 0.0, 1, 0);
                }
                bool memoryTest = experiment.testMemoryIntegration();
                if (config.verboseOutput) {
                    printProgress("Memory testing", 1.0, 1, 1);
                }
                std::cout << "Memory integration test: " << (memoryTest ? "PASSED" : "FAILED") << std::endl;
                std::cout << std::endl;
            }
            
            if (config.showNeuromodulationStats) {
                std::cout << "Running neuromodulation integration tests..." << std::endl;
                if (config.verboseOutput) {
                    printProgress("Neuromodulation testing", 0.0, 1, 0);
                }
                bool neuromodulationTest = experiment.testNeuromodulationIntegration();
                if (config.verboseOutput) {
                    printProgress("Neuromodulation testing", 1.0, 1, 1);
                }
                std::cout << "Neuromodulation integration test: " << (neuromodulationTest ? "PASSED" : "FAILED") << std::endl;
                std::cout << std::endl;
            }
            
            // Phase 3: Checkpoint and replay tests
            if (config.enableCheckpointing || config.enableReplay) {
                std::cout << "--- Checkpoint and Replay Tests ---" << std::endl;
                
                if (config.enableCheckpointing) {
                    std::cout << "Testing checkpoint save/load..." << std::endl;
                    if (config.verboseOutput) {
                        printProgress("Checkpoint testing", 0.0, 1, 0);
                    }
                    bool checkpointTest = experiment.testCheckpointing();
                    if (config.verboseOutput) {
                        printProgress("Checkpoint testing", 1.0, 1, 1);
                    }
                    std::cout << "Checkpoint test: " << (checkpointTest ? "PASSED" : "FAILED") << std::endl;
                    std::cout << std::endl;
                }
                
                if (config.enableReplay) {
                    std::cout << "Testing replay system..." << std::endl;
                    if (config.verboseOutput) {
                        printProgress("Replay testing", 0.0, 1, 0);
                    }
                    bool replayTest = experiment.testReplay();
                    if (config.verboseOutput) {
                        printProgress("Replay testing", 1.0, 1, 1);
                    }
                    std::cout << "Replay test: " << (replayTest ? "PASSED" : "FAILED") << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        
        // Phase 4: Full integration experiment
        if (config.mode != "verify") {
            std::cout << "--- Full Integration Experiment ---" << std::endl;
            
            // Create configuration for the experiment
            Phase6Config phase6Config;
            phase6Config.maxSteps = config.maxSteps;
            phase6Config.neuronCount = config.neuronCount;
            phase6Config.regionCount = config.regionCount;
            phase6Config.connectionProbability = config.connectionProbability;
            phase6Config.enableCheckpointing = config.enableCheckpointing;
            phase6Config.enableReplay = config.enableReplay;
            phase6Config.enableDevelopment = config.enableDevelopment;
            phase6Config.checkpointPath = config.checkpointPath;
            
            // Record start time for the full experiment
            auto fullStartTime = std::chrono::high_resolution_clock::now();
            
            // Run the full experiment with progress reporting
            std::cout << "Running full integration experiment..." << std::endl;
            
            Phase6IntegrationResult result = experiment.run(phase6Config);
            
            // Record end time
            auto endTime = std::chrono::high_resolution_clock::now();
            auto fullElapsed = std::chrono::duration<double>(endTime - fullStartTime).count();
            result.totalWallClockTime = fullElapsed;
            result.startTime = time(nullptr);
            result.endTime = time(nullptr);
            
            // Print final results
            std::cout << std::endl;
            std::cout << "=== FINAL RESULTS ===" << std::endl;
            std::cout << "Total reward: " << result.totalReward << std::endl;
            std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
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
            
            // Print detailed system statistics
            if (config.verboseOutput) {
                printSystemStats(result, config.maxSteps);
            }
            
            // Save results to file
            if (!config.outputFile.empty()) {
                saveResults(result, config.maxSteps, config.outputFile);
            }
            
            // Print elapsed time
            std::cout << std::endl;
            std::cout << "=== EXECUTION SUMMARY ===" << std::endl;
            std::cout << "Full experiment time: " << formatElapsedTime(fullElapsed) << std::endl;
            std::cout << "Average step rate: " << (config.maxSteps > 0 ? (config.maxSteps / std::max(1.0, fullElapsed)) : 0) << " steps/second" << std::endl;
        }
        
        // Calculate total elapsed time
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalElapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << std::endl;
        std::cout << "=== DEMO COMPLETED ===" << std::endl;
        std::cout << "Total execution time: " << formatElapsedTime(totalElapsed) << std::endl;
        std::cout << "Demo finished successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}