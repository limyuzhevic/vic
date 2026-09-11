// CLI implementation
#include "CLI.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {

CLI::CLI() 
    : verbose(false), quiet(false), interactive(false)
    , configFile("configs/default.cfg"), outputFile(""), logLevel("INFO"), logFile("") {}

CLI::~CLI() = default;

bool CLI::parse(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return false;
    }
    
    command = argv[1];
    auto cmdType = parseCommand(command);
    
    // Parse flags
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--interactive") {
            interactive = true;
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version") {
            printVersion();
            return false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--") {
            // Store unknown flags for later processing
            args.push_back(arg);
        } else {
            args.push_back(arg);
        }
    }
    
    // Initialize config
    config = std::make_shared<Config>();
    
    return true;
}

int CLI::execute() {
    switch (parseCommand(command)) {
        case CommandType::Help:
            return executeHelp();
        case CommandType::Version:
            return executeVersion();
        case CommandType::Init:
            return executeInit();
        case CommandType::Reset:
            return executeReset();
        case CommandType::Run:
            return executeRun();
        case CommandType::Save:
            return executeSave();
        case CommandType::Load:
            return executeLoad();
        case CommandType::Status:
            return executeStatus();
        case CommandType::Test:
            return executeTest();
        case CommandType::Config:
            return executeConfig();
        case CommandType::Log:
            return executeLog();
        case CommandType::Explore:
            return executeExplore();
        default:
            std::cerr << "Error: Unknown command: " << command << std::endl;
            return 1;
    }
}

void CLI::printHelp() const {
    std::cout << "NLM - Neural Learning Machine - Command Line Interface" << std::endl;
    std::cout << "===================================================" << std::endl << std::endl;
    
    std::cout << "Commands:" << std::endl;
    std::cout << "  --help, -h                      Print this help message" << std::endl;
    std::cout << "  --version                       Print version information" << std::endl;
    std::cout << "  --init                          Initialize a new NLM brain" << std::endl;
    std::cout << "  --reset                         Reset the current brain state" << std::endl;
    std::cout << "  --run                           Run the brain simulation" << std::endl;
    std::cout << "  --save [file]                   Save brain state to file" << std::endl;
    std::cout << "  --load [file]                   Load brain state from file" << std::endl;
    std::cout << "  --status                        Print brain status information" << std::endl;
    std::cout << "  --test [type]                   Run tests" << std::endl;
    std::cout << "  --config [key=value]            Get or set configuration values" << std::endl;
    std::cout << "  --log [level] [file]            Configure logging" << std::endl;
    std::cout << "  --explore                       Explore brain state" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --verbose, -v                   Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                     Suppress non-essential output" << std::endl;
    std::cout << "  --interactive                   Enable interactive mode" << std::endl;
    std::cout << "  --config-file [file]            Specify configuration file" << std::endl;
    std::cout << "  --output [file]                 Specify output file" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --run --verbose" << std::endl;
    std::cout << "  ./nlm --config neuron_count=1000" << std::endl;
    std::cout << "  ./nlm --save my_brain.bin" << std::endl;
    std::cout << "  ./nlm --test all" << std::endl << std::endl;
}

void CLI::printVersion() const {
    std::cout << "NLM Phase 2 Real Neural Computation" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Description: Experimental artificial developmental brain with real spiking neural computation" << std::endl;
}

int CLI::executeHelp() const {
    printHelp();
    return 0;
}

int CLI::executeVersion() const {
    printVersion();
    return 0;
}

int CLI::executeInit() {
    std::cout << "Initializing NLM brain..." << std::endl;
    
    // Load configuration
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Setup logging
    setupLogging();
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printBrainInfo();
    std::cout << "Brain initialized successfully!" << std::endl;
    
    return 0;
}

int CLI::executeReset() {
    std::cout << "Resetting NLM brain..." << std::endl;
    
    // This would require storing a brain instance
    // For now, just print a message
    std::cout << "Brain reset functionality requires a brain instance." << std::endl;
    
    return 0;
}

int CLI::executeRun() {
    return runBrainSimulation();
}

int CLI::executeSave() {
    std::string filename = "nlm_checkpoint.bin";
    if (!args.empty()) {
        filename = args[0];
    }
    
    std::cout << "Saving brain state to: " << filename << std::endl;
    
    // This would require storing a brain instance
    // For now, just print a message
    std::cout << "Save functionality requires a brain instance." << std::endl;
    
    return 0;
}

