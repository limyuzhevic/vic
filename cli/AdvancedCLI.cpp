// AdvancedCLI implementation
#include "AdvancedCLI.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>

namespace nlm {

AdvancedCLI::AdvancedCLI(std::shared_ptr<Config> config,
                         std::shared_ptr<Brain> brain,
                         std::shared_ptr<Logger> logger)
    : config(config), brain(brain), logger(logger) {
    initialize();
}

bool AdvancedCLI::initialize() {
    // Initialize components
    commandFramework = std::make_shared<CommandExecutionFramework>(logger);
    outputFormatter = std::make_shared<CommandOutputFormatter>(logger);
    
    // Register all commands
    registerCommands();
    
    // Initialize command-specific handlers
    brainCommands = std::make_shared<BrainCommands>(outputFormatter, logger);
    simulationCommands = std::make_shared<SimulationCommands>(outputFormatter, logger);
    inspectionCommands = std::make_shared<InspectionCommands>(outputFormatter, logger);
    analysisCommands = std::make_shared<AnalysisCommands>(outputFormatter, logger);
    debuggingCommands = std::make_shared<DebuggingCommands>(outputFormatter, logger);
    exportImportCommands = std::make_shared<ExportImportCommands>(outputFormatter, logger);
    advancedFeatures = std::make_shared<AdvancedFeatures>(outputFormatter, logger);
    
    commandParser = std::make_shared<AdvancedCommandLineParser>(config, brain, logger);
    
    logger->log("Advanced CLI initialized successfully", LogLevel::Info);
    return true;
}

void AdvancedCLI::cleanup() {
    // Cleanup components
    logger->log("Cleaning up Advanced CLI", LogLevel::Info);
    
    advancedFeatures.reset();
    exportImportCommands.reset();
    debuggingCommands.reset();
    analysisCommands.reset();
    inspectionCommands.reset();
    simulationCommands.reset();
    brainCommands.reset();
    
    commandParser.reset();
    outputFormatter.reset();
    commandFramework.reset();
    
    logger->log("Advanced CLI cleanup completed", LogLevel::Info);
}

bool AdvancedCLI::runAdvancedCLI(int argc, char* argv[]) {
    parseAdvancedCommandLine(argc, argv);
    
    if (options.help) {
        printAdvancedHelp();
        return true;
    }
    
    if (options.version) {
        printVersion();
        return true;
    }
    
    return executeParsedCommands();
}

bool AdvancedCLI::runOriginalCLI(int argc, char* argv[]) {
    return handleOriginalCommandLine(argc, argv);
}

void AdvancedCLI::parseCommandLine(int argc, char* argv[]) {
    // Parse original command line options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--log-level") {
            if (arg.find('=') != std::string::npos) {
                options.logLevel = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--verbose" || arg == "-V") {
            options.verbose = true;
        } else if (arg == "--script" || arg == "-s") {
            if (i + 1 < argc) {
                options.scriptFile = argv[++i];
            }
        }
    }
}

void AdvancedCLI::parseAdvancedCommandLine(int argc, char* argv[]) {
    // Parse advanced command line options
    bool foundAdvancedCommand = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--advanced" || arg == "-a") {
            options.advanced = true;
        } else if (options.advanced && !foundAdvancedCommand) {
            // This is the advanced command
            options.advancedCommand = arg;
            foundAdvancedCommand = true;
        } else if (foundAdvancedCommand) {
            // Advanced command arguments
            options.advancedArgs.push_back(arg);
        } else {
            // Parse as original options
            parseCommandLine(i, &argv[i]);  // Parse from current position
            break;  // Stop after first non-advanced command
        }
    }
}

bool AdvancedCLI::executeParsedCommands() {
    if (!commandParser->parseArguments(1, nullptr)) {  // Simplified - would pass actual argc/argv
        logger->log("Error: Failed to parse command line", LogLevel::Error);
        return false;
    }
    
    // Execute all command groups
    commandParser->executeBrainCommands();
    commandParser->executeSimulationCommands();
    commandParser->executeInspectionCommands();
    commandParser->executeAnalysisCommands();
    commandParser->executeDebuggingCommands();
    commandParser->executeExportImportCommands();
    commandParser->executeAdvancedCommands();
    
    return true;
}

void AdvancedCLI::printVersion() {
    std::cout << "NLM Advanced CLI v2.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void AdvancedCLI::printHelp() {
    std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic commands:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration file" << std::endl;
    std::cout << "  --script <file>         Run script file" << std::endl;
    std::cout << "  --quiet, -q             Run in quiet mode" << std::endl;
    std::cout << "  --verbose, -V           Run in verbose mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced commands:" << std::endl;
    std::cout << "  --advanced, -a          Use advanced command mode" << std::endl;
    std::cout << "  <command> <args>        Execute advanced command" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on specific commands, use --advanced <command> --help" << std::endl;
}

void AdvancedCLI::printAdvancedHelp() {
    commandFramework->printHelp();
}

bool AdvancedCLI::handleOriginalCommandLine(int argc, char* argv[]) {
    parseCommandLine(argc, argv);
    
    if (options.help) {
        printHelp();
        return true;
    }
    
    if (options.version) {
        printVersion();
        return true;
    }
    
    // Original CLI execution (placeholder)
    logger->log("Original CLI execution (placeholder)", LogLevel::Info);
    
    return true;
}

void AdvancedCLI::registerCommands() {
    // Register command groups
    commandFramework->registerCommandGroup("brain", "Brain Management Commands");
    commandFramework->registerCommandGroup("simulation", "Simulation Control Commands");
    commandFramework->registerCommandGroup("inspection", "Real-time Inspection Commands");
    commandFramework->registerCommandGroup("analysis", "Analysis Commands");
    commandFramework->registerCommandGroup("debugging", "Debugging Commands");
    commandFramework->registerCommandGroup("export", "Export/Import Commands");
    commandFramework->registerCommandGroup("advanced", "Advanced Features");
    
    // Register individual commands (placeholder)
    // commandFramework->registerCommand("--brain-info", "Display brain information", 
    //                                  "Show detailed brain state information", "brain");
}
