#include "Config.hpp"
#include "../BasePimpl.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <cctype>

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(Impl()) {}

Config::~Config() = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
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

// Add missing includes at the top
#include <iostream>
#include <sstream>
#include <limits>
#include <cctype>

// Helper functions for parsing
static std::optional<ConfigValue> parseJSONValue(const std::string& str);
static std::string formatJSONValue(const ConfigValue& value);

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing - supports basic object format
    // In a full implementation, you'd use a proper JSON library
    
    clear(); // Clear existing config
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Skip whitespace and newlines
        while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        // Check for opening brace
        if (content[pos] == '{') {
            pos++; // Skip '{'
            
            // Parse object
            while (pos < content.size() && content[pos] != '}') {
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) {
                    pos++;
                }
                
                if (pos >= content.size()) break;
                
                // Parse key
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != '"' && content[pos] != '\'') {
                    pos++;
                }
                
                if (pos >= content.size()) break;
                
                char quoteChar = content[pos];
                pos++; // Skip opening quote
                
                while (pos < content.size() && content[pos] != quoteChar) {
                    pos++;
                }
                
                if (pos >= content.size()) break;
                
                std::string key = content.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) {
                    pos++;
                }
                
                if (pos < content.size() && content[pos] == ':') {
                    pos++; // Skip ':'
                    
                    // Skip whitespace
                    while (pos < content.size() && std::isspace(content[pos])) {
                        pos++;
                    }
                    
                    // Parse value
                    size_t valueStart = pos;
                    
                    // Handle different value types
                    if (content[pos] == '"' || content[pos] == '\'') {
                        // String value
                        char strQuote = content[pos];
                        pos++;
                        while (pos < content.size() && content[pos] != strQuote) {
                            if (content[pos] == '\\' && pos + 1 < content.size()) {
                                pos += 2; // Skip escaped character
                            } else {
                                pos++;
                            }
                        }
                        if (pos < content.size()) pos++; // Skip closing quote
                    } else if (content[pos] == '{') {
                        // Object value - for now, skip
                        int depth = 1;
                        pos++;
                        while (pos < content.size() && depth > 0) {
                            if (content[pos] == '{') depth++;
                            else if (content[pos] == '}') depth--;
                            pos++;
                        }
                    } else if (content[pos] == '[') {
                        // Array value - for now, skip
                        int depth = 1;
                        pos++;
                        while (pos < content.size() && depth > 0) {
                            if (content[pos] == '[') depth++;
                            else if (content[pos] == ']') depth--;
                            pos++;
                        }
                    } else {
                        // Primitive value (number or boolean)
                        while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                            if (std::isspace(content[pos])) break;
                            pos++;
                        }
                    }
                    
                    std::string valueStr = content.substr(valueStart, pos - valueStart);
                    
                    // Parse and set the value
                    auto parsedValue = parseJSONValue(valueStr);
                    if (parsedValue) {
                        set(key, *parsedValue, ConfigSource::File);
                    }
                    
                    // Skip comma if present
                    while (pos < content.size() && std::isspace(content[pos])) {
                        pos++;
                    }
                    if (pos < content.size() && content[pos] == ',') {
                        pos++; // Skip comma
                    }
                }
            }
        } else {
            // Skip to next line or brace
            while (pos < content.size() && content[pos] != '}') {
                pos++;
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Simple YAML parsing - supports key: value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing config
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        // Skip YAML list items (-) for now
        if (line.find('-') == 0) {
            continue;
        }
        
        // Parse key: value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
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

// Save to JSON format
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->get()->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": " << formatJSONValue(entry.value);
    }
    
    file << "\n}\n";
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->get()->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << ": \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << ": " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double>) {
                file << entry.key << ": " << arg << "\n";
            } else {
                file << entry.key << ": " << arg << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Detect file format by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->get()->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
        
        return true;
    }
}

// Helper functions for parsing
static std::optional<ConfigValue> parseJSONValue(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    const std::string& s = str;
    
    // String
    if ((s.front() == '"' && s.back() == '"') || 
        (s.front() == '\'' && s.back() == '\'')) {
        // Remove quotes and handle escape sequences
        std::string result = s.substr(1, s.size() - 2);
        // TODO: Handle escape sequences like \", \\, \n, \t, etc.
        return ConfigValue(result);
    }
    
    // Boolean
    if (s == "true") return ConfigValue(true);
    if (s == "false") return ConfigValue(false);
    
    // Number
    try {
        if (s.find('.') != std::string::npos) {
            // Double
            size_t pos;
            double val = std::stod(s, &pos);
            if (pos == s.size()) {
                return ConfigValue(val);
            }
        } else {
            // Integer
            size_t pos;
            long long val = std::stoll(s, &pos);
            if (pos == s.size()) {
                if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                    return ConfigValue(static_cast<int>(val));
                } else {
                    return ConfigValue(val);
                }
            }
        }
    } catch (const std::exception&) {
        // Not a number
    }
    
    return std::nullopt;
}

static std::string formatJSONValue(const ConfigValue& value) {
    std::string result;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || 
                            std::is_same_v<T, int64_t> ||
                            std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << arg;
            result = oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[]"; // TODO: Format array
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[]"; // TODO: Format array
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[]"; // TODO: Format array
        } else {
            std::ostringstream oss;
            oss << arg;
            result = oss.str();
        }
    }, value);
    
    return result;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->get()->entries.begin(), pImpl->get()->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->get()->entries.end()) {
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
    auto it = std::find_if(pImpl->get()->entries.begin(), pImpl->get()->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->get()->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->get()->entries.emplace_back(key, value, source);
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
    return std::any_of(pImpl->get()->entries.begin(), pImpl->get()->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->get()->entries.erase(
        std::remove_if(pImpl->get()->entries.begin(), pImpl->get()->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->get()->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->get()->entries.size());
    for (const auto& entry : pImpl->get()->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->get()->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->get()->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->get()->entries) {
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
