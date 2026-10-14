#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <random>
#include <cctype>

namespace nlm {

// Global registry for profiles
static std::map<std::string, ConfigProfile> g_profiles;

// Initialize built-in profiles
static void initializeBuiltInProfiles() {
    if (g_profiles.empty()) {
        ConfigProfile scientificProfile;
        scientificProfile.name = "scientific";
        scientificProfile.description = "Profile optimized for scientific research with high precision";
        scientificProfile.inheritsFrom = {"default"};
        scientificProfile.settings = {
            {"brain.neuron_count", 5000},
            {"brain.synapse_density", 0.15},
            {"brain.connection_probability", 0.05},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.001},
            {"plasticity.hebbian.enable", true},
            {"neuromod.dopamine.scale", 1.0},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true}
        };
        scientificProfile.isExperimental = false;
        scientificProfile.version = "1.0";
        g_profiles["scientific"] = scientificProfile;

        ConfigProfile performanceProfile;
        performanceProfile.name = "performance";
        performanceProfile.description = "Profile optimized for maximum performance with reduced accuracy";
        performanceProfile.inheritsFrom = {"default"};
        performanceProfile.settings = {
            {"brain.neuron_count", 2000},
            {"brain.synapse_density", 0.08},
            {"brain.connection_probability", 0.02},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.0005},
            {"plasticity.hebbian.enable", false},
            {"neuromod.dopamine.scale", 0.5},
            {"neuromod.curiosity.enable", false},
            {"neuromod.novelty.enable", false}
        };
        performanceProfile.isExperimental = false;
        performanceProfile.version = "1.0";
        g_profiles["performance"] = performanceProfile;

        ConfigProfile debugProfile;
        debugProfile.name = "debug";
        debugProfile.description = "Profile for debugging and development with extensive logging";
        debugProfile.inheritsFrom = {"default"};
        debugProfile.settings = {
            {"brain.neuron_count", 100},
            {"brain.synapse_density", 0.2},
            {"brain.connection_probability", 0.1},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.01},
            {"plasticity.hebbian.enable", true},
            {"neuromod.dopamine.scale", 2.0},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"logging.level", "debug"},
            {"checkpoint.enabled", false}
        };
        debugProfile.isExperimental = true;
        debugProfile.version = "1.0";
        g_profiles["debug"] = debugProfile;

        ConfigProfile defaultProfile;
        defaultProfile.name = "default";
        defaultProfile.description = "Default configuration profile";
        defaultProfile.inheritsFrom = {};
        defaultProfile.settings = {
            {"brain.neuron_count", 1000},
            {"brain.synapse_density", 0.1},
            {"brain.connection_probability", 0.05},
            {"plasticity.stdp.enable", true},
            {"plasticity.stdp.learning_rate", 0.001},
            {"plasticity.hebbian.enable", true},
            {"neuromod.dopamine.scale", 1.0},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true}
        };
        defaultProfile.isExperimental = false;
        defaultProfile.version = "1.0";
        g_profiles["default"] = defaultProfile;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::function<ConfigValue(ConfigValue)>> macros;
    std::vector<ConfigChangeCallback> changeCallbacks;
    ConfigStats stats;
    bool debugMode;
    std::string lastFormat;
    
    Impl() : debugMode(false), lastFormat("json") {
        initializeBuiltInProfiles();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (format == "auto" || format == "json") {
        if (parseJSON(content)) {
            pImpl->lastFormat = "json";
            return true;
        }
    }

    if (format == "auto" || format == "yaml") {
        if (parseYAML(content)) {
            pImpl->lastFormat = "yaml";
            return true;
        }
    }

    if (format == "auto" || format == "ini") {
        if (parseINI(content)) {
            pImpl->lastFormat = "ini";
            return true;
        }
    }

    if (format == "auto" || format == "toml") {
        if (parseTOML(content)) {
            pImpl->lastFormat = "toml";
            return true;
        }
    }

    // Try simple key=value format as fallback
    std::istringstream stream(content);
    parseKeyValueStream(stream);

    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseJSON(content);
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseYAML(content);
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseINI(content);
}

bool Config::loadFromTOML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseTOML(content);
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(std::vector<std::string>(argv, argv + argc));
}

