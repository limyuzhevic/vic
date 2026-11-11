// Advanced Configuration Implementation
// This file implements the enhanced configuration system with JSON/YAML support

#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<JsonConfigParser> jsonParser;
    std::shared_ptr<YamlConfigParser> yamlParser;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default parsers
    pImpl->jsonParser = ConfigParserFactory::createJsonParser();
    pImpl->yamlParser = ConfigParserFactory::createYamlParser();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFileFormat format = detectFileFormat(filepath);
    return loadFromFile(filepath, format);
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setLastError("Cannot open file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadWithParser(content, format);
}

bool Config::loadFromJson(const std::string& jsonString) {
    clear();
    return pImpl->jsonParser && pImpl->jsonParser->parse(jsonString, pImpl->entries);
}

bool Config::loadFromYaml(const std::string& yamlString) {
    clear();
    return pImpl->yamlParser && pImpl->yamlParser->parse(yamlString, pImpl->entries);
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
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
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

bool Config::validate() const {
    // Basic validation - can be extended with custom validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            setLastError("Empty key found in configuration");
            return false;
        }
        
        // Check for duplicate keys
        int count = std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (count > 1) {
            setLastError("Duplicate key found in configuration: " + entry.key);
            return false;
        }
    }
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        bool exists = false;
        for (auto& ourEntry : pImpl->entries) {
            if (ourEntry.key == entry.key) {
                // Update value but keep original source if present
                ourEntry.value = entry.value;
                if (ourEntry.source == ConfigSource::Default) {
                    ourEntry.source = entry.source;
                }
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            pImpl->entries.push_back(entry);
        }
    }
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    std::fill(std::begin(stats.entriesBySource), std::end(stats.entriesBySource), 0);
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
    }
    
    stats.isValid = validate();
    stats.validationError = getLastError();
    
    return stats;
}

bool Config::validateRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            setLastError("Missing required key: " + key);
            return false;
        }
    }
    return true;
}

ConfigFileFormat Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigFileFormat::Unknown;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return detectFormat(content);
}

ConfigFileFormat Config::detectFormat(const std::string& content) {
    if (isJsonFile(content)) {
        return ConfigFileFormat::JSON;
    } else if (isYamlFile(content)) {
        return ConfigFileFormat::YAML;
    } else if (isSimpleKeyValueFile(content)) {
        return ConfigFileFormat::SimpleKeyValue;
    }
    return ConfigFileFormat::Unknown;
}

void Config::setJsonParser(std::shared_ptr<JsonConfigParser> parser) {
    pImpl->jsonParser = std::move(parser);
}

void Config::setYamlParser(std::shared_ptr<YamlConfigParser> parser) {
    pImpl->yamlParser = std::move(parser);
}

bool Config::loadWithParser(const std::string& content, ConfigFileFormat format) {
    switch (format) {
        case ConfigFileFormat::JSON:
            return loadFromJson(content);
        case ConfigFileFormat::YAML:
            return loadFromYaml(content);
        case ConfigFileFormat::SimpleKeyValue:
            // Use the existing simple parser logic
            std::vector<std::string> lines;
            std::stringstream ss(content);
            std::string line;
            while (std::getline(ss, line)) {
                lines.push_back(line);
            }
            
            for (const auto& line : lines) {
                std::string trimmedLine = trim(line);
                if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/') {
                    continue;
                }
                
                size_t pos = trimmedLine.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(trimmedLine.substr(0, pos));
                    std::string value = trim(trimmedLine.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        default:
            setLastError("Unsupported file format");
            return false;
    }
}

bool Config::isJsonFile(const std::string& content) {
    // Simple JSON detection - first non-whitespace char is { or [
    size_t start = content.find_first_not_of(" \t\n\r");
    return start != std::string::npos && (content[start] == '{' || content[start] == '[');
}

bool Config::isYamlFile(const std::string& content) {
    // Simple YAML detection - look for key: value patterns
    std::stringstream ss(content);
    std::string line;
    size_t colonCount = 0;
    
    while (std::getline(ss, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        
        // Count colons in the line (simple heuristic)
        for (char c : trimmed) {
            if (c == ':') colonCount++;
        }
    }
    
    return colonCount >= 2;
}

bool Config::isSimpleKeyValueFile(const std::string& content) {
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
            continue;
        }
        
        // Look for key=value pattern
        size_t pos = trimmed.find('=');
        if (pos == std::string::npos || pos == 0) {
            return false;
        }
    }
    
    return true;
}

void Config::setLastError(const std::string& error) {
    pImpl->lastError = error;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

// Default JSON parser implementation
bool DefaultJsonConfigParser::parse(const std::string& jsonString, std::vector<ConfigEntry>& entries) {
    // In a real implementation, this would use nlohmann::json or similar
    // For now, return false to indicate JSON parsing is not implemented
    // This allows the basic key-value format to continue working
    return false;
}

// Default YAML parser implementation
bool DefaultYamlConfigParser::parse(const std::string& yamlString, std::vector<ConfigEntry>& entries) {
    // In a real implementation, this would use yaml-cpp or similar
    // For now, return false to indicate YAML parsing is not implemented
    // This allows the basic key-value format to continue working
    return false;
}

// Parser factory implementation
std::shared_ptr<JsonConfigParser> ConfigParserFactory::createJsonParser() {
    return std::make_shared<DefaultJsonConfigParser>();
}

std::shared_ptr<YamlConfigParser> ConfigParserFactory::createYamlParser() {
    return std::make_shared<DefaultYamlConfigParser>();
}

ConfigFileFormat ConfigParserFactory::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigFileFormat::Unknown;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Config::detectFormat(buffer.str());
}

std::string ConfigParserFactory::getFormatExtension(ConfigFileFormat format) {
    switch (format) {
        case ConfigFileFormat::JSON: return ".json";
        case ConfigFileFormat::YAML: return ".yaml";
        case ConfigFileFormat::TOML: return ".toml";
        case ConfigFileFormat::SimpleKeyValue: return ".cfg";
        default: return ".txt";
    }
}

} // namespace nlm
