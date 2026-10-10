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
        StatusReporter::info("Available commands:");
        StatusReporter::info("");
        
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
            StatusReporter::info("");
        }
        
        StatusReporter::info("For detailed help on a specific command, use:")
        StatusReporter::info("  nlm <command> --help")
        StatusReporter::info("");
        StatusReporter::info("Global options:")
        StatusReporter::info("  --help, -h          Show this help message")
        StatusReporter::info("  --version           Show version information")
        StatusReporter::info("  --verbose, -v       Enable verbose output")
        StatusReporter::info("  --quiet, -q         Suppress non-essential output")
        StatusReporter::info("  --config FILE        Configuration file path")
        StatusReporter::info("  --output FILE        Output file path")
        StatusReporter::info("  --output-format FMT  Output format (text, json, csv)")
        StatusReporter::info("  --profile            Enable profiling")
        StatusReporter::info("  --benchmark          Enable benchmarking")
        StatusReporter::info("  --threads N          Number of threads to use")
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

// OptionParser implementation

void OptionParser::addOption(const Option& opt) {
    options.push_back(opt);
}

void OptionParser::addGlobalOptions() {
    // Add global options to parser
    addOption(Option("--verbose", 'v', OptionType::FLAG, "Enable verbose output", "", false));
    addOption(Option("--quiet", 'q', OptionType::FLAG, "Suppress non-essential output", "", false));
    addOption(Option("--log-level", 'l', OptionType::STRING, "Log level (debug, info, warning, error)", "info", false));
    addOption(Option("--config", 'c', OptionType::STRING, "Configuration file path", "configs/default.cfg", false));
    addOption(Option("--output", 'o', OptionType::STRING, "Output file path", "", false));
    addOption(Option("--output-format", '\0', OptionType::STRING, "Output format (text, json, csv)", "text", false));
    addOption(Option("--profile", '\0', OptionType::FLAG, "Enable profiling", "", false));
    addOption(Option("--benchmark", '\0', OptionType::FLAG, "Enable benchmarking", "", false));
    addOption(Option("--threads", '\0', OptionType::INTEGER, "Number of threads to use", "1", false));
    addOption(Option("--help", 'h', OptionType::FLAG, "Show help message", "", false));
    addOption(Option("--version", '\0', OptionType::FLAG, "Show version information", "", false));
}

bool OptionParser::parse(const std::vector<std::string>& args) {
    parsedOptions.clear();
    positionalArgs.clear();
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        // Check if it's an option
        bool isOption = false;
        OptionType matchedType = OptionType::FLAG;
        std::string matchedName;
        
        for (const auto& opt : options) {
            if (arg == "--" + opt.name || 
                (opt.shortName != '\0' && arg == "-" + std::string(1, opt.shortName))) {
                isOption = true;
                matchedType = opt.type;
                matchedName = opt.name;
                break;
            }
        }
        
        if (isOption) {
            // Handle flag options
            if (matchedType == OptionType::FLAG) {
                parsedOptions[matchedName] = "true";
            } else {
                // Handle options with values
                if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                    parsedOptions[matchedName] = args[++i];
                } else {
                    StatusReporter::error("Option " + matchedName + " requires a value");
                    return false;
                }
            }
        } else {
            // Positional argument
            positionalArgs.push_back(arg);
        }
    }
    
    return validateRequiredOptions();
}

bool OptionParser::parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

std::string OptionParser::getOption(const std::string& name) const {
    auto it = parsedOptions.find(name);
    if (it != parsedOptions.end()) {
        return it->second;
    }
    return "";
}

bool OptionParser::hasOption(const std::string& name) const {
    return parsedOptions.find(name) != parsedOptions.end();
}

bool OptionParser::validateRequiredOptions() const {
    for (const auto& opt : options) {
        if (opt.required && !hasOption(opt.name)) {
            StatusReporter::error("Missing required option: " + opt.name);
            return false;
        }
    }
    return true;
}

std::vector<std::string> OptionParser::getMissingRequiredOptions() const {
    std::vector<std::string> missing;
    for (const auto& opt : options) {
        if (opt.required && !hasOption(opt.name)) {
            missing.push_back(opt.name);
        }
    }
    return missing;
}

// HelpSystem implementation

void HelpSystem::registerCommand(const std::string& name, const std::string& description,
                               const std::string& usage,
                               const std::vector<std::string>& examples) {
    commandDescriptions[name] = description;
    commandUsage[name] = usage;
    commandExamples[name] = examples;
}

void HelpSystem::showHelp(const std::string& commandName) const {
    if (commandName.empty()) {
        StatusReporter::info("NLM - Neural Learning Machine CLI")
        StatusReporter::info("=" * 60)
        StatusReporter::info("Available commands:")
        StatusReporter::info("")
        
        for (const auto& [name, desc] : commandDescriptions) {
            StatusReporter::info("  " + name + " - " + desc)
        }
        
        StatusReporter::info("")
        StatusReporter::info("For detailed help, use 'nlm <command> --help'")
    } else {
        showCommandHelp(commandName)
    }
}

