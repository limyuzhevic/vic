#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>

// Try to include nlohmann/json if available
try {
    #include <nlohmann/json.hpp>
    #define HAS_JSON true
} catch (...) {
    #define HAS_JSON false
}

// Try to include yaml-cpp if available
try {
    #include <yaml-cpp/yaml.h>
    #define HAS_YAML true
} catch (...) {
    #define HAS_YAML false
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
    // Detect file format by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Try JSON first (highest priority)
    if (extension == ".json") {
        return loadFromJSON(filepath);
    }
    // Try YAML next
    else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(filepath);
    }
    // Fall back to simple key=value format (backward compatibility)
    else {
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
        file << entry.key << " = " << serializeValue(entry.value) << "\n";
    }
    
    return true;
}

std::string Config::serializeValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_arithmetic_v<T>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[\"";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << "\", \";
                oss << arg[i];
            }
            oss << "\"]";
        } else {
            oss << "UNKNOWN_TYPE";
        }
    }, value);
    return oss.str();
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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// JSON/YAML parser implementations
bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to Config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Parse JSON value to ConfigValue
            ConfigValue value = parseJSONValue(it.value());
            
            // Try to get description from comments (if any)
            std::string description = "";
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
#else
    // JSON support not available
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Convert YAML to Config entries
        for (const auto& node : yamlData) {
            std::string key = node.first.as<std::string>();
            
            // Parse YAML value to ConfigValue
            ConfigValue value = parseYAMLValue(node.second);
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
#else
    // YAML support not available
    return false;
#endif
}

ConfigValue Config::parseJSONValue(const nlohmann::json& value) const {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_array()) {
        // Determine array type
        std::vector<std::string> stringArray;
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        
        for (const auto& item : value) {
            if (item.is_string()) {
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                intArray.push_back(item.get<int>());
            } else if (item.is_number_float()) {
                doubleArray.push_back(item.get<double>());
            }
        }
        
        // Try to determine best type
        if (!stringArray.empty()) {
            return stringArray;
        } else if (!intArray.empty()) {
            return intArray;
        } else if (!doubleArray.empty()) {
            return doubleArray;
        } if (value.is_null()) {
            return "NULL_VALUE";
        } else {
            return "UNKNOWN_JSON_VALUE";
        }
    } else {
        return "UNKNOWN_JSON_VALUE";
    }
}

ConfigValue Config::parseYAMLValue(const YAML::Node& value) const {
    if (value.IsScalar()) {
        std::string str = value.as<std::string>();
        
        // Try to convert to appropriate type
        if (stringToBool(str)) {
            return stringToBool(str);
        } else if (auto num = stringToNumber<int>(str)) {
            return *num;
        } else if (auto num = stringToNumber<double>(str)) {
            return *num;
        } else if (auto vec = stringToVectorInt(str)) {
            return *vec;
        } else if (auto vec = stringToVectorDouble(str)) {
            return *vec;
        } else if (auto vec = stringToVectorString(str)) {
            return *vec;
        } else {
            return str;
        }
    } else if (value.IsSequence()) {
        std::vector<std::string> stringArray;
        
        for (const auto& item : value) {
            if (item.IsScalar()) {
                stringArray.push_back(item.as<std::string>());
            }
        }
        
        return stringArray;
    } else {
        return "UNKNOWN_YAML_VALUE";
    }
}

// Additional helper method implementations

// Parse string to configuration value
ConfigValue Config::parseValue(const std::string& valueStr, const std::string& key) const {
    // Remove quotes first
    std::string str = valueStr;
    if (str.size() >= 2 && ((str.front() == '\"' && str.back() == '\"') || 
                           (str.front() == '\'' && str.back() == '\''))) {
        str = str.substr(1, str.size() - 2);
    }
    
    // Try different types in order of preference
    if (stringToBool(str)) {
        return stringToBool(str);
    } else if (auto num = stringToNumber<int>(str)) {
        return *num;
    } else if (auto num = stringToNumber<double>(str)) {
        return *num;
    } else if (auto vec = stringToVectorInt(str)) {
        return *vec;
    } else if (auto vec = stringToVectorDouble(str)) {
        return *vec;
    } else if (auto vec = stringToVectorString(str)) {
        return *vec;
    } else {
        return str; // Default to string
    }
}

// Convert string to boolean
bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "yes" || lower == "1" || lower == "on";
}

// Convert string to number - template specializations
template<typename T>
std::optional<T> Config::stringToNumber(const std::string& str) {
    return std::nullopt;
}

// Specializations for common numeric types
template<> std::optional<int> Config::stringToNumber<int>(const std::string& str) {
    try {
        size_t pos;
        int value = std::stoi(str, &pos);
        if (pos == str.size()) {
            return value;
        }
    } catch (...) {}
    return std::nullopt;
}

template<> std::optional<double> Config::stringToNumber<double>(const std::string& str) {
    try {
        size_t pos;
        double value = std::stod(str, &pos);
        if (pos == str.size()) {
            return value;
        }
    } catch (...) {}
    return std::nullopt;
}

// Convert string to vector
std::optional<std::vector<int>> Config::stringToVectorInt(const std::string& str) {
    if (str.empty()) return std::nullopt;
    
    std::vector<int> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        try {
            result.push_back(std::stoi(token));
        } catch (...) {
            return std::nullopt;
        }
    }
    return result;
}

std::optional<std::vector<double>> Config::stringToVectorDouble(const std::string& str) {
    if (str.empty()) return std::nullopt;
    
    std::vector<double> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        try {
            result.push_back(std::stod(token));
        } catch (...) {
            return std::nullopt;
        }
    }
    return result;
}

std::optional<std::vector<std::string>> Config::stringToVectorString(const std::string& str) {
    if (str.empty()) return std::nullopt;
    
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.empty()) continue;
        
        // Remove quotes from each element
        if (token.size() >= 2 && ((token.front() == '\"' && token.back() == '\"') || 
                                 (token.front() == '\'' && token.back() == '\''))) {
            token = token.substr(1, token.size() - 2);
        }
        
        result.push_back(token);
    }
    return result;
}

// File format detection
std::string Config::getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    return path.extension().string();
}

bool Config::isTextFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first few bytes and check for binary content
    char buffer[1024];
    file.read(buffer, sizeof(buffer));
    size_t bytesRead = file.gcount();
    
    // Check for null bytes which indicate binary files
    for (size_t i = 0; i < bytesRead; ++i) {
        if (buffer[i] == '\0') {
            return false;
        }
    }
    
    return true;
}

} // namespace nlm
