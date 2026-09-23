#include "Config.hpp"
#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validationEnabled;
    bool environmentExpansionEnabled;
    bool typeCoercionEnabled;
};

Config::Config() : pImpl(std::make_unique<Impl>()), advancedConfig(std::make_unique<ConfigManager>()) {
    pImpl->validationEnabled = true;
    pImpl->environmentExpansionEnabled = true;
    pImpl->typeCoercionEnabled = true;
    advancedConfig->setValidation(true);
    advancedConfig->setEnvironmentExpansion(true);
    advancedConfig->setTypeCoercion(true);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension == ".json" || extension == ".yaml" || extension == ".yml") {
        return advancedConfig->loadFromFile(filepath, pImpl->validationEnabled);
    } else {
        // Use legacy key=value format
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
}

bool Config::loadFromArgs(int argc, char** argv) {
    return advancedConfig->loadFromArgs(argc, argv);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension == ".json" || extension == ".yaml" || extension == ".yml") {
        return advancedConfig->saveToFile(filepath, true);
    } else {
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
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Try advanced config first
    auto advancedValue = advancedConfig->get<T>(key);
    if (advancedValue.has_value()) {
        return advancedValue;
    }
    
    // Fallback to legacy implementation
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
    // Try advanced config first
    if (advancedConfig->has(key)) {
        auto advancedValue = advancedConfig->getOr<T>(key, defaultValue);
        if (advancedValue != defaultValue) {
            return advancedValue;
        }
    }
    
    // Fallback to legacy implementation
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Update both legacy and advanced config
    if (pImpl->validationEnabled && advancedConfig->has(key)) {
        advancedConfig->set(key, value, pImpl->validationEnabled);
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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

void Config::set(const std::string& key, const std::vector<float>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return advancedConfig->has(key) || std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                                                   [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    advancedConfig->remove(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    auto advancedKeys = advancedConfig->getKeys();
    std::vector<std::string> legacyKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (std::find(advancedKeys.begin(), advancedKeys.end(), entry.key) == advancedKeys.end()) {
            legacyKeys.push_back(entry.key);
        }
    }
    
    advancedKeys.insert(advancedKeys.end(), legacyKeys.begin(), legacyKeys.end());
    return advancedKeys;
}

void Config::clear() {
    advancedConfig->clear();
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "=== Legacy Configuration (" << pImpl->entries.size() << " entries) ===" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << " = " << "PLACEHOLDER_VALUE (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    oss << std::endl;
    oss << "=== Advanced Configuration ===" << std::endl;
    oss << advancedConfig->getSummary();
    return oss.str();
}

ConfigManager& Config::getAdvancedConfig() {
    return *advancedConfig;
}

const ConfigManager& Config::getAdvancedConfig() const {
    return *advancedConfig;
}

void Config::setAdvancedSchema(const ConfigSchema& schema) {
    advancedConfig->setSchema(schema);
}

const ConfigSchema& Config::getAdvancedSchema() const {
    return advancedConfig->getSchema();
}

void Config::setValidation(bool enabled) {
    pImpl->validationEnabled = enabled;
    advancedConfig->setValidation(enabled);
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled && advancedConfig->isValidationEnabled();
}

void Config::setEnvironmentExpansion(bool enabled) {
    pImpl->environmentExpansionEnabled = enabled;
    advancedConfig->setEnvironmentExpansion(enabled);
}

bool Config::isEnvironmentExpansionEnabled() const {
    return pImpl->environmentExpansionEnabled && advancedConfig->isEnvironmentExpansionEnabled();
}

void Config::setTypeCoercion(bool enabled) {
    pImpl->typeCoercionEnabled = enabled;
    advancedConfig->setTypeCoercion(enabled);
}

bool Config::isTypeCoercionEnabled() const {
    return pImpl->typeCoercionEnabled && advancedConfig->isTypeCoercionEnabled();
}

Config::operator ConfigManager&() {
    return *advancedConfig;
}

Config::operator const ConfigManager&() const {
    return *advancedConfig;
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
