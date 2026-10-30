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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(file);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFormat(file);
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Use proper value serialization instead of placeholder
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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
        
        file << "\n";
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

// JSON loading implementation
bool Config::loadFromJSON(std::ifstream& file) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing - for now we look for key-value patterns
    // A more robust implementation would use a proper JSON library
    size_t pos = 0;
    while ((pos = content.find("\",\", pos)) != std::string::npos) {
        size_t keyStart = content.find('"', pos + 2);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_not_of(": \n\t", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        size_t valueEnd = valueStart;
        if (content[valueStart] == '"') {
            // String value
            valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            size_t boolEnd = content.find_first_of(",}\n\t ", valueStart);
            if (boolEnd == std::string::npos) break;
            std::string boolStr = content.substr(valueStart, boolEnd - valueStart);
            bool val = (boolStr == "true" || boolStr == "True" || boolStr == "1");
            set(key, val, ConfigSource::File);
        } else if (content[valueStart] == '-' || std::isdigit(content[valueStart])) {
            // Number (integer or float)
            size_t numEnd = content.find_first_of(",}\n\t ", valueStart);
            if (numEnd == std::string::npos) break;
            std::string numStr = content.substr(valueStart, numEnd - valueStart);
            
            // Try as integer first
            try {
                size_t posEnd;
                long long intVal = std::stoll(numStr, &posEnd);
                if (posEnd == numStr.size()) {
                    set(key, intVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Try as double
            try {
                size_t posEnd;
                double doubleVal = std::stod(numStr, &posEnd);
                if (posEnd == numStr.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
        }
        
        pos = valueEnd + 1;
    }
    
    return true;
}

// YAML loading implementation (basic)
bool Config::loadFromYAML(std::ifstream& file) {
    std::string line;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            currentKey.clear();
            continue;
        }
        
        // Check for key: value pattern
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            if (value.empty()) {
                currentKey.clear();
                continue;
            }
            
            // Parse value based on content
            if (value[0] == '"' && value.back() == '"') {
                // String value
                value = value.substr(1, value.size() - 2);
                set(currentKey, value, ConfigSource::File);
            } else if (value == "true" || value == "false") {
                // Boolean value
                bool val = (value == "true");
                set(currentKey, val, ConfigSource::File);
            } else if (std::isdigit(value[0]) || (value[0] == '-' && value.size() > 1 && std::isdigit(value[1]))) {
                // Number (integer or float)
                try {
                    size_t posEnd;
                    long long intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(currentKey, intVal, ConfigSource::File);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(currentKey, doubleVal, ConfigSource::File);
                        continue;
                    }
                } catch (...) {}
                
                // If not a valid number, treat as string
                set(currentKey, value, ConfigSource::File);
            } else {
                // Assume string
                set(currentKey, value, ConfigSource::File);
            }
        } else {
            // Continuation line or nested structure - ignore for now
            currentKey.clear();
        }
    }
    
    return true;
}

// Simple key=value format loader (backward compatibility)
bool Config::loadFromSimpleFormat(std::ifstream& file) {
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

// Enhanced saveToFile implementation
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
    if (ext == ".json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Output value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                    file << arg;
                } else if constexpr (std::is_floating_point_v<T>) {
                    file << arg;
                } else {
                    file << "null";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else if (ext == ".yaml" || ext == ".yml") {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": ";
            
            // Output value based on type
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
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Output value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

// Enhanced set method with nested key support
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string actualKey = key;
    std::string nestedKey;
    
    // Check for nested key (e.g., "brain.neuron_count")
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        nestedKey = key;
        actualKey = key.substr(0, dotPos);
        
        // Check if actual key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
        
        if (it != pImpl->entries.end()) {
            // Convert to JSON-like structure if not already
            // For now, just store the full key as string
            actualKey = key;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // If nested key was used, store both
        if (!nestedKey.empty() && nestedKey != actualKey) {
            // Check if nested key already exists
            auto nestedIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&nestedKey](const ConfigEntry& e) { return e.key == nestedKey; });
            
            if (nestedIt == pImpl->entries.end()) {
                pImpl->entries.emplace_back(nestedKey, value, source, "Nested key from " + actualKey);
            }
        }
    } else {
        pImpl->entries.emplace_back(actualKey, value, source);
    }
}

// Enhanced get method with nested key support
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string actualKey = key;
    std::string nestedKey;
    
    // Check for nested key (e.g., "brain.neuron_count")
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        nestedKey = key;
        actualKey = key.substr(0, dotPos);
        
        // First try the actual key
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
        
        if (it != pImpl->entries.end()) {
            try {
                return std::get<T>(it->value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        
        // If not found, try the nested key
        auto nestedIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&nestedKey](const ConfigEntry& e) { return e.key == nestedKey; });
        
        if (nestedIt != pImpl->entries.end()) {
            try {
                return std::get<T>(nestedIt->value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Enhanced has method with nested key support
bool Config::has(const std::string& key) const {
    std::string actualKey = key;
    std::string nestedKey;
    
    // Check for nested key (e.g., "brain.neuron_count")
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        nestedKey = key;
        actualKey = key.substr(0, dotPos);
        
        // Check if actual key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
        
        if (it != pImpl->entries.end()) {
            return true;
        }
        
        // Check if nested key exists
        auto nestedIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&nestedKey](const ConfigEntry& e) { return e.key == nestedKey; });
        
        return nestedIt != pImpl->entries.end();
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
}

// Enhanced remove method with nested key support
void Config::remove(const std::string& key) {
    std::string actualKey = key;
    std::string nestedKey;
    
    // Check for nested key (e.g., "brain.neuron_count")
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        nestedKey = key;
        actualKey = key.substr(0, dotPos);
        
        // Remove both if they exist
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&actualKey, &nestedKey](const ConfigEntry& e) {
                    return e.key == actualKey || e.key == nestedKey;
                }),
            pImpl->entries.end()
        );
    } else {
        // Simple removal
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&actualKey](const ConfigEntry& e) { return e.key == actualKey; }),
            pImpl->entries.end()
        );
    }
}

// Enhanced getKeys method
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Enhanced clear method
clear() {
    pImpl->entries.clear();
}

// Enhanced summary method
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                oss << arg;
            } else if constexpr (std::is_floating_point_v<T>) {
                oss << arg;
            } else {
                oss << "null";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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
