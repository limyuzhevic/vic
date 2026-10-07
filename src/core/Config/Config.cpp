#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cstddef>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first
    if (parseJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    clear();
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
    
    // Save in JSON format for better readability and future parsing
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> ||
                                  std::is_same_v<T, int> ||
                                  std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            } else {
                file << "null"; // For unknown types
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
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

// JSON parsing methods
bool Config::parseJSON(const std::string& jsonStr) {
    size_t pos = 0;
    if (!skipWhitespace(jsonStr, pos)) return false;
    
    if (jsonStr[pos] != '{') {
        return false; // Not a JSON object
    }
    
    return parseJSONObject(jsonStr, pos);
}

bool Config::parseJSONObject(const std::string& jsonStr, size_t& pos) {
    size_t startPos = pos;
    
    if (!skipWhitespace(jsonStr, pos) || jsonStr[pos] != '{') {
        return false;
    }
    pos++; // Skip '{'
    
    bool firstPair = true;
    while (pos < jsonStr.length()) {
        if (!skipWhitespace(jsonStr, pos)) break;
        
        if (jsonStr[pos] == '}') {
            pos++; // Skip '}'
            return true;
        }
        
        if (!firstPair) {
            if (jsonStr[pos] != ',') {
                return false;
            }
            pos++; // Skip ','
        }
        firstPair = false;
        
        if (!skipWhitespace(jsonStr, pos)) break;
        
        // Parse key
        std::string key;
        size_t keyEnd;
        if (jsonStr[pos] != '"') {
            return false; // Expected quoted string
        }
        
        key = extractString(jsonStr, pos + 1, keyEnd);
        if (key.empty()) {
            return false;
        }
        pos = keyEnd;
        
        if (!skipWhitespace(jsonStr, pos) || jsonStr[pos] != ':') {
            return false;
        }
        pos++; // Skip ':'
        
        if (!skipWhitespace(jsonStr, pos)) break;
        
        // Parse value
        ConfigValue value;
        bool valueParsed = false;
        
        if (jsonStr[pos] == '"') {
            valueParsed = parseString(jsonStr, pos, value);
        } else if (jsonStr[pos] >= '0' && jsonStr[pos] <= '9' || jsonStr[pos] == '-') {
            valueParsed = parseNumber(jsonStr, pos, value);
        } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
            bool boolVal;
            if (parseBool(jsonStr, pos, boolVal)) {
                value = boolVal;
                valueParsed = true;
            }
        } else if (jsonStr[pos] == 'n') {
            if (parseNull(jsonStr, pos)) {
                value = std::string("null");
                valueParsed = true;
            }
        } else if (jsonStr[pos] == '[') {
            valueParsed = parseArray(jsonStr, pos, value);
        } else {
            return false; // Unsupported value type
        }
        
        if (!valueParsed) {
            return false;
        }
        
        set(key, value, ConfigSource::File);
        
        if (!skipWhitespace(jsonStr, pos)) break;
    }
    
    return false; // Reached end without finding '}'
}

bool Config::parseArray(const std::string& jsonStr, size_t& pos, ConfigValue& outValue) {
    if (!skipWhitespace(jsonStr, pos) || jsonStr[pos] != '[') {
        return false;
    }
    pos++; // Skip '['
    
    std::vector<double> doubleArray;
    std::vector<int> intArray;
    std::vector<std::string> stringArray;
    
    bool firstElement = true;
    while (pos < jsonStr.length()) {
        if (!skipWhitespace(jsonStr, pos)) break;
        
        if (jsonStr[pos] == ']') {
            pos++; // Skip ']'
            
            // Choose the best array type
            if (!stringArray.empty()) {
                outValue = stringArray;
            } else if (!intArray.empty()) {
                outValue = intArray;
            } else {
                outValue = doubleArray;
            }
            return true;
        }
        
        if (!firstElement) {
            if (jsonStr[pos] != ',') {
                return false;
            }
            pos++; // Skip ','
        }
        firstElement = false;
        
        if (!skipWhitespace(jsonStr, pos)) break;
        
        // Parse element
        ConfigValue elementValue;
        if (jsonStr[pos] == '"') {
            std::string strVal;
            if (!parseString(jsonStr, pos, strVal)) {
                return false;
            }
            stringArray.push_back(strVal);
        } else if (jsonStr[pos] >= '0' && jsonStr[pos] <= '9' || jsonStr[pos] == '-') {
            double numVal;
            if (parseNumberAsDouble(jsonStr, pos, numVal)) {
                // Try to store as int if it's an integer
                if (std::abs(numVal - static_cast<int>(numVal)) < 0.0001) {
                    intArray.push_back(static_cast<int>(numVal));
                } else {
                    doubleArray.push_back(numVal);
                }
            } else {
                return false;
            }
        } else {
            return false; // Unsupported element type
        }
    }
    
    return false;
}

