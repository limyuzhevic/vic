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
    // Check file extension for format
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < filepath.size()) {
        ext = filepath.substr(dotPos);
    }
    
    if (ext == ".json" || ext == ".json5" || ext == ".jsonc") {
        return loadFromJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Fall back to simple key=value format for backward compatibility
        return loadFromKeyValueFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // TODO: Implement JSON parsing using nlohmann/json or similar
    // For now, maintain backward compatibility with simple format
    return loadFromKeyValueFile(filepath);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML parsing using YAML-cpp or similar
    // For now, maintain backward compatibility with simple format
    return loadFromKeyValueFile(filepath);
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension for format
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos + 1 < filepath.size()) {
        ext = filepath.substr(dotPos);
    }
    
    if (ext == ".json" || ext == ".json5" || ext == ".jsonc") {
        return saveToJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        return saveToKeyValueFile(filepath);
    }
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    // TODO: Implement JSON serialization
    // For now, fall back to simple format
    return saveToKeyValueFile(filepath);
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    // TODO: Implement YAML serialization
    // For now, fall back to simple format
    return saveToKeyValueFile(filepath);
}

bool Config::saveToKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << '"' << arg[i] << '"';
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

void Config::setAdvancedConfig(const std::string& category, const std::string& key, 
                               const ConfigValue& value, ConfigSource source) {
    std::string fullKey = category + "." + key;
    set(fullKey, value, source);
}

std::optional<ConfigValue> Config::getAdvancedConfig(const std::string& category,
                                                      const std::string& key) const {
    std::string fullKey = category + "." + key;
    return get(fullKey);
}

std::vector<std::string> Config::getAdvancedConfigKeys(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(category + ".") == 0) {
            keys.push_back(entry.key.substr(category.length() + 1));
        }
    }
    return keys;
}

void Config::setNeuralConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("neural", key, value, source);
}

std::optional<ConfigValue> Config::getNeuralConfig(const std::string& key) const {
    return getAdvancedConfig("neural", key);
}

void Config::setPlasticityConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("plasticity", key, value, source);
}

std::optional<ConfigValue> Config::getPlasticityConfig(const std::string& key) const {
    return getAdvancedConfig("plasticity", key);
}

void Config::setMemoryConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("memory", key, value, source);
}

std::optional<ConfigValue> Config::getMemoryConfig(const std::string& key) const {
    return getAdvancedConfig("memory", key);
}

void Config::setNeuromodulationConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("neuromodulation", key, value, source);
}

std::optional<ConfigValue> Config::getNeuromodulationConfig(const std::string& key) const {
    return getAdvancedConfig("neuromodulation", key);
}

void Config::setCognitiveConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("cognitive", key, value, source);
}

std::optional<ConfigValue> Config::getCognitiveConfig(const std::string& key) const {
    return getAdvancedConfig("cognitive", key);
}

void Config::setDevelopmentConfig(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setAdvancedConfig("development", key, value, source);
}

std::optional<ConfigValue> Config::getDevelopmentConfig(const std::string& key) const {
    return getAdvancedConfig("development", key);
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

} // namespace nlm
