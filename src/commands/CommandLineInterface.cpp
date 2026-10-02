#include "commands/CommandLineInterface.hpp"
#include "tools/BrainCtlTool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>

CommandLineInterface::CommandLineInterface() : currentMode_("batch") {}

CommandLineInterface::~CommandLineInterface() {}

bool CommandLineInterface::initialize(int argc, char* argv[]) {
    if (!parseArguments(argc, argv)) {
        return false;
    }
    
    // Register tools
    registerTool(std::make_unique<BrainCtlTool>());
    
    return true;
}

bool CommandLineInterface::run() {
    if (!processCommand()) {
        return false;
    }
    
    return true;
}

bool CommandLineInterface::shutdown() {
    for (auto& pair : tools_) {
        pair.second->shutdown();
    }
    return true;
}

bool CommandLineInterface::registerTool(std::unique_ptr<CommandLineTool> tool) {
    std::string name = tool->getName();
    if (tools_.find(name) != tools_.end()) {
        std::cerr << "Error: Tool already registered: " << name << std::endl;
        return false;
    }
    
    tools_[name] = std::move(tool);
    return true;
}

CommandLineTool* CommandLineInterface::getTool(const std::string& name) const {
    auto it = tools_.find(name);
    if (it != tools_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void CommandLineInterface::printUsage() const {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << "\nUsage:" << std::endl;
    std::cout << "  " << programName_ << " [options] <command> [arguments]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --interactive, -i          Interactive mode" << std::endl;
    std::cout << "  --batch, -b                Batch mode (default)" << std::endl;
    std::cout << "  --script <file>            Run script from file" << std::endl;
    std::cout << "  --verbose, -v              Verbose output" << std::endl;
    std::cout << "  --help, -h                 Show help" << std::endl;
    std::cout << "  --version, --ver           Show version" << std::endl;
    std::cout << "  --config <file>            Load configuration file" << std::endl;
    std::cout << "  --log <file>              Log output to file" << std::endl;
    std::cout << "\nCommands:" << std::endl;
    for (const auto& pair : tools_) {
        std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
    }
}

void CommandLineInterface::printVersion() const {
    std::cout << "NLM Command Line Interface v1.0.0" << std::endl;
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << std::endl;
}

void CommandLineInterface::printHelp() const {
    printUsage();
    std::cout << "\nFor detailed help on a specific tool, use:" << std::endl;
    std::cout << "  " << programName_ << " <tool-name> --help" << std::endl;
}

bool CommandLineInterface::parseArguments(int argc, char* argv[]) {
    programName_ = std::string(argv[0]);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version" || arg == "--ver") {
            printVersion();
            return false;
        } else if (arg == "--interactive" || arg == "-i") {
            currentMode_ = "interactive";
        } else if (arg == "--batch" || arg == "-b") {
            currentMode_ = "batch";
        } else if (arg == "--script" && i + 1 < argc) {
            // Process script file
            std::string scriptFile = argv[++i];
            return handleScriptMode(scriptFile);
        } else if (arg == "--config" && i + 1 < argc) {
            // TODO: Load config file
            i++;
        } else if (arg == "--log" && i + 1 < argc) {
            // TODO: Setup log file
            i++;
        } else if (arg.substr(0, 2) == "--") {
            std::cerr << "Warning: Unknown option: " << arg << std::endl;
        } else {
            // Check if this is a known tool
            std::string toolName = arg;
            if (tools_.find(toolName) != tools_.end()) {
                // Skip tool names for now
                args_.push_back(toolName);
            } else {
                args_.push_back(arg);
            }
        }
    }
    
    // If no tool specified, show help
    if (args_.empty()) {
        printHelp();
        return false;
    }
    
    // Extract tool name and arguments
    if (!args_.empty()) {
        std::string toolName = args_[0];
        args_.erase(args_.begin());
        
        CommandLineTool* tool = getTool(toolName);
        if (!tool) {
            std::cerr << "Error: Unknown tool: " << toolName << std::endl;
            return false;
        }
        
        if (!tool->initialize()) {
            std::cerr << "Error: Failed to initialize tool: " << toolName << std::endl;
            return false;
        }
        
        return true;
    }
    
    return true;
}

bool CommandLineInterface::processCommand() const {
    if (tools_.empty()) {
        return true;
    }
    
    // For now, just show what's available
    std::cout << "Available tools:" << std::endl;
    for (const auto& pair : tools_) {
        std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
    }
    
    return true;
}

bool CommandLineInterface::handleInteractiveMode() const {
    std::cout << "Entering interactive mode. Type 'help' for commands." << std::endl;
    
    std::string command;
    while (std::getline(std::cin, command)) {
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "help") {
            printHelp();
        } else {
            // TODO: Process command
            std::cout << "Command not implemented yet: " << command << std::endl;
        }
    }
    
    return true;
}

bool CommandLineInterface::handleBatchMode() const {
    // TODO: Process batch commands
    std::cout << "Batch mode not implemented yet." << std::endl;
    return true;
}

bool CommandLineInterface::handleScriptMode(const std::string& scriptPath) const {
    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open script file: " << scriptPath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // TODO: Process script line
        if (!line.empty() && line[0] != '#') {
            std::cout << "Script line: " << line << std::endl;
        }
    }
    
    file.close();
    return true;
}