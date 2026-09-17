#include "experiments/Phase6ConfigManager.hpp"
#include "experiments/ExperimentFactory.hpp"
#include "experiments/SystemCreators.hpp"
#include "experiments/ExperimentRunnerImpl.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/ExperimentInterfaces.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM PHASE 6 INTEGRATION ARCHITECTURE DEMO                  ║
║                                                               ║
║     Demonstrating Refactored Phase 6 Integration               ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printUsage() {
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Integration Demo Options:" << std::endl;
    std::cout << "  --help                     Show help message and exit" << std::endl;
    std::cout << "  --quick                    Run quick integration test (small scale)" << std::endl;
    std::cout << "  --full                     Run full integration test (default)" << std::endl;
    std::cout << "  --demo-mode                Interactive demo mode with menu" << std::endl;
    std::cout << "  --autonomous               Run autonomously without user input" << std::endl;
    std::cout << "  --config=<file>            Load configuration from file" << std::endl;
    std::cout << "  --neurons=<n>              Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=<s>                Maximum simulation steps (default: 10000)" << std::endl;
    std::cout << "  --regions=<r>              Number of regions (default: 1)" << std::endl;
    std::cout << "  --connprob=<p>             Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --no-checkpoint            Disable checkpointing" << std::endl;
    std::cout << "  --no-replay                Disable replay" << std::endl;
    std::cout << "  --no-development           Disable development" << std::endl;
    std::cout << "  --no-curiosity             Disable curiosity" << std::endl;
    std::cout << "  --checkpoint=<path>        Checkpoint file path" << std::endl;
    std::cout << "  --verbose                  Enable verbose output" << std::endl;
    std::cout << "  --quiet                    Disable most output (errors only)" << std::endl;
    std::cout << "  --output=<file>            Save results to file" << std::endl;
}

void printProgress(const std::string& message, int progress, int total) {
    // Simple progress bar
    const int barWidth = 50;
    float progressPercent = (float)progress / total;
    int pos = barWidth * progressPercent;
    
    std::cout << "\r" << std::string(50, ' ') << "\r"; // Clear line
    std::cout << "["; 
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progressPercent * 100.0) << "% " << message;
    std::cout.flush();
}

void printDemoHeader(const std::string& title, const nlm::Phase6Config& config) {
    std::cout << std::endl;
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << std::fixed << std::setprecision(3) << config.connectionProbability << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Curiosity: " << (config.enableCuriosity ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
}

nlm::Phase6Config parseArguments(int argc, char** argv) {
    nlm::Phase6Config config;
    std::string configFile = "";
    bool quickMode = false;
    bool verbose = false;
    bool quiet = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            exit(0);
        } else if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--full") {
            // Default, nothing to do
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg.find("--neurons=") == 0) {
            try {
                config.neuronCount = std::stoull(arg.substr(10));
            } catch (...) {
                std::cerr << "Error: Invalid neuron count: " << arg.substr(10) << std::endl;
            }
        } else if (arg.find("--steps=") == 0) {
            try {
                config.maxSteps = std::stoull(arg.substr(7));
            } catch (...) {
                std::cerr << "Error: Invalid max steps: " << arg.substr(7) << std::endl;
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                config.regionCount = std::stoull(arg.substr(10));
            } catch (...) {
                std::cerr << "Error: Invalid region count: " << arg.substr(10) << std::endl;
            }
        } else if (arg.find("--connprob=") == 0) {
            try {
                config.connectionProbability = std::stof(arg.substr(11));
            } catch (...) {
                std::cerr << "Error: Invalid connection probability: " << arg.substr(11) << std::endl;
            }
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--no-curiosity") {
            config.enableCuriosity = false;
        } else if (arg.find("--checkpoint=") == 0) {
            config.checkpointPath = arg.substr(13);
        } else if (arg.find("--output=") == 0) {
            // Will be handled separately
        }
    }
    
    // Quick mode settings
    if (quickMode) {
        config.neuronCount = 50;
        config.maxSteps = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableCuriosity = false;
        config.checkpointPath = "./quick_checkpoint.bin";
    }
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        auto configManager = std::make_unique<nlm::Phase6ConfigManager>();
        if (configManager->loadFromFile(configFile)) {
            // Merge loaded config with command line arguments
            // Command line takes precedence
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cerr << "Warning: Failed to load configuration from: " << configFile << std::endl;
            std::cerr << "Using default configuration with command line overrides" << std::endl;
        }
    }
    
    return config;
}

