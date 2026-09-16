#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace nlm {

// Forward declarations for JSON
struct json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Simple JSON parser implementation for configuration files
class SimpleJSONParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries) {
        // Simple JSON key-value parser - handle basic objects and arrays
        size_t pos = 0;
        while (pos < content.size()) {
            skipWhitespaceAndComments(content, pos);
            
            if (pos >= content.size()) break;
            
            if (content[pos] == '}') break;  // End of object
            
            // Parse key
            std::string key = parseString(content, pos);
            if (key.empty()) {
                // Try to parse unquoted key (simple case)
                key = parseUnquotedKey(content, pos);
            }
            
            skipWhitespaceAndComments(content, pos);
            
            if (pos >= content.size() || content[pos] != ':') {
                return false;  // Invalid format
            }
            
            pos++;  // Skip colon
            skipWhitespaceAndComments(content, pos);
            
            // Parse value
            ConfigValue value;
            if (content[pos] == '"' || content[pos] == '\'') {
                value = parseString(content, pos);
            } else if (content[pos] == '{') {
                // Object - skip for now
                skipObject(content, pos);
                continue;
            } else if (content[pos] == '[') {
                // Array - skip for now
                skipArray(content, pos);
                continue;
            } else if (content[pos] == '-' || std::isdigit(content[pos])) {
                value = parseNumber(content, pos);
            } else if (content[pos] == 't' || content[pos] == 'f') {
                value = parseBool(content, pos);
            } else if (content[pos] == 'n') {
                value = parseNull(content, pos);
            } else {
                return false;  // Unknown type
            }
            
            // Add entry with default source
            entries.emplace_back(key, value, ConfigSource::File);
            
            skipWhitespaceAndComments(content, pos);
            
            if (pos < content.size() && content[pos] == ',') {
                pos++;  // Skip comma
            }
        }
        
        return true;
    }
    
private:
    static void skipWhitespaceAndComments(const std::string& content, size_t& pos) {
        while (pos < content.size()) {
            char c = content[pos];
            if (std::isspace(c)) {
                pos++;
            } else if (c == '/' && pos + 1 < content.size() && content[pos + 1] == '/') {
                // Skip line comment
                size_t endLine = content.find('\n', pos);
                pos = (endLine == std::string::npos) ? content.size() : endLine + 1;
            } else if (c == '/' && pos + 1 < content.size() && content[pos + 1] == '*') {
                // Skip block comment
                size_t endComment = content.find("*/", pos);
                if (endComment == std::string::npos) return;
                pos = endComment + 2;
            } else {
                break;
            }
        }
    }
    
    static std::string parseString(const std::string& content, size_t& pos) {
        char quote = content[pos];
        pos++;  // Skip opening quote
        
        std::string result;
        while (pos < content.size() && content[pos] != quote) {
            if (content[pos] == '\\' && pos + 1 < content.size()) {
                // Handle escape sequences
                pos++;  // Skip backslash
                if (pos < content.size()) {
                    result += content[pos];
                    pos++;
                }
            } else {
                result += content[pos];
                pos++;
            }
        }
        
        if (pos < content.size() && content[pos] == quote) {
            pos++;  // Skip closing quote
        }
        
        return result;
    }
    
    static std::string parseUnquotedKey(const std::string& content, size_t& pos) {
        size_t start = pos;
        while (pos < content.size() && content[pos] != ':' && content[pos] != ',' && content[pos] != '}') {
            pos++;
        }
        
        return content.substr(start, pos - start);
    }
    
    static ConfigValue parseNumber(const std::string& content, size_t& pos) {
        size_t start = pos;
        if (content[pos] == '-') pos++;
        
        // Check for decimal point
        bool hasDecimal = false;
        while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.')) {
            if (content[pos] == '.') {
                if (hasDecimal) break;  // Second decimal point
                hasDecimal = true;
            }
            pos++;
        }
        
        std::string numStr = content.substr(start, pos - start);
        
        if (hasDecimal) {
            double value;
            try {
                value = std::stod(numStr);
                return value;
            } catch (...) {
                return 0.0;
            }
        } else {
            // Check if it's an integer (fits in int64_t)
            try {
                size_t end;
                int64_t value = std::stoll(numStr, &end);
                if (end == numStr.size()) return value;
            } catch (...) {}
            
            try {
                int value = std::stoi(numStr);
                return value;
            } catch (...) {}
        }
        
        return 0.0;
    }
    
    static ConfigValue parseBool(const std::string& content, size_t& pos) {
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
        return false;
    }
    
    static ConfigValue parseNull(const std::string& content, size_t& pos) {
        if (content.substr(pos, 4) == "null") {
            pos += 4;
            return std::string();  // Empty string for null
        }
        return std::string();
    }
    
    static void skipObject(const std::string& content, size_t& pos) {
        pos++;  // Skip {
        int braceCount = 1;
        while (pos < content.size() && braceCount > 0) {
            if (content[pos] == '{') braceCount++;
            else if (content[pos] == '}') braceCount--;
            pos++;
        }
    }
    
    static void skipArray(const std::string& content, size_t& pos) {
        pos++;  // Skip [
        int bracketCount = 1;
        while (pos < content.size() && bracketCount > 0) {
            if (content[pos] == '[') bracketCount++;
            else if (content[pos] == ']') bracketCount--;
            pos++;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try JSON parser first (modern format)
    pImpl->entries.clear();
    if (SimpleJSONParser::parse(content, pImpl->entries)) {
        NLM_LOG_INFO("Successfully parsed config file as JSON: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value parser (legacy format)
    pImpl->entries.clear();
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        try {
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
                NLM_LOG_INFO("Config entry from " + filepath + ": " + key + " = " + value);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Error parsing config line " + std::to_string(lineNumber) + " in " + filepath + ": " + e.what());
            // Continue parsing other lines
        }
    }
    
    NLM_LOG_INFO("Successfully parsed config file (legacy format): " + filepath);
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Write key
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << val[j] << "\"";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
    NLM_LOG_INFO("Config saved to JSON file: " + filepath);
    return true;
}

std::string Config::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << val;
        } else {
            oss << "null";
        }
    }, value);
    
    return oss.str();
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
