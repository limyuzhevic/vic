#include "CLIFramework.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <future>
#include <csignal>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// ArgumentParser implementation
class ArgumentParser::Impl {
private:
    std::vector<Command> commands;
    std::map<std::string, Command> commandMap;
    
    // Tab completion and suggestion logic
    std::vector<std::string> getSubstrings(const std::string& str, size_t minLen = 2) {
        std::vector<std::string> substrings;
        for (size_t i = 0; i < str.length() - minLen + 1; ++i) {
            substrings.push_back(str.substr(i, minLen));
        }
        return substrings;
    }
    
    std::vector<std::string> findMatches(const std::vector<std::string>& candidates,
                                         const std::string& partial) {
        std::vector<std::string> matches;
        for (const auto& candidate : candidates) {
            if (candidate.find(partial) != std::string::npos) {
                matches.push_back(candidate);
            }
        }
        return matches;
    }
    
public:
    void addCommand(const Command& cmd) {
        commands.push_back(cmd);
        commandMap[cmd.name] = cmd;
        for (const auto& alias : cmd.aliases) {
            commandMap[alias] = cmd;
        }
    }
    
    const Command* getCommand(const std::string& name) const {
        auto it = commandMap.find(name);
        return it != commandMap.end() ? &it->second : nullptr;
    }
    
    std::vector<Command> getAllCommands() const {
        return commands;
    }
    
    std::vector<std::string> getCommandSuggestions(const std::string& partial) const {
        std::vector<std::string> candidates;
        for (const auto& cmd : commands) {
            candidates.push_back(cmd.name);
            for (const auto& alias : cmd.aliases) {
                candidates.push_back(alias);
            }
        }
        
        std::vector<std::string> matches = findMatches(candidates, partial);
        std::sort(matches.begin(), matches.end());
        matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
        return matches;
    }
    
    std::vector<std::string> getArgumentSuggestions(const std::string& partial) const {
        std::vector<std::string> candidates;
        
        // For now, return empty - could be enhanced with command-specific suggestions
        return candidates;
    }
    
    bool validateArguments(const std::string& command, 
                         const std::map<std::string, std::string>& args) const {
        const Command* cmd = getCommand(command);
        if (!cmd) return false;
        
        for (const auto& arg : cmd->arguments) {
            if (arg.required) {
                std::string argName = "--" + arg.name;
                if (arg.shortName != "") {
                    argName = "-" + arg.shortName;
                }
                if (args.find(argName) == args.end()) {
                    std::cerr << "Error: Required argument '--" << arg.name << "' is missing\n";
                    return false;
                }
            }
        }
        return true;
    }
    
    void printCommandHelp(const std::string& commandName) const {
        const Command* cmd = getCommand(commandName);
        if (!cmd) {
            std::cout << "Error: Command not found: '" << commandName << "'\n";
            return;
        }
        
        std::cout << "Command: " << cmd->name << "\n";
        std::cout << "Description: " << cmd->description << "\n";
        std::cout << "Category: " << cmd->category << "\n";
        std::cout << "\nArguments:\n";
        
        if (cmd->arguments.empty()) {
            std::cout << "  (none)" << std::endl;
        } else {
            for (const auto& arg : cmd->arguments) {
                std::string argSpec;
                if (arg.shortName != "") {
                    argSpec += "-" + arg.shortName + ", ";
                }
                argSpec += "--" + arg.name;
                
                std::cout << "  " << argSpec;
                if (arg.required) std::cout << " [REQUIRED]";
                std::cout << "\n    " << arg.description;
                if (!arg.defaultValue.empty()) {
                    std::cout << " (default: " << arg.defaultValue << ")";
                }
                if (!arg.choices.empty()) {
                    std::cout << " (choices: ";
                    for (size_t i = 0; i < arg.choices.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << arg.choices[i];
                    }
                    std::cout << ")";
                }
                std::cout << "\n";
            }
        }
        
        std::cout << "\nAliases: ";
        if (cmd->aliases.empty()) {
            std::cout << "(none)";
        } else {
            for (size_t i = 0; i < cmd->aliases.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << cmd->aliases[i];
            }
        }
        std::cout << "\n";
    }
    
