#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        // Convert to lowercase for comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    try {
        // Try JSON format first
        if (ext == "json" || ext.empty()) {
            return loadFromJSON(filepath);
        }
        // Try YAML format if yaml-cpp is available
        else if (ext == "yaml" || ext == "yml") {
            return loadFromYAML(filepath);
        }
        else {
            // Fallback to simple key=value format for backward compatibility
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        // Log error and return false
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine file type based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        // Convert to lowercase for comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    try {
        // Try JSON format first
        if (ext == "json" || ext.empty()) {
            return saveToJSON(filepath);
        }
        // Try YAML format if yaml-cpp is available
        else if (ext == "yaml" || ext == "yml") {
            return saveToYAML(filepath);
        }
        else {
            // Fallback to simple key=value format for backward compatibility
            return saveToSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        
        // Process entries
        for (const auto& entry : pImpl->entries) {
            // Create JSON entry with metadata
            nlohmann::json jsonEntry;
            jsonEntry["value"] = configValueToJSON(entry.value);
            jsonEntry["source"] = static_cast<int>(entry.source);
            
            if (!entry.description.empty()) {
                jsonEntry["description"] = entry.description;
            }
            
            jsonConfig[entry.key] = jsonEntry;
        }
        
        // Write formatted JSON
        file << jsonConfig.dump(4); // 4 spaces indentation
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        // TODO: Implement YAML saving if yaml-cpp is available
        // For now, fallback to simple format
        return saveToSimpleFormat(filepath);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
    }
    
    return true;
}

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
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
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
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON entries
        if (jsonConfig.is_object()) {
            for (const auto& [key, value] : jsonConfig.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                if (validateValueType(configValue, key)) {
                    set(key, configValue, ConfigSource::File);
                } else {
                    // Log validation error but continue processing other entries
                    return false;
                }
            }
        } else {
            // Invalid JSON structure
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log JSON parsing error
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        // TODO: Implement YAML parsing if yaml-cpp is available
        // For now, fallback to simple format
        return loadFromSimpleFormat(filepath);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing entries
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                // Try to convert string value to appropriate type
                if (value.find('.') != std::string::npos) {
                    // Try double first
                    try {
                        double doubleValue = std::stod(value);
                        set(key, doubleValue, ConfigSource::File);
                    } catch (...) {
                        // Fall back to string
                        set(key, value, ConfigSource::File);
                    }
                } else if (value == "true" || value == "false") {
                    // Boolean value
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                } else {
                    // Try integer
                    try {
                        int intValue = std::stoi(value);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        // Fall back to string
                        set(key, value, ConfigSource::File);
                    }
                }
            } catch (const std::exception& e) {
                // Log error and continue
                continue;
            }
        } else {
            // Malformed line - log error but continue
            continue;
        }
    }
    
    return true;
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&jsonValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    
    return jsonValue;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue configValue;
    
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Determine array element type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_string()) {
                std::vector<std::string> strVec = jsonValue.get<std::vector<std::string>>();
                configValue = strVec;
            } else if (jsonValue[0].is_number_integer()) {
                std::vector<int> intVec = jsonValue.get<std::vector<int>>();
                configValue = intVec;
            } else if (jsonValue[0].is_number_float()) {
                std::vector<double> doubleVec = jsonValue.get<std::vector<double>>();
                configValue = doubleVec;
            }
        }
    }
    
    return configValue;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

bool Config::validateValueType(const ConfigValue& value, const std::string& key) {
    // Basic validation - could be enhanced with more sophisticated schema validation
    return true;
}
