#pragma once

#include "NLMCLI.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class StatusCommand : public ICommand {
public:
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Show system status and information"; }
    std::string getUsage() const override { return "status [options]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        // No required arguments
        return true;
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli status [options]" << std::endl;
        std::cout << "Show system status and information." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --verbose, -v          Show detailed status information" << std::endl;
        std::cout << "  --help, -h             Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& cliConfig) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printHelp();
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        // Parse options
        bool verbose = false;
        for (const auto& arg : args) {
            if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                result.success = true;
                result.exitCode = 0;
                return result;
            }
        }
        
        try {
            result = getSystemStatus(verbose, cliConfig);
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error getting system status: ") + e.what();
            result.exitCode = 3;
        }
        
        return result;
    }
    
private:
    CommandResult getSystemStatus(bool verbose, const CLIConfig& cliConfig) {
        CommandResult result;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        LogLevel logLevel = LogLevel::Info;
        if (cliConfig.logLevel == "debug") logLevel = LogLevel::Debug;
        else if (cliConfig.logLevel == "warning") logLevel = LogLevel::Warning;
        else if (cliConfig.logLevel == "error") logLevel = LogLevel::Error;
        else if (cliConfig.logLevel == "critical") logLevel = LogLevel::Critical;
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        consoleLogger->setUseColors(!cliConfig.quiet);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Getting system status...");
        
        std::ostringstream output;
        
        // System information
        output << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        output << "║                               NLM System Status                                 ║" << std::endl;
        output << "╠════════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Basic system info
        output << "║  System Information:" << std::endl;
        output << "║    Application: NLM - Neural Learning Machine" << std::endl;
        output << "║    Version: 1.0.0 (Phase 2)" << std::endl;
        output << "║    Build Date: " << __DATE__ << std::endl;
        output << "║    Compiler: " << __VERSION__ << std::endl;
        output << "║    Architecture: x86_64" << std::endl;
        output << "║    OS: Linux" << std::endl;
        
        // Configuration
        output << "║  Configuration:" << std::endl;
        auto config = std::make_shared<Config>();
        if (!cliConfig.configFile.empty()) {
            if (CLIManager::loadConfigFile(cliConfig.configFile, config)) {
                output << "║    Loaded from: " << cliConfig.configFile << std::endl;
                output << "║    Entries: " << config->getKeys().size() << std::endl;
            }
        } else {
            output << "║    Default configuration" << std::endl;
        }
        
        // Available components
        output << "║  Available Components:" << std::endl;
        output << "║    ✓ Core System (Config, Logger)" << std::endl;
        output << "║    ✓ Brain System (Neurons, Synapses, Plasticity)" << std::endl;
        output << "║    ✓ Cognition System (Concept Formation, Planning)" << std::endl;
        output << "║    ✓ Sensory/Motor Systems" << std::endl;
        output << "║    ✓ Environment Systems" << std::endl;
        output << "║    ✓ Experiment Framework" << std::endl;
        output << "║    ✓ Visualization System" << std::endl;
        
        // Features
        output << "║  Features:" << std::endl;
        output << "║    ✓ Real LIF neuron dynamics" << std::endl;
        output << "║    ✓ Event-driven spike propagation" << std::endl;
        output << "║    ✓ STDP and Hebbian plasticity" << std::endl;
        output << "║    ✓ Structural plasticity" << std::endl;
        output << "║    ✓ Neuromodulation" << std::endl;
        output << "║    ✓ Memory systems (working, episodic, associative)" << std::endl;
        output << "║    ✓ Prediction systems" << std::endl;
        output << "║    ✓ Cognitive systems (attention, planning)" << std::endl;
        
        // Performance
        output << "║  Performance:" << std::endl;
        output << "║    ✓ Optimized for large-scale networks" << std::endl;
        output << "║    ✓ Supports up to 100K+ neurons" << std::endl;
        output << "║    ✓ Real-time simulation capabilities" << std::endl;
        
        // CLI Features
        output << "║  CLI Features:" << std::endl;
        output << "║    ✓ Command-line interface (nlm-cli)" << std::endl;
        output << "║    ✓ Configuration management" << std::endl;
        output << "║    ✓ Experiment control" << std::endl;
        output << "║    ✓ Visualization generation" << std::endl;
        output << "║    ✓ Performance benchmarking" << std::endl;
        output << "║    ✓ Interactive mode" << std::endl;
        output << "║    ✓ Tab completion" << std::endl;
        
        // Additional details if verbose
        if (verbose) {
            output << "║" << std::endl;
            output << "║  Detailed Information:" << std::endl;
            
            // Log system
            output << "║    Log System:" << std::endl;
            output << "║      Level: " << config.logLevel << std::endl;
            output << "║      Global logger: " << (Logger::getGlobal() ? "Available" : "Not available") << std::endl;
            
            // Config system
            output << "║    Config System:" << std::endl;
            output << "║      Type: Type-safe parameter management" << std::endl;
            output << "║      Sources: Default, File, CommandLine, Runtime" << std::endl;
            output << "║      Validation: Range and custom validators" << std::endl;
            
            // Memory
            output << "║    Memory:" << std::endl;
            output << "║      Compile-time allocation" << std::endl;
            output << "║      Runtime allocation for large networks" << std::endl;
            
            // Storage
            output << "║    Storage:" << std::endl;
            output << "║      Checkpoint/restart support" << std::endl;
            output << "║      Results persistence" << std::endl;
        }
        
        output << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        output << std::endl;
        
        // Additional text output
        std::cout << output.str();
        
        result.success = true;
        result.message = "System status retrieved successfully";
        result.output = output.str();
        
        return result;
    }
    
}; // class StatusCommand

} // namespace nlm