// Advanced Configuration Management System Implementation

#include "core/Config/ConfigManager.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <filesystem>
#include <ctime>
#include <mutex>
#include <any>

namespace nlm {
namespace config {

// ConfigOption implementation
std::string ConfigOption::getString() const {
    return defaultValue;
}

int ConfigOption::getInt() const {
    try {
        return std::stoi(defaultValue);
    } catch (...) {
        return 0;
    }
}

float ConfigOption::getFloat() const {
    try {
        return std::stof(defaultValue);
    } catch (...) {
        return 0.0f;
    }
}

double ConfigOption::getDouble() const {
    try {
        return std::stod(defaultValue);
    } catch (...) {
        return 0.0;
    }
}

bool ConfigOption::getBool() const {
    return defaultValue == "true" || defaultValue == "1" || defaultValue == "yes";
}

std::vector<std::string> ConfigOption::getVectorString() const {
    std::vector<std::string> result;
    std::stringstream ss(defaultValue);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = utils::trim(item);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<int> ConfigOption::getVectorInt() const {
    std::vector<int> result;
    std::vector<std::string> strings = getVectorString();
    for (const auto& str : strings) {
        try {
            result.push_back(std::stoi(str));
        } catch (...) {
            result.push_back(0);
        }
    }
    return result;
}

std::vector<float> ConfigOption::getVectorFloat() const {
    std::vector<float> result;
    std::vector<std::string> strings = getVectorString();
    for (const auto& str : strings) {
        try {
            result.push_back(std::stof(str));
        } catch (...) {
            result.push_back(0.0f);
        }
    }
    return result;
}

std::map<std::string, std::string> ConfigOption::getMapStringString() const {
    std::map<std::string, std::string> result;
    // Simple key=value parsing
    std::stringstream ss(defaultValue);
    std::string line;
    while (std::getline(ss, line)) {
        line = utils::trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, eqPos));
            std::string value = utils::trim(line.substr(eqPos + 1));
            result[key] = value;
        }
    }
    return result;
}

