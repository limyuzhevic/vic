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

// Factory method implementations
Config Config::createDefaultConfig() {
    Config config;
    config.set("agent.type", std::string("default"));
    config.set("agent.learning.enabled", true);
    config.set("agent.exploration.enabled", true);
    config.set("brain.plasticity.enabled", true);
    config.set("memory.working.capacity", size_t(1000));
    config.set("memory.episodic.enabled", true);
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    config.set("agent.type", std::string("minimal"));
    
    return config;
}

Config Config::createProductionConfig() {
    Config config;
    config.set("agent.type", std::string("production"));
    config.set("agent.performance.optimization", true);
    config.set("agent.memory.management.enabled", true);
    config.set("brain.plasticity.enabled", false);
    config.set("brain.development.enabled", false);
    config.set("agent.validation.strict", true);
    
    return config;
}

Config Config::createDevelopmentConfig() {
    Config config;
    config.set("agent.type", std::string("development"));
    config.set("agent.learning.enabled", true);
    config.set("agent.exploration.enabled", true);
    config.set("agent.debug.mode", true);
    config.set("agent.logging.level", std::string("debug"));
    config.set("brain.plasticity.enabled", true);
    config.set("brain.development.enabled", true);
    
    return config;
}

// Preset application
void Config::applyPreset(const std::string& presetName) {
    if (presetName == "default") {
        *this = createDefaultConfig();
    } else if (presetName == "minimal") {
        *this = createMinimalConfig();
    } else if (presetName == "production") {
        *this = createProductionConfig();
    } else if (presetName == "development") {
        *this = createDevelopmentConfig();
    }
}

// Clone implementation
Config Config::clone() const {
    Config cloned;
    cloned.pImpl->entries = pImpl->entries;
    return cloned;
}

// Hash implementation
std::hash<std::string> hasher;
size_t Config::getHash() const {
    size_t hash = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::string entryStr = entry.key;
        std::visit([&entryStr](auto&& arg) {
            std::ostringstream oss;
            oss << arg;
            entryStr += "=" + oss.str();
        }, entry.value);
        
        hash ^= hasher(entryStr);
    }
    
    return hash;
}

// Equality implementation
bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

// Filter implementation
Config Config::filter(const std::string& pattern) const {
    Config filtered;
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(toLower(pattern)) != std::string::npos) {
            filtered.pImpl->entries.push_back(entry);
        }
    }
    
    return filtered;
}

// Validation implementation
std::vector<std::string> Config::validate(const std::string& schema) const {
    std::vector<std::string> errors;
    
    // TODO: Implement schema validation
    // For now, return empty list (no errors)
    
    return errors;
}

// Statistics implementation
std::map<std::string, double> Config::getStatistics() const {
    std::map<std::string, double> stats;
    
    stats["entry_count"] = static_cast<double>(pImpl->entries.size());
    
    // Count by source type
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    stats["default_count"] = static_cast<double>(defaultCount);
    stats["file_count"] = static_cast<double>(fileCount);
    stats["command_line_count"] = static_cast<double>(commandLineCount);
    stats["runtime_count"] = static_cast<double>(runtimeCount);
    
    return stats;
}

// Metadata implementation
std::map<std::string, std::string> Config::getMetadata() const {
    std::map<std::string, std::string> metadata;
    
    // TODO: Implement metadata storage
    // For now, return empty map
    
    return metadata;
}

void Config::setMetadata(const std::string& key, const std::string& value) {
    // TODO: Implement metadata storage
    // For now, placeholder
}

void Config::addTag(const std::string& tag) {
    // TODO: Implement tag storage
    // For now, placeholder
}

std::vector<std::string> Config::getTags() const {
    std::vector<std::string> tags;
    
    // TODO: Implement tag retrieval
    // For now, return empty vector
    
    return tags;
}

bool Config::hasTag(const std::string& tag) const {
    // TODO: Implement tag checking
    // For now, return false
    
    return false;
}

// Merge implementation
void Config::merge(const Config& other, bool overwrite) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

// Group operations
std::vector<std::string> Config::getKeysInGroup(const std::string& groupName) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(toLower(groupName)) != std::string::npos) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::setGroup(const std::string& groupName, const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        std::string key = groupName + "." + pair.first;
        set(key, pair.second);
    }
}

std::map<std::string, std::optional<ConfigValue>> Config::getGroup(const std::string& groupName) const {
    std::map<std::string, std::optional<ConfigValue>> groupValues;
    std::string prefix = groupName + ".";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            std::string key = entry.key.substr(prefix.size());
            groupValues[key] = entry.value;
        }
    }
    
    return groupValues;
}

// Batch operations
void Config::setValues(const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        set(pair.first, pair.second);
    }
}

std::map<std::string, std::optional<ConfigValue>> Config::getValues(const std::vector<std::string>& keys) const {
    std::map<std::string, std::optional<ConfigValue>> result;
    
    for (const auto& key : keys) {
        result[key] = get<ConfigValue>(key);
    }
    
    return result;
}

std::map<std::string, bool> Config::hasKeys(const std::vector<std::string>& keys) const {
    std::map<std::string, bool> result;
    
    for (const auto& key : keys) {
        result[key] = has(key);
    }
    
    return result;
}

// Explicit template instantiations for existing methods
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

// Explicit template instantiations for new methods
template std::optional<ConfigValue> Config::get<ConfigValue>(const std::string&) const;
template ConfigValue Config::getOr<ConfigValue>(const std::string&, const ConfigValue&) const;

} // namespace nlm
