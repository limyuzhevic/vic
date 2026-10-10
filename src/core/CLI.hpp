#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <sstream>

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

using namespace nlm;

// CLI Framework
class CLICommand {
public:
    virtual ~CLICommand() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual void execute(const std::vector<std::string>& args) = 0;
    virtual bool requiresBrain() const { return false; }
    virtual bool isGlobal() const { return false; }
};

class CLICommandRegistry {
private:
    std::vector<std::unique_ptr<CLICommand>> commands;
    std::map<std::string, CLICommand*> commandMap;
    
public:
    void registerCommand(std::unique_ptr<CLICommand> cmd) {
        commands.push_back(std::move(cmd));
        auto* ptr = commands.back().get();
        commandMap[ptr->getName()] = ptr;
    }
    
    CLICommand* getCommand(const std::string& name) {
        auto it = commandMap.find(name);
        return (it != commandMap.end()) ? it->second : nullptr;
    }
    
    std::vector<CLICommand*> getAllCommands() {
        std::vector<CLICommand*> result;
        for (auto& cmd : commands) {
            result.push_back(cmd.get());
        }
        return result;
    }
    
    void printHelp() {
        std::cout << "Available commands:" << std::endl;
        std::cout << std::left << std::setw(20) << "Command" 
                  << std::setw(40) << "Description" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        for (auto& cmd : commands) {
            if (cmd->isGlobal()) continue;
            std::cout << std::left << std::setw(20) << cmd->getName()
                      << std::setw(40) << cmd->getDescription() << std::endl;
        }
    }
};

// Global commands implementation
class HelpCommand : public CLICommand {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information about commands"; }
    std::string getUsage() const override { return "help [command]"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "=== NLM Command Line Interface ===" << std::endl;
            std::cout << "Use 'nlm help <command>' for detailed help on a specific command." << std::endl;
            std::cout << "\nCommand categories:" << std::endl;
            std::cout << "  Global Options: help, version, verbose, debug, config" << std::endl;
            std::cout << "  Configuration: load-config, save-config, template, validate" << std::endl;
            std::cout << "  Execution: run-simulation, run-experiment, run-demo" << std::endl;
            std::cout << "  Analysis: analyze-results, benchmark, profile" << std::endl;
            std::cout << "  Utility: check-config, backup, export" << std::endl;
        } else {
            CLICommand* cmd = registry.getCommand(args[0]);
            if (cmd) {
                std::cout << "Command: " << cmd->getName() << std::endl;
                std::cout << "Description: " << cmd->getDescription() << std::endl;
                std::cout << "Usage: " << cmd->getUsage() << std::endl;
            } else {
                std::cout << "Unknown command: " << args[0] << std::endl;
            }
        }
    }
    bool isGlobal() const override { return true; }
    
    static CLICommandRegistry registry;
};

class VersionCommand : public CLICommand {
public:
    std::string getName() const override { return "version"; }
    std::string getDescription() const override { return "Show version information"; }
    std::string getUsage() const override { return "version"; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Version: 2.0.0 (Phase 2 Enhanced)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    bool isGlobal() const override { return true; }
};

class VerboseCommand : public CLICommand {
public:
    std::string getName() const override { return "verbose"; }
    std::string getDescription() const override { return "Enable verbose output"; }
    std::string getUsage() const override { return "verbose [level]"; }
    void execute(const std::vector<std::string>& args) override {
        auto logger = Logger::getInstance();
        LogLevel level = LogLevel::Info;
        
        if (!args.empty()) {
            if (args[0] == "debug") level = LogLevel::Debug;
            else if (args[0] == "info") level = LogLevel::Info;
            else if (args[0] == "warning") level = LogLevel::Warning;
            else if (args[0] == "error") level = LogLevel::Error;
        }
        
        logger->setLevel(level);
        std::cout << "Verbose level set to: " << (level == LogLevel::Debug ? "DEBUG" :
                                                   level == LogLevel::Info ? "INFO" :
                                                   level == LogLevel::Warning ? "WARNING" : "ERROR") << std::endl;
    }
    bool isGlobal() const override { return true; }
};

class DebugCommand : public CLICommand {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { return "Enable debug mode with additional logging"; }
    std::string getUsage() const override { return "debug [true/false]"; }
    void execute(const std::vector<std::string>& args) override {
        bool debug = true;
        if (!args.empty() && args[0] == "false") debug = false;
        
        auto config = std::make_shared<Config>();
        config->set("debug_mode", debug, ConfigSource::CommandLine);
        config->set("log_level", debug ? "Debug" : "Info", ConfigSource::CommandLine);
        
        std::cout << "Debug mode " << (debug ? "enabled" : "disabled") << std::endl;
    }
    bool isGlobal() const override { return true; }
};

class ConfigCommand : public CLICommand {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Configuration file management";
    std::string getUsage() const override { return "config [file]"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Configuration file: default.cfg (use --config to specify)" << std::endl;
        } else {
            std::cout << "Configuration file set to: " << args[0] << std::endl;
        }
    }
    bool isGlobal() const override { return true; }
};

// Configuration commands
class LoadConfigCommand : public CLICommand {
public:
    std::string getName() const override { return "load-config"; }
    std::string getDescription() const override { return "Load configuration from file"; }
    std::string getUsage() const override { return "load-config <filepath>"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No file specified" << std::endl;
            return;
        }
        
        std::string filepath = args[0];
        std::cout << "Loading configuration from: " << filepath << std::endl;
        // Implementation would load config from file
    }
};

class SaveConfigCommand : public CLICommand {
public:
    std::string getName() const override { return "save-config"; }
    std::string getDescription() const override { return "Save current configuration to file"; }
    std::string getUsage() const override { return "save-config <filepath>"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No file specified" << std::endl;
            return;
        }
        
