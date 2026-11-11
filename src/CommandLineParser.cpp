#include "CommandLineParser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

namespace nlm {

CommandLineParser::CommandLineParser() {
    initializeOptions();
    initializeCommands();
}

CommandLineParser::~CommandLineParser() {}

void CommandLineParser::initializeOptions() {
    // Define command line options
    options = {
        {"h", "help", "Show this help message", "", false, "", false},
        {"v", "version", "Show version information", "", false, "", false},
        {"c", "config", "Load configuration from file", "", true, "config_file", false},
        {"", "config", "Load configuration from file (long form)", "", true, "config_file", false},
        {"n", "neurons", "Number of neurons for simulation", "1000", true, "count", false},
        {"", "neuron-count", "Number of neurons for simulation", "1000", true, "count", false},
        {"r", "regions", "Number of regions for simulation", "1", true, "count", false},
        {"", "region-count", "Number of regions for simulation", "1", true, "count", false},
        {"p", "probability", "Connection probability (0.0-1.acia)", "0.1", true, "value", false},
        {"", "probability", "Connection probability (0.0-1.acia)", "0.1", true, "value", false},
        {"t", "timestep", "Simulation timestep in seconds", "0.001", true, "seconds", false},
        {"", "timestep", "Simulation timestep in seconds", "0.001", true, "seconds", false},
        {"s", "steps", "Number of simulation steps to run", "10000", true, "count", false},
        {"", "steps", "Number of simulation steps to run", "10000", true, "count", false},
        {"a", "autorestart", "Automatically restart simulation after completion", "", false, "", false},
        {"", "auto-restart", "Automatically restart simulation after completion", "", false, "", false},
        {"d", "debug", "Enable debug logging", "", false, "", false},
        {"", "debug", "Enable debug logging", "", false, "", false},
        {"v", "verbose", "Enable verbose output", "", false, "", false},
        {"", "verbose", "Enable verbose output", "", false, "", false},
        {"l", "load", "Load brain state from checkpoint", "", true, "path", false},
        {"", "load", "Load brain state from checkpoint", "", true, "path", false},
        {"save", "Save brain state to checkpoint", "", true, "path", false},
        {"save", "Save brain state to checkpoint", "", true, "path", false},
        {"w", "world", "Enable world simulation", "", false, "", false},
        {"", "world", "Enable world simulation", "", false, "", false},
        {"m", "memory", "Enable memory systems", "", false, "", false},
        {"", "memory", "Enable memory systems", "", false, "", false},
        {"cog", "cognition", "Enable cognition systems", "", false, "", false},
        {", "cognition", "Enable cognition systems", "", false, "", false},
        {"p", "prediction", "Enable prediction system", "", false, "", false},
        {"", "prediction", "Enable prediction system", "", false, "", false},
        {"neu", "neuromodulation", "Enable neuromodulation systems", "", false, "", false},
        {"", "neuromodulation", "Enable neuromodulation systems", "", false, "", false},
        {"dev", "development", "Enable development system", "", false, "", false},
        {"", "development", "Enable development system", "", false, "", false},
        {"viz", "visualize", "Start visualization system", "", false, "", false},
        {"", "visualize", "Start visualization system", "", false, "", false},
        {"con", "console", "Start interactive debug console", "", false, "", false},
        {"", "console", "Start interactive debug console", "", false, "", false},
        {"scr", "script", "Execute Lua/Python script", "", true, "script_path", false},
        {"", "script", "Execute Lua/Python script", "", true, "script_path", false},
        {"q", "quit", "Quit the application", "", false, "", false},
        {"", "quit", "Quit the application", "", false, "", false},
        {"x", "exit", "Exit the application", "", false, "", false},
        {"", "exit", "Exit the application", "", false, "", false},
        {"step", "Step execution", "Execute single step", "", true, "steps", false},
        {"run", "Run simulation", "Start full simulation run", "", true, "config", false},
        {"pause", "Pause simulation", "Pause current simulation", "", false, "", false},
        {"resume", "Resume simulation", "Resume paused simulation", "", false, "", false},
        {"status", "Show status", "Display current brain and simulation status", "", false, "", false},
        {"config", "Show configuration", "Display current configuration", "", false, "", false},
    };
}

void CommandLineParser::initializeCommands() {
    validCommands = {"run", "step", "pause", "resume", "save", "load", "config", "status", 
                     "debug", "visualize", "script", "help", "version", "quit", "exit"};
    
    commandMap = {
        {"run", CommandType::RUN},
        {"step", CommandType::STEP},
        {"pause", CommandType::PAUSE},
        {"resume", CommandType::RESUME},
        {"save", CommandType::SAVE},
        {"load", CommandType::LOAD},
        {"config", CommandType::CONFIG},
        {"status", CommandType::STATUS},
        {"debug", CommandType::DEBUG},
        {"visualize", CommandType::VISUALIZE},
        {"script", CommandType::SCRIPT},
        {"help", CommandType::HELP},
        {"version", CommandType::VERSION},
        {"quit", CommandType::QUIT},
        {"exit", CommandType::EXIT},
    };
}

ParsedArguments CommandLineParser::parse(int argc, char** argv) {
    return parseArguments(argc, argv);
}

ParsedArguments CommandLineParser::parseArguments(int argc, char** argv) {
    ParsedArguments args;
    args.isValid = true;
    
    // Check for help or version first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h" || arg == "/h") {
            args.helpRequested = true;
            args.isValid = true;
            return args;
        }
        if (arg == "--version" || arg == "-v") {
            args.versionRequested = true;
            args.isValid = true;
            return args;
        }
    }
    
