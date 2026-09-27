// CLI - Command Line Interface Utilities Implementation

#include "core/CLI/CLI.hpp"
#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace cli {

// CLIParser implementation
void CLIParser::addArgument(const Argument& arg) {
    arguments.push_back(arg);
}

bool CLIParser::parse(int argc, char* argv[]) {
    programName = argv[0];
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        } else if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            loadConfigFromFile(configFile);
        } else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t eqPos = key.find('=');
            if (eqPos != std::string::npos) {
                std::string valueKey = key.substr(0, eqPos);
                std::string value = key.substr(eqPos + 1);
                setConfigValue(valueKey, value);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                setConfigValue(key, argv[++i]);
            } else {
                setConfigValue(key, "true");
            }
        } else if (arg.substr(0, 1) == "-") {
            std::string shortOpt = arg.substr(1);
            for (char c : shortOpt) {
                for (const auto& argDef : arguments) {
                    if (argDef.shortOpt == c) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            setConfigValue(argDef.name, argv[++i]);
                        } else {
                            setConfigValue(argDef.name, "true");
                        }
                        break;
                    }
                }
            }
        }
    }
    
    return validateRequiredArgs();
}

std::string CLIParser::getValue(const std::string& name) const {
    auto it = parsedValues.find(name);
    return (it != parsedValues.end()) ? it->second : "";
}

bool CLIParser::hasValue(const std::string& name) const {
    return parsedValues.find(name) != parsedValues.end();
}

void CLIParser::printHelp() const {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << description << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& arg : arguments) {
        std::cout << "  ";
        if (arg.shortOpt) {
            std::cout << "-" << arg.shortOpt << ", ";
        }
        if (!arg.longOpt.empty()) {
            std::cout << "--" << arg.longOpt;
        }
        std::cout << std::endl;
        std::cout << "      " << arg.description << std::endl;
        if (!arg.defaultValue.empty()) {
            std::cout << "      Default: " << arg.defaultValue << std::endl;
        }
        if (arg.required) {
            std::cout << "      Required" << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "General Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --help-full             Show all available options" << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6)" << std::endl;
    std::cout << "Build: " << utils::getBuildInfo() << std::endl;
}

bool CLIParser::loadConfigFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = utils::trim(line);
        if (line.empty()) continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, eqPos));
            std::string value = utils::trim(line.substr(eqPos + 1));
            setConfigValue(key, value);
        }
    }
    
    file.close();
    return true;
}

void CLIParser::saveConfigToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot save config file: " << filename << std::endl;
        return;
    }
    
    for (const auto& pair : config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
}

void CLIParser::setConfigValue(const std::string& key, const std::string& value) {
    config[key] = value;
    
    // Find corresponding argument and update parsed values
    for (const auto& arg : arguments) {
        if (arg.name == key) {
            parsedValues[key] = value;
            if (arg.callback) {
                arg.callback(value);
            }
            break;
        }
    }
}

std::string CLIParser::getConfigValue(const std::string& key) const {
    auto it = config.find(key);
    return (it != config.end()) ? it->second : "";
}

void CLIParser::parseArgument(const std::string& arg) {
    // Implementation for argument parsing
}

bool CLIParser::validateRequiredArgs() const {
    for (const auto& arg : arguments) {
        if (arg.required && !hasValue(arg.name)) {
            std::cerr << "Error: Required argument --" << arg.name << " is missing" << std::endl;
            return false;
        }
    }
    return true;
}

// UnifiedLogger implementation
UnifiedLogger::UnifiedLogger() : currentLevel(Level::INFO), maxLogSize(10 * 1024 * 1024), maxLogFiles(5) {
    addConsoleSink();
}

UnifiedLogger::~UnifiedLogger() {
    // Cleanup sinks
}

UnifiedLogger& UnifiedLogger::getInstance() {
    static UnifiedLogger instance;
    return instance;
}

void UnifiedLogger::setLevel(Level level) {
    currentLevel = level;
}

void UnifiedLogger::log(Level level, const std::string& module, const std::string& message,
                        const std::string& file, int line,
                        const std::map<std::string, std::string>& metadata) {
    if (level < currentLevel) return;
    
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.module = module;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.metadata = metadata;
    
    logBuffer.push_back(entry);
    
    // Output to sinks
    for (void* sink : sinks) {
        // This would be implemented with proper sink classes
    }
}