bool ConfigOption::validateValue(const std::string& value) const {
    // Check allowed values
    if (!allowedValues.empty()) {
        if (std::find(allowedValues.begin(), allowedValues.end(), value) == allowedValues.end()) {
            return false;
        }
    }
    
    // Check regex if provided
    if (!validationRegex.empty()) {
        std::regex pattern(validationRegex);
        if (!std::regex_match(value, pattern)) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigOption::getValidationError(const std::string& value) const {
    if (!validateValue(value)) {
        if (!allowedValues.empty()) {
            return "Value must be one of: " + utils::join(allowedValues, ", ");
        } else if (!validationRegex.empty()) {
            return "Value does not match pattern: " + validationRegex;
        }
    }
    return "";
}

std::string ConfigOption::serialize() const {
    std::stringstream ss;
    ss << "key:" << key << "\n";
    ss << "description:" << description << "\n";
    ss << "type:" << static_cast<int>(type) << "\n";
    ss << "required:" << (required ? "true" : "false") << "\n";
    ss << "default:" << defaultValue << "\n";
    
    if (!allowedValues.empty()) {
        ss << "allowed:";
        for (const auto& val : allowedValues) {
            ss << " " << val;
        }
        ss << "\n";
    }
    
    if (!validationRegex.empty()) {
        ss << "validation:" << validationRegex << "\n";
    }
    
    ss << "sensitive:" << (sensitive ? "true" : "false") << "\n";
    
    return ss.str();
}

bool ConfigOption::deserialize(const std::string& str) {
    std::istringstream iss(str);
    std::string line;
    
    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = utils::trim(line.substr(0, colonPos));
        std::string value = utils::trim(line.substr(colonPos + 1));
        
        if (key == "key") {
            this->key = value;
        } else if (key == "description") {
            description = value;
        } else if (key == "type") {
            type = static_cast<ConfigType>(std::stoi(value));
        } else if (key == "required") {
            required = (value == "true");
        } else if (key == "default") {
            defaultValue = value;
        } else if (key == "allowed") {
            std::istringstream valStream(value);
            std::string allowedVal;
            while (valStream >> allowedVal) {
                allowedValues.push_back(allowedVal);
            }
        } else if (key == "validation") {
            validationRegex = value;
        } else if (key == "sensitive") {
            sensitive = (value == "true");
        }
    }
    
    return true;
}

bool ConfigOption::operator==(const ConfigOption& other) const {
    return key == other.key && description == other.description &&
           type == other.type && required == other.required &&
           defaultValue == other.defaultValue && allowedValues == other.allowedValues &&
           validationRegex == other.validationRegex && sensitive == other.sensitive;
}

// ConfigManager implementation
ConfigManager::ConfigManager() {
    configFilePath = "configs/";
    historyEnabled = false;
    maxHistoryEntries = 1000;
    
    // Initialize with default options
    registerOption("neuron_count", "Number of neurons in the brain", ConfigType::INT, false, "1000");
    registerOption("region_count", "Number of brain regions", ConfigType::INT, false, "1");
    registerOption("connection_probability", "Probability of synaptic connections", ConfigType::FLOAT, false, "0.1");
    registerOption("simulation_timestep", "Time step for simulation", ConfigType::FLOAT, false, "0.001");
    registerOption("random_seed", "Random seed for reproducible experiments", ConfigType::INT, false, "42");
    registerOption("log_level", "Logging level (OFF, FATAL, ERROR, WARN, INFO, DEBUG, TRACE)", ConfigType::STRING, false, "INFO");
    registerOption("enable_development", "Enable developmental plasticity", ConfigType::BOOL, false, "true");
    registerOption("enable_checkpointing", "Enable checkpoint saving", ConfigType::BOOL, false, "true");
    registerOption("enable_replay", "Enable memory replay", ConfigType::BOOL, false, "true");
    registerOption("output_file", "Output file path", ConfigType::STRING, false, "");
    registerOption("verbose", "Enable verbose output", ConfigType::BOOL, false, "false");
    registerOption("benchmark", "Run performance benchmark", ConfigType::BOOL, false, "false");
    registerOption("profile", "Enable profiling", ConfigType::BOOL, false, "false");
    registerOption("checkpoint_path", "Checkpoint file path", ConfigType::STRING, false, "./checkpoint.bin");
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::registerOption(const ConfigOption& option) {
    std::lock_guard<std::mutex> lock(mutex);
    options[option.key] = option;
    
    // Initialize value from default if not already set
    if (values.find(option.key) == values.end()) {
        values[option.key] = ConfigValue(option.key, option.defaultValue, "default");
    }
}

void ConfigManager::registerOption(const std::string& key, const std::string& description,
                                   ConfigType type, bool required, const std::string& defaultValue,
                                   const std::vector<std::string>& allowedValues) {
    ConfigOption option;
    option.key = key;
    option.description = description;
    option.type = type;
    option.required = required;
    option.defaultValue = defaultValue;
    option.allowedValues = allowedValues;
    
    registerOption(option);
}

std::string ConfigManager::getString(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second.rawValue;
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

double ConfigManager::getDouble(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool ConfigManager::getBool(const std::string& key) const {
    std::string value = getString(key);
    return value == "true" || value == "1" || value == "yes";
}

void ConfigManager::setString(const std::string& key, const std::string& value,
                             const std::string& source) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto optIt = options.find(key);
    if (optIt == options.end()) {
        return; // Unknown option
    }
    
    const ConfigOption& option = optIt->second;
    
    // Validate value
    if (!option.validateValue(value)) {
        return; // Invalid value
    }
    
    std::string oldValue = "";
    auto valIt = values.find(key);
    if (valIt != values.end()) {
        oldValue = valIt->second.rawValue;
    }
    
    values[key] = ConfigValue(key, value, source);
    updateHistory(key, value);
    
    // Call change callback if set
    if (changeCallback && oldValue != value) {
        changeCallback(key, oldValue, value);
    }
}

void ConfigManager::setInt(const std::string& key, int value, const std::string& source) {
    setString(key, std::to_string(value), source);
}

void ConfigManager::setFloat(const std::string& key, float value, const std::string& source) {
    setString(key, std::to_string(value), source);
}

void ConfigManager::setDouble(const std::string& key, double value, const std::string& source) {
    setString(key, std::to_string(value), source);
}

void ConfigManager::setBool(const std::string& key, bool value, const std::string& source) {
    setString(key, value ? "true" : "false", source);
}

bool ConfigManager::loadFromFile(const std::string& filename, ConfigFormat format) {
    switch (format) {
        case ConfigFormat::INI:
            return loadINI(filename);
        case ConfigFormat::JSON:
            return loadJSON(filename);
        case ConfigFormat::YAML:
            return loadYAML(filename);
        default:
            return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filename, ConfigFormat format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    switch (format) {
        case ConfigFormat::INI:
            return saveINI(filename);
        case ConfigFormat::JSON:
            return saveJSON(filename);
        case ConfigFormat::YAML:
            return saveYAML(filename);
        default:
            return false;
    }
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
                setString(valueKey, value, "command_line");
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                setString(key, argv[++i], "command_line");
            } else {
                setString(key, "true", "command_line");
            }
        } else if (arg.substr(0, 1) == "-") {
            // Handle short options
            std::string shortOpt = arg.substr(1);
            for (char c : shortOpt) {
                std::map<char, std::string> shortToLong = {
                    {'h', "help"}, {'v', "version"}, {'c', "config"},
                    {'n', "neuron-count"}, {'t', "timestep"}, {'s', "seed"}
                };
                
                if (shortToLong.find(c) != shortToLong.end()) {
                    setString(shortToLong[c], "true", "command_line");
                }
            }
        }
    }
    
    return true;
}

bool ConfigManager::loadFromEnvironment(const std::map<std::string, std::string>& env) {
    for (const auto& pair : env) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Convert environment variable key to config key
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        
        // Remove NLM_ prefix if present
        if (key.substr(0, 4) == "nlm_") {
            key = key.substr(4);
        }
        
        // Replace _ with -
        std::replace(key.begin(), key.end(), '_', '-');
        
        setString(key, value, "environment");
    }
    
    return true;
}

