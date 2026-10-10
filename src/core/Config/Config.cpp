#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "ConfigPresets.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigValidator validator;
    ConfigPresets presets;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation rules
    pImpl->validator = ConfigValidator::createDefaultValidator();
    
    // Initialize with default presets
    pImpl->presets = ConfigPresets();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
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
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to determine value type
            bool success = false;
            
            // Try integer
            try {
                size_t posEnd;
                int intVal = std::stoi(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
            
            // Try int64_t
            if (!success) {
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::File);
                        success = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!success) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        success = true;
                    }
                } catch (...) {}
            }
            
            // Try bool
            if (!success) {
                if (value == "true" || value == "1" || value == "yes") {
                    set(key, true, ConfigSource::File);
                    success = true;
                } else if (value == "false" || value == "0" || value == "no") {
                    set(key, false, ConfigSource::File);
                    success = true;
                }
            }
            
            // Treat as string if not parsed as other types
            if (!success) {
                set(key, value, ConfigSource::File);
            }
            
            std::cout << "Loaded from file: " << key << " = " << value << std::endl;
        } else {
            std::cerr << "Warning: Invalid format at line " << lineNumber 
                     << ": \"" << line << "\" (expected key=value)" << std::endl;
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
                std::cout << "Loaded from command line: " << key << " = " << value << std::endl;
            } else {
                std::cerr << "Warning: Invalid command line argument format: " 
                         << arg << " (expected --key=value)" << std::endl;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            std::cout << "Loaded from command line: " << key << " = " << value << std::endl;
        } else {
            std::cerr << "Warning: Invalid command line argument: " << arg << std::endl;
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Neural Learning Machine Configuration" << std::endl;
    file << "# Generated on: " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << 
            static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = " << getValueAsString(entry.value) << "\n";
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

// Helper function implementations

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time_t_now, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Config::getValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
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

bool Config::validate() const {
    auto result = pImpl->validator.validate(*this);
    if (result.hasErrors()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cerr << "  Error: " << error << std::endl;
        }
        return false;
    }
    
    if (result.hasWarnings()) {
        std::cout << "Configuration warnings:" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "  Warning: " << warning << std::endl;
        }
    }
    
    return true;
}

bool Config::applyPreset(const std::string& name, ConfigSource source) {
    try {
        pImpl->presets.applyPreset(*this, name, source);
        std::cout << "Applied preset: " << name << std::endl;
        return true;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error applying preset '" << name << "': " << e.what() << std::endl;
        return false;
    }
}

bool Config::applyPresetType(PresetType type, ConfigSource source) {
    pImpl->presets.applyPresetType(*this, type, source);
    std::cout << "Applied preset type: " << static_cast<int>(type) << std::endl;
    return true;
}

void Config::mergePreset(const std::string& name, ConfigSource source) {
    pImpl->presets.mergePreset(*this, name, source);
    std::cout << "Merged preset: " << name << std::endl;
}

Config Config::createFromPreset(const std::string& name) {
    Config config;
    config.applyPreset(name, ConfigSource::Default);
    return config;
}

Config Config::createFromPresetType(PresetType type) {
    Config config;
    config.applyPresetType(type, ConfigSource::Default);
    return config;
}

std::string Config::generateDiff(const Config& other) const {
    return ConfigDiffer::generateDiff(*this, other);
}

std::string Config::generateHumanReadableDiff(const Config& other) const {
    return ConfigDiffer::generateHumanReadableDiff(*this, other);
}

bool Config::isCompatibleWith(const Config& other) const {
    return ConfigDiffer::areCompatible(*this, other);
}

std::vector<std::string> Config::getCompatibilityIssues(const Config& other) const {
    return ConfigDiffer::getCompatibilityIssues(*this, other);
}

Config Config::sanitize() const {
    return ConfigSanitizer::sanitize(*this);
}

Config Config::clone() const {
    Config clone;
    ConfigMerger::applyConfig(clone, *this, ConfigSource::Runtime);
    return clone;
}