bool Config::loadFromArgs(const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];

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
        else if (arg[0] == '-' && i + 1 < args.size()) {
            std::string key = arg.substr(1);
            std::string value = args[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    for (const auto& envPair : std::filesystem::environment()) {
        std::string key = envPair.first;
        std::string value = envPair.second;
        
        if (key.size() > prefix.size() && key.compare(0, prefix.size(), prefix) == 0) {
            std::string configKey = key.substr(prefix.size());
            set(configKey, value, ConfigSource::Environment);
            loaded = true;
        }
    }
    return loaded;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    if (format == "json" || format.empty()) {
        return saveToJSON(filepath);
    } else if (format == "yaml") {
        return saveToYAML(filepath);
    } else if (format == "ini") {
        return saveToINI(filepath);
    } else if (format == "toml") {
        return saveToTOML(filepath);
    }

    return false;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << serializeToJSON();
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << serializeToYAML();
    return true;
}

bool Config::saveToINI(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << serializeToINI();
    return true;
}

bool Config::saveToTOML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << serializeToTOML();
    return true;
}

bool Config::loadProfile(const std::string& profileName, bool merge) {
    auto it = g_profiles.find(profileName);
    if (it == g_profiles.end()) {
        return false;
    }

    const ConfigProfile& profile = it->second;

    // Load parent profiles if any
    for (const auto& parentName : profile.inheritsFrom) {
        auto parentIt = g_profiles.find(parentName);
        if (parentIt != g_profiles.end()) {
            const ConfigProfile& parent = parentIt->second;
            for (const auto& setting : parent.settings) {
                set(setting.first, setting.second, ConfigSource::Profile);
            }
        }
    }

    // Load profile settings
    for (const auto& setting : profile.settings) {
        if (merge && has(setting.first)) {
            // Keep existing value if merging
            continue;
        }
        set(setting.first, setting.second, ConfigSource::Profile);
    }

    return true;
}

bool Config::saveProfile(const std::string& profileName) const {
    ConfigProfile profile;
    profile.name = profileName;
    profile.description = "Saved profile";
    profile.inheritsFrom = {};
    profile.settings.clear();
    profile.isExperimental = false;
    profile.version = "1.0";

    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Profile || 
            entry.source == ConfigSource::File ||
            entry.source == ConfigSource::CommandLine) {
            profile.settings[entry.key] = entry.value;
        }
    }

    g_profiles[profileName] = profile;
    return true;
}

bool Config::deleteProfile(const std::string& profileName) {
    auto it = g_profiles.find(profileName);
    if (it == g_profiles.end()) {
        return false;
    }

    g_profiles.erase(it);
    return true;
}

std::vector<std::string> Config::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& pair : g_profiles) {
        profiles.push_back(pair.first);
    }
    return profiles;
}

void Config::registerProfile(const ConfigProfile& profile) {
    g_profiles[profile.name] = profile;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    ConfigValue oldValue;
    bool hasOldValue = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        hasOldValue = true;
        
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Create new entry
        ConfigEntry newEntry;
        newEntry.key = key;
        newEntry.value = value;
        newEntry.source = source;
        newEntry.description = "";
        newEntry.category = "general";
        newEntry.dataType = "auto";
        newEntry.validationRules = "";
        newEntry.required = false;
        newEntry.minValue = std::numeric_limits<double>::lowest();
        newEntry.maxValue = std::numeric_limits<double>::max();
        newEntry.allowedValues = {};
        
        pImpl->entries.push_back(newEntry);
        it = std::prev(pImpl->entries.end());
    }
    
    // Notify callbacks if value changed
    if (hasOldValue && oldValue != value) {
        notifyChange(key, oldValue, value);
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

template<typename T>
bool Config::setWithValidation(const std::string& key, const T& value,
                             const std::string& description,
                             const std::string& category,
                             const std::string& dataType,
                             const std::string& validationRules,
                             bool required,
                             double minValue,
                             double maxValue) {
    ValidationResult validation = validateValue(key, ConfigValue(value));
    if (!validation.valid) {
        return false;
    }
    
    set(key, value, ConfigSource::Runtime);
    
    // Update metadata for the entry
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.description = description;
            entry.category = category;
            entry.dataType = dataType;
            entry.validationRules = validationRules;
            entry.required = required;
            entry.minValue = minValue;
            entry.maxValue = maxValue;
            break;
        }
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

template<typename T>
T Config::getOrWithDefault(const std::string& key, const T& defaultValue, ConfigSource minSource) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return defaultValue;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    int sourcePriority = static_cast<int>(it->source);
    int minPriority = static_cast<int>(minSource);
    
    if (sourcePriority >= minPriority) {
        return val.value();
    } else {
        return defaultValue;
    }
}

int Config::getInt(const std::string& key) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : 0;
}

double Config::getDouble(const std::string& key) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : 0.0;
}

bool Config::getBool(const std::string& key) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : false;
}

std::string Config::getString(const std::string& key) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : "";
}

std::vector<double> Config::getVector(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>();
}

std::vector<std::vector<double>> Config::getMatrix(const std::string& key) const {
    auto val = get<std::vector<std::vector<double>>>(key);
    return val.has_value() ? val.value() : std::vector<std::vector<double>>();
}

