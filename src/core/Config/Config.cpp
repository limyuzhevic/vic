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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t successfulEntries = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config format in " + filepath + ": line " + 
                           std::to_string(lineNumber) + ": missing '='")
;            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key in " + filepath + ": line " + 
                           std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        ++successfulEntries;
    }
    
    if (successfulEntries == 0 && lineNumber > 0) {
        NLM_LOG_WARNING("No valid configuration entries found in " + filepath);
    } else {
        NLM_LOG_INFO("Loaded " + std::to_string(successfulEntries) + 
                    " configuration entries from " + filepath);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    size_t processedArgs = 0;
    size_t validEntries = 0;
    
    for (int i = 1; i < argc; ++i) {
        ++processedArgs;
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Invalid command line argument: empty key in " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                ++validEntries;
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 >= argc) {
                NLM_LOG_WARNING("Missing value for command line argument: " + arg);
                continue;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid command line argument: empty key");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            ++validEntries;
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    
    if (validEntries == 0 && processedArgs > 0) {
        NLM_LOG_WARNING("No valid configuration arguments found");
    } else {
        NLM_LOG_INFO("Processed " + std::to_string(processedArgs) + 
                    " command line arguments, " + std::to_string(validEntries) + 
                    " valid configuration entries");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (auto-generated)\n";
    file << "# Source: " << pImpl->entries.size() << " configuration entries\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << "# End of configuration\n";
    
    if (!file.good()) {
        NLM_LOG_ERROR("Error writing to config file: " + filepath);
        return false;
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
    if (val.has_value()) {
        return val.value();
    }
    
    // Log missing key at debug level to avoid spam
    NLM_LOG_DEBUG("Configuration key not found: " + key + ", using default value");
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set configuration: empty key not allowed");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        
        std::string sourceStr;
        switch (source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        
        NLM_LOG_DEBUG("Updated configuration key: " + key + " = " + 
                     std::to_string(std::visit([](auto&& arg) { return arg; }, value)) + 
                     " (source: " + sourceStr + ")");
    } else {
        // Add new entry
        std::string sourceStr;
        switch (source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new configuration key: " + key + " (source: " + sourceStr + ")");
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "================================================================================\n";
    
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
        oss << "]\n";
        
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        
        oss << "  Source: " << sourceStr << " (" << static_cast<int>(entry.source) << ")\n";
        
        if (!entry.description.empty()) {
            oss << "  Description: " << entry.description << "\n";
        }
        
        oss << "\n";
    }
    
    oss << "================================================================================\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    
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