    // Parse positional arguments
    std::vector<std::string> positionalArgs;
    CommandType currentCommand = CommandType::RUN; // Default command
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Check for flags with arguments (--flag value or --flag=value)
        if (stringStartsWith(arg, "--")) {
            std::string flag = arg.substr(2);
            std::string flagName = extractOptionName(flag);
            std::string flagValue;
            
            if (flag.find('=') != std::string::npos) {
                // --flag=value format
                size_t pos = flag.find('=');
                flagName = flag.substr(0, pos);
                flagValue = flag.substr(pos + 1);
                args.options[flagName] = flagValue;
            } else {
                // --flag value format
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    flagValue = argv[++i];
                    args.options[flagName] = flagValue;
                } else {
                    args.errorMessage = "Option --" + flagName + " requires a value";
                    args.isValid = false;
                }
            }
        } 
        else if (stringStartsWith(arg, "-")) {
            std::string flag = arg.substr(1);
            std::string flagName = extractOptionName(flag);
            std::string flagValue;
            
            if (flag.find('=') != std::string::npos) {
                // -flag=value format
                size_t pos = flag.find('=');
                flagName = flag.substr(0, pos);
                flagValue = flag.substr(pos + 1);
                args.options[flagName] = flagValue;
            } else {
                // Check if this is a short option with argument (-f value)
                auto it = options.find(flag);
                if (it != options.end() && it->second.hasArgument) {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        flagValue = argv[++i];
                        args.options[flagName] = flagValue;
                    } else {
                        args.errorMessage = "Option -" + flagName + " requires a value";
                        args.isValid = false;
                    }
                } else {
                    // Simple flag
                    args.options[flagName] = "true";
                }
            }
        } 
        else {
            // Positional argument
            positionalArgs.push_back(arg);
            
            // Check if it's a command
            std::string cmd = normalizeString(arg);
            if (isCommandValid(cmd)) {
                currentCommand = commandMap[cmd];
                args.commandArg = arg; // Keep original case for display
            }
        }
    }
    
    args.positionalArgs = positionalArgs;
    args.command = currentCommand;
    
    // Set defaults for required options
    if (args.options.find("neuron_count") == args.options.end()) {
        args.options["neuron_count"] = "1000";
    }
    if (args.options.find("region_count") == args.options.end()) {
        args.options["region_count"] = "1";
    }
    if (args.options.find("connection_probability") == args.options.end()) {
        args.options["connection_probability"] = "0.1";
    }
    if (args.options.find("timestep") == args.options.end()) {
        args.options["timestep"] = "0.001";
    }
    if (args.options.find("steps") == args.options.end()) {
        args.options["steps"] = "10000";
    }
    
    currentArgs = args;
    return args;
}

CommandType CommandLineParser::parseCommand(const std::string& cmd) const {
    std::string normalized = normalizeString(cmd);
    auto it = commandMap.find(normalized);
    if (it != commandMap.end()) {
        return it->second;
    }
    return CommandType::HELP; // Default to help for unknown commands
}

std::string CommandLineParser::normalizeString(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool CommandLineParser::stringStartsWith(const std::string& str, const std::string& prefix) const {
    if (str.length() < prefix.length()) return false;
    return str.compare(0, prefix.length(), prefix) == 0;
}

std::string CommandLineParser::extractOptionName(const std::string& arg) const {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(0, pos);
    }
    return arg;
}