    void printHelp() const {
        std::cout << "NLM Command Line Interface\n";
        std::cout << "==========================\n\n";
        
        std::cout << "Available commands by category:\n\n";
        
        std::map<std::string, std::vector<Command>> commandsByCategory;
        for (const auto& cmd : commands) {
            commandsByCategory[cmd.category].push_back(cmd);
        }
        
        for (const auto& category : commandsByCategory) {
            std::cout << category.first << ":\n";
            for (const auto& cmd : category.second) {
                std::cout << "  " << cmd.name << " - " << cmd.description << "\n";
            }
            std::cout << "\n";
        }
        
        std::cout << "For more information about a specific command, use 'help <command>'.\n";
        std::cout << "Example: nlm help run\n";
    }
    
    void printCompletion(const std::string& partial) const {
        std::vector<std::string> suggestions;
        
        // Command suggestions
        auto cmdSuggestions = getCommandSuggestions(partial);
        suggestions.insert(suggestions.end(), cmdSuggestions.begin(), cmdSuggestions.end());
        
        if (!suggestions.empty()) {
            std::cout << "Possible completions:\n";
            for (const auto& suggestion : suggestions) {
                std::cout << "  " << suggestion << "\n";
            }
        }
    }
};

ArgumentParser::ArgumentParser() : pImpl(std::make_unique<Impl>()) {}
ArgumentParser::~ArgumentParser() = default;

bool ArgumentParser::parse(int argc, char** argv, std::string& command,
                           std::map<std::string, std::string>& args,
                           std::vector<std::string>& unknownArgs) {
    if (argc < 1) {
        return false;
    }
    
    args.clear();
    unknownArgs.clear();
    
    // First argument is either command or --help/--version
    std::string firstArg = argv[1];
    
    if (firstArg == "--help" || firstArg == "-h") {
        command = "help";
        return true;
    } else if (firstArg == "--version" || firstArg == "-v") {
        command = "version";
        return true;
    } else if (firstArg == "--complete") {
        command = "complete";
        // Next argument would be the partial command
        if (argc > 2) {
            args["partial"] = argv[2];
        }
        return true;
    }
    
    // Try to find command
    const Command* cmd = pImpl->getCommand(firstArg);
    if (!cmd) {
        // Command not found
        unknownArgs.push_back(firstArg);
        return false;
    }
    
    command = firstArg;
    
    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Long format: --key=value or --key value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            } else {
                args[arg.substr(2)] = "true"; // boolean flag
            }
        } else if (arg[0] == '-' && arg.length() > 1) {
            // Short format: -k value or -k
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                // -k value
                std::string key = arg.substr(1);
                args[key] = argv[++i];
            } else {
                // -k (boolean flag)
                std::string key = arg.substr(1);
                args[key] = "true";
            }
        } else {
            // Unknown argument (treated as positional)
            unknownArgs.push_back(arg);
        }
    }
    
    return true;
}

void ArgumentParser::addCommand(const Command& cmd) {
    pImpl->addCommand(cmd);
}

const Command* ArgumentParser::getCommand(const std::string& name) const {
    return pImpl->getCommand(name);
}

std::vector<Command> ArgumentParser::getAllCommands() const {
    return pImpl->getAllCommands();
}

std::vector<std::string> ArgumentParser::getCommandSuggestions(const std::string& partial) const {
    return pImpl->getCommandSuggestions(partial);
}

std::vector<std::string> ArgumentParser::getArgumentSuggestions(const std::string& partial) const {
    return pImpl->getArgumentSuggestions(partial);
}

bool ArgumentParser::validateArguments(const std::string& command, 
                                      const std::map<std::string, std::string>& args) const {
    return pImpl->validateArguments(command, args);
}

template<typename T>
T ArgumentParser::convertValue(const std::string& value) {
    // Implementation would be specialized for different types
    // This is a placeholder for type conversion logic
    throw std::runtime_error("Type conversion not implemented for template type T");
}

void ArgumentParser::printCommandHelp(const std::string& command) const {
    pImpl->printCommandHelp(command);
}

void ArgumentParser::printHelp() const {
    pImpl->printHelp();
}

void ArgumentParser::printCompletion(const std::string& partial) const {
    pImpl->printCompletion(partial);
}

// CommandExecutor implementation
class CommandExecutor::Impl {
private:
    std::vector<std::string> commandHistory;
    std::string workingDirectory;
    std::map<std::string, Command> registeredCommands;
    
public:
    void registerCommand(const Command& cmd) {
        registeredCommands[cmd.name] = cmd;
    }
    
