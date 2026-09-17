#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <cstring>

// For JSON parsing
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, ConfigSource::File);
}

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    // First try to parse as JSON if available
#ifdef HAVE_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            const auto& val = *it;
            
            if (val.is_string()) {
                configValue = val.get<std::string>();
            } else if (val.is_boolean()) {
                configValue = val.get<bool>();
            } else if (val.is_number_integer()) {
                configValue = val.get<int64_t>();
            } else if (val.is_number_unsigned()) {
                configValue = val.get<uint64_t>();
            } else if (val.is_number()) {
                if (val.is_number_integer()) {
                    configValue = val.get<int64_t>();
                } else {
                    configValue = val.get<double>();
                }
            } else if (val.is_array()) {
                // Handle arrays (convert to std::vector)
                std::vector<std::variant<int, double, std::string>> arrayData;
                for (const auto& element : val) {
                    if (element.is_string()) {
                        arrayData.push_back(element.get<std::string>());
                    } else if (element.is_number_integer()) {
                        arrayData.push_back(element.get<int64_t>());
                    } else if (element.is_number()) {
                        arrayData.push_back(element.get<double>());
                    }
                }
                configValue = arrayData;
            } else {
                // Default to string for unknown types
                configValue = val.dump();
            }
            
            set(key, configValue, source);
        }
        return true;
    } catch (const std::exception& e) {
        // Fall back to simple key=value format
    }
#endif
    
    // Fallback to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty() || value.empty()) {
                continue; // Skip malformed lines
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                // Try to infer type from value string
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    set(key, boolVal, source);
                } else {
                    // Try integer parsing
                    bool isInt = true;
                    size_t posEnd;
                    try {
                        int64_t intVal = std::stoll(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, intVal, source);
                            continue;
                        }
                    } catch (...) {
                        isInt = false;
                    }
                    
                    // Try double parsing
                    bool isDouble = true;
                    try {
                        double doubleVal = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, doubleVal, source);
                            continue;
                        }
                    } catch (...) {
                        isDouble = false;
                    }
                    
                    // Default to string
                    set(key, value, source);
                }
            } catch (const std::exception& e) {
                // If parsing fails, store as string anyway
                set(key, value, source);
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, ConfigSource::CommandLine);
}

bool Config::loadFromArgs(int argc, char** argv, ConfigSource source) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!key.empty() && !value.empty()) {
                    set(key, value, source);
                } else {
                    success = false;
                }
            } else {
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!key.empty() && !value.empty()) {
                set(key, value, source);
            } else {
                success = false;
            }
        }
        // Handle --flag format (boolean true)
        else if (arg.substr(0, 2) == "--" && (arg.size() == 2 || arg.find('=') == std::string::npos)) {
            std::string key = arg.substr(2);
            if (!key.empty()) {
                set(key, true, source);
            } else {
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, false);
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n\n";
    
#ifdef HAVE_NLOHMANN_JSON
    // Use JSON format if available
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        std::string description = entry.description.empty() ? 
            std::string("Configuration key: ") + entry.key : 
            entry.description;
        
        // Escape newlines and quotes in description
        size_t pos = 0;
        while ((pos = description.find('\n', pos)) != std::string::npos) {
            description.replace(pos, 1, "\\n");
            pos += 2;
        }
        
        j[entry.key] = std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, uint64_t> ||
                                 std::is_same_v<T, int> ||
                                 std::is_same_v<T, unsigned> ||
                                 std::is_same_v<T, long> ||
                                 std::is_same_v<T, unsigned long>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double> || 
                                 std::is_same_v<T, float>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                // Convert to JSON array
                nlohmann::json array = arg;
                return array;
            } else {
                // For unknown types, convert to string
                std::ostringstream oss;
                oss << arg;
                return oss.str();
            }
        }, entry.value);
        
        // Add comment if available
        if (!entry.description.empty()) {
            // Store description separately for better readability
            j[entry.key + "_comment"] = entry.description;
        }
    }
    
    if (prettyPrint) {
        file << j.dump(2);
    } else {
        file << j.dump();
    }
#else
    // Fallback to simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, uint64_t> ||
                                 std::is_same_v<T, int> ||
                                 std::is_same_v<T, unsigned> ||
                                 std::is_same_v<T, long> ||
                                 std::is_same_v<T, unsigned long>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double> || 
                                 std::is_same_v<T, float>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "\"" << arg << "\"";
            }
        }, entry.value);
        file << "\n\n";
    }
#endif
    
    return true;
}

// Template implementation for get
grouped by type

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

// Template implementation for getOr
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Template implementation for setTyped
template<typename T>
void Config::setTyped(const std::string& key, const T& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Implementation of getKeys with source filtering
std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Implementation of summary with source filtering
std::string Config::summary(ConfigSource source) const {
    std::ostringstream oss;
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    oss << "Configuration (" << count << " entries from " << static_cast<int>(source) << "):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
    }
    return oss.str();
}