template<typename T>
std::optional<T> Config::getNested(const std::string& key) const {
    if (!isNestedKey(key)) {
        return get<T>(key);
    }
    
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return get<T>(key);
    }
    
    std::string parentKey = key.substr(0, dotPos);
    std::string childKey = key.substr(dotPos + 1);
    
    auto parentValue = get<std::map<std::string, std::any>>(parentKey);
    if (!parentValue.has_value()) {
        return std::nullopt;
    }
    
    const auto& nestedMap = parentValue.value();
    auto childIt = nestedMap.find(childKey);
    if (childIt == nestedMap.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(childIt->second);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getNestedOr(const std::string& key, const T& defaultValue) const {
    auto val = getNested<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::validate() const {
    ValidationResult result;
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = validateKey(entry.key);
        if (!entryResult.valid) {
            result.valid = false;
            result.errorMessage += "Key \"" + entry.key + "\": " + entryResult.errorMessage + "; ";
        }
        if (!entryResult.warnings.empty()) {
            result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
        }
    }
    return result.valid;
}

bool Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return !hasKeyMetadata(key);  // Missing required key is not an error unless required
    }
    
    return validateValue(key, it->value).valid;
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        result.valid = false;
        result.errorMessage = "Key not found in configuration";
        return result;
    }
    
    const ConfigEntry& entry = *it;
    
    // Check required field
    if (entry.required && !has(key)) {
        result.valid = false;
        result.errorMessage = "Required key \"" + key + "\" is missing";
        return result;
    }
    
    // Check data type
    if (!validateValueType(value, entry.dataType)) {
        result.valid = false;
        result.errorMessage = "Value type mismatch for key \"" + key + "\"";
        return result;
    }
    
    // Check range
    if (!validateValueRange(value, entry.minValue, entry.maxValue)) {
        result.valid = false;
        result.errorMessage = "Value out of range for key \"" + key + "\"";
        return result;
    }
    
    // Check allowed values
    if (!validateValueAllowed(value, entry.allowedValues)) {
        result.valid = false;
        result.errorMessage = "Value not in allowed set for key \"" + key + "\"";
        return result;
    }
    
    // Check validation rules
    if (!entry.validationRules.empty() && !validateWithRules(value, entry.validationRules)) {
        result.valid = false;
        result.errorMessage = "Validation rules failed for key \"" + key + "\"";
        return result;
    }
    
    return result;
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        int sourceIndex = static_cast<int>(entry.source);
        if (sourceIndex >= 0 && sourceIndex < 4) {
            stats.entriesBySource[sourceIndex]++;
        }
    }
    
    stats.lastModified = std::chrono::system_clock::now();
    stats.memoryUsage = calculateMemoryUsage();
    
    return stats;
}

std::string Config::getConfigurationSummary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "] (" << static_cast<int>(entry.source) << ") category: " << entry.category << "\n";
    }
    
    return oss.str();
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::map<std::string, ConfigValue> Config::getCategory(const std::string& category) const {
    std::map<std::string, ConfigValue> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            result[entry.key] = entry.value;
        }
    }
    return result;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
}

void Config::removeByCategory(const std::string& category) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&category](const ConfigEntry& e) { return e.category == category; });
    pImpl->entries.erase(it, pImpl->entries.end());
}

void Config::clear() {
    pImpl->entries.clear();
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if we already have this key
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        } else {
            // Update existing entry
            it->value = entry.value;
            it->source = source;
        }
    }
}

void Config::inheritFrom(const Config& parent, ConfigSource source) {
    for (const auto& entry : parent.pImpl->entries) {
        // Check if we already have this key
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Inherit parent entry
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
        }
    }
}

bool Config::backup(const std::string& filepath) const {
    return saveToFile(filepath + ".backup", "json");
}

bool Config::restore(const std::string& filepath) {
    return loadFromFile(filepath, "json");
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
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

std::vector<std::string> Config::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.dataType == type) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::transform(const std::function<ConfigValue(ConfigValue)>& transformFunc) {
    for (auto& entry : pImpl->entries) {
        try {
            ConfigValue oldValue = entry.value;
            entry.value = transformFunc(entry.value);
            if (oldValue != entry.value) {
                notifyChange(entry.key, oldValue, entry.value);
            }
        } catch (...) {
            // Skip entries that can't be transformed
        }
    }
}

void Config::filter(const std::function<bool(const ConfigEntry&)>& predicate) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(), predicate);
    pImpl->entries.erase(it, pImpl->entries.end());
}

