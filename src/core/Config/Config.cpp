#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigProfile> profiles;
    std::string schemaName;
    bool useJsonFormat = false;
    std::vector<std::string> protectedKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool useJsonFormat) {
    pImpl->useJsonFormat = useJsonFormat;
    
    if (useJsonFormat) {
        // Try to parse as JSON first
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Convert JSON to config entries
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(*it);
                setWithSource(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            // Fall back to simple format
            pImpl->useJsonFormat = false;
        }
    }
    
    // Simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            setWithSource(key, value, ConfigSource::File);
        } else {
            // Log warning for malformed line
        }
    }
    
    return true;
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        auto jsonData = nlohmann::json::parse(jsonString);
        return loadFromJsonInternal(jsonData, "");
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setWithSource(key, value, ConfigSource::CommandLine)) {
                    success = false;
                }
            } else {
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setWithSource(key, value, ConfigSource::CommandLine)) {
                success = false;
            }
        }
    }
    return success;
}

void Config::loadFromEnvironment(const std::string& prefix) {
    // Note: This would need platform-specific code
    // For now, it's a placeholder for future implementation
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pImpl->useJsonFormat) {
        nlohmann::json jsonData = saveToJson();
        file << jsonData.dump(2);
    } else {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "CONFIG_VALUE_" << entry.key << "\n";
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return convertToType<T>(it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
std::optional<T> Config::getTyped(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return convertToType<T>(it->value);
}

template<typename T>
std::vector<T> Config::getVector(const std::string& key) const {
    std::vector<T> result;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::visit([&](auto&& v) {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, std::vector<T>>) {
                result = v;
            } else if constexpr (std::is_same_v<V, std::vector<double>>) {
                for (const auto& item : v) {
                    result.push_back(static_cast<T>(item));
                }
            } else if constexpr (std::is_same_v<V, std::vector<int64_t>>) {
                for (const auto& item : v) {
                    result.push_back(static_cast<T>(item));
                }
            } else if constexpr (std::is_same_v<V, std::vector<int>>) {
                for (const auto& item : v) {
                    result.push_back(static_cast<T>(item));
                }
            }
        }, it->value);
    }
    
    return result;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto val = getTyped<std::string>(key);
    return val;
}

std::optional<int64_t> Config::getInt(const std::string& key) const {
    auto val = getTyped<int64_t>(key);
    return val;
}

std::optional<double> Config::getFloat(const std::string& key) const {
    auto val = getTyped<double>(key);
    return val;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto val = getTyped<bool>(key);
    return val;
}

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    return setWithSource(key, value, source);
}

bool Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    return setWithSource(key, value, source);
}

bool Config::setInt(const std::string& key, int64_t value, ConfigSource source) {
    return setWithSource(key, value, source);
}

bool Config::setFloat(const std::string& key, double value, ConfigSource source) {
    return setWithSource(key, value, source);
}

bool Config::setBool(const std::string& key, bool value, ConfigSource source) {
    return setWithSource(key, value, source);
}

bool Config::setVector(const std::string& key, const std::vector<double>& values, ConfigSource source) {
    return setWithSource(key, values, source);
}

template<typename T>
bool Config::setWithSource(const std::string& key, const T& value, ConfigSource source) {
    // Check if key is protected
    if (std::find(pImpl->protectedKeys.begin(), pImpl->protectedKeys.end(), key) != pImpl->protectedKeys.end()) {
        return false;
    }
    
    // Create entry
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = "Dynamic configuration entry";
    
    // Check if entry already exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.push_back(std::move(entry));
    }
    
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return validateValue(key, it->value);
}

std::vector<std::string> Config::getValidationErrors(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return {"Key not found"};
    }
    
    return getValueValidationErrors(key, it->value);
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Also remove from protected keys
    pImpl->protectedKeys.erase(
        std::remove(pImpl->protectedKeys.begin(), pImpl->protectedKeys.end(), key),
        pImpl->protectedKeys.end()
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

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->protectedKeys.clear();
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
                                 std::is_same_v<T, std::vector<int64_t>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<bool>> ||
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

std::string Config::summary(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration from " << static_cast<int>(source) << " (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<int64_t>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<bool>> ||
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
            oss << "]\n";
        }
    }
    return oss.str();
}

void Config::addProfile(const ConfigProfile& profile) {
    pImpl->profiles.push_back(profile);
}

