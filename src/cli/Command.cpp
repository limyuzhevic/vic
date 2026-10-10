// CommandLine implementation

#include "CommandLine.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>

namespace nlm {
namespace cli {

CommandLine::CommandLine() {
    helpSystem = std::make_unique<HelpSystem>();
}

void CommandLine::addCommand(std::unique_ptr<Command> cmd) {
    commands.push_back(std::move(cmd));
}

Command* CommandLine::getCommand(const std::string& name) const {
    for (const auto& cmd : commands) {
        if (cmd->getName() == name) {
            return cmd.get();
        }
    }
    return nullptr;
}

bool CommandLine::execute(int argc, char** argv) {
    if (!parseGlobalOptions(argc, argv)) {
        return false;
    }
    
    // Help and version requests
    if (globalOptions.helpRequested) {
        showHelp();
        return true;
    }
    
    if (globalOptions.versionRequested) {
        showVersion();
        return true;
    }
    
    if (argc < 2) {
        showHelp();
        return false;
    }
    
    std::string commandName = argv[1];
    Command* cmd = getCommand(commandName);
    if (!cmd) {
        StatusReporter::error("Unknown command: " + commandName);
        showHelp();
        return false;
    }
    
    // Convert remaining arguments
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (!parseCommandOptions(commandName, args)) {
        return false;
    }
    
    ProgressIndicator progress(!globalOptions.quiet);
    if (globalOptions.enableProgress && !globalOptions.quiet && 
        cmd->getCategory() == CommandCategory::SIMULATION) {
        progress.start("Executing command: " + commandName, 100);
    }
    
    // Execute the command
    bool success = cmd->execute(args);
    
    if (progress.isEnabled()) {
        progress.complete();
    }
    
    return success;
}

bool CommandLine::parseGlobalOptions(int argc, char** argv) {
    OptionParser parser;
    parser.addGlobalOptions();
    
    // Convert argv to vector of strings
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (!parser.parse(args)) {
        StatusReporter::error("Invalid global options");
        return false;
    }
    
    // Extract global options from parsed values
    globalOptions.verbose = parser.hasOption("--verbose");
    globalOptions.quiet = parser.hasOption("--quiet");
    globalOptions.logLevel = parser.getOption("--log-level");
    globalOptions.configFile = parser.getOption("--config") != "" ? 
                               parser.getOption("--config") : globalOptions.configFile;
    globalOptions.enableProfiling = parser.hasOption("--profile");
    globalOptions.enableBenchmarking = parser.hasOption("--benchmark");
    
    // Parse threads option
    std::string threadsStr = parser.getOption("--threads");
    if (!threadsStr.empty()) {
        try {
            globalOptions.maxThreads = std::stoi(threadsStr);
        } catch (...) {
            globalOptions.maxThreads = 1;
        }
    }
    
    // Parse output options
    globalOptions.outputFormat = parser.getOption("--output-format");
    globalOptions.outputFile = parser.getOption("--output");
    
    return true;
}

bool CommandLine::parseCommandOptions(const std::string& cmdName,
                                     const std::vector<std::string>& args) {
    Command* cmd = getCommand(cmdName);
    if (!cmd) {
        return false;
    }
    
    // For now, basic argument parsing - can be enhanced later
    return cmd->validateArguments(args);
}

void CommandLine::showHelp(const std::string& commandName) const {
    if (commandName.empty()) {
        StatusReporter::info("NLM - Neural Learning Machine CLI");
        StatusReporter::info("=" * 60);
        StatusReporter::info("Available commands:");
        StatusReporter::info("")
        
        // Group commands by category
        std::unordered_map<CommandCategory, std::vector<Command*>> categorizedCommands;
        
        for (const auto& cmd : commands) {
            categorizedCommands[cmd->getCategory()].push_back(cmd.get());
        }
        
        for (auto& [category, cmds] : categorizedCommands) {
            StatusReporter::info(static_cast<int>(category) + ": " + 
                              CommandCategoryToString(category) + 
                              " commands:");
            for (const auto& cmd : cmds) {
                StatusReporter::info("  " + cmd->getName() + " - " + cmd->getDescription());
            }
            StatusReporter::info("")
        }
        
        StatusReporter::info("For detailed help on a specific command, use:")
        StatusReporter::info("  nlm <command> --help")
        StatusReporter::info("")
        StatusReporter::info("Global options:")
        StatusReporter::info("  --help, -h          Show this help message");
        StatusReporter::info("  --version           Show version information");
        StatusReporter::info("  --verbose, -v       Enable verbose output");
        StatusReporter::info("  --quiet, -q         Suppress non-essential output");
        StatusReporter::info("  --config FILE        Configuration file path");
        StatusReporter::info("  --output FILE        Output file path");
        StatusReporter::info("  --output-format FMT  Output format (text, json, csv)");
        StatusReporter::info("  --profile            Enable profiling");
        StatusReporter::info("  --benchmark          Enable benchmarking");
        StatusReporter::info("  --threads N          Number of threads to use");
    } else {
        Command* cmd = getCommand(commandName);
        if (cmd) {
            cmd->showHelp();
        } else {
            StatusReporter::error("Unknown command: " + commandName);
        }
    }
}

void CommandLine::showVersion() const {
    StatusReporter::info("NLM Neural Learning Machine v0.1.0");
    StatusReporter::info("Phase 2: Real Neural Computation");
    StatusReporter::info("Copyright (c) 2026 NLM Team");
}

std::string CommandLine::CommandCategoryToString(CommandCategory category) const {
    switch (category) {
        case CommandCategory::SIMULATION: return "Simulation";
        case CommandCategory::CONFIGURATION: return "Configuration";
        case CommandCategory::DEBUGGING: return "Debugging";
        case CommandCategory::VALIDATION: return "Validation";
        case CommandCategory::PLUGIN: return "Plugin";
        case CommandCategory::PERFORMANCE: return "Performance";
        case CommandCategory::ANALYSIS: return "Analysis";
        case CommandCategory::UTILITIES: return "Utilities";
        default: return "Unknown";
    }
}

// Command implementation

Command::~Command() = default;

bool Command::validateArguments(const std::vector<std::string>& args) const {
    return true;
}

std::string Command::getUsage() const {
    return "";
}

std::vector<std::string> Command::getExamples() const {
    return {};
}

} // namespace cli
} // namespace nlm
