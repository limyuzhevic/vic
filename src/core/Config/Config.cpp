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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + 
                       ": '=' not found in: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + 
                       ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Failed to parse config line " + std::to_string(lineNumber) +
                       ": " + line + " (" + std::string(e.what()) + ")");
        }
    }
    
    if (!hasValidEntries) {
        NLM_LOG_WARN("Config file " + filepath + " has no valid configuration entries");
    }
    
    return hasValidEntries;
}

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "Logger/Logger.hpp"

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + 
                       ": '=' not found in: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + 
                       ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Failed to parse config line " + std::to_string(lineNumber) +
                       ": " + line + " (" + std::string(e.what()) + ")");
        }
    }
    
    if (!hasValidEntries) {
        NLM_LOG_WARN("Config file " + filepath + " has no valid configuration entries");
    }
    
    return hasValidEntries;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasValidArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!key.empty()) {
                    try {
                        set(key, value, ConfigSource::CommandLine);
                        hasValidArgs = true;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Failed to parse command line argument --" + key + "=" + value + ": " + std::string(e.what()));
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!key.empty()) {
                try {
                    set(key, value, ConfigSource::CommandLine);
                    hasValidArgs = true;
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to parse command line argument -" + key + " " + value + ": " + std::string(e.what()));
                }
            }
        }
    }
    
    if (!hasValidArgs) {
        NLM_LOG_WARN("No valid command line arguments found");
    }
    
    return hasValidArgs;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << std::ctime(&std::time(nullptr));
        file << "\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>) {
                            file << "\"" << value[i] << "\"";
                        } else {
                            file << value[i];
                        }
                    }
                    file << "]";
                } else {
                    file << value;
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Validate key format (basic check for valid characters)
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '/';
    })) {
        NLM_LOG_WARN("Config key contains unusual characters: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Log change if source is different
        if (it->source != source) {
            NLM_LOG_INFO("Config updated: " + key + " (source: " + std::to_string(static_cast<int>(source)) + ")");
        }
        it->value = value;
        it->source = source;
    } else {
        NLM_LOG_INFO("Config added: " + key);
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

bool Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARN("Attempted to remove config with empty key");
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_INFO("Config removed: " + key);
        pImpl->entries.erase(it);
        return true;
    } else {
        NLM_LOG_WARN("Attempted to remove non-existent config key: " + key);
        return false;
    }
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
    oss << "=" << std::string(50, '=') << "\n";
    
    // Group entries by source for better organization
    std::map<ConfigSource, std::vector<const ConfigEntry*>> groupedEntries;
    for (const auto& entry : pImpl->entries) {
        groupedEntries[entry.source].push_back(&entry);
    }
    
    // Print entries by source
    const std::map<ConfigSource, std::string> sourceNames = {
        {ConfigSource::Default, "DEFAULT"},
        {ConfigSource::File, "FILE"},
        {ConfigSource::CommandLine, "COMMAND_LINE"},
        {ConfigSource::Runtime, "RUNTIME"}
    };
    
    for (const auto& [source, entries] : groupedEntries) {
        oss << "[" << sourceNames.at(source) << "]\n";
        for (const auto* entry : entries) {
            oss << "  " << std::left << std::setw(25) << entry->key << " = ";
            
            // Format the value for display
            std::visit([&oss](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[" << value.size() << " elements]";
                    if (value.size() <= 3) {
                        oss << " [";
                        for (size_t i = 0; i < value.size(); ++i) {
                            if (i > 0) oss << ", ";
                            if constexpr (std::is_same_v<T, std::vector<std::string>) {
                                oss << "\"" << value[i] << "\"";
                            } else {
                                oss << value[i];
                            }
                        }
                        oss << "]";
                    }
                } else {
                    oss << value;
                }
            }, entry->value);
            
            oss << "\n";
        }
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Attempted to get config with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        NLM_LOG_WARN("Config key not found: " + key);
        return std::nullopt;
    }
    
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to get config value for key " + key + ": " + std::string(e.what()));
        return std::nullopt;
    }
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