    const Command* getCommand(const std::string& name) const {
        auto it = registeredCommands.find(name);
        return it != registeredCommands.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> getCommandHistory() const {
        return commandHistory;
    }
    
    void addToHistory(const std::string& command) {
        commandHistory.push_back(command);
        if (commandHistory.size() > 1000) {
            commandHistory.erase(commandHistory.begin());
        }
    }
    
    void setWorkingDirectory(const std::string& path) {
        workingDirectory = path;
        std::filesystem::current_path(path);
    }
    
    std::string getWorkingDirectory() const {
        return workingDirectory;
    }
};

CommandExecutor::CommandExecutor() : pImpl(std::make_unique<Impl>()) {
    pImpl->setWorkingDirectory(std::filesystem::current_path().string());
}

CommandExecutor::~CommandExecutor() = default;

CommandResult CommandExecutor::execute(const std::string& command, 
                                     const std::map<std::string, std::string>& args,
                                     const std::shared_ptr<Config>& config) {
    CommandResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    const Command* cmd = pImpl->getCommand(command);
    if (!cmd) {
        result.success = false;
        result.error = "Command not found: '" + command + "'";
        return result;
    }
    
    // Add to history
    std::string fullCommand = command;
    for (const auto& arg : args) {
        fullCommand += " --" + arg.first + "=" + arg.second;
    }
    pImpl->addToHistory(fullCommand);
    
    // TODO: Execute command with arguments
    // This would call cmd->handler(args) if handler was registered
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

void CommandExecutor::registerCommand(const Command& cmd) {
    pImpl->registerCommand(cmd);
}

CommandResult CommandExecutor::executeFromString(const std::string& commandLine,
                                                const std::shared_ptr<Config>& config) {
    // Simple parsing for command line string
    std::istringstream iss(commandLine);
    std::string token;
    iss >> token;
    
    std::map<std::string, std::string> args;
    std::string argName;
    bool inArg = false;
    std::string argValue;
    
    while (iss >> token) {
        if (token.substr(0, 2) == "--") {
            // New argument
            if (!argName.empty()) {
                args[argName] = argValue;
            }
            argName = token.substr(2);
            argValue.clear();
            inArg = false;
        } else if (token[0] == '-' && token.length() > 1) {
            // Short argument
            if (!argName.empty()) {
                args[argName] = argValue;
            }
            argName = token.substr(1);
            argValue.clear();
            inArg = false;
        } else {
            // Argument value
            if (!argName.empty()) {
                argValue = token;
                inArg = true;
            }
        }
    }
    
    if (!argName.empty()) {
        args[argName] = argValue;
    }
    
    return execute(token, args, config);
}

std::vector<std::string> CommandExecutor::getCommandHistory() const {
    return pImpl->getCommandHistory();
}

void CommandExecutor::clearCommandHistory() {
    pImpl->commandHistory.clear();
}

void CommandExecutor::setWorkingDirectory(const std::string& path) {
    pImpl->setWorkingDirectory(path);
}

std::string CommandExecutor::getWorkingDirectory() const {
    return pImpl->getWorkingDirectory();
}

// CLIContext implementation
class CLIContext::Impl {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    CommandExecutor commandExecutor;
    ArgumentParser argumentParser;
    
    std::map<std::string, std::chrono::system_clock::time_point> savedContexts;
    std::map<std::string, std::shared_ptr<Config>> savedConfigs;
    
public:
    void initialize(const std::shared_ptr<Config>& config) {
        this->config = config;
        this->logger = Logger::getGlobal();
        
        // Register built-in commands
        registerBuiltInCommands();
    }
    
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) {
        this->config = config;
    }
    
    std::shared_ptr<Logger> getLogger() const {
        return logger;
    }
    
    void setLogger(const std::shared_ptr<Logger>& logger) {
        this->logger = logger;
    }
    
    CommandExecutor& getCommandExecutor() {
        return commandExecutor;
    }
    
    ArgumentParser& getArgumentParser() {
        return argumentParser;
    }
    
    void saveContext(const std::string& name) {
        savedContexts[name] = std::chrono::system_clock::now();
        savedConfigs[name] = config;
    }
    
    bool restoreContext(const std::string& name) {
        auto it = savedConfigs.find(name);
        if (it == savedConfigs.end()) {
            return false;
        }
        
        config = it->second;
        return true;
    }
    
    std::vector<std::string> listContexts() const {
        std::vector<std::string> names;
        for (const auto& pair : savedContexts) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void cleanupContexts(double maxAgeSeconds) {
        auto now = std::chrono::system_clock::now();
        for (auto it = savedContexts.begin(); it != savedContexts.end(); ) {
            auto age = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
            if (age > maxAgeSeconds) {
                savedConfigs.erase(it->first);
                it = savedContexts.erase(it);
            } else {
                ++it;
            }
        }
    }
    
private:
    void registerBuiltInCommands() {
        // Help command
        Command helpCmd("help", "Show help information", "General", CommandType::Help);
        helpCmd.aliases = {"h", "?"};
        commandExecutor.registerCommand(helpCmd);
        
        // Version command
        Command versionCmd("version", "Show version information", "General", CommandType::Version);
        versionCmd.aliases = {"v"};
        commandExecutor.registerCommand(versionCmd);
        
        // List command
        Command listCmd("list", "List available commands or items", "General", CommandType::List);
        listCmd.aliases = {"ls", "available"};
        commandExecutor.registerCommand(listCmd);
        
        // Run command
        Command runCmd("run", "Run simulation or experiment", "Execution", CommandType::Run);
        runCmd.arguments = {
            CommandArg("experiment", "Experiment name or file", "string", true),
            CommandArg("steps", "Number of simulation steps", "int", false),
            CommandArg("config", "Configuration file path", "file", false)
        };
        commandExecutor.registerCommand(runCmd);
        
        // Debug command
        Command debugCmd("debug", "Enable debug mode", "Debug", CommandType::Debug);
        debugCmd.arguments = {
            CommandArg("module", "Module to debug", "string", false),
            CommandArg("level", "Debug level (0-3)", "int", false)
        };
        commandExecutor.registerCommand(debugCmd);
        
        // Benchmark command
        Command benchmarkCmd("benchmark", "Run performance benchmarks", "Performance", CommandType::Benchmark);
        benchmarkCmd.arguments = {
            CommandArg("type", "Benchmark type", "string", false),
            CommandArg("duration", "Benchmark duration in seconds", "float", false)
        };
        commandExecutor.registerCommand(benchmarkCmd);
        
        // Configure command
        Command configCmd("configure", "Load/save configuration", "Configuration", CommandType::Configure);
        configCmd.arguments = {
            CommandArg("action", "Action: load, save, set, get", "string", true),
            CommandArg("file", "Configuration file path", "file", false)
        };
        commandExecutor.registerCommand(configCmd);
    }
};

CLIContext::CLIContext() : pImpl(std::make_unique<Impl>()) {}
CLIContext::~CLIContext() = default;

void CLIContext::initialize(const std::shared_ptr<Config>& config) {
    pImpl->initialize(config);
}

std::shared_ptr<Config> CLIContext::getConfig() const {
    return pImpl->getConfig();
}

void CLIContext::setConfig(const std::shared_ptr<Config>& config) {
    pImpl->setConfig(config);
}

std::shared_ptr<Logger> CLIContext::getLogger() const {
    return pImpl->getLogger();
}

void CLIContext::setLogger(const std::shared_ptr<Logger>& logger) {
    pImpl->setLogger(logger);
}

CommandExecutor& CLIContext::getCommandExecutor() {
    return pImpl->getCommandExecutor();
}

ArgumentParser& CLIContext::getArgumentParser() {
    return pImpl->getArgumentParser();
}

void CLIContext::saveContext(const std::string& name) {
    pImpl->saveContext(name);
}

bool CLIContext::restoreContext(const std::string& name) {
    return pImpl->restoreContext(name);
}

std::vector<std::string> CLIContext::listContexts() const {
    return pImpl->listContexts();
}

void CLIContext::cleanupContexts(double maxAgeSeconds) {
    pImpl->cleanupContexts(maxAgeSeconds);
}

// CommandUtils implementation
namespace {
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
}

std::string CommandUtils::formatDuration(const std::chrono::milliseconds& duration) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = duration - std::chrono::duration_cast<std::chrono::milliseconds>(seconds);
    
    if (seconds.count() >= 60) {
        auto minutes = seconds / 60;
        seconds %= 60;
        auto hours = minutes / 60;
        minutes %= 60;
        
        std::ostringstream oss;
        oss << hours.count() << "h " << minutes.count() << "m " 
            << seconds.count() << "." << milliseconds.count() << "s";
        return oss.str();
    } else if (seconds.count() >= 1) {
        std::ostringstream oss;
        oss << seconds.count() << "." << milliseconds.count() << "s";
        return oss.str();
    } else {
        std::ostringstream oss;
        oss << milliseconds.count() << "ms";
        return oss.str();
    }
}

std::string CommandUtils::formatMemory(size_t bytes) {
    if (bytes >= 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
    } else if (bytes >= 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + "MB";
    } else if (bytes >= 1024) {
        return std::to_string(bytes / 1024) + "KB";
    } else {
        return std::to_string(bytes) + "B";
    }
}

size_t CommandUtils::parseFileSize(const std::string& spec) {
    size_t value = 0;
    size_t i = 0;
    
    while (i < spec.length() && std::isdigit(spec[i])) {
        value = value * 10 + (spec[i] - '0');
        i++;
    }
    
    if (i >= spec.length()) {
        return value;
    }
    
    char unit = std::tolower(spec[i]);
    i++;
    
    switch (unit) {
        case 'k': return value * 1024;
        case 'm': return value * 1024 * 1024;
        case 'g': return value * 1024 * 1024 * 1024;
        case 't': return value * 1024 * 1024 * 1024 * 1024;
        default: return value;
    }
}

bool CommandUtils::validateFile(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool CommandUtils::validateDirectory(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool CommandUtils::ensureDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            return std::filesystem::create_directories(path);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory " << path << ": " << e.what() << std::endl;
        return false;
    }
}

std::string CommandUtils::getTimestamp(bool includeMilliseconds) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    if (includeMilliseconds) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    }
    
    return oss.str();
}