bool ConfigManager::saveTemplate(const std::string& name, const std::string& description) {
    std::string dir = getTemplateDir();
    std::string filename = getTemplateFilePath(name);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Save template header
    file << "# NLM Configuration Template" << std::endl;
    file << "# Name: " << name << std::endl;
    file << "# Description: " << description << std::endl;
    file << "# Generated: " << utils::getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    // Save configuration options
    file << "[Options]" << std::endl;
    for (const auto& pair : options) {
        file << pair.first << " = " << pair.second.defaultValue << std::endl;
    }
    
    file.close();
    return true;
}

bool ConfigManager::loadTemplate(const std::string& name) {
    std::string filename = getTemplateFilePath(name);
    if (!std::filesystem::exists(filename)) {
        return false;
    }
    
    // Clear current options (except maybe core ones)
    std::vector<std::string> coreKeys = {"neuron_count", "region_count", "connection_probability"};
    std::map<std::string, ConfigOption> originalOptions;
    for (const auto& key : coreKeys) {
        auto it = options.find(key);
        if (it != options.end()) {
            originalOptions[key] = it->second;
        }
    }
    
    options.clear();
    values.clear();
    
    // Load from file
    if (!loadINI(filename)) {
        // Restore core options
        options = originalOptions;
        for (const auto& pair : options) {
            values[pair.first] = ConfigValue(pair.first, pair.second.defaultValue, "default");
        }
        return false;
    }
    
    // Reinitialize values from default options
    for (const auto& pair : options) {
        if (values.find(pair.first) == values.end()) {
            values[pair.first] = ConfigValue(pair.first, pair.second.defaultValue, "default");
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getAvailableTemplates() const {
    std::vector<std::string> result;
    std::string dir = getTemplateDir();
    
    if (!std::filesystem::exists(dir)) {
        return result;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && 
            entry.path().extension() == ".nlm" &&
            entry.path().filename().string().find('.') == std::string::npos) {
            result.push_back(entry.path().filename().string());
        }
    }
    
    return result;
}

bool ConfigManager::deleteTemplate(const std::string& name) {
    std::string filename = getTemplateFilePath(name);
    if (!std::filesystem::exists(filename)) {
        return false;
    }
    
    return std::filesystem::remove(filename);
}

void ConfigManager::createGroup(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    groups.push_back(ConfigGroup(name, description));
}

void ConfigManager::addOptionToGroup(const std::string& groupName, const std::string& optionKey) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto groupIt = std::find_if(groups.begin(), groups.end(),
        [&groupName](const ConfigGroup& group) { return group.name == groupName; });
    
    if (groupIt != groups.end()) {
        auto optIt = std::find(groupIt->optionKeys.begin(), groupIt->optionKeys.end(), optionKey);
        if (optIt == groupIt->optionKeys.end()) {
            groupIt->optionKeys.push_back(optionKey);
        }
    }
}

void ConfigManager::removeGroup(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::find_if(groups.begin(), groups.end(),
        [&name](const ConfigGroup& group) { return group.name == name; });n
    if (it != groups.end()) {
        groups.erase(it);
    }
}

std::vector<ConfigGroup> ConfigManager::getGroups() const {
    std::lock_guard<std::mutex> lock(mutex);
    return groups;
}

bool ConfigManager::validate() const {
    std::lock_guard<std::mutex> lock(mutex);
    validationErrors.clear();
    
    for (const auto& pair : options) {
        const ConfigOption& option = pair.second;
        
        // Check if option is required
        if (option.required && values.find(option.key) == values.end()) {
            validationErrors.push_back("Required option missing: " + option.key);
        }
        
        // Check if option has a value
        if (values.find(option.key) != values.end()) {
            const ConfigValue& value = values.at(option.key);
            
            // Validate the value
            if (!option.validateValue(value.rawValue)) {
                validationErrors.push_back("Invalid value for option '" + option.key + "': " + 
                                        option.getValidationError(value.rawValue));
            }
        }
    }
    
    return validationErrors.empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    return validationErrors;
}

void ConfigManager::enableHistory(bool enabled, size_t maxHistoryEntries) {
    std::lock_guard<std::mutex> lock(mutex);
    historyEnabled = enabled;
    this->maxHistoryEntries = maxHistoryEntries;
}

std::vector<ConfigValue> ConfigManager::getHistory(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = history.find(key);
    if (it != history.end()) {
        return it->second;
    }
    return std::vector<ConfigValue>();
}

bool ConfigManager::hasChanged(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = values.find(key);
    if (it == values.end()) {
        return false;
    }
    return it->second.modified;
}

std::string ConfigManager::getCurrentSource(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second.source;
    }
    return "default";
}