void HelpSystem::showCommandHelp(const std::string& commandName) const {
    auto descIt = commandDescriptions.find(commandName)
    if (descIt == commandDescriptions.end()) {
        StatusReporter::error("Unknown command: " + commandName)
        return
    }
    
    StatusReporter::info("Command: " + commandName)
    StatusReporter::info("Description: " + descIt->second)
    
    auto usageIt = commandUsage.find(commandName)
    if (usageIt != commandUsage.end() && !usageIt->second.empty()) {
        StatusReporter::info("Usage: " + usageIt->second)
    }
    
    auto examplesIt = commandExamples.find(commandName)
    if (examplesIt != commandExamples.end() && !examplesIt->second.empty()) {
        StatusReporter::info("")
        StatusReporter::info("Examples:")
        for (const auto& example : examplesIt->second) {
            StatusReporter::info("  " + example)
        }
    }
}

void HelpSystem::showAllCommands() const {
    for (const auto& [name, desc] : commandDescriptions) {
        StatusReporter::info("  " + name + " - " + desc)
    }
}

void HelpSystem::showCategoryHelp(CommandCategory category) const {
    StatusReporter::info("Commands in category " + CommandCategoryToString(category) + ":")
    for (const auto& [name, desc] : commandDescriptions) {
        // Note: This would need category information from commands
        // For now, just show all commands
        StatusReporter::info("  " + name + " - " + desc)
    }
}

void HelpSystem::startInteractiveHelp() const {
    StatusReporter::info("Interactive help mode")
    StatusReporter::info("Type 'help <command>' for command-specific help")
    StatusReporter::info("Type 'list' to see all commands")
    StatusReporter::info("Type 'exit' to return to command mode")
}

void HelpSystem::addExample(const std::string& commandName, const std::string& example) {
    commandExamples[commandName].push_back(example)
}

// ConfigManager implementation

ConfigManager::ConfigManager(const std::string& file) : configFile(file) {
    loadFromFile(file)
}

bool ConfigManager::loadFromFile(const std::string& file) {
    configFile = file
    configValues.clear()
    
    std::ifstream inputFile(file)
    if (!inputFile.is_open()) {
        StatusReporter::warning("Could not open config file: " + file)
        return false
    }
    
    std::string line
    while (std::getline(inputFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue
        
        size_t pos = line.find('=')
        if (pos == std::string::npos) continue
        
        std::string key = line.substr(0, pos)
        std::string value = line.substr(pos + 1)
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"))
        key.erase(key.find_last_not_of(" \t\r\n") + 1)
        value.erase(0, value.find_first_not_of(" \t\r\n"))
        value.erase(value.find_last_not_of(" \t\r\n") + 1)
        
        // Try to determine type
        if (value == "true" || value == "false") {
            configValues[key] = (value == "true")
        } else if (value.find('.') != std::string::npos) {
            try {
                configValues[key] = std::stod(value)
            } catch (...) {
                configValues[key] = value
            }
        } else {
            try {
                configValues[key] = std::stoll(value)
            } catch (...) {
                configValues[key] = value
            }
        }
    }
    
    inputFile.close()
    StatusReporter::info("Loaded configuration from: " + file)
    return true
}

bool ConfigManager::saveToFile(const std::string& file) const {
    std::ofstream outputFile(file)
    if (!outputFile.is_open()) {
        StatusReporter::error("Could not open file for writing: " + file)
        return false
    }
    
    for (const auto& [key, value] : configValues) {
        outputFile << key << " = "
        std::visit([&outputFile](auto&& arg) {
            using T = std::decay_t<decltype(arg)>
            if constexpr (std::is_same_v<T, std::string>) {
                outputFile << arg
            } else if constexpr (std::is_same_v<T, bool>) {
                outputFile << (arg ? "true" : "false")
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, long> || 
                                 std::is_same_v<T, long long>) {
                outputFile << arg
            } else if constexpr (std::is_same_v<T, double>) {
                outputFile << std::setprecision(6) << arg
            }
        }, value)
        outputFile << "\n"
    }
    
    outputFile.close()
    return true
}

template<typename T>
T ConfigManager::getValue(const std::string& key, const T& defaultValue) const {
    auto it = configValues.find(key)
    if (it == configValues.end()) {
        return defaultValue
    }
    
    const auto& value = it->second
    try {
        return std::get<T>(value)
    } catch (const std::bad_variant_access&) {
        // Try to convert string to target type
        std::string strValue = std::get<std::string>(value)
        if constexpr (std::is_same_v<T, int64_t>) {
            return std::stoll(strValue)
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(strValue)
        } else if constexpr (std::is_same_v<T, bool>) {
            return strValue == "true" || strValue == "1" || strValue == "yes"
        } else {
            return defaultValue
        }
    }
}

void ConfigManager::setValue(const std::string& key, 
                           const std::variant<std::string, int64_t, double, bool>& value) {
    configValues[key] = value
}

void ConfigManager::mergeWithCommandLine(const std::vector<std::string>& args) {
    // Simple merging: command line arguments override config file values
    // This can be enhanced with proper precedence handling
    OptionParser parser
    parser.addGlobalOptions()
    
    if (parser.parse(args)) {
        // Merge parsed options into config
        if (parser.hasOption("--config")) {
            setValue("config_file", parser.getOption("--config"))
        }
        if (parser.hasOption("--verbose")) {
            setValue("verbose_mode", true)
        }
        if (parser.hasOption("--quiet")) {
            setValue("quiet_mode", true)
        }
    }
}

void ConfigManager::loadFromEnvironment() {
    // Load configuration from environment variables
    // Variables should follow NLM_ prefix convention
    for (const auto& env : std::vector<std::string>{
        "NLM_LOG_LEVEL", "NLM_VERBOSE", "NLM_QUIET", 
        "NLM_CONFIG_FILE", "NLM_OUTPUT_FORMAT", "NLM_THREADS"
    }) {
        std::string value = getEnvVar(env)
        if (!value.empty()) {
            // Convert environment variable name to config key
            std::string key = env.substr(4) // Remove NLM_ prefix
            std::transform(key.begin(), key.end(), key.begin(), ::tolower)
            setValue(key, value)
        }
    }
}

std::string ConfigManager::getEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str())
    return value ? value : ""
}

