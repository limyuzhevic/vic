#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    if (filepath.find(".json") != std::string::npos) {
        return loadFromJSON(filepath);
    }
    
    // Fall back to simple key=value format for backward compatibility
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parser for basic structures
    return parseJSON(content);
}

bool Config::parseJSON(const std::string& jsonContent) {
    // Remove whitespace and comments
    std::string cleaned;
    bool inString = false;
    bool escaped = false;
    
    for (char c : jsonContent) {
        if (escaped) {
            cleaned += c;
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            cleaned += c;
            continue;
        }
        
        if (c == '"') {
            inString = !inString;
            cleaned += c;
            continue;
        }
        
        if (!inString && (c == '/' || c == '#')) {
            // Skip comments
            while (++c && c != '\n' && c != '\r');
        }
        
        if (!inString && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            // Skip whitespace
            continue;
        }
        
        cleaned += c;
    }
    
    // Parse key-value pairs (simple JSON object)
    size_t pos = 0;
    while (pos < cleaned.size()) {
        // Find key
        size_t keyStart = cleaned.find('\"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleaned.find('\"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find value
        size_t valueStart = cleaned.find(':', keyEnd);
        if (valueStart == std::string::npos) break;
        
        size_t valueEnd = findMatchingBracket(cleaned, valueStart + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
        
        // Parse value based on type
        set(key, valueStr, ConfigSource::File);
        
        pos = valueEnd + 1;
        
        // Skip comma
        if (pos < cleaned.size() && cleaned[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

size_t Config::findMatchingBracket(const std::string& str, size_t start) {
    if (start >= str.size()) return std::string::npos;
    
    char bracket = str[start];
    if (bracket != '\"' && bracket != '[' && bracket != '{') {
        // Not a JSON value, find next comma or end
        size_t comma = str.find(',', start);
        size_t brace = str.find('}', start);
        size_t bracketClose = str.find(']', start);
        
        size_t result = std::string::npos;
        if (comma != std::string::npos) result = comma;
        if (brace != std::string::npos && (result == std::string::npos || brace < result)) result = brace;
        if (bracketClose != std::string::npos && (result == std::string::npos || bracketClose < result)) result = bracketClose;
        
        return result;
    }
    
    if (bracket == '\"') {
        // String value - find closing quote
        size_t pos = start + 1;
        while (pos < str.size()) {
            if (str[pos] == '\\') {
                pos += 2; // Skip escaped character
                continue;
            }
            if (str[pos] == '\"') return pos;
            pos++;
        }
    } else if (bracket == '[') {
        // Array - count brackets
        int depth = 1;
        size_t pos = start + 1;
        while (pos < str.size() && depth > 0) {
            if (str[pos] == '\\') {
                pos += 2; // Skip escaped character
                continue;
            }
            if (str[pos] == '[') depth++;
            else if (str[pos] == ']') depth--;
            else if (str[pos] == '\"') {
                pos++; // Skip string
                while (pos < str.size() && str[pos] != '\"') {
                    if (str[pos] == '\\') pos += 2;
                    else pos++;
                }
            }
            pos++;
        }
        return pos - 1;
    } else if (bracket == '{') {
        // Object - count braces
        int depth = 1;
        size_t pos = start + 1;
        while (pos < str.size() && depth > 0) {
            if (str[pos] == '\\') {
                pos += 2; // Skip escaped character
                continue;
            }
            if (str[pos] == '{') depth++;
            else if (str[pos] == '}') depth--;
            else if (str[pos] == '\"') {
                pos++; // Skip string
                while (pos < str.size() && str[pos] != '\"') {
                    if (str[pos] == '\\') pos += 2;
                    else pos++;
                }
            }
            pos++;
        }
        return pos - 1;
    }
    
    return std::string::npos;
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
    
    // Check if file has .json extension
    if (filepath.find(".json") != std::string::npos) {
        return saveToJSON(filepath);
    }
    
    // Default to simple key=value format for backward compatibility
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
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
    }
    
    file << "\n}\n";
    
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
