#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <iomanip>
#include <cmath>

namespace nlm {

// Configuration validator implementation
class DefaultConfigValidator : public ConfigValidator {
public:
    bool validate(const ConfigEntry& entry) const override {
        if (entry.validationRule.empty()) {
            return true; // No validation rule specified
        }
        
        // Parse validation rule (simplified)
        // Format: type(min,max) or type(length) or type(values)
        std::string type = entry.validationRule;
        
        if (entry.validationRule.find('(') != std::string::npos) {
            size_t pos = entry.validationRule.find('(');
            type = entry.validationRule.substr(0, pos);
            std::string params = entry.validationRule.substr(pos + 1, 
                entry.validationRule.find(')') - pos - 1);
            return validateWithParams(type, params, entry);
        }
        
        // Simple type validation
        return validateType(type, entry);
    }
    
    std::string getErrorMessage(const ConfigEntry& entry) const override {
        return "Validation failed for key '" + entry.key + "' with value '" + 
               getValueString(entry.value) + "'";
    }
    
private:
    bool validateWithParams(const std::string& type, const std::string& params, 
                           const ConfigEntry& entry) const {
        if (type == "int" || type == "int64") {
            auto value = std::get_if<int64_t>(&entry.value);
            if (!value) return false;
            return validateIntParams(*value, params);
        } else if (type == "double" || type == "float") {
            auto value = std::get_if<double>(&entry.value);
            if (!value) return false;
            return validateDoubleParams(*value, params);
        } else if (type == "string") {
            auto value = std::get_if<std::string>(&entry.value);
            if (!value) return false;
            return validateStringParams(*value, params);
        } else if (type == "vector") {
            return validateVectorParams(entry, params);
        }
        return false;
    }
    
    bool validateType(const std::string& type, const ConfigEntry& entry) const {
        if (type == "int" || type == "int64") {
            return std::get_if<int64_t>(&entry.value) != nullptr;
        } else if (type == "double" || type == "float") {
            return std::get_if<double>(&entry.value) != nullptr;
        } else if (type == "bool") {
            return std::get_if<bool>(&entry.value) != nullptr;
        } else if (type == "string") {
            return std::get_if<std::string>(&entry.value) != nullptr;
        } else if (type == "int_vector") {
            return std::get_if<std::vector<int>>(&entry.value) != nullptr;
        } else if (type == "double_vector") {
            return std::get_if<std::vector<double>>(&entry.value) != nullptr;
        } else if (type == "string_vector") {
            return std::get_if<std::vector<std::string>>(&entry.value) != nullptr;
        }
        return false;
    }
    
    bool validateIntParams(int64_t value, const std::string& params) const {
        if (params.find(',') != std::string::npos) {
            // Range validation: int(1,100)
            size_t commaPos = params.find(',');
            std::string minStr = params.substr(0, commaPos);
            std::string maxStr = params.substr(commaPos + 1);
            try {
                int64_t minVal = std::stoll(minStr);
                int64_t maxVal = std::stoll(maxStr);
                return value >= minVal && value <= maxVal;
            } catch (...) {
                return false;
            }
        }
        return true; // No range specified
    }
    
    bool validateDoubleParams(double value, const std::string& params) const {
        if (params.find(',') != std::string::npos) {
            size_t commaPos = params.find(',');
            std::string minStr = params.substr(0, commaPos);
            std::string maxStr = params.substr(commaPos + 1);
            try {
                double minVal = std::stod(minStr);
                double maxVal = std::stod(maxStr);
                return value >= minVal && value <= maxVal;
            } catch (...) {
                return false;
            }
        }
        return true;
    }
    
    bool validateStringParams(const std::string& value, const std::string& params) const {
        if (params.empty()) return true;
        
        // Length validation: string(1,100)
        if (params.find(',') != std::string::npos) {
            size_t commaPos = params.find(',');
            std::string minStr = params.substr(0, commaPos);
            std::string maxStr = params.substr(commaPos + 1);
            try {
                size_t minLen = std::stoull(minStr);
                size_t maxLen = std::stoull(maxStr);
                size_t len = value.length();
                return len >= minLen && len <= maxLen;
            } catch (...) {
                return false;
            }
        }
        return true;
    }
    
    bool validateVectorParams(const ConfigEntry& entry, const std::string& params) const {
        // Vector size validation: vector(1,10)
        auto vecValue = std::get_if<std::vector<std::string>>(&entry.value);
        if (!vecValue) return false;
        
        if (params.find(',') != std::string::npos) {
            size_t commaPos = params.find(',');
            std::string minStr = params.substr(0, commaPos);
            std::string maxStr = params.substr(commaPos + 1);
            try {
                size_t minLen = std::stoull(minStr);
                size_t maxLen = std::stoull(maxStr);
                size_t len = vecValue->size();
                return len >= minLen && len <= maxLen;
            } catch (...) {
                return false;
            }
        }
        return true;
    }
    
    std::string getValueString(const ConfigValue& value) const {
        std::ostringstream oss;
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
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validator(std::make_unique<DefaultConfigValidator>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::string format = detectFileFormat(filepath);
    if (format == "json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file: " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseJSON(buffer.str());
    } else if (format == "yaml") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file: " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseYAML(buffer.str());
    } else {
        // Fallback to simple key=value format
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
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigError& e) {
                    NLM_LOG_WARNING(std::string("Failed to parse command line argument --") + key + ": " + e.what());
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigError& e) {
                NLM_LOG_WARNING(std::string("Failed to parse command line argument -") + key + ": " + e.what());
                success = false;
            }
        }
    }
    return success;
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    std::string detectedFormat = format;
    if (format == "auto") {
        detectedFormat = detectFormatFromContent(content);
    }
    
