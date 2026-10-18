/**
 * NLM Phase 6 Demo Application
 * 
 * A comprehensive command-line interface for running Phase 6 integration tests
 * with advanced configuration options and detailed reporting.
 */

#include "experiments/Phase6DemoMain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace nlm;

/**
 * Command-line option parser
 */
struct DemoOptions {
    // Core simulation parameters
    uint64_t neuronCount = 1000;
    uint64_t maxSteps = 10000;
    uint64_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Feature flags
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableLogging = true;
    bool verboseMode = false;
    
    // Output and reporting
    std::string checkpointPath = "./nlm_checkpoint.bin";
    std::string outputPath = "";
    std::string configFile = "";
    
    // Advanced options
    std::string logLevel = "INFO";
    float timestep = 0.001f;
    float rewardDecay = 0.95f;
    float noveltyThreshold = 0.5f;
    float curiosityThreshold = 0.3f;
    
    // Mode options
    bool verifyOnly = false;
    bool quickMode = false;
    bool benchmarkMode = false;
    
    // Print usage information
    static void printUsage(const char* programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "NLM Phase 6 Integration Demo - Command Line Interface" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Core Options:" << std::endl;
        std::cout << "  --neurons N           Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --steps S             Maximum simulation steps (default: 10000)" << std::endl;
        std::cout << "  --regions R           Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --conn-prob P         Connection probability (default: 0.1)" << std::endl;
        std::cout << "  --timestep T          Simulation timestep (default: 0.001)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Feature Flags:" << std::endl;
        std::cout << "  --no-checkpointing    Disable checkpoint save/load" << std::endl;
        std::cout << "  --no-replay           Disable replay system" << std::endl;
        std::cout << "  --no-development      Disable developmental system" << std::endl;
        std::cout << "  --no-logging          Disable logging" << std::endl;
        std::cout << "  --verbose             Enable verbose output" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --checkpoint-path P   Checkpoint file path (default: ./nlm_checkpoint.bin)" << std::endl;
        std::cout << "  --output-file F       Save results to file" << std::endl;
        std::cout << "  --config-file F       Load configuration from file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --log-level L         Log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
        std::cout << "  --reward-decay D      Reward decay factor (default: 0.95)" << std::endl;
        std::cout << "  --novelty-threshold T  Novelty threshold (default: 0.5)" << std::endl;
        std::cout << "  --curiosity-threshold T  Curiosity threshold (default: 0.3)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Mode Options:" << std::endl;
        std::cout << "  --verify-only         Only verify integration (no simulation)" << std::endl;
        std::cout << "  --quick               Quick demo (fewer neurons/steps)" << std::endl;
        std::cout << "  --benchmark           Benchmark mode (many runs for timing)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName << " --neurons 500 --steps 5000" << std::endl;
        std::cout << "  " << programName << " --quick --verbose" << std::endl;
        std::cout << "  " << programName << " --verify-only" << std::endl;
        std::cout << std::endl;
    }
    
    /**
     * Parse command line arguments
     */
    static DemoOptions parseArgs(int argc, char* argv[]) {
        DemoOptions options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                exit(0);
            }
            else if (arg == "--version") {
                std::cout << "NLM Phase 6 Demo v1.0.0" << std::endl;
                std::cout << "Build with advanced integration features" << std::endl;
                exit(0);
            }
            
            // Core options
            else if (arg == "--neurons") {
                if (i + 1 < argc) options.neuronCount = std::stoul(argv[++i]);
            }
            else if (arg == "--steps") {
                if (i + 1 < argc) options.maxSteps = std::stoull(argv[++i]);
            }
            else if (arg == "--regions") {
                if (i + 1 < argc) options.regionCount = std::stoul(argv[++i]);
            }
            else if (arg == "--conn-prob") {
                if (i + 1 < argc) options.connectionProbability = std::stof(argv[++i]);
            }
            else if (arg == "--timestep") {
                if (i + 1 < argc) options.timestep = std::stof(argv[++i]);
            }
            
            // Feature flags
            else if (arg == "--no-checkpointing") options.enableCheckpointing = false;
            else if (arg == "--no-replay") options.enableReplay = false;
            else if (arg == "--no-development") options.enableDevelopment = false;
            else if (arg == "--no-logging") options.enableLogging = false;
            else if (arg == "--verbose") options.verboseMode = true;
            
            // Output options
            else if (arg == "--checkpoint-path") {
                if (i + 1 < argc) options.checkpointPath = argv[++i];
            }
            else if (arg == "--output-file") {
                if (i + 1 < argc) options.outputPath = argv[++i];
            }
            else if (arg == "--config-file") {
                if (i + 1 < argc) options.configFile = argv[++i];
            }
            
            // Advanced options
            else if (arg == "--log-level") {
                if (i + 1 < argc) options.logLevel = argv[++i];
            }
            else if (arg == "--reward-decay") {
                if (i + 1 < argc) options.rewardDecay = std::stof(argv[++i]);
            }
            else if (arg == "--novelty-threshold") {
                if (i + 1 < argc) options.noveltyThreshold = std::stof(argv[++i]);
            }
            else if (arg == "--curiosity-threshold") {
                if (i + 1 < argc) options.curiosityThreshold = std::stof(argv[++i]);
            }
            
            // Mode options
            else if (arg == "--verify-only") options.verifyOnly = true;
            else if (arg == "--quick") options.quickMode = true;
            else if (arg == "--benchmark") options.benchmarkMode = true;
            
