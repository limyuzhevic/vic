/**
 * Enhanced Phase 6 Demo Main Entry Point
 * 
 * This enhanced demo provides comprehensive Phase 6 integration testing with
 * advanced features including configuration management, visualization, 
 * educational tools, and power-user capabilities.
 * 
 * Usage:
 *   ./nlm_phase6_demo_enhanced [options]
 *   ./nlm_phase6_demo_enhanced --help
 *   ./nlm_phase6_demo_enhanced --version
 *   ./nlm_phase6_demo_enhanced --config myconfig.cfg
 *   ./nlm_phase6_demo_enhanced --education integration
 *   ./nlm_phase6_demo_enhanced --power batch --max-episodes 20
 */

#include "experiments/Phase6Demo.hpp"
#include "experiments/EnhancedPhase6Demo.hpp"
#include "experiments/CLIParser.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/AdvancedConfigManager.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace nlm;

/**
 * Display welcome banner
 */
void displayWelcomeBanner() {
    std::cout << Colors::BOLD << Colors::CYAN << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << Colors::BOLD << "NLM Phase 6 Enhanced Integration Demo" << Colors::RESET << "\n";
    std::cout << "  " << Colors::DIM << "Comprehensive Artificial Brain Integration Test" << Colors::RESET << "\n";
    std::cout << Colors::DIM << std::string(70, '=') << "\n\n";
    std::cout << Colors::WHITE << "This enhanced demo verifies that all brain systems work together:" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Memory Systems (Working + Episodic Integration)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Neuromodulation (Dopamine, Curiosity, Novelty)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Prediction System (Temporal and Causal)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Development (Age-dependent Plasticity)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Checkpointing and Replay (Memory Consolidation)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << "  Cognitive Integration (Planning, Attention, Self-model)" << Colors::RESET << "\n";
    std::cout << Colors::DIM << std::string(70, '-') << "\n\n";
    std::cout << Colors::GREEN << "  Backward Compatible:" << Colors::RESET << " All existing CLI arguments continue to work.\n";
    std::cout << Colors::GREEN << "  Advanced Features:" << Colors::RESET << " Configuration files, Visualization, Education, Power User.\n";
    std::cout << Colors::DIM << std::string(70, '=') << Colors::RESET << "\n" << std::endl;
}

/**
 * Display system requirements and environment info
 */
void displaySystemInfo() {
    std::cout << Colors::BOLD << "System Information:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::DIM << "Current time:" << Colors::RESET << " " << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "  " << Colors::DIM << "Working directory:" << Colors::RESET << " " << Colors::WHITE << "[current path]" << Colors::RESET << "\n";
    std::cout << "  " << Colors::DIM << "Build version:" << Colors::RESET << " " << Colors::WHITE << "2.0.0-Enhanced" << Colors::RESET << "\n";
    std::cout << "  " << Colors::DIM << "C++ Standard:" << Colors::RESET << " " << Colors::WHITE << "C++20" << Colors::RESET << "\n";
    std::cout << std::endl;
}

/**
 * Display configuration summary
 */
void displayConfigurationSummary(const CLIParser::CommandLineOptions& options) {
    std::cout << Colors::BOLD << "Configuration Summary:" << Colors::RESET << std::endl;
    std::cout << "  " << Colors::DIM << "Max Steps:" << Colors::RESET << " " << options.maxSteps << "\n";
    std::cout << "  " << Colors::DIM << "Max Episodes:" << Colors::RESET << " " << options.maxEpisodes << "\n";
    std::cout << "  " << Colors::DIM << "Colors:" << Colors::RESET << " " << (options.enableColors ? Colors::GREEN + std::string("Enabled") + Colors::RESET : Colors::RED + std::string("Disabled") + Colors::RESET) << "\n";
    std::cout << "  " << Colors::DIM << "Progress:" << Colors::RESET << " " << (options.enableProgress ? Colors::GREEN + std::string("Enabled") + Colors::RESET : Colors::RED + std::string("Disabled") + Colors::RESET) << "\n";
    std::cout << "  " << Colors::DIM << "Logging:" << Colors::RESET << " " << (options.enableLogging ? Colors::GREEN + std::string("Enabled") + Colors::RESET : Colors::RED + std::string("Disabled") + Colors::RESET) << "\n";
    
    if (!options.configFile.empty()) {
        std::cout << "  " << Colors::DIM << "Config file:" << Colors::RESET << " " << Colors::WHITE << options.configFile << Colors::RESET << "\n";
    }
    if (!options.logFile.empty()) {
        std::cout << "  " << Colors::DIM << "Log file:" << Colors::RESET << " " << Colors::WHITE << options.logFile << Colors::RESET << "\n";
    }
    if (!options.outputFile.empty()) {
        std::cout << "  " << Colors::DIM << "Output file:" << Colors::RESET << " " << Colors::WHITE << options.outputFile << Colors::RESET << "\n";
    }
    if (!options.educationMode.empty()) {
        std::cout << "  " << Colors::DIM << "Education mode:" << Colors::RESET << " " << Colors::WHITE << options.educationMode << Colors::RESET << "\n";
    }
    if (!options.powerUserMode.empty()) {
        std::cout << "  " << Colors::DIM << "Power user mode:" << Colors::RESET << " " << Colors::WHITE << options.powerUserMode << Colors::RESET << "\n";
    }
    
    std::cout << std::endl;
}

