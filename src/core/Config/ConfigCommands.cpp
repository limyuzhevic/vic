#include "ConfigCommands.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <cctype>
#include <regex>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ConfigCommandSystem::Impl {
    std::shared_ptr<Config> config;
    std::vector<std::string> history;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> templates;
    std::string lastError;
    int errorCode;
    
    Impl() : config(std::make_shared<Config>()), errorCode(0) {}
};

ConfigCommandSystem::ConfigCommandSystem() : pImpl(std::make_unique<Impl>()) {}

ConfigCommandSystem::~ConfigCommandSystem() = default;

bool ConfigCommandSystem::executeCommand(const std::string& command) {
    // Parse command and execute
    std::vector<std::string> args;
    if (!parseCommandArgs(command, args)) {
        pImpl->lastError = "Invalid command format: " + command;
        pImpl->errorCode = 1;
        return false;
    }
    
    if (args.empty()) {
        pImpl->lastError = "Empty command";
        pImpl->errorCode = 2;
        return false;
    }
    
    std::string cmd = args[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "set") {
        return setCommand(args);
    } else if (cmd == "get") {
        // Note: getCommand doesn't return bool, this is an error
        return false;
    } else if (cmd == "remove") {
        return removeCommand(args);
    } else if (cmd == "list") {
        listCommand(args);
        return true;
    } else if (cmd == "validate") {
        return validateCommand(args);
    } else if (cmd == "save") {
        return saveCommand(args);
    } else if (cmd == "load") {
        return loadCommand(args);
    } else if (cmd == "backup") {
        return backupCommand(args);
    } else if (cmd == "restore") {
        return restoreCommand(args);
    } else if (cmd == "export") {
        return exportCommand(args);
    } else if (cmd == "import") {
        return importCommand(args);
    } else {
        pImpl->lastError = "Unknown command: " + cmd;
        pImpl->errorCode = 3;
        return false;
    }
}

bool ConfigCommandSystem::executeFileCommand(const std::string& filepath) {
    // Execute commands from a file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open command file: " + filepath;
        pImpl->errorCode = 4;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        if (!executeCommand(line)) {
            pImpl->lastError = "Error in command file " + filepath + ", line " + 
                             std::to_string(lineNumber) + ": " + pImpl->lastError;
            return false;
        }
        
        addConfigHistory(line, getTimestamp());
    }
    
    return true;
}

bool ConfigCommandSystem::setConfig(const std::string& key, const std::string& value) {
    return setAdvancedConfig(key, value);
}

bool ConfigCommandSystem::getConfig(const std::string& key, std::string& value) const {
    auto val = pImpl->config->get<std::string>(key);
    if (val.has_value()) {
        value = val.value();
        return true;
    }
    return false;
}

bool ConfigCommandSystem::removeConfig(const std::string& key) {
    pImpl->config->remove(key);
    return true;
}

bool ConfigCommandSystem::listConfig() const {
    auto keys = pImpl->config->getKeys();
    for (const auto& key : keys) {
        auto val = pImpl->config->get<std::string>(key);
        if (val.has_value()) {
            std::cout << key << " = " << val.value() << std::endl;
        }
    }
    return true;
}

bool ConfigCommandSystem::validateConfig() const {
    return validateConfigConsistency();
}

bool ConfigCommandSystem::checkConfigConsistency() const {
    float score = calculateConfigConsistencyScore();
    if (score < 0.5f) {
        pImpl->lastError = "Configuration inconsistency detected (score: " + 
                         std::to_string(score) + ")";
        pImpl->errorCode = 5;
        return false;
    }
    return true;
}

// Configuration commands
bool ConfigCommandSystem::setCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "set command requires key and value";
        pImpl->errorCode = 6;
        return false;
    }
    
    std::string key = args[1];
    std::string value = args.size() > 2 ? args[2] : "";
    
    return setAdvancedConfig(key, value);
}

bool ConfigCommandSystem::getCommand(const std::vector<std::string>& args) const {
    // Note: This is a const version, should be different from non-const
    if (args.size() < 2) {
        return false;
    }
    
    // Implementation should be here
    return false;
}

bool ConfigCommandSystem::removeCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "remove command requires key";
        pImpl->errorCode = 7;
        return false;
    }
    
    return removeConfig(args[1]);
}

