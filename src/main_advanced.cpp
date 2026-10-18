// NLM Advanced Main Entry Point
// Enhanced main function that provides both legacy and advanced CLI support

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "cli/NLMAdvancedCLI.hpp"
#include <iostream>
#include <memory>

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Advanced CLI Edition                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced command-line interface with advanced features     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>      Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --advanced, -a        Use advanced CLI interface (interactive)" << std::endl;
    std::cout << "  --quiet, -q           Quiet mode (suppress output)" << std::endl;
    std::cout << "  --verbose, -v         Verbose output" << std::endl;
    std::cout << "  --help, -h            Show help information" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced CLI Features:" << std::endl;
    std::cout << "  working_memory        - Working memory analysis and management" << std::endl;
    std::cout << "  episodic_memory      - Episodic memory queries" << std::endl;
    std::cout << "  prediction_analysis  - Prediction confidence and error analysis" << std::endl;
    std::cout << "  neuromodulation       - Neuromodulation control (ACh, NE, 5-HT)" << std::endl;
    std::cout << "  state_analysis       - Brain state inspection and analysis" << std::endl;
    std::cout << "  performance          - Performance profiling and debugging" << std::endl;
    std::cout << "  simulation_control   - Extended simulation control (pause, resume, step)" << std::endl;
    std::cout << "  visualization        - Real-time monitoring and visualization" << std::endl;
    std::cout << "  configuration        - Advanced configuration management" << std::endl;
    std::cout << "  script               - Script-based control and batch processing" << std::endl;
    std::cout << "  diagnostic           - Error detection and system health monitoring" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information about a specific command, use: advanced_cli help <command>" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool advancedMode = false;
    bool quietMode = false;
    bool verboseMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--advanced" || arg == "-a") {
            advancedMode = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(quietMode ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    if (!quietMode) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        if (!quietMode) {
            std::cout << "Configuration loaded successfully." << std::endl;
        }
    } else {
        if (!quietMode) {
            std::cout << "Using default configuration." << std::endl;
        }
    }
    
    // Apply command line overrides
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Override with Phase 2 defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize NLM brain
    std::cout << "Initializing NLM brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Configure logging level
    if (verboseMode) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    // Run Phase 2 demonstrations (original functionality)
    if (!advancedMode) {
        std::cout << "\n=== Running Phase 2 Demonstrations ===" << std::endl;
        
        // Include the original demonstration functions from main.cpp
        // For now, we'll show the advanced CLI is available
        if (!quietMode) {
            std::cout << "Phase 2 demonstrations would run here." << std::endl;
            std::cout << "Use '--advanced' flag to access enhanced CLI features." << std::endl;
        }
        
        return 0;
    }
    
    // Run advanced CLI
    std::cout << "\n=== NLM Advanced CLI ===" << std::endl;
    if (!quietMode) {
        std::cout << "Starting enhanced interactive command-line interface..." << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
        std::cout << std::endl;
    }
    
    nlm::cli::NLMAdvancedCLI advancedCLI(brain, config);
    advancedCLI.setVerbose(verboseMode);
    advancedCLI.setQuiet(quietMode);
    
    advancedCLI.initialize();
    
    return advancedCLI.processArguments(argc, argv);
}