bool Config::removeProfile(const std::string& name) {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });
    
    if (it == pImpl->profiles.end()) {
        return false;
    }
    
    pImpl->profiles.erase(it);
    return true;
}

bool Config::activateProfile(const std::string& name) {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });n    
    if (it == pImpl->profiles.end()) {
        return false;
    }
    
    // Deactivate all other profiles
    for (auto& profile : pImpl->profiles) {
        profile.active = false;
    }
    
    it->active = true;
    return true;
}

bool Config::deactivateProfile(const std::string& name) {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });n    
    if (it == pImpl->profiles.end()) {
        return false;
    }
    
    it->active = false;
    return true;
}

bool Config::hasProfile(const std::string& name) const {
    return std::any_of(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });
}

std::vector<std::string> Config::getProfileNames() const {
    std::vector<std::string> names;
    for (const auto& profile : pImpl->profiles) {
        names.push_back(profile.name);
    }
    return names;
}

std::vector<std::string> Config::getActiveProfileNames() const {
    std::vector<std::string> names;
    for (const auto& profile : pImpl->profiles) {
        if (profile.active) {
            names.push_back(profile.name);
        }
    }
    return names;
}

bool Config::applyProfile(const std::string& name) {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });n    
    if (it == pImpl->profiles.end()) {
        return false;
    }
    
    // Resolve inheritance
    std::vector<ConfigProfile> resolvedProfiles;
    resolveProfileInheritance(*it, resolvedProfiles);
    
    // Apply settings from all resolved profiles
    for (const auto& profile : resolvedProfiles) {
        for (const auto& setting : profile.settings) {
            setWithSource(setting.first, setting.second, ConfigSource::Profile);
        }
    }
    
    return true;
}

ConfigProfile Config::getProfile(const std::string& name) const {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
        [&name](const ConfigProfile& p) { return p.name == name; });n    
    if (it == pImpl->profiles.end()) {
        return ConfigProfile{}; // Return empty profile
    }
    
    return *it;
}

bool Config::setBatch(const std::vector<std::pair<std::string, ConfigValue>>& settings) {
    bool success = true;
    for (const auto& setting : settings) {
        if (!set(setting.first, setting.second, ConfigSource::Runtime)) {
            success = false;
        }
    }
    return success;
}

bool Config::applyBatch(const std::vector<std::pair<std::string, ConfigValue>>& settings, ConfigSource source) {
    bool success = true;
    for (const auto& setting : settings) {
        if (!setWithSource(setting.first, setting.second, source)) {
            success = false;
        }
    }
    return success;
}

void Config::protect(const std::string& key) {
    pImpl->protectedKeys.push_back(key);
}

void Config::unprotect(const std::string& key) {
    pImpl->protectedKeys.erase(
        std::remove(pImpl->protectedKeys.begin(), pImpl->protectedKeys.end(), key),
        pImpl->protectedKeys.end()
    );
}

bool Config::isProtected(const std::string& key) const {
    return std::find(pImpl->protectedKeys.begin(), pImpl->protectedKeys.end(), key) != pImpl->protectedKeys.end();
}

bool Config::validate() const {
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            allValid = false;
        }
    }
    return allValid;
}

std::vector<std::string> Config::getAllValidationErrors() const {
    std::vector<std::string> allErrors;
    for (const auto& entry : pImpl->entries) {
        auto errors = getValueValidationErrors(entry.key, entry.value);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
    }
    return allErrors;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            return false;
        }
        
        if (entry.value != it->value) {
            return false;
        }
        
        if (entry.source != it->source) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        setWithSource(entry.key, entry.value, source);
    }
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        if (validateValue(entry.key, entry.value)) {
            stats.validEntries++;
        } else {
            stats.invalidEntries++;
        }
        
        stats.entriesBySource[entry.source]++;
        
        if (isProtected(entry.key)) {
            stats.protectedEntries++;
        }
    }
    
    return stats;
}

std::vector<ConfigEntry> Config::getEntries(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}
bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Note: This would need access to schema for validation
    // For now, just return true
    return true;
}

std::vector<std::string> Config::getValueValidationErrors(const std::string& key, const ConfigValue& value) const {
    return {};
}

bool Config::loadFromJsonInternal(const nlohmann::json& json, const std::string& schemaName) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(*it);
        setWithSource(key, value, ConfigSource::File);
    }
    return true;
}

