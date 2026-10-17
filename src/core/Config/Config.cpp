#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string fileFormat;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper: Detect file format from extension
std::string Config::detectFileFormat(const std::string& filepath) {
    if (filepath.empty()) return "";
    
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") return "json";
    if (ext == "yaml" || ext == "yml") return "yaml";
    if (ext == "cfg" || ext == "conf") return "conf";
    return "unknown";
}

// JSON parsing implementation
bool Config::parseJSON(const std::string& content) {
    // Simple JSON parser for key-value pairs (object only, no nested objects)
    // Supports: { "key": "value", "number": 42, "bool": true }
    size_t pos = 0;
    bool inObject = false;
    bool inString = false;
    std::string currentKey;
    std::string currentValue;
    bool parsingKey = true;
    bool escapeNext = false;
    
    while (pos < content.size()) {
        char c = content[pos++];
        
        if (escapeNext) {
            currentValue += c;
            escapeNext = false;
            continue;
        }
        
        if (inString) {
            if (c == '\\') {
                escapeNext = true;
                continue;
            }
            if (c == '"') {
                inString = false;
                // Parse complete key-value pair
                if (parsingKey) {
                    currentKey = currentValue;
                    currentValue.clear();
                    parsingKey = false;
                } else {
                    // Set the value
                    set(currentKey, currentValue, ConfigSource::File);
                    currentKey.clear();
                    currentValue.clear();
                    parsingKey = true;
                }
            } else {
                currentValue += c;
            }
        } else if (c == '"') {
            inString = true;
        } else if (c == '{') {
            inObject = true;
        } else if (c == '}') {
            inObject = false;
            // Parse any remaining key-value
            if (!parsingKey && !currentKey.empty()) {
                set(currentKey, currentValue, ConfigSource::File);
            }
        } else if (c == ':') {
            parsingKey = false;
        } else if (c == ',' && !inString) {
            // Parse complete key-value pair
            if (!parsingKey && !currentKey.empty()) {
                set(currentKey, currentValue, ConfigSource::File);
                currentKey.clear();
                currentValue.clear();
                parsingKey = true;
            }
        } else if (inObject && std::isspace(c)) {
            continue; // Skip whitespace
        }
    }
    
    return true;
}

// YAML parsing implementation (simplified)
bool Config::parseYAML(const std::string& content) {
    // Simple YAML parser for key-value pairs (no nested structures)
    // Supports: key: value
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line = trim(line);
        if (line.empty()) continue;
        
        // Find first colon
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        // Remove quotes from value
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\'"))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Convert string value to appropriate type
        if (value == "true") {
            set(key, true, ConfigSource::File);
        } else if (value == "false") {
            set(key, false, ConfigSource::File);
        } else {
            // Try to parse as number
            bool isNumber = true;
            for (char c : value) {
                if (!std::isdigit(c) && c != '.' && c != '-') {
                    isNumber = false;
                    break;
                }
            }
            
            if (isNumber) {
                // Try double first
                try {
                    double d = std::stod(value);
                    // Check if it's actually an int
                    size_t pos;
                    int i = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        set(key, i, ConfigSource::File);
                    } else {
                        set(key, d, ConfigSource::File);
                    }
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            } else {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

// Simple key=value parser (original implementation)
bool Config::parseKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) return false;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    // Clear existing entries
    clear();
    
    // Parse based on format
    bool success = false;
    if (format == "json") {
        success = parseJSON(content);
        pImpl->fileFormat = "json";
    } else if (format == "yaml" || format == "yml") {
        success = parseYAML(content);
        pImpl->fileFormat = "yaml";
    } else {
        // Try key=value format for .conf, .cfg, or unknown extensions
        success = parseKeyValue(content);
        pImpl->fileFormat = "conf";
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

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) return false;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use the stored format, or detect from filepath if not set
    std::string format = pImpl->fileFormat.empty() ? detectFileFormat(filepath) : pImpl->fileFormat;
    
    // Write header
    if (format == "json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (value ? "true" : "false");
                } else {
                    file << value;
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else if (format == "yaml" || format == "yml") {
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            file << entry.key << ": ";
            
            std::visit([&file](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (value ? "true" : "false");
                } else {
                    file << value;
                }
            }, entry.value);
            
            file << "\n\n";
        }
    } else {
        // Default to key=value format for .conf, .cfg, or unknown extensions
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            file << entry.key << " = ";
            
            std::visit([&file](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (value ? "true" : "false");
                } else {
                    file << value;
                }
            }, entry.value);
            
            file << "\n\n";
        }
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
    oss << "File format: " << pImpl->fileFormat << "\n";
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

} // namespace nlm
