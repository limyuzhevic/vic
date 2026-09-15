#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSection> sections;
    ConfigSchema schema;
    std::map<std::string, ConfigValue> defaults;
    std::map<std::string, ConfigValue> runtimeDefaults;
    std::map<std::string, ConfigSource> sources;
};

Config::Config() : pImpl(std::make_unique<Impl>()), version("1.0.0"), context("default") {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with multiple formats
bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format detection
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromText(filepath);
    }
}

// Load from JSON file
bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Convert JSON to config entries
        fromJSON(j, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Load from YAML file
bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        fromYAML(doc, "", ConfigSource::File);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Load from text format (backward compatible)
bool Config::loadFromText(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
            
            // Support nested keys like "a.b.c = value"
            if (key.find('.') != std::string::npos) {
                // Convert to flat key for backward compatibility
                set(key, parseValue(value), ConfigSource::File);
            } else {
                set(key, parseValue(value), ConfigSource::File);
            }
        }
    }
    
    return true;
}

// Convert JSON to config entries
void Config::fromJSON(const nlohmann::json& j, const std::string& prefix, ConfigSource source) {
    for (const auto& item : j.items()) {
        std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
        
        if (item.is_object()) {
            fromJSON(item, key, source);
        } else {
            ConfigValue value = jsonToConfigValue(item);
            set(key, value, source);
        }
    }
}

// Convert YAML to config entries
void Config::fromYAML(const YAML::Node& node, const std::string& prefix, ConfigSource source) {
    for (const auto& item : node) {
        std::string key;
        if (item.first.IsScalar()) {
            key = item.first.as<std::string>();
        } else {
            continue;
        }
        
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (item.second.IsMap()) {
            fromYAML(item.second, fullKey, source);
        } else {
            ConfigValue value = yamlToConfigValue(item.second);
            set(fullKey, value, source);
        }
    }
}

// Convert JSON value to ConfigValue
ConfigValue Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<int64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (j[0].is_number()) {
                std::vector<double> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (j[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        return std::vector<int>();
    }
    return "";
}

// Convert YAML value to ConfigValue
ConfigValue Config::yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string& value = node.as<std::string>();
        // Try to detect type
        if (value == "true" || value == "false") {
            return value == "true";
        } else if (value.find('.') != std::string::npos) {
            try {
                double d = std::stod(value);
                return d;
            } catch (...) {
                return value;
            }
        } else {
            try {
                int i = std::stoi(value);
                return i;
            } catch (...) {
                return value;
            }
        }
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            if (node[0].IsScalar()) {
                std::vector<std::string> vec;
                for (const auto& item : node) {
                    vec.push_back(item.as<std::string>());
                }
                return vec;
            }
        }
        return std::vector<std::string>();
    }
    return "";
}

// Parse string value to appropriate type
ConfigValue Config::parseValue(const std::string& valueStr) {
    // Trim whitespace
    std::string value = trim(valueStr);
    
    // Check for quoted string
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\'')) {
        return value.substr(1, value.size() - 2);
    }
    
    // Try to detect type
    if (value == "true" || value == "false") {
        return value == "true";
    } else if (value.find('.') != std::string::npos) {
        try {
            double d = std::stod(value);
            return d;
        } catch (...) {
            return value;
        }
    } else {
        try {
            int i = std::stoi(value);
            return i;
        } catch (...) {
            try {
                int64_t l = std::stoll(value);
                return l;
            } catch (...) {
                return value;
            }
        }
    }
}

// Convert ConfigValue to string
std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
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
                set(key, parseValue(value), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, parseValue(value), ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        return saveToText(filepath);
    }
}

// Save to JSON file
bool Config::saveToJSON(const std::string& filepath) const {
    nlohmann::json j;
    
    // Convert config entries to JSON
    for (const auto& entry : pImpl->entries) {
        setJSONValue(j, entry.key, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(4);
    return true;
}

// Save to YAML file
bool Config::saveToYAML(const std::string& filepath) const {
    YAML::Emitter out;
    
    // Convert config entries to YAML
    out << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        setYAMLValue(out, entry.key, entry.value);
    }
    out << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << out.c_str();
    return true;
}

// Save to text format
bool Config::saveToText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << valueToString(entry.value) << "\n\n";
    }
    
    return true;
}

// Set JSON value recursively
void Config::setJSONValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> segments = extractKeySegments(key, '.');
    
    nlohmann::json* current = &j;
    for (size_t i = 0; i < segments.size() - 1; ++i) {
        if (!current->contains(segments[i])) {
            (*current)[segments[i]] = nlohmann::json::object();
        }
        current = &((*current)[segments[i]]);
    }
    
    (*current)[segments.back()] = configValueToJSON(value);
}

// Set YAML value recursively
void Config::setYAMLValue(YAML::Emitter& out, const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> segments = extractKeySegments(key, '.');
    
    // YAML emitter doesn't support nested setting easily, so we'll use a different approach
    // For simplicity, we'll just output flat key-value pairs
    out << YAML::Key << key << YAML::Value;
    yamlValueToEmitter(out, value);
    out << YAML::Newline;
}

// Convert ConfigValue to JSON
nlohmann::json Config::configValueToJSON(const ConfigValue& value) const {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

// Convert value to YAML emitter
void Config::yamlValueToEmitter(YAML::Emitter& out, const ConfigValue& value) const {
    std::visit([&out](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            out << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            out << YAML::DoubleQuoted << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            out << YAML::Flow << arg;
        }
    }, value);
}

