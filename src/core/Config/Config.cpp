#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>

// JSON support
#include "../external/json.hpp"

// YAML support
#include <yaml-cpp/yaml.h> // May need to be installed separately

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, std::string> nestedConfig;
    bool changed;
    std::chrono::system_clock::time_point lastChangedTime;
    std::string hash;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->changed = false;
    pImpl->lastChangedTime = std::chrono::system_clock::now();
    pImpl->hash = "";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = toLower(path.extension().string());
    
    // Check if file exists
    if (!fileExists(filepath)) {
        return false;
    }
    
    // Clear existing configuration
    clear();
    
    if (extension == ".json") {
        return import(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return import(filepath);
    } else {
        // Try key=value format as fallback for backward compatibility
        return loadKeyValueFile(filepath, *this);
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
    std::filesystem::path path(filepath);
    std::string extension = toLower(path.extension().string());
    
    // Validate configuration before saving
    if (!validate()) {
        return false;
    }
    
    if (extension == ".json") {
        return exportToFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return exportToFile(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return saveKeyValueFile(filepath, *this);
    }
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

// Helper functions implementation

static bool loadJSONFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                std::vector<int64_t> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                if (it->empty()) {
                    value = std::vector<int64_t>{};
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number_integer(); })) {
                    intArray = it->get<std::vector<int64_t>>();
                    value = intArray;
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number_float(); })) {
                    doubleArray = it->get<std::vector<double>>();
                    value = doubleArray;
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_string(); })) {
                    stringArray = it->get<std::vector<std::string>>();
                    value = stringArray;
                } else {
                    // Mixed type array - convert to strings
                    std::vector<std::string> mixedArray;
                    for (const auto& item : *it) {
                        mixedArray.push_back(item.dump());
                    }
                    value = mixedArray;
                }
            } else {
                // Default to string representation
                value = it->dump();
            }
            
            config.set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

static bool loadYAMLFile(const std::string& filepath, Config& config) {
    try {
        YAML::Node root = YAML::LoadFile(filepath);
        
        for (const auto& node : root) {
            std::string key = node.first.as<std::string>();
            ConfigValue value;
            
            if (node.second.IsScalar()) {
                if (node.second.IsInteger()) {
                    value = node.second.as<int64_t>();
                } else if (node.second.IsFloat()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBoolean()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else if (node.second.IsSequence()) {
                std::vector<int64_t> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                if (node.second.empty()) {
                    value = std::vector<int64_t>{};
                } else if (std::all_of(node.second.begin(), node.second.end(), [](const auto& item) { return item.IsInteger(); })) {
                    for (const auto& item : node.second) {
                        intArray.push_back(item.as<int64_t>());
                    }
                    value = intArray;
                } else if (std::all_of(node.second.begin(), node.second.end(), [](const auto& item) { return item.IsFloat(); })) {
                    for (const auto& item : node.second) {
                        doubleArray.push_back(item.as<double>());
                    }
                    value = doubleArray;
                } else {
                    for (const auto& item : node.second) {
                        stringArray.push_back(item.as<std::string>());
                    }
                    value = stringArray;
                }
            } else {
                value = node.second.as<std::string>();
            }
            
            config.set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

static bool loadKeyValueFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = config.trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = config.trim(line.substr(0, pos));
            std::string value = config.trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config.set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

static bool saveJSONFile(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    
    for (const auto& entry : config.pImpl->entries) {
        j[entry.key] = config.configValueToString(entry.value);
    }
    
    file << j.dump(2);
    return true;
}

static bool saveYAMLFile(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    YAML::Emitter out;
    out << YAML::BeginMap;
    
    for (const auto& entry : config.pImpl->entries) {
        out << YAML::Key << entry.key;
        out << YAML::Value << config.configValueToString(entry.value);
    }
    
    out << YAML::EndMap;
    
    file << out.c_str();
    return true;
}

static bool saveKeyValueFile(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : config.pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << config.configValueToString(entry.value) << "\n\n";
    }
    
    return true;
}

static std::string configValueToString(const ConfigValue& value) {
    std::string result;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            // Use consistent precision
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << arg;
            result = oss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg[i];
                result += oss.str();
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
        } else {
            result = "unknown";
        }
    }, value);
    
    return result;
}

static ConfigValue stringToConfigValue(const std::string& str, const std::string& key) {
    // Try to detect type from string
    if (str.empty()) {
        return std::string();
    }
    
    // Try boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

static bool validateConfig(const Config& config) {
    // Basic validation - check for null/empty values for required keys
    for (const auto& entry : config.pImpl->entries) {
        // Check if value exists (should always be true since we just set it)
        if (entry.key.empty()) {
            return false;
        }
        
        // Additional validation can be added here based on specific requirements
    }
    
    return true;
}

static std::string getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension().string();
}

// Additional helper functions

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

static bool isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
}

