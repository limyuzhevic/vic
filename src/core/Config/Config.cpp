#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <chrono>
#include <iomanip>
#include "../Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format based on extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yaml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Try to parse as JSON using rapidjson if available
    // This is a placeholder implementation - would need rapidjson/yaml-cpp integration
    NLM_LOG_WARNING("JSON config loading not yet implemented - falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Try to parse as YAML using yaml-cpp if available  
    // This is a placeholder implementation - would need yaml-cpp integration
    NLM_LOG_WARNING("YAML config loading not yet implemented - falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath, std::map<std::string, std::string>& outEntries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + 
                           " has no '=' separator: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty() || value.empty()) {
            NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + 
                           " has empty key or value");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Basic type validation and conversion
        if (!validateConfigKey(key) || !validateConfigValue(key, value)) {
            NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + 
                           " has invalid key or value: " + key + " = " + value);
            continue;
        }
        
        outEntries[key] = value;
    }
    
    if (outEntries.empty()) {
        NLM_LOG_WARNING("Config file " + filepath + " contains no valid entries");
    }
    
    return true;
}

bool Config::validateConfigKey(const std::string& key) {
    // Valid config keys should not contain special characters (except underscore)
    // Should be reasonable length
    if (key.empty() || key.length() > 100) {
        return false;
    }
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateConfigValue(const std::string& key, const std::string& value) {
    // Basic validation based on expected key types
    std::string lowerKey = toLower(key);
    
    // For numeric keys, validate numeric format
    if (lowerKey.find("seed") != std::string::npos || 
        lowerKey.find("count") != std::string::npos ||
        lowerKey.find("rate") != std::string::npos ||
        lowerKey.find("timestep") != std::string::npos ||
        lowerKey.find("weight") != std::string::npos) {
        // Try to parse as double
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            if (pos != value.length()) {
                return false; // Not a complete number
            }
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return true;
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
        NLM_LOG_ERROR("Cannot save config to file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write value with appropriate formatting based on type
        file << entry.key << " = ";
        writeConfigValueToStream(file, entry.value);
        file << "\n\n";
    }
    
    return true;
}

std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Config::writeConfigValueToStream(std::ostream& stream, const ConfigValue& value) const {
    std::visit([&stream](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            stream << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            stream << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << arg[i];
            }
            stream << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            stream << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << arg[i];
            }
            stream << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            stream << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) stream << ", ";
                stream << "\"" << arg[i] << "\"";
            }
            stream << "]";
        } else {
            stream << arg;
        }
    }, value);
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