CommandUtils::ProgressReporter::ProgressReporter(const std::string& description,
                                                 size_t totalSteps,
                                                 size_t updateInterval)
    : pImpl(std::make_unique<Impl>()) {
    // TODO: Implement progress reporter
}

CommandUtils::ProgressReporter::~ProgressReporter() = default;

void CommandUtils::ProgressReporter::update(size_t currentStep, const std::string& status) {
    // TODO: Implement progress update
}

void CommandUtils::ProgressReporter::setDetails(const std::string& details) {
    // TODO: Implement details setting
}

void CommandUtils::ProgressReporter::finish() {
    // TODO: Implement progress finish
}

bool CommandUtils::ProgressReporter::isActive() const {
    return pImpl != nullptr;
}

bool CommandUtils::loadConfigFromFile(const std::string& filepath,
                                     std::shared_ptr<Config>& config) {
    config = std::make_shared<Config>();
    return config->loadFromFile(filepath);
}

bool CommandUtils::saveConfigToFile(const std::string& filepath,
                                   const std::shared_ptr<Config>& config) {
    if (!config) return false;
    return config->saveToFile(filepath);
}

std::string CommandUtils::getDefaultConfigPath(const std::string& command) {
    std::string filename = "nlm_" + command + ".cfg";
    return std::filesystem::current_path().string() + "/" + filename;
}

