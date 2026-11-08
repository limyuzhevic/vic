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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <stdexcept>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Modern configuration file parsing with enhanced error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Enhanced parsing with better error handling
        try {
            parseLine(line, lineNum, ConfigSource::File);
        } catch (const std::exception& e) {
            // Log parsing error but continue with other lines
            // In production, you'd use a proper logger here
            std::cerr << "Config parsing error at line " << lineNum << ": " << e.what() << std::endl;
            continue;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced command-line argument parsing
    if (!argv) return false;
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) continue;
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                std::cerr << "Warning: Invalid command-line argument format: " << arg << std::endl;
                success = false;
            }
        }n        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            std::cerr << "Warning: Invalid command-line argument: " << arg << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Enhanced file saving with better error handling
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Enhanced value serialization with proper quoting
        switch (entry.value.index()) {
            case 0: { // int
                file << std::get<int>(entry.value);
                break;
            }
            case 1: { // int64_t
                file << std::get<int64_t>(entry.value);
                break;
            }
            case 2: { // double
                file << std::get<double>(entry.value);
                break;
            }
            case 3: { // bool
                file << (std::get<bool>(entry.value) ? "true" : "false");
                break;
            }
            case 4: { // std::string
                file << "\"" << std::get<std::string>(entry.value) << "\"";
                break;
            }
            case 5: { // std::vector<int>
                const auto& vec = std::get<std::vector<int>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case 6: { // std::vector<double>
                const auto& vec = std::get<std::vector<double>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
                break;
            }
            case 7: { // std::vector<std::string>
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << vec[i] << "\"";
                }
                file << "]";
                break;
            }
            default:
                file << "UNKNOWN_VALUE";
        }
        
        file << "\n\n";
    }
    
    file.flush();
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKey(key);
    validateValue(value, key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "");
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
    validateKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    validateKey(key);
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
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
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
    if (str.empty()) return str;
    
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

void Config::parseLine(const std::string& line, int lineNum, ConfigSource source) {
    // Enhanced parsing with validation
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid format: expected 'key = value'");
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string valueStr = trim(line.substr(pos + 1));
    
    if (key.empty()) {
        throw std::runtime_error("Empty key name");
    }
    
    validateKey(key);
    
    ConfigValue value = parseValue(valueStr);
    set(key, value, source);
}

ConfigValue Config::parseValue(const std::string& valueStr) {
    // Enhanced value parsing with type detection
    std::string trimmed = trim(valueStr);
    
    // Boolean parsing
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    // Integer parsing
    if (std::regex_match(trimmed, std::regex("^[+-]?\d+$"))) {
        try {
            return std::stoll(trimmed);
        } catch (const std::exception&) {
            // Fallback to double
        }
    }
    
    // Double parsing
    if (std::regex_match(trimmed, std::regex("^[+-]?\d*\.\d+$|^[+-]?\d+$"))) {
        try {
            return std::stod(trimmed);
        } catch (const std::exception&) {
            // Try string
        }
    }
    
    // String parsing (remove quotes)
    if (trimmed.size() >= 2 && ((trimmed.front() == '"' && trimmed.back() == '"') ||
                                (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        return trimmed.substr(1, trimmed.size() - 2);
    }
    
    // Array parsing (simplified)
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::string inner = trimmed.substr(1, trimmed.size() - 2);
        return parseArray(inner);
    }
    
    // Default to string
    return trimmed;
}

ConfigValue Config::parseArray(const std::string& arrayStr) {
    // Simplified array parsing - could be enhanced with proper JSON parsing
    std::vector<ConfigValue> values;
    
    if (arrayStr.empty()) {
        return values;
    }
    
    size_t pos = 0;
    size_t commaPos;
    std::string token;
    
    while ((commaPos = arrayStr.find(',', pos)) != std::string::npos) {
        token = trim(arrayStr.substr(pos, commaPos - pos));
        values.push_back(parseValue(token));
        pos = commaPos + 1;
    }
    
    token = trim(arrayStr.substr(pos));
    values.push_back(parseValue(token));
    
    // Determine array type based on contents
    if (values.empty()) {
        return std::vector<int>{};
    }
    
    // Check if all elements are int
    bool allInt = std::all_of(values.begin(), values.end(),
        [](const ConfigValue& v) { return std::holds_alternative<int>(v); });
    if (allInt) {
        std::vector<int> intVec;
        for (const auto& v : values) {
            intVec.push_back(std::get<int>(v));
        }
        return intVec;
    }
    
    // Check if all elements are double
    bool allDouble = std::all_of(values.begin(), values.end(),
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) || std::holds_alternative<int>(v); });
    if (allDouble) {
        std::vector<double> doubleVec;
        for (const auto& v : values) {
            if (std::holds_alternative<double>(v)) {
                doubleVec.push_back(std::get<double>(v));
            } else {
                doubleVec.push_back(static_cast<double>(std::get<int>(v)));
            }
        }
        return doubleVec;
    }
    
    // Check if all elements are string
    bool allString = std::all_of(values.begin(), values.end(),
        [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); });
    if (allString) {
        std::vector<std::string> stringVec;
        for (const auto& v : values) {
            stringVec.push_back(std::get<std::string>(v));
        }
        return stringVec;
    }
    
    // Mixed types - return as vector of generic ConfigValue
    return values;
}

void Config::validateKey(const std::string& key) {
    if (key.empty()) {
        throw std::runtime_error("Key cannot be empty");
    }
    
    // Key should only contain valid characters
    if (!std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; })) {
        throw std::runtime_error("Key contains invalid characters: " + key);
    }
}

void Config::validateValue(const ConfigValue& value, const std::string& key) {
    // Validate based on value type
    std::visit([this, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            // String validation
            if (arg.empty()) {
                throw std::runtime_error("String value for key '" + key + "' cannot be empty");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Vector validation
            if (arg.size() > 1000) {
                throw std::runtime_error("Vector size for key '" + key + "' exceeds maximum limit");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Vector validation
            if (arg.size() > 1000) {
                throw std::runtime_error("Vector size for key '" + key + "' exceeds maximum limit");
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Vector validation
            if (arg.size() > 1000) {
                throw std::runtime_error("Vector size for key '" + key + "' exceeds maximum limit");
            }
            for (const auto& str : arg) {
                if (str.empty()) {
                    throw std::runtime_error("String in vector for key '" + key + "' cannot be empty");
                }
            }
        }
    }, value);
}

} // namespace nlm

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

} // namespace nlm