void Config::sort(const std::function<bool(const ConfigEntry&, const ConfigEntry&)>& comparator) {
    std::sort(pImpl->entries.begin(), pImpl->entries.end(), comparator);
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        switch (entry.value.index()) {
            case 0: { // int
                int val = std::get<int>(entry.value);
                oss << val;
                break;
            }
            case 1: { // int64_t
                int64_t val = std::get<int64_t>(entry.value);
                oss << val;
                break;
            }
            case 2: { // double
                double val = std::get<double>(entry.value);
                oss << std::setprecision(6) << val;
                break;
            }
            case 3: { // bool
                bool val = std::get<bool>(entry.value);
                oss << (val ? "true" : "false");
                break;
            }
            case 4: { // std::string
                const std::string& val = std::get<std::string>(entry.value);
                oss << "\"" << escapeString(val) << "\"";
                break;
            }
            case 5: { // std::vector<int>
                const auto& val = std::get<std::vector<int>>(entry.value);
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << val[j];
                }
                oss << "]";
                break;
            }
            case 6: { // std::vector<double>
                const auto& val = std::get<std::vector<double>>(entry.value);
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << std::setprecision(6) << val[j];
                }
                oss << "]";
                break;
            }
            case 7: { // std::vector<std::string>
                const auto& val = std::get<std::vector<std::string>>(entry.value);
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << "\"" << escapeString(val[j]) << "\"";
                }
                oss << "]";
                break;
            }
            case 8: { // std::vector<std::vector<double>>
                const auto& val = std::get<std::vector<std::vector<double>>>(entry.value);
                oss << "[[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << "], [";
                    for (size_t k = 0; k < val[j].size(); ++k) {
                        if (k > 0) oss << ", ";
                        oss << std::setprecision(6) << val[j][k];
                    }
                }
                oss << "]]";
                break;
            }
            case 9: { // std::map<std::string, std::any>
                const auto& val = std::get<std::map<std::string, std::any>>(entry.value);
                oss << "{";
                bool first = true;
                for (const auto& pair : val) {
                    if (!first) oss << ", ";
                    first = false;
                    oss << "\"" << pair.first << "\": ";
                    oss << "any_object";
                }
                oss << "}";
                break;
            }
        }
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::importFromJSON(const std::string& json) {
    clear();
    return parseJSON(json);
}

