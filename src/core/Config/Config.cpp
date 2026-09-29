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

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid file path: filepath is empty");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check if file is readable and not empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
    } else {
        file.seekg(0, std::ios::beg);
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid config entry in " + filepath + ": line " + std::to_string(lineNumber) + 
                             " - key is empty");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value
            if (value.empty()) {
                NLM_LOG_WARNING("Empty value for key '" + key + "' in " + filepath + ": line " + std::to_string(lineNumber));
            }
            
            set(key, value, ConfigSource::File);
            validEntries++;
        } else {
            NLM_LOG_WARNING("Invalid config format in " + filepath + ": line " + std::to_string(lineNumber) + 
                           " - no '=' found: " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(validEntries) + " config entries from " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("Invalid arguments: argv is null");
        return false;
    }
    
    size_t argCount = 0;
    size_t validEntries = 0;
    
    for (int i = 1; i < argc; ++i) {
        argCount++;
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            NLM_LOG_WARNING("Empty argument at position " + std::to_string(i));
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_ERROR("Invalid argument format: key is empty in " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                validEntries++;
            } else {
                NLM_LOG_WARNING("Invalid argument format in " + arg + " - no '=' found");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid argument format: key is empty in " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            validEntries++;
        } else {
            NLM_LOG_WARNING("Unknown argument format in " + arg);
        }
    }
    
    NLM_LOG_INFO("Parsed " + std::to_string(validEntries) + " config entries from " + 
                 std::to_string(argCount) + " command line arguments");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid file path: filepath is empty");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    // Write entries with validation
    size_t entryCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_WARNING("Skipping config entry with empty key");
            continue;
        }
        
        file << "# " << entry.description << "\n";
        
        // Write key-value pair with proper formatting
        file << entry.key << " = ";
        
        // Handle different value types
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Write double with reasonable precision
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
        }, entry.value);
        
        file << "\n\n";
        entryCount++;
    }
    
    NLM_LOG_INFO("Saved " + std::to_string(entryCount) + " config entries to " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Invalid key: key is empty");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for config key: " + key + " - expected type not available");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for config key: " + key);
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Invalid key: key cannot be empty");
        return;
    }
    
    // Basic key validation - no special characters or whitespace
    if (key.find_first_of("\n\r\t \v\f") != std::string::npos) {
        NLM_LOG_ERROR("Invalid key: key contains whitespace: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new configuration key: " + key);
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
    if (key.empty()) {
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to remove config entry with empty key");
        return;
    }
    
    size_t erasedCount = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    size_t removedCount = erasedCount - pImpl->entries.size();
    if (removedCount > 0) {
        NLM_LOG_DEBUG("Removed " + std::to_string(removedCount) + " config entries with key: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (!entry.key.empty()) {
            keys.push_back(entry.key);
        } else {
            NLM_LOG_WARNING("Skipping config entry with empty key in getKeys()");
        }
    }
    return keys;
}

void Config::clear() {
    size_t entryCount = pImpl->entries.size();
    pImpl->entries.clear();
    if (entryCount > 0) {
        NLM_LOG_DEBUG("Cleared " + std::to_string(entryCount) + " config entries");
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "===========================================\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Value: [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " ]\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Description: " << entry.description << "\n";
        oss << "\n";
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
