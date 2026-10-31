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
    // Enhanced file loading with support for multiple formats:
    // 1. Simple key=value format (legacy)
    // 2. Basic JSON format (object only)
    // 3. YAML-like format (key: value)
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try JSON format first (if content starts with '{' or '[')
    if (!content.empty() && content[0] == '{') {
        return loadFromJSON(content);
    }
    // Try YAML-like format (key: value pairs)
    else if (loadFromYAML(content)) {
        return true;
    }
    // Fall back to simple key=value format
    else {
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        while (std::getline(stream, line)) {
            ++lineNumber;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
                
                try {
                    set(key, value, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error parsing config line ") + 
                                  std::to_string(lineNumber) + ": " + e.what());
                }
            } else {
                NLM_LOG_WARNING(std::string("Ignoring malformed config line ") + 
                              std::to_string(lineNumber) + ": " + line);
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
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// JSON parsing helper methods
bool Config::loadFromJSON(const std::string& jsonContent) {
    // Simple JSON parser for basic objects only
    // This is a lightweight implementation that doesn't require external dependencies
    if (jsonContent.empty() || jsonContent[0] != '{') {
        NLM_LOG_WARNING("Invalid JSON format: expected object { }");
        return false;
    }
    
    size_t pos = 1;
    while (pos < jsonContent.size()) {
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) {
            pos++;
        }
        
        if (pos >= jsonContent.size()) break;
        
        // Parse key (string)
        if (jsonContent[pos] != '"') {
            NLM_LOG_WARNING("Invalid JSON: expected string key");
            return false;
        }
        
        size_t keyStart = ++pos;
        while (pos < jsonContent.size() && jsonContent[pos] != '"') {
            if (jsonContent[pos] == '\\') pos++; // Skip escaped characters
            pos++;
        }
        
        if (pos >= jsonContent.size()) {
            NLM_LOG_WARNING("Invalid JSON: unterminated string key");
            return false;
        }
        
        std::string key = jsonContent.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) {
            pos++;
        }
        
        if (pos >= jsonContent.size() || jsonContent[pos] != ':') {
            NLM_LOG_WARNING("Invalid JSON: expected colon after key");
            return false;
        }
        
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) {
            pos++;
        }
        
        // Parse value
        bool success = false;
        if (pos < jsonContent.size()) {
            if (jsonContent[pos] == '"') {
                // String value
                size_t valueStart = ++pos;
                while (pos < jsonContent.size() && jsonContent[pos] != '"') {
                    if (jsonContent[pos] == '\\') pos++; // Skip escaped characters
                    pos++;
                }
                if (pos < jsonContent.size()) {
                    std::string value = jsonContent.substr(valueStart, pos - valueStart);
                    set(key, value, ConfigSource::File);
                    success = true;
                }
            } else if (jsonContent[pos] == 't' || jsonContent[pos] == 'f') {
                // Boolean value
                if (pos + 4 <= jsonContent.size() && 
                    ((jsonContent.substr(pos, 5) == "true") || 
                     (jsonContent.substr(pos, 5) == "false"))) {
                    std::string value = jsonContent.substr(pos, 5);
                    set(key, value, ConfigSource::File);
                    success = true;
                    pos += 4;
                }
            } else if (jsonContent[pos] == 'n') {
                // Null value
                if (pos + 3 <= jsonContent.size() && jsonContent.substr(pos, 4) == "null") {
                    set(key, "", ConfigSource::File); // Treat null as empty string
                    success = true;
                    pos += 3;
                }
            } else if (jsonContent[pos] == '-' || (jsonContent[pos] >= '0' && jsonContent[pos] <= '9')) {
                // Number value
                size_t valueStart = pos;
                while (pos < jsonContent.size() && 
                       (jsonContent[pos] == '-' || jsonContent[pos] == '.' || 
                        (jsonContent[pos] >= '0' && jsonContent[pos] <= '9') ||
                        jsonContent[pos] == 'e' || jsonContent[pos] == 'E' ||
                        jsonContent[pos] == '+' || jsonContent[pos] == '-')) {
                    pos++;
                }
                if (pos > valueStart) {
                    std::string value = jsonContent.substr(valueStart, pos - valueStart);
                    set(key, value, ConfigSource::File);
                    success = true;
                }
            } else if (jsonContent[pos] == '[') {
                // Array value (simplified)
                NLM_LOG_WARNING("JSON arrays not yet supported");
                return false;
            }
        }
        
        if (!success) {
            NLM_LOG_WARNING(std::string("Invalid JSON value for key: ") + key);
            return false;
        }
        
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) {
            pos++;
        }
        
        if (pos < jsonContent.size() && jsonContent[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < jsonContent.size() && jsonContent[pos] == '}') {
            break; // End of object
        } else {
            NLM_LOG_WARNING("Invalid JSON: expected comma or closing brace");
            return false;
        }
    }
    
    if (jsonContent.back() != '}') {
        NLM_LOG_WARNING("Invalid JSON: missing closing brace");
        return false;
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& content) {
    // Simple YAML-like parser for key: value pairs
    // This handles basic YAML format used in configuration files
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Parse key: value pairs
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle YAML multiline values
            if (value.empty() || value[0] == '\n' || value[0] == '"' || value[0] == '\'') {
                // Skip multiline value for now
                NLM_LOG_WARNING(std::string("YAML multiline values not supported at line ") + 
                              std::to_string(lineNumber));
                continue;
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Error parsing YAML line ") + 
                              std::to_string(lineNumber) + ": " + e.what());
            }
        } else {
            // Check if it's a YAML block sequence indicator
            if (line.find("- ") == 0) {
                NLM_LOG_WARNING(std::string("YAML sequences not supported at line ") + 
                              std::to_string(lineNumber));
                continue;
            }
            NLM_LOG_WARNING(std::string("Ignoring malformed YAML line ") + 
                          std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}