std::string Config::exportToYAML() const {
    std::ostringstream oss;
    oss << "# Configuration exported by NLM\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.key << " = " << entry.description << "\n";
        oss << entry.key << ": ";
        
        switch (entry.value.index()) {
            case 0: { // int
                int val = std::get<int>(entry.value);
                oss << val;
                break;
            }
            case 1: { // int64_t
                int64_t val = std::get<int64_t>(entry.value);
                oss << val;
                break;
            }
            case 2: { // double
                double val = std::get<double>(entry.value);
                oss << std::setprecision(6) << val;
                break;
            }
            case 3: { // bool
                bool val = std::get<bool>(entry.value);
                oss << (val ? "true" : "false");
                break;
            }
            case 4: { // std::string
                const std::string& val = std::get<std::string>(entry.value);
                oss << "\"" << val << "\"";
                break;
            }
            default:
                oss << "# Unsupported type";
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

std::string Config::exportToINI() const {
    std::ostringstream oss;
    oss << "# Configuration exported by NLM\n";
    
    std::string currentSection = "";
    for (const auto& entry : pImpl->entries) {
        if (!currentSection.empty() && entry.category != currentSection) {
            oss << "\n";
        }
        
        if (entry.category != currentSection) {
            currentSection = entry.category;
            oss << "[" << currentSection << "]\n";
        }
        
        oss << entry.key << " = ";
        
        switch (entry.value.index()) {
            case 0: { // int
                int val = std::get<int>(entry.value);
                oss << val;
                break;
            }
            case 1: { // int64_t
                int64_t val = std::get<int64_t>(entry.value);
                oss << val;
                break;
            }
            case 2: { // double
                double val = std::get<double>(entry.value);
                oss << std::setprecision(6) << val;
                break;
            }
            case 3: { // bool
                bool val = std::get<bool>(entry.value);
                oss << (val ? "true" : "false");
                break;
            }
            case 4: { // std::string
                const std::string& val = std::get<std::string>(entry.value);
                oss << "\"" << val << "\"";
                break;
            }
            default:
                oss << "# Unsupported type";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::exportToTOML() const {
    std::ostringstream oss;
    oss << "# Configuration exported by NLM\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        
        switch (entry.value.index()) {
            case 0: { // int
                int val = std::get<int>(entry.value);
                oss << entry.key << " = " << val << "\n\n";
                break;
            }
            case 1: { // int64_t
                int64_t val = std::get<int64_t>(entry.value);
                oss << entry.key << " = " << val << "\n\n";
                break;
            }
            case 2: { // double
                double val = std::get<double>(entry.value);
                oss << entry.key << " = " << std::setprecision(6) << val << "\n\n";
                break;
            }
            case 3: { // bool
                bool val = std::get<bool>(entry.value);
                oss << entry.key << " = " << (val ? "true" : "false") << "\n\n";
                break;
            }
            case 4: { // std::string
                const std::string& val = std::get<std::string>(entry.value);
                oss << entry.key << " = \"" << val << "\"\n\n";
                break;
            }
            default:
                oss << "# Unsupported type\n\n";
        }
    }
    
    return oss.str();
}

void Config::addDefaultProfile(const std::string& name) {
    ConfigProfile profile;
    profile.name = name;
    profile.description = "Default profile for " + name;
    profile.inheritsFrom = {};
    profile.settings = {
        {"brain.neuron_count", 1000},
        {"brain.synapse_density", 0.1},
        {"brain.connection_probability", 0.05},
        {"plasticity.stdp.enable", true},
        {"plasticity.stdp.learning_rate", 0.001},
        {"plasticity.hebbian.enable", true},
        {"neuromod.dopamine.scale", 1.0},
        {"neuromod.curiosity.enable", true},
        {"neuromod.novelty.enable", true}
    };
    profile.isExperimental = false;
    profile.version = "1.0";
    g_profiles[name] = profile;
}

void Config::addScientificProfile(const std::string& name) {
    ConfigProfile profile;
    profile.name = name;
    profile.description = "Scientific research profile with high precision";
    profile.inheritsFrom = {"default"};
    profile.settings = {
        {"brain.neuron_count", 5000},
        {"brain.synapse_density", 0.15},
        {"brain.connection_probability", 0.05},
        {"plasticity.stdp.enable", true},
        {"plasticity.stdp.learning_rate", 0.001},
        {"plasticity.hebbian.enable", true},
        {"neuromod.dopamine.scale", 1.0},
        {"neuromod.curiosity.enable", true},
        {"neuromod.novelty.enable", true},
        {"simulation.timestep", 0.0001}
    };
    profile.isExperimental = false;
    profile.version = "1.0";
    g_profiles[name] = profile;
}

void Config::addPerformanceProfile(const std::string& name) {
    ConfigProfile profile;
    profile.name = name;
    profile.description = "Performance profile optimized for speed";
    profile.inheritsFrom = {"default"};
    profile.settings = {
        {"brain.neuron_count", 500},
        {"brain.synapse_density", 0.05},
        {"brain.connection_probability", 0.02},
        {"plasticity.stdp.enable", true},
        {"plasticity.stdp.learning_rate", 0.0005},
        {"plasticity.hebbian.enable", false},
        {"neuromod.dopamine.scale", 0.5},
        {"neuromod.curiosity.enable", false},
        {"neuromod.novelty.enable", false},
        {"simulation.timestep", 0.001}
    };
    profile.isExperimental = false;
    profile.version = "1.0";
    g_profiles[name] = profile;
}

void Config::addDebugProfile(const std::string& name) {
    ConfigProfile profile;
    profile.name = name;
    profile.description = "Debug profile for development and testing";
    profile.inheritsFrom = {"default"};
    profile.settings = {
        {"brain.neuron_count", 100},
        {"brain.synapse_density", 0.2},
        {"brain.connection_probability", 0.1},
        {"plasticity.stdp.enable", true},
        {"plasticity.stdp.learning_rate", 0.01},
        {"plasticity.hebbian.enable", true},
        {"neuromod.dopamine.scale", 2.0},
        {"neuromod.curiosity.enable", true},
        {"neuromod.novelty.enable", true},
        {"logging.level", "debug"},
        {"checkpoint.enabled", false}
    };
    profile.isExperimental = true;
    profile.version = "1.0";
    g_profiles[name] = profile;
}

void Config::enableDebugMode(bool enabled) {
    pImpl->debugMode = enabled;
    if (enabled) {
        std::cout << "[DEBUG] Configuration debug mode enabled\n";
    }
}

bool Config::isDebugMode() const {
    return pImpl->debugMode;
}

std::string Config::getDebugInfo() const {
    std::ostringstream oss;
    oss << "=== Configuration Debug Info ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Debug mode: " << (pImpl->debugMode ? "enabled" : "disabled") << "\n";
    oss << "Last format: " << pImpl->lastFormat << "\n";
    oss << "Memory usage: " << calculateMemoryUsage() << " bytes\n";
    
    oss << "\n=== Entries ===\n";
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Category: " << entry.category << "\n";
        oss << "  Type: " << entry.dataType << "\n";
        oss << "  Required: " << (entry.required ? "yes" : "no") << "\n";
    }
    
    return oss.str();
}

void Config::logConfiguration() const {
    if (pImpl->debugMode) {
        std::cout << getDebugInfo();
    }
}

void Config::addValidationRule(const std::string& key, const std::string& rule) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->validationRules = rule;
    }
}

void Config::removeValidationRule(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->validationRules = "";
    }
}