// Implementation of parseValue and formatValue helpers
bool Config::parseValue(const std::string& str, ConfigValue& value) {
    // Try boolean
    if (str == "true") {
        value = true;
        return true;
    } else if (str == "false") {
        value = false;
        return true;
    }
    
    // Try integer
    size_t pos;
    try {
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try unsigned integer
    try {
        uint64_t uintVal = std::stoull(str, &pos);
        if (pos == str.size()) {
            value = uintVal;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try float
    try {
        float floatVal = std::stof(str, &pos);
        if (pos == str.size()) {
            value = floatVal;
            return true;
        }
    } catch (...) {}
    
    // Try array format [a,b,c]
    if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<std::variant<int, double, std::string>> arrayData;
        
        size_t start = 0;
        size_t posEnd;
        bool validArray = true;
        
        while (start < inner.size()) {
            size_t commaPos = inner.find(',', start);
            size_t endPos;
            
            if (commaPos == std::string::npos) {
                endPos = inner.size();
            } else {
                endPos = commaPos;
            }
            
            std::string elementStr = inner.substr(start, endPos - start);
            
            // Trim whitespace
            size_t elementStart = elementStr.find_first_not_of(" \t\r\n");
            size_t elementEnd = elementStr.find_last_not_of(" \t\r\n");
            if (elementStart == std::string::npos || elementEnd == std::string::npos) {
                validArray = false;
                break;
            }
            
            elementStr = elementStr.substr(elementStart, elementEnd - elementStart + 1);
            
            // Try to parse element
            ConfigValue elementValue;
            if (parseValue(elementStr, elementValue)) {
                // Convert to appropriate type for array
                if (std::holds_alternative<int64_t>(elementValue) || std::holds_alternative<uint64_t>(elementValue)) {
                    arrayData.push_back(static_cast<int>(std::get<int64_t>(elementValue)));
                } else if (std::holds_alternative<double>(elementValue) || std::holds_alternative<float>(elementValue)) {
                    arrayData.push_back(std::get<double>(elementValue));
                } else if (std::holds_alternative<bool>(elementValue) || std::holds_alternative<int>(elementValue)) {
                    arrayData.push_back(static_cast<int>(std::get<bool>(elementValue)));
                } else if (std::holds_alternative<std::string>(elementValue)) {
                    arrayData.push_back(std::get<std::string>(elementValue));
                }
            } else {
                validArray = false;
                break;
            }
            
            start = commaPos == std::string::npos ? inner.size() : commaPos + 1;
            
            // Skip whitespace after comma
            while (start < inner.size() && (inner[start] == ' ' || inner[start] == '\t' || inner[start] == '\r' || inner[start] == '\n')) {
                start++;
            }
        }
        
        if (validArray) {
            value = arrayData;
            return true;
        }
    }
    
    // Default to string
    value = str;
    return true; // Always succeed, fall back to string
}

std::string Config::formatValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes and special characters
            std::string result = arg;
            // Replace " with \" and handle newlines
            size_t pos = 0;
            while ((pos = result.find('"', pos)) != std::string::npos) {
                result.replace(pos, 1, "\\\"");
                pos += 2;
            }
            pos = 0;
            while ((pos = result.find('\n', pos)) != std::string::npos) {
                result.replace(pos, 1, "\\n");
                pos += 2;
            }
            pos = 0;
            while ((pos = result.find('\r', pos)) != std::string::npos) {
                result.replace(pos, 1, "\\r");
                pos += 2;
            }
            pos = 0;
            while ((pos = result.find('\t', pos)) != std::string::npos) {
                result.replace(pos, 1, "\\t");
                pos += 2;
            }
            return "\"" + result + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t> ||
                             std::is_same_v<T, int> ||
                             std::is_same_v<T, unsigned> ||
                             std::is_same_v<T, long> ||
                             std::is_same_v<T, unsigned long>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double> || 
                             std::is_same_v<T, float>) {
            // Use default precision for doubles, fixed for floats
            if constexpr (std::is_same_v<T, float>) {
                return std::to_string(arg);
            } else {
                return std::to_string(arg);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            // For unknown types, use string conversion
            std::ostringstream oss;
            oss << arg;
            return oss.str();
        }
    }, value);
}

// Implementation of isNumeric, isInteger, isBoolean helpers
bool Config::isNumeric(const std::string& str, double* value) {
    try {
        double val = std::stod(str);
        if (value) *value = val;
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isInteger(const std::string& str, int64_t* value) {
    try {
        int64_t val = std::stoll(str);
        if (value) *value = val;
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isBoolean(const std::string& str, bool* value) {
    if (str == "true") {
        if (value) *value = true;
        return true;
    } else if (str == "false") {
        if (value) *value = false;
        return true;
    }
    return false;
}

// Implementation of remaining methods
void Config::removeAll() {
    pImpl->entries.clear();
}

void Config::clear(ConfigSource source) {
    if (source == ConfigSource::Runtime) {
        pImpl->entries.clear();
    } else {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [source](const ConfigEntry& e) { return e.source == source; }),
            pImpl->entries.end()
        );
    }
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        ConfigEntry newEntry = entry;
        newEntry.source = source; // Override with new source
        
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry
            it->value = entry.value;
            it->source = source;
            if (!entry.description.empty()) {
                it->description = entry.description;
            }
        } else {
            // Add new entry
            pImpl->entries.push_back(newEntry);
        }
    }
}