bool ConfigManager::equals(const ConfigManager& other) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::lock_guard<std::mutex> otherLock(other.mutex);
    
    if (options.size() != other.options.size()) {
        return false;
    }
    
    for (const auto& pair : options) {
        const std::string& key = pair.first;
        auto it1 = values.find(key);
        auto it2 = other.values.find(key);
        
        if ((it1 == values.end()) != (it2 == other.values.end())) {
            return false;
        }
        
        if (it1 != values.end() && it2 != other.values.end() &&
            it1->second.rawValue != it2->second.rawValue) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::diff(const ConfigManager& other) const {
    std::vector<std::string> result;
    std::lock_guard<std::mutex> lock(mutex);
    std::lock_guard<std::mutex> otherLock(other.mutex);
    
    for (const auto& pair : options) {
        const std::string& key = pair.first;
        
        auto it1 = values.find(key);
        auto it2 = other.values.find(key);
        
        std::string value1 = (it1 != values.end()) ? it1->second.rawValue : "";
        std::string value2 = (it2 != other.values.end()) ? it2->second.rawValue : "";
        
        if (value1 != value2) {
            result.push_back("Key: " + key + "\n  Current: " + value1 + "\n  Other: " + value2 + "\n");
        }
    }
    
    return result;
}

std::string ConfigManager::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    
    bool first = true;
    for (const auto& pair : values) {
        if (!first) ss << ",\n";
        first = false;
        
        ss << "  \"" << pair.first << "\": \"" << pair.second.rawValue << "\"";
    }
    
    ss << "\n}\n";
    return ss.str();
}

std::string ConfigManager::exportToYAML() const {
    std::stringstream ss;
    ss << "# NLM Configuration\n";
    
    for (const auto& pair : values) {
        ss << pair.first << ": " << pair.second.rawValue << "\n";
    }
    
    return ss.str();
}

