#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Add more efficient lookup structures
    std::unordered_map<std::string, size_t> keyIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // Enhanced version with better error handling and progress reporting
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file size for optimization
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize == 0) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    size_t errorEntries = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
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
            
            // Validate key format
            if (key.empty() || !isValidKey(key)) {
                NLM_LOG_WARNING("Invalid key format at line " + std::to_string(lineNumber) + ": " + key);
                ++errorEntries;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set the value (with validation)
            if (setWithValidation(key, value, ConfigSource::File)) {
                ++validEntries;
            } else {
                ++errorEntries;
            }
        } else {
            NLM_LOG_WARNING("Invalid line format at line " + std::to_string(lineNumber) + ": " + line);
            ++errorEntries;
        }
    }
    
    NLM_LOG_INFO("Config file loaded: " + filepath + " (" + std::to_string(validEntries) + 
                " entries, " + std::to_string(errorEntries) + " errors, " + std::to_string(lineNumber) + 
                " lines)");
    
    return validEntries > 0;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments...");
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!setWithValidation(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Invalid command line argument: " + arg);
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Missing value for argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!setWithValidation(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_WARNING("Invalid command line argument: " + arg + " " + value);
                success = false;
            }
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Command line arguments processed successfully");
    } else {
        NLM_LOG_WARNING("Some command line arguments were invalid");
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "\"" << getValueAsString(entry.value) << "\"\n\n";
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
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Use the enhanced validation version
    setWithValidation(key, value, source);
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
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
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
    pImpl->keyIndex.clear();
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

// Helper methods for enhanced functionality
bool Config::isValidKey(const std::string& key) const {
    if (key.empty() || key.length() > 100) {
        return false;
    }
    
    // Check for valid characters (alphanumeric, underscore, hyphen, dot)
    return std::all_of(key.begin(), key.end(), 
        [](char c) { return std::isalnum(c) || c == '_' || c == '-' || c == '.'; });
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check for duplicate keys
    auto existing = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (existing != pImpl->entries.end()) {
        NLM_LOG_WARNING("Overwriting existing config key: " + key);
        existing->value = value;
        existing->source = source;
        return true;
    }
    
    // Basic validation for key format
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Invalid key format: " + key);
        return false;
    }
    
    // Add to entries
    pImpl->entries.emplace_back(key, value, source);
    
    // Update index for faster lookups
    pImpl->keyIndex[key] = pImpl->entries.size() - 1;
    
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::getValueAsString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::stringstream ss;
            ss << "[";n            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::stringstream ss;
            ss << "[";n            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::stringstream ss;
            ss << "[";n            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
            return ss.str();
        } else {
            return "\"UNKNOWN_TYPE\"";
        }
    }, value);
}

// Explicit template instantiations (same as before)
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