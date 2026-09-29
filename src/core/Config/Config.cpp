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
        return false;
    }
    
    // Detect file format by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Simple JSON parsing implementation (no external dependencies)
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseJSON(content);
}

bool Config::parseJSON(const std::string& jsonStr) {
    // Simple JSON parsing for object types only
    // This is a minimal implementation focusing on common types
    
    size_t pos = 0;
    bool in_object = false;
    bool in_string = false;
    char quoteChar = '\0';
    std::string currentKey;
    std::string currentValue;
    
    while (pos < jsonStr.size()) {
        char c = jsonStr[pos];
        
        if (c == '"' && !in_string) {
            // Start of string
            in_string = true;
            quoteChar = c;
            pos++;
            continue;
        } else if (c == quoteChar && in_string) {
            // End of string
            in_string = false;
            quoteChar = '\0';
            pos++;
            continue;
        } else if (c == ':' && !in_string && in_object) {
            // Key-value separator
            currentKey = trim(currentKey);
            pos++;
            continue;
        } else if (c == ',' && !in_string && !in_object) {
            // Value separator (simple parsing)
            setValueFromString(currentKey, currentValue);
            currentKey.clear();
            currentValue.clear();
            pos++;
            continue;
        } else if (c == '{' && !in_string) {
            // Start of object - skip for now
            in_object = true;
            pos++;
            continue;
        } else if (c == '}' && !in_string && in_object) {
            // End of object
            in_object = false;
            // Set last key-value pair if any
            if (!currentKey.empty()) {
                setValueFromString(currentKey, currentValue);
            }
            pos++;
            continue;
        } else if (!in_string && !in_object && c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            // Skip non-object content
            pos++;
            continue;
        }
        
        if (in_string) {
            currentValue += c;
        }
        
        pos++;
    }
    
    return true;
}

void Config::setValueFromString(const std::string& key, const std::string& valueStr) {
    std::string value = trim(valueStr);
    
    // Try to determine type based on content
    if (value.empty() || value == "null") {
        set(key, "", ConfigSource::File);
    } else if (value == "true" || value == "false") {
        bool boolValue = (value == "true");
        set(key, boolValue, ConfigSource::File);
    } else if (value[0] == '"' && value.back() == '"') {
        // String with quotes
        set(key, value.substr(1, value.size() - 2), ConfigSource::File);
    } else {
        // Try numeric parsing
        try {
            // Try integer
            size_t pos;
            long long intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        try {
            // Try double
            size_t pos;
            double doubleValue = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleValue, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        // Fall back to string
        set(key, value, ConfigSource::File);
    }
}

// Minimal YAML parsing implementation
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, treat YAML as simple key=value format
    // TODO: Implement proper YAML parsing with libyaml-cpp or similar
    return loadFromSimpleFormat(filepath);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine output format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json" || ext == "JSON") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return saveToYAML(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
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
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Format value as JSON
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple YAML format: key: value
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << ": ";
        
        // Format value as YAML
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
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
