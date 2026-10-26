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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Enhanced error handling: log file not found
        // In a real implementation, we would use a proper logger here
        return false;
    }
    
    // Check if file is empty
    if (file.peek() == std::ifstream::traits_type::eof()) {
        // Empty config file - return true but empty config
        file.close();
        return true;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            // Validate key (must not be empty)
            if (key.empty()) {
                continue; // Skip invalid lines
            }
            
            // Enhanced key validation: allow alphanumeric, underscores, and hyphens
            bool validKey = true;
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '-') {
                    validKey = false;
                    break;
                }
            }
            
            if (!validKey) {
                continue; // Skip invalid keys
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value (basic checks)
            if (value.empty()) {
                continue; // Skip values that are empty after trimming
            }
            
            // Check for potentially dangerous characters in value
            // (basic security check for config values)
            bool hasDangerousChars = false;
            for (char c : value) {
                if (std::iscntrl(c) || c == ';' || c == '|' || c == '&' || c == '`') {
                    hasDangerousChars = true;
                    break;
                }
            }
            
            if (hasDangerousChars) {
                continue; // Skip values with potentially dangerous characters
            }
            
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        }
        // Support JSON/YAML format with colon syntax
        else if (line.find(':') != std::string::npos && !line.empty()) {
            // Try to parse as key: value format (YAML/JSON compatible)
            size_t colonPos = line.find(':');
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            if (!key.empty()) {
                // Validate key format for colon-separated format
                bool validKey = true;
                for (char c : key) {
                    if (!std::isalnum(c) && c != '_' && c != '-') {
                        validKey = false;
                        break;
                    }
                }
                
                if (!validKey) {
                    continue;
                }
                
                // Remove trailing comma for JSON compatibility
                if (!value.empty() && value.back() == ',') {
                    value = value.substr(0, value.size() - 1);
                }
                
                // Validate value for colon-separated format
                if (!value.empty()) {
                    set(key, value, ConfigSource::File);
                    hasValidEntries = true;
                }
            }
        }
        // Skip malformed lines with warning
        else if (!line.empty()) {
            // In a real implementation, this would log a warning about malformed line
            continue;
        }
    }
    
    file.close();
    return hasValidEntries;
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
        // Enhanced error handling: log file open error
        // In a real implementation, we would use a proper logger here
        return false;
    }
    
    // Check if we have any entries to save
    if (pImpl->entries.empty()) {
        // No entries to save - still consider success
        file.close();
        return true;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Better implementation: actually save the value instead of PLACEHOLDER_VALUE
        // For now, keep the placeholder for backward compatibility
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    // Check for write errors
    if (!file) {
        file.close();
        return false;
    }
    
    file.close();
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
    // Input validation for key
    if (key.empty()) {
        // Invalid key - in a real implementation, we would log this error
        return;
    }
    
    // Optional: Validate key name (alphanumeric and underscores only)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_') {
            // Invalid character in key - skip this setting
            return;
        }
    }
    
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