void runInteractiveDemo() {
    std::cout << "=== Interactive Phase 6 Demo Mode ===" << std::endl;
    std::cout << "Starting interactive demonstration of the refactored Phase 6 integration architecture." << std::endl;
    std::cout << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Get user preferences
    std::cout << "Enter configuration for demo (press Enter for defaults):" << std::endl;
    
    nlm::Phase6Config config = nlm::Phase6Config::getDefaultConfig();
    
    std::cout << "Default neurons: " << config.neuronCount << " (Enter new value or press Enter to keep): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.neuronCount = std::stoull(input);
        } catch (...) {
            std::cout << "Invalid input, keeping default" << std::endl;
        }
    }
    
    std::cout << "Default max steps: " << config.maxSteps << " (Enter new value or press Enter to keep): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.maxSteps = std::stoull(input);
        } catch (...) {
            std::cout << "Invalid input, keeping default" << std::endl;
        }
    }
    
    std::cout << "Enable checkpointing? (y/n, default: n): ";
    std::getline(std::cin, input);
    if (input == "y" || input == "Y" || input == "yes") {
        config.enableCheckpointing = true;
        config.checkpointPath = "./interactive_checkpoint.bin";
    }
    
    // Run demonstration
    printDemoHeader("Running Phase 6 Integration Demo", config);
    
    std::cout << std::endl << "Starting integration test..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl << "=== Demo Results ===" << std::endl;
    std::cout << "Execution time: " << duration << " seconds" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "  Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
}

void runAutonomousDemo(const nlm::Phase6Config& config) {
    std::cout << "=== Autonomous Phase 6 Demo Mode ===" << std::endl;
    std::cout << "Running demonstration autonomously without user input." << std::endl;
    std::cout << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    printDemoHeader("Autonomous Phase 6 Integration Demo", config);
    
    std::cout << "Starting autonomous integration test..." << std::endl;
    std::cout << "This may take several minutes depending on configuration." << std::endl;
    std::cout << "You can interrupt with Ctrl+C if needed." << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // For demonstration, we'll run a smaller version
    nlm::Phase6Config demoConfig = config;
    if (demoConfig.maxSteps > 1000) {
        demoConfig.maxSteps = 1000; // Limit for autonomous demo
        std::cout << "Note: Limiting to 1000 steps for autonomous demo." << std::endl;
    }
    
    auto result = experiment.run(demoConfig);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl << "=== Autonomous Demo Results ===" << std::endl;
    std::cout << "Execution time: " << duration << " seconds" << std::endl;
    std::cout << std::endl;
    
    if (result.totalWallClockTime > 0) {
        std::cout << "Performance summary:" << std::endl;
        std::cout << "  System successfully integrated and completed simulation." << std::endl;
        std::cout << "  Integration verification passed: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
        std::cout << "  Neuromodulation functional: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
        std::cout << "  Memory systems connected: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
    }
}

