#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>

// JSON library support
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// YAML library support
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Convert string value to appropriate ConfigValue type
static ConfigValue stringToConfigValue(const std::string& str) {
    // Trim the string first to remove whitespace
    std::string value = trim(str);
    
    // Check for boolean values (case-insensitive)
    if (value == "true" || value == "TRUE") {
        return true;
    }
    if (value == "false" || value == "FALSE") {
        return false;
    }
    
    // Check for numeric values
    try {
        // Try integer (including int64_t)
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            return intVal;
        }
    } catch (const std::exception&) {}
    
    try {
        // Try double
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            return doubleVal;
        }
    } catch (const std::exception&) {}
    
    // Default to string (already quoted in .cfg files)
    return value;
}

// Load and parse JSON configuration file
static bool loadJsonConfig(const std::string& filepath, std::vector<ConfigEntry>& entries) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        for (const auto& [key, value] : json.items()) {
            ConfigValue configValue;
            
            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Handle arrays by converting each element to string first
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    } else if (item.is_number_integer()) {
                        stringArray.push_back(std::to_string(item.get<int64_t>()));
                    } else if (item.is_number_float()) {
                        stringArray.push_back(std::to_string(item.get<double>()));
                    } else if (item.is_boolean()) {
                        stringArray.push_back(item.get<bool>() ? "true" : "false");
                    }
                }
                
                // Try to determine array element type (same logic as JSON)
                if (!stringArray.empty()) {
                    bool allInts = true;
                    bool allFloats = true;
                    bool allStrings = true;
                    
                    for (const auto& str : stringArray) {
                        try {
                            size_t pos;
                            std::stoll(str, &pos);
                            if (pos != str.size()) allInts = false;
                        } catch (...) {
                            allInts = false;
                        }
                        
                        try {
                            size_t pos;
                            std::stod(str, &pos);
                            if (pos != str.size()) allFloats = false;
                        } catch (...) {
                            allFloats = false;
                        }
                        
                        if (str != "true" && str != "false") {
                            allStrings = false;
                        }
                    }
                    
                    if (allInts) {
                        std::vector<int64_t> intArray;
                        for (const auto& str : stringArray) {
                            intArray.push_back(std::stoll(str));
                        }
                        configValue = intArray;
                    } else if (allFloats) {
                        std::vector<double> doubleArray;
                        for (const auto& str : stringArray) {
                            doubleArray.push_back(std::stod(str));
                        }
                        configValue = doubleArray;
                    } else {
                        configValue = stringArray;
                    }
                }
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Handle JSON parsing errors (invalid JSON format)
        return false;
    }
#else
    // JSON library not available - cannot parse JSON files
    return false;
#endif
}

// Load and parse YAML configuration file
static bool loadYamlConfig(const std::string& filepath, std::vector<ConfigEntry>& entries) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        
        for (const auto& node : yaml) {
            std::string key = node.first.as<std::string>();
            ConfigValue configValue;
            
            if (node.second.IsScalar()) {
                const std::string& value = node.second.as<std::string>();
                configValue = stringToConfigValue(value);
            } else if (node.second.IsSequence()) {
                // Handle YAML arrays
                std::vector<std::string> stringArray;
                for (const auto& item : node.second) {
                    if (item.IsScalar()) {
                        stringArray.push_back(item.as<std::string>());
                    }
                }
                
                // Try to determine array element type (same logic as JSON)
                if (!stringArray.empty()) {
                    bool allInts = true;
                    bool allFloats = true;
                    bool allStrings = true;
                    
                    for (const auto& str : stringArray) {
                        try {
                            size_t pos;
                            std::stoll(str, &pos);
                            if (pos != str.size()) allInts = false;
                        } catch (...) {
                            allInts = false;
                        }
                        
                        try {
                            size_t pos;
                            std::stod(str, &pos);
                            if (pos != str.size()) allFloats = false;
                        } catch (...) {
                            allFloats = false;
                        }
                        
                        if (str != "true" && str != "false") {
                            allStrings = false;
                        }
                    }
                    
                    if (allInts) {
                        std::vector<int64_t> intArray;
                        for (const auto& str : stringArray) {
                            intArray.push_back(std::stoll(str));
                        }
                        configValue = intArray;
                    } else if (allFloats) {
                        std::vector<double> doubleArray;
                        for (const auto& str : stringArray) {
                            doubleArray.push_back(std::stod(str));
                        }
                        configValue = doubleArray;
                    } else {
                        configValue = stringArray;
                    }
                }
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Handle YAML parsing errors (invalid YAML format)
        return false;
    }
#else
    // YAML library not available - cannot parse YAML files
    return false;
#endif
}

// Load and parse .cfg (key=value) configuration file
static bool loadCfgConfig(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, stringToConfigValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Clear existing entries before loading to support file reloading
    clear();
    
    // Detect file format by extension (JSON, YAML, YML, or fallback to .cfg)
    std::string format = detectFileFormat(filepath);
    
    // Parse using appropriate library based on format
    bool success = false;
    if (format == "json") {
        success = loadJsonConfig(filepath, pImpl->entries);
    } else if (format == "yaml") {
        success = loadYamlConfig(filepath, pImpl->entries);
    } else { // "cfg" format - maintains backward compatibility
        success = loadCfgConfig(filepath, pImpl->entries);
    }
    
    return success;
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

// Convert ConfigValue to JSON value for saving
static nlohmann::json configValueToJson(const ConfigValue& value) {
#ifdef HAVE_NLOHMANN_JSON
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
#endif
    // Fallback (should not happen)
    return "PLACEHOLDER_VALUE";
}

// Save configuration to JSON file
static bool saveJsonConfig(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json json;
        
        for (const auto& entry : entries) {
            json[entry.key] = configValueToJson(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(4); // Pretty print with 4-space indentation
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

// Save configuration to YAML file
static bool saveYamlConfig(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Emitter yaml;
        yaml << YAML::BeginMap;
        
        for (const auto& entry : entries) {
            // Get the key string
            std::string key = entry.key;
            
            // Determine type and emit appropriate YAML value
            if (std::holds_alternative<int>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                yaml << YAML::Key << key;
                yaml << YAML::Value << std::get<std::vector<std::string>>(entry.value);
            } else {
                // Unknown type, use string representation
                yaml << YAML::Key << key;
                yaml << YAML::Value << "PLACEHOLDER_VALUE";
            }
        }
        
        yaml << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << yaml.c_str();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

// Save configuration to .cfg (key=value) format
static bool saveCfgConfig(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || 
                         std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << item;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << item;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << "\"" << item << "\"";
                    first = false;
                }
                file << "]";
            } else {
                file << "PLACEHOLDER_VALUE";
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Detect file format by extension
    std::string format = detectFileFormat(filepath);
    
    // Save using appropriate library based on format
    bool success = false;
    if (format == "json") {
        success = saveJsonConfig(filepath, pImpl->entries);
    } else if (format == "yaml") {
        success = saveYamlConfig(filepath, pImpl->entries);
    } else { // "cfg" format
        success = saveCfgConfig(filepath, pImpl->entries);
    }
    
    return success;
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

// Explicit template instantiations
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
