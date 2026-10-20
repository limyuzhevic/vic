#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cmath>

namespace nlm {

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with validation and error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced validation
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
            
            // Validate key
            if (key.empty() || key.find_first_of(" \t") != std::string::npos) {
                NLM_LOG_WARNING("Invalid key at line " + std::to_string(lineNum) + ": '" + key + "'");
                hasErrors = true;
                continue;
            }
            
            // Try to set value with validation
            setWithValidation(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNum) + ": missing '='");
            hasErrors = true;
        }
    }
    
    return !hasErrors;
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
    
    // Write header
    file << "# NLM Configuration File - Saved " << getCurrentTimestamp() << "\n";
    file << "# Generated from " << static_cast<int>(pImpl->entries.size()) << " configuration entries\n";
    file << "\n";
    
    // Save all entries
    for (const auto& entry : pImpl->entries) {
        // Write description if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write source indicator
        file << "# Source: ";
        switch (entry.source) {
            case ConfigSource::Default: file << "Default"; break;
            case ConfigSource::File: file << "File"; break;
            case ConfigSource::CommandLine: file << "Command Line"; break;
            case ConfigSource::Runtime: file << "Runtime"; break;
        }
        file << "\n";
        
        // Write the actual key-value pair
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in string values
                std::string escaped = arg;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                file << '"' << escaped << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format float with reasonable precision
                file << std::fixed << std::setprecision(6) << arg;
            } else {
                // For other types, use default formatting
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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

void Config::setWithValidation(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Enhanced set method with validation
    std::string errorMsg;
    
    // Try to determine type from value and validate
    bool isValid = true;
    ConfigValue validatedValue;
    
    // Check for special boolean values
    if (valueStr == "true" || valueStr == "false") {
        bool boolValue = (valueStr == "true");
        validatedValue = boolValue;
    }
    // Check for integer values (including negative)
    else if (isInteger(valueStr)) {
        int intValue = std::stoi(valueStr);
        validatedValue = intValue;
    }
    // Check for float values
    else if (isFloat(valueStr)) {
        float floatValue = std::stof(valueStr);
        validatedValue = floatValue;
    }
    // Check for integer64 values
    else if (isInteger64(valueStr)) {
        int64_t int64Value = std::stoll(valueStr);
        validatedValue = int64Value;
    }
    // Otherwise treat as string
    else {
        validatedValue = valueStr;
    }
    
    // Apply validation based on key if known validation rules exist
    if (isValid) {
        // Apply value
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = validatedValue;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, validatedValue, source);
        }
    }
}

bool Config::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return true;
}

bool Config::isInteger64(const std::string& str) const {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return true;
}

bool Config::isFloat(const std::string& str) const {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false; // Multiple decimal points
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return hasDecimal || start < str.size(); // Allow integer-like floats
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