bool ConfigCommandSystem::listCommand(const std::vector<std::string>& args) const {
    if (!listConfig()) {
        return false;
    }
    return true;
}

bool ConfigCommandSystem::validateCommand(const std::vector<std::string>& args) const {
    if (!validateConfig()) {
        return false;
    }
    return true;
}

bool ConfigCommandSystem::saveCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "save command requires filename";
        pImpl->errorCode = 8;
        return false;
    }
    
    return pImpl->config->saveToFile(args[1]);
}

bool ConfigCommandSystem::loadCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "load command requires filename";
        pImpl->errorCode = 9;
        return false;
    }
    
    // Clear existing config
    pImpl->config->clear();
    
    // Load from file (will use fallback key=value format for now)
    if (!pImpl->config->loadFromFile(args[1])) {
        pImpl->lastError = "Failed to load configuration from: " + args[1];
        pImpl->errorCode = 10;
        return false;
    }
    
    return true;
}

bool ConfigCommandSystem::backupCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "backup command requires filename";
        pImpl->errorCode = 11;
        return false;
    }
    
    // Simple backup: save current config
    return saveCommand(args);
}

bool ConfigCommandSystem::restoreCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "restore command requires filename";
        pImpl->errorCode = 12;
        return false;
    }
    
    // Simple restore: load backup
    return loadCommand(args);
}

bool ConfigCommandSystem::exportCommand(const std::vector<std::string>& args) const {
    if (args.size() < 2) {
        pImpl->lastError = "export command requires format and filename";
        pImpl->errorCode = 13;
        return false;
    }
    
    std::string format = args[1];
    std::string filepath = args.size() > 2 ? args[2] : "";
    
    if (format == "json") {
        return exportToJSON(filepath);
    } else if (format == "yaml") {
        return exportToYAML(filepath);
    } else if (format == "ini") {
        return exportToINI(filepath);
    } else if (format == "properties") {
        return exportToProperties(filepath);
    } else {
        pImpl->lastError = "Unsupported export format: " + format;
        pImpl->errorCode = 14;
        return false;
    }
}

bool ConfigCommandSystem::importCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        pImpl->lastError = "import command requires format and filename";
        pImpl->errorCode = 15;
        return false;
    }
    
    std::string format = args[1];
    std::string filepath = args.size() > 2 ? args[2] : "";
    
    // Clear existing config
    pImpl->config->clear();
    
    if (format == "json") {
        return importFromJSON(filepath);
    } else if (format == "yaml") {
        return importFromYAML(filepath);
    } else if (format == "ini") {
        return importFromINI(filepath);
    } else if (format == "properties") {
        return importFromProperties(filepath);
    } else {
        pImpl->lastError = "Unsupported import format: " + format;
        pImpl->errorCode = /column_16;
        return false;
    }
}

// Configuration validation
bool ConfigCommandSystem::validateFileFormat(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for validation: " + filepath;
        pImpl->errorCode = 16;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
    
    // Check if it's JSON
    if (!content.empty() && content[0] == '{') {
        try {
            auto jsonData = json::parse(content);
            return true;
        } catch (...) {
            // Fall through to other formats
        }
    }
    
    // Check if it's YAML (basic check)
    if (content.find(':') != std::string::npos && content.find('-') != std::string::npos) {
        // Likely YAML
        return true;
    }
    
    // Check if it's key=value format
    std::istringstream iss(content);
    std::string line;
    int keyValueCount = 0;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        if (line.find('=') != std::string::npos) {
            keyValueCount++;
        }
    }
    
    return keyValueCount > 0;
}

bool ConfigCommandSystem::validateKeyFormat(const std::string& key) const {
    return isValidKey(key);
}

