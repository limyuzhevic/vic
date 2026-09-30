#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace nlm {

class Config::Impl {
private:
    struct ConfigEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
        std::string description;
        std::chrono::steady_clock::time_point timestamp;
        
        ConfigEntry() : source(ConfigSource::Default), timestamp(std::chrono::steady_clock::now()) {}
        ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
            : key(k), value(v), source(s), description(desc), timestamp(std::chrono::steady_clock::now()) {}
    };
    
    // Optimizations:
    // 1. Case-insensitive key lookup for convenience
    std::unordered_map<std::string, std::unique_ptr<ConfigEntry>> entriesByKey;
    std::vector<std::unique_ptr<ConfigEntry>> entries; // Maintain insertion order
    
    // Validation and type checking cache
    mutable std::unordered_map<std::string, bool> validKeys;
    mutable bool validationDirty;
    
public:
    Impl() : validationDirty(true) {}
    
    // Optimized key lookup with case-insensitive support
    ConfigEntry* findEntry(const std::string& key) {
        auto it = entriesByKey.find(key);
        if (it != entriesByKey.end()) {
            return it->second.get();
        }
        
        // Try case-insensitive lookup for convenience
        std::string lowerKey = toLower(key);
        auto lowerIt = entriesByKey.find(lowerKey);
        if (lowerIt != entriesByKey.end()) {
            // Move to proper case key for consistency
            entriesByKey[key] = std::move(lowerIt->second);
            entriesByKey.erase(lowerIt);
            return entriesByKey[key].get();
        }
        
        return nullptr;
    }
    
    const ConfigEntry* findEntry(const std::string& key) const {
        auto it = entriesByKey.find(key);
        if (it != entriesByKey.end()) {
            return it->second.get();
        }
        
        // Try case-insensitive lookup
        std::string lowerKey = toLower(key);
        auto lowerIt = entriesByKey.find(lowerKey);
        if (lowerIt != entriesByKey.end()) {
            return lowerIt->second.get();
        }
        
        return nullptr;
    }
    
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
        // Mark validation dirty
        validationDirty = true;
        
        // Create entry
        auto entry = std::make_unique<ConfigEntry>(key, value, source, description);
        
        // Insert into both containers
        entries.push_back(std::move(entry));
        entriesByKey[key] = std::move(entries.back());
    }
    
    void removeEntry(const std::string& key) {
        validationDirty = true;
        
        // Remove from both containers
        auto it = entriesByKey.find(key);
        if (it != entriesByKey.end()) {
            entriesByKey.erase(it);
        }
        
        // Remove from vector (preserve order)
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const std::unique_ptr<ConfigEntry>& e) { return e->key == key; }),
            entries.end()
        );
    }
    
    size_t size() const { return entries.size(); }
    bool empty() const { return entries.empty(); }
    
    const std::vector<std::unique_ptr<ConfigEntry>>& getAllEntries() const { return entries; }
    
    bool isValidKey(const std::string& key) const {
        if (!validationDirty) {
            auto it = validKeys.find(key);
            if (it != validKeys.end()) {
                return it->second;
            }
        }
        
        // Validate all keys
        validKeys.clear();
        validationDirty = false;
        
        for (const auto& entry : entries) {
            bool isValid = !entry->key.empty() && 
                          entry->key.find('=') == std::string::npos &&
                          entry->key.find('\n') == std::string::npos;
            validKeys[entry->key] = isValid;
        }
        
        return validKeys[key];
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot load config from empty filepath");
        return false;
    }
    
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) {
        NLM_LOG_ERROR("Config file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file size to avoid loading huge files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    const size_t maxConfigSize = 10 * 1024 * 1024; // 10MB limit
    if (fileSize > maxConfigSize) {
        NLM_LOG_ERROR("Config file too large: " + filepath + " (" + 
                     std::to_string(fileSize) + " bytes)");
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line (no '='): " + filepath + ":" + 
                           std::to_string(lineNumber) + " - " + line);
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key in line " + std::to_string(lineNumber));
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    NLM_LOG_INFO("Loaded config from " + filepath + " (" + 
                 std::to_string(pImpl->size()) + " entries, line " + 
                 std::to_string(lineNumber) + ")");
    
    return !hasErrors;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 1) {
        return true; // No arguments, nothing to do
    }
    
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument (missing '='): " + arg);
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        else {
            NLM_LOG_WARNING("Unknown command line argument format: " + arg);
            hasErrors = true;
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->size()) + " config arguments from command line");
    return !hasErrors;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save config to empty filepath");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entryPtr : pImpl->getAllEntries()) {
        const auto& entry = *entryPtr;
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format value based on type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved config to " + filepath + " (" + 
                 std::to_string(pImpl->size()) + " entries)");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    const Impl::ConfigEntry* entry = pImpl->findEntry(key);
    if (!entry) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(entry->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for key '" + key + "': expected " + 
                     typeName<T>() + ", got different type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    // Validate key format
    if (key.find('=') != std::string::npos) {
        NLM_LOG_ERROR("Invalid key name contains '=': " + key);
        return;
    }
    
    if (key.find('\n') != std::string::npos) {
        NLM_LOG_ERROR("Invalid key name contains newline: " + key);
        return;
    }
    
    // Check if key already exists
    auto existing = pImpl->findEntry(key);
    if (existing) {
        // Update existing entry
        existing->value = value;
        existing->source = source;
        existing->timestamp = std::chrono::steady_clock::now();
    } else {
        // Add new entry
        pImpl->addEntry(key, value, source);
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
    return pImpl->findEntry(key) != nullptr;
}

void Config::remove(const std::string& key) {
    pImpl->removeEntry(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->size());
    
    for (const auto& entryPtr : pImpl->getAllEntries()) {
        keys.push_back(entryPtr->key);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->entriesByKey.clear();
    pImpl->validationDirty = true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->size() << " entries):\n";
    
    for (const auto& entryPtr : pImpl->getAllEntries()) {
        const auto& entry = *entryPtr;
        oss << "  " << entry.key << " = [";
        
        // Format value based on type
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

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Helper function to get type name for error messages
template<typename T>
std::string typeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    else return "unknown";
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
