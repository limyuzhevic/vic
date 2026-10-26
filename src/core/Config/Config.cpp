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
    // Enhanced file format support: Try multiple formats in order
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON format first
    if (loadFromJSON(content, ConfigSource::File)) {
        return true;
    }
    
    // Try YAML format second
    if (loadFromYAML(content, ConfigSource::File)) {
        return true;
    }
    
    // Fall back to simple key=value format
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
        
        // Save configuration value with proper formatting
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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

// Helper functions for JSON parsing
bool Config::loadFromJSON(const std::string& content, ConfigSource source) {
    try {
        // Simple JSON parsing implementation
        // In a production environment, you would use a proper JSON library like nlohmann::json
        size_t pos = 0;
        while (pos < content.length()) {
            // Skip whitespace
            while (pos < content.length() && std::isspace(content[pos])) {
                pos++;
            }
            
            if (pos >= content.length()) break;
            
            // Check for opening brace
            if (content[pos] == '{') {
                pos++;
                continue;
            }
            
            // Check for closing brace
            if (content[pos] == '}') {
                pos++;
                continue;
            }
            
            // Extract key (expecting "key": value format)
            if (content[pos] == '\"') {
                pos++;
                size_t keyStart = pos;
                while (pos < content.length() && content[pos] != '\"') {
                    pos++;
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                // Skip whitespace
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                
                // Check for colon
                if (pos < content.length() && content[pos] == ':') {
                    pos++;
                    // Skip whitespace after colon
                    while (pos < content.length() && std::isspace(content[pos])) {
                        pos++;
                    }
                    
                    // Extract value
                    std::string valueStr = extractJsonValue(content, pos);
                    set(key, valueStr, source);
                }
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& content, ConfigSource source) {
    try {
        // Simple YAML parsing implementation
        // In a production environment, you would use a proper YAML library like YAML-CPP
        size_t pos = 0;
        while (pos < content.length()) {
            // Skip whitespace and comments
            while (pos < content.length() && 
                   (std::isspace(content[pos]) || content[pos] == '#')) {
                if (content[pos] == '#') {
                    // Skip until end of line
                    while (pos < content.length() && content[pos] != '\n') {
                        pos++;
                    }
                } else {
                    pos++;
                }
            }
            
            if (pos >= content.length()) break;
            
            // Check for document start
            if (content[pos] == '-') {
                pos++;
                // Skip whitespace
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                
                // Check if it's a key-value pair
                if (content[pos] == '\"') {
                    pos++;
                    size_t keyStart = pos;
                    while (pos < content.length() && content[pos] != '\"') {
                        pos++;
                    }
                    std::string key = content.substr(keyStart, pos - keyStart);
                    pos++; // Skip closing quote
                    
                    // Skip whitespace
                    while (pos < content.length() && std::isspace(content[pos])) {
                        pos++;
                    }
                    
                    // Check for colon
                    if (pos < content.length() && content[pos] == ':') {
                        pos++;
                        // Skip whitespace after colon
                        while (pos < content.length() && std::isspace(content[pos])) {
                            pos++;
                        }
                        
                        // Extract value
                        std::string valueStr = extractYamlValue(content, pos);
                        set(key, valueStr, source);
                    }
                }
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::extractJsonValue(const std::string& json, size_t& pos) {
    if (pos >= json.length()) return "";
    
    char c = json[pos];
    
    if (c == '\"') {
        // String value
        pos++;
        size_t start = pos;
        while (pos < json.length() && json[pos] != '\"') {
            if (json[pos] == '\\' && pos + 1 < json.length()) {
                pos += 2; // Skip escaped character
            } else {
                pos++;
            }
        }
        std::string value = json.substr(start, pos - start);
        pos++; // Skip closing quote
        return value;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return "true";
        } else if (json.substr(pos, 5) == "false") {
            pos += 5;
            return "false";
        }
    } else if (c == 'n') {
        // Null value
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return "";
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number (integer or float)
        size_t start = pos;
        while (pos < json.length() && (json[pos] == '-' || json[pos] == '+' || 
                                       json[pos] == '.' || (json[pos] >= '0' && json[pos] <= '9') ||
                                       json[pos] == 'e' || json[pos] == 'E')) {
            pos++;
        }
        return json.substr(start, pos - start);
    } else if (c == '[') {
        // Array - for now, treat as string
        pos++;
        size_t start = pos;
        int bracketCount = 1;
        while (pos < json.length() && bracketCount > 0) {
            if (json[pos] == '[') bracketCount++;
            else if (json[pos] == ']') bracketCount--;
            pos++;
        }
        return json.substr(start, pos - start - 1);
    }
    
    return "";
}

std::string Config::extractYamlValue(const std::string& yaml, size_t& pos) {
    if (pos >= yaml.length()) return "";
    
    char c = yaml[pos];
    
    if (c == '\"') {
        // String value
        pos++;
        size_t start = pos;
        while (pos < yaml.length() && yaml[pos] != '\"') {
            pos++;
        }
        std::string value = yaml.substr(start, pos - start);
        pos++; // Skip closing quote
        return value;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (yaml.substr(pos, 4) == "true") {
            pos += 4;
            return "true";
        } else if (yaml.substr(pos, 5) == "false") {
            pos += 5;
            return "false";
        }
    } else if (c == 'n') {
        // Null value
        if (yaml.substr(pos, 4) == "null") {
            pos += 4;
            return "";
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number (integer or float)
        size_t start = pos;
        while (pos < yaml.length() && (yaml[pos] == '-' || yaml[pos] == '+' || 
                                       yaml[pos] == '.' || (yaml[pos] >= '0' && yaml[pos] <= '9') ||
                                       yaml[pos] == 'e' || yaml[pos] == 'E')) {
            pos++;
        }
        return yaml.substr(start, pos - start);
    }
    
    // For YAML, we'll handle multi-line values and lists differently
    // For now, return the rest of the line
    size_t start = pos;
    while (pos < yaml.length() && yaml[pos] != '\n') {
        pos++;
    }
    return yaml.substr(start, pos - start);
}

std::string Config::extractJsonKey(const std::string& json, size_t& pos) {
    if (pos >= json.length() || json[pos] != '\"') return "";
    
    pos++;
    size_t start = pos;
    while (pos < json.length() && json[pos] != '\"') {
        pos++;
    }
    std::string key = json.substr(start, pos - start);
    pos++; // Skip closing quote
    return key;
}

std::string Config::extractYamlKey(const std::string& yaml, size_t& pos) {
    if (pos >= yaml.length() || yaml[pos] != '\"') return "";
    
    pos++;
    size_t start = pos;
    while (pos < yaml.length() && yaml[pos] != '\"') {
        pos++;
    }
    std::string key = yaml.substr(start, pos - start);
    pos++; // Skip closing quote
    return key;
}

bool Config::parseJsonKeyValue(const std::string& json, const std::string& key, ConfigValue& value) {
    size_t pos = 0;
    while (pos < json.length()) {
        // Find next key
        size_t keyPos = json.find('\"' + key + '\"', pos);
        if (keyPos == std::string::npos) return false;
        
        // Move past the key
        keyPos += key.length() + 2; // +2 for quotes
        
        // Skip whitespace
        while (keyPos < json.length() && std::isspace(json[keyPos])) {
            keyPos++;
        }
        
        if (keyPos >= json.length() || json[keyPos] != ':') return false;
        
        // Move past colon
        keyPos++;
        
        // Skip whitespace
        while (keyPos < json.length() && std::isspace(json[keyPos])) {
            keyPos++;
        }
        
        // Extract value
        std::string valueStr = extractJsonValue(json, keyPos);
        
        // Convert to appropriate type
        if (valueStr.empty()) {
            value = std::string("");
        } else if (valueStr == "true" || valueStr == "false") {
            value = valueStr == "true";
        } else {
            // Try to parse as number
            try {
                size_t pos = 0;
                int intVal = std::stoi(valueStr, &pos);
                if (pos == valueStr.length()) {
                    value = intVal;
                } else {
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.length()) {
                        value = doubleVal;
                    } else {
                        value = valueStr;
                    }
                }
            } catch (...) {
                value = valueStr;
            }
        }
        
        return true;
    }
    return false;
}

bool Config::parseYamlKeyValue(const std::string& yaml, const std::string& key, ConfigValue& value) {
    size_t pos = 0;
    while (pos < yaml.length()) {
        // Find next key (simplified YAML parsing)
        size_t keyPos = yaml.find(key, pos);
        if (keyPos == std::string::npos) return false;
        
        // Check if it's actually a key (preceded by space and colon)
        if (keyPos > 0 && (yaml[keyPos - 1] == ' ' || yaml[keyPos - 1] == '\t') &&
            keyPos + key.length() < yaml.length() && yaml[keyPos + key.length()] == ':') {
            
            // Move past key and colon
            keyPos += key.length() + 1;
            
            // Skip whitespace after colon
            while (keyPos < yaml.length() && std::isspace(yaml[keyPos])) {
                keyPos++;
            }
            
            // Extract value
            std::string valueStr = extractYamlValue(yaml, keyPos);
            
            // Convert to appropriate type
            if (valueStr.empty()) {
                value = std::string("");
            } else if (valueStr == "true" || valueStr == "false") {
                value = valueStr == "true";
            } else {
                // Try to parse as number
                try {
                    size_t pos = 0;
                    int intVal = std::stoi(valueStr, &pos);
                    if (pos == valueStr.length()) {
                        value = intVal;
                    } else {
                        double doubleVal = std::stod(valueStr, &pos);
                        if (pos == valueStr.length()) {
                            value = doubleVal;
                        } else {
                            value = valueStr;
                        }
                    }
                } catch (...) {
                    value = valueStr;
                }
            }
            
            return true;
        }
        
        pos = keyPos + 1;
    }
    return false;
}
