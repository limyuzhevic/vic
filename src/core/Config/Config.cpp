#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateTypes;
    bool allowDuplicates;
    std::string encoding;
    
    Impl() : entries(), validateTypes(true), allowDuplicates(false), encoding("utf-8") {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Configuration object created");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced implementation with better error handling, type validation, and extensibility

    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check if file appears to be JSON (starts with { or [)
    bool isJson = false;
    if (!content.empty()) {
        char firstChar = content[0];
        isJson = (firstChar == '{' || firstChar == '[');
    }
    
    if (isJson) {
        NLM_LOG_INFO("Detected JSON configuration file");
        // TODO: Implement JSON parsing
        return false;
    } else {
        NLM_LOG_INFO("Using simple key=value configuration format");
        return loadSimpleConfig(content);
    }
}

bool Config::loadSimpleConfig(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found at line " + std::to_string(lineNumber));
                hasErrors = true;
                continue;
            }
            
            // Validate value is not empty (unless it's a boolean or numeric default)
            if (value.empty() && key != "enabled" && key != "disabled") {
                NLM_LOG_WARNING("Empty value for key '" + key + "' at line " + std::to_string(lineNumber));
                hasErrors = true;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Basic validation for common key patterns
            validateKeyPattern(key, lineNumber, hasErrors);
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid line format at line " + std::to_string(lineNumber) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file parsing completed with warnings");
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
    return true;
}

void Config::validateKeyPattern(const std::string& key, size_t lineNumber, bool& hasErrors) {
    // Validate key contains only valid characters
    static const std::regex validKeyPattern("^[a-zA-Z][a-zA-Z0-9_]*$");
    
    if (!std::regex_match(key, validKeyPattern)) {
        NLM_LOG_WARNING("Invalid key format '" + key + "' at line " + std::to_string(lineNumber) + ". "
                       "Keys should be alphanumeric with underscores and start with a letter.");
        hasErrors = true;
    }
    
    // Check for duplicate keys if duplicates are not allowed
    if (!pImpl->allowDuplicates) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });n        if (it != pImpl->entries.end()) {
            NLM_LOG_WARNING("Duplicate key '" + key + "' at line " + std::to_string(lineNumber));
            hasErrors = true;
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_DEBUG("Processing command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Set config from command line: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_DEBUG("Set config from command line: " + key + " = " + value);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write in JSON format for better compatibility
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON representation
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
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j] << (j < arg.size() - 1 ? "," : "");
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << std::setprecision(6) << arg[j] << (j < arg.size() - 1 ? "," : "");
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << arg[j] << "\"" << (j < arg.size() - 1 ? "," : "");
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        file << "," << "\n";
    }
    
    if (!pImpl->entries.empty()) {
        file.seekp(-2, std::ios_base::cur); // Remove trailing comma and newline
        file << "\n";
    }
    
    file << "}\n";
    
    NLM_LOG_INFO("Configuration saved to " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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
        NLM_LOG_WARNING("Type mismatch for key '" + key + "': expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for key '" + key + "': " + std::to_string(defaultValue));
        return defaultValue;
    }
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