// NLM Command-Line Interface (nlm_command)
// Main entry point for the command-line tool that provides real-time control
// and monitoring of NLM brain simulations

#include "CommandLineInterface.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Forward declarations for main functions
void printBanner();
void parseCommandLineArguments(int argc, char** argv, std::string& configFile, 
                               bool& interactiveMode, bool& batchMode,
                               std::string& batchCommand, std::string& scriptFile);
void showVersion();
void showUsage(const std::string& programName);

// Main function for nlm_command executable
int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Command-Line Interface...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Command-Line Interface ===");
    NLM_LOG_INFO("Starting command-line interface for NLM brain control");
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool interactiveMode = true;
    bool batchMode = false;
    std::string batchCommand;
    std::string scriptFile;
    
    parseCommandLineArguments(argc, argv, configFile, interactiveMode, batchMode,
                              batchCommand, scriptFile);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Create agent brain
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    
    // Initialize world
    SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);  // 100x100 world with 16x16 vision
    agentBrain->initialize(world);
    
    // Initialize command-line interface
    CommandLineInterface cli;
    cli.initialize(brain, agentBrain);
    
    // Show available commands
    std::cout << cli.showHelp() << std::endl;
    
    // Execute commands based on mode
    if (batchMode && !batchCommand.empty()) {
        std::cout << "Executing batch command: " + batchCommand << std::endl;
        std::string result = cli.executeCommand(batchCommand);
        std::cout << result << std::endl;
    } else if (!scriptFile.empty()) {
        std::cout << "Executing commands from script: " + scriptFile << std::endl;
        // TODO: Implement script execution
        std::cout << "Script execution not implemented yet." << std::endl;
    } else if (interactiveMode) {
        std::cout << "Interactive mode. Type 'help' for available commands." << std::endl;
        std::string command;
        while (true) {
            std::cout << "nlm_command> ";
            std::getline(std::cin, command);
            
            if (command == "exit" || command == "quit") {
                std::cout << "Exiting..." << std::endl;
                break;
            }
            
            if (!command.empty()) {
                std::string result = cli.executeCommand(command);
                std::cout << result << std::endl;
            }
        }
    } else {
        std::cout << "No commands specified." << std::endl;
        std::cout << "Use --help for usage information." << std::endl;
    }
    
    NLM_LOG_INFO("NLM Command-Line Interface finished.");
    
    return 0;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM COMMAND-LINE INTERFACE                               ║
    ║     Neural Learning Machine Runtime Control                    ║
    ║                                                               ║
    ║     Real-time control and monitoring of NLM brains            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void parseCommandLineArguments(int argc, char** argv, std::string& configFile,
                               bool& interactiveMode, bool& batchMode,
                               std::string& batchCommand, std::string& scriptFile) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showUsage(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            showVersion();
            exit(0);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--batch" && i + 1 < argc) {
            batchMode = true;
            batchCommand = argv[++i];
            interactiveMode = false;
        } else if (arg == "--script" && i + 1 < argc) {
            scriptFile = argv[++i];
            interactiveMode = false;
        } else if (arg == "--non-interactive") {
            interactiveMode = false;
        } else if (arg == "--daemon") {
            // TODO: Implement daemon mode
            std::cout << "Daemon mode not implemented yet." << std::endl;
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            showUsage(argv[0]);
            exit(1);
        }
    }
}

void showVersion() {
    std::cout << "NLM Command-Line Interface v1.0.0" << std::endl;
    std::cout << "Built for NLM Phase 2: Real Neural Computation" << std::endl;
}

void showUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --batch <command>       Execute a single command in batch mode" << std::endl;
    std::cout << "  --script <file>         Execute commands from script file" << std::endl;
    std::cout << "  --non-interactive       Run without interactive mode" << std::endl;
    std::cout << "  --daemon                Run as daemon process (not implemented)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --batch \"step 10; save test.bin\"" << std::endl;
    std::cout << "  " << programName << " --script commands.txt" << std::endl;
    std::cout << "  " << programName << " --config my_config.cfg" << std::endl;
}

} // namespace nlm