void UnifiedLogger::fatal(const std::string& module, const std::string& message) {
    log(Level::FATAL, module, message);
}

void UnifiedLogger::error(const std::string& module, const std::string& message) {
    log(Level::ERROR, module, message);
}

void UnifiedLogger::warn(const std::string& module, const std::string& message) {
    log(Level::WARN, module, message);
}

void UnifiedLogger::info(const std::string& module, const std::string& message) {
    log(Level::INFO, module, message);
}

void UnifiedLogger::debug(const std::string& module, const std::string& message) {
    log(Level::DEBUG, module, message);
}

void UnifiedLogger::trace(const std::string& module, const std::string& message) {
    log(Level::TRACE, module, message);
}

std::string UnifiedLogger::formatLogEntry(const LogEntry& entry) const {
    std::stringstream ss;
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "."
       << std::setfill('0') << std::setw(3)
       << (entry.timestamp.time_since_epoch().count() % 1000)
       << " " << levelToString(entry.level) << " "
       << entry.module << ": " << entry.message;
    
    if (!entry.file.empty()) {
        ss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    for (const auto& meta : entry.metadata) {
        ss << " [" << meta.first << "=" << meta.second << "]";
    }
    
    return ss.str();
}

std::string UnifiedLogger::formatColored(const LogEntry& entry) const {
    // ANSI color codes implementation
    return formatLogEntry(entry);
}

void UnifiedLogger::addConsoleSink() {
    // Add console sink implementation
}

void UnifiedLogger::addFileSink(const std::string& filename) {
    // Add file sink implementation
}

void UnifiedLogger::addJsonSink(const std::string& filename) {
    // Add JSON sink implementation
}

std::vector<UnifiedLogger::LogEntry> UnifiedLogger::getRecentLogs(size_t limit) const {
    if (logBuffer.size() <= limit) {
        return logBuffer;
    }
    
    std::vector<LogEntry> recent(logBuffer.end() - limit, logBuffer.end());
    return recent;
}

std::vector<UnifiedLogger::LogEntry> UnifiedLogger::searchLogs(const std::string& pattern, Level level) const {
    std::vector<LogEntry> results;
    // Implementation for searching logs
    return results;
}

void UnifiedLogger::setLogRotation(size_t maxSize, size_t maxFiles) {
    maxLogSize = maxSize;
    maxLogFiles = maxFiles;
    rotateLogs();
}

void UnifiedLogger::logPerformance(const std::string& operation, std::chrono::duration<double> duration,
                                   const std::map<std::string, std::string>& metadata) {
    std::stringstream ss;
    ss << "Performance: " << operation << " took " << duration.count() << "s";
    
    for (const auto& meta : metadata) {
        ss << " [" << meta.first << "=" << meta.second << "]";
    }
    
    info("Performance", ss.str());
}

void UnifiedLogger::rotateLogs() {
    // Log rotation implementation
}

std::string UnifiedLogger::levelToString(Level level) const {
    switch (level) {
        case Level::OFF: return "OFF";
        case Level::FATAL: return "FATAL";
        case Level::ERROR: return "ERROR";
        case Level::WARN: return "WARN";
        case Level::INFO: return "INFO";
        case Level::DEBUG: return "DEBUG";
        case Level::TRACE: return "TRACE";
        default: return "UNKNOWN";
    }
}

// ConfigManager implementation
ConfigManager::ConfigManager() {
    // Register default options
    registerOption({"neuron_count", "Number of neurons in the brain",
                   "int", false, "1000"});
    registerOption({"region_count", "Number of brain regions",
                   "int", false, "1"});
    registerOption({"connection_probability", "Probability of synaptic connections",
                   "float", false, "0.1"});
    registerOption({"simulation_timestep", "Time step for simulation",
                   "float", false, "0.001"});
    registerOption({"random_seed", "Random seed for reproducible experiments",
                   "int", false, "42"});
    registerOption({"log_level", "Logging level",
                   "string", false, "INFO"});
    registerOption({"enable_development", "Enable developmental plasticity",
                   "bool", false, "true"});
    registerOption({"enable_checkpointing", "Enable checkpoint saving",
                   "bool", false, "true"});
    registerOption({"enable_replay", "Enable memory replay",
                   "bool", false, "true"});
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::registerOption(const ConfigValue& option) {
    options[option.key] = option;
}

void ConfigManager::registerOption(const std::string& key, const std::string& description,
                                   const std::string& type, bool required,
                                   const std::string& defaultValue) {
    ConfigValue option;
    option.key = key;
    option.description = description;
    option.type = type;
    option.required = required;
    option.defaultValue = defaultValue;
    option.allowedValues = {};
    
    registerOption(option);
}

std::string ConfigManager::getString(const std::string& key) const {
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second;
    }
    
    auto optIt = options.find(key);
    if (optIt != options.end()) {
        return optIt->second.defaultValue;
    }
    
    return "";
}

int ConfigManager::getInt(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stoi(value);
    } catch (...) {
        return 0;
    }
}

