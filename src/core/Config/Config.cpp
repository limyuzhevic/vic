#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <limits>
#include <cctype>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace {

// Helper functions for type conversion and validation

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Validates that a string can be converted to an integer within range
template<typename T>
bool isValidInteger(const std::string& str, T& outValue, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
    static_assert(std::is_integral<T>::value, "T must be integral");
    
    // Trim whitespace
    std::string trimmed = trim(str);
    
    // Check for empty string
    if (trimmed.empty()) {
        return false;
    }
    
    // Check for sign
    bool negative = false;
    size_t start = 0;
    if (trimmed[0] == '-') {
        negative = true;
        start = 1;
    } else if (trimmed[0] == '+') {
        start = 1;
    }
    
    // Ensure all characters are digits
    for (size_t i = start; i < trimmed.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(trimmed[i]))) {
            return false;
        }
    }
    
    try {
        // Convert to integer
        size_t pos;
        T value;
        if constexpr (std::is_same_v<T, int64_t>) {
            value = std::stoll(trimmed);
        } else if constexpr (std::is_same_v<T, long>) {
            value = std::stoll(trimmed);
        } else if constexpr (std::is_same_v<T, long long>) {
            value = std::stoll(trimmed);
        } else if constexpr (std::is_same_v<T, int>) {
            value = std::stoi(trimmed);
        } else if constexpr (std::is_same_v<T, short>) {
            value = std::stoi(trimmed);
        } else if constexpr (std::is_same_v<T, signed char>) {
            value = std::stoi(trimmed);
        } else {
            value = std::stoi(trimmed);
        }
        
        // Check range
        if (negative && -value < min) return false;
        if (!negative && value > max) return false;
        
        outValue = value;
        return true;
    } catch (const std::out_of_range&) {
        return false;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

// Validates that a string can be converted to a double within range
template<typename T>
bool isValidDouble(const std::string& str, T& outValue, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    
    // Trim whitespace
    std::string trimmed = trim(str);
    
    // Check for empty string
    if (trimmed.empty()) {
        return false;
    }
    
    // Check for sign
    bool negative = false;
    size_t start = 0;
    if (trimmed[0] == '-') {
        negative = true;
        start = 1;
    } else if (trimmed[0] == '+') {
        start = 1;
    }
    
    // Check for NaN or Inf
    if (trimmed == "nan" || trimmed == "inf" || trimmed == "infinity" || 
        trimmed == "nan" || trimmed == "inf") {
        return false;
    }
    
    try {
        // Convert to double
        size_t pos;
        double value = std::stod(trimmed, &pos);
        
        // Check if we consumed the entire string (excluding trailing whitespace)
        if (pos != trimmed.length()) {
            return false;
        }
        
        // Check range
        if (negative && -value < min) return false;
        if (!negative && value > max) return false;
        
        outValue = static_cast<T>(value);
        return true;
    } catch (const std::out_of_range&) {
        return false;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

// Validates that a string can be converted to a boolean
bool isValidBoolean(const std::string& str) {
    std::string trimmed = trim(toLower(str));
    return (trimmed == "true" || trimmed == "false" || trimmed == "1" || trimmed == "0");
}

// Converts string to boolean
bool stringToBool(const std::string& str) {
    std::string trimmed = trim(toLower(str));
    if (trimmed == "true" || trimmed == "1") return true;
    if (trimmed == "false" || trimmed == "0") return false;
    throw ConfigParseException("Invalid boolean value: '" + str + "'");
}

// Converts string to integer with type conversion and range checking
template<typename T>
T stringToInteger(const std::string& str, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
    static_assert(std::is_integral<T>::value, "T must be integral");
    
    T value;
    if (!isValidInteger(str, value, min, max)) {
        throw ConfigParseException("Invalid integer value: '" + str + "' for type " + std::string(typeid(T).name()));
    }
    return value;
}

// Converts string to double with type conversion and range checking
template<typename T>
T stringToDouble(const std::string& str, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
    static_assert(std::is_floating_point<T>::value, "T must be floating point");
    
    T value;
    if (!isValidDouble(str, value, min, max)) {
        throw ConfigParseException("Invalid double value: '" + str + "' for type " + std::string(typeid(T).name()));
    }
    return value;
}

// Validates key format
bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    
    // Allow alphanumeric, underscores, hyphens, and dots
    std::regex keyPattern("^[a-zA-Z0-9_.-]+$");
    return std::regex_match(key, keyPattern);
}

// Validates file path exists and is readable
bool validateFilePath(const std::string& filepath) {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath) &&
           std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read;
}

// Helper to convert ConfigValue to string
std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    if (!validateFilePath(filepath)) {
        NLM_LOG_ERROR("Config file does not exist or is not readable: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": no '=' found: " + line);
            success = false;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == \'\' && value.back() == \'\'))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate key format
        if (!isValidKey(key)) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": invalid key format: " + key);
            success = false;
            continue;
        }
        
        try {
            // Try to infer type and set with validation
            // Try boolean first (case-insensitive)
            std::string lowerValue = toLower(value);
            if (lowerValue == "true" || lowerValue == "false" || lowerValue == "1" || lowerValue == "0") {
                bool boolValue = stringToBool(value);
                set(key, boolValue, ConfigSource::File);
            } else {
                // Try integer
                int intValue;
                if (isValidInteger(value, intValue)) {
                    set(key, intValue, ConfigSource::File);
                } else {
                    // Try double
                    double doubleValue;
                    if (isValidDouble(value, doubleValue)) {
                        set(key, doubleValue, ConfigSource::File);
                    } else {
                        // Default to string
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        } catch (const ConfigException& e) {
            NLM_LOG_ERROR("Error parsing config line " + std::to_string(lineNum) + ": " + e.what());
            success = false;
            continue;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error parsing config line " + std::to_string(lineNum) + ": " + e.what());
            success = false;
            continue;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    } else {
        NLM_LOG_WARNING("Configuration loaded with errors from: " + filepath);
    }
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1) {
        NLM_LOG_WARNING("No command line arguments provided");
        return true;
    }
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(arg.substr(2, pos - 2));
                std::string value = trim(arg.substr(pos + 1));
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("Invalid command line argument: empty key in " + arg);
                    success = false;
                    continue;
                }
                
                if (!isValidKey(key)) {
                    NLM_LOG_WARNING("Invalid command line argument: key format invalid in " + arg);
                    success = false;
                    continue;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigException& e) {
                    NLM_LOG_ERROR("Error parsing command line argument " + arg + ": " + e.what());
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line format: missing '=' in " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = trim(arg.substr(1));
            std::string value = trim(argv[++i]);
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid command line argument: empty key in " + arg);
                success = false;
                continue;
            }
            
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Invalid command line argument: key format invalid in " + arg);
                success = false;
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigException& e) {
                NLM_LOG_ERROR("Error parsing command line argument " + arg + ": " + e.what());
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path directory exists
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path.parent_path())) {
        NLM_LOG_ERROR("Config directory does not exist: " + path.parent_path().string());
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    // Set high precision for floating point values
    file.precision(15);
    file << std::fixed;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
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
        // Type mismatch - try to convert if possible
        try {
            if constexpr (std::is_same_v<T, int>) {
                // Try to convert from other numeric types
                if (std::holds_alternative<int64_t>(it->value)) {
                    return static_cast<int>(std::get<int64_t>(it->value));
                } else if (std::holds_alternative<double>(it->value)) {
                    double val = std::get<double>(it->value);
                    if (val >= std::numeric_limits<int>::lowest() && val <= std::numeric_limits<int>::max()) {
                        return static_cast<int>(val);
                    }
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (std::holds_alternative<int>(it->value)) {
                    return static_cast<int64_t>(std::get<int>(it->value));
                } else if (std::holds_alternative<double>(it->value)) {
                    double val = std::get<double>(it->value);
                    if (val >= std::numeric_limits<int64_t>::lowest() && val <= std::numeric_limits<int64_t>::max()) {
                        return static_cast<int64_t>(val);
                    }
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::holds_alternative<int>(it->value)) {
                    return static_cast<double>(std::get<int>(it->value));
                } else if (std::holds_alternative<int64_t>(it->value)) {
                    return static_cast<double>(std::get<int64_t>(it->value));
                }
            }
            
            // Try to convert from string for numeric types
            if (std::holds_alternative<std::string>(it->value)) {
                const std::string& strValue = std::get<std::string>(it->value);
                if constexpr (std::is_same_v<T, int>) {
                    return stringToInteger<int>(strValue);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    return stringToInteger<int64_t>(strValue);
                } else if constexpr (std::is_same_v<T, double>) {
                    return stringToDouble<double>(strValue);
                }
            }
            
            // Try to convert from bool for numeric types
            if (std::holds_alternative<bool>(it->value)) {
                bool boolVal = std::get<bool>(it->value);
                if constexpr (std::is_same_v<T, int>) {
                    return static_cast<int>(boolVal);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    return static_cast<int64_t>(boolVal);
                } else if constexpr (std::is_same_v<T, double>) {
                    return static_cast<double>(boolVal);
                }
            }
        } catch (const ConfigParseException& e) {
            // Type conversion failed, return nullopt
        } catch (const std::exception&) {
            // Other conversion errors, return nullopt
        }
        
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        throw ConfigValidationException("Invalid key format: '" + key + "'");
    }
    
    // Validate value based on type
    try {
        if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            for (const auto& item : vec) {
                if (item < std::numeric_limits<int>::lowest() || item > std::numeric_limits<int>::max()) {
                    throw ConfigValidationException("Integer value out of range in vector for key: '" + key + "'");
                }
            }
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            for (const auto& item : vec) {
                if (std::isinf(item) || std::isnan(item)) {
                    throw ConfigValidationException("Invalid value (NaN or Inf) in vector for key: '" + key + "'");
                }
            }
        }
    } catch (const ConfigValidationException&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigValidationException("Error validating value for key '" + key + "': " + e.what());
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
    try {
        set(key, ConfigValue(value), source);
    } catch (const ConfigException& e) {
        NLM_LOG_ERROR("Error setting config key '" + key + "': " + e.what());
        throw;
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (std::isinf(value) || std::isnan(value)) {
        throw ConfigValidationException("Invalid value (NaN or Inf) for key '" + key + "'");
    }
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
// Only instantiate commonly used types to avoid linker errors
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