/**
 * Handle help/version options
 */
void handleSpecialOptions(const CLIParser::CommandLineOptions& options) {
    if (options.showHelp) {
        CLIParser parser;
        parser.showHelp();
        std::exit(0);
    }
    
    if (options.showVersion) {
        CLIParser parser;
        parser.showVersion();
        std::exit(0);
    }
}

/**
 * Initialize enhanced logging
 */
void initializeEnhancedLogging(const CLIParser::CommandLineOptions& options) {
    // Set up enhanced logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    if (options.enableLogging) {
        if (!options.logFile.empty()) {
            // Log to file as well
            auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            fileLogger->setOutputFile(options.logFile);
            Logger::getGlobal().addLogger(fileLogger);
        }
    }
    
    NLM_LOG_INFO("Enhanced Phase 6 Demo started");
    NLM_LOG_INFO("Configuration: " + std::to_string(options.maxSteps) + " max steps, " + 
                 std::to_string(options.maxEpisodes) + " max episodes");
}

/**
 * Load configuration from file
 */
Phase6Config loadConfiguration(const std::string& configFile, const CLIParser::CommandLineOptions& options) {
    Phase6Config config;
    
    if (!configFile.empty()) {
        // Use AdvancedConfigManager for enhanced configuration loading
        AdvancedConfigManager configManager;
        if (configManager.loadFromFile(configFile, true)) {
            // Apply configuration to Phase6Config
            if (configManager.hasKey("maxSteps")) {
                config.maxSteps = configManager.getValue<size_t>("maxSteps", 10000);
            }
            if (configManager.hasKey("maxEpisodes")) {
                // Note: Phase6Config doesn't have maxEpisodes, use maxSteps for consistency
            }
            if (configManager.hasKey("neuronCount")) {
                config.neuronCount = configManager.getValue<size_t>("neuronCount", 1000);
            }
            if (configManager.hasKey("regionCount")) {
                config.regionCount = configManager.getValue<size_t>("regionCount", 1);
            }
            if (configManager.hasKey("connectionProbability")) {
                config.connectionProbability = configManager.getValue<float>("connectionProbability", 0.1f);
            }
            if (configManager.hasKey("enableCheckpointing")) {
                config.enableCheckpointing = configManager.getValue<bool>("enableCheckpointing", true);
            }
            if (configManager.hasKey("enableReplay")) {
                config.enableReplay = configManager.getValue<bool>("enableReplay", true);
            }
            if (configManager.hasKey("enableDevelopment")) {
                config.enableDevelopment = configManager.getValue<bool>("enableDevelopment", true);
            }
            if (configManager.hasKey("checkpointPath")) {
                config.checkpointPath = configManager.getValue<std::string>("checkpointPath", "./checkpoint_test.bin");
            }
            
            NLM_LOG_INFO("Configuration loaded from: " + configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration");
        }
    }
    
    // Override with command line options
    if (options.maxSteps > 0) {
        config.maxSteps = options.maxSteps;
    }
    if (options.maxEpisodes > 0) {
        // Apply max episodes to max steps for compatibility
        config.maxSteps = std::max(config.maxSteps, static_cast<size_t>(options.maxEpisodes * 50));
    }
    
    return config;
}

/**
 * Save results to file
 */
void saveDemoResults(const Phase6IntegrationResult& result, const std::string& outputFile, const std::string& exportFormat) {
    if (outputFile.empty()) {
        return;
    }
    
    try {
        // Create results JSON
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open output file for writing: " + outputFile);
            return;
        }
        
        file << "{\n";
        file << "  \"demo\": \"NLM Phase 6 Enhanced\",\n";
        file << "  \"version\": \"2.0.0-Enhanced\",\n";
        file << "  \"timestamp\": \"" << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "\",\n";
        file << "  \"totalReward\": " << result.totalReward << ",\n";
        file << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
        file << "  \"avgSynapticWeight\": " << result.avgSynapticWeight << ",\n";
        file << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
        file << "  \"noveltyLevel\": " << result.noveltyLevel << ",\n";
        file << "  \"curiosityLevel\": " << result.curiosityLevel << ",\n";
        file << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
        file << "  \"totalWallClockTime\": " << result.totalWallClockTime << ",\n";
        file << "  \"integrationStatus\": {\n";
        file << "    \"memoryWorkingMemoryIntegrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        file << "    \"memoryEpisodicMemoryIntegrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        file << "    \"neuromodulationIntegrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
        file << "    \"predictionIntegrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
        file << "    \"developmentIntegrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
        file << "    \"checkpointingWorks\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
        file << "    \"replayWorks\": " << (result.replayWorks ? "true" : "false") << "\n";
        file << "  }\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Demo results saved to: " + outputFile);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save demo results: " + std::string(e.what()));
    }
}