std::string ConfigManager::exportToINI() const {
    std::stringstream ss;
    ss << "[NLM Configuration]\n";
    
    for (const auto& pair : values) {
        ss << pair.first << " = " << pair.second.rawValue << "\n";
    }
    
    return ss.str();
}

bool ConfigManager::importFromJSON(const std::string& json) {
    // Simplified JSON parsing for key-value pairs
    if (json.empty() || json[0] != '{') {
        return false;
    }
    
    std::istringstream iss(json);
    std::string line;
    bool inObject = false;
    std::string currentKey;
    
    while (std::getline(iss, line)) {
        line = utils::trim(line);
        
        if (line.empty() || line[0] == '}') {
            continue;
        }
        
        if (line.find("\": \"")) {
            size_t colonPos = line.find("\": \"");
            if (colonPos != std::string::npos) {
                std::string key = utils::trim(line.substr(0, colonPos));
                std::string value = utils::trim(line.substr(colonPos + 3)); // Skip ": "
                size_t endQuote = value.find('\"');
                if (endQuote != std::string::npos) {
                    value = value.substr(0, endQuote);
                }
                
                setString(key, value, "json_import");
            }
        }
    }
    
    return true;
}

bool ConfigManager::importFromYAML(const std::string& yaml) {
    // Simplified YAML parsing for key-value pairs
    std::istringstream iss(yaml);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = utils::trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, colonPos));
            std::string value = utils::trim(line.substr(colonPos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && value[0] == '"' && value[value.size() - 1] == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            setString(key, value, "yaml_import");
        }
    }
    
    return true;
}

bool ConfigManager::importFromINI(const std::string& ini) {
    return loadINI("dummy"); // Delegate to INI loader
}

std::string ConfigManager::getConfigFilePath() const {
    return configFilePath;
}

void ConfigManager::setConfigFilePath(const std::string& path) {
    configFilePath = path;
}

bool ConfigManager::configFileExists() const {
    return std::filesystem::exists(configFilePath);
}

void ConfigManager::setChangeCallback(std::function<void(const std::string& key, const std::string& oldValue, const std::string& newValue)> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    changeCallback = callback;
}

void ConfigManager::updateHistory(const std::string& key, const std::string& value) {
    if (!historyEnabled) return;
    
    ConfigValue configValue(key, value, getCurrentSource(key));
    configValue.modified = true;
    
    std::lock_guard<std::mutex> lock(mutex);
    history[key].push_back(configValue);
    
    if (history[key].size() > maxHistoryEntries) {
        history[key].erase(history[key].begin());
    }
}

std::string ConfigManager::getOptionTypeName(ConfigType type) const {
    switch (type) {
        case ConfigType::STRING: return "string";
        case ConfigType::INT: return "int";
        case ConfigType::FLOAT: return "float";
        case ConfigType::DOUBLE: return "double";
        case ConfigType::BOOL: return "bool";
        case ConfigType::VECTOR_STRING: return "vector<string>";
        case ConfigType::VECTOR_INT: return "vector<int>";
        case ConfigType::VECTOR_FLOAT: return "vector<float>";
        case ConfigType::MAP_STRING_STRING: return "map<string,string>";
        default: return "unknown";
    }
}

std::string ConfigManager::convertToString(const std::any& value) const {
    // Simple implementation - could be extended
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    
    return "";
}

std::any ConfigManager::convertToType(const std::string& value, ConfigType type) const {
    // Simple implementation - could be extended
    switch (type) {
        case ConfigType::STRING:
            return value;
        case ConfigType::INT:
            try {
                return std::stoi(value);
            } catch (...) {
                return 0;
            }
        case ConfigType::FLOAT:
            try {
                return std::stof(value);
            } catch (...) {
                return 0.0f;
            }
        case ConfigType::DOUBLE:
            try {
                return std::stod(value);
            } catch (...) {
                return 0.0;
            }
        case ConfigType::BOOL:
            return (value == "true" || value == "1" || value == "yes");
        default:
            return value;
    }
}