// Enhanced section management
void Config::addSection(const std::string& name, const std::string& description) {
    pImpl->sections[name] = ConfigSection(name, description);
}

bool Config::hasSection(const std::string& name) const {
    return pImpl->sections.find(name) != pImpl->sections.end();
}

ConfigSection* Config::getSection(const std::string& name) {
    auto it = pImpl->sections.find(name);
    if (it != pImpl->sections.end()) {
        return &it->second;
    }
    return nullptr;
}

const ConfigSection* Config::getSection(const std::string& name) const {
    auto it = pImpl->sections.find(name);
    if (it != pImpl->sections.end()) {
        return &it->second;
    }
    return nullptr;
}

void Config::removeSection(const std::string& name) {
    pImpl->sections.erase(name);
}

std::vector<std::string> Config::getSectionNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->sections.size());
    for (const auto& pair : pImpl->sections) {
        names.push_back(pair.first);
    }
    return names;
}

// Schema management
void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

ValidationResult Config::validate() const {
    // Convert entries to map for validation
    std::map<std::string, ConfigEntry> entryMap;
    for (const auto& entry : pImpl->entries) {
        entryMap[entry.key] = entry;
    }
    
    // Validate against schema if set
    if (!pImpl->schema.schemaEntries.empty()) {
        ValidationResult result = pImpl->schema.validate(entryMap);
        if (!result.valid) {
            return result;
        }
    }
    
    // Additional validations can be added here
    return ValidationResult();
}

// Default values management
void Config::setDefault(const std::string& key, const ConfigValue& value) {
    pImpl->defaults[key] = value;
}

std::optional<ConfigValue> Config::getDefault(const std::string& key) const {
    auto it = pImpl->defaults.find(key);
    if (it != pImpl->defaults.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Inheritance
void Config::inheritFrom(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

// Export/Import
bool Config::exportToFile(const std::string& filepath, const std::string& format) const {
    return saveToFile(filepath); // saveToFile already supports multiple formats
}

bool Config::importFromFile(const std::string& filepath) {
    clear();
    return loadFromFile(filepath);
}

// Versioning
void Config::setVersion(const std::string& version) {
    this->version = version;
}

// Context-aware configuration
void Config::setContext(const std::string& context) {
    this->context = context;
}

// Configuration merging
void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (override || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

// Template implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Check defaults
        auto defaultIt = pImpl->defaults.find(key);
        if (defaultIt != pImpl->defaults.end()) {
            try {
                return std::get<T>(defaultIt->second);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Nested configuration access (key.format.key.nested)
template<typename T>
std::optional<T> Config::get(const std::string& key, char delimiter) const {
    // Check sections first
    for (const auto& section : pImpl->sections) {
        std::optional<T> value = section.second.get<T>(key);
        if (value.has_value()) {
            return value;
        }
    }
    
    // Check for exact match with nested key
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Check defaults
        auto defaultIt = pImpl->defaults.find(key);
        if (defaultIt != pImpl->defaults.end()) {
            try {
                return std::get<T>(defaultIt->second);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    // Try to get from current entry
    try {
        T value = std::get<T>(it->value);
        return value;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Get nested value with default
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, char delimiter) const {
    auto val = get<T>(key, delimiter);
    return val.has_value() ? val.value() : defaultValue;
}

// Get nested value with default (using dot as default delimiter)
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return getOr<T>(key, defaultValue, '.');
}

// Check if nested key exists
bool Config::has(const std::string& key, char delimiter) const {
    // Check sections first
    for (const auto& section : pImpl->sections) {
        if (section.second.has(key)) {
            return true;
        }
    }
    
    // Check for exact match with nested key
    if (std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; })) {
        return true;
    }
    
    // Parse nested key like "format.key.nested"
    std::vector<std::string> segments = extractKeySegments(key, delimiter);
    
    // Check for partial match (first segment)
    if (!segments.empty() && std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&segments](const ConfigEntry& e) { return e.key == segments[0]; })) {
        return true;
    }
    
    return false;
}

// Legacy has function
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Helper function to extract key segments from nested notation
std::vector<std::string> Config::extractKeySegments(const std::string& key, char delimiter) {
    std::vector<std::string> segments;
    size_t start = 0;
    size_t end = key.find(delimiter);
    
    while (end != std::string::npos) {
        segments.push_back(key.substr(start, end - start));
        start = end + 1;
        end = key.find(delimiter, start);
    }
    segments.push_back(key.substr(start));
    
    return segments;
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
    
    // Update section if it exists
    std::string sectionName = "general";
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        sectionName = key.substr(0, dotPos);
    }
    
    if (pImpl->sections.find(sectionName) != pImpl->sections.end()) {
        pImpl->sections[sectionName].addEntry(key, value, source);
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
    
    // Also remove from sections
    for (auto& section : pImpl->sections) {
        section.second.remove(key);
    }
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
    pImpl->sections.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (v" << version << ") Context: " << context << "\n";
    oss << "  Entries: " << pImpl->entries.size() << "\n";
    oss << "  Sections: " << pImpl->sections.size() << "\n";
    oss << "  Schema: " << pImpl->schema.schemaEntries.size() << " entries\n";
    oss << "  Defaults: " << pImpl->defaults.size() << " entries\n\n";
    
    // Show sections
    for (const auto& section : pImpl->sections) {
        oss << section.second.summary() << "\n";
    }
    
    // Show entries
    oss << "Global Entries:\n";
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