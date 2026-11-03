#include "Config.hpp"
#include "ConfigurationTemplate.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool dirty = false;
    std::unordered_map<std::string, Source> sourceMap;
    std::unordered_map<std::string, std::string> descriptionMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    setDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setDefaults() {
    // Set default configuration values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
}

// IConfiguration interface implementation
void Config::set(const std::string& key, const std::string& value) {
    set(key, ConfigValue(value), ConfigSource::Runtime);
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto value = getAs<std::string>(key);
    return value ? std::optional<std::string>(*value) : std::nullopt;
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto value = getAs<int>(key);
    return value ? std::optional<int>(*value) : std::nullopt;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto value = getAs<double>(key);
    return value ? std::optional<double>(*value) : std::nullopt;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto value = getAs<bool>(key);
    return value ? std::optional<bool>(*value) : std::nullopt;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto value = getAs<std::string>(key);
    return value ? *value : defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto value = getAs<int>(key);
    return value ? *value : defaultValue;
}


double Config::getDouble(const std::string& key, double defaultValue) const {
    auto value = getAs<double>(key);
    return value ? *value : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto value = getAs<bool>(key);
    return value ? *value : defaultValue;
}

bool Config::has(const std::string& key) const {
    return getAs<std::string>(key).has_value();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        pImpl->dirty = true;
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->dirty = true;
    setDefaults();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->sourceMap.find(key);
    return it != pImpl->sourceMap.end() ? static_cast<ConfigSource>(it->second) : ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptionMap.find(key);
    return it != pImpl->descriptionMap.end() ? it->second : "";
}

std::shared_ptr<IConfiguration> Config::getSection(const std::string& section) const {
    // For now, return null - can be implemented for hierarchical config
    return nullptr;
}

void Config::setSection(const std::string& section, const std::shared_ptr<IConfiguration>& config) {
    // For now, no-op - can be implemented for hierarchical config
}

void Config::addChangeCallback(const ConfigurationChangeCallback& callback) {
    // For now, no-op - can be implemented for change notification
}

void Config::removeChangeCallback(const ConfigurationChangeCallback& callback) {
    // For now, no-op - can be implemented for change notification
}

std::string Config::toString() const {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) ss << ", ";
        ss << "\"" << entry.key << "\": ";
        
        if (std::holds_alternative<std::string>(entry.value)) {
            ss << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<int>(entry.value)) {
            ss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            ss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            ss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            ss << (std::get<bool>(entry.value) ? "true" : "false");
        }
        
        first = false;
    }
    ss << "}";
    return ss.str();
}