float ConfigManager::getFloat(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stof(value);
    } catch (...) {
        return 0.0f;
    }
}

bool ConfigManager::getBool(const std::string& key) const {
    std::string value = getString(key);
    return value == "true" || value == "1" || value == "yes";
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    values[key] = value;
    validateOption(options[key], value);
}

void ConfigManager::setInt(const std::string& key, int value) {
    values[key] = std::to_string(value);
    validateOption(options[key], values[key]);
}

void ConfigManager::setFloat(const std::string& key, float value) {
    values[key] = std::to_string(value);
    validateOption(options[key], values[key]);
}

void ConfigManager::setBool(const std::string& key, bool value) {
    values[key] = value ? "true" : "false";
    validateOption(options[key], values[key]);
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = utils::trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, eqPos));
            std::string value = utils::trim(line.substr(eqPos + 1));
            setString(key, value);
        }
    }
    
    file.close();
    return true;
}

bool ConfigManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : values) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
    return true;
}

bool ConfigManager::loadFromCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t eqPos = key.find('=');
            if (eqPos != std::string::npos) {
                std::string valueKey = key.substr(0, eqPos);
                std::string value = key.substr(eqPos + 1);
                setString(valueKey, value);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                setString(key, argv[++i]);
            } else {
                setString(key, "true");
            }
        } else if (arg.substr(0, 1) == "-") {
            // Handle short options
            std::string shortOpt = arg.substr(1);
            for (char c : shortOpt) {
                // Map short options to keys
                std::map<char, std::string> shortToLong = {
                    {'h', "help"}, {'v', "version"}
                };
                
                if (shortToLong.find(c) != shortToLong.end()) {
                    setString(shortToLong[c], "true");
                }
            }
        }
    }
    
    return true;
}

bool ConfigManager::validate() const {
    validationErrors.clear();
    
    for (const auto& pair : options) {
        const ConfigValue& option = pair.second;
        
        if (option.required && values.find(option.key) == values.end()) {
            validationErrors.push_back("Required option missing: " + option.key);
        }
        
        if (values.find(option.key) != values.end()) {
            validateOption(option, values.at(option.key));
        }
    }
    
    return validationErrors.empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    return validationErrors;
}

std::string ConfigManager::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    
    bool first = true;
    for (const auto& pair : values) {
        if (!first) ss << ",\n";
        first = false;
        
        ss << "  \"" << pair.first << "\": \"" << pair.second << "\"";
    }
    
    ss << "\n}\n";
    return ss.str();
}