        std::string filepath = args[0];
        std::cout << "Saving configuration to: " << filepath << std::endl;
        // Implementation would save config to file
    }
};

class TemplateCommand : public CLICommand {
public:
    std::string getName() const override { return "template"; }
    std::string getDescription() const override { return "Show configuration templates"; }
    std::string getUsage() const override { return "template [name]"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Available templates:" << std::endl;
            std::cout << "  default - Default NLM configuration" << std::endl;
            std::cout << "  demo - Demo configuration for quick testing" << std::endl;
            std::cout << "  benchmark - Benchmark configuration for performance testing" << std::endl;
            std::cout << "  research - Research configuration for scientific experiments" << std::endl;
        } else {
            std::cout << "Template: " << args[0] << std::endl;
            // Implementation would show template content
        }
    }
};

class ValidateCommand : public CLICommand {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { return "Validate configuration or experiment setup"; }
    std::string getUsage() const override { return "validate [config|experiment]"; }
    void execute(const std::vector<std::string>& args) override {
        std::string type = args.empty() ? "config" : args[0];
        std::cout << "Validating " << type << "..." << std::endl;
        // Implementation would validate config or experiment
    }
};

// Execution commands
class RunSimulationCommand : public CLICommand {
public:
    std::string getName() const override { return "run-simulation"; }
    std::string getDescription() const override { return "Run neural simulation with current configuration"; }
    std::string getUsage() const override { return "run-simulation [steps] [--config file] [--brain]"; }
    bool requiresBrain() const override { return true; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "Running neural simulation..." << std::endl;
        // Implementation would run simulation
    }
};

class RunExperimentCommand : public CLICommand {
public:
    std::string getName() const override { return "run-experiment"; }
    std::string getDescription() const override { return "Run scientific experiment";
    std::string getUsage() const override { return "run-experiment <type> [steps] [--config file] [--brain]"; }
    bool requiresBrain() const override { return true; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No experiment type specified" << std::endl;
            return;
        }
        std::cout << "Running experiment: " << args[0] << std::endl;
        // Implementation would run experiment
    }
};

class RunDemoCommand : public CLICommand {
public:
    std::string getName() const override { return "run-demo"; }
    std::string getDescription() const override { return "Run demonstration of NLM capabilities";
    std::string getUsage() const override { return "run-demo <phase> [args] [--config file] [--brain]"; }
    bool requiresBrain() const override { return true; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No demo phase specified" << std::endl;
            return;
        }
        std::cout << "Running demo: " << args[0] << std::endl;
        // Implementation would run demo
    }
};

// Analysis commands
class AnalyzeResultsCommand : public CLICommand {
public:
    std::string getName() const override { return "analyze-results"; }
    std::string getDescription() const override { return "Analyze experiment results"; }
    std::string getUsage() const override { return "analyze-results <result_file> [--output format]"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No result file specified" << std::endl;
            return;
        }
        std::cout << "Analyzing results from: " << args[0] << std::endl;
        // Implementation would analyze results
    }
};

class BenchmarkCommand : public CLICommand {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Run performance benchmark"; }
    std::string getUsage() const override { return "benchmark [--config file] [--brain] [--output file]"; }
    bool requiresBrain() const override { return true; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "Running benchmark..." << std::endl;
        // Implementation would run benchmark
    }
};

class ProfileCommand : public CLICommand {
public:
    std::string getName() const override { return "profile"; }
    std::string getDescription() const override { return "Profile performance and resource usage"; }
    std::string getUsage() const override { return "profile [--config file] [--brain] [--output file]"; }
    bool requiresBrain() const override { return true; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "Profiling performance..." << std::endl;
        // Implementation would profile performance
    }
};

// Utility commands
class CheckConfigCommand : public CLICommand {
public:
    std::string getName() const override { return "check-config"; }
    std::string getDescription() const override { return "Check configuration validity and completeness"; }
    std::string getUsage() const override { return "check-config [--config file]"; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "Checking configuration..." << std::endl;
        // Implementation would check config
    }
};

class BackupCommand : public CLICommand {
public:
    std::string getName() const override { return "backup"; }
    std::string getDescription() const override { return "Create backup of current state"; }
    std::string getUsage() const override { return "backup [--config file] [--output file]"; }
    void execute(const std::vector<std::string>& args) override {
        std::cout << "Creating backup..." << std::endl;
        // Implementation would create backup
    }
};

class ExportCommand : public CLICommand {
public:
    std::string getName() const override { return "export"; }
    std::string getDescription() const override { return "Export results or configuration"; }
    std::string getUsage() const override { return "export <type> <file> [--format json|xml|csv]"; }
    void execute(const std::vector<std::string>& args) override {
        if (args.size() < 2) {
            std::cout << "Error: Missing type or file parameter" << std::endl;
            return;
        }
        std::cout << "Exporting " << args[0] << " to " << args[1] << std::endl;
        // Implementation would export data
    }
};

// Global registry instance
CLICommandRegistry HelpCommand::registry;
CLICommandRegistry VersionCommand::registry;
CLICommandRegistry VerboseCommand::registry;
CLICommandRegistry DebugCommand::registry;
CLICommandRegistry ConfigCommand::registry;

// Command parser
class CLIParser {
private:
    CLICommandRegistry& registry;
    
public:
    CLIParser(CLICommandRegistry& reg) : registry(reg) {}
    
    bool parseCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            return false;
        }
        
        std::string cmdName = args[0];
        CLICommand* cmd = registry.getCommand(cmdName);
        
        if (!cmd) {
            std::cout << "Unknown command: " << cmdName << std::endl;
            std::cout << "Use 'nlm help' to see available commands." << std::endl;
            return false;
        }
        
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        cmd->execute(cmdArgs);
        return true;
    }
};