bool ConfigCommandSystem::validateValueFormat(const std::string& key, const std::string& value) const {
    // Basic validation based on key
    if (key.find("rate") != std::string::npos || key.find("percentage") != std::string::npos) {
        // Should be numeric between 0 and 100
        try {
            double val = std::stod(value);
            return val >= 0.0 && val <= 100.0;
        } catch (...) {
            return false;
        }
    } else if (key.find("time") != std::string::npos || key.find("delay") != std::string::npos) {
        // Should be numeric and positive
        try {
            double val = std::stod(value);
            return val >= 0.0;
        } catch (...) {
            return false;
        }
    } else if (key.find("enabled") != std::string::npos || key.find("active") != std::string::npos) {
        // Should be boolean
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    
    // For other keys, accept any value
    return true;
}

// Configuration analysis
size_t ConfigCommandSystem::getConfigCount() const {
    return pImpl->config->getKeys().size();
}

std::vector<std::string> ConfigCommandSystem::getConfigKeys() const {
    return pImpl->config->getKeys();
}

std::string ConfigCommandSystem::getConfigSummary() const {
    return pImpl->config->summary();
}

std::map<std::string, std::string> ConfigCommandSystem::getConfigAsMap() const {
    std::map<std::string, std::string> configMap;
    auto keys = pImpl->config->getKeys();
    
    for (const auto& key : keys) {
        auto val = pImpl->config->get<std::string>(key);
        if (val.has_value()) {
            configMap[key] = val.value();
        }
    }
    
    return configMap;
}

// Configuration history
bool ConfigCommandSystem::addConfigHistory(const std::string& command, const std::string& timestamp) {
    pImpl->history.push_back("[" + timestamp + "] " + command);
    return true;
}

std::vector<std::string> ConfigCommandSystem::getConfigHistory() const {
    return pImpl->history;
}

bool ConfigCommandSystem::clearConfigHistory() {
    pImpl->history.clear();
    return true;
}

// Configuration diff
std::string ConfigCommandSystem::getConfigDiff(const std::string& otherConfigFile) const {
    // Simple diff implementation
    std::ifstream otherFile(otherConfigFile);
    if (!otherFile.is_open()) {
        return "Cannot open other config file: " + otherConfigFile;
    }
    
    std::string diff = "Configuration differences:\n";
    
    // For now, just return a simple message
    diff += "This is a placeholder implementation of config diff.\n";
    diff += "Full implementation would compare two config files and show differences.\n";
    
    return diff;
}

bool ConfigCommandSystem::compareConfigs(const std::string& file1, const std::string& file2) const {
    // Simple comparison
    std::ifstream f1(file1);
    std::ifstream f2(file2);
    
    if (!f1.is_open() || !f2.is_open()) {
        pImpl->lastError = "Cannot open config files for comparison";
        pImpl->errorCode = 17;
        return false;
    }
    
    std::string content1((std::istreambuf_iterator<char>(f1), std::istreambuf_iterator<char>()));
    std::string content2((std::istreambuf_iterator<char>(f2), std::istreambuf_iterator<char>()));
    
    if (content1 == content2) {
        return true;  // Configs are identical
    }
    
    // For now, just return false if different
    return false;
}

// Configuration validation rules
bool ConfigCommandSystem::setValidationRules(const std::string& rulesFile) {
    // Placeholder for validation rules
    pImpl->lastError = "Validation rules not yet implemented";
    pImpl->errorCode = 18;
    return false;
}

bool ConfigCommandSystem::validateAgainstRules(const std::string& configFile) const {
    // Placeholder for validation against rules
    pImpl->lastError = "Validation against rules not yet implemented";
    pImpl->errorCode = 19;
    return false;
}

// Configuration dependencies
bool ConfigCommandSystem::addConfigDependency(const std::string& key, const std::string& dependsOn) {
    pImpl->dependencies[key] = dependsOn;
    return true;
}

bool ConfigCommandSystem::checkConfigDependencies() const {
    for (const auto& dep : pImpl->dependencies) {
        if (!pImpl->config->has(dep.first) || !pImpl->config->has(dep.second)) {
            pImpl->lastError = "Missing configuration dependency: " + dep.first + " depends on " + dep.second;
            pImpl->errorCode = 20;
            return false;
        }
    }
    return true;
}

bool ConfigCommandSystem::removeConfigDependency(const std::string& key) {
    auto it = pImpl->dependencies.find(key);
    if (it != pImpl->dependencies.end()) {
        pImpl->dependencies.erase(it);
    }
    return true;
}

// Configuration templates
bool ConfigCommandSystem::saveConfigTemplate(const std::string& templateName) {
    pImpl->templates[templateName] = getConfigSummary();
    return true;
}

bool ConfigCommandSystem::loadConfigTemplate(const std::string& templateName) {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        pImpl->lastError = "Template not found: " + templateName;
        pImpl->errorCode = 21;
        return false;
    }
    
    // Clear existing config
    pImpl->config->clear();
    
    // In a real implementation, this would load the template config
    pImpl->lastError = "Template loading not yet implemented";
    pImpl->errorCode = 22;
    return false;
}