std::string ConfigManager::exportToYAML() const {
    std::stringstream ss;
    ss << "# NLM Configuration\n";
    
    for (const auto& pair : values) {
        ss << pair.first << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

std::string ConfigManager::exportToINI() const {
    std::stringstream ss;
    ss << "[NLM Configuration]\n";
    
    for (const auto& pair : values) {
        ss << pair.first << " = " << pair.second << "\n";
    }
    
    return ss.str();
}

bool ConfigManager::importFromJSON(const std::string& json) {
    // JSON parsing implementation
    return false;
}

bool ConfigManager::importFromYAML(const std::string& yaml) {
    // YAML parsing implementation
    return false;
}

bool ConfigManager::importFromINI(const std::string& ini) {
    // INI parsing implementation
    return false;
}

std::vector<ConfigManager::ConfigValue> ConfigManager::getAllOptions() const {
    std::vector<ConfigValue> result;
    for (const auto& pair : options) {
        result.push_back(pair.second);
    }
    return result;
}

ConfigManager::ConfigValue ConfigManager::getOption(const std::string& key) const {
    auto it = options.find(key);
    if (it != options.end()) {
        return it->second;
    }
    
    ConfigValue empty;
    empty.key = key;
    empty.description = "Unknown option";
    empty.type = "string";
    empty.required = false;
    return empty;
}

void ConfigManager::loadTemplate(const std::string& name) {
    // Template loading implementation
}

void ConfigManager::saveTemplate(const std::string& name) {
    // Template saving implementation
}

std::vector<std::string> ConfigManager::getAvailableTemplates() const {
    return {"default", "research", "production", "debug"};
}

void ConfigManager::validateOption(const ConfigValue& option, const std::string& value) const {
    // Validation implementation
    if (!option.allowedValues.empty()) {
        if (std::find(option.allowedValues.begin(), option.allowedValues.end(), value) == option.allowedValues.end()) {
            validationErrors.push_back("Invalid value for option '" + option.key + "': " + value);
        }
    }
}

// CommandHistory implementation
CommandHistory::CommandHistory() : maxHistorySize(1000) {
}

CommandHistory::~CommandHistory() {
}

void CommandHistory::addCommand(const std::string& command, bool success, double executionTime,
                               const std::string& output, const std::map<std::string, std::string>& metadata) {
    CommandEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.command = command;
    entry.success = success;
    entry.executionTime = executionTime;
    entry.output = output;
    entry.metadata = metadata;
    
    commands.push_back(entry);
    
    if (commands.size() > maxHistorySize) {
        commands.erase(commands.begin(), commands.begin() + (commands.size() - maxHistorySize));
    }
}

std::vector<CommandHistory::CommandEntry> CommandHistory::getRecentCommands(size_t limit) const {
    if (commands.size() <= limit) {
        return commands;
    }
    
    std::vector<CommandEntry> recent(commands.end() - limit, commands.end());
    return recent;
}

std::vector<CommandHistory::CommandEntry> CommandHistory::searchCommands(const std::string& pattern) const {
    std::vector<CommandEntry> results;
    // Command search implementation
    return results;
}

void CommandHistory::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    // Save command history to file
    file.close();
}

bool CommandHistory::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Load command history from file
    file.close();
    return true;
}

void CommandHistory::clear() {
    commands.clear();
}

// utils namespace implementation
namespace utils {

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::stringstream ss;
    bool first = true;
    for (const auto& part : parts) {
        if (!first) ss << delimiter;
        ss << part;
        first = false;
    }
    return ss.str();
}

std::string format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // Implementation for format string
    std::vector<char> buffer(256);
    va_list args_copy;
    va_copy(args_copy, args);
    
    while (true) {
        va_list args_copy2;
        va_copy(args_copy2, args_copy);
        int result = std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args_copy2);
        va_end(args_copy2);
        
        if (result >= 0 && result < static_cast<int>(buffer.size())) {
            return std::string(buffer.data(), result);
        }
        
        buffer.resize(buffer.size() * 2);
        va_end(args_copy);
        va_copy(args_copy, args);
    }
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::chrono::duration<double> getElapsedTime(const std::chrono::system_clock::time_point& start) {
    return std::chrono::system_clock::now() - start;
}

bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool directoryExists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR) != 0;
}

std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return filename.substr(dotPos + 1);
}

std::string getBasename(const std::string& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == std::string::npos) return path;
    return path.substr(slashPos + 1);
}

std::string getDirectory(const std::string& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == std::string::npos) return ".";
    return path.substr(0, slashPos);
}

void setProcessTitle(const std::string& title) {
    // Set process title implementation
}

void setProcessPriority(const std::string& priority) {
    // Set process priority implementation
}

std::string getSystemInfo() {
    // System info implementation
    return "Linux x86_64";
}

std::string getCompilerInfo() {
    // Compiler info implementation
    return "g++ 11.0+";
}

std::string getBuildInfo() {
    return "NLM Phase 6 - Final Integration";
}

} // namespace utils

} // namespace cli
} // namespace nlm