bool ConfigManager::loadINI(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        line = utils::trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, eqPos));
            std::string value = utils::trim(line.substr(eqPos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && ((value[0] == '"' && value[value.size() - 1] == '"') ||
                                       (value[0] == '\'' && value[value.size() - 1] == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to register if not already registered
            if (options.find(key) == options.end()) {
                // Guess type based on value
                ConfigType guessedType = ConfigType::STRING;
                if (value == "true" || value == "false" || value == "1" || value == "0") {
                    guessedType = ConfigType::BOOL;
                } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                    guessedType = ConfigType::INT;
                } else if (std::regex_match(value, std::regex("^-?\d*\.\d+$"))) {
                    guessedType = ConfigType::FLOAT;
                }
                
                registerOption(key, "Configuration option from INI file", guessedType, false, value);
            }
            
            setString(key, value, "ini_file");
        }
    }
    
    file.close();
    return true;
}

bool ConfigManager::loadJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return importFromJSON(content);
}

bool ConfigManager::loadYAML(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return importFromYAML(content);
}

bool ConfigManager::saveINI(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[NLM Configuration]\n";
    file << "# Generated by NLM ConfigManager" << std::endl;
    file << "# Generated: " << utils::getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& pair : values) {
        file << pair.first << " = " << pair.second.rawValue << std::endl;
    }
    
    file.close();
    return true;
}

bool ConfigManager::saveJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << exportToJSON();
    file.close();
    return true;
}

bool ConfigManager::saveYAML(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << exportToYAML();
    file.close();
    return true;
}

std::string ConfigManager::getTemplateDir() const {
    return "templates/";
}

std::string ConfigManager::getTemplateFilePath(const std::string& name) const {
    std::string dir = getTemplateDir();
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    return dir + name + ".nlm";
}

// ConfigPresets implementation
ConfigManager ConfigPresets::getDefaultConfig() {
    ConfigManager config;
    
    config.setString("neuron_count", "1000");
    config.setString("region_count", "1");
    config.setString("connection_probability", "0.1");
    config.setString("simulation_timestep", "0.001");
    config.setString("random_seed", "42");
    config.setString("log_level", "INFO");
    config.setString("enable_development", "true");
    config.setString("enable_checkpointing", "true");
    config.setString("enable_replay", "true");
    
    return config;
}

ConfigManager ConfigPresets::getResearchConfig() {
    ConfigManager config = getDefaultConfig();
    
    config.setString("neuron_count", "5000");
    config.setString("region_count", "3");
    config.setString("connection_probability", "0.05");
    config.setString("log_level", "DEBUG");
    
    return config;
}

ConfigManager ConfigPresets::getProductionConfig() {
    ConfigManager config = getDefaultConfig();
    
    config.setString("neuron_count", "10000");
    config.setString("region_count", "2");
    config.setString("connection_probability", "0.15");
    config.setString("log_level", "WARN");
    config.setString("enable_development", "false");
    
    return config;
}

ConfigManager ConfigPresets::getDebugConfig() {
    ConfigManager config = getDefaultConfig();
    
    config.setString("neuron_count", "100");
    config.setString("region_count", "1");
    config.setString("simulation_timestep", "0.01");
    config.setString("random_seed", "12345");
    config.setString("log_level", "TRACE");
    
    return config;
}

ConfigManager ConfigPresets::getPerformanceConfig() {
    ConfigManager config = getDefaultConfig();
    
    config.setString("neuron_count", "2000");
    config.setString("region_count", "1");
    config.setString("connection_probability", "0.2");
    config.setString("log_level", "INFO");
    
    return config;
}

ConfigManager ConfigPresets::getMinimalConfig() {
    ConfigManager config;
    
    config.setString("neuron_count", "100");
    config.setString("region_count", "1");
    
    return config;
}

ConfigManager ConfigPresets::getFullConfig() {
    ConfigManager config = getDefaultConfig();
    
    config.setString("verbose", "true");
    config.setString("benchmark", "true");
    config.setString("profile", "true");
    config.setString("enable_development", "true");
    config.setString("enable_checkpointing", "true");
    config.setString("enable_replay", "true");
    
    return config;
}

ConfigManager ConfigPresets::getCloudConfig() {
    ConfigManager config = getProductionConfig();
    
    config.setString("output_file", "cloud_output.bin");
    config.setString("checkpoint_path", "/cloud/checkpoints/");
    
    return config;
}

