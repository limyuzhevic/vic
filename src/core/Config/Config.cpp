// Config.cpp - Improved configuration management implementation
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <unordered_map>
#include <ctime>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Index for faster lookups
    std::unordered_map<size_t, size_t> keyIndex;
    // Sections for organizational access
    std::map<std::string, std::vector<ConfigEntry>> sections;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromJSON("{\n        \"random_seed\": 42,\n        \"simulation_timestep\": 0.001,\n        \"neuron_count\": 1000,\n        \"region_count\": 1,\n        \"connection_probability\": 0.1,\n        \"stdp_ltp_weight\": 0.01,\n        \"stdp_ltd_weight\": 0.012,\n        \"stdp_tau\": 20.0,\n        \"synaptogenesis_rate\": 0.0001,\n        \"pruning_rate\": 0.00001,\n        \"replay_interval\": 100,\n        \"consolidation_interval\": 1000,\n        \"checkpoint_dir\": \"./checkpoints\"\n    }");
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        // Simple JSON parser for basic key-value pairs
        std::map<std::string, std::string> parsed = parseJSON(jsonStr);
        for (const auto& pair : parsed) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        return false;
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
    
    // Write as simple key=value format
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
        
        // Update index
        size_t hash = hashKey(key);
        pImpl->keyIndex[hash] = std::distance(pImpl->entries.begin(), it);
        
        // Update section
        std::string section = extractSection(key);
        pImpl->sections[section].push_back(*it);
    } else {
        ConfigEntry entry(key, value, source);
        entry.required = false;
        
        pImpl->entries.push_back(entry);
        
        // Update index
        size_t hash = hashKey(key);
        pImpl->keyIndex[hash] = pImpl->entries.size() - 1;
        
        // Update section
        std::string section = extractSection(key);
        pImpl->sections[section].push_back(entry);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source);
}

void Config::setFloat(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::hasType(const std::string& key, ConfigValue::variant_type type) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return isValidValueType(it->value, type);
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    pImpl->entries.erase(it, pImpl->entries.end());
    
    // Rebuild index and sections
    pImpl->keyIndex.clear();
    pImpl->sections.clear();
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        size_t hash = hashKey(pImpl->entries[i].key);
        pImpl->keyIndex[hash] = i;
        
        std::string section = extractSection(pImpl->entries[i].key);
        pImpl->sections[section].push_back(pImpl->entries[i]);
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

const std::vector<ConfigEntry>& Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
    pImpl->sections.clear();
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.required) {
            // Check if value is valid for the type
            if (std::holds_alternative<std::monostate>(entry.value)) {
                errors.push_back("Required key \"" + entry.key + "\" has no value");
            }
        }
    }
    
    return errors;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
            size_t hash = hashKey(entry.key);
            pImpl->keyIndex[hash] = pImpl->entries.size() - 1;
            
            std::string section = extractSection(entry.key);
            pImpl->sections[section].push_back(entry);
        } else {
            // Update existing entry, but keep higher priority source
            if (resolveConflict(it->source, entry.source) == entry.source) {
                it->value = entry.value;
                it->source = entry.source;
                it->description = entry.description;
                it->required = entry.required;
            }
        }
    }
}

bool Config::applyConstraints(const std::map<std::string, std::function<bool(const ConfigValue&)>>& constraints) {
    bool allValid = true;
    for (const auto& constraint : constraints) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&constraint](const ConfigEntry& e) { return e.key == constraint.first; });
        
        if (it != pImpl->entries.end()) {
            if (!constraint.second(it->value)) {
                allValid = false;
            }
        }
    }
    return allValid;
}

size_t Config::size() const {
    return pImpl->entries.size();
}

bool Config::empty() const {
    return pImpl->entries.empty();
}

// Iterator implementations
class Config::iterator {
    // TODO: Implement iterator for Config
};

class Config::const_iterator {
    // TODO: Implement const_iterator for Config
};

Config::iterator Config::begin() {
    // TODO: Implement
    return iterator();
}

Config::iterator Config::end() {
    // TODO: Implement
    return iterator();
}

Config::const_iterator Config::begin() const {
    // TODO: Implement
    return const_iterator();
}

Config::const_iterator Config::end() const {
    // TODO: Implement
    return const_iterator();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

void Config::overrideWithArgs(int argc, char** argv) {
    // Save current entries
    std::vector<ConfigEntry> backup = pImpl->entries;
    
    // Clear and reload with args (overriding)
    clear();
    loadFromArgs(argc, argv);
}

std::map<std::string, std::map<std::string, ConfigValue>> Config::getBySection() const {
    std::map<std::string, std::map<std::string, ConfigValue>> result;
    
    for (const auto& section : pImpl->sections) {
        for (const auto& entry : section.second) {
            // Remove section prefix from key
            std::string key = entry.key;
            size_t pos = key.find('.');
            if (pos != std::string::npos) {
                key = key.substr(pos + 1);
            }
            result[section.first][key] = entry.value;
        }
    }
    
    return result;
}

void Config::setSection(const std::string& section, const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        std::string key = section + "." + pair.first;
        set(key, pair.second);
    }
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

std::string Config::toJSON(const ConfigValue& value) {
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
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue Config::fromJSON(const std::string& str) {
    // Simple implementation - assumes str is already a parsed value
    return str;
}

std::map<std::string, std::string> Config::parseJSON(const std::string& jsonStr) {
    std::map<std::string, std::string> result;
    
    // Very simple parser for demonstration
    // In production, use a proper JSON library
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Find next key (after { or ,)
        pos = jsonStr.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyStart = pos;
        pos = jsonStr.find('"', keyStart + 1);
        if (pos == std::string::npos) break;
        
        std::string key = jsonStr.substr(keyStart + 1, pos - keyStart - 1);
        
        // Find value
        pos = jsonStr.find(':', pos);
        if (pos == std::string::npos) break;
        
        // Skip whitespace
        size_t valueStart = jsonStr.find_first_not_of(" \t\n\r", pos + 1);
        if (valueStart == std::string::npos) break;
        
        // Find end of value (could be , or })
        size_t valueEnd = jsonStr.find_first_of(",}", valueStart);
        if (valueEnd == std::string::npos) break;
        
        std::string value = jsonStr.substr(valueStart, valueEnd - valueStart);
        
        // Remove quotes from value if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        result[key] = value;
        
        // Move to next position
        pos = valueEnd + 1;
    }
    
    return result;
}

bool Config::isValidValueType(const ConfigValue& value, ConfigValue::variant_type type) const {
    return std::holds_alternative<std::decay_t<decltype(type)>>(value);
}

size_t Config::hashKey(const std::string& key) {
    size_t hash = 0;
    for (char c : key) {
        hash = (hash * 31) + static_cast<size_t>(c);
    }
    return hash;
}

ConfigSource Config::resolveConflict(ConfigSource source1, ConfigSource source2) {
    // Lower numeric value wins (Default < File < CommandLine < Runtime < Programmatic)
    return (static_cast<int>(source1) < static_cast<int>(source2)) ? source1 : source2;
}

std::string Config::extractSection(const std::string& key) {
    size_t pos = key.find('.');
    if (pos != std::string::npos) {
        return key.substr(0, pos);
    }
    return "";
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