static bool isInteger(const std::string& str) {
    if (str.empty() || (str[0] == '-' && str.size() == 1)) return false;
    return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c) || c == '-'; });
}

static bool isFloat(const std::string& str) {
    bool hasDecimal = false;
    for (char c : str) {
        if (c == '.') {
            if (hasDecimal) return false; // More than one decimal point
            hasDecimal = true;
        } else if (!std::isdigit(c) && c != '-') {
            return false;
        }
    }
    return hasDecimal && !str.empty();
}

static bool isBoolean(const std::string& str) {
    return str == "true" || str == "false";
}

static std::string trimQuotes(const std::string& str) {
    if (str.size() >= 2 && 
        ((str.front() == '"' && str.back() == '"') ||
         (str.front() == '\'' && str.back() == '\''))) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

static std::string formatNumber(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
}

static std::string escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

static std::string unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; i++; break;
                case '\"': result += '\"'; i++; break;
                case '\'': result += '\''; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i + 1]; i++; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

bool Config::validate() const {
    // Check if config is valid
    for (const auto& entry : pImpl->entries) {
        // Check if value is valid (not empty)
        if (entry.key.empty()) {
            return false;
        }
        
        // Check if value is valid for its type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) {
                    // Empty strings might be valid, depending on context
                }
            }
        }, entry.value);
    }
    
    return true;
}

