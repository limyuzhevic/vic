#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Schema support
    struct SchemaEntry {
        std::string key;
        std::string description;
        ConfigValue defaultValue;
        ConfigSource source;
        bool required;
        std::string validationRegex;
        
        SchemaEntry() : key(), description(), defaultValue(), source(ConfigSource::Default), required(false) {}
        SchemaEntry(const std::string& k, const std::string& desc, const ConfigValue& def, 
                   ConfigSource src = ConfigSource::Default, bool req = false, const std::string& regex = "")
            : key(k), description(desc), defaultValue(def), source(src), required(req), validationRegex(regex) {}
    };
    std::vector<SchemaEntry> schema;
    
    // Statistics
    size_t defaultCount = 0;
    size_t fileCount = 0;
    size_t commandLineCount = 0;
    
    // File format preference
    FileFormat fileFormat = FileFormat::KeyValue;
    
    // Help text
    std::string helpText = "NLM Configuration System\n"
                          "======================\n\n"
                          "This configuration system supports:\n"
                          "  - Key-value pair files (key=value format)\n"
                          "  - Command line arguments (--key=value)\n"
                          "  - Runtime setting calls\n\n"
                          "Common configuration options:\n"
                          "  random_seed: Random number generator seed (int)\n"
                          "  neuron_count: Number of neurons to create (size_t)\n"
                          "  region_count: Number of brain regions (size_t)\n"
                          "  connection_probability: Probability of connections (float)\n"
                          "  simulation_timestep: Time step duration (double)\n\n"
                          "For detailed information, use --help.";
    
    bool validate(const ConfigEntry& entry) const {
        // Check if entry exists in schema
        auto it = std::find_if(schema.begin(), schema.end(),
                               [&entry](const SchemaEntry& se) { return se.key == entry.key; });
        
        if (it != schema.end()) {
            // Validate against schema
            if (it->required && !hasRequiredType(entry.value)) {
                return false;
            }
            if (!it->validationRegex.empty() && !validateWithRegex(entry.value, it->validationRegex)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool hasRequiredType(const ConfigValue& value) const {
        // Check if value has any type that could be considered "non-empty"
        return !std::holds_alternative<std::monostate>(value);
    }
    
    bool validateWithRegex(const ConfigValue& value, const std::string& regex) const {
        // Simple placeholder implementation - would need regex library
        return true;
    }
    
    std::string getSourceName(ConfigSource source) const {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }
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
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            pImpl->fileCount++;
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
                pImpl->commandLineCount++;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            pImpl->commandLineCount++;
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
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
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
        // Update statistics
        if (source == ConfigSource::Default) pImpl->defaultCount++;
        else if (source == ConfigSource::File) pImpl->fileCount++;
        else if (source == ConfigSource::CommandLine) pImpl->commandLineCount++;
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
    pImpl->defaultCount = 0;
    pImpl->fileCount = 0;
    pImpl->commandLineCount = 0;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, "
        << pImpl->defaultCount << " default, " << pImpl->fileCount << " file, "
        << pImpl->commandLineCount << " command-line):\n";
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
        oss << "] (" << pImpl->getSourceName(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    return true;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Check if entry exists in schema
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                           [&entry](const Impl::SchemaEntry& se) { return se.key == entry.key; });
    
    if (it != pImpl->schema.end()) {
        // Validate against schema
        if (it->required && !hasRequiredType(entry.value)) {
            return false;
        }
        if (!it->validationRegex.empty() && !validateWithRegex(entry.value, it->validationRegex)) {
            return false;
        }
    }
    
    return true;
}

bool Config::hasRequiredType(const ConfigValue& value) const {
    // Check if value has any type that could be considered "non-empty"
    return !std::holds_alternative<std::monostate>(value);
}

bool Config::validateWithRegex(const ConfigValue& value, const std::string& regex) const {
    // Simple placeholder implementation - would need regex library
    return true;
}

void Config::addSchemaEntry(const std::string& key, const std::string& description, 
                           const ConfigValue& defaultValue, ConfigSource source, 
                           bool required, const std::string& validationRegex) {
    pImpl->schema.emplace_back(key, description, defaultValue, source, required, validationRegex);
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                           [&key](const Impl::SchemaEntry& se) { return se.key == key; });
    if (it != pImpl->schema.end()) {
        return it->description;
    }
    return "";
}

ConfigValue Config::getDefaultValue(const std::string& key) const {
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                           [&key](const Impl::SchemaEntry& se) { return se.key == key; });
    if (it != pImpl->schema.end()) {
        return it->defaultValue;
    }
    return ConfigValue();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                           [&key](const Impl::SchemaEntry& se) { return se.key == key; });
    if (it != pImpl->schema.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

void Config::printHelp() const {
    std::cout << getHelpText() << std::endl;
}

std::string Config::getHelpText() const {
    return pImpl->helpText;
}

std::unique_ptr<Config> Config::clone() const {
    auto cloned = std::make_unique<Config>();
    cloned->pImpl->entries = pImpl->entries;
    cloned->pImpl->schema = pImpl->schema;
    cloned->pImpl->defaultCount = pImpl->defaultCount;
    cloned->pImpl->fileCount = pImpl->fileCount;
    cloned->pImpl->commandLineCount = pImpl->commandLineCount;
    cloned->pImpl->fileFormat = pImpl->fileFormat;
    cloned->pImpl->helpText = pImpl->helpText;
    return cloned;
}

void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (override || !has(entry.key)) {
            pImpl->entries.push_back(entry);
        }
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getDefaultEntryCount() const {
    return pImpl->defaultCount;
}

size_t Config::getFileEntryCount() const {
    return pImpl->fileCount;
}

size_t Config::getCommandLineEntryCount() const {
    return pImpl->commandLineCount;
}

void Config::setFileFormat(FileFormat format) {
    pImpl->fileFormat = format;
}

Config::FileFormat Config::getFileFormat() const {
    return pImpl->fileFormat;
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

bool Config::isJsonValue(const std::string& value) {
    // Simple JSON detection
    if (value.empty()) return false;
    char first = value.front();
    char last = value.back();
    return (first == '{' && last == '}') || (first == '[' && last == ']') ||
           (first == '"' && last == '"') || (first == '\'' && last == '\'');
}

bool Config::isYamlValue(const std::string& value) {
    // Simple YAML detection
    return value.find(':') != std::string::npos;
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
