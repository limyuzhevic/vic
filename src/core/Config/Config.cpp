#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>

namespace nlm {

// Helper function to trim whitespace from both ends
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parsing first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parsing - just read and parse basic JSON
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Parse basic JSON - look for key:value pairs
        size_t pos = 0;
        while (pos < content.length()) {
            // Skip whitespace and comments
            while (pos < content.length() && (std::isspace(content[pos]) || content[pos] == '#')) {
                if (content[pos] == '#') {
                    // Skip comment
                    while (pos < content.length() && content[pos] != '\n') {
                        pos++;
                    }
                } else {
                    pos++;
                }
            }
            
            if (pos >= content.length()) break;
            
            // Find key (string until ':')
            size_t keyStart = pos;
            while (pos < content.length() && content[pos] != ':') {
                pos++;
            }
            
            if (pos >= content.length()) break;
            
            std::string key = trim(content.substr(keyStart, pos - keyStart));
            pos++; // Skip ':'
            
            // Skip whitespace after ':'
            while (pos < content.length() && std::isspace(content[pos])) {
                pos++;
            }
            
            // Parse value based on type
            ConfigValue value;
            size_t valueStart = pos;
            
            if (pos < content.length() && content[pos] == '"') {
                // String value
                pos++; // Skip opening quote
                while (pos < content.length() && content[pos] != '"') {
                    pos++;
                }
                if (pos < content.length()) {
                    std::string strValue = content.substr(valueStart + 1, pos - valueStart - 1);
                    value = strValue;
                    pos++; // Skip closing quote
                }
            } else if (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '-')) {
                // Numeric value (int or float)
                while (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-' || content[pos] == 'e' || content[pos] == 'E')) {
                    pos++;
                }
                std::string numStr = trim(content.substr(valueStart, pos - valueStart));
                
                // Try to parse as integer first
                try {
                    size_t posEnd;
                    long long intVal = std::stoll(numStr, &posEnd);
                    if (posEnd == numStr.length()) {
                        value = intVal;
                    } else {
                        // Try float
                        float floatVal = std::stof(numStr);
                        value = static_cast<double>(floatVal);
                    }
                } catch (...) {
                    // Try float
                    try {
                        float floatVal = std::stof(numStr);
                        value = static_cast<double>(floatVal);
                    } catch (...) {
                        // Try bool
                        if (toLower(numStr) == "true") {
                            value = true;
                        } else if (toLower(numStr) == "false") {
                            value = false;
                        } else {
                            // Default to string
                            value = numStr;
                        }
                    }
                }
            } else if (pos < content.length() && (toLower(content.substr(pos, 4)) == "true" || toLower(content.substr(pos, 5)) == "false")) {
                // Boolean value
                size_t boolEnd = pos;
                while (boolEnd < content.length() && !std::isspace(content[boolEnd]) && content[boolEnd] != ',' && content[boolEnd] != '}') {
                    boolEnd++;
                }
                std::string boolStr = trim(content.substr(pos, boolEnd - pos));
                bool boolValue = (toLower(boolStr) == "true");
                value = boolValue;
                pos = boolEnd;
            } else {
                // Default to string
                while (pos < content.length() && content[pos] != ',' && content[pos] != '}') {
                    pos++;
                }
                value = trim(content.substr(valueStart, pos - valueStart));
            }
            
            // Skip comma or closing brace
            if (pos < content.length() && (content[pos] == ',' || content[pos] == '}')) {
                pos++;
            }
            
            // Add entry
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (...) {
        // JSON parsing failed, fall back to simple format
        return loadFromSimpleFormat(filepath);
    }
}

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
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << arg;
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

// Explicit template instantiations
// The instantiations will be automatically generated when this file is compiled

} // namespace nlm