bool Config::validate() const {
    return validationErrors().empty();
}

std::string Config::validationErrors() const {
    std::string errors;
    
    // Check for duplicate keys
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(keys.begin(), keys.end(), entry.key) != keys.end()) {
            if (!errors.empty()) errors += "\n";
            errors += "Duplicate key: " + entry.key;
        } else {
            keys.push_back(entry.key);
        }
    }
    
    // Validate individual keys and values
    for (const auto& entry : pImpl->entries) {
        ConfigError keyError = validateKey(entry.key);
        if (keyError != ConfigError::Success) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid key '" + entry.key + "': " + std::to_string(static_cast<int>(keyError));
        }
        
        ConfigError valueError = validateValue(entry.value);
        if (valueError != ConfigError::Success) {
            if (!errors.empty()) errors += "\n";
            errors += "Invalid value for key '" + entry.key + "': " + std::to_string(static_cast<int>(valueError));
        }
    }
    
    return errors;
}

size_t Config::entryCount() const {
    return pImpl->entries.size();
}

size_t Config::entryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

Config Config::clone(ConfigSource source) const {
    Config cloned;
    for (const auto& entry : pImpl->entries) {
        if (source == ConfigSource::Runtime || entry.source == source) {
            cloned.set(entry.key, entry.value, entry.source);
        }
    }
    return cloned;
}

Config Config::createDefault() {
    Config config;
    
    // Set common default configuration values
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", 42);
    config.set("neuron_count", 500);
    config.set("region_count", 1);
    config.set("connection_probability", 0.15f);
    config.set("max_energy", 100.0);
    config.set("energy_decay_rate", 0.01);
    config.set("plasticity_rate", 0.1);
    
    return config;
}

Config Config::createFromEnvironment() {
    Config config;
    
    // Check environment variables for configuration
    const char* envTimestep = std::getenv("NLM_SIMULATION_TIMESTEP");
    if (envTimestep) {
        try {
            double timestep = std::stod(envTimestep);
            config.set("simulation_timestep", timestep, ConfigSource::Environment);
        } catch (...) {
            // Ignore parsing errors, use default
        }
    }
    
    const char* envSeed = std::getenv("NLM_RANDOM_SEED");
    if (envSeed) {
        try {
            int64_t seed = std::stoll(envSeed);
            config.set("random_seed", seed, ConfigSource::Environment);
        } catch (...) {
            // Ignore parsing errors, use default
        }
    }
    
    const char* envNeurons = std::getenv("NLM_NEURON_COUNT");
    if (envNeurons) {
        try {
            int64_t count = std::stoll(envNeurons);
            config.set("neuron_count", count, ConfigSource::Environment);
        } catch (...) {
            // Ignore parsing errors, use default
        }
    }
    
    return config;
}

Config Config::createFromCommandLine(int argc, char** argv) {
    Config config;
    config.loadFromArgs(argc, argv, ConfigSource::CommandLine);
    return config;
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-')) {
            return false;
        }
    }
    
    return true;
}

ConfigError Config::validateKey(const std::string& key) {
    if (key.empty()) {
        return ConfigError::InvalidFormat;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-')) {
            return ConfigError::InvalidFormat;
        }
    }
    
    return ConfigError::Success;
}

ConfigError Config::validateValue(const ConfigValue& value) {
    // Check for null/empty values where appropriate
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                return ConfigError::InvalidFormat;
            }
        }
    }, value);
    
    return ConfigError::Success;
}

void Config::cleanup() {
    // Clean up any internal resources
    // Currently nothing to do, but reserved for future enhancements
}

namespace nlm {
    // Explicit template instantiations for get
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<float> Config::get<float>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    // Explicit template instantiations for getOr
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template float Config::getOr<float>(const std::string&, const float&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    
    // Explicit template instantiations for setTyped
    template void Config::setTyped<int>(const std::string&, const int&, ConfigSource);
    template void Config::setTyped<int64_t>(const std::string&, const int64_t&, ConfigSource);
    template void Config::setTyped<uint64_t>(const std::string&, const uint64_t&, ConfigSource);
    template void Config::setTyped<double>(const std::string&, const double&, ConfigSource);
    template void Config::setTyped<float>(const std::string&, const float&, ConfigSource);
    template void Config::setTyped<bool>(const std::string&, const bool&, ConfigSource);
    template void Config::setTyped<std::string>(const std::string&, const std::string&, ConfigSource);
}
