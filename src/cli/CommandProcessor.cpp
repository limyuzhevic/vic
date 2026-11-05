// NLM CLI Command Processor
// Processes and executes shell commands

#include "CommandProcessor.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace nlm;

CommandProcessor::CommandProcessor() : verboseMode(false) {
    // Register standard commands
    registerCommand("help", "Show help information", [this](const std::vector<std::string>& args) {
        showHelpCommand();
    });
    
    registerCommand("run", "Run a NLM demo", [this](const std::vector<std::string>& args) {
        runDemoCommand(args);
    });
    
    registerCommand("benchmark", "Run performance benchmarks", [this](const std::vector<std::string>& args) {
        benchmarkCommand(args);
    });
    
    registerCommand("experiment", "Run an experiment", [this](const std::vector<std::string>& args) {
        experimentCommand(args);
    });
    
    registerCommand("batch", "Run batch processing", [this](const std::vector<std::string>& args) {
        batchCommand(args);
    });
    
    registerCommand("config", "Show current configuration", [this](const std::vector<std::string>& args) {
        configCommand(args);
    });
    
    registerCommand("status", "Show system status", [this](const std::vector<std::string>& args) {
        statusCommand(args);
    });
    
    registerCommand("verbose", "Enable verbose output", [this](const std::vector<std::string>& args) {
        setVerboseMode(true);
    });
    
    registerCommand("quiet", "Disable verbose output", [this](const std::vector<std::string>& args) {
        setVerboseMode(false);
    });
}

CommandProcessor::~CommandProcessor() = default;

void CommandProcessor::registerCommand(const std::string& name, const std::string& description, 
                                       const CommandHandler& handler) {
    commands[name] = {description, handler};
    commandNames.push_back(name);
}

CommandResult CommandProcessor::execute(const std::string& commandLine) {
    CommandResult result;
    
    // Parse command line
    std::vector<std::string> tokens = tokenize(commandLine);
    if (tokens.empty()) {
        result.success = false;
        result.errorMessage = "Empty command";
        return result;
    }
    
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    // Find command
    auto it = commands.find(cmd);
    if (it == commands.end()) {
        result.success = false;
        result.errorMessage = "Unknown command: " + cmd;
        return result;
    }
    
    // Execute command
    try {
        CommandHandler handler = it->second.handler;
        result = handler(tokens);
        
        // Add command to history
        std::stringstream ss;
        ss << commandLine;
        commandHistory.push_back(commandLine);
        
        return result;
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("Command execution failed: ") + e.what();
        return result;
    }
}

std::vector<std::string> CommandProcessor::tokenize(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::stringstream ss(commandLine);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void CommandProcessor::showHelpCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    if (!args.empty() && args[0] == "detailed") {
        // Show detailed help with examples
        result.success = true;
        result.output = "Detailed NLM Shell Commands:\n\n";
        for (const auto& name : commandNames) {
            const auto& cmd = commands[name];
            result.output += "  " + name + " - " + cmd.description + "\n";
        }
        result.output += "\nUse 'command help <command>' for specific command details.\n";
    } else {
        // Show basic help
        result.success = true;
        result.output = "Available commands:\n";
        for (const auto& name : commandNames) {
            const auto& cmd = commands[name];
            result.output += "  " + name + " - " + cmd.description + "\n";
        }
        result.output += "\nUse 'help detailed' for more information.\n";
    }
}

void CommandProcessor::runDemoCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    if (args.size() < 2) {
        result.success = false;
        result.errorMessage = "Usage: run <demo_type> [options]";
        return;
    }
    
    std::string demoType = args[1];
    std::transform(demoType.begin(), demoType.end(), demoType.begin(), ::tolower);
    
    // Build command for main CLI
    std::stringstream cmd;
    cmd << "nlm";
    
    if (verboseMode) {
        cmd << " --verbose";
    }
    
    cmd << " " << demoType;
    
    // Add any additional arguments
    for (size_t i = 2; i < args.size(); ++i) {
        cmd << " " << args[i];
    }
    
    // Execute using system
    int systemResult = std::system(cmd.str().c_str());
    
    if (systemResult == 0) {
        result.success = true;
        result.output = "Demo executed successfully. Check main output for details.\n";
    } else {
        result.success = false;
        result.errorMessage = "Demo execution failed with return code: " + std::to_string(systemResult);
    }
}

void CommandProcessor::benchmarkCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    // Build benchmark command
    std::stringstream cmd;
    cmd << "nlm benchmark";
    
    if (verboseMode) {
        cmd << " --verbose";
    }
    
    // Add any additional arguments
    for (size_t i = 1; i < args.size(); ++i) {
        cmd << " " << args[i];
    }
    
    // Execute using system
    int systemResult = std::system(cmd.str().c_str());
    
    if (systemResult == 0) {
        result.success = true;
        result.output = "Benchmark completed successfully.\n";
    } else {
        result.success = false;
        result.errorMessage = "Benchmark failed with return code: " + std::to_string(systemResult);
    }
}

void CommandProcessor::experimentCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    if (args.size() < 2) {
        result.success = false;
        result.errorMessage = "Usage: experiment <name> [options]";
        return;
    }
    
    std::string experimentName = args[1];
    
    // TODO: Implement experiment execution
    result.success = true;
    result.output = "Experiment execution not yet implemented.\n";
    result.output += "Experiment name: " + experimentName + "\n";
}

void CommandProcessor::batchCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    if (args.size() < 2) {
        result.success = false;
        result.errorMessage = "Usage: batch <batch_file> [options]";
        return;
    }
    
    std::string batchFile = args[1];
    
    // Build batch command
    std::stringstream cmd;
    cmd << "nlm --batch " << batchFile;
    
    if (verboseMode) {
        cmd << " --verbose";
    }
    
    // Execute using system
    int systemResult = std::system(cmd.str().c_str());
    
    if (systemResult == 0) {
        result.success = true;
        result.output = "Batch processing completed successfully.\n";
    } else {
        result.success = false;
        result.errorMessage = "Batch processing failed with return code: " + std::to_string(systemResult);
    }
}

void CommandProcessor::configCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    // Load current config
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile("configs/default.cfg")) {
        result.success = true;
        result.output = "Current configuration:\n";
        result.output += config->summary();
    } else {
        result.success = false;
        result.errorMessage = "Failed to load configuration file.";
    }
}

void CommandProcessor::statusCommand(const std::vector<std::string>& args) {
    CommandResult result;
    
    result.success = true;
    result.output = "NLM Shell Status:\n";
    result.output += "  Version: NLM Phase 6.2\n";
    result.output += "  Commands available: " + std::to_string(commandNames.size()) + "\n";
    result.output += "  Command history: " + std::to_string(commandHistory.size()) + " entries\n";
    result.output += "  Verbose mode: " + (verboseMode ? "enabled" : "disabled") + "\n";
}

void CommandProcessor::setVerboseMode(bool verbose) {
    verboseMode = verbose;
}