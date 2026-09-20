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
    // NEW: JSON/YAML parser implementation with fallback to simple key=value format
    
    // First try JSON parsing (if jsoncpp library is available)
    bool success = false;
    
    // Try JSON parsing first
    success = parseJSON(filepath);
    
    // If JSON parsing fails and file extension suggests YAML, try YAML
    if (!success && (filepath.size() > 5 && filepath.substr(filepath.size() - 5) == ".yaml") ||
        (filepath.size() > 4 && filepath.substr(filepath.size() - 4) == ".yml")) {
        success = parseYAML(filepath);
    }
    
    // If both JSON/YAML parsing fail, fall back to simple key=value format
    if (!success) {
        success = parseSimpleKeyValue(filepath);
    }
    
    return success;
}

bool Config::parseJSON(const std::string& filepath) {
    // Simple JSON parser - in production, would use a proper JSON library like jsoncpp
    // For now, implement a basic JSON parser that handles simple key-value pairs
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON object {
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Simple JSON parsing - extract key-value pairs
        // This is a simplified implementation
        size_t pos = content.find('{');
        if (pos != std::string::npos) {
            pos = content.find('"', pos);
            while (pos != std::string::npos) {
                size_t keyEnd = content.find('"', pos + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = content.substr(pos + 1, keyEnd - pos - 1);
                
                pos = content.find('"', keyEnd + 1);
                if (pos == std::string::npos) break;
                
                pos = content.find(':', pos);
                if (pos == std::string::npos) break;
                
                pos = content.find('"', pos + 1);
                if (pos == std::string::npos) break;
                
                size_t valueEnd = content.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                
                std::string value = content.substr(pos + 1, valueEnd - pos - 1);
                
                // Remove quotes from key and value
                if (!key.empty() && key.front() == '"' && key.back() == '"') {
                    key = key.substr(1, key.size() - 2);
                }
                if (!value.empty() && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
                
                pos = content.find('"', valueEnd + 1);
            }
            
            return true;
        }
    }
    
    return false;
}

bool Config::parseYAML(const std::string& filepath) {
    // Simple YAML parser - in production, would use a proper YAML library like yaml-cpp
    // For now, implement a basic YAML parser that handles key: value pairs
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments
        line = trim(line);
        if (line.empty() || line.substr(0, 2) == "# " || line.substr(0, 1) == "%") {
            continue;
        }
        
        // Parse YAML key: value
        size_t pos = line.find(':');
        if (pos != std::string::npos && pos > 0) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes from value if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Handle YAML boolean values
            if (value == "true" || value == "yes" || value == "on") {
                set(key, true, ConfigSource::File);
            } else if (value == "false" || value == "no" || value == "off") {
                set(key, false, ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                // Try to parse as float
                try {
                    double floatValue = std::stod(value);
                    set(key, floatValue, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Try to parse as int
                try {
                    size_t posInt = 0;
                    int intValue = std::stoi(value, &posInt);
                    if (posInt == value.size()) {
                        set(key, intValue, ConfigSource::File);
                    } else {
                        set(key, value, ConfigSource::File);
                    }
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        }
    }
    
    return true;
}

bool Config::parseSimpleKeyValue(const std::string& filepath) {
    // Original simple key=value parser - kept for backward compatibility
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
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
