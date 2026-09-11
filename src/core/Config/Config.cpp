#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>

namespace nlm {

// Helper function implementations
static bool parseNumber(const std::string& str, ConfigValue& result) {
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            result = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            result = doubleVal;
            return true;
        }
    } catch (...) {}
    
    return false;
}

static bool parseBoolean(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0") {
        result = false;
        return true;
    }
    return false;
}

static bool parseArray(const std::string& str, std::vector<std::string>& result) {
    // Simple array parsing for basic cases
    if (str.empty() || str[0] != '[' || str.back() != ']') {
        return false;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) {
            end = content.size();
        }
        
        std::string item = trim(content.substr(pos, end - pos));
        // Remove quotes from string items
        if (item.size() >= 2 && ((item.front() == '"' && item.back() == '"') ||
                                 (item.front() == '\'' && item.back() == '\''))) {
            item = item.substr(1, item.size() - 2);
        }
        
        result.push_back(item);
        pos = end + 1;
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == ',')) {
            pos++;
        }
    }
    
    return true;
}

// Config file parsing function implementations
bool Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parser - handle {"key": "value", "number": 123, "bool": true}
    if (content.empty() || content[0] != '{') {
        return false;
    }
    
    // Parse JSON object
    size_t pos = 1; // Skip opening brace
    while (pos < content.size() && content[pos] != '}') {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip whitespace and colon
        size_t valueStart = content.find(':', keyEnd);
        if (valueStart == std::string::npos) {
            break;
        }
        valueStart = content.find_first_not_of("\t\n\r ", valueStart + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Parse value based on first character
        ConfigValue value;
        bool parsed = false;
        
        if (content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd != std::string::npos) {
                value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                parsed = true;
                pos = valueEnd;
            }
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            if (parseBoolean(content.substr(valueStart), value)) {
                parsed = true;
                // Find end of value
                size_t commaPos = content.find(',', valueStart);
                size_t bracePos = content.find('}', valueStart);
                size_t nextPos = std::min(commaPos != std::string::npos ? commaPos : SIZE_MAX,
                                         bracePos != std::string::npos ? bracePos : SIZE_MAX);
                if (nextPos != std::string::npos) {
                    pos = nextPos;
                }
            }
        } else if (content[valueStart] == '-' || isdigit(content[valueStart])) {
            // Number value
            size_t commaPos = content.find(',', valueStart);
            size_t bracePos = content.find('}', valueStart);
            size_t nextPos = std::min(commaPos != std::string::npos ? commaPos : SIZE_MAX,
                                     bracePos != std::string::npos ? bracePos : SIZE_MAX);
            if (nextPos != std::string::npos) {
                std::string numStr = content.substr(valueStart, nextPos - valueStart);
                if (parseNumber(numStr, value)) {
                    parsed = true;
                    pos = nextPos;
                }
            }
        }
        
        if (!parsed) {
            break;
        }
        
        // Skip whitespace and comma or brace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) {
            pos++;
        }
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        }
        
        break;
    }
    
    return true;
}

bool Config::parseYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Parse key: value format (YAML-like)
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Parse value based on its content
            ConfigValue parsedValue;
            bool parsed = false;
            
            // Try to parse as number
            if (parseNumber(value, parsedValue)) {
                parsed = true;
            } 
            // Try to parse as boolean
            else {
                bool boolVal;
                if (parseBoolean(value, boolVal)) {
                    parsedValue = boolVal;
                    parsed = true;
                }
            }
            
            if (!parsed) {
                // Try to parse as array
                std::vector<std::string> arrayVal;
                if (parseArray(value, arrayVal)) {
                    parsedValue = arrayVal;
                    parsed = true;
                } else {
                    // Default to string (remove quotes if present)
                    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                             (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    parsedValue = value;
                    parsed = true;
                }
            }
            
            if (parsed) {
                set(key, parsedValue, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::parseKeyValueFile(const std::string& filepath) {
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

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    try {
        if (extension == ".json") {
            return parseJsonFile(filepath);
        } else if (extension == ".yaml" || extension == ".yml") {
            return parseYamlFile(filepath);
        } else if (extension == ".cfg" || extension == ".txt") {
            // Keep backward compatibility with simple key=value format
            return parseKeyValueFile(filepath);
        } else {
            // Unknown file type, try key=value format as fallback
            return parseKeyValueFile(filepath);
        }
    } catch (const std::exception& e) {
        // Error handling for parsing failures
        return false;
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
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
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

static std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
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
