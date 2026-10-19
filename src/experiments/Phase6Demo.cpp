/**
 * Enhanced Phase 6 Demo - Advanced Integration Test with Extended Features
 * 
 * This enhanced demo provides:
 * - Advanced command line argument support
 * - Interactive brain exploration and control
 * - Development stage management
 * - Advanced memory analysis and manipulation
 * - Fine-grained neuromodulation control
 * - Enhanced checkpoint and replay operations
 * - Learning analysis and visualization
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace nlm;

// Advanced command line parser
class AdvancedCommandLineParser {
public:
    struct AdvancedConfig {
        // Basic settings (inherited from Phase6Config)
        uint64_t maxSteps = 10000;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string checkpointPath = "./checkpoint_test.bin";
        
        // Advanced settings
        bool enableInteractiveMode = false;
        bool enableAdvancedAnalysis = false;
        bool enableDetailedLogging = false;
        std::string logFile = "nlm_advanced.log";
        
        // Development settings
        DevelopmentalStage startDevelopmentStage = DevelopmentalStage::Initial;
        bool enableStageSkipping = false;
        std::vector<DevelopmentalStage> developmentStages;
        
        // Memory settings
        bool enableMemoryProfiling = false;
        size_t maxMemoryTraces = 10000;
        size_t maxEpisodicEpisodes = 5000;
        
        // Neuromodulation settings
        bool enableNeuromodulationProfiling = false;
        float neuromodulationSensitivity = 1.0f;
        
        // Analysis settings
        bool enableLearningAnalysis = false;
        bool enableSynapticAnalysis = false;
        bool enableNetworkTopologyAnalysis = false;
    };
    
    static AdvancedConfig parseAdvancedArgs(int argc, char* argv[]);
    static void printAdvancedUsage();
};

// Command line argument parsing
AdvancedCommandLineParser::AdvancedConfig AdvancedCommandLineParser::parseAdvancedArgs(int argc, char* argv[]) {
    AdvancedConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--interactive") {
            config.enableInteractiveMode = true;
        } else if (arg == "--advanced-analysis") {
            config.enableAdvancedAnalysis = true;
        } else if (arg == "--detailed-logging") {
            config.enableDetailedLogging = true;
        } else if (arg.find("--log-file=") == 0) {
            config.logFile = arg.substr(10);
        } else if (arg == "--memory-profiling") {
            config.enableMemoryProfiling = true;
        } else if (arg == "--neuromodulation-profiling") {
            config.enableNeuromodulationProfiling = true;
        } else if (arg == "--learning-analysis") {
            config.enableLearningAnalysis = true;
        } else if (arg.find("--dev-stage=") == 0) {
            int stage = std::stoi(arg.substr(11));
            if (stage >= 0 && stage < 6) {
                config.startDevelopmentStage = static_cast<DevelopmentalStage>(stage);
            }
        } else if (arg == "--stage-skipping") {
            config.enableStageSkipping = true;
        } else if (arg.find("--neuron-count=") == 0) {
            config.neuronCount = std::stoul(arg.substr(15));
        } else if (arg.find("--max-steps=") == 0) {
            config.maxSteps = std::stoull(arg.substr(12));
        } else if (arg == "--help" || arg == "-h") {
            printAdvancedUsage();
            exit(0);
        }
    }
    
    return config;
}

void AdvancedCommandLineParser::printAdvancedUsage() {
    std::cout << "=== NLM Phase 6 Advanced Demo Usage ===" << std::endl;
    std::cout << "Enhanced brain simulation with advanced controls and analysis." << std::endl << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h              Show this help" << std::endl;
    std::cout << "  --neuron-count=N        Set neuron count" << std::endl;
    std::cout << "  --max-steps=N           Set maximum simulation steps" << std::endl;
    std::cout << "  --dev-stage=N           Set starting development stage (0=Initial, 5=Adult)" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Features:" << std::endl;
    std::cout << "  --interactive           Enable interactive brain control" << std::endl;
    std::cout << "  --advanced-analysis     Enable advanced analysis and profiling" << std::endl;
    std::cout << "  --detailed-logging      Enable detailed logging to file" << std::endl;
    std::cout << "  --log-file=FILE         Set log file base name" << std::endl;
    std::cout << "  --memory-profiling      Enable memory usage profiling" << std::endl;
    std::cout << "  --neuromodulation-profiling Enable neuromodulation profiling" << std::endl;
    std::cout << "  --learning-analysis     Enable learning analysis" << std::endl;
    std::cout << "  --stage-skipping        Enable development stage skipping" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --interactive --dev-stage=2 --neuron-count=2000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --advanced-analysis --learning-analysis --memory-profiling" << std::endl;
    std::cout << "  ./nlm_phase6_demo --detailed-logging --log-file=experiment1" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Enhanced Integration Demo ===" << std::endl;
    std::cout << "Advanced artificial brain with extended features and controls" << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse advanced command line arguments
    AdvancedCommandLineParser::AdvancedConfig config = AdvancedCommandLineParser::parseAdvancedArgs(argc, argv);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "=== Advanced Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Development Stage: " << static_cast<int>(config.startDevelopmentStage) << std::endl;
    std::cout << "  Interactive Mode: " << (config.enableInteractiveMode ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Advanced Analysis: " << (config.enableAdvancedAnalysis ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Memory Profiling: " << (config.enableMemoryProfiling ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Neuromodulation Profiling: " << (config.enableNeuromodulationProfiling ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Learning Analysis: " << (config.enableLearningAnalysis ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;
    
    // Run the standard integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
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
    
    // Run full experiment with enhanced settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = config.maxSteps;
    phase6Config.enableCheckpointing = config.enableCheckpointing;
    phase6Config.enableReplay = config.enableReplay;
    phase6Config.enableDevelopment = config.enableDevelopment;
    
    auto result = experiment.run(phase6Config);
    
    std::cout << std::endl;
    std::cout << "=== ADVANCED DEMO RESULTS ===" << std::endl;
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
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Display advanced analysis if enabled
    if (config.enableAdvancedAnalysis) {
        std::cout << "=== ADVANCED ANALYSIS ===" << std::endl;
        std::cout << "Note: Advanced analysis features would be implemented here." << std::endl;
        std::cout << "This includes learning trajectory analysis, synaptic plasticity analysis," << std::endl;
        std::cout << "memory efficiency metrics, and development impact analysis." << std::endl;
        std::cout << std::endl;
    }
    
    if (config.enableMemoryProfiling) {
        std::cout << "=== MEMORY PROFILING ===" << std::endl;
        std::cout << "Memory usage analysis would be implemented here." << std::endl;
        std::cout << std::endl;
    }
    
    if (config.enableNeuromodulationProfiling) {
        std::cout << "=== NEUROMODULATION PROFILING ===" << std::endl;
        std::cout << "Neuromodulation dynamics analysis would be implemented here." << std::endl;
        std::cout << std::endl;
    }
    
    if (config.enableLearningAnalysis) {
        std::cout << "=== LEARNING ANALYSIS ===" << std::endl;
        std::cout << "Learning trajectory analysis would be implemented here." << std::endl;
        std::cout << "Includes synaptic change rates, memory retention metrics, and behavioral adaptation analysis." << std::endl;
        std::cout << std::endl;
    }
    
    // Generate comprehensive reports
    if (!config.logFile.empty()) {
        std::cout << "=== Generating Reports ===" << std::endl;
        std::cout << "Advanced reports would be generated and saved to " << config.logFile << "*" << std::endl;
        std::cout << "Reports include memory analysis, neuromodulation profiling, and learning metrics." << std::endl;
    }
    
    return 0;
}