nlohmann::json Config::saveToJson() const {
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = ConfigValueToJson(entry.value);
    }
    return jsonData;
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

template<typename T>
std::optional<T> Config::convertToType(const ConfigValue& value) {
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, value);
}

bool Config::matchesValidationRule(const ConfigValue& value, const ValidationRuleConfig& rule) {
    switch (rule.rule) {
        case ValidationRule::Positive:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_arithmetic_v<V>) {
                    return v > 0;
                }
                return false;
            }, value);
        
        case ValidationRule::NonNegative:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_arithmetic_v<V>) {
                    return v >= 0;
                }
                return false;
            }, value);
        
        case ValidationRule::Range:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_arithmetic_v<V> && rule.min && rule.max) {
                    return static_cast<double>(v) >= *rule.min && static_cast<double>(v) <= *rule.max;
                }
                return false;
            }, value);
        
        case ValidationRule::Length:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<V, std::string>) {
                    size_t len = v.length();
                    if (rule.minLength && len < *rule.minLength) return false;
                    if (rule.maxLength && len > *rule.maxLength) return false;
                    return true;
                } else if constexpr (std::is_same_v<V, std::vector<typename V::value_type>>) {
                    size_t len = v.size();
                    if (rule.minLength && len < *rule.minLength) return false;
                    if (rule.maxLength && len > *rule.maxLength) return false;
                    return true;
                }
                return false;
            }, value);
        
        case ValidationRule::OneOf:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<V, std::string>) {
                    if (!rule.allowedValues) return false;
                    return std::find(rule.allowedValues->begin(), rule.allowedValues->end(), v) != rule.allowedValues->end();
                }
                return false;
            }, value);
        
        case ValidationRule::Regex:
            return std::visit([rule](auto&& v) -> bool {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<V, std::string> && rule.pattern) {
                    // Note: This would require regex library
                    // For now, implement a simple check
                    return v.find(*rule.pattern) != std::string::npos;
                }
                return false;
            }, value);
        
        case ValidationRule::Custom:
            return rule.customValidator ? rule.customValidator(value) : true;
        
        case ValidationRule::Positive:
        default:
            return true;
    }
}

void Config::resolveProfileInheritance(ConfigProfile& profile, std::vector<ConfigProfile>& allProfiles) const {
    // Check for circular references
    std::vector<std::string> visited;
    resolveProfileInheritanceHelper(profile, allProfiles, visited);
}

nlohmann::json Config::ConfigValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& v) -> nlohmann::json {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<int>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<V, std::vector<int64_t>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<V, std::vector<uint64_t>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<V, std::vector<double>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<V, std::vector<bool>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<V, std::vector<std::string>>) {
            return nlohmann::json(v);
        } else {
            return nlohmann::json(v);
        }
    }, value);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    // Handle different JSON types
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_array()) {
        // Determine array element type
        if (!json.empty()) {
            auto firstElement = json[0];
            if (firstElement.is_string()) {
                std::vector<std::string> result;
                for (const auto& item : json) {
                    result.push_back(item.get<std::string>());
                }
                return result;
            } else if (firstElement.is_number_integer()) {
                std::vector<int64_t> result;
                for (const auto& item : json) {
                    result.push_back(item.get<int64_t>());
                }
                return result;
            } else if (firstElement.is_number_float()) {
                std::vector<double> result;
                for (const auto& item : json) {
                    result.push_back(item.get<double>());
                }
                return result;
            } else if (firstElement.is_boolean()) {
                std::vector<bool> result;
                for (const auto& item : json) {
                    result.push_back(item.get<bool>());
                }
                return result;
            }
        }
        // Default to empty vector
        return std::vector<double>();
    }
    
    // Default for unrecognized types
    return std::string();
}

// Explicit template instantiations
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

// Validation helper function implementations
template<typename T>
bool isInRange(const T& value, double min, double max) {
    return static_cast<double>(value) >= min && static_cast<double>(value) <= max;
}

template<typename T>
bool isPositive(const T& value) {
    return static_cast<double>(value) > 0;
}

template<typename T>
bool isNonNegative(const T& value) {
    return static_cast<double>(value) >= 0;
}

// OneOf validation
template<typename T>
bool isOneOf(const T& value, const std::vector<T>& allowed) {
    return std::find(allowed.begin(), allowed.end(), value) != allowed.end();
}

} // namespace nlm