    if (detectedFormat == "json") {
        return parseJSON(content);
    } else if (detectedFormat == "yaml") {
        return parseYAML(content);
    } else {
        // Parse as simple key=value format
        std::istringstream stream(content);
        std::string line;
        bool success = true;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return success;
    }
}

bool Config::parseJSON(const std::string& content) {
    try {
        // Simple JSON parsing implementation
        // In a real implementation, you'd use a proper JSON library like nlohmann/json
        
        // For now, implement a simplified parser
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '/' || line[0] == '}') {
                continue;
            }
            
            // Look for key-value pairs in JSON format: "key": "value"
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Clean up key and value
                key = trim(key);
                value = trim(value);
                
                // Remove quotes from key and value
                if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                    key = key.substr(1, key.size() - 2);
                }
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Parse value type
                ConfigValue configValue;
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else if (std::regex_match(value, std::regex("^\d+$"))) {
                    configValue = std::stoll(value);
                } else if (std::regex_match(value, std::regex("^\d+\.\d+$"))) {
                    configValue = std::stod(value);
                } else {
                    configValue = value;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("JSON parsing failed: ") + e.what());
    }
}

bool Config::parseYAML(const std::string& content) {
    try {
        // Simple YAML parsing implementation
        // In a real implementation, you'd use a proper YAML library like yaml-cpp
        
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '-') {
                continue;
            }
            
            // Look for key-value pairs in YAML format: key: value
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Remove quotes from value
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Parse value type
                ConfigValue configValue;
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else if (std::regex_match(value, std::regex("^\d+$"))) {
                    configValue = std::stoll(value);
                } else if (std::regex_match(value, std::regex("^\d+\.\d+$"))) {
                    configValue = std::stod(value);
                } else {
                    configValue = value;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception& e) {
        throw ConfigParseError(std::string("YAML parsing failed: ") + e.what());
    }
}

bool Config::parseINI(const std::string& content) {
    return loadFromString(content, "ini");
}

std::string Config::detectFileFormat(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return "unknown";
    }
    
    // Check file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    if (ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    if (ext == ".ini" || ext == ".cfg") return "ini";
    
    // Try to detect from content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "unknown";
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    firstLine = toLower(trim(firstLine));
    if (firstLine.find('{') != std::string::npos) {
        return "json";
    } else if (firstLine.find("key:") != std::string::npos || 
               firstLine.find("section:") != std::string::npos) {
        return "yaml";
    } else {
        return "ini";
    }
}

std::string Config::detectFormatFromContent(const std::string& content) {
    std::string firstLine = trim(content);
    if (firstLine.empty()) {
        return "unknown";
    }
    
    firstLine = toLower(firstLine);
    if (firstLine.find('{') != std::string::npos || 
        firstLine.find('"') != std::string::npos && firstLine.find(':') != std::string::npos) {
        return "json";
    } else if (firstLine.find("key:") != std::string::npos || 
               firstLine.find("section:") != std::string::npos) {
        return "yaml";
    } else {
        return "ini";
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "// NLM Configuration File\n";
    file << "// Generated at: " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description;
        if (!entry.validationRule.empty()) {
            file << " [" << entry.validationRule << "]";
        }
        file << "\n";
        
        switch (entry.value.index()) {
            case 0: // int
            case 1: // int64_t
                file << entry.key << " = " << std::get<int64_t>(entry.value);
                break;
            case 2: // double
                file << entry.key << " = " << std::fixed << std::setprecision(6) 
                     << std::get<double>(entry.value);
                break;
            case 3: // bool
                file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false");
                break;
            case 4: // std::string
                file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"";
                break;
            case 5: // std::vector<int>
                file << entry.key << " = [";
                for (size_t i = 0; i < std::get<std::vector<int>>(entry.value).size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::get<std::vector<int>>(entry.value)[i];
                }
                file << "]";
                break;
            case 6: // std::vector<double>
                file << entry.key << " = [";
                for (size_t i = 0; i < std::get<std::vector<double>>(entry.value).size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::get<std::vector<double>>(entry.value)[i];
                }
                file << "]";
                break;
            case 7: // std::vector<std::string>
                file << entry.key << " = [";
                for (size_t i = 0; i < std::get<std::vector<std::string>>(entry.value).size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << std::get<std::vector<std::string>>(entry.value)[i] << "\"";
                }
                file << "]";
                break;
        }
        file << "\n\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, "", "", source);
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

void Config::set(const std::string& key, const ConfigValue& value, 
                 const std::string& description, const std::string& validationRule,
                 ConfigSource source) {
    // Validate the value if validation rule is specified
    ConfigEntry entry(key, value, source, description, validationRule);
    if (!validateValue(entry)) {
        throw ConfigValidationError("Configuration validation failed for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description;
        it->validationRule = validationRule;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, validationRule);
    }
}

bool Config::validateValue(const ConfigEntry& entry) const {
    return validator->validate(entry);
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.validationRule.empty()) {
            oss << " [valid:" << entry.validationRule << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return false;
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry)) {
            errors.push_back(validator->getErrorMessage(entry));
        }
    }
    return errors;
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

std::string Config::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
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