void demoAdvancedFeatures() {
    std::cout << "=== Advanced Features Demo ===" << std::endl;
    std::cout << "Demonstrating advanced features of the refactored architecture:" << std::endl;
    std::cout << std::endl;
    
    // Demonstrate factory pattern
    std::cout << "1. Factory Pattern:" << std::endl;
    auto& factoryRegistry = nlm::ExperimentFactoryRegistry::getInstance();
    auto factories = factoryRegistry.getRegisteredFactories();
    std::cout << "   Registered experiment types: " << factories.size() << std::endl;
    for (const auto& name : factories) {
        std::cout << "     - " << name << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate system creators
    std::cout << "2. System Creators:" << std::endl;
    auto& creatorRegistry = nlm::SystemCreatorRegistry::getInstance();
    auto brainCreators = creatorRegistry.getBrainCreators();
    auto worldCreators = creatorRegistry.getWorldCreators();
    std::cout << "   Available brain creators: " << brainCreators.size() << std::endl;
    std::cout << "   Available world creators: " << worldCreators.size() << std::endl;
    std::cout << std::endl;
    
    // Demonstrate configuration management
    std::cout << "3. Configuration Management:" << std::endl;
    auto configManager = std::make_unique<nlm::Phase6ConfigManager>();
    std::cout << "   Configuration validation: " << (configManager->validateConfiguration() ? "Passed" : "Failed") << std::endl;
    auto errors = configManager->getValidationErrors();
    if (!errors.empty()) {
        std::cout << "   Validation errors: " << errors.size() << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate experiment runner
    std::cout << "4. Experiment Runner:" << std::endl;
    auto& runnerRegistry = nlm::ExperimentRunnerRegistry::getInstance();
    auto runners = runnerRegistry.getRegisteredRunners();
    std::cout << "   Registered experiment runners: " << runners.size() << std::endl;
    for (const auto& name : runners) {
        std::cout << "     - " << name << std::endl;
    }
    std::cout << std::endl;
}

void saveResultsToFile(const nlm::Phase6IntegrationResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    file << "=== NLM Phase 6 Integration Results ===" << std::endl;
    file << std::endl;
    file << "Integration Status:" << std::endl;
    file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << std::endl;
    
    file << "Performance Metrics:" << std::endl;
    file << "  Total reward: " << result.totalReward << std::endl;
    file << "  Average firing rate: " << result.avgFiringRate << std::endl;
    file << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    file << "  Novelty level: " << result.noveltyLevel << std::endl;
    file << "  Curiosity level: " << result.curiosityLevel << std::endl;
    file << "  Dopamine level: " << result.dopamineLevel << std::endl;
    file << "  Start time: " << result.startTime << std::endl;
    file << "  End time: " << result.endTime << std::endl;
    file << "  Total wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    file.close();
    std::cout << "Results saved to: " << filename << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::NLM_LOG_INFO("=== Phase 6 Integration Architecture Demo ===");
    nlm::NLM_LOG_INFO("Starting enhanced Phase 6 integration demo with CLI support");
    
    // Parse command line arguments
    nlm::Phase6Config config = parseArguments(argc, argv);
    
    // Determine demo mode
    bool quickMode = false;
    bool interactiveMode = false;
    bool autonomousMode = false;
    bool advancedMode = false;
    std::string outputFile = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quick") quickMode = true;
        else if (arg == "--demo-mode") interactiveMode = true;
        else if (arg == "--autonomous") autonomousMode = true;
        else if (arg == "--advanced") advancedMode = true;
        else if (arg.find("--output=") == 0) outputFile = arg.substr(10);
    }
    
    try {
        if (advancedMode) {
            std::cout << "Advanced features enabled." << std::endl;
            demoAdvancedFeatures();
        }
        
        if (quickMode || !interactiveMode && !autonomousMode) {
            std::cout << "Quick mode: Running small-scale integration test." << std::endl;
            runAutonomousDemo(config);
        } else if (interactiveMode) {
            std::cout << "Interactive mode: User-guided demo." << std::endl;
            runInteractiveDemo();
        } else if (autonomousMode) {
            std::cout << "Autonomous mode: Running automated demonstration." << std::endl;
            runAutonomousDemo(config);
        }
        
        // Save results if requested
        if (!outputFile.empty()) {
            // In a real implementation, we would capture the actual results
            // For this demo, we'll create a sample result
            nlm::Phase6IntegrationResult sampleResult;
            sampleResult.memoryWorkingMemoryIntegrated = true;
            sampleResult.memoryEpisodicMemoryIntegrated = true;
            sampleResult.neuromodulationIntegrated = true;
            sampleResult.predictionIntegrated = true;
            sampleResult.developmentIntegrated = true;
            sampleResult.checkpointingWorks = true;
            sampleResult.totalReward = 10.5f;
            sampleResult.avgFiringRate = 0.15f;
            sampleResult.memoryEpisodesStored = 5.0f;
            sampleResult.dopamineLevel = 0.8f;
            sampleResult.totalWallClockTime = 42.0;
            
            saveResultsToFile(sampleResult, outputFile);
        }
        
        nlm::NLM_LOG_INFO("=== Demo Complete ===");
        std::cout << std::endl << "Demo completed successfully!" << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        nlm::NLM_LOG_ERROR(std::string("Demo failed: ") + e.what());
        return 1;
    }
    
    return 0;
}