bool Config::fromString(const std::string& data) {
    try {
        // Simple JSON-like parsing for basic types
        if (data.empty() || data.front() != '{' || data.back() != '}') {
            return false;
        }
        
        // Remove braces and parse key-value pairs
        std::string content = data.substr(1, data.size() - 2);
        size_t pos = 0;
        
        while (pos < content.size()) {
            // Find key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value separator
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Skip whitespace
            while (colonPos < content.size() && 
                   (content[colonPos] == ' ' || content[colonPos] == '\t')) {
                colonPos++;
            }
            
            // Find value end
            size_t valueStart = colonPos;
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            
            // Trim value
            valueStr = trim(valueStr);
            
            // Parse value based on content
            if (valueStr.front() == '"' && valueStr.back() == '"') {
                // String value
                valueStr = valueStr.substr(1, valueStr.size() - 2);
                set(key, valueStr, ConfigSource::Runtime);
            } else if (valueStr == "true" || valueStr == "false") {
                // Boolean value
                bool boolValue = (valueStr == "true");
                set(key, boolValue, ConfigSource::Runtime);
            } else {
                // Try numeric value
                try {
                    if (valueStr.find('.') != std::string::npos) {
                        // Double value
                        double doubleValue = std::stod(valueStr);
                        set(key, doubleValue, ConfigSource::Runtime);
                    } else {
                        // Integer value
                        int intValue = std::stoi(valueStr);
                        set(key, intValue, ConfigSource::Runtime);
                    }
                } catch (...) {
                    // Ignore parsing errors for now
                }
            }
            
            // Move to next key-value pair
            pos = valueEnd;
            while (pos < content.size() && (content[pos] == ',' || content[pos] == ' ' || content[pos] == '\t')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::equals(const IConfiguration& other) const {
    const Config* otherConfig = dynamic_cast<const Config*>(&other);
    if (!otherConfig) {
        return false;
    }
    
    return pImpl->entries.size() == otherConfig->pImpl->entries.size();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total entries: " << pImpl->entries.size() << std::endl;
    ss << "Sources:" << std::endl;
    
    std::unordered_map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    for (const auto& pair : sourceCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    ss << std::endl << "Keys:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            ss << " - " << entry.description;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

bool Config::validate() const {
    // Basic validation - can be extended with more sophisticated rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            return false;
        }
        
        // Validate value based on type
        if (std::holds_alternative<std::string>(entry.value)) {
            // String values are always valid
        } else if (std::holds_alternative<int>(entry.value)) {
            int val = std::get<int>(entry.value);
            if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max()) {
                return false;
            }
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            int64_t val = std::get<int64_t>(entry.value);
            if (val < std::numeric_limits<int64_t>::min() || val > std::numeric_limits<int64_t>::max()) {
                return false;
            }
        } else if (std::holds_alternative<double>(entry.value)) {
            double val = std::get<double>(entry.value);
            if (std::isnan(val) || std::isinf(val)) {
                return false;
            }
        } else if (std::holds_alternative<bool>(entry.value)) {
            // Boolean values are always valid
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_WARNING("Configuration file does not exist: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            NLM_LOG_ERROR("Configuration path is not a file: " + filepath);
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        // Set file modification time for change detection
        auto fileTime = std::filesystem::last_write_time(filepath);
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with error handling
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING(std::string("Invalid configuration line " + std::to_string(lineNumber) + ": missing '='"));
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("Invalid configuration line " + std::to_string(lineNumber) + ": empty key"));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value
            if (!validateConfigValue(key, value)) {
                NLM_LOG_WARNING(std::string("Invalid configuration value for key '" + key + "' at line " + std::to_string(lineNumber)));
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
            // Set description if not already set
            if (getDescription(key).empty()) {
                setDescription(key, "Loaded from file: " + filepath);
            }
        }
        
        pImpl->dirty = true;
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc <= 1) {
            return true; // No arguments to process
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            size_t argLineNumber = i;
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (!validateConfigValue(key, value)) {
                        NLM_LOG_WARNING(std::string("Invalid command line argument: --" + key + "=" + value));
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                    setDescription(key, "Command line argument");
                } else {
                    NLM_LOG_WARNING(std::string("Invalid command line argument format: " + arg));
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (!validateConfigValue(key, value)) {
                    NLM_LOG_WARNING(std::string("Invalid command line argument: -" + key + " " + value));
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                setDescription(key, "Command line argument");
            } else {
                NLM_LOG_WARNING(std::string("Unrecognized command line argument: " + arg));
            }
        }
        
        pImpl->dirty = true;
        NLM_LOG_INFO("Successfully loaded configuration from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from command line: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Check if directory exists and is writable
        std::filesystem::path path(filepath);
        std::filesystem::path dir = path.parent_path();
        
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            NLM_LOG_ERROR("Configuration directory does not exist: " + dir.string());
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        // Write configuration with proper formatting
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on: " << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << " (Source: " << static_cast<int>(entry.source) << ")" << std::endl;
            
            // Write key-value pair
            file << entry.key << " = ";
            
            if (std::holds_alternative<std::string>(entry.value)) {
                file << "\"" << std::get<std::string>(entry.value) << "\"";
            } else if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            }
            
            file << std::endl << std::endl;
        }
        
        file.flush();
        
        // Verify file was written correctly
        if (file.fail()) {
            NLM_LOG_ERROR("Failed to write configuration to file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file: " + std::string(e.what()));
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        pImpl->dirty = true;
    } else {
        pImpl->entries.push_back(ConfigEntry(key, value, source));
        pImpl->sourceMap[key] = static_cast<Source>(source);
        pImpl->dirty = true;
    }
}

void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptionMap[key] = description;
}

bool Config::validateConfigValue(const std::string& key, const std::string& value) const {
    // Basic validation rules
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters in key
    if (key.find_first_of(" \"':;(){}[]<>!@#$%^&*+=?/~`|\\") != std::string::npos) {
        return false;
    }
    
    // Basic value validation based on key patterns
    if (key == "random_seed") {
        try {
            int64_t seed = std::stoll(value);
            return seed > 0;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return timestep > 0.0 && timestep < 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "neuron_count") {
        try {
            int64_t count = std::stoll(value);
            return count > 0 && count < 1000000;
        } catch (...) {
            return false;
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            return prob >= 0.0 && prob <= 1.0;
        } catch (...) {
            return false;
        }
    }
    
    // For other keys, accept any non-empty value
    return !value.empty();
}

std::shared_ptr<ConfigurationTemplate> Config::getTemplateImpl() const {
    auto templateImpl = std::make_shared<ConfigurationTemplate>();
    
    for (const auto& entry : pImpl->entries) {
        std::string stringValue;
        if (std::holds_alternative<std::string>(entry.value)) {
            stringValue = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<int>(entry.value)) {
            stringValue = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            stringValue = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            stringValue = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            stringValue = std::get<bool>(entry.value) ? "true" : "false";
        }
        
        templateImpl->set(entry.key, stringValue);
    }
    
    return templateImpl;
}

// Template method implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return getAs<T>(key);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = getAs<T>(key);
    return value ? *value : defaultValue;
}

} // namespace nlm
