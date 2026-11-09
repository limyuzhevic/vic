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
    // Try to load as JSON first
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
    }
    
    // Try to load as YAML
    if (loadFromYAML(filepath)) {
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_WARNING("Configuration file not in JSON/YAML format, falling back to key=value format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Trim whitespace
    content = trim(content);
    
    // Check if it looks like JSON (starts with { and ends with })
    if (content.empty() || content.front() != '{' || content.back() != '}') {
        return false;
    }
    
    // Simple JSON parsing - extract key-value pairs
    size_t pos = 1;
    while (pos < content.size() - 1) {
        // Find next key
        pos = content.find('"', pos);
        if (pos == std::string::npos) return false;
        
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) return false;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 2; // Skip ", then maybe : and space
        
        // Skip colon and optional whitespace
        while (pos < content.size() && (content[pos] == ':') || 
               std::isspace(content[pos])) {
            if (content[pos] == ':') pos++;
            if (std::isspace(content[pos])) pos++;
        }
        
        // Extract value
        if (content[pos] == '"') {
            // String value
            size_t valEnd = content.find('"', pos + 1);
            if (valEnd == std::string::npos) return false;
            std::string value = content.substr(pos + 1, valEnd - pos - 1);
            set(key, value, ConfigSource::File);
            pos = valEnd + 2;
        } else if (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '.') {
            // Number value
            size_t valEnd = content.find_first_of(",}", pos);
            if (valEnd == std::string::npos) valEnd = content.size();
            std::string valueStr = content.substr(pos, valEnd - pos);
            try {
                set(key, std::stod(valueStr), ConfigSource::File);
            } catch (const std::exception& e) {
                return false;
            }
            pos = valEnd;
        } else {
            return false; // Unknown format
        }
        
        // Skip comma and whitespace
        while (pos < content.size() && (content[pos] == ',') || std::isspace(content[pos])) {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool foundYamlMarkers = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        if (line == "---" || line.find("yaml:") != std::string::npos) {
            foundYamlMarkers = true;
            continue;
        }
        
        if (!foundYamlMarkers && (line.find("---") != std::string::npos || line.find("...") != std::string::npos)) {
            continue;
        }
        
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            if (value.find("[]") != std::string::npos) {
                value = "[]";
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Configuration file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            NLM_LOG_DEBUG("Parsed config: " + key + " = " + value);
        } else {
            NLM_LOG_WARNING("Invalid line in config file (" + filepath + ":" + std::to_string(lineNumber) + "): " + line);
        }
    }
    
    if (pImpl->entries.empty()) {
        NLM_LOG_ERROR("No valid configuration entries found in: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loaded simple format configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            NLM_LOG_INFO("Available command line arguments:");
            NLM_LOG_INFO("  --config=<file>      : Specify configuration file");
            NLM_LOG_INFO("  -c <file>            : Shorthand for config file");
            NLM_LOG_INFO("  --help, -h           : Show this help message");
            NLM_LOG_INFO("  --list-configs        : List all available configuration keys");
            NLM_LOG_INFO("  --config-value=<key>  : Show current value for a specific key");
            NLM_LOG_INFO("  --set=<key>=<value>   : Set a configuration value");
            return true; // Special handling for help
        }
        
        if (arg == "--list-configs") {
            std::vector<std::string> keys = getKeys();
            NLM_LOG_INFO("Available configuration keys:");
            for (const auto& key : keys) {
                auto value = get<std::string>(key);
                if (value.has_value()) {
                    NLM_LOG_INFO("  " + key + " = \"" + value.value() + "\"");
                } else {
                    NLM_LOG_INFO("  " + key + " = <uninitialized>");
                }
            }
            return true;
        }
        
        if (arg.substr(0, 18) == "--config-value=") {
            std::string key = arg.substr(18);
            auto value = get<std::string>(key);
            if (value.has_value()) {
                NLM_LOG_INFO("Configuration value for \"" + key + "\": \"" + value.value() + "\"");
            } else {
                NLM_LOG_WARNING("Unknown configuration key: " + key);
            }
            return true;
        }
        
        if (arg.substr(0, 7) == "--set=") {
            std::string pair = arg.substr(7);
            size_t eqPos = pair.find('=');
            if (eqPos != std::string::npos) {
                std::string key = pair.substr(0, eqPos);
                std::string value = pair.substr(eqPos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Set configuration \"" + key + "\" = \"" + value + "\"");
                hasArgs = true;
            }
        }
        
        else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                hasArgs = true;
            }
        }
        
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            hasArgs = true;
        }
    }
    
    if (hasArgs) {
        NLM_LOG_INFO("Command line arguments applied successfully");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
        return false;
    }
    
    // Save configuration in simple format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "<uninitialized>";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration \"" + key + "\" to " + std::to_string(value) + " (source: " + std::to_string(static_cast<int>(source)) + ")");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new configuration \"" + key + "\" = " + std::to_string(value));
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
    NLM_LOG_DEBUG("Removed configuration \"" + key + "\"");
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
    NLM_LOG_INFO("Cleared all configuration entries (" + std::to_string(pImpl->entries.size()) + " entries removed)");
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