bool ConfigCommandSystem::listConfigTemplates() const {
    for (const auto& template : pImpl->templates) {
        std::cout << template.first << std::endl;
    }
    return true;
}

// Configuration export formats
bool ConfigCommandSystem::exportToJSON(const std::string& filepath) const {
    json jsonData;
    auto configMap = getConfigAsMap();
    
    for (const auto& pair : configMap) {
        jsonData[pair.first] = pair.second;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for JSON export: " + filepath;
        pImpl->errorCode = 23;
        return false;
    }
    
    file << jsonData.dump(2);
    return true;
}

bool ConfigCommandSystem::exportToYAML(const std::string& filepath) const {
    // For YAML, we'll use the same format as JSON for now
    return exportToJSON(filepath);
}

bool ConfigCommandSystem::exportToINI(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for INI export: " + filepath;
        pImpl->errorCode = 24;
        return false;
    }
    
    for (const auto& pair : getConfigAsMap()) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    return true;
}

bool ConfigCommandSystem::exportToProperties(const std::string& filepath) const {
    // Similar to INI format
    return exportToINI(filepath);
}

// Configuration import formats
bool ConfigCommandSystem::importFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for JSON import: " + filepath;
        pImpl->errorCode = 25;
        return false;
    }
    
    try {
        json jsonData = json::parse(file);
        
        // Clear existing config
        pImpl->config->clear();
        
        // Import from JSON
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string value = it.value().dump();
            
            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            setAdvancedConfig(key, value);
        }
        
        return true;
    } catch (...) {
        pImpl->lastError = "Error parsing JSON file: " + filepath;
        pImpl->errorCode = 26;
        return false;
    }
}

bool ConfigCommandSystem::importFromYAML(const std::string& filepath) {
    // For now, use JSON import (simplified)
    return importFromJSON(filepath);
}

bool ConfigCommandSystem::importFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for INI import: " + filepath;
        pImpl->errorCode = 27;
        return false;
    }
    
    // Clear existing config
    pImpl->config->clear();
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            setAdvancedConfig(key, value);
        }
    }
    
    return true;
}

bool ConfigCommandSystem::importFromProperties(const std::string& filepath) {
    // Similar to INI import
    return importFromINI(filepath);
}

// Private helper methods
std::string ConfigCommandSystem::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigCommandSystem::isValidKey(const std::string& key) const {
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool ConfigCommandSystem::isValidValue(const std::string& value) const {
    // Basic validation - check for common invalid characters
    for (char c : value) {
        if (c == '\0' || c == '\n' || c == '\r' || c == '\t') {
            return false;
        }
    }
    
    return true;
}

std::string ConfigCommandSystem::getTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* timeInfo = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    
    return oss.str();
}

bool ConfigCommandSystem::parseCommandArgs(const std::string& command, std::vector<std::string>& args) const {
    std::istringstream iss(command);
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    return !args.empty();
}

std::string ConfigCommandSystem::getErrorMessage(int errorCode) const {
    static const std::map<int, std::string> errorMessages = {
        {1, "Invalid command format"},
        {2, "Empty command"},
        {3, "Unknown command"},
        {4, "Cannot open command file"},
        {5, "Configuration inconsistency detected"},
        {6, "set command requires key and value"},
        {7, "remove command requires key"},
        {8, "save command requires filename"},
        {9, "load command requires filename"},
        {10, "Failed to load configuration"},
        {11, "backup command requires filename"},
        {12, "restore command requires filename"},
        {13, "export command requires format and filename"},
        {14, "Unsupported export format"},
        {15, "import command requires format and filename"},
        {16, "Cannot open file for validation"},
        {17, "Cannot open config files for comparison"},
        {18, "Validation rules not yet implemented"},
        {19, "Validation against rules not yet implemented"},
        {20, "Missing configuration dependency"},
        {21, "Template not found"},
        {22, "Template loading not yet implemented"},
        {23, "Cannot open file for JSON export"},
        {24, "Cannot open file for INI export"},
        {25, "Cannot open file for JSON import"},
        {26, "Error parsing JSON file"},
        {27, "Cannot open file for INI import"}
    };
    
    auto it = errorMessages.find(errorCode);
    if (it != errorMessages.end()) {
        return it->second;
    }
    
    return "Unknown error";
}

