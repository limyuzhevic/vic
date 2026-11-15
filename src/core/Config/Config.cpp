#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string errorMessage;
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
                 (value.front() == \'\' && value.back() == \'\'))) {
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
        file << entry.key << " = ";
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else {
                file << value;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
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

// JSON/YAML Parsing Implementations

static std::string getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filepath.substr(pos + 1);
}

// Detect file format based on extension
static std::string detectFileFormat(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "jsonl") {
        return "json";
    } else if (ext == "yaml" || ext == "yml") {
        return "yaml";
    }
    return "keyvalue";
}

// Convert JSON value to ConfigValue
static ConfigValue jsonValueToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Check if it's an array of integers or doubles or strings
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                std::vector<int64_t> intArray;
                for (const auto& item : j) {
                    intArray.push_back(item.get<int64_t>());
                }
                return intArray;
            } else if (j[0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : j) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (j[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : j) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Empty array or mixed types - fallback to string representation
        return j.dump();
    } else if (j.is_object()) {
        // Convert object to string representation for now
        return j.dump();
    } else {
        // null, number without type, etc.
        return j.dump();
    }
}

// Convert ConfigValue to string for saving
static std::string configValueToString(const ConfigValue& value) {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + val + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int64_t> ||
                            std::is_same_v<T, double>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            std::string result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(val[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(val[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + val[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return std::to_string(val);
        }
    }, value);
}

// Parse key=value format file
static bool parseKeyValueFile(Config* config, const std::string& filepath) {
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
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config->set(key, value, ConfigSource::File);
        }
    }
    return true;
}

// Parse JSON file
static bool parseJSONFile(Config* config, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Parse JSON object recursively
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                ConfigValue value = jsonValueToConfigValue(it.value());
                config->set(it.key(), value, ConfigSource::File);
            }
        }
        return true;
    } catch (const json::parse_error& e) {
        // Fall back to key=value format if JSON parsing fails
        return parseKeyValueFile(config, filepath);
    } catch (...) {
        return false;
    }
}

// Parse YAML file (placeholder - would need a YAML parser library)
static bool parseYAMLFile(Config* config, const std::string& filepath) {
    // For now, fallback to key=value format
    // In a real implementation, you would use a YAML parser like YAML-cpp
    return parseKeyValueFile(config, filepath);
}

// Config implementation for new methods

// Configuration validation
bool Config::validate() const {
    return !pImpl->entries.empty(); // Basic validation - should check all entries
}

std::string Config::getLastError() const {
    return ""; // Placeholder - should track errors
}

void Config::clearLastError() {
    // Placeholder - clear error state
}

// Configuration file operations with better error reporting
bool Config::loadFromFile(const std::string& filepath, bool silent) {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Try to parse based on file extension
    if (ext == "json" || ext == "jsonl") {
        return parseJSONFile(this, filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return parseYAMLFile(this, filepath);
    } else {
        // Fall back to key=value format for backward compatibility
        return parseKeyValueFile(this, filepath);
    }
}

bool Config::saveToFile(const std::string& filepath, bool silent) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else {
                file << value;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

// Configuration type checking
template<typename T>
std::optional<T> Config::safeGet(const std::string& key, T defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Configuration batch operations
void Config::applyConfig(const std::string& filepath, ConfigSource source) {
    if (loadFromFile(filepath)) {
        // Mark entries loaded from file as having this source
        for (auto& entry : pImpl->entries) {
            if (entry.source == ConfigSource::File) {
                entry.source = source;
            }
        }
    }
}

void Config::mergeConfig(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Don't overwrite existing entries from higher priority sources
        if (!has(entry.key) || static_cast<int>(entry.source) > static_cast<int>(getSourceForKey(entry.key))) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

// Error handling
void Config::setError(const std::string& error) {
    // Placeholder - should store error message
    errorMessage = error;
}

std::string Config::getFormattedError(const std::string& operation) const {
    // Placeholder - should format error message
    if (!errorMessage.empty()) {
        return "Error during " + operation + ": " + errorMessage;
    }
    return "Error during " + operation;
}

// Source getter for a specific key
ConfigSource Config::getSourceForKey(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry.source;
        }
    }
    return ConfigSource::Default;
}

// Explicit template instantiations for safeGet
template std::optional<int> Config::safeGet<int>(const std::string&, const int&) const;
template std::optional<int64_t> Config::safeGet<int64_t>(const std::string&, const int64_t&) const;
template std::optional<uint64_t> Config::safeGet<uint64_t>(const std::string&, const uint64_t&) const;
template std::optional<float> Config::safeGet<float>(const std::string&, const float&) const;
template std::optional<double> Config::safeGet<double>(const std::string&, const double&) const;
template std::optional<bool> Config::safeGet<bool>(const std::string&, const bool&) const;
template std::optional<std::string> Config::safeGet<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