std::vector<std::string> Config::getValidationRules(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return {};
    }
    
    std::vector<std::string> rules;
    std::istringstream stream(it->validationRules);
    std::string rule;
    while (std::getline(stream, rule)) {
        if (!rule.empty()) {
            rules.push_back(rule);
        }
    }
    return rules;
}

template<typename T>
bool Config::increment(const std::string& key, T amount) {
    if (!has(key)) {
        return false;
    }
    
    auto val = get<T>(key);
    if (!val.has_value()) {
        return false;
    }
    
    T newValue = val.value() + amount;
    set(key, newValue, ConfigSource::Runtime);
    return true;
}

template<typename T>
bool Config::decrement(const std::string& key, T amount) {
    return increment(key, -amount);
}

bool Config::isGreater(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    try {
        if (std::holds_alternative<int>(it->value) && std::holds_alternative<int>(value)) {
            return std::get<int>(it->value) > std::get<int>(value);
        } else if (std::holds_alternative<double>(it->value) && std::holds_alternative<double>(value)) {
            return std::get<double>(it->value) > std::get<double>(value);
        }
    } catch (...) {
    }
    return false;
}

bool Config::isLess(const std::string& key, const ConfigValue& value) const {
    return isGreater(value, key);
}

bool Config::isEqual(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->value == value;
}

void Config::addMacro(const std::string& name, const ConfigValue& value) {
    pImpl->macros[name] = value;
}

ConfigValue Config::getMacro(const std::string& name) const {
    auto it = pImpl->macros.find(name);
    return (it != pImpl->macros.end()) ? it->second : ConfigValue();
}

bool Config::hasMacro(const std::string& name) const {
    return pImpl->macros.find(name) != pImpl->macros.end();
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    auto it = std::find(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback);
    if (it != pImpl->changeCallbacks.end()) {
        pImpl->changeCallbacks.erase(it);
    }
}

void Config::notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& callback : pImpl->changeCallbacks) {
        try {
            callback(key, oldValue, newValue);
        } catch (...) {
            // Ignore exceptions in callbacks
        }
    }
}

// Implementation of parsing methods
bool Config::parseJSON(const std::string& content) {
    // Simple JSON parser implementation
    // This would ideally use a proper JSON library, but for now we implement a basic parser
    
    size_t pos = 0;
    if (content.find('{') == std::string::npos) {
        return false;
    }
    
    pos = content.find('{') + 1;
    while (pos < content.length() && content[pos] != '}') {
        // Find key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;

        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 1;

        // Find colon
        while (pos < content.length() && content[pos] != ':') pos++;
        if (pos >= content.length()) break;
        pos++;

        // Skip whitespace
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) pos++;

        // Parse value
        ConfigValue value;
        if (pos < content.length() && content[pos] == '"') {
            // String
            size_t valueStart = pos + 1;
            size_t valueEnd = content.find('"', valueStart);
            if (valueEnd != std::string::npos) {
                value = content.substr(valueStart, valueEnd - valueStart);
                pos = valueEnd + 1;
            }
        } else if (pos < content.length() && content[pos] == 't') {
            // Boolean true
            value = true;
            pos += 4; // "true"
        } else if (pos < content.length() && content[pos] == 'f') {
            // Boolean false
            value = false;
            pos += 5; // "false"
        } else if (content.substr(pos, 4) == "null") {
            // Skip null
            pos += 4;
        } else if (content[pos] == '{' || content[pos] == '[') {
            // For arrays and objects, we'll skip for now
            char closing = content[pos] == '{' ? '}' : ']';
            int depth = 1;
            pos++;
            while (pos < content.length() && depth > 0) {
                if (content[pos] == '{' || content[pos] == '[') depth++;
                else if (content[pos] == '}' || content[pos] == ']') depth--;
                pos++;
            }
        } else {
            // Number (integer or float)
            size_t numEnd = pos;
            while (numEnd < content.length() && (isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == '-' || content[numEnd] == 'e' || content[numEnd] == 'E')) {
                numEnd++;
            }
            std::string numStr = content.substr(pos, numEnd - pos);
            // Try to parse as integer first
            try {
                size_t posInt = 0;
                long long intVal = std::stoll(numStr, &posInt);
                if (posInt == numStr.length()) {
                    value = static_cast<int>(intVal);
                } else {
                    // Try as double
                    double doubleVal = std::stod(numStr);
                    value = doubleVal;
                }
            } catch (...) {
                // If parsing fails, skip
                break;
            }
            pos = numEnd;
        }

        set(key, value, ConfigSource::File);

        // Skip whitespace and comma
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == ',')) {
            if (content[pos] == ',') pos++;
            pos++;
        }
    }

    return true;
}

