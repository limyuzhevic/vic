// =============================================================================
// Enhanced Configuration Parser with JSON/YAML Support
// =============================================================================

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to detect file format based on extension and content
static std::string detectFormat(const std::string& filepath) {
    if (filepath.size() < 5) return "unknown";
    
    // Check file extension
    std::string ext = filepath.substr(filepath.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    if (ext == ".cfg" || ext == ".txt") {
        // For config files, auto-detect based on content
        std::ifstream file(filepath);
        if (!file.is_open()) return "unknown";
        
        std::string line;
        if (std::getline(file, line)) {
            // Check if it looks like JSON (starts with { or [)
            if (line.find_first_not_of(" \t\n\r") == '{' || 
                line.find_first_not_of(" \t\n\r") == '[') {
                return "json";
            }
            // Check if it looks like YAML (has key: value format)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos && 
                line.find_first_not_of(" \t\n\r", colonPos + 1) != std::string::npos) {
                return "yaml";
            }
        }
        return "keyvalue";
    }
    return "keyvalue";
}

// Enhanced parsing functions for different formats
static bool parseKeyValueFormat(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) return false;
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return true;
}

static bool parseJSONFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified JSON parsing - in production, use a proper JSON library
    // This implementation handles basic JSON objects with key-value pairs
    
    // Remove whitespace
    std::string json = content;
    json.erase(std::remove_if(json.begin(), json.end(), ::isspace), json.end());
    
    if (json.empty() || json.front() != '{' || json.back() != '}') {
        return false;
    }
    
    json = json.substr(1, json.size() - 2); // Remove braces
    
    // Parse key-value pairs (simplified)
    size_t pos = 0;
    while (pos < json.size()) {
        // Find next key
        size_t keyStart = json.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = json.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 2; // Skip ",
        
        if (pos >= json.size() || json[pos] != ':') break;
        pos++; // Skip :
        
        // Parse value
        if (pos >= json.size()) break;
        
        // Handle different JSON value types
        char firstChar = json[pos];
        std::string valueStr;
        
        if (firstChar == '"') { // String
            size_t valEnd = json.find('"', pos + 1);
            if (valEnd == std::string::npos) break;
            valueStr = json.substr(pos + 1, valEnd - pos - 1);
            pos = valEnd + 2; // Skip ",
        } else if (firstChar == 't' || firstChar == 'f') { // Boolean
            if (json.substr(pos, 4) == "true" || json.substr(pos, 5) == "false") {
                size_t valEnd = json.find_first_of(",}", pos);
                if (valEnd == std::string::npos) valEnd = json.size();
                valueStr = json.substr(pos, valEnd - pos);
                pos = valEnd;
            } else {
                break;
            }
        } else if (firstChar == 'n') { // null (ignored)
            break;
        } else { // Number
            size_t valEnd = json.find_first_of(",}", pos);
            if (valEnd == std::string::npos) valEnd = json.size();
            valueStr = json.substr(pos, valEnd - pos);
            pos = valEnd;
        }
        
        // Convert value string to appropriate type
        ConfigValue configValue;
        if (valueStr == "true") {
            configValue = true;
        } else if (valueStr == "false") {
            configValue = false;
        } else {
            try {
                // Try to parse as integer
                size_t posUnused;
                long long intVal = std::stoll(valueStr, &posUnused);
                if (posUnused == valueStr.size()) {
                    configValue = static_cast<long long>(intVal);
                } else {
                    // Try double
                    double doubleVal = std::stod(valueStr, &posUnused);
                    if (posUnused == valueStr.size()) {
                        configValue = doubleVal;
                    } else {
                        // String
                        configValue = valueStr;
                    }
                }
            } catch (...) {
                configValue = valueStr;
            }
        }
        
        entries.emplace_back(key, configValue, ConfigSource::File);
        
        // Skip comma
        if (pos < json.size() && json[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

static bool parseYAMLFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified YAML parsing - in production, use a proper YAML library
    // This implementation handles simple key: value format
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key: value pairs
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        // Remove quotes from value if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Convert value to appropriate type
        ConfigValue configValue;
        
        // Check if it's a boolean
        if (value == "true") {
            configValue = true;
        } else if (value == "false") {
            configValue = false;
        } else {
            try {
                // Try to parse as integer
                size_t posUnused;
                long long intVal = std::stoll(value, &posUnused);
                if (posUnused == value.size()) {
                    configValue = static_cast<long long>(intVal);
                } else {
                    // Try double
                    double doubleVal = std::stod(value, &posUnused);
                    if (posUnused == value.size()) {
                        configValue = doubleVal;
                    } else {
                        // String
                        configValue = value;
                    }
                }
            } catch (...) {
                configValue = value;
            }
        }
        
        entries.emplace_back(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Clear existing entries
    clear();
    
    // Detect file format
    std::string format = detectFormat(filepath);
    
    if (format == "json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open JSON file: " << filepath << std::endl;
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        
        if (!parseJSONFormat(content, pImpl->entries)) {
            std::cerr << "Error: Failed to parse JSON file: " << filepath << std::endl;
            return false;
        }
        return true;
    } else if (format == "yaml") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open YAML file: " << filepath << std::endl;
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        
        if (!parseYAMLFormat(content, pImpl->entries)) {
            std::cerr << "Error: Failed to parse YAML file: " << filepath << std::endl;
            return false;
        }
        return true;
    } else { // keyvalue format (original format)
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
            
            std::string key, value;
            if (parseKeyValueFormat(line, key, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
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
    // Determine file extension and format based on input
    std::string format = detectFormat(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Add header based on format
    if (format == "json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                   std::is_same_v<T, std::vector<double>> ||
                                   std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "\"" << arg[j] << "\"";
                        } else {
                            file << arg[j];
                        }
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else if (format == "yaml") {
        for (const auto& entry : pImpl->entries) {
            file << entry.key << ": ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                   std::is_same_v<T, std::vector<double>> ||
                                   std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "\"" << arg[j] << "\"";
                        } else {
                            file << arg[j];
                        }
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n";
        }
    } else { // keyvalue format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
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

// Add helper functions for advanced configuration management
void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[j] << "\"";
                    } else {
                        oss << arg[j];
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    oss << "}\n";
    return oss.str();
}

std::string Config::exportToYAML() const {
    std::ostringstream oss;
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[j] << "\"";
                    } else {
                        oss << arg[j];
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "\n";
    }
    return oss.str();
}

// Configuration validation and utilities
bool Config::validate() const {
    // Basic validation - checks for required keys and valid values
    bool isValid = true;
    
    // Check for required keys (examples - customize based on actual requirements)
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            std::cerr << "Warning: Required configuration key not found: " << key << std::endl;
            isValid = false;
        }
    }
    
    // Validate numeric ranges
    auto neuronCount = get<int64_t>("neuron_count");
    if (neuronCount.has_value() && neuronCount.value() <= 0) {
        std::cerr << "Warning: neuron_count must be positive" << std::endl;
        isValid = false;
    }
    
    auto timestep = get<double>("simulation_timestep");
    if (timestep.has_value() && timestep.value() <= 0.0) {
        std::cerr << "Warning: simulation_timestep must be positive" << std::endl;
        isValid = false;
    }
    
    return isValid;
}

std::string Config::statistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    
    // Count by type
    int intCount = 0, doubleCount = 0, stringCount = 0, boolCount = 0,
        vectorCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                intCount++;
            } else if constexpr (std::is_same_v<T, double>) {
                doubleCount++;
            } else if constexpr (std::is_same_v<T, std::string>) {
                stringCount++;
            } else if constexpr (std::is_same_v<T, bool>) {
                boolCount++;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                vectorCount++;
            }
        }, entry.value);
    }
    
    oss << "Integers: " << intCount << "\n";
    oss << "Floats: " << doubleCount << "\n";
    oss << "Strings: " << stringCount << "\n";
    oss << "Booleans: " << boolCount << "\n";
    oss << "Arrays/Vectors: " << vectorCount << "\n";
    
    // Count by source
    oss << "\nBy source:\n";
    int defaultCount = 0, fileCount = 0, cliCount = 0, runtimeCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                defaultCount++;
                break;
            case ConfigSource::File:
                fileCount++;
                break;
            case ConfigSource::CommandLine:
                cliCount++;
                break;
            case ConfigSource::Runtime:
                runtimeCount++;
                break;
        }
    }
    
    oss << "Default: " << defaultCount << "\n";
    oss << "File: " << fileCount << "\n";
    oss << "Command Line: " << cliCount << "\n";
    oss << "Runtime: " << runtimeCount << "\n";
    
    return oss.str();
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
