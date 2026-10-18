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

// Forward declarations for JSON/YAML parsing helpers
namespace {
    bool parseSimpleLine(const std::string& line, std::string& key, std::string& value);
    bool parseJSONValue(const std::string& jsonStr, ConfigValue& outValue);
    bool parseYAMLValue(const std::string& yamlStr, ConfigValue& outValue);
}

// Helper functions for parsing different formats
namespace {
    bool parseSimpleLine(const std::string& line, std::string& key, std::string& value) {
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) return false;
        
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        if (key.empty()) return false;
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return true;
    }
    
    bool parseJSONValue(const std::string& jsonStr, ConfigValue& outValue) {
        // Simplified JSON parsing for basic types
        if (jsonStr.empty()) return false;
        
        // Try to parse as boolean
        if (jsonStr == "true" || jsonStr == "false") {
            outValue = (jsonStr == "true");
            return true;
        }
        
        // Try to parse as integer
        bool isInteger = true;
        for (char c : jsonStr) {
            if (!std::isdigit(c) && c != '-') {
                isInteger = false;
                break;
            }
        }
        if (isInteger && !jsonStr.empty()) {
            try {
                outValue = std::stoi(jsonStr);
                return true;
            } catch (...) {
                // Fall through to other parsers
            }
        }
        
        // Try to parse as float/double
        bool isFloat = true;
        bool hasDecimal = false;
        for (char c : jsonStr) {
            if (!std::isdigit(c) && c != '.' && c != '-') {
                isFloat = false;
                break;
            }
            if (c == '.') hasDecimal = true;
        }
        if (isFloat && (hasDecimal || jsonStr.find('.') != std::string::npos)) {
            try {
                outValue = std::stod(jsonStr);
                return true;
            } catch (...) {
                // Fall through to string parser
            }
        }
        
        // Default to string
        outValue = jsonStr;
        return true;
    }
    
    bool parseYAMLValue(const std::string& yamlStr, ConfigValue& outValue) {
        // Simplified YAML parsing for basic types
        if (yamlStr.empty()) return false;
        
        // Try to parse as boolean
        if (yamlStr == "true" || yamlStr == "false") {
            outValue = (yamlStr == "true");
            return true;
        }
        
        // Try to parse as integer
        bool isInteger = true;
        bool negative = false;
        size_t start = 0;
        if (yamlStr[0] == '-') {
            negative = true;
            start = 1;
        }
        for (size_t i = start; i < yamlStr.size(); ++i) {
            if (!std::isdigit(yamlStr[i])) {
                isInteger = false;
                break;
            }
        }
        if (isInteger && !yamlStr.empty()) {
            try {
                int val = std::stoi(yamlStr);
                outValue = val;
                return true;
            } catch (...) {
                // Fall through to other parsers
            }
        }
        
        // Try to parse as float
        bool isFloat = true;
        bool hasDecimal = false;
        size_t decPos = yamlStr.find('.');
        if (decPos != std::string::npos) hasDecimal = true;
        for (size_t i = start; i < yamlStr.size(); ++i) {
            if (i == decPos) continue; // Skip decimal point
            if (!std::isdigit(yamlStr[i])) {
                isFloat = false;
                break;
            }
        }
        if (isFloat && hasDecimal) {
            try {
                double val = std::stod(yamlStr);
                outValue = val;
                return true;
            } catch (...) {
                // Fall through to string parser
            }
        }
        
        // Default to string
        outValue = yamlStr;
        return true;
    }
}

// Implementation of new file loading methods
bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool hasValidContent = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (parseSimpleLine(line, key, value)) {
            set(key, value, ConfigSource::File);
            hasValidContent = true;
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    if (!hasValidContent) {
        NLM_LOG_WARNING("Config file " + filepath + " contains no valid entries");
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    // Simplified JSON parsing - in a real implementation, use a proper JSON library
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Remove whitespace and comments for simplicity
    // This is a placeholder - real implementation would use nlohmann::json or similar
    NLM_LOG_WARNING("JSON parsing not fully implemented, using simple format for " + filepath);
    
    // Fall back to simple format parsing
    return loadFromSimple(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML config file: " + filepath);
        return false;
    }
    
    // Simplified YAML parsing - in a real implementation, use a proper YAML library
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Remove whitespace and comments for simplicity
    // This is a placeholder - real implementation would use yaml-cpp or similar
    NLM_LOG_WARNING("YAML parsing not fully implemented, using simple format for " + filepath);
    
    // Fall back to simple format parsing
    return loadFromSimple(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_INFO("No command line arguments provided");
        return true;
    }
    
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
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
                continue;
            }
            
            if (i + 1 < argc) {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Missing value for command line argument: " + arg);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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

// Add these new methods to Config.cpp

void Config::setDefault(const std::string& key, const ConfigValue& value) {
    // Set a default value that can only be overridden by higher priority sources
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        pImpl->entries.emplace_back(key, value, ConfigSource::Default);
    }
}

bool Config::mergeFrom(const Config& other) {
    // Merge another config into this one, respecting source priority
    bool changed = false;
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
            changed = true;
        } else if (entry.source > it->source) {
            // Higher priority source overrides existing one
            it->value = entry.value;
            it->source = entry.source;
            changed = true;
        }
    }
    return changed;
}

std::string Config::summary(const std::string& filter) const {
    std::ostringstream oss;
    int count = 0;
    for (const auto& entry : pImpl->entries) {
        if (filter.empty() || toLower(entry.key).find(toLower(filter)) != std::string::npos) {
            oss << "  " << entry.key << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << " (" << static_cast<int>(entry.source) << ")\n";
            count++;
        }
    }
    
    std::ostringstream finalOss;
    finalOss << "Configuration (" << count << " entries):\n" << oss.str();
    if (count == 0) {
        finalOss << "  (No entries match filter)\n";
    }
    return finalOss.str();
}

bool Config::validate(const std::vector<std::pair<std::string, std::function<bool(const ConfigValue&)>>>& validators) const {
    bool allValid = true;
    for (const auto& validator : validators) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&validator](const ConfigEntry& e) { return e.key == validator.first; });
        
        if (it != pImpl->entries.end()) {
            if (!validator.second(it->value)) {
                NLM_LOG_ERROR("Validation failed for config key: " + validator.first);
                allValid = false;
            }
        }
    }
    return allValid;
}

void Config::logChanges(const Config& oldConfig) const {
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(oldConfig.pImpl->entries.begin(), oldConfig.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == oldConfig.pImpl->entries.end() || it->value != entry.value) {
            NLM_LOG_INFO("Config changed: " + entry.key + " (from " + std::to_string(static_cast<int>(it->source)) +
                        " to " + std::to_string(static_cast<int>(entry.source)) + ")");
        }
    }
}