bool Config::parseString(const std::string& jsonStr, size_t& pos, std::string& outValue) {
    if (!skipWhitespace(jsonStr, pos) || jsonStr[pos] != '"') {
        return false;
    }
    pos++; // Skip opening '"'
    
    size_t startPos = pos;
    while (pos < jsonStr.length() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.length()) {
            // Handle escape sequences
            pos += 2; // Skip escaped character
        } else {
            pos++;
        }
    }
    
    if (pos >= jsonStr.length() || jsonStr[pos] != '"') {
        return false; // Unterminated string
    }
    
    outValue = jsonStr.substr(startPos, pos - startPos);
    pos++; // Skip closing '"'
    return true;
}

bool Config::parseNumber(const std::string& jsonStr, size_t& pos, ConfigValue& outValue) {
    size_t startPos = pos;
    bool isDouble = false;
    
    if (jsonStr[pos] == '-') {
        pos++;
    }
    
    while (pos < jsonStr.length() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '.')) {
        if (jsonStr[pos] == '.') {
            isDouble = true;
        }
        pos++;
    }
    
    if (pos == startPos) {
        return false; // No digits
    }
    
    std::string numStr = jsonStr.substr(startPos, pos - startPos);
    
    try {
        if (isDouble) {
            double val = std::stod(numStr);
            outValue = val;
        } else {
            // Try to parse as int first, then as int64_t
            if (numStr.find('.') == std::string::npos) {
                try {
                    int64_t intVal = std::stoll(numStr);
                    outValue = intVal;
                } catch (...) {
                    double val = std::stod(numStr);
                    outValue = val;
                }
            } else {
                double val = std::stod(numStr);
                outValue = val;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseNumberAsDouble(const std::string& jsonStr, size_t& pos, double& outValue) {
    size_t startPos = pos;
    bool isDouble = false;
    
    if (jsonStr[pos] == '-') {
        pos++;
    }
    
    while (pos < jsonStr.length() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '.')) {
        if (jsonStr[pos] == '.') {
            isDouble = true;
        }
        pos++;
    }
    
    if (pos == startPos) {
        return false; // No digits
    }
    
    std::string numStr = jsonStr.substr(startPos, pos - startPos);
    
    try {
        outValue = std::stod(numStr);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseBool(const std::string& jsonStr, size_t& pos, bool& outValue) {
    if (!skipWhitespace(jsonStr, pos)) return false;
    
    if (jsonStr.substr(pos, 4) == "true") {
        outValue = true;
        pos += 4;
        return true;
    } else if (jsonStr.substr(pos, 5) == "false") {
        outValue = false;
        pos += 5;
        return true;
    }
    
    return false;
}

bool Config::parseNull(const std::string& jsonStr, size_t& pos) {
    if (!skipWhitespace(jsonStr, pos)) return false;
    
    if (jsonStr.substr(pos, 4) == "null") {
        pos += 4;
        return true;
    }
    
    return false;
}

bool Config::skipWhitespace(const std::string& jsonStr, size_t& pos) {
    while (pos < jsonStr.length() && std::isspace(jsonStr[pos])) {
        pos++;
    }
    return pos < jsonStr.length();
}

std::string Config::extractString(const std::string& jsonStr, size_t startPos, size_t& endPos) {
    size_t pos = startPos;
    std::string result;
    
    while (pos < jsonStr.length() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.length()) {
            // Handle escape sequences
            result += jsonStr[pos + 1];
            pos += 2;
        } else {
            result += jsonStr[pos];
            pos++;
        }
    }
    
    endPos = pos;
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