bool Config::parseYAML(const std::string& content) {
    // Simple YAML parser - for now just parse as key=value pairs
    return parseKeyValue(content);
}

bool Config::parseINI(const std::string& content) {
    return parseKeyValue(content);
}

bool Config::parseTOML(const std::string& content) {
    // Simple TOML parser - for now just parse as key=value pairs
    return parseKeyValue(content);
}

bool Config::parseKeyValue(const std::string& content) {
    std::istringstream stream(content);
    parseKeyValueStream(stream);
    return true;
}

void Config::parseKeyValueStream(std::istringstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '/') {
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

            // Parse value based on type
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    // Try as integer
                    size_t posInt = 0;
                    long long intVal = std::stoll(value, &posInt);
                    if (posInt == value.length()) {
                        configValue = static_cast<int>(intVal);
                    } else {
                        // Try as double
                        double doubleVal = std::stod(value);
                        configValue = doubleVal;
                    }
                } catch (...) {
                    // Try as string
                    configValue = value;
                }
            }

            set(key, configValue, ConfigSource::File);
        }
    }
}

// Implementation of validation methods
bool Config::validateValueType(const ConfigValue& value, const std::string& expectedType) const {
    if (expectedType == "auto") {
        return true; // No type checking
    }

    if (expectedType == "int" && std::holds_alternative<int>(value)) return true;
    if (expectedType == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
    if (expectedType == "double" && std::holds_alternative<double>(value)) return true;
    if (expectedType == "bool" && std::holds_alternative<bool>(value)) return true;
    if (expectedType == "string" && std::holds_alternative<std::string>(value)) return true;
    if (expectedType == "vector" && (
        std::holds_alternative<std::vector<int>>(value) ||
        std::holds_alternative<std::vector<double>>(value) ||
        std::holds_alternative<std::vector<std::string>>(value))) return true;
    if (expectedType == "matrix" && std::holds_alternative<std::vector<std::vector<double>>>(value)) return true;
    if (expectedType == "object" && std::holds_alternative<std::map<std::string, std::any>>(value)) return true;

    return false;
}

bool Config::validateValueRange(const ConfigValue& value, double minValue, double maxValue) const {
    if (minValue == std::numeric_limits<double>::lowest() && maxValue == std::numeric_limits<double>::max()) {
        return true; // No range checking
    }

    try {
        if (std::holds_alternative<int>(value)) {
            double val = static_cast<double>(std::get<int>(value));
            return val >= minValue && val <= maxValue;
        } else if (std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val >= minValue && val <= maxValue;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= minValue && val <= maxValue;
        }
    } catch (...) {
    }

    return false;
}

bool Config::validateValueAllowed(const ConfigValue& value, const std::vector<std::string>& allowedValues) const {
    if (allowedValues.empty()) {
        return true; // No allowed values checking
    }

    try {
        if (std::holds_alternative<std::string>(value)) {
            const std::string& val = std::get<std::string>(value);
            return std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
        }
    } catch (...) {
    }

    return false;
}

bool Config::validateWithRules(const ConfigValue& value, const std::string& rules) const {
    // Simple rule validation - for now just check if rules contain known patterns
    // This could be extended with more sophisticated validation logic
    
    if (rules.empty()) {
        return true;
    }

    // Example rules: "required", "positive", "range(0,100)", "pattern:[0-9]+"
    if (rules.find("required") != std::string::npos) {
        // Check if this is a mandatory key
        // For now, just return true as we can't easily check if a key is required
        return true;
    }

    if (rules.find("positive") != std::string::npos) {
        try {
            if (std::holds_alternative<int>(value) && std::get<int>(value) >= 0) return true;
            if (std::holds_alternative<double>(value) && std::get<double>(value) >= 0) return true;
        } catch (...) {
        }
        return false;
    }

    if (rules.find("range(") != std::string::npos) {
        // Parse range rule like "range(0,100)"
        size_t start = rules.find("range(") + 6;
        size_t end = rules.find(")", start);
        if (start != std::string::npos && end != std::string::npos) {
            std::string rangeStr = rules.substr(start, end - start);
            size_t commaPos = rangeStr.find(',');
            if (commaPos != std::string::npos) {
                try {
                    double minVal = std::stod(rangeStr.substr(0, commaPos));
                    double maxVal = std::stod(rangeStr.substr(commaPos + 1));
                    return validateValueRange(value, minVal, maxVal);
                } catch (...) {
                }
            }
        }
    }

    return true; // If we can't parse the rule, assume it's valid
}

// Implementation of helper methods
std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
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

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes";
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

int64_t Config::stringToInt64(const std::string& str) {
    return std::stoll(str);
}

bool Config::isNestedKey(const std::string& key) const {
    return key.find('.') != std::string::npos;
}

std::string Config::resolveKey(const std::string& key) const {
    if (!isNestedKey(key)) {
        return key;
    }
    // For now, just return the key as-is
    // Could implement more sophisticated key resolution
    return key;
}

bool Config::hasKeyMetadata(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::updateStats() const {
    // Update statistics
    // This would be called when the configuration is modified
}

size_t Config::calculateMemoryUsage() const {
    size_t totalSize = sizeof(Config);
    
    // Estimate size of entries
    for (const auto& entry : pImpl->entries) {
        totalSize += sizeof(ConfigEntry) + entry.key.size();
        
        // Estimate size of value
        totalSize += estimateValueSize(entry.value);
    }
    
    // Estimate size of macros
    for (const auto& macro : pImpl->macros) {
        totalSize += sizeof(std::string) + macro.first.size();
        totalSize += estimateValueSize(macro.second);
    }
    
    return totalSize;
}

size_t Config::estimateValueSize(const ConfigValue& value) const {
    size_t size = 0;
    
    if (std::holds_alternative<int>(value)) {
        size = sizeof(int);
    } else if (std::holds_alternative<int64_t>(value)) {
        size = sizeof(int64_t);
    } else if (std::holds_alternative<double>(value)) {
        size = sizeof(double);
    } else if (std::holds_alternative<bool>(value)) {
        size = sizeof(bool);
    } else if (std::holds_alternative<std::string>(value)) {
        size = sizeof(std::string) + std::get<std::string>(value).size();
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        size = sizeof(std::vector<int>) + vec.size() * sizeof(int);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        size = sizeof(std::vector<double>) + vec.size() * sizeof(double);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        size = sizeof(std::vector<std::string>);
        for (const auto& str : vec) {
            size += sizeof(std::string) + str.size();
        }
    } else if (std::holds_alternative<std::vector<std::vector<double>>>(value)) {
        const auto& matrix = std::get<std::vector<std::vector<double>>>(value);
        size = sizeof(std::vector<std::vector<double>>);
        for (const auto& row : matrix) {
            size += sizeof(std::vector<double>) + row.size() * sizeof(double);
        }
    } else if (std::holds_alternative<std::map<std::string, std::any>>(value)) {
        const auto& obj = std::get<std::map<std::string, std::any>>(value);
        size = sizeof(std::map<std::string, std::any>);
        for (const auto& pair : obj) {
            size += sizeof(std::string) + pair.first.size() + sizeof(std::any);
        }
    }
    
    return size;
}

// Explicit template instantiations
namespace {
    // Template instantiations for setWithValidation
    template bool Config::setWithValidation<int>(const std::string&, const int&, const std::string&, const std::string&, const std::string&, const std::string&, bool, double, double);
    template bool Config::setWithValidation<double>(const std::string&, const double&, const std::string&, const std::string&, const std::string&, const std::string&, bool, double, double);
    template bool Config::setWithValidation<std::string>(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, bool, double, double);
    template bool Config::setWithValidation<bool>(const std::string&, const bool&, const std::string&, const std::string&, const std::string&, const std::string&, bool, double, double);
    
    // Template instantiations for getNestedOr
    template T Config::getNestedOr<int>(const std::string&, const int&) const;
    template T Config::getNestedOr<double>(const std::string&, const double&) const;
    template T Config::getNestedOr<std::string>(const std::string&, const std::string&) const;
    template T Config::getNestedOr<bool>(const std::string&, const bool&) const;
    
    // Template instantiations for increment/decrement
    template bool Config::increment<int>(const std::string&, int);
    template bool Config::decrement<int>(const std::string&, int);
    
    // Template instantiations for get
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
    template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
    template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
    template std::optional<std::vector<std::vector<double>>> Config::get<std::vector<std::vector<double>>>(const std::string&) const;
    template std::optional<std::map<std::string, std::any>> Config::get<std::map<std::string, std::any>>(const std::string&) const;
    
    // Template instantiations for getOr
    template int Config::getOr<int>(const std::string&, const int&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    
    // Template instantiations for getOrWithDefault
    template int Config::getOrWithDefault<int>(const std::string&, const int&, ConfigSource) const;
    template double Config::getOrWithDefault<double>(const std::string&, const double&, ConfigSource) const;
    template std::string Config::getOrWithDefault<std::string>(const std::string&, const std::string&, ConfigSource) const;
    
    // Template instantiations for increment/decrement
    template bool Config::increment<double>(const std::string&, double);
    template bool Config::decrement<double>(const std::string&, double);
}

} // namespace nlm