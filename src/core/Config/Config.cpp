#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loader: supports both simple key=value format and JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect if file is JSON by reading first few characters
    file.seekg(0, std::ios::beg);
    char firstChar = file.get();
    if (file.eof()) {
        return false; // Empty file
    }
    
    bool isJson = false;
    if (firstChar == '{') {
        isJson = true;
    } else {
        // Check for JSON-like structure with whitespace
        file.seekg(0, std::ios::beg);
        std::string firstLine;
        std::getline(file, firstLine);
        firstLine = trim(firstLine);
        if (firstLine.size() >= 2 && firstLine[0] == '{' && firstLine.back() == '}') {
            isJson = true;
        }
    }
    
    // Reset file position
    file.clear();
    file.seekg(0, std::ios::beg);
    
    if (isJson) {
        return loadFromJsonFile(filepath);
    } else {
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value parser
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

bool Config::loadFromJsonFile(const std::string& filepath) {
    // TODO: Implement proper JSON parser using nlohmann/json when available
    // For now, implement a minimal JSON parser that handles basic types
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - could be enhanced with proper JSON library
    try {
        parseJsonContent(content);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void Config::parseJsonContent(const std::string& json) {
    // Enhanced JSON parser implementation
    // This is a simplified parser that could be replaced with nlohmann/json
    // for production use
    
    // Remove whitespace for parsing
    std::string cleanJson;
    for (char c : json) {
        if (!std::isspace(c)) {
            cleanJson += c;
        }
    }
    
    if (cleanJson.empty() || cleanJson[0] != '{') {
        throw std::runtime_error("Invalid JSON format");
    }
    
    // Find top-level key-value pairs (simplified approach)
    size_t pos = 1;  // Skip opening brace
    while (pos < cleanJson.size()) {
        // Find next key
        size_t keyStart = cleanJson.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleanJson.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleanJson.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 2;  // Skip ":"
        
        // Find value
        auto [value, nextPos] = parseJsonValue(cleanJson, pos);
        set(key, value, ConfigSource::File);
        
        pos = nextPos;
        if (pos < cleanJson.size() && cleanJson[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < cleanJson.size() && cleanJson[pos] == '}') {
            break; // End of object
        }
    }
}

std::pair<ConfigValue, size_t> Config::parseJsonValue(const std::string& json, size_t startPos) {
    // Parse JSON value and return pair of value and position after value
    if (startPos >= json.size()) {
        throw std::runtime_error("Unexpected end of JSON");
    }
    
    char firstChar = json[startPos];
    
    if (firstChar == '"') {
        // String value
        size_t endPos = json.find('"', startPos + 1);
        if (endPos == std::string::npos) {
            throw std::runtime_error("Unterminated string in JSON");
        }
        std::string value = json.substr(startPos + 1, endPos - startPos - 1);
        return {value, endPos + 1};
    } else if (firstChar == '{') {
        // Object - for now, just skip it
        size_t endPos = json.find('}', startPos);
        if (endPos == std::string::npos) {
            throw std::runtime_error("Unterminated object in JSON");
        }
        return {std::string("[object]"), endPos + 1};
    } else if (firstChar == '[') {
        // Array - for now, parse as comma-separated values
        return parseJsonArray(json, startPos);
    } else if (firstChar == 't' || firstChar == 'f') {
        // Boolean value
        size_t endPos = json.find_first_of(",}", startPos);
        if (endPos == std::string::npos) endPos = json.size();
        std::string boolStr = json.substr(startPos, endPos - startPos);
        bool value = (boolStr == "true");
        return {value, endPos};
    } else {
        // Number
        size_t endPos = json.find_first_of(",}", startPos);
        if (endPos == std::string::npos) endPos = json.size();
        std::string numStr = json.substr(startPos, endPos - startPos);
        
        // Try to parse as int64_t first, then double
        try {
            size_t pos;
            int64_t intVal = std::stoll(numStr, &pos);
            if (pos == numStr.size()) {
                return {intVal, endPos};
            }
        } catch (...) {}
        
        try {
            size_t pos;
            double doubleVal = std::stod(numStr, &pos);
            if (pos == numStr.size()) {
                return {doubleVal, endPos};
            }
        } catch (...) {}
        
        // If parsing failed, return as string
        return {numStr, endPos};
    }
}

std::pair<ConfigValue, size_t> Config::parseJsonArray(const std::string& json, size_t startPos) {
    // Parse JSON array
    std::vector<ConfigValue> array;
    size_t pos = startPos + 1;  // Skip opening bracket
    
    while (pos < json.size() && json[pos] != ']') {
        auto [value, nextPos] = parseJsonValue(json, pos);
        array.push_back(value);
        pos = nextPos;
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < json.size() && json[pos] == ']') {
            break;
        }
    }
    
    if (pos >= json.size() || json[pos] != ']') {
        throw std::runtime_error("Unterminated array in JSON");
    }
    
    return {array, pos + 1};
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
        file << entry.key << " = ";
        
        // Write the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and write with quotes
                file << "\"" << escapeString(arg) << "\"";
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
                    file << std::fixed << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << escapeString(arg[i]) << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        file << "\n";
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

// Escape quotes and special characters for safe string representation
std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\0"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control characters - escape as octal
                    result += "\\" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    return result;
}

// Reverse escape string (for future parsing enhancement)
std::string Config::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; i++; break;
                case '\"': result += '\"'; i++; break;
                case '0': result += '\0'; i++; break;
                case '\n': result += '\n'; i++; break;
                case '\r': result += '\r'; i++; break;
                case '\t': result += '\t'; i++; break;
                default:
                    // Try to parse as octal for control characters
                    if (i + 2 < str.size() && std::isdigit(str[i + 1]) && 
                        std::isdigit(str[i + 2])) {
                        int octal = (str[i + 1] - '0') * 64 + (str[i + 2] - '0');
                        if (octal < 256) {
                            result += static_cast<char>(octal);
                            i += 2;
                            break;
                        }
                    }
                    // If we can't parse, just keep the escape sequence
                    result += str[i];
            }
        } else {
            result += str[i];
        }
    }
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