            // Handle --key value pairs
            else if (arg.find("--") == 0 && arg.size() > 2) {
                std::string key = arg.substr(2);
                size_t equals = key.find('=');
                if (equals != std::string::npos) {
                    std::string value = key.substr(equals + 1);
                    key = key.substr(0, equals);
                    
                    // Try to convert value based on type (simplified)
                    if (key == "neurons") options.neuronCount = std::stoul(value);
                    else if (key == "steps") options.maxSteps = std::stoull(value);
                    else if (key == "regions") options.regionCount = std::stoul(value);
                    else if (key == "conn-prob") options.connectionProbability = std::stof(value);
                    else if (key == "timestep") options.timestep = std::stof(value);
                }
            }
        }
        
        // Quick mode settings
        if (options.quickMode) {
            options.neuronCount = 500;
            options.maxSteps = 2000;
            options.regionCount = 1;
            options.connectionProbability = 0.15f;
            options.enableCheckpointing = false; // Faster for quick demo
        }
        
        return options;
    }
    
    /**
     * Load configuration from file
     */
    static void loadConfigFromFile(DemoOptions& options, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open config file: " << filename << std::endl;
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) continue;
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            // Parse based on key
            if (key == "neurons") options.neuronCount = std::stoul(value);
            else if (key == "steps") options.maxSteps = std::stoull(value);
            else if (key == "regions") options.regionCount = std::stoul(value);
            else if (key == "conn-prob") options.connectionProbability = std::stof(value);
            else if (key == "timestep") options.timestep = std::stof(value);
            else if (key == "enable_checkpointing") options.enableCheckpointing = (value == "true" || value == "1");
            else if (key == "enable_replay") options.enableReplay = (value == "true" || value == "1");
            else if (key == "enable_development") options.enableDevelopment = (value == "true" || value == "1");
        }
    }
    
    /**
     * Convert log level string to enum
     */
    static Logger::Level stringToLogLevel(const std::string& levelStr) {
        std::string upper = levelStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        
        if (upper == "DEBUG") return Logger::Level::Debug;
        if (upper == "WARN") return Logger::Level::Warn;
        if (upper == "ERROR") return Logger::Level::Error;
        return Logger::Level::Info; // Default
    }
};

/**
 * Main demo application
 */
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Advanced Command Line Interface" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    
    // Parse command line options
    DemoOptions options = DemoOptions::parseArgs(argc, argv);
    
    // Load configuration from file if specified
    if (!options.configFile.empty()) {
        std::cout << "Loading configuration from: " << options.configFile << std::endl;
        DemoOptions::loadConfigFromFile(options, options.configFile);
    }
    
    // Initialize logger
    auto logger = Logger::getInstance();
    logger->setLevel(DemoOptions::stringToLogLevel(options.logLevel));
    
    if (options.enableLogging) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(DemoOptions::stringToLogLevel(options.logLevel));
        logger->addLogger(consoleLogger);
    }
    
    // Show configuration summary
    std::cout << "=== Configuration ===" << std::endl;
    std::cout << "Neurons: " << options.neuronCount << std::endl;
    std::cout << "Steps: " << options.maxSteps << std::endl;
    std::cout << "Regions: " << options.regionCount << std::endl;
    std::cout << "Connection Probability: " << options.connectionProbability << std::endl;
    std::cout << "Timestep: " << options.timestep << "s" << std::endl;
    std::cout << "Checkpointing: " << (options.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Replay: " << (options.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Development: " << (options.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Verbose Mode: " << (options.verboseMode ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Create Phase 6 experiment configuration
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.regionCount = options.regionCount;
    config.connectionProbability = options.connectionProbability;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = options.checkpointPath;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run based on mode
    if (options.verifyOnly) {
        std::cout << "=== Integration Verification ===" << std::endl;
        bool success = experiment.verifyIntegration();
        std::cout << "Integration: " << (success ? "PASSED" : "FAILED") << std::endl;
        return success ? 0 : 1;
    }
    
    if (options.benchmarkMode) {
        std::cout << "=== Benchmark Mode ===" << std::endl;
        std::cout << "Running multiple trials for timing..." << std::endl;
        // For now, just run once - could be extended for benchmarking
    }
    
    std::cout << "=== Running Phase 6 Integration Experiment ===" << std::endl;
    std::cout << "This will test the integration of all brain systems:" << std::endl;
    std::cout << "- Memory systems (working + episodic)" << std::endl;
    std::cout << "- Neuromodulation (dopamine, curiosity)" << std::endl;
    std::cout << "- Prediction and cognition systems" << std::endl;
    std::cout << "- Development and plasticity" << std::endl;
    std::cout << "- Checkpointing and replay" << std::endl;
    std::cout << std::endl;
    
    // Run the experiment
    Phase6IntegrationResult result = experiment.run(config);
    
    // Display results
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    // Display integration status
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ FAILED") << std::endl;
    std::cout << std::endl;
    
    // Save results to file if requested
    if (!options.outputPath.empty()) {
        std::cout << "Saving results to: " << options.outputPath << std::endl;
        std::ofstream outputFile(options.outputPath);
        if (outputFile.is_open()) {
            outputFile << "=== NLM Phase 6 Integration Results ===" << std::endl;
            outputFile << "Total Reward: " << result.totalReward << std::endl;
            outputFile << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            outputFile << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            outputFile << "Dopamine Level: " << result.dopamineLevel << std::endl;
            outputFile << "Novelty Level: " << result.noveltyLevel << std::endl;
            outputFile << "Curiosity Level: " << result.curiosityLevel << std::endl;
            outputFile << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
            outputFile << std::endl;
            outputFile << "=== Integration Status ===" << std::endl;
            outputFile << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "FAILED") << std::endl;
            outputFile.close();
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    
    return 0;
}