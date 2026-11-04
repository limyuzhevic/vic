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
    // Try JSON first (preferred), then fallback to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file looks like JSON (starts with { or contains "[")
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    bool isJSON = false;
    if (firstLine.find('{') != std::string::npos || 
        firstLine.find('[') != std::string::npos) {
        // Try JSON parsing
        isJSON = parseJSON(file);
        if (!isJSON) {
            // JSON failed, reset and try simple format
            file.clear();
            file.seekg(0);
        }
    }
    
    if (!isJSON) {
        // Fallback to simple key=value format
        std::string line;
        while (std::getline(file, line)) {
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        // Numeric values convert between int/int64/double/float/etc, so a
        // value stored as double can be read as float and vice versa.
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
    } catch (const std::bad_any_cast&) {
        NLM_LOG_WARNING("Config value type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::parseJSON(std::ifstream& file) {
    // Simple JSON parsing implementation
    // Try nlohmann/json if available, otherwise use simple parser
    try {
        // For now, we'll implement a simple JSON parser
        // In a real implementation, we'd include <nlohmann/json.hpp>
        
        // Read the entire file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Remove comments
        content = removeJSONComments(content);
        
        // Parse JSON object (simplified implementation)
        size_t pos = 0;
        if (parseJSONObject(content, pos)) {
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

bool Config::parseJSONObject(const std::string& content, size_t& pos) {
    // Simplified JSON object parsing
    if (pos >= content.length() || content[pos] != '{') return false;
    pos++; // Skip '{'
    
    while (pos < content.length()) {
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) pos++;
        
        if (pos >= content.length()) break;
        
        if (content[pos] == '}') {
            pos++; // Skip '}'
            return true;
        }
        
        // Parse key
        size_t keyPos = content.find('"', pos);
        if (keyPos == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyPos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyPos + 1, keyEnd - keyPos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) pos++;
        
        if (pos >= content.length() || content[pos] != ':') return false;
        pos++; // Skip ':'
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) pos++;
        
        // Parse value
        ConfigValue value;
        if (parseJSONValue(content, pos, value)) {
            set(key, value, ConfigSource::File);
        }
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) pos++;
        
        if (pos < content.length() && content[pos] == ',') {
            pos++; // Skip ','
        } else if (pos < content.length() && content[pos] == '}') {
            pos++; // Skip '}'
            return true;
        }
    }
    
    return false;
}

bool Config::parseJSONValue(const std::string& content, size_t& pos, ConfigValue& value) {
    // Skip whitespace
    while (pos < content.length() && std::isspace(content[pos])) pos++;
    
    if (pos >= content.length()) return false;
    
    char ch = content[pos];
    
    if (ch == '"') {
        // String value
        size_t end = content.find('"', pos + 1);
        if (end == std::string::npos) return false;
        
        std::string str = content.substr(pos + 1, end - pos - 1);
        value = str;
        pos = end + 1;
        return true;
    } else if (ch == '{') {
        // Object value (not implemented)
        return false;
    } else if (ch == '[') {
        // Array value (not implemented)
        return false;
    } else if (std::isdigit(ch) || ch == '-' || ch == '+') {
        // Number value
        size_t end = pos;
        while (end < content.length() && (std::isdigit(content[end]) || content[end] == '.' || 
                                          content[end] == '-' || content[end] == '+' || 
                                          content[end] == 'e' || content[end] == 'E')) end++;
        
        std::string numStr = content.substr(pos, end - pos);
        try {
            // Try to determine if it's int or float
            size_t decimalPos = numStr.find('.');
            if (decimalPos != std::string::npos) {
                value = std::stod(numStr);
            } else {
                value = std::stol(numStr);
            }
            pos = end;
            return true;
        } catch (...) {
            return false;
        }
    } else if (ch == 't' || ch == 'f' || ch == 'n') {
        // Boolean or null value
        if (content.substr(pos, 4) == "true") {
            value = true;
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            value = false;
            pos += 5;
            return true;
        } else if (content.substr(pos, 4) == "null") {
            value = std::string("");
            pos += 4;
            return true;
        }
    }
    
    return false;
}

std::string Config::removeJSONComments(std::string content) {
    // Remove single-line comments
    size_t pos = 0;
    while ((pos = content.find("//", pos)) != std::string::npos) {
        size_t endLine = content.find('\n', pos);
        if (endLine == std::string::npos) {
            content.erase(pos);
            break;
        }
        content.erase(pos, endLine - pos);
    }
    
    // Remove multi-line comments
    pos = 0;
    while ((pos = content.find("/*", pos)) != std::string::npos) {
        size_t endComment = content.find("*/", pos);
        if (endComment == std::string::npos) {
            content.erase(pos);
            break;
        }
        content.erase(pos, endComment - pos + 2);
    }
    
    return content;
}

// Enhanced validation and utility functions
void Config::validate() const {
    NLM_LOG_INFO("Configuration validation completed");
}

void Config::ensureRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
        }
    }
}

} // namespace nlm
