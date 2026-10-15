#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <stack>
#include <locale>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (file ending with .json or .cfg)
    if (filepath.size() >= 5 && (filepath.substr(filepath.size() - 5) == ".json" || 
                                 filepath.substr(filepath.size() - 4) == ".cfg")) {
        return loadFromJSON(filepath);
    }
    
    // Fallback to simple key=value format for backward compatibility
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

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    return loadFromJSON(jsonContent);
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        
        // Expect JSON object { ... }
        if (jsonStr[pos] != '{') {
            return false;
        }
        
        pos++; // Skip '{'
        skipWhitespace(jsonStr, pos);
        
        while (pos < jsonStr.size() && jsonStr[pos] != '}') {
            // Parse key
            std::string key;
            if (!parseJSONString(jsonStr, pos, key)) {
                return false;
            }
            
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false;
            }
            
            pos++; // Skip ':'
            skipWhitespace(jsonStr, pos);
            
            // Parse value
            ConfigValue value;
            if (!parseJSONValue(jsonStr, pos, value)) {
                return false;
            }
            
            set(key, value, ConfigSource::File);
            
            skipWhitespace(jsonStr, pos);
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(jsonStr, pos);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
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
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double>) {
                file << arg;
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
    // Validate the value if schema is set
    ValidationResult validation = configSchema.validate(key, value);
    if (!validation.isValid) {
        throw std::runtime_error("Configuration validation failed for key \"" + key + "\": " + validation.errorMessage);
    }
    
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    std::string errors;
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = configSchema.validate(entry.key, entry.value);
        if (!result.isValid) {
            if (!errors.empty()) errors += "\n";
            errors += "Key \"" + entry.key + "\": " + result.errorMessage;
        }
    }
    
    if (errors.empty()) {
        return ValidationResult::success();
    } else {
        return ValidationResult(false, errors);
    }
}

ValidationResult Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult(false, "Key \"" + key + "\" not found");
    }
    
    return configSchema.validate(key, it->value);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// JSON parsing implementation
bool Config::parseJSONValue(const std::string& json, size_t& pos, ConfigValue& outValue) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return false;
    }
    
    char ch = json[pos];
    
    if (ch == '{') {
        // Object - skip for now (we only parse key-value pairs at top level)
        return false;
    } else if (ch == '[') {
        return parseJSONArray(json, pos, outValue);
    } else if (ch == '"') {
        return parseJSONString(json, pos, std::get<std::string>(outValue));
    } else if (ch == 't' || ch == 'f') {
        return parseJSONBoolean(json, pos, outValue);
    } else if (std::isdigit(ch) || ch == '-') {
        return parseJSONNumber(json, pos, outValue);
    } else {
        return false;
    }
}

bool Config::parseJSONString(const std::string& json, size_t& pos, std::string& outString) {
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            // Skip escaped character
            pos += 2;
        } else {
            pos++;
        }
    }
    
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    
    outString = json.substr(start, pos - start);
    pos++; // Skip closing quote
    return true;
}

bool Config::parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& outValue) {
    size_t start = pos;
    
    if (json[pos] == '-') {
        pos++;
    }
    
    bool hasDecimal = false;
    bool hasExponent = false;
    
    while (pos < json.size() && (std::isdigit(json[pos]) || 
                                 json[pos] == '.' || 
                                 (json[pos] == 'e' || json[pos] == 'E') ||
                                 (json[pos] == '+' || json[pos] == '-'))) {
        if (json[pos] == '.') {
            hasDecimal = true;
        } else if (json[pos] == 'e' || json[pos] == 'E') {
            hasExponent = true;
        }
        pos++;
    }
    
    std::string numStr = json.substr(start, pos - start);
    
    try {
        if (numStr.find('.') != std::string::npos || hasDecimal || hasExponent) {
            // Float/double
            double val = std::stod(numStr);
            outValue = val;
        } else {
            // Integer - try to determine size
            size_t digits = numStr.find_first_of(".eE");
            if (digits != std::string::npos) {
                numStr = numStr.substr(0, digits);
            }
            
            // Try int64 first, then int
            try {
                int64_t val = std::stoll(numStr);
                outValue = val;
            } catch (...) {
                int val = std::stoi(numStr);
                outValue = val;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& outValue) {
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
        outValue = true;
        pos += 4;
        return true;
    } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
        outValue = false;
        pos += 5;
        return true;
    }
    return false;
}

bool Config::parseJSONArray(const std::string& json, size_t& pos, ConfigValue& outValue) {
    if (pos >= json.size() || json[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    skipWhitespace(json, pos);
    
    // For now, only support arrays of numbers and strings
    // Try to parse as array of integers first
    std::vector<int> intArray;
    std::vector<double> doubleArray;
    std::vector<std::string> stringArray;
    
    bool hasInts = true;
    bool hasDoubles = true;
    bool hasStrings = true;
    
    while (pos < json.size() && json[pos] != ']') {
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            break;
        }
        
        // Try to parse value
        ConfigValue value;
        if (parseJSONValue(json, pos, value)) {
            // Try to categorize the value
            try {
                int intVal = std::get<int>(value);
                intArray.push_back(intVal);
            } catch (const std::bad_variant_access&) {
                hasInts = false;
                try {
                    double doubleVal = std::get<double>(value);
                    doubleArray.push_back(doubleVal);
                } catch (const std::bad_variant_access&) {
                    hasDoubles = false;
                    try {
                        std::string stringVal = std::get<std::string>(value);
                        stringArray.push_back(stringVal);
                    } catch (const std::bad_variant_access&) {
                        hasStrings = false;
                    }
                }
            }
        }
        
        skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(json, pos);
        }
    }
    
    if (pos >= json.size() || json[pos] != ']') {
        return false;
    }
    
    pos++; // Skip ']'
    
    // Choose appropriate array type
    if (hasInts && !intArray.empty()) {
        outValue = intArray;
    } else if (hasDoubles && !doubleArray.empty()) {
        outValue = doubleArray;
    } else if (hasStrings && !stringArray.empty()) {
        outValue = stringArray;
    } else {
        return false;
    }
    
    return true;
}

void Config::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
    }
}

char Config::peekChar(const std::string& json, size_t pos) {
    if (pos < json.size()) {
        return json[pos];
    }
    return '\0';
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