std::string CommandLineParser::extractOptionValue(const std::string& arg) const {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

bool CommandLineParser::isCommandValid(const std::string& cmd) const {
    return commandMap.find(normalizeString(cmd)) != commandMap.end();
}

bool CommandLineParser::isOptionValid(const std::string& option) const {
    return options.find(option) != options.end();
}

bool CommandLineParser::isFlagValid(const std::string& flag) const {
    return options.find(flag) != options.end();
}

void CommandLineParser::printHelp() const {
    std::cout << "=== NLM Neural Learning Machine Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS] [COMMAND] [ARGS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "AVAILABLE COMMANDS:" << std::endl;
    for (const auto& cmd : validCommands) {
        std::cout << "  " << cmd << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "COMMON OPTIONS:" << std::endl;
    for (const auto& pair : options) {
        if (!pair.second.flag.empty() || !pair.second.longFlag.empty()) {
            std::cout << "  ";
            if (!pair.second.flag.empty()) {
                std::cout << "-" << pair.second.flag << ", ";
            }
            if (!pair.second.longFlag.empty()) {
                std::cout << "--" << pair.second.longFlag;
            }
            if (pair.second.hasArgument) {
                std::cout << " <" << pair.second.argumentName << ">";
            }
            std::cout << "  " << pair.second.description << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm run --neurons 2000 --timestep 0.01" << std::endl;
    std::cout << "  nlm step --steps 100" << std::endl;
    std::cout << "  nlm pause" << std::endl;
    std::cout << "  nlm resume" << std::endl;
    std::cout << "  nlm save --path brain_state.bin" << std::endl;
    std::cout << "  nlm load --path brain_state.bin" << std::endl;
    std::cout << "  nlm debug" << std::endl;
    std::cout << "  nlm visualize" << std::endl;
    std::cout << "  nlm status" << std::endl;
    std::cout << "  nlm config" << std::endl;
    std::cout << "  nlm --help" << std::endl;
    std::cout << std::endl;
    
    printAvailableCommands();
    printAvailableOptions();
}

void CommandLineParser::printVersion() const {
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 Final Integration)" << std::endl;
    std::cout << "Build: 2026-10-31" << std::endl;
    std::cout << "Description: Experimental artificial developmental brain" << std::endl;
    std::cout << std::endl;
    std::cout << "Architecture: Closed-loop neural system with memory, prediction, and cognition" << std::endl;
    std::cout << "Core Technologies: LIF neurons, STDP/Hebbian plasticity, structural plasticity" << std::endl;
    std::cout << "Integration: All Phase 1-6 systems connected into coherent brain loop" << std::endl;
    std::cout << std::endl;
}

void CommandLineParser::printOptionDetails(const CommandLineOption& option) const {
    std::cout << "  Option: ";
    if (!option.flag.empty()) std::cout << "-" << option.flag << ", ";
    if (!option.longFlag.empty()) std::cout << "--" << option.longFlag;
    std::cout << std::endl;
    std::cout << "  Description: " << option.description << std::endl;
    if (option.hasArgument) {
        std::cout << "  Argument: <" << option.argumentName << ">" << std::endl;
    }
    if (!option.defaultValue.empty()) {
        std::cout << "  Default: " << option.defaultValue << std::endl;
    }
    std::cout << std::endl;
}

void CommandLineParser::printCommandUsage(const std::string& cmd) const {
    std::cout << "Command: " << cmd << std::endl;
    // Command-specific usage would go here
    std::cout << "  Usage: nlm " << cmd << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
}

void CommandLineParser::printAvailableCommands() const {
    std::cout << "AVAILABLE COMMANDS:" << std::endl;
    for (const auto& cmd : validCommands) {
        std::cout << "  " << cmd << " - ";
        if (cmd == "run") std::cout << "Run full simulation";
        else if (cmd == "step") std::cout << "Execute single step";
        else if (cmd == "pause") std::cout << "Pause simulation";
        else if (cmd == "resume") std::cout << "Resume paused simulation";
        else if (cmd == "save") std::cout << "Save brain state to checkpoint";
        else if (cmd == "load") std::cout << "Load brain state from checkpoint";
        else if (cmd == "config") std::cout << "Show current configuration";
        else if (cmd == "status") std::cout << "Display brain and simulation status";
        else if (cmd == "debug") std::cout << "Start interactive debug console";
        else if (cmd == "visualize") std::cout << "Start visualization system";
        else if (cmd == "script") std::cout << "Execute script";
        else if (cmd == "help") std::cout << "Show help information";
        else if (cmd == "version") std::cout << "Show version information";
        else if (cmd == "quit" || cmd == "exit") std::cout << "Exit application";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void CommandLineParser::printAvailableOptions() const {
    std::cout << "AVAILABLE OPTIONS:" << std::endl;
    for (const auto& pair : options) {
        const auto& option = pair.second;
        if (!option.description.empty()) {
            std::cout << "  ";
            if (!option.flag.empty()) std::cout << "-" << option.flag << ", ";
            if (!option.longFlag.empty()) std::cout << "--" << option.longFlag;
            std::cout << "  " << option.description << std::endl;
        }
    }
    std::cout << std::endl;
}

} // namespace nlm