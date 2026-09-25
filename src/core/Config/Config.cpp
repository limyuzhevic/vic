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
    // Try to parse as JSON first (preferred)
    try {
        return loadFromJSON(filepath);
    } catch (...) {
        // Fall back to simple key=value format
        NLM_LOG_INFO("Failed to parse " + filepath + " as JSON, trying simple format");
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file: " + filepath);
            return false;
        }
        
        std::string line;
        size_t lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
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
                
                // Validate key format (no spaces, reasonable length)
                if (key.empty() || key.find(' ') != std::string::npos || key.length() > 100) {
                    NLM_LOG_WARNING("Skipping invalid key on line " + std::to_string(lineNum) +
                                   ": '" + key + "'");
                    continue;
                }
                
                set(key, value, ConfigSource::File);
            } else if (!line.empty()) {
                NLM_LOG_WARNING("Skipping malformed line " + std::to_string(lineNum) +
                               ": '" + line + "'");
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot create config file: " + filepath);
        return false;
    }
    
    // Write in JSON format for better readability and future parsing
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": \"" << escapeJSONString(entry.value) << "\"";
        if (!entry.description.empty()) {
            file << " // " << entry.description;
        }
    }
    
    file << "\n}\n";
    
    NLM_LOG_INFO("Saved configuration to " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

std::string Config::escapeJSONString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32 || c == '\x7f') {
                    // Control character, use hex escape
                    char hex[10];
                    snprintf(hex, sizeof(hex), "\\u%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
        }
    }
    return result;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser for configuration files
    // Supports basic types: numbers, strings, booleans
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for JSON parsing: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Strip BOM if present
    if (content.size() >= 3 && content[0] == '\xEF' && content[1] == '\xBB' && content[2] == '\xBF') {
        content = content.substr(3);
    }
    
    // Parse key-value pairs
    size_t pos = 0;
    bool inObject = false;
    std::string currentKey;
    bool isFirstPair = true;
    
    auto trimWhitespace = [](const std::string& str) -> std::string {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        return str.substr(start, end - start + 1);
    };
    
    // Simple JSON parsing - handle object at top level
    while (pos < content.size()) {
        char c = content[pos];
        
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            // Skip whitespace
            pos++;
            continue;
        }
        
        if (c == '"') {
            // Parse quoted string as key
            size_t keyEnd = content.find('"', pos + 1);
            if (keyEnd == std::string::npos) {
                NLM_LOG_ERROR("Unterminated string literal in JSON config");
                return false;
            }
            
            currentKey = content.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace after key
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != ':') {
                NLM_LOG_ERROR("Missing ':' after key '" + currentKey + "' in JSON config");
                return false;
            }
            pos++;
            
            // Skip whitespace after colon
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }
            
            // Parse value
            std::string value;
            if (pos < content.size() && content[pos] == '"') {
                // String value
                size_t valueEnd = content.find('"', pos + 1);
                if (valueEnd == std::string::npos) {
                    NLM_LOG_ERROR("Unterminated string literal for value of key '" + currentKey + "'");
                    return false;
                }
                value = content.substr(pos + 1, valueEnd - pos - 1);
                pos = valueEnd + 1;
            } else if (pos < content.size() && (content[pos] == '-' || std::isdigit(content[pos]))) {
                // Number value (int or float)
                size_t valueEnd = pos;
                while (valueEnd < content.size() && (std::isdigit(content[valueEnd]) || content[valueEnd] == '.' || content[valueEnd] == '-')) {
                    valueEnd++;
                }
                value = content.substr(pos, valueEnd - pos);
                pos = valueEnd;
            } else if (pos < content.size() && (content[pos] == 't' || content[pos] == 'f')) {
                // Boolean value
                if (content.substr(pos, 4) == "true") {
                    value = "1";
                    pos += 4;
                } else if (content.substr(pos, 5) == "false") {
                    value = "0";
                    pos += 5;
                } else {
                    NLM_LOG_ERROR("Invalid boolean value for key '" + currentKey + "' in JSON config");
                    return false;
                }
            } else if (pos < content.size() && content[pos] == ',') {
                NLM_LOG_ERROR("Missing value for key '" + currentKey + "' in JSON config");
                return false;
            }
            
            if (!currentKey.empty() && !value.empty()) {
                set(currentKey, value, ConfigSource::File);
                currentKey.clear();
            }
            
            // Skip whitespace after value
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }
            
            if (pos < content.size() && content[pos] == ',') {
                pos++;
                // Skip whitespace after comma
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
                    pos++;
                }
            }
            
        } else if (c == '}') {
            // End of object
            pos++;
            break;
        } else if (c == '{') {
            // Start of object (for future support)
            pos++;
            // Skip to closing brace
            while (pos < content.size() && content[pos] != '}') {
                pos++;
            }
            if (pos < content.size()) {
                pos++;  // Skip closing brace
            }
        } else {
            pos++;
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries) using JSON parser");
    return true;
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
