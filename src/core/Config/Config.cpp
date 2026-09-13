#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Simple key=value format loader (original placeholder)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Simple key=value format saver
bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    return true;
}

// JSON loader
bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        for (auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays
                if (value.empty()) {
                    configValue = std::vector<std::string>{};
                } else if (value[0].is_string()) {
                    std::vector<std::string> strArray;
                    strArray.reserve(value.size());
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                } else if (value[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    intArray.reserve(value.size());
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    configValue = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(value.size());
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

// JSON saver
bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = entry.value;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with 2 spaces
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON writing error: " + std::string(e.what()));
        return false;
    }
}

// YAML loader
bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        if (!doc.IsMap()) {
            NLM_LOG_ERROR("Invalid YAML format: root must be a map");
            return false;
        }
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            YAML::Node valueNode = node.second;
            
            ConfigValue configValue;
            
            if (valueNode.IsScalar()) {
                std::string strValue = valueNode.as<std::string>();
                
                // Try to infer type
                if (strValue == "true" || strValue == "false") {
                    configValue = strValue == "true";
                } else {
                    // Try to parse as number
                    try {
                        size_t pos;
                        int intValue = std::stoi(strValue, &pos);
                        if (pos == strValue.length()) {
                            configValue = intValue;
                        } else {
                            double doubleValue = std::stod(strValue, &pos);
                            if (pos == strValue.length()) {
                                configValue = doubleValue;
                            } else {
                                configValue = strValue;
                            }
                        }
                    } catch (...) {
                        configValue = strValue;
                    }
                }
            } else if (valueNode.IsSequence()) {
                std::vector<std::string> strArray;
                strArray.reserve(valueNode.size());
                for (const auto& item : valueNode) {
                    strArray.push_back(item.as<std::string>());
                }
                configValue = strArray;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

// YAML saver
bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << entry.value;
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML writing error: " + std::string(e.what()));
        return false;
    }
}

// Convert string to appropriate ConfigValue type
bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    if (valueStr.empty()) {
        return false;
    }
    
    // Try to infer type
    bool success = false;
    
    // Try int
    try {
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, intValue, source);
            success = true;
        }
    } catch (...) {}
    
    // Try int64_t
    if (!success) {
        try {
            size_t pos;
            long long intValue = std::stoll(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, intValue, source);
                success = true;
            }
        } catch (...) {}
    }
    
    // Try double
    if (!success) {
        try {
            size_t pos;
            double doubleValue = std::stod(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, doubleValue, source);
                success = true;
            }
        } catch (...) {}
    }
    
    // Try bool
    if (!success) {
        if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", source);
            success = true;
        }
    }
    
    // Default to string
    if (!success) {
        set(key, valueStr, source);
        success = true;
    }
    
    return success;
}

// Helper to convert ConfigValue to string for error messages
std::string Config::valueToString(const ConfigValue& value) const {
    std::string result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
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
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else {
            result = std::to_string(arg);
        }
    }, value);
    return result;
}

// Validate config value based on key
bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation rules
    if (key == "neuron_count") {
        int64_t neurons;
        if (std::visit([&](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                neurons = arg;
                return arg > 0 && arg <= 1000000;
            } else if constexpr (std::is_same_v<T, int>) {
                neurons = arg;
                return arg > 0 && arg <= 1000000;
            }
            return false;
        }, value)) {
            return true;
        }
        return false;
    }
    
    if (key == "connection_probability") {
        double prob;
        if (std::visit([&](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                prob = arg;
                return arg >= 0.0 && arg <= 1.0;
            }
            return false;
        }, value)) {
            return true;
        }
        return false;
    }
    
    return true; // No validation errors
}

// Validate all current config values
void Config::validateAll() {
    bool hasErrors = false;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            NLM_LOG_ERROR("Config validation failed for key: " + entry.key + 
                        " with value: " + valueToString(entry.value));
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_ERROR("Configuration validation completed with errors");
    } else {
        NLM_LOG_INFO("Configuration validation passed");
    }
}

// Print help message
void Config::printHelp() {
    std::cout << "=== NLM Configuration ===" << std::endl;
    std::cout << "Usage: program [--key=value] [-key value] [--help] [--version] [--validate]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available options:" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --validate          Validate current configuration" << std::endl;
    std::cout << "  --key=value         Set configuration key to value" << std::endl;
    std::cout << "  -key value          Set configuration key to value (alternative format)" << std::endl;
    std::cout << std::endl;
    std::cout << "Common configuration keys:" << std::endl;
    std::cout << "  neuron_count                Number of neurons (1-1000000)" << std::endl;
    std::cout << "  connection_probability      Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  random_seed                Random seed for reproducibility" << std::endl;
    std::cout << std::endl;
}

// Print version information
void Config::printVersion() {
    std::cout << "NLM (Neural Learning Machine) version 0.1.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Python bindings available via pynlm module" << std::endl;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Detect file format by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    toLower(ext);
    
    bool success = false;
    if (ext == "json") {
        success = loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromYAML(filepath);
    } else {
        // Default to simple key=value format
        success = loadFromSimpleFormat(filepath);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to parse config file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Invalid config value for key \"" + key + "\": " + value);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Missing value for config key: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Invalid config value for key \"" + key + "\": " + value);
                success = false;
            }
        }
        // Handle --help, --version flags
        else if (arg == "--help") {
            printHelp();
        } else if (arg == "--version") {
            printVersion();
        } else if (arg == "--validate") {
            // Validate all current config values
            validateAll();
        } else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config file: " + filepath);
        return false;
    }
    
    // Detect file format by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    toLower(ext);
    
    bool success = false;
    if (ext == "json") {
        success = saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = saveToYAML(filepath);
    } else {
        // Default to simple key=value format
        success = saveToSimpleFormat(filepath);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to write config file: " + filepath);
    }
    
    return success;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Validate the new value before setting
        if (!validateValue(key, value)) {
            NLM_LOG_WARNING("Config validation failed for key \"" + key + "\" with value: " + valueToString(value));
            return;
        }
        
        it->value = value;
        it->source = source;
    } else {
        // Validate the new value before adding
        if (!validateValue(key, value)) {
            NLM_LOG_WARNING("Config validation failed for new key \"" + key + "\" with value: " + valueToString(value));
            return;
        }
        
        pImpl->entries.emplace_back(key, value, source);
    }
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