#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <regex>

// Optional includes for JSON/YAML parsing (compile-time conditional)
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

// Helper function to check file extension
static bool hasExtension(const std::string& filename, const std::string& ext) {
    return filename.size() >= ext.size() && 
           filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

// Helper function to detect file format
static ConfigSource detectFileFormat(const std::string& filepath) {
    if (hasExtension(filepath, ".json")) {
        return ConfigSource::File;
    } else if (hasExtension(filepath, ".yaml") || hasExtension(filepath, ".yml")) {
        return ConfigSource::File;
    } else {
        return ConfigSource::File; // Default to file source
    }
}

// Validation functions
static bool validateKey(const std::string& key);
static bool validateValue(const std::string& key, const std::string& value);

// Helper for JSON/YAML conversion
static std::string jsonToString(const nlohmann::json& j);
static std::string yamlToString(const YAML::Node& node);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_set<std::string> validKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format and use appropriate parser
    ConfigSource source = detectFileFormat(filepath);
    
    if (source == ConfigSource::File && hasExtension(filepath, ".json")) {
        #ifdef HAVE_NLOHMANN_JSON
            return loadFromJSON(filepath);
        #else
            return false; // JSON not available
        #endif
    } else if (source == ConfigSource::File && hasExtension(filepath, ".yaml") || hasExtension(filepath, ".yml")) {
        #ifdef HAVE_YAML_CPP
            return loadFromYAML(filepath);
        #else
            return false; // YAML not available
        #endif
    } else {
        // Fall back to simple key=value format for backward compatibility
        return loadFromKeyValueFile(filepath);
    }
}

// Helper function to detect file format
static ConfigSource detectFileFormat(const std::string& filepath) {
    if (hasExtension(filepath, ".json")) {
        return ConfigSource::File;
    } else if (hasExtension(filepath, ".yaml") || hasExtension(filepath, ".yml")) {
        return ConfigSource::File;
    } else {
        return ConfigSource::File; // Default to file source
    }
}

// JSON loading implementation
bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        if (!jsonData.is_object()) {
            return false;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            if (validateKey(key)) {
                std::string strValue;
                
                if (value.is_string()) {
                    strValue = value.get<std::string>();
                    set(key, strValue, ConfigSource::File);
                } else if (value.is_number_integer()) {
                    set(key, value.get<int>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // Handle arrays
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(jsonToString(item));
                    }
                    set(key, strArray, ConfigSource::File);
                } else {
                    // Unknown type, convert to string
                    strValue = value.dump();
                    set(key, strValue, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false; // JSON not compiled
#endif
}

// YAML loading implementation
bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Loader loader;
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        if (!yamlData.IsMap()) {
            return false;
        }
        
        for (const auto& pair : yamlData) {
            const std::string& key = pair.first.as<std::string>();
            
            if (validateKey(key)) {
                const YAML::Node& value = pair.second;
                
                if (value.IsScalar()) {
                    if (value.IsSequence()) {
                        // Handle sequences (arrays)
                        std::vector<std::string> strArray;
                        for (const auto& item : value) {
                            strArray.push_back(yamlToString(item));
                        }
                        set(key, strArray, ConfigSource::File);
                    } else {
                        std::string strValue = yamlToString(value);
                        
                        // Try to infer type
                        if (strValue == "true" || strValue == "false") {
                            set(key, strValue == "true", ConfigSource::File);
                        } else if (std::regex_match(strValue, std::regex("^[-+]?[0-9]*\\.?[0-9]+$"))) {
                            double numVal = std::stod(strValue);
                            // Check if it's an integer
                            if (numVal == static_cast<int64_t>(numVal)) {
                                set(key, static_cast<int>(numVal), ConfigSource::File);
                            } else {
                                set(key, numVal, ConfigSource::File);
                            }
                        } else {
                            set(key, strValue, ConfigSource::File);
                        }
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false; // YAML not compiled
#endif
}

// Legacy key=value file loading
bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
        
        // Parse simple key=value pairs
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
            
            if (validateKey(key) && validateValue(key, value)) {
                set(key, value, ConfigSource::File);
            } else {
                // Log validation error (could add logging system)
                continue;
            }
        } else {
            // Log parsing error (could add logging system)
            continue;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Validation functions implementation
static bool validateKey(const std::string& key) {
    // Basic validation: key should not be empty
    if (key.empty()) return false;
    
    // Key should not contain special characters
    static const std::regex validKeyRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(key, validKeyRegex);
}

static bool validateValue(const std::string& key, const std::string& value) {
    // Basic validation: value should not be empty for certain keys
    if (key == "random_seed" || key == "neuron_count" || key == "region_count") {
        if (value.empty()) return false;
        
        // Check if value is numeric
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            if (pos != value.size()) return false;
        } catch (...) {
            return false;
        }
    }
    
    // For boolean keys, value should be true/false
    if (key == "log_to_file" || key == "visualization_enabled") {
        if (value != "true" && value != "false") return false;
    }
    
    return true;
}

// Conversion helper functions implementation
static std::string jsonToString(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return std::to_string(j.get<int64_t>());
    } else if (j.is_number_float()) {
        return std::to_string(j.get<double>());
    } else if (j.is_boolean()) {
        return j.get<bool>() ? "true" : "false";
    } else {
        return j.dump();
    }
}

static std::string yamlToString(const YAML::Node& node) {
    if (node.IsScalar()) {
        return node.as<std::string>();
    } else if (node.IsSequence()) {
        std::string result = "[";
        for (size_t i = 0; i < node.size(); ++i) {
            if (i > 0) result += ", ";
            result += yamlToString(node[i]);
        }
        result += "]";
        return result;
    } else {
        return node.as<std::string>();
    }
}
