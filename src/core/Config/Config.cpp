#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> jsonCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Config file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        NLM_LOG_INFO("Using simple key=value format for config file: " + filepath);
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // For now, implement a simple JSON parser
    // In a real implementation, would use a proper JSON library
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - looking for key: "value" pairs
    size_t pos = 0;
    while (pos < content.length()) {
        // Find next " key":
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        trim(key);
        
        // Find colon after key
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value (could be string, number, boolean, or array)
        size_t valueStart = content.find_first_not_of(" \t\n\r:", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Handle different value types
        if (content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, valueStr, ConfigSource::File);
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            if (content.substr(valueStart, 4) == "true" || content.substr(valueStart, 5) == "false") {
                size_t valueEnd = content.find_first_of(",}\]\", valueStart);
                if (valueEnd == std::string::npos) break;
                std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                bool valueBool = (valueStr == "true");
                set(key, valueBool, ConfigSource::File);
            }
        } else if (content[valueStart] == '-' || std::isdigit(content[valueStart])) {
            // Number value (integer or float)
            size_t valueEnd = content.find_first_of(",}\]\", valueStart);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            
            // Try to parse as double first
            try {
                double doubleValue = std::stod(valueStr);
                // Check if it's actually an integer
                if (valueStr.find('.') == std::string::npos && valueStr.find('e') == std::string::npos && valueStr.find('E') == std::string::npos) {
                    set(key, static_cast<int64_t>(doubleValue), ConfigSource::File);
                } else {
                    set(key, doubleValue, ConfigSource::File);
                }
            } catch (...) {
                // Fallback to string
                set(key, valueStr, ConfigSource::File);
            }
        } else if (content[valueStart] == '[') {
            // Array value (simplified - only handles simple arrays of strings/numbers)
            size_t arrayEnd = content.find(']', valueStart);
            if (arrayEnd == std::string::npos) break;
            std::string arrayStr = content.substr(valueStart + 1, arrayEnd - valueStart - 1);
            
            // Parse array - for now just store as string
            set(key, arrayStr, ConfigSource::File);
        }
        
        pos = valueStart + 1;
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " entries from JSON file: " + filepath);
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // For now, use the same implementation as JSON but with YAML-style parsing
    // YAML is similar to JSON but with more complex syntax
    return loadFromJSONFile(filepath);  // TODO: Implement proper YAML parsing
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '%') {
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
        } else {
            // Skip malformed lines with warning
            NLM_LOG_WARNING("Skipping malformed config line " + std::to_string(lineNumber) + " in " + filepath + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " entries from simple file: " + filepath);
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
    // Auto-detect extension for format
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (ext == "json" || ext == "JSON") {
        return saveToJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return saveToYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Handle different value types
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<int>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<double>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << vec[j] << "\"";
            }
            file << "]";
        }
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "}\n";
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " entries to JSON file: " + filepath);
    return true;
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    // YAML format is similar to JSON but with different syntax
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration\n";
    file << "# Generated by NLM Config System\n\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << entry.key << ": ";
        
        // Handle different value types
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            // Remove trailing zeros for cleaner YAML
            double val = std::get<double>(entry.value);
            if (std::abs(val - static_cast<int64_t>(val)) < 0.0001) {
                file << static_cast<int64_t>(val);
            } else {
                file << val;
            }
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else {
            // For complex types, use string representation
            file << "# " << entry.key << ": [complex type not easily serializable]";
        }
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " entries to YAML file: " + filepath);
    return true;
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Handle different value types
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        }
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " entries to simple file: " + filepath);
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