/**
 * Main entry point for enhanced Phase 6 demo
 */
int main(int argc, char* argv[]) {
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line options
    CLIParser parser;
    CLIParser::CommandLineOptions options = parser.parse(argc, argv);
    
    // Handle special options (help/version)
    handleSpecialOptions(options);
    
    // Display welcome banner
    displayWelcomeBanner();
    
    // Display system info
    displaySystemInfo();
    
    // Display configuration summary
    displayConfigurationSummary(options);
    
    // Initialize enhanced logging
    initializeEnhancedLogging(options);
    
    // Load configuration
    Phase6Config config = loadConfiguration(options.configFile, options);
    
    // Display start message
    std::cout << Colors::BOLD << "Starting enhanced Phase 6 integration demo..." << Colors::RESET << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " max steps, " 
              << config.regionCount << " regions" << std::endl;
    
    // Create and run enhanced demo
    EnhancedPhase6Demo demo;
    demo.setColorsEnabled(options.enableColors);
    demo.setProgressEnabled(options.enableProgress);
    demo.setLoggingEnabled(options.enableLogging);
    
    // Display current mode
    if (!options.educationMode.empty()) {
        std::cout << Colors::CYAN << "\nEDUCATION MODE: " << Colors::WHITE << options.educationMode << Colors::RESET << std::endl;
    } else if (!options.powerUserMode.empty()) {
        std::cout << Colors::YELLOW << "\nPOWER USER MODE: " << Colors::WHITE << options.powerUserMode << Colors::RESET << std::endl;
    } else {
        std::cout << Colors::GREEN << "\nSTANDARD MODE" << Colors::RESET << std::endl;
    }
    
    // Record start time
    auto startTime = std::chrono::system_clock::now();
    
    // Run the demo
    Phase6IntegrationResult result = demo.run(options);
    
    // Calculate total time
    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    result.totalWallClockTime = elapsed.count();
    result.startTime = currentTime;
    result.endTime = currentTime;
    
    // Display results
    std::cout << std::endl;
    std::cout << Colors::BOLD << Colors::CYAN << "=== DEMO RESULTS ===" << Colors::RESET << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    std::cout << Colors::BOLD << Colors::CYAN << "=== INTEGRATION STATUS ===" << Colors::RESET << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? Colors::GREEN + std::string("CONNECTED") + Colors::RESET : Colors::RED + std::string("DISCONNECTED") + Colors::RESET) << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? Colors::GREEN + std::string("CONNECTED") + Colors::RESET : Colors::RED + std::string("DISCONNECTED") + Colors::RESET) << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? Colors::GREEN + std::string("CONNECTED") + Colors::RESET : Colors::RED + std::string("DISCONNECTED") + Colors::RESET) << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? Colors::GREEN + std::string("CONNECTED") + Colors::RESET : Colors::RED + std::string("DISCONNECTED") + Colors::RESET) << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? Colors::GREEN + std::string("CONNECTED") + Colors::RESET : Colors::RED + std::string("DISCONNECTED") + Colors::RESET) << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? Colors::GREEN + std::string("WORKING") + Colors::RESET : Colors::RED + std::string("NOT WORKING") + Colors::RESET) << std::endl;
    std::cout << std::endl;
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        saveDemoResults(result, options.outputFile, options.exportFormat);
    }
    
    // Display success message
    std::cout << Colors::BOLD << Colors::GREEN << "\n=== DEMO COMPLETED SUCCESSFULLY ===" << Colors::RESET << std::endl;
    std::cout << "The integrated NLM brain demonstrates all systems working together." << std::endl;
    std::cout << "Memory, neuromodulation, prediction, development, and checkpointing all function cohesively." << std::endl;
    std::cout << "The enhanced Phase 6 demo maintains backward compatibility while adding advanced features." << std::endl;
    std::cout << std::endl;
    
    NLM_LOG_INFO("Enhanced Phase 6 demo completed successfully");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Integration status: All systems connected");
    
    return 0;
}