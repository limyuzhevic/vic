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
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    // Try JSON first
    if (parseJSON(content)) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (JSON format)");
        return true;
    }
    
    // Try YAML
    if (parseYAML(content)) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (YAML format)");
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_WARNING("Configuration file " + filepath + " uses legacy simple format");
    if (!parseSimpleFormat(content)) {
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
        return false;
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
    
    // Write a comment indicating the file format
    file << "# NLM Configuration File\n";
    file << "# This file is in JSON format\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
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

bool Config::parseJSON(const std::string& content) {
    // Simple JSON parser implementation
    // Look for key: value pairs in JSON format
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next " key": value pair
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value (skip whitespace)
        size_t valueStart = content.find_first_not_of(" \t\n\r{", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string valueStr;
        char quoteChar = 0;
        
        // Check if value is quoted
        if (content[valueStart] == '"' || content[valueStart] == '\'') {
            quoteChar = content[valueStart];
            valueStart++;
            size_t valueEnd = content.find(quoteChar, valueStart);
            if (valueEnd == std::string::npos) break;
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        } else if (content[valueStart] == '[') {
            // Array - simplified handling
            size_t bracketEnd = content.find(']', valueStart);
            if (bracketEnd == std::string::npos) break;
            valueStr = content.substr(valueStart, bracketEnd - valueStart + 1);
        } else {
            // Number or boolean
            size_t valueEnd = content.find_first_of(",}\n\r", valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.size();
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        }
        
        // Trim whitespace from key and value
        std::string trimmedKey = trim(key);
        std::string trimmedValue = trim(valueStr);
        
        // Set the value
        set(trimmedKey, trimmedValue, ConfigSource::File);
        
        // Update position
        if (quoteChar) {
            pos = content.find(quoteChar, valueStart) + 1;
        } else {
            pos = content.find_first_of(",}\n\r", valueStart) + 1;
        }
    }
    
    return true;
}

bool Config::parseYAML(const std::string& content) {
    // Simple YAML parser implementation
    size_t pos = 0;
    while (pos < content.size()) {
        // Skip whitespace
        size_t lineStart = content.find_first_not_of(" \t\n\r", pos);
        if (lineStart == std::string::npos) break;
        
        // Skip comments
        if (content[lineStart] == '#') {
            size_t newlinePos = content.find('\n', lineStart);
            pos = (newlinePos == std::string::npos) ? content.size() : newlinePos + 1;
            continue;
        }
        
        // Parse key: value
        size_t colonPos = content.find(':', lineStart);
        if (colonPos == std::string::npos) break;
        
        // Extract key
        std::string key = content.substr(lineStart, colonPos - lineStart);
        key = trim(key);
        
        // Find value
        size_t valueStart = content.find_first_not_of(" \t", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string valueStr;
        char quoteChar = 0;
        
        // Check if value is quoted
        if (content[valueStart] == '"' || content[valueStart] == '\'') {
            quoteChar = content[valueStart];
            valueStart++;
            size_t valueEnd = content.find(quoteChar, valueStart);
            if (valueEnd == std::string::npos) break;
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        } else {
            // Number or boolean
            size_t valueEnd = content.find_first_of("\n\r", valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.size();
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        }
        
        // Trim whitespace
        std::string trimmedKey = trim(key);
        std::string trimmedValue = trim(valueStr);
        
        // Set the value
        set(trimmedKey, trimmedValue, ConfigSource::File);
        
        // Update position
        if (quoteChar) {
            pos = content.find(quoteChar, valueStart) + 1;
        } else {
            pos = content.find_first_of("\n\r", valueStart) + 1;
        }
    }
    
    return true;
}

bool Config::parseSimpleFormat(const std::string& content) {
    // Legacy simple format parsing (kept for backward compatibility)
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

bool Config::stringToBool(const std::string& str) const {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1") return true;
    if (lower == "false" || lower == "no" || lower == "0") return false;
    return false; // Default for invalid input
}

double Config::stringToDouble(const std::string& str) const {
    return std::stod(str);
}

int Config::stringToInt(const std::string& str) const {
    return std::stoi(str);
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
