#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <memory>
#include <limits>
#include <iomanip>

namespace nlm {

static std::string getFileExtension(const std::string& filepath);
static bool isYamlExtension(const std::string& ext);
static bool isJsonExtension(const std::string& ext);
static bool isConfigExtension(const std::string& ext);

// Helper function to detect file format
std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) return "";
    
    std::string ext = filepath.substr(pos);
    for (char& c : ext) c = std::tolower(c);
    return ext;
}

bool Config::isYamlExtension(const std::string& ext) {
    return ext == ".yaml" || ext == ".yml";
}

bool Config::isJsonExtension(const std::string& ext) {
    return ext == ".json";
}

bool Config::isConfigExtension(const std::string& ext) {
    return ext == ".cfg" || ext == ".txt" || ext == ".ini";
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Simple JSON parser implementation
bool parseJsonString(const std::string& jsonStr, Config& config) {
    // Simple JSON parser that extracts key-value pairs
    // This is a basic implementation that handles simple JSON objects
    // For production use, consider using a dedicated JSON library
    
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Skip whitespace
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
        
        if (pos >= jsonStr.size()) break;
        
        if (jsonStr[pos] == '{') {
            pos++;
            // Parse JSON object
            while (pos < jsonStr.size() && jsonStr[pos] != '}') {
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                
                if (pos >= jsonStr.size()) break;
                
                // Find key (string in quotes)
                if (jsonStr[pos] == '"') {
                    pos++;
                    size_t keyStart = pos;
                    while (pos < jsonStr.size() && jsonStr[pos] != '"') pos++;
                    if (pos >= jsonStr.size()) break;
                    
                    std::string key = jsonStr.substr(keyStart, pos - keyStart);
                    pos++; // Skip closing quote
                    
                    // Skip whitespace
                    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                    
                    if (pos >= jsonStr.size() || jsonStr[pos] != ':') break;
                    pos++;
                    
                    // Skip whitespace
                    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                    
                    // Parse value based on first character
                    if (jsonStr[pos] == '"') {
                        // String value
                        pos++;
                        size_t valueStart = pos;
                        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                            if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
                                pos++; // Skip escape character
                            }
                            pos++;
                        }
                        std::string value = jsonStr.substr(valueStart, pos - valueStart);
                        pos++; // Skip closing quote
                        config.set(key, value, ConfigSource::File);
                    } else if (jsonStr[pos] == '-' || (jsonStr[pos] >= '0' && jsonStr[pos] <= '9')) {
                        // Number value
                        size_t valueStart = pos;
                        while (pos < jsonStr.size() && (jsonStr[pos] == '-' || jsonStr[pos] == '+' || 
                                                     (jsonStr[pos] >= '0' && jsonStr[pos] <= '9') ||
                                                     jsonStr[pos] == '.' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
                            pos++;
                        }
                        std::string numStr = jsonStr.substr(valueStart, pos - valueStart);
                        try {
                            double num = std::stod(numStr);
                            // Try to convert to int if it's a whole number
                            if (num == static_cast<int>(num) && 
                                num >= std::numeric_limits<int>::min() && 
                                num <= std::numeric_limits<int>::max()) {
                                config.set(key, static_cast<int>(num), ConfigSource::File);
                            } else {
                                config.set(key, num, ConfigSource::File);
                            }
                        } catch (...) {
                            // If parsing fails, store as string
                            config.set(key, numStr, ConfigSource::File);
                        }
                    } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                        // Boolean value
                        std::string boolStr;
                        while (pos < jsonStr.size() && (jsonStr[pos] == 't' || jsonStr[pos] == 'f' || 
                                                     jsonStr[pos] == 'r' || jsonStr[pos] == 'u' || 
                                                     jsonStr[pos] == 'e')) {
                            boolStr += jsonStr[pos++];
                        }
                        if (boolStr == "true") {
                            config.set(key, true, ConfigSource::File);
                        } else if (boolStr == "false") {
                            config.set(key, false, ConfigSource::File);
                        } else {
                            config.set(key, boolStr, ConfigSource::File);
                        }
                    } else if (jsonStr[pos] == 'n') {
                        // null value
                        while (pos < jsonStr.size() && jsonStr[pos] == 'n') pos++;
                        config.set(key, std::string("null"), ConfigSource::File);
                    } else if (jsonStr[pos] == '[') {
                        // Array value - skip for now
                        pos++;
                        while (pos < jsonStr.size() && jsonStr[pos] != ']') {
                            pos++;
                        }
                        if (pos < jsonStr.size()) pos++;
                    }
                    
                    // Skip whitespace and comma
                    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                    if (pos < jsonStr.size() && jsonStr[pos] == ',') pos++;
                } else {
                    // Skip invalid character
                    pos++;
                }
            }
        } else {
            pos++;
        }
    }
    return true;
}