bool CommandUtils::exportResults(const std::string& filepath,
                                const CommandResult& result,
                                const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        // TODO: Implement JSON export
        file << "{\"success\": " << (result.success ? "true" : "false") << "}";
    } else {
        file << "Result: " << result.output;
    }
    
    return true;
}

bool CommandUtils::exportResults(const std::string& filepath,
                                const std::vector<std::map<std::string, std::string>>& data,
                                const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "csv") {
        if (!data.empty()) {
            // Write header
            size_t col = 0;
            for (const auto& pair : data[0]) {
                if (col > 0) file << ",";
                file << pair.first;
                col++;
            }
            file << "\n";
            
            // Write data
            for (const auto& row : data) {
                col = 0;
                for (const auto& pair : row) {
                    if (col > 0) file << ",";
                    file << pair.second;
                    col++;
                }
                file << "\n";
            }
        }
    }
    
    return true;
}

void CommandUtils::saveCommandHistory(const std::vector<std::string>& history,
                                     const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& cmd : history) {
            file << cmd << "\n";
        }
    }
}

bool CommandUtils::loadCommandHistory(std::vector<std::string>& history,
                                      const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            history.push_back(line);
        }
    }
    
    return true;
}

std::string CommandUtils::formatError(const std::string& error,
                                      const std::string& command) {
    if (!command.empty()) {
        return "Error in command '" + command + "': " + error;
    }
    return "Error: " + error;
}

