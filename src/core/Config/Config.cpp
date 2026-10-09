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
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0, std::ios::beg);
    
    // Check if file starts with typical JSON indicators
    bool isJson = false;
    if (!firstLine.empty()) {
        // Trim whitespace
        size_t firstChar = firstLine.find_first_not_of(" \t\n\r");
        if (firstChar != std::string::npos) {
            char firstNonSpace = firstLine[firstChar];
            if (firstNonSpace == '{' || firstNonSpace == '[') {
                isJson = true;
            }
        }
    }
    
    if (isJson) {
        return loadFromJson(file);
    } else {
        // Fall back to simple key=value format for backward compatibility
        return loadFromSimpleFormat(file);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setFromString(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setFromString(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open config file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(val) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                bool vecFirst = true;
                for (int v : val) {
                    if (!vecFirst) file << ", ";
                    vecFirst = false;
                    file << v;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool vecFirst = true;
                for (double v : val) {
                    if (!vecFirst) file << ", ";
                    vecFirst = false;
                    file << std::fixed << std::setprecision(6) << v;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool vecFirst = true;
                for (const std::string& v : val) {
                    if (!vecFirst) file << ", ";
                    vecFirst = false;
                    file << "\"" << escapeString(v) << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    NLM_LOG_INFO("Configuration saved to: " + filepath);
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

// Helper functions for JSON parsing and serialization

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            default:
                if (c >= 32 && c < 127) {
                    result += c;
                } else {
                    result += "\\u";
                    result += "0123456789abcdef"[(c >> 12) & 0xF];
                    result += "0123456789abcdef"[(c >> 8) & 0xF];
                    result += "0123456789abcdef"[(c >> 4) & 0xF];
                    result += "0123456789abcdef"[ c & 0xF];
                }
        }
    }
    return result;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Determine value type by attempting conversions
    std::string trimmedKey = trim(key);
    std::string trimmedValue = trim(valueStr);
    
    // Try int
    try {
        size_t pos;
        long int intVal = std::stol(trimmedValue, &pos);
        if (pos == trimmedValue.length()) {
            set(trimmedKey, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(trimmedValue, &pos);
        if (pos == trimmedValue.length()) {
            set(trimmedKey, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Try bool
    if (trimmedValue == "true" || trimmedValue == "false" || 
        trimmedValue == "yes" || trimmedValue == "no" ||
        trimmedValue == "on" || trimmedValue == "off" ||
        trimmedValue == "1" || trimmedValue == "0") {
        bool boolVal = (trimmedValue == "true" || trimmedValue == "yes" || 
                       trimmedValue == "on" || trimmedValue == "1");
        set(trimmedKey, boolVal, source);
        return;
    }
    
    // Check if it's a list (contains commas)
    if (trimmedValue.find(',') != std::string::npos) {
        // Parse as list
        std::vector<std::string> tokens;
        std::stringstream ss(trimmedValue);
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }
        
        // Try to parse as integer list
        bool allInts = true;
        std::vector<int> intList;
        for (const std::string& token : tokens) {
            try {
                size_t pos;
                long int intVal = std::stol(token, &pos);
                if (pos != token.length()) allInts = false;
                else intList.push_back(intVal);
            } catch (...) {
                allInts = false;
            }
        }
        
        if (allInts && !intList.empty()) {
            set(trimmedKey, intList, source);
            return;
        }
        
        // Try to parse as double list
        bool allDoubles = true;
        std::vector<double> doubleList;
        for (const std::string& token : tokens) {
            try {
                size_t pos;
                double doubleVal = std::stod(token, &pos);
                if (pos != token.length()) allDoubles = false;
                else doubleList.push_back(doubleVal);
            } catch (...) {
                allDoubles = false;
            }
        }
        
        if (allDoubles && !doubleList.empty()) {
            set(trimmedKey, doubleList, source);
            return;
        }
        
        // Try to parse as string list (default)
        set(trimmedKey, tokens, source);
        return;
    }
    
    // Default to string
    set(trimmedKey, trimmedValue, source);
}

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
            
            setFromString(key, value, ConfigSource::File);
        }
    }
    return true;
// Helper functions for JSON parsing and serialization

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            default:
                if (c >= 32 && c < 127) {
                    result += c;
                } else {
                    result += "\\u";
                    result += "0123456789abcdef"[(c >> 12) & 0xF];
                    result += "0123456789abcdef"[(c >> 8) & 0xF];
                    result += "0123456789abcdef"[(c >> 4) & 0xF];
                    result += "0123456789abcdef"[ c & 0xF];
                }
        }
    }
    return result;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    std::string trimmedKey = trim(key);
    std::string trimmedValue = trim(valueStr);
    
    // Try int
    try {
        size_t pos;
        long int intVal = std::stol(trimmedValue, &pos);
        if (pos == trimmedValue.length()) {
            set(trimmedKey, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(trimmedValue, &pos);
        if (pos == trimmedValue.length()) {
            set(trimmedKey, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Try bool
    if (trimmedValue == "true" || trimmedValue == "false" || 
        trimmedValue == "yes" || trimmedValue == "no" ||
        trimmedValue == "on" || trimmedValue == "off" ||
        trimmedValue == "1" || trimmedValue == "0") {
        bool boolVal = (trimmedValue == "true" || trimmedValue == "yes" || 
                       trimmedValue == "on" || trimmedValue == "1");
        set(trimmedKey, boolVal, source);
        return;
    }
    
    // Check if it's a list (contains commas)
    if (trimmedValue.find(',') != std::string::npos) {
        std::vector<std::string> tokens;
        std::stringstream ss(trimmedValue);
        std::string token;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }
        
        // Try to parse as integer list
        bool allInts = true;
        std::vector<int> intList;
        for (const std::string& token : tokens) {
            try {
                size_t pos;
                long int intVal = std::stol(token, &pos);
                if (pos != token.length()) allInts = false;
                else intList.push_back(intVal);
            } catch (...) {
                allInts = false;
            }
        }
        
        if (allInts && !intList.empty()) {
            set(trimmedKey, intList, source);
            return;
        }
        
        // Try to parse as double list
        bool allDoubles = true;
        std::vector<double> doubleList;
        for (const std::string& token : tokens) {
            try {
                size_t pos;
                double doubleVal = std::stod(token, &pos);
                if (pos != token.length()) allDoubles = false;
                else doubleList.push_back(doubleVal);
            } catch (...) {
                allDoubles = false;
            }
        }
        
        if (allDoubles && !doubleList.empty()) {
            set(trimmedKey, doubleList, source);
            return;
        }
        
        // Default to string list
        set(trimmedKey, tokens, source);
        return;
    }
    
    // Default to string
    set(trimmedKey, trimmedValue, source);
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
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
            
            setFromString(key, value, ConfigSource::File);
        }
    }
    return true;
}

bool Config::loadFromJson(std::ifstream& file) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        
        parseJsonString(jsonContent);
        
        NLM_LOG_INFO("Successfully loaded JSON configuration");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config: " + std::string(e.what()));
        return false;
    }
}

void Config::parseJsonString(const std::string& jsonStr) {
    size_t pos = 0;
    while (pos < jsonStr.length()) {
        while (pos < jsonStr.length() && std::isspace(jsonStr[pos])) pos++;
        if (pos >= jsonStr.length()) break;
        
        if (jsonStr[pos] != '"') {
            size_t newlinePos = jsonStr.find('\n', pos);
            if (newlinePos == std::string::npos) break;
            pos = newlinePos + 1;
            continue;
        }
        
        size_t keyStart = pos + 1;
        size_t keyEnd = jsonStr.find('"', keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonStr.substr(keyStart, keyEnd - keyStart);
        pos = keyEnd + 1;
        
        while (pos < jsonStr.length() && std::isspace(jsonStr[pos])) pos++;
        if (pos >= jsonStr.length() || jsonStr[pos] != ':') {
            size_t newlinePos = jsonStr.find('\n', pos);
            if (newlinePos == std::string::npos) break;
            pos = newlinePos + 1;
            continue;
        }
        pos++;
        
        while (pos < jsonStr.length() && std::isspace(jsonStr[pos])) pos++;
        if (pos >= jsonStr.length()) break;
        
        if (jsonStr[pos] == '"') {
            size_t valueStart = pos + 1;
            size_t valueEnd = jsonStr.find('"', valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string value = jsonStr.substr(valueStart, valueEnd - valueStart);
            setFromString(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
            size_t boolEnd = jsonStr.find_first_of(",}\n", pos);
            if (boolEnd == std::string::npos) break;
            
            std::string boolStr = jsonStr.substr(pos, boolEnd - pos);
            setFromString(key, boolStr, ConfigSource::File);
            pos = boolEnd;
        } else if (jsonStr[pos] == 'n') {
            setFromString(key, "", ConfigSource::File);
            pos++;
        } else if (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-' || jsonStr[pos] == '+') {
            size_t numEnd = jsonStr.find_first_of(",}\n", pos);
            if (numEnd == std::string::npos) break;
            
            std::string numStr = jsonStr.substr(pos, numEnd - pos);
            setFromString(key, numStr, ConfigSource::File);
            pos = numEnd;
        }
        
        while (pos < jsonStr.length() && jsonStr[pos] != ',' && jsonStr[pos] != '}') {
            pos++;
        }
        if (pos < jsonStr.length() && jsonStr[pos] == ',') {
            pos++;
        }
    }
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