bool ConfigPresets::loadPreset(ConfigManager& config, const std::string& presetName) {
    ConfigManager preset;
    
    if (presetName == "default") {
        preset = getDefaultConfig();
    } else if (presetName == "research") {
        preset = getResearchConfig();
    } else if (presetName == "production") {
        preset = getProductionConfig();
    } else if (presetName == "debug") {
        preset = getDebugConfig();
    } else if (presetName == "performance") {
        preset = getPerformanceConfig();
    } else if (presetName == "minimal") {
        preset = getMinimalConfig();
    } else if (presetName == "full") {
        preset = getFullConfig();
    } else if (presetName == "cloud") {
        preset = getCloudConfig();
    } else {
        return false;
    }
    
    config = preset;
    return true;
}

bool ConfigPresets::savePreset(const ConfigManager& config, const std::string& presetName) {
    return config.saveTemplate(presetName);
}

std::vector<std::string> ConfigPresets::getAvailablePresets() {
    return {"default", "research", "production", "debug", "performance", "minimal", "full", "cloud"};
}

// helpers namespace implementation
namespace helpers {

CLIConfigOptions parseCLIConfigOptions(int argc, char* argv[]) {
    CLIConfigOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.logLevel = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-V") {
            // Version handling
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg == "--profile") {
            options.profile = true;
        } else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.customOptions[key] = argv[++i];
            } else {
                options.customOptions[key] = "true";
            }
        }
    }
    
    return options;
}

std::map<std::string, std::string> loadEnvironmentConfig(const std::string& prefix) {
    std::map<std::string, std::string> result;
    
    // This would read environment variables in a real implementation
    // For now, return empty map
    return result;
}

bool validateConfigFile(const std::string& filename, ConfigFormat format) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFormat::INI:
            // Basic INI validation
            return true;
        case ConfigFormat::JSON:
            // Basic JSON validation
            return !filename.empty() && filename.substr(filename.size() - 5) == ".json";
        case ConfigFormat::YAML:
            // Basic YAML validation
            return !filename.empty() && (filename.substr(filename.size() - 5) == ".yaml" || 
                                       filename.substr(filename.size() - 4) == ".yml");
        default:
            return false;
    }
}

std::vector<std::string> getConfigValidationErrors(const std::string& filename, ConfigFormat format) {
    std::vector<std::string> errors;
    
    if (!validateConfigFile(filename, format)) {
        errors.push_back("Invalid configuration file");
    }
    
    return errors;
}

double compareConfigs(const ConfigManager& config1, const ConfigManager& config2) {
    std::vector<std::string> diff = config1.diff(config2);
    return diff.size() / (double)(config1.getOptionCount() + config2.getOptionCount());
}

std::vector<std::string> findConfigDifferences(const ConfigManager& config1, const ConfigManager& config2) {
    return config1.diff(config2);
}

std::string getConfigSummary(const ConfigManager& config) {
    std::stringstream ss;
    ss << "NLM Configuration Summary:" << std::endl;
    ss << "Options: " << config.getOptionCount() << std::endl;
    ss << "Set: " << config.getSetCount() << std::endl;
    ss << "Completeness: " << config.getCompleteness() << "%" << std::endl;
    ss << "Valid: " << (config.isValid() ? "Yes" : "No") << std::endl;
    
    if (!config.isValid()) {
        auto errors = config.getValidationErrors();
        if (!errors.empty()) {
            ss << std::endl << "Validation Errors:" << std::endl;
            for (const auto& error : errors) {
                ss << "  - " << error << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::string getConfigStatistics(const ConfigManager& config) {
    std::stringstream ss;
    ss << "NLM Configuration Statistics:" << std::endl;
    ss << "Total Options: " << config.getOptionCount() << std::endl;
    ss << "Set Options: " << config.getSetCount() << std::endl;
    ss << "Default Options: " << (config.getOptionCount() - config.getSetCount()) << std::endl;
    ss << "Validation Status: " << (config.isValid() ? "PASS" : "FAIL") << std::endl;
    
    return ss.str();
}

bool backupConfig(const ConfigManager& config, const std::string& backupFile) {
    return config.saveToFile(backupFile);
}

bool restoreConfig(ConfigManager& config, const std::string& backupFile) {
    return config.loadFromFile(backupFile);
}

} // namespace helpers

} // namespace config
} // namespace nlm
