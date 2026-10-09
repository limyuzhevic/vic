// NLM CLI - Neural Learning Machine Command Line Interface
// 
// Main entry point for the NLM command-line interface with support for
// various subcommands and configuration management.

#include "CLIParser.hpp"
#include "CommandBase.hpp"
#include "RunCommand.hpp"
#include "ConfigCommand.hpp"
#include "SaveLoadCommand.hpp"
#include "BenchmarkCommand.hpp"
#include "DebugCommand.hpp"
#include "VisualizeCommand.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>

using namespace nlm;

// NLM Logo
const std::string NLM_LOGO = R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
)";

void printHelp(const std::map<std::string, CommandBase*>& commands) {
    std::cout << NLM_LOGO;
    std::cout << std::endl;
    std::cout << "Usage: nlm <command> [options] [arguments]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    
    for (const auto& pair : commands) {
        const std::string& cmd = pair.first;
        CommandBase* command = pair.second;
        
        // Left-align command names
        std::cout << "  " << std::left << std::setw(15) << cmd 
                  << "  " << command->getDescription() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Use 'nlm <command> --help' for more information about a specific command." << std::endl;
}

void printVersion() {
    std::cout << NLM_LOGO;
    std::cout << std::endl;
    std::cout << "Version: 2.0.0 (Phase 2)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;
    std::cout << "NLM is an experimental artificial developmental brain system" << std::endl;
    std::cout << "implementing real spiking neural computation with plasticity." << std::endl;
}

int main(int argc, char** argv) {
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize NLM configuration system
    auto config = std::make_shared<Config>();
    
    // Load configuration from file if specified
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 8) == "--config=") {
            configFile = arg.substr(8);
        }
    }
    
    config->loadFromFile(configFile);
    config->loadFromArgs(argc, argv);
    
    // Initialize default configuration values
    config->set("nlm.version", "2.0.0", ConfigSource::Default);
    config->set("nlm.author", "NLM Development Team", ConfigSource::Default);
    config->set("nlm.license", "MIT", ConfigSource::Default);
    
    // Register all available commands
    std::map<std::string, CommandBase*> commands;
    
    commands["run"] = new RunCommand();
    commands["config"] = new ConfigCommand();
    commands["save"] = new SaveLoadCommand("save");
    commands["load"] = new SaveLoadCommand("load");
    commands["benchmark"] = new BenchmarkCommand();
    commands["debug"] = new DebugCommand();
    commands["visualize"] = new VisualizeCommand();
    commands["help"] = new HelpCommand(commands);
    commands["version"] = new VersionCommand();
    
    // No arguments or help/version requested
    if (argc <= 1) {
        printHelp(commands);
        return 0;
    }
    
    std::string commandName = argv[1];
    
    // Handle special commands
    if (commandName == "help" || commandName == "--help" || commandName == "-h") {
        printHelp(commands);
        return 0;
    }
    
    if (commandName == "version" || commandName == "--version") {
        printVersion();
        return 0;
    }
    
    // Find and execute the requested command
    auto it = commands.find(commandName);
    if (it == commands.end()) {
        std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
        std::cerr << "Use 'nlm --help' to see available commands." << std::endl;
        return 1;
    }
    
    CommandBase* command = it->second;
    
    // Parse command line arguments
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Create CLI parser
    CLIParser parser(args);
    
    // Parse global options
    bool globalVerbose = parser.getBool("--verbose", "-v", false);
    bool globalQuiet = parser.getBool("--quiet", "-q", false);
    
    // Set log level based on global options
    if (globalVerbose && !globalQuiet) {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (globalQuiet) {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Execute command
    int result = command->execute(config, parser);
    
    // Clean up
    for (auto& pair : commands) {
        delete pair.second;
    }
    
    return result;
}
