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
    // Implement proper JSON/YAML parser with fallback to simple format
    // Primary support: JSON format (.json extension)
    // Fallback: Simple key=value format with comment support
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json") {
        return loadFromJSONFile(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Fallback to simple format
        return loadFromSimpleFile(filepath);
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
    if (start == std::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Internal loading methods
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parser - for now, handle basic key-value pairs
    // A full JSON parser would use a library or implement recursive descent
    std::string line;
    bool inObject = false;
    bool escaped = false;
    
    while (std::getline(file, line)) {
        // Handle JSON escaping
        if (escaped) {
            escaped = false;
            continue;
        }
        
        for (char& c : line) {
            if (c == '\\') {
                escaped = !escaped;
            } else if (c == '{' && !inObject) {
                inObject = true;
            } else if (c == '}' && inObject) {
                inObject = false;
            } else if (c == '"' && !inObject) {
                // Simple JSON value extraction
                parseSimpleJSONLine(line);
                break;
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Handle YAML key-value pairs
        parseSimpleYAMLLine(line);
    }
    
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
        parseSimpleConfigLine(line);
    }
    
    return true;
}

void Config::parseSimpleJSONLine(const std::string& line) {
    // Very simple JSON parser for basic key-value pairs
    size_t keyEnd = line.find('"');
    if (keyEnd == std::string::npos) {
        return;
    }
    
    size_t keyStart = line.find('"', keyEnd + 1);
    if (keyStart == std::string::npos) {
        return;
    }
    
    std::string key = line.substr(keyEnd + 1, keyStart - keyEnd - 1);
    
    // Find value (skip whitespace and colon)
    size_t valuePos = line.find(':', keyStart);
    if (valuePos == std::string::npos) {
        return;
    }
    
    std::string valueStr = trim(line.substr(valuePos + 1));
    if (valueStr.empty()) {
        return;
    }
    
    // Parse value based on type
    parseConfigValue(key, valueStr);
}

void Config::parseSimpleYAMLLine(const std::string& line) {
    // Very simple YAML parser for basic key-value pairs
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
        return;
    }
    
    std::string key = trim(line.substr(0, colonPos));
    std::string valueStr = trim(line.substr(colonPos + 1));
    
    if (valueStr.empty()) {
        return;
    }
    
    // Parse value based on type
    parseConfigValue(key, valueStr);
}

void Config::parseSimpleConfigLine(const std::string& line) {
    // Original simple key=value parser with improvements
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return;
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Parse value based on type
    parseConfigValue(key, value);
}

void Config::parseConfigValue(const std::string& key, const std::string& valueStr) {
    // Attempt to parse value into appropriate type
    // Try int first
    try {
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intValue, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleValue, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Try bool
    if (valueStr == "true" || valueStr == "false") {
        bool boolValue = (valueStr == "true");
        set(key, boolValue, ConfigSource::File);
        return;
    }
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Value = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, int64Value, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, valueStr, ConfigSource::File);
}

void Config::skipWhitespace(const std::string& line, size_t& pos) {
    while (pos < line.size() && std::isspace(line[pos])) {
        pos++;
    }
}

// Template instantiations for get() and getOr() methods
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

}} // namespace nlm
