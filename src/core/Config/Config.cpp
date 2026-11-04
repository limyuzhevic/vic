#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <json/json.h>  // For JSON parsing (if available)
#include <map>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
    std::mutex mutex;
    std::string version = "1.0.0";
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    if (format == "json" && isValidJsonFile(filepath)) {
        return parseJsonFile(filepath, pImpl->entries);
    } else {
        return parseSimpleFile(filepath, pImpl->entries);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> args = parseCommandLineArgs(argc, argv);
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
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
        else if (arg[0] == '-' && i + 1 < args.size()) {
            std::string key = arg.substr(1);
            std::string value = args[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --flag (boolean flag)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string format = detectFileFormat(filepath);
    
    if (format == "json") {
        return writeJsonFile(filepath, pImpl->entries);
    } else {
        return writeSimpleFile(filepath, pImpl->entries);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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

template<typename T>
bool Config::setTyped(const std::string& key, const T& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check schema validation
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    
    if (schemaIt != pImpl->schema.end()) {
        // Check type matches schema
        std::type_index valueType(typeid(T));
        if (valueType != schemaIt->type) {
            std::cerr << "Error: Type mismatch for key \"" << key << "\": expected " 
                     << typeIndexToString(schemaIt->type) << ", got " 
                     << typeIndexToString(valueType) << std::endl;
            return false;
        }
    }
    
    set(key, value, source);
    return true;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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

void Config::addSchema(const std::string& key, const std::type_index& type, ConfigSource source,
                       const std::string& description, const std::string& defaultValue,
                       bool required) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema.emplace_back(key, type, source, description, defaultValue, required);
}

bool Config::hasSchema(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
}

SchemaValidationResult Config::validateSchema() const {
    SchemaValidationResult result;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check all schema entries
    for (const auto& schemaEntry : pImpl->schema) {
        bool hasValue = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&schemaEntry](const ConfigEntry& e) { return e.key == schemaEntry.key; });
        
        if (schemaEntry.required && !hasValue) {
            result.addError("Required configuration key \"" + schemaEntry.key + "\" is missing");
        }
    }
    
    // Check all values against schema
    for (const auto& entry : pImpl->entries) {
        bool hasSchemaEntry = std::any_of(pImpl->schema.begin(), pImpl->schema.end(),
            [&entry](const ConfigSchemaEntry& s) { return s.key == entry.key; });
        
        if (hasSchemaEntry && !validateValueType(entry.value)) {
            // Only report if we can determine the actual type
            result.addWarning("Value type mismatch for key \"" + entry.key + "\"");
        }
    }
    
    return result;
}

void Config::printHelp() const {
    std::cout << "NLM Configuration Help" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "\nCommon command line options:" << std::endl;
    std::cout << "  --config=file.cfg          Load configuration from file" << std::endl;
    std::cout << "  --key=value              Set configuration value" << std::endl;
    std::cout << "  --flag                   Set boolean flag to true" << std::endl;
    std::cout << "  -k value                 Short form of --key" << std::endl;
    std::cout << "\nConfiguration values:" << std::endl;
    std::cout << "  Type: int, double, string, bool" << std::endl;
    std::cout << "  Example: --random_seed=42 --simulation_timestep=0.001" << std::endl;
    std::cout << "\nFile formats: JSON (.json) and simple key=value (.cfg)" << std::endl;
}

std::string Config::getVersion() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->version;
}

std::string Config::getVersionString() {
    return "NLM Configuration v1.0.0";
}

size_t Config::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

bool Config::isValidJsonFile(const std::string& filepath) {
    // Simple JSON validation - check if file starts with { or [
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Skip whitespace
    size_t start = firstLine.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    
    char firstChar = firstLine[start];
    return firstChar == '{' || firstChar == '[';
}

bool Config::isValidYamlFile(const std::string& filepath) {
    // Simple YAML validation - check for key: value pattern
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        // Check for key: value pattern
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "unknown";
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    // Remove leading whitespace
    firstLine = trim(firstLine);
    
    if (firstLine.empty()) return "unknown";
    
    // Check for JSON
    if (firstLine[0] == '{' || firstLine[0] == '[') {
        return "json";
    }
    
    // Check for simple key=value
    if (firstLine.find('=') != std::string::npos) {
        return "simple";
    }
    
    return "unknown";
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

bool Config::stringToBool(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        result = false;
        return true;
    }
    return false;
}

int Config::stringToInt(const std::string& str, int& result) {
    try {
        size_t pos;
        int val = std::stoi(str, &pos);
        if (pos == str.size()) {
            result = val;
            return true;
        }
    } catch (...) {}
    return false;
}

double Config::stringToDouble(const std::string& str, double& result) {
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) {
            result = val;
            return true;
        }
    } catch (...) {}
    return false;
}

template<typename T>
bool Config::validateValueType(const ConfigValue& value) const {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                  std::is_same_v<T, double> || std::is_same_v<T, float> ||
                  std::is_same_v<T, bool> || std::is_same_v<T, std::string> ||
                  std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> ||
                  std::is_same_v<T, std::vector<std::string>>) {
        return true;
    }
    return false;
}

std::string Config::typeIndexToString(const std::type_index& type) const {
    if (type == typeid(int)) return "int";
    if (type == typeid(int64_t)) return "int64_t";
    if (type == typeid(double)) return "double";
    if (type == typeid(float)) return "float";
    if (type == typeid(bool)) return "bool";
    if (type == typeid(std::string)) return "string";
    if (type == typeid(std::vector<int>)) return "std::vector<int>";
    if (type == typeid(std::vector<double>)) return "std::vector<double>";
    if (type == typeid(std::vector<std::string>)) return "std::vector<string>";
    return "unknown";
}

std::vector<std::string> Config::parseCommandLineArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return args;
}

std::string Config::normalizeFlag(const std::string& flag) {
    std::string normalized = flag;
    // Remove leading dashes
    if (normalized.substr(0, 2) == "--") {
        normalized = normalized.substr(2);
    } else if (normalized[0] == '-') {
        normalized = normalized.substr(1);
    }
    return normalized;
}

bool Config::parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    // Simple JSON parsing implementation
    // This is a simplified version - in production you'd use a proper JSON library
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        // Simple key=value parsing for now
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
            
            entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseSimpleFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
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
            
            entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::writeJsonFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << v << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (v ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << v;
            } else {
                file << "null"; // Fallback for unsupported types
            }
        }, entry.value);
        
        if (i < entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

bool Config::writeSimpleFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << v << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (v ? "true" : "false") << "\n";
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << entry.key << " = " << v << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    file << v[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << v << "\n";
            }
        }, entry.value);
    }
    
    return true;
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

template bool Config::setTyped<int>(const std::string&, const int&, ConfigSource);
template bool Config::setTyped<int64_t>(const std::string&, const int64_t&, ConfigSource);
template bool Config::setTyped<double>(const std::string&, const double&, ConfigSource);
template bool Config::setTyped<float>(const std::string&, const float&, ConfigSource);
template bool Config::setTyped<bool>(const std::string&, const bool&, ConfigSource);
template bool Config::setTyped<std::string>(const std::string&, const std::string&, ConfigSource);
} // namespace nlm

// Version function for backwards compatibility
namespace nlm {
    std::string getConfigVersion() {
        return Config::getVersionString();
    }
}