// ProgressIndicator implementation

void ProgressIndicator::start(const std::string& msg, int total, bool indeterminate) {
    if (!enabled) return
    
    this->message = msg
    this->totalSteps = total
    this->currentStep = 0
    this->indeterminate = indeterminate
    
    // Simple progress bar for now
    if (indeterminate) {
        StatusReporter::progress("[?] " + message)
    } else {
        StatusReporter::progress("[0%]")
    }
}

void ProgressIndicator::update(int step) {
    if (!enabled) return
    
    currentStep = step
    if (totalSteps > 0) {
        int percentage = static_cast<int>(100.0 * currentStep / totalSteps)
        StatusReporter::progress("[" + std::to_string(percentage) + "%]")
    }
}

void ProgressIndicator::update() {
    update(++currentStep)
}

void ProgressIndicator::complete() {
    if (!enabled) return
    
    if (indeterminate) {
        StatusReporter::progress("[?] Complete")
    } else {
        StatusReporter::progress("[100%]")
    }
    StatusReporter::info("")
}

// StatusReporter implementation

GlobalOptions StatusReporter::globalOpts
bool StatusReporter::coloredOutput = true

void StatusReporter::info(const std::string& message) {
    if (globalOpts.quiet) return
    
    std::cout << "[INFO] " << message << "\n"
}

void StatusReporter::warning(const std::string& message) {
    if (globalOpts.quiet) return
    
    std::cerr << "[WARNING] " << message << "\n"
}

void StatusReporter::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << "\n"
}

void StatusReporter::success(const std::string& message) {
    if (globalOpts.quiet) return
    
    std::cout << "[SUCCESS] " << message << "\n"
}

void StatusReporter::progress(const std::string& message) {
    if (globalOpts.quiet) return
    
    std::cout << "\r" << message << std::flush
}
}

// Environment implementation

std::string Environment::getVar(const std::string& name, const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str())
    return value ? value : defaultValue
}

int Environment::getVar(const std::string& name, int defaultValue) {
    const char* value = std::getenv(name.c_str())
    if (!value) return defaultValue
    
    try {
        return std::stoi(value)
    } catch (...) {
        return defaultValue
    }
}


double Environment::getVar(const std::string& name, double defaultValue) {
    const char* value = std::getenv(name.c_str())
    if (!value) return defaultValue
    
    try {
        return std::stod(value)
    } catch (...) {
        return defaultValue
    }
}


bool Environment::getVar(const std::string& name, bool defaultValue) {
    const char* value = std::getenv(name.c_str())
    if (!value) return defaultValue
    
    std::string val = value
    std::transform(val.begin(), val.end(), val.begin(), ::tolower)
    return val == "true" || val == "1" || val == "yes"
}


std::vector<std::string> Environment::getVarList(const std::string& name, char separator) {
    std::vector<std::string> result
    const char* value = std::getenv(name.c_str())
    if (!value) return result
    
    std::string str(value)
    std::stringstream ss(str)
    std::string item
    while (std::getline(ss, item, separator)) {
        if (!item.empty()) {
            result.push_back(item)
        }
    }
    
    return result
}
}


bool Environment::setVar(const std::string& name, const std::string& value) {
    // Note: This is environment-specific and may not work on all platforms
    // For portability, this is mainly for demonstration
    return std::setenv(name.c_str(), value.c_str(), 1) == 0
}


bool Environment::isSet(const std::string& name) {
    return std::getenv(name.c_str()) != nullptr
}

} // namespace cli
} // namespace nlm
