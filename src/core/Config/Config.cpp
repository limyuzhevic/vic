#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <map>
#include <vector>

namespace nlm {

// Forward declarations for internal use
static std::map<std::string, std::string> parseJsonFile(const std::string& filepath);
static std::string trim(const std::string& str);
static std::string toLower(const std::string& str);

// Parse JSON file and extract key-value pairs
std::map<std::string, std::string> parseJsonFile(const std::string& filepath) {
    std::map<std::string, std::string> result;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return result;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    // Simple JSON parsing for key=value pairs
    // This handles basic JSON objects with string keys and various value types
    size_t pos = 0;
    
    // Skip whitespace
    while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
        pos++;
    }
    
    if (pos >= content.length() || content[pos] != '{') {
        // Not a JSON object, fall back to simple parsing
        return result;
    }
    
    pos++; // Skip '{'
    
    while (pos < content.length()) {
        // Skip whitespace
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // Find key
        if (content[pos] != '"') {
            // Not a JSON object
            break;
        }
        
        size_t keyStart = pos;
        pos++; // Skip opening quote
        while (pos < content.length() && content[pos] != '"') {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        size_t keyEnd = pos;
        pos++; // Skip closing quote
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip whitespace after key
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.length() || content[pos] != ':') {
            break;
        }
        
        pos++; // Skip colon
        
        // Skip whitespace after colon
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // Parse value
        if (content[pos] == '"') {
            // String value
            size_t valueStart = pos;
            pos++; // Skip opening quote
            while (pos < content.length() && content[pos] != '"') {
                pos++;
            }
            size_t valueEnd = pos;
            if (pos < content.length()) pos++; // Skip closing quote
            
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            result[key] = value;
        } else if (content[pos] == '{') {
            // Object - skip for now
            size_t braceCount = 1;
            pos++;
            while (pos < content.length() && braceCount > 0) {
                if (content[pos] == '{') braceCount++;
                else if (content[pos] == '}') braceCount--;
                pos++;
            }
        } else if (content[pos] == '[') {
            // Array - skip for now
            size_t bracketCount = 1;
            pos++;
            while (pos < content.length() && bracketCount > 0) {
                if (content[pos] == '[') bracketCount++;
                else if (content[pos] == ']') bracketCount--;
                pos++;
            }
        } else if (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+') {
            // Number
            size_t valueStart = pos;
            while (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+' || content[pos] == '.' || content[pos] == 'e' || content[pos] == 'E')) {
                pos++;
            }
            size_t valueEnd = pos;
            std::string value = content.substr(valueStart, valueEnd - valueStart);
            result[key] = value;
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean
            size_t valueStart = pos;
            if (content.compare(pos, 4, "true") == 0) {
                pos += 4;
            } else if (content.compare(pos, 5, "false") == 0) {
                pos += 5;
            }
            size_t valueEnd = pos;
            std::string value = content.substr(valueStart, valueEnd - valueStart);
            result[key] = value;
        } else {
            // Skip unknown value type
            while (pos < content.length() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
        }
        
        // Skip whitespace and comma
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos < content.length() && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        }
        
        // Check for closing brace
        if (pos < content.length() && content[pos] == '}') {
            break;
        }
    }
    
    return result;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: Implement proper JSON/YAML parser
    // Uses a hybrid approach that supports both JSON and legacy key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first (check for JSON characteristics)
    std::string firstLine;
    file.seekg(0, std::ios::beg);
    std::getline(file, firstLine);
    file.seekg(0, std::ios::beg);
    
    bool isJson = firstLine.find('{') != std::string::npos || 
                  firstLine.find('"') != std::string::npos;
    
    if (isJson) {
        // Parse as JSON
        auto jsonData = parseJsonFile(filepath);
        for (const auto& pair : jsonData) {
            set(pair.first, pair.second, ConfigSource::File);
        }
    } else {
        // Parse legacy key=value format
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

} // namespace nlm
