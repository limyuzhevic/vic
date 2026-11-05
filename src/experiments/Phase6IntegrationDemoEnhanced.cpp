// NLM Phase 6 Integration Demo - Enhanced CLI
// Unified demonstration of all NLM Phase 6 capabilities
// This demo showcases the complete integration of all brain systems

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace nlm;

// Phase 6 integration options structure
struct Phase6IntegrationOptions {
    bool enableIntegrationVerification;
    bool enableMemoryIntegration;
    bool enableNeuromodulationIntegration;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    int neuronCount;
    int maxSteps;
    std::string outputFile;
    std::string logLevel;
    bool verbose;
    bool quiet;
    bool help;
    bool version;
    std::map<std::string, std::string> experimentParams;
};

// Parse command line arguments for Phase 6 integration
Phase6IntegrationOptions parsePhase6IntegrationOptions(int argc, char* argv[]) {
    Phase6IntegrationOptions options;
    
    // Set defaults
    options.enableIntegrationVerification = true;
    options.enableMemoryIntegration = true;
    options.enableNeuromodulationIntegration = true;
    options.enableCheckpointing = true;
    options.enableReplay = true;
    options.enableDevelopment = true;
    options.neuronCount = 500;
    options.maxSteps = 2000;
    options.outputFile = "";
    options.logLevel = "info";
    options.verbose = false;
    options.quiet = false;
    options.help = false;
    options.version = false;
    
    // Parse common options first
    auto commonOptions = parseCommonOptions(argc, argv, "nlm_phase6_demo");
    
    // Override with specific options (simple parsing for now)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 2) == "--") {
            std::string opt = arg.substr(2);
            size_t equalsPos = opt.find('=');
            if (equalsPos != std::string::npos) {
                std::string value = opt.substr(equalsPos + 1);
                opt = opt.substr(0, equalsPos);
                
                if (opt == "neuron-count") options.neuronCount = std::stoi(value);
                else if (opt == "max-steps") options.maxSteps = std::stoi(value);
                else if (opt == "timestep") options.experimentParams["timestep"] = value;
                else if (opt == "enable-integration") options.enableIntegrationVerification = (value == "1" || value == "true" || value == "yes");
                else if (opt == "enable-memory") options.enableMemoryIntegration = (value == "1" || value == "true" || value == "yes");
                else if (opt == "enable-neuromodulation") options.enableNeuromodulationIntegration = (value == "1" || value == "true" || value == "yes");
                else if (opt == "enable-checkpoint") options.enableCheckpointing = (value == "1" || value == "true" || value == "yes");
                else if (opt == "enable-replay") options.enableReplay = (value == "1" || value == "true" || value == "yes");
                else if (opt == "enable-development") options.enableDevelopment = (value == "1" || value == "true" || value == "yes");
                else if (opt == "log-level") options.logLevel = value;
                else if (opt == "output-file") options.outputFile = value;
                else if (opt == "checkpoint-file") options.experimentParams["checkpointFile"] = value;
                else if (opt == "log-file") options.experimentParams["logFile"] = value;
                else {
                    options.experimentParams[opt] = value;
                }
            }
        }
    }
    
    // Set flags from common options
    options.verbose = commonOptions.verbose;
    options.quiet = commonOptions.quiet;
    options.logLevel = commonOptions.logLevel;
    
    return options;
}

void printPhase6IntegrationHelp() {
    std::cout << "NLM Phase 6 Integration Demo - Enhanced CLI" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Description:" << std::endl;
    std::cout << "  This demo runs the Phase 6 integration experiment to verify" << std::endl;
    std::cout << "  that all brain systems are properly connected." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --verbose, -v             Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q               Quiet mode (suppress info messages)" << std::endl;
    std::cout << "  --config FILE             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output FILE             Output file for results" << std::endl;
    std::cout << "  --neuron-count N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps N             Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --timestep VAL            Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --log-level LEVEL         Logging level (trace, debug, info, warn, error, critical)" << std::endl;
    std::cout << "  --enable-integration      Enable integration verification (default: true)" << std::endl;
    std::cout << "  --disable-integration     Disable integration verification" << std::endl;
    std::cout << "  --enable-memory           Enable memory integration (default: true)" << std::endl;
    std::cout << "  --disable-memory          Disable memory integration" << std::endl;
    std::cout << "  --enable-neuromodulation   Enable neuromodulation integration (default: true)" << std::endl;
    std::cout << "  --disable-neuromodulation  Disable neuromodulation integration" << std::endl;
    std::cout << "  --enable-checkpoint       Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --disable-checkpoint      Disable checkpointing" << std::endl;
    std::cout << "  --enable-replay           Enable replay (default: true)" << std::endl;
    std::cout << "  --disable-replay          Disable replay" << std::endl;
    std::cout << "  --enable-development      Enable development (default: true)" << std::endl;
    std::cout << "  --disable-development     Disable development" << std::endl;
    std::cout << "  --checkpoint-file FILE    Checkpoint file path" << std::endl;
    std::cout << "  --log-file FILE           Log file path" << std::endl;
    std::cout << "  --plasticity              Enable synaptic plasticity" << std::endl;
    std::cout << "  --no-plasticity           Disable synaptic plasticity" << std::endl;
    std::cout << "  --curiosity               Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --no-curiosity            Disable curiosity-driven exploration" << std::endl;
    std::cout << "  --reward                  Enable reward modulation" << std::endl;
    std::cout << "  --no-reward               Disable reward modulation" << std::endl;
    std::cout << std::endl;
    std::cout << "The enhanced CLI provides comprehensive control over all aspects" << std::endl;
    std::cout << "of the Phase 6 integration experiment with detailed logging and" << std::endl;
    std::cout << "output options." << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse options
    Phase6IntegrationOptions options = parsePhase6IntegrationOptions(argc, argv);
    
    if (options.help) {
        printPhase6IntegrationHelp();
        return 0;
    }
    
    if (options.version) {
        std::cout << "NLM Phase 6 Integration Demo - Enhanced Version 0.1.0" << std::endl;
        std::cout << "This demo showcases the complete integration of all brain systems." << std::endl;
        return 0;
    }
    
    // Setup logging based on options
    setupCommonLogging(options.logLevel, options.verbose, options.quiet);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
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
    
    // Run full experiment with configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    
    // Add experiment parameters
    for (const auto& pair : options.experimentParams) {
        if (pair.first == "timestep") {
            // Convert timestep string to float
            try {
                float timestep = std::stof(pair.second);
                config.timestep = timestep;
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid timestep value: " << pair.second << std::endl;
            }
        }
    }
    
    auto result = experiment.run(config);
    
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
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Export metrics if output file specified
    if (!options.outputFile.empty()) {
        std::map<std::string, double> additionalMetrics;
        additionalMetrics["neuron_count"] = static_cast<double>(options.neuronCount);
        additionalMetrics["max_steps"] = static_cast<double>(options.maxSteps);
        additionalMetrics["plasticity_enabled"] = options.enablePlasticity ? 1.0 : 0.0;
        additionalMetrics["development_enabled"] = options.enableDevelopment ? 1.0 : 0.0;
        additionalMetrics["curiosity_enabled"] = options.enableCuriosity ? 1.0 : 0.0;
        additionalMetrics["reward_enabled"] = options.enableRewardModulation ? 1.0 : 0.0;
        
        exportMetrics(options.outputFile, result, additionalMetrics);
        std::cout << std::endl;
        std::cout << "Results exported to: " << options.outputFile << std::endl;
    }
    
    return 0;
}

} // namespace nlm