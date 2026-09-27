// Include necessary headers for advanced configuration support
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <numeric>

namespace nlm {

// JSON parsing implementation
bool Config::parseJSONConfig(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            const auto& value = *it;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Parse array
                if (value.empty()) {
                    configValue = std::vector<double>();
                } else {
                    // Check array element type
                    bool allNumbers = std::all_of(value.begin(), value.end(), 
                                                 [](const auto& v) { return v.is_number(); });
                    bool allIntegers = std::all_of(value.begin(), value.end(), 
                                                  [](const auto& v) { return v.is_number_integer(); });
                    
                    if (allNumbers && !allIntegers) {
                        std::vector<double> vec;
                        for (const auto& v : value) {
                            vec.push_back(v.get<double>());
                        }
                        configValue = vec;
                    } else if (allIntegers) {
                        std::vector<int64_t> vec;
                        for (const auto& v : value) {
                            vec.push_back(v.get<int64_t>());
                        }
                        configValue = vec;
                    } else {
                        std::vector<std::string> vec;
                        for (const auto& v : value) {
                            vec.push_back(v.get<std::string>());
                        }
                        configValue = vec;
                    }
                }
            } else {
                // Skip complex objects for now
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_DEBUG("JSON parsing failed: " + std::string(e.what()));
        return false;
    }
}

// YAML parsing implementation
bool Config::parseYAMLConfig(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        
        for (const auto& it : yaml) {
            std::string key = it.first.as<std::string>();
            const YAML::Node& value = it.second;
            
            // Convert YAML node to ConfigValue
            ConfigValue configValue;
            
            if (value.IsScalar()) {
                // Check scalar type
                if (value.IsInt()) {
                    configValue = value.as<int64_t>();
                } else if (value.IsFloat()) {
                    configValue = value.as<double>();
                } else if (value.IsBool()) {
                    configValue = value.as<bool>();
                } else {
                    configValue = value.as<std::string>();
                }
            } else if (value.IsSequence()) {
                // Parse sequence (array)
                bool allNumbers = std::all_of(value.begin(), value.end(),
                                             [](const YAML::Node& n) { return n.IsScalar() && (n.IsInt() || n.IsFloat()); });
                bool allBooleans = std::all_of(value.begin(), value.end(),
                                               [](const YAML::Node& n) { return n.IsBool(); });
                
                if (allNumbers) {
                    std::vector<double> vec;
                    for (const auto& v : value) {
                        if (v.IsInt()) {
                            vec.push_back(static_cast<double>(v.as<int64_t>()));
                        } else {
                            vec.push_back(v.as<double>());
                        }
                    }
                    configValue = vec;
                } else if (allBooleans) {
                    std::vector<bool> vec;
                    for (const auto& v : value) {
                        vec.push_back(v.as<bool>());
                    }
                    configValue = vec;
                } else {
                    std::vector<std::string> vec;
                    for (const auto& v : value) {
                        vec.push_back(v.as<std::string>());
                    }
                    configValue = vec;
                }
            } else {
                // Skip complex mappings for now
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_DEBUG("YAML parsing failed: " + std::string(e.what()));
        return false;
    }
}

// Enhanced saveToFile method with JSON support
bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Write in JSON format for better readability and interoperability
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            
            // Convert ConfigValue to JSON representation
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t> || 
                                     std::is_same_v<T, uint64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::setprecision(6) << val;
                } else if constexpr (std::is_same_v<T, std::vector<int> ||
                                     std::is_same_v<T, std::vector<int64_t> ||
                                     std::is_same_v<T, std::vector<uint64_t> ||
                                     std::is_same_v<T, std::vector<double> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    bool vecFirst = true;
                    for (const auto& elem : val) {
                        if (!vecFirst) file << ", ";
                        vecFirst = false;
                        
                        if constexpr (std::is_same_v<T, std::vector<std::string>) {
                            file << "\"" << elem << "\"";
                        } else {
                            file << elem;
                        }
                    }
                    file << "]";
                }
            }, entry.value);
        }
        
        file << "\n}\n";
        file.flush();
        
        if (!file.good()) {
            NLM_LOG_ERROR("Error writing to config file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving config file " + filepath + ": " + e.what());
        return false;
    }
}

// Helper method to convert string to appropriate type
void Config::setConfigValue(const std::string& key, const std::string& value) {
    try {
        // Try to parse as boolean
        if (value == "true" || value == "false") {
            set(key, value == "true", ConfigSource::File);
            return;
        }
        
        // Try to parse as integer
        size_t pos;
        try {
            if (value.find('.') == std::string::npos) {
                int64_t intVal = std::stoll(value);
                set(key, intVal, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            double doubleVal = std::stod(value);
            set(key, doubleVal, ConfigSource::File);
            return;
        } catch (...) {}
        
        // Default to string
        set(key, value, ConfigSource::File);
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to parse config value for key '" + key + "': " + e.what());
        // Set as string as fallback
        set(key, value, ConfigSource::File);
    }
}

// Enhanced summary method
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Validate configuration method
bool Config::validate() const {
    // Basic validation - can be extended with specific validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_ERROR("Config entry has empty key");
            return false;
        }
        
        // Validate specific values if needed
        // Example: validate that certain keys have valid ranges
    }
    
    return true;
}