Config Config::merge(const Config& other) const {
    return ConfigMerger::mergeConfigs(*this, other);
}

Config Config::merge(const Config& other, ConfigSource preference) const {
    return ConfigMerger::mergeConfigs(*this, other, preference);
}

Config Config::withSource(ConfigSource source) const {
    Config result = *this;
    for (auto& entry : result.pImpl->entries) {
        entry.source = source;
    }
    return result;
}

Config Config::withoutSource(ConfigSource source) const {
    Config result = *this;
    result.pImpl->entries.erase(
        std::remove_if(result.pImpl->entries.begin(), result.pImpl->entries.end(),
            [source](const ConfigEntry& entry) { return entry.source == source; }),
        result.pImpl->entries.end()
    );
    return result;
}

bool Config::hasAnySource(ConfigSource source) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& entry) { return entry.source == source; });
}

ConfigSource Config::getHighestPrioritySource() const {
    // Define priority order: CommandLine > File > Runtime > Default
    ConfigSource highest = ConfigSource::Default;
    int highestPriority = 3;
    
    for (const auto& entry : pImpl->entries) {
        int priority = 3; // Default
        if (entry.source == ConfigSource::CommandLine) priority = 0;
        else if (entry.source == ConfigSource::File) priority = 1;
        else if (entry.source == ConfigSource::Runtime) priority = 2;
        
        if (priority < highestPriority) {
            highestPriority = priority;
            highest = entry.source;
        }
    }
    
    return highest;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

Config Config::getConfigBySource(ConfigSource source) const {
    Config result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.set(entry.key, entry.value, source);
        }
    }
    return result;
}

Config Config::getConfigWithoutSource(ConfigSource source) const {
    Config result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source != source) {
            result.set(entry.key, entry.value, entry.source);
        }
    }
    return result;
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Config {\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    std::map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "  Entries by source:" << std::endl;
    for (const auto& pair : sourceCounts) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "  Keys: [";
    bool first = true;
    for (const auto& key : getKeys()) {
        if (!first) oss << ", ";
        oss << key;
        first = false;
    }
    oss << "]\n";
    oss << "}";
    
    return oss.str();
}

std::string Config::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& key : getKeys()) {
        if (!first) oss << ",\n";
        
        auto value = get(key);
        if (value.has_value()) {
            oss << "  \"" << key << "\": " << jsonValueToString(*value);
        }
        
        first = false;
    }
    
    oss << "\n}";
    return oss.str();
}

std::string Config::toYaml() const {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& key : getKeys()) {
        if (!first) oss << "\n";
        
        auto value = get(key);
        if (value.has_value()) {
            oss << key << ": " << yamlValueToString(*value);
        }
        
        first = false;
    }
    
    return oss.str();
}

std::string Config::jsonValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeJsonString(val) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << val;
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

std::string Config::yamlValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeYamlString(val) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << val;
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

std::string Config::escapeJsonString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string Config::escapeYamlString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        if (c == '\'') oss << "\\'";
        else oss << c;
    }
    return oss.str();
}

Config ConfigSanitizer::sanitize(const Config& config) {
    Config sanitized = config;
    
    // Create default schema for sanitization
    ConfigSchema schema = ConfigSchema::createDefaultSchema();
    
    // Apply sanitization
    for (auto& entry : sanitized.pImpl->entries) {
        if (schema.hasEntry(entry.key)) {
            const auto& schemaEntry = schema.getEntry(entry.key);
            sanitized.set(entry.key, ConfigSanitizer::sanitizeValue(entry.value, schemaEntry), ConfigSource::Runtime);
        }
    }
    
    return sanitized;
}

ConfigValue ConfigSanitizer::sanitizeValue(const ConfigValue& value, const ConfigSchemaEntry& entry) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, double>) {
            if (entry.range.first != 0.0 || entry.range.second != 0.0) {
                if (val < entry.range.first) val = entry.range.first;
                if (val > entry.range.second) val = entry.range.second;
            }
        }
    }, value);
    
    return value;
}

} // namespace nlm