// Configuration validation
bool ConfigCommandSystem::validateNumeric(const std::string& value) const {
    try {
        std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigCommandSystem::validateInteger(const std::string& value) const {
    try {
        std::stoll(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigCommandSystem::validateFloat(const std::string& value) const {
    return validateNumeric(value);
}

bool ConfigCommandSystem::validateBoolean(const std::string& value) const {
    return value == "true" || value == "false" || value == "1" || value == "0";
}

bool ConfigCommandSystem::validateString(const std::string& value) const {
    return !value.empty() && isValidValue(value);
}

bool ConfigCommandSystem::validateVector(const std::string& value) const {
    // Very basic check for vector format
    return value.find('[') != std::string::npos && value.find(']') != std::string::npos;
}

// Command parsing
bool ConfigCommandSystem::parseSetCommand(const std::vector<std::string>& args) {
    // Implementation
    return false;
}

bool ConfigCommandSystem::parseGetCommand(const std::vector<std::string>& args) const {
    // Implementation
    return false;
}

bool ConfigCommandSystem::parseRemoveCommand(const std::vector<std::string>& args) {
    // Implementation
    return false;
}

bool ConfigCommandSystem::parseListCommand(const std::vector<std::string>& args) const {
    // Implementation
    return false;
}

bool ConfigCommandSystem::parseSaveCommand(const std::vector<std::string>& args) {
    // Implementation
    return false;
}

bool ConfigCommandSystem::parseLoadCommand(const std::vector<std::string>& args) {
    // Implementation
    return false;
}

// Configuration file operations
bool ConfigCommandSystem::saveConfigToFile(const std::string& filepath) const {
    return pImpl->config->saveToFile(filepath);
}

bool ConfigCommandSystem::loadConfigFromFile(const std::string& filepath) {
    return pImpl->config->loadFromFile(filepath);
}

bool ConfigCommandSystem::backupConfig(const std::string& filepath) const {
    return saveConfigToFile(filepath);
}

bool ConfigCommandSystem::restoreConfig(const std::string& filepath) {
    return loadConfigFromFile(filepath);
}

// Configuration analysis
float ConfigCommandSystem::calculateConfigConsistencyScore() const {
    // Simple scoring based on number of entries
    size_t count = getConfigCount();
    if (count == 0) return 0.0f;
    
    // For now, return a score based on configuration completeness
    // Higher score for more complete configurations
    return std::min(1.0f, static_cast<float>(count) / 100.0f);
}

std::vector<std::string> ConfigCommandSystem::findConfigConflicts() const {
    // For now, return empty (no conflicts detected)
    return std::vector<std::string>();
}

std::vector<std::string> ConfigCommandSystem::findConfigDuplicates() const {
    // For now, return empty (no duplicates detected)
    return std::vector<std::string>();
}

// Configuration history
bool ConfigCommandSystem::logConfigChange(const std::string& key, const std::string& oldValue, const std::string& newValue) {
    std::string timestamp = getTimestamp();
    std::string command = "set " + key + " = " + newValue;
    return addConfigHistory(command, timestamp);
}

// Configuration export
bool ConfigCommandSystem::exportToFormat(const std::string& format, const std::string& filepath) const {
    if (format == "json") {
        return exportToJSON(filepath);
    } else if (format == "yaml") {
        return exportToYAML(filepath);
    } else if (format == "ini") {
        return exportToINI(filepath);
    } else if (format == "properties") {
        return exportToProperties(filepath);
    }
    
    pImpl->lastError = "Unsupported export format: " + format;
    pImpl->errorCode = 28;
    return false;
}

bool ConfigCommandSystem::importFromFormat(const std::string& format, const std::string& filepath) {
    if (format == "json") {
        return importFromJSON(filepath);
    } else if (format == "yaml") {
        return importFromYAML(filepath);
    } else if (format == "ini") {
        return importFromINI(filepath);
    } else if (format == "properties") {
        return importFromProperties(filepath);
    }
    
    pImpl->lastError = "Unsupported import format: " + format;
    pImpl->errorCode = 29;
    return false;
}

} // namespace nlm