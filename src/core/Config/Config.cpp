#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <variant>
#include <string>
#include <vector>

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
        return false;
    }
    
    // Try to detect file format based on extension and content
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return loadFromJSON(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAML(filepath);
    } else if (extension == "cfg" || extension == "txt" || extension == "ini") {
        // For backward compatibility, support simple key=value format
        return loadFromSimpleFormat(filepath);
    } else {
        // Default to simple format for unknown extensions
        return loadFromSimpleFormat(filepath);
    }
}

// Simple key=value format loader (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// JSON file loader
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parser (placeholder for now)
    // In a real implementation, this would use a proper JSON library like nlohmann::json
    // For now, implement a basic JSON parser that handles simple objects
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Remove whitespace
    std::string cleanContent;
    for (char c : content) {
        if (!std::isspace(c)) {
            cleanContent += c;
        }
    }
    
    if (cleanContent.empty() || cleanContent[0] != '{') {
        return false; // Not a valid JSON object
    }
    
    // Parse simple key-value pairs (no nested objects or arrays for now)
    size_t pos = 1; // Skip opening {
    while (pos < cleanContent.size()) {
        if (cleanContent[pos] == '}') {
            break; // End of object
        }
        
        // Find key
        size_t keyStart = pos;
        while (pos < cleanContent.size() && cleanContent[pos] != ':') {
            pos++;
        }
        if (pos >= cleanContent.size()) {
            break; // Malformed JSON
        }
        
        std::string key = cleanContent.substr(keyStart, pos - keyStart);
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < cleanContent.size() && (cleanContent[pos] == ' ' || cleanContent[pos] == '\t')) {
            pos++;
        }
        
        // Parse value
        size_t valueStart = pos;
        if (cleanContent[pos] == '"') {
            // String value
            while (pos < cleanContent.size() && cleanContent[pos] != '\"') {
                pos++;
            }
            if (pos >= cleanContent.size()) {
                break; // Malformed JSON
            }
            pos++; // Skip closing quote
        } else if (cleanContent[pos] == '{' || cleanContent[pos] == '[') {
            // For now, skip complex values (objects/arrays)
            size_t braceCount = 1;
            pos++;
            while (pos < cleanContent.size() && braceCount > 0) {
                if (cleanContent[pos] == '{' || cleanContent[pos] == '[') {
                    braceCount++;
                } else if (cleanContent[pos] == '}' || cleanContent[pos] == ']') {
                    braceCount--;
                }
                pos++;
            }
        } else {
            // Simple value (number, boolean, null)
            while (pos < cleanContent.size() && cleanContent[pos] != ',' && cleanContent[pos] != '}') {
                pos++;
            }
        }
        
        std::string value = cleanContent.substr(valueStart, pos - valueStart);
        
        // Clean up quotes and parse value
        if (!value.empty() && value.front() == '\"' && value.back() == '\"') {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to parse value as different types
        try {
            if (value == "true") {
                set(key, true, ConfigSource::File);
            } else if (value == "false") {
                set(key, false, ConfigSource::File);
            } else if (value == "null") {
                // Skip null values
            } else {
                // Try to parse as number
                try {
                    if (value.find('.') != std::string::npos) {
                        set(key, std::stod(value), ConfigSource::File);
                    } else {
                        set(key, std::stoll(value), ConfigSource::File);
                    }
                } catch (...) {
                    // Not a number, treat as string
                    set(key, value, ConfigSource::File);
                }
            }
        } catch (...) {
            // Skip problematic values
        }
        
        // Skip comma or closing brace
        while (pos < cleanContent.size() && (cleanContent[pos] == ',' || cleanContent[pos] == ' ')) {
            pos++;
        }
    }
    
    return true;
}

// YAML file loader (basic implementation)
bool Config::loadFromYAML(const std::string& filepath) {
    // For now, treat as simple format since implementing full YAML parser
    // would require adding yaml-cpp dependency
    return loadFromSimpleFormat(filepath);
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