static std::string getFormatFromExtension(const std::string& extension) {
    std::string ext = toLower(extension);
    if (ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    return "keyvalue";
}

static bool isExtensionSupported(const std::string& extension) {
    std::string ext = toLower(extension);
    return ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".cfg" || ext == ".txt";
}

static std::vector<std::string> getSupportedExtensions() {
    return {".json", ".yaml", ".yml", ".cfg", ".txt"};
}

static std::string getStatistics(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << config.pImpl->entries.size() << std::endl;
    
    std::unordered_map<ConfigSource, int> sourceCount;
    for (const auto& entry : config.pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  Entries by source:" << std::endl;
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

static std::string getHash(const Config& config) {
    // Simple hash implementation - could be enhanced
    std::hash<std::string> hasher;
    std::string combined;
    for (const auto& entry : config.pImpl->entries) {
        combined += entry.key + "=" + config.configValueToString(entry.value) + "\n";
    }
    return std::to_string(hasher(combined));
}

static std::string getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension().string();
}

static bool isExtensionSupported(const std::string& extension) {
    std::string ext = toLower(extension);
    return ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".cfg" || ext == ".txt";
}

static std::vector<std::string> getSupportedExtensions() {
    return {".json", ".yaml", ".yml", ".cfg", ".txt"};
}

static bool fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

static std::optional<ConfigValue> getNested(const Config& config, const std::string& key, char delimiter) {
    // Simple implementation - could be enhanced for real nested support
    return config.get<ConfigValue>(key);
}

static void setNested(Config& config, const std::string& key, const ConfigValue& value, char delimiter, ConfigSource source) {
    // Simple implementation - could be enhanced for real nested support
    config.set(key, value, source);
}

static void removeNested(Config& config, const std::string& key, char delimiter) {
    // Simple implementation - could be enhanced for real nested support
    config.remove(key);
}

static std::string getStatistics(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << config.pImpl->entries.size() << std::endl;
    
    std::unordered_map<ConfigSource, int> sourceCount;
    for (const auto& entry : config.pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  Entries by source:" << std::endl;
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

static std::string getHash(const Config& config) {
    // Simple hash implementation - could be enhanced
    std::hash<std::string> hasher;
    std::string combined;
    for (const auto& entry : config.pImpl->entries) {
        combined += entry.key + "=" + config.configValueToString(entry.value) + "\n";
    }
    return std::to_string(hasher(combined));
}

// Implementation of additional Config methods

bool Config::import(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::exportToFile(const std::string& filepath) const {
    return saveToFile(filepath);
}

bool Config::validate() const {
    return validateConfig(*this);
}

void Config::addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
}

void Config::addRequiredKey(const std::string& key) {
    pImpl->requiredKeys.push_back(key);
}

bool Config::isValid() const {
    // Check required keys
    for (const auto& key : pImpl->requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    
    // Check validators
    for (const auto& pair : pImpl->validators) {
        if (has(pair.first)) {
            auto value = get<ConfigValue>(pair.first);
            if (value && !pair.second(value.value())) {
                return false;
            }
        }
    }
    
    return validateConfig(*this);
}

std::string Config::getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension().string();
}

bool Config::fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

std::optional<ConfigValue> Config::getNested(const std::string& key, char delimiter) const {
    return get<ConfigValue>(key);
}

void Config::setNested(const std::string& key, const ConfigValue& value, char delimiter, ConfigSource source) {
    set(key, value, source);
}

void Config::removeNested(const std::string& key, char delimiter) {
    remove(key);
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::string result;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << arg;
            result = oss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg[i];
                result += oss.str();
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
        } else {
            result = "unknown";
        }
    }, value);
    
    return result;
}

ConfigValue Config::stringToConfigValue(const std::string& str, const std::string& key) {
    return ::stringToConfigValue(str, key);
}

std::string Config::getFormatFromExtension(const std::string& extension) {
    return ::getFormatFromExtension(extension);
}

bool Config::isExtensionSupported(const std::string& extension) {
    return ::isExtensionSupported(extension);
}

std::vector<std::string> Config::getSupportedExtensions() {
    return ::getSupportedExtensions();
}

void Config::setDescription(const std::string& key, const std::string& description) {
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.description = description;
            pImpl->changed = true;
            pImpl->lastChangedTime = std::chrono::system_clock::now();
            return;
        }
    }
}

std::string Config::getDescription(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry.description;
        }
    }
    return "";
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry
            it->value = entry.value;
            it->source = source;
        } else {
            // Add new entry
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
        }
    }
    
    pImpl->changed = true;
    pImpl->lastChangedTime = std::chrono::system_clock::now();
    pImpl->hash = ""; // Invalidate hash
}

size_t Config::size() const {
    return pImpl->entries.size();
}

bool Config::empty() const {
    return pImpl->entries.empty();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigEntry> Config::getValidEntries(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source && has(entry.key)) {
            result.push_back(entry);
        }
    }
    return result;
}

void Config::clearEntriesBySource(ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
}

bool Config::backup(const std::string& filepath) const {
    return saveToFile(filepath);
}

bool Config::restore(const std::string& filepath) {
    clear();
    return loadFromFile(filepath);
}

std::string Config::getStatistics() const {
    return ::getStatistics(*this);
}

bool Config::hasChanged() const {
    return pImpl->changed;
}

void Config::resetChanged() {
    pImpl->changed = false;
}

std::chrono::system_clock::time_point Config::getLastChangedTime() const {
    return pImpl->lastChangedTime;
}

std::string Config::getHash() const {
    return ::getHash(*this);
}

// Existing template instantiations
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