// Simple YAML parser implementation
bool parseYamlString(const std::string& yamlStr, Config& config) {
    // Simple YAML parser that extracts key-value pairs
    // This is a basic implementation that handles simple YAML key-value pairs
    // For production use, consider using a dedicated YAML library
    
    std::istringstream iss(yamlStr);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        
        // Skip if line starts with '-' (list item)
        if (!line.empty() && line[0] == '-') {
            continue;
        }
        
        // Parse key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            // Trim key
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            std::string value = line.substr(colonPos + 1);
            // Trim value
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes if present
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

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Detect file format by extension
        std::string ext = getFileExtension(filepath);
        
        if (isJsonExtension(ext)) {
            // Load JSON file
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open JSON file for reading: " + filepath);
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string jsonStr = buffer.str();
            
            return parseJsonString(jsonStr, *this);
        } else if (isYamlExtension(ext)) {
            // Load YAML file
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open YAML file for reading: " + filepath);
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string yamlStr = buffer.str();
            
            return parseYamlString(yamlStr, *this);
        } else if (isConfigExtension(ext)) {
            // Load legacy key=value format
            return loadFromFileLegacy(filepath);
        } else {
            // Unknown format, try legacy format
            return loadFromFileLegacy(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromFileLegacy(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for reading (legacy format): " + filepath);
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading legacy config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Detect file format by extension
    std::string ext = getFileExtension(filepath);
    
    if (isJsonExtension(ext)) {
        // Save as JSON
        saveToJsonFile(filepath);
    } else if (isYamlExtension(ext)) {
        // Save as YAML
        saveToYamlFile(filepath);
    } else {
        // Save as legacy key=value format
        saveToFileLegacy(filepath);
    }
    
    return true;
}

void Config::saveToFileLegacy(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing (legacy format): " + filepath);
            return;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << getValueString(entry.value) << "\n";
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving legacy config file " + filepath + ": " + std::string(e.what()));
    }
}

void Config::saveToJsonFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file for writing: " + filepath);
            return;
        }
        
        file << "{\n";
        
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << escapeJsonString(entry.key) << "\": " << configValueToJson(entry.value);
        }
        
        file << "\n}\n";
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving JSON config file " + filepath + ": " + std::string(e.what()));
    }
}

void Config::saveToYamlFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML config file for writing: " + filepath);
            return;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated from NLM Config system\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": " << configValueToYaml(entry.value) << "\n";
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving YAML config file " + filepath + ": " + std::string(e.what()));
    }
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::configValueToJson(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeJsonString(arg) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << escapeJsonString(arg[i]) << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::configValueToYaml(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeYamlString(arg) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << escapeYamlString(arg[i]) << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::escapeJsonString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string Config::escapeYamlString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        if (c == '\'') {
            oss << "''";
        } else if (c == '"') {
            oss << "\"\"";
        } else if (c == '\n') {
            oss << "\\n";
        } else if (c == '\t') {
            oss << "\\t";
        } else {
            oss << c;
        }
    }
    return oss.str();
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
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without trailing zeros
                std::ostringstream tempOss;
                tempOss << std::fixed << std::setprecision(6) << arg;
                std::string result = tempOss.str();
                // Remove trailing zeros and possibly decimal point
                size_t pos = result.find_last_not_of('0');
                if (pos != std::string::npos && result[pos] == '.') {
                    pos++;
                }
                result.erase(pos + 1, std::string::npos);
                oss << result;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    // Format double without trailing zeros
                    std::ostringstream tempOss;
                    tempOss << std::fixed << std::setprecision(6) << item;
                    std::string result = tempOss.str();
                    size_t pos = result.find_last_not_of('0');
                    if (pos != std::string::npos && result[pos] == '.') {
                        pos++;
                    }
                    result.erase(pos + 1, std::string::npos);
                    oss << result;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
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