// Merge configuration method
void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Override existing entries, add new ones
        set(entry.key, entry.value, source);
    }
}

// Get configuration statistics
ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stats.stringValues++;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.boolValues++;
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, uint64_t>) {
                stats.intValues++;
            } else if constexpr (std::is_same_v<T, double>) {
                stats.floatValues++;
            } else if constexpr (std::is_same_v<T, std::vector<int> ||
                                 std::is_same_v<T, std::vector<int64_t> ||
                                 std::is_same_v<T, std::vector<uint64_t> ||
                                 std::is_same_v<T, std::vector<double> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                stats.arrayValues++;
            }
        }, entry.value);
    }
    
    return stats;
}

// Batch operations
void Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

// Copy operations
void Config::copyFrom(const Config& other) {
    pImpl->entries = other.pImpl->entries;
}

void Config::copyTo(Config& other) const {
    other.clear();
    other.pImpl->entries = pImpl->entries;
}

// Deep copy implementation
Config Config::clone() const {
    Config result;
    result.copyFrom(*this);
    return result;
}

// Comparison operators
bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

// Enhanced loadFromArgs with better error handling
bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Malformed command line argument (missing '='): " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Unknown command line argument format: " + arg);
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading from command line args: " + std::string(e.what()));
        return false;
    }
}

// Validate specific key type
bool Config::validateKeyType(const std::string& key, std::type_index typeIndex) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    // Check if the stored value matches the expected type
    const auto& value = it->value;
    
    if (typeIndex == typeid(int) || typeIndex == typeid(int64_t) || typeIndex == typeid(uint64_t)) {
        return std::holds_alternative<int>(value) || 
               std::holds_alternative<int64_t>(value) ||
               std::holds_alternative<uint64_t>(value);
    } else if (typeIndex == typeid(double)) {
        return std::holds_alternative<double>(value);
    } else if (typeIndex == typeid(bool)) {
        return std::holds_alternative<bool>(value);
    } else if (typeIndex == typeid(std::string)) {
        return std::holds_alternative<std::string>(value);
    } else if (typeIndex == typeid(std::vector<int>) ||
               typeIndex == typeid(std::vector<int64_t>) ||
               typeIndex == typeid(std::vector<uint64_t>) ||
               typeIndex == typeid(std::vector<double>) ||
               typeIndex == typeid(std::vector<std::string>)) {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<int64_t>>(value) ||
               std::holds_alternative<std::vector<uint64_t>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return false;
}

// Get configuration keys matching pattern
std::vector<std::string> Config::getKeysMatching(const std::string& pattern) const {
    std::vector<std::string> matchingKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (matchesPattern(entry.key, pattern)) {
            matchingKeys.push_back(entry.key);
        }
    }
    
    return matchingKeys;
}

// Pattern matching helper
bool Config::matchesPattern(const std::string& key, const std::string& pattern) const {
    // Simple wildcard matching - can be enhanced with regex
    std::string::size_type star = 0;
    std::string::size_type keyPos = 0;
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '*') {
            if (i + 1 < pattern.size()) {
                star = i + 1;
                keyPos = key.find_first_of(pattern.substr(i + 1), 0);
                if (keyPos == std::string::npos) {
                    keyPos = key.size();
                }
            } else {
                // '*' at the end matches everything
                return true;
            }
        } else if (keyPos < key.size() && pattern[i] == key[keyPos]) {
            ++keyPos;
        } else if (star > 0) {
            // Try next position
            keyPos = key.find_first_of(pattern.substr(star), keyPos);
            if (keyPos == std::string::npos) {
                keyPos = key.size();
            }
            i = star - 1; // Re-process this position
        } else {
            return false;
        }
    }
    
    return keyPos == key.size();
}

// Get configuration value as string
std::string Config::getValueAsString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, std::vector<int> ||
                             std::is_same_v<T, std::vector<int64_t> ||
                             std::is_same_v<T, std::vector<uint64_t> ||
                             std::is_same_v<T, std::vector<double> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& elem : val) {
                if (!first) oss << ", ";
                first = false;
                if constexpr (std::is_same_v<T, std::vector<std::string>) {
                    oss << "\"" << elem << "\"";
                } else {
                    oss << elem;
                }
            }
            oss << "]";
        }
    }, it->value);
    
    return oss.str();
}

// Get all configuration values as vector of strings
std::vector<std::pair<std::string, std::string>> Config::getAllAsStrings() const {
    std::vector<std::pair<std::string, std::string>> result;
    
    for (const auto& entry : pImpl->entries) {
        result.emplace_back(entry.key, getValueAsString(entry.key));
    }
    
    return result;
}

} // namespace nlm

// Explicit template instantiations for Config::get and Config::getOr
namespace nlm {

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

