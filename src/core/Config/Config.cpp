#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ConfigResult Config::loadFromFile(const std::string& filepath) {
    ConfigFormat format = detectFormat(filepath);
    return loadFromFile(filepath, format);
}

ConfigResult Config::loadFromFile(const std::string& filepath, ConfigFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult::Error(ConfigError::FileNotFound, "Cannot open configuration file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromString(content, format);
}

ConfigResult Config::loadFromString(const std::string& content, ConfigFormat format) {
    clear();
    
    switch (format) {
        case ConfigFormat::INI:
            return parseINI(content);
        case ConfigFormat::JSON:
            return parseJSON(content);
        case ConfigFormat::YAML:
            return parseYAML(content);
        default:
            return ConfigResult::Error(ConfigError::InvalidFormat, "Unsupported configuration format");
    }
}

ConfigResult Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult::Error(ConfigError::FileNotFound, "Cannot write configuration file: " + filepath);
    }
    
    // Save in INI format by default
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    
    return ConfigResult::Success();
}

ConfigFormat Config::detectFormat(const std::string& filepath) {
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        extension = filepath.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    if (extension == "json") {
        return ConfigFormat::JSON;
    } else if (extension == "yaml" || extension == "yml") {
        return ConfigFormat::YAML;
    } else if (extension == "ini" || extension == "cfg" || extension == "conf") {
        return ConfigFormat::INI;
    } else {
        return ConfigFormat::Unknown;
    }
}

const std::string& Config::getLastError() const {
    return pImpl->lastError;
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

ConfigError Config::parseValue(const std::string& valueStr, ConfigValue& outValue) {
    std::string trimmed = trim(valueStr);
    
    if (trimmed.empty()) {
        outValue = std::string();
        return ConfigError::None;
    }
    
    // Try boolean
    if (trimmed == "true" || trimmed == "false") {
        bool boolVal = (trimmed == "true");
        outValue = boolVal;
        return ConfigError::None;
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(trimmed, &pos);
        if (pos == trimmed.length()) {
            outValue = intVal;
            return ConfigError::None;
        }
    } catch (...) {}
    
    // Try int64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(trimmed, &pos);
        if (pos == trimmed.length()) {
            outValue = int64Val;
            return ConfigError::None;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.length()) {
            outValue = doubleVal;
            return ConfigError::None;
        }
    } catch (...) {}
    
    // Try string (remove quotes if present)
    if ((trimmed.front() == '\"' && trimmed.back() == '\"') ||
        (trimmed.front() == '\'' && trimmed.back() == '\'')) {
        outValue = trimmed.substr(1, trimmed.length() - 2);
        return ConfigError::None;
    }
    
    // Try vector<int>
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::vector<int> vec;
        std::string inner = trimmed.substr(1, trimmed.length() - 2);
        std::stringstream ss(inner);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                vec.push_back(std::stoi(item));
            } catch (...) {
                return ConfigError::InvalidValue;
            }
        }
        outValue = vec;
        return ConfigError::None;
    }
    
    // Try vector<double>
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::vector<double> vec;
        std::string inner = trimmed.substr(1, trimmed.length() - 2);
        std::stringstream ss(inner);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                vec.push_back(std::stod(item));
            } catch (...) {
                return ConfigError::InvalidValue;
            }
        }
        outValue = vec;
        return ConfigError::None;
    }
    
    // Try vector<string>
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::vector<std::string> vec;
        std::string inner = trimmed.substr(1, trimmed.length() - 2);
        std::stringstream ss(inner);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            // Remove quotes from string items
            if ((item.front() == '\"' && item.back() == '\"') ||
                (item.front() == '\'' && item.back() == '\'')) {
                item = item.substr(1, item.length() - 2);
            }
            vec.push_back(item);
        }
        outValue = vec;
        return ConfigError::None;
    }
    
    // Default to string
    outValue = trimmed;
    return ConfigError::None;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << '\"' << arg << '\"';
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

ConfigResult Config::parseINI(const std::string& content) {
    clear();
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    size_t lineNum = 0;
    
    while (std::getline(stream, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Handle sections
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            pImpl->lastError = "Parse error at line " + std::to_string(lineNum) + ": missing '=' in key=value pair";
            return ConfigResult::Error(ConfigError::ParseError, pImpl->lastError);
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        ConfigValue configValue;
        ConfigError error = parseValue(value, configValue);
        if (error != ConfigError::None) {
            pImpl->lastError = "Parse error at line " + std::to_string(lineNum) + ": invalid value format for key '" + key + "'";
            return ConfigResult::Error(error, pImpl->lastError);
        }
        
        // Prepend section to key if present
        std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
        set(fullKey, configValue, ConfigSource::File);
    }
    
    return ConfigResult::Success();
}

#ifdef HAS_NLOHMANN_JSON
ConfigResult Config::parseJSON(const std::string& content) {
    clear();
    
    try {
        auto json = nlohmann::json::parse(content);
        
        // Recursively parse JSON and populate entries
        parseJSONRecursive("", json);
        
        return ConfigResult::Success();
    } catch (const std::exception& e) {
        pImpl->lastError = "JSON parse error: " + std::string(e.what());
        return ConfigResult::Error(ConfigError::ParseError, pImpl->lastError);
    }
}

void Config::parseJSONRecursive(const std::string& prefix, const nlohmann::json& json) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJSONRecursive(key, it.value());
        }
    } else if (json.is_array()) {
        // Handle arrays as a special case
        std::vector<ConfigValue> vec;
        for (const auto& item : json) {
            ConfigValue val;
            ConfigError error = jsonToConfigValue(item, val);
            if (error == ConfigError::None) {
                vec.push_back(val);
            }
        }
        set(prefix, ConfigValue(vec), ConfigSource::File);
    } else {
        ConfigValue val;
        ConfigError error = jsonToConfigValue(json, val);
        if (error == ConfigError::None) {
            set(prefix, val, ConfigSource::File);
        }
    }
}

ConfigError Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& outValue) {
    if (json.is_boolean()) {
        outValue = json.get<bool>();
        return ConfigError::None;
    } else if (json.is_number_integer()) {
        outValue = json.get<int64_t>();
        return ConfigError::None;
    } else if (json.is_number_float()) {
        outValue = json.get<double>();
        return ConfigError::None;
    } else if (json.is_string()) {
        outValue = json.get<std::string>();
        return ConfigError::None;
    } else if (json.is_array()) {
        // Handle arrays of numbers
        std::vector<double> vec;
        for (const auto& item : json) {
            if (item.is_number()) {
                vec.push_back(item.get<double>());
            } else if (item.is_number_integer()) {
                vec.push_back(item.get<int64_t>());
            } else {
                return ConfigError::InvalidValue;
            }
        }
        outValue = vec;
        return ConfigError::None;
    } else if (json.is_array()) {
        // Handle arrays of strings
        std::vector<std::string> vec;
        for (const auto& item : json) {
            if (item.is_string()) {
                vec.push_back(item.get<std::string>());
            } else {
                return ConfigError::InvalidValue;
            }
        }
        outValue = vec;
        return ConfigError::None;
    }
    
    return ConfigError::InvalidValue;
}
#endif

} // namespace nlm