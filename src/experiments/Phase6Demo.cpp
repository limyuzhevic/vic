/**
 * Enhanced Phase 6 Demo - Interactive Integration Test
 * 
 * This demo provides an enhanced interactive interface for running the Phase 6 integration experiment.
 * Users can control the experiment, view real-time results, and analyze integration status.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <iomanip>

using namespace nlm;

// Enhanced welcome banner with better visual appeal
void printWelcomeBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Phase 6 Integration Demo                        ║
    ║                                                               ║
    ║     Interactive Artificial Brain Integration Test            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print system information and experiment configuration
void printSystemInfo(const Phase6Config& config) {
    std::cout << "\n=== SYSTEM CONFIGURATION ===" << std::endl;
    std::cout << "Integration Test Configuration:" << std::endl;
    std::cout << "  • Maximum Steps: " << config.maxSteps << std::endl;
    std::cout << "  • Neuron Count: " << config.neuronCount << std::endl;
    std::cout << "  • Region Count: " << config.regionCount << std::endl;
    std::cout << "  • Connection Probability: " << config.connectionProbability * 100 << "%" << std::endl;
    std::cout << "  • Enable Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  • Enable Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  • Enable Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "  • Checkpoint Path: " << config.checkpointPath << std::endl;
}

// Print experiment progress
void printProgress(const std::string& message, int current, int total, int width = 50) {
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(progress * width);
    
    std::cout << "\rProgress: [";
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
    std::cout << " (" << current << "/" << total << ")";
    std::cout << " - " << message;
    std::cout.flush();
}

// Print comprehensive results
void printResults(const Phase6IntegrationResult& result) {
    std::cout << "\n=== COMPREHENSIVE RESULTS ===" << std::endl;
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  • Total Reward: " << result.totalReward << std::endl;
    std::cout << "  • Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  • Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "  • Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  • Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "  • Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "  • Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "  • Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
}

// Print integration status with detailed analysis
void printIntegrationStatus(const Phase6IntegrationResult& result) {
    std::cout << "\n=== INTEGRATION STATUS ANALYSIS ===" << std::endl;
    std::cout << "System Integration Assessment:" << std::endl;
    
    std::cout << "  Memory Working Memory System:" << std::endl;
    std::cout << "    Status: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "  Episodic Memory System:" << std::endl;
    std::cout << "    Status: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "  Neuromodulation System:" << std::endl;
    std::cout << "    Status: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "  Prediction System:" << std::endl;
    std::cout << "    Status: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "  Development System:" << std::endl;
    std::cout << "    Status: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "  Checkpointing System:" << std::endl;
    std::cout << "    Status: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    
    std::cout << "  Replay System:" << std::endl;
    std::cout << "    Status: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
}

// Print overall assessment
void printOverallAssessment(const Phase6IntegrationResult& result) {
    std::cout << "\n=== OVERALL ASSESSMENT ===" << std::endl;
    
    // Calculate overall integration score
    int integratedSystems = 0;
    int totalSystems = 6;
    
    if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
    if (result.neuromodulationIntegrated) integratedSystems++;
    if (result.predictionIntegrated) integratedSystems++;
    if (result.developmentIntegrated) integratedSystems++;
    if (result.checkpointingWorks) integratedSystems++;
    
    float integrationScore = static_cast<float>(integratedSystems) / totalSystems * 100;
    
    std::cout << "Integration Score: " << integrationScore << "% (" << integratedSystems << "/" << totalSystems << " systems)" << std::endl;
    
    std::cout << "Overall Status: ";
    if (integrationScore >= 90) {
        std::cout << "✓ EXCELLENT - All systems fully integrated" << std::endl;
    } else if (integrationScore >= 75) {
        std::cout << "✓ GOOD - Most systems properly integrated" << std::endl;
    } else if (integrationScore >= 50) {
        std::cout << "⚠ FAIR - Some systems need improvement" << std::endl;
    } else {
        std::cout << "✗ POOR - Significant integration issues" << std::endl;
    }
    
    std::cout << "\nRecommendations:" << std::endl;
    if (integrationScore < 90) {
        std::cout << "  • Review disconnected systems and their integration points" << std::endl;
        std::cout << "  • Check configuration parameters for disconnected systems" << std::endl;
        std::cout << "  • Verify data flow between disconnected components" << std::endl;
        std::cout << "  • Consider system-specific tuning for better integration" << std::endl;
    }
}

// Interactive control menu
void printMenu() {
    std::cout << "\n=== INTERACTIVE CONTROLS ===" << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  r              - Run full integration experiment" << std::endl;
    std::cout << "  v [steps]       - Run verification with custom steps" << std::endl;
    std::cout << "  i              - Show integration status" << std::endl;
    std::cout << "  c              - Show configuration" << std::endl;
    std::cout << "  h              - Show this help" << std::endl;
    std::cout << "  x              - Exit demo" << std::endl;
    std::cout << ">>> ";
}

// Interactive demonstration mode
bool runInteractiveDemo() {
    std::cout << "\n=== INTERACTIVE DEMONSTRATION MODE ===" << std::endl;
    std::cout << "Use the interactive controls to explore the integration experiment." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    // Configure with defaults
    config.maxSteps = 10000;
    config.neuronCount = 1000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    std::string command;
    bool running = true;
    
    while (running) {
        printMenu();
        
        if (!std::getline(std::cin, command)) {
            break; // EOF
        }
        
        if (command.empty()) continue;
        
        char cmd = command[0];
        std::string arg = command.substr(1);
        
        switch (cmd) {
            case 'r': {
                std::cout << "\nRunning full integration experiment..." << std::endl;
                auto result = experiment.run(config);
                printResults(result);
                printIntegrationStatus(result);
                printOverallAssessment(result);
                break;
            }
            
            case 'v': {
                try {
                    int steps = 10000; // Default
                    if (!arg.empty()) {
                        steps = std::stoi(arg);
                    }
                    
                    std::cout << "\nRunning verification with " << steps << " steps..." << std::endl;
                    config.maxSteps = steps;
                    
                    auto result = experiment.run(config);
                    printResults(result);
                    printIntegrationStatus(result);
                    printOverallAssessment(result);
                    
                } catch (const std::exception& e) {
                    std::cout << "Error: Invalid step count - " << e.what() << std::endl;
                }
                break;
            }
            
            case 'i': {
                std::cout << "\nRunning integration verification..." << std::endl;
                bool integrationOK = experiment.verifyIntegration();
                
                if (integrationOK) {
                    std::cout << "Integration verification PASSED ✓" << std::endl;
                } else {
                    std::cout << "Integration verification FAILED ✗" << std::endl;
                }
                break;
            }
            
            case 'c': {
                printSystemInfo(config);
                break;
            }
            
            case 'h': {
                printMenu();
                break;
            }
            
            case 'x': {
                std::cout << "\nExiting demo..." << std::endl;
                running = false;
                break;
            }
            
            default: {
                std::cout << "Unknown command. Type 'h' for help." << std::endl;
                break;
            }
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    printWelcomeBanner();
    
    std::cout << "Initializing NLM Enhanced Phase 6 Integration Demo..." << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Check if user wants interactive mode
    bool interactiveMode = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--interactive" || arg == "-i") {
            interactiveMode = true;
            break;
        }
    }
    
    if (interactiveMode) {
        // Run interactive demo
        if (runInteractiveDemo()) {
            std::cout << "\nInteractive demo completed successfully!" << std::endl;
            return 0;
        } else {
            std::cout << "\nInteractive demo interrupted." << std::endl;
            return 1;
        }
    }
    
    // Run non-interactive demo
    std::cout << "\nRunning non-interactive integration test..." << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.maxSteps = 10000;
    config.neuronCount = 1000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    printResults(result);
    printIntegrationStatus(result);
    printOverallAssessment(result);
    
    std::cout << "\nEnhanced Phase 6 integration demo completed successfully!" << std::endl;
    
    return 0;
}