std::string CommandUtils::formatSuccess(const std::string& message,
                                       const std::chrono::milliseconds& duration) {
    std::ostringstream oss;
    oss << "Success: " << message;
    if (duration.count() > 0) {
        oss << " (took " << formatDuration(duration) << ")";
    }
    return oss.str();
}

std::string CommandUtils::getEnvironmentVariable(const std::string& name,
                                                 const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str());
    return value ? value : defaultValue;
}

bool CommandUtils::setEnvironmentVariable(const std::string& name,
                                         const std::string& value) {
#ifdef _WIN32
    return _putenv_s(name.c_str(), value.c_str()) == 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

std::string CommandUtils::getPlatformInfo() {
    std::ostringstream oss;
    oss << "Platform: " << (sizeof(void*) * 8) << "-bit";
    oss << "\nCompiler: " << NLM_COMPILER_INFO;
    oss << "\nStandard: C++" << NLM_CXX_STANDARD;
    return oss.str();
}

std::string CommandUtils::InteractiveUtils::readInput(const std::string& prompt,
                                                     int timeoutSeconds,
                                                     const std::vector<std::string>& suggestions) {
    std::cout << prompt;
    
    if (timeoutSeconds > 0) {
        // TODO: Implement timeout for input
    }
    
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int CommandUtils::InteractiveUtils::showMenu(const std::string& title,
                                           const std::vector<std::string>& options,
                                           int timeoutSeconds) {
    std::cout << title << "\n";
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << i + 1 << ". " << options[i] << "\n";
    }
    
    std::string input = readInput("> ");
    
    try {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= static_cast<int>(options.size())) {
            return choice;
        }
    } catch (...) {}
    
    return -1;
}

bool CommandUtils::InteractiveUtils::confirm(const std::string& question,
                                            bool defaultAnswer) {
    std::string input = readInput(question + " (y/n): ");
    
    if (input.empty()) {
        return defaultAnswer;
    }
    
    char response = std::tolower(input[0]);
    return response == 'y';
}

// Global CLI context instance
CLIContext globalCLIContext;

// Convenience functions
namespace CLI {
    void initialize(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        globalCLIContext.initialize(config);
    }
    
    CommandResult execute(const std::string& commandLine,
                         const std::shared_ptr<Config>& config) {
        if (!isInitialized()) {
            initialize();
        }
        
        CommandExecutor& executor = globalCLIContext.getCommandExecutor();
        return executor.executeFromString(commandLine, config);
    }
    
    CommandResult execute(const std::string& commandLine) {
        return execute(commandLine, globalCLIContext.getConfig());
    }
    
    void help(const std::string& command) {
        if (!isInitialized()) {
            initialize();
        }
        
        ArgumentParser& parser = globalCLIContext.getArgumentParser();
        if (command.empty()) {
            parser.printHelp();
        } else {
            parser.printCommandHelp(command);
        }
    }
    
    void version() {
        std::cout << "NLM Neural Learning Machine v0.1.0\n";
        std::cout << "Built with " << NLM_COMPILER_INFO << "\n";
        std::cout << "C++ standard: " << NLM_CXX_STANDARD << "\n";
    }
    
    bool isInitialized() {
        return globalCLIContext.getConfig() != nullptr;
    }
    
    void cleanup() {
        // TODO: Cleanup CLI resources
    }
}

} // namespace nlm

// Compiler and platform detection macros
#if defined(_MSC_VER)
    #define NLM_COMPILER_INFO "MSVC"
#elif defined(__GNUC__) && !defined(__clang__)
    #define NLM_COMPILER_INFO "GCC " __VERSION__"
#elif defined(__clang__)
    #define NLM_COMPILER_INFO "Clang " __clang_version__"
#else
    #define NLM_COMPILER_INFO "Unknown"
#endif

#define NLM_CXX_STANDARD "20"

#endif // NLM_CLI_FRAMEWORK_HPP