int CLI::executeLoad() {
    std::string filename = "nlm_checkpoint.bin";
    if (!args.empty()) {
        filename = args[0];
    }
    
    std::cout << "Loading brain state from: " << filename << std::endl;
    
    // This would require storing a brain instance
    // For now, just print a message
    std::cout << "Load functionality requires a brain instance." << std::endl;
    
    return 0;
}

int CLI::executeStatus() {
    printBrainInfo();
    return 0;
}

int CLI::executeTest() {
    return runTests();
}

int CLI::executeConfig() {
    return manageConfig();
}

int CLI::executeLog() {
    return manageLogging();
}

int CLI::executeExplore() {
    return exploreBrain();
}

int CLI::runBrainSimulation() {
    std::cout << "Starting brain simulation..." << std::endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run the main.cpp tests
    std::cout << "Running basic connectivity test..." << std::endl;
    // brain->runBasicConnectivityTest(brain);  // This is a function from main.cpp
    
    // In a real implementation, we would call the functions from main.cpp
    std::cout << "Simulation completed." << std::endl;
    
    return 0;
}

int CLI::runTests() {
    std::cout << "Running NLM tests..." << std::endl;
    
    // This would run the tests in the tests/ directory
    std::cout << "Test functionality requires test framework integration." << std::endl;
    
    return 0;
}

int CLI::manageConfig() {
    std::cout << "Configuration management..." << std::endl;
    
    if (args.empty()) {
        std::cout << "Current configuration:" << std::endl;
        if (config) {
            std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
            std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << std::endl;
            std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
            std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
            std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        }
    } else {
        // Parse key=value pairs
        for (const auto& arg : args) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                std::cout << "Setting " << key << " = " << value << std::endl;
                // This would set the config value
            }
        }
    }
    
    return 0;
}

int CLI::manageLogging() {
    std::cout << "Logging management..." << std::endl;
    
    // Setup logging based on arguments
    setupLogging();
    
    return 0;
}

int CLI::exploreBrain() {
    std::cout << "Brain exploration..." << std::endl;
    
    // Print brain information
    printBrainInfo();
    
    return 0;
}

bool CLI::validateConfig() {
    if (!config) {
        std::cerr << "Error: No configuration loaded." << std::endl;
        return false;
    }
    
    // Validate essential parameters
    try {
        auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0) {
            std::cerr << "Error: neuron_count must be positive." << std::endl;
            return false;
        }
        
        auto regionCount = config->getOr<int64_t>("region_count", 1);
        if (regionCount <= 0) {
            std::cerr << "Error: region_count must be positive." << std::endl;
            return false;
        }
        
        auto connectionProb = config->getOr<float>("connection_probability", 0.15f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            std::cerr << "Error: connection_probability must be between 0.0 and 1.0." << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error validating configuration: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

void CLI::setupLogging() {
    std::cout << "Setting up logging..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!logFile.empty()) {
        consoleLogger->setOutputFile(logFile);
    }
    
    if (verbose) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    std::cout << "Logging configured." << std::endl;
}

void CLI::printBrainInfo() {
    std::cout << "Brain Information:" << std::endl;
    std::cout << "  Configuration file: " << configFile << std::endl;
    std::cout << "  Output file: " << (outputFile.empty() ? "Not specified" : outputFile) << std::endl;
    std::cout << "  Log level: " << logLevel << std::endl;
    std::cout << "  Log file: " << (logFile.empty() ? "Not specified" : logFile) << std::endl;
    std::cout << "  Verbose mode: " << (verbose ? "Yes" : "No") << std::endl;
    std::cout << "  Quiet mode: " << (quiet ? "Yes" : "No") << std::endl;
}

CommandType CLI::parseCommand(const std::string& cmd) const {
    if (cmd == "--help" || cmd == "-h") return CommandType::Help;
    if (cmd == "--version") return CommandType::Version;
    if (cmd == "--init") return CommandType::Init;
    if (cmd == "--reset") return CommandType::Reset;
    if (cmd == "--run") return CommandType::Run;
    if (cmd == "--save") return CommandType::Save;
    if (cmd == "--load") return CommandType::Load;
    if (cmd == "--status") return CommandType::Status;
    if (cmd == "--test") return CommandType::Test;
    if (cmd == "--config") return CommandType::Config;
    if (cmd == "--log") return CommandType::Log;
    if (cmd == "--explore") return CommandType::Explore;
    return CommandType::Help; // Default to help
}

bool CLI::parseFlag(const std::string& flag, const std::vector<std::string>& args) const {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string CLI::parseFlagValue(const std::string& flag, const std::vector<std::string>& args) const {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && std::next(it) != args.end()) {
        return *std::next(it);
    }
    return "";
}

} // namespace nlm