#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, Config> inheritanceSources;
    std::map<std::string, ConfigPreset> presets;
    std::vector<ValidationRule> validationRules;
    std::map<std::string, std::vector<ConfigEntry>> snapshots;
    std::unordered_map<std::string, double> performanceMetrics;
    bool monitoringEnabled = false;
    bool cachingEnabled = false;
    std::unordered_map<std::string, std::string> cache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Add default presets
    pImpl->presets = {
        {createDevelopmentPreset()},
        {createProductionPreset()},
        {createTestingPreset()},
        {createBenchmarkPreset()}
    };
    
    // Add default validation rules
    pImpl->validationRules = {
        createRangeValidator("simulation_timestep", 0.0001, 0.1, "Timestep must be between 0.0001s and 0.1s"),
        createRangeValidator("neuron_count", 1, 1000000, "Neuron count must be between 1 and 1,000,000"),
        createEnumValidator("learning_strategy", {"conservative", "balanced", "aggressive"}, "Invalid learning strategy"),
        createRangeValidator("plasticity_rate", 0.0, 2.0, "Plasticity rate must be between 0.0 and 2.0"),
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then YAML, then TOML, then simple format
    if (filepath.substr(filepath.find_last_of(".") + 1) == "json") {
        return parseJSON(filepath);
    } else if (filepath.substr(filepath.find_last_of(".") + 1) == "yaml" || 
               filepath.substr(filepath.find_last_of(".") + 1) == "yml") {
        return parseYAML(filepath);
    } else if (filepath.substr(filepath.find_last_of(".") + 1) == "toml") {
        return parseTOML(filepath);
    } else {
        return loadFromArgs(0, nullptr); // Placeholder for simple format
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        clear();
        
        for (const auto& [key, value] : json.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Determine array type
                if (!value.empty()) {
                    if (value.front().is_number_integer()) {
                        std::vector<int> arr;
                        for (const auto& item : value) {
                            arr.push_back(item.get<int>());
                        }
                        configValue = arr;
                    } else if (value.front().is_number_float()) {
                        std::vector<double> arr;
                        for (const auto& item : value) {
                            arr.push_back(item.get<double>());
                        }
                        configValue = arr;
                    } else if (value.front().is_string()) {
                        std::vector<std::string> arr;
                        for (const auto& item : value) {
                            arr.push_back(item.get<std::string>());
                        }
                        configValue = arr;
                    } else if (value.front().is_boolean()) {
                        std::vector<bool> arr;
                        for (const auto& item : value) {
                            arr.push_back(item.get<bool>());
                        }
                        configValue = arr;
                    } else {
                        configValue = std::vector<double>(); // Default to double
                    }
                } else {
                    configValue = std::vector<double>();
                }
            } else if (value.is_object()) {
                std::map<std::string, ConfigValue> obj;
                for (const auto& [subKey, subValue] : value.items()) {
                    ConfigValue subConfigValue;
                    if (subValue.is_string()) {
                        subConfigValue = subValue.get<std::string>();
                    } else if (subValue.is_number_integer()) {
                        subConfigValue = subValue.get<int64_t>();
                    } else if (subValue.is_number_float()) {
                        subConfigValue = subValue.get<double>();
                    } else if (subValue.is_boolean()) {
                        subConfigValue = subValue.get<bool>();
                    }
                    obj[subKey] = subConfigValue;
                }
                configValue = obj;
            } else {
                configValue = value.get<std::string>(); // Fallback
            }
            
            set(key, configValue, ConfigSource::File, "Loaded from JSON");
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        clear();
        
        for (const auto& it : yaml) {
            std::string key = it.first.as<std::string>();
            ConfigValue configValue;
            
            if (it.second.IsScalar()) {
                std::string value = it.second.as<std::string>();
                // Try to convert to appropriate type
                if (value == "true" || value == "false") {
                    configValue = value == "true";
                } else {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            configValue = doubleVal;
                        } else {
                            configValue = value;
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
            } else if (it.second.IsSequence()) {
                std::vector<double> arr;
                for (const auto& item : it.second) {
                    arr.push_back(item.as<double>());
                }
                configValue = arr;
            } else if (it.second.IsMap()) {
                std::map<std::string, ConfigValue> obj;
                for (const auto& subIt : it.second) {
                    std::string subKey = subIt.first.as<std::string>();
                    ConfigValue subConfigValue;
                    
                    if (subIt.second.IsScalar()) {
                        std::string subValue = subIt.second.as<std::string>();
                        subConfigValue = subValue;
                    }
                    
                    obj[subKey] = subConfigValue;
                }
                configValue = obj;
            }
            
            set(key, configValue, ConfigSource::File, "Loaded from YAML");
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromTOML(const std::string& tomlString) {
    // For now, use simple format for TOML
    return loadFromArgs(0, nullptr);
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    
    for (const auto& env : std::filesystem::current_path()) {
        (void)env; // Avoid unused variable warning
    }
    
    // This would normally read from environment variables
    // For now, just return false
    return loaded;
}

bool Config::loadFromArgs(int argc, char** argv, const std::string& prefix) {
    // Enhanced command line argument parsing
    bool loaded = false;
    
    if (argc <= 1) {
        return loaded;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --prefix.key=value format
        if (arg.find('=') != std::string::npos && arg.find('.') != std::string::npos) {
            // Advanced: --prefix.key=value format
            size_t firstDot = arg.find('.');
            size_t eqPos = arg.find('=');
            
            if (firstDot < eqPos) {
                std::string prefixPart = arg.substr(0, firstDot);
                std::string key = arg.substr(firstDot + 1, eqPos - firstDot - 1);
                std::string value = arg.substr(eqPos + 1);
                
                if (!prefix.empty() && prefixPart != prefix) {
                    continue;
                }
                
                // Apply prefix
                if (!prefix.empty()) {
                    key = prefix + key;
                }
                
                set(key, value, ConfigSource::CommandLine, "Command line argument");
                loaded = true;
            }
        }
        // Handle --key=value format
        else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            std::string key = arg.substr(2);
            size_t pos = key.find('=');
            std::string value = key.substr(pos + 1);
            key = key.substr(0, pos);
            
            if (!prefix.empty() && key.find(prefix) != 0) {
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine, "Command line argument");
            loaded = true;
        }
        // Handle -k value format
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine, "Command line argument");
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::loadFromDatabase(const std::string& connectionString, const std::string& query) {
    // Placeholder for database loading
    return false;
}

bool Config::loadFromNetwork(const std::string& url, const std::string& authToken) {
    // Placeholder for network loading
    return false;
}

bool Config::merge(const Config& other) {
    bool changed = false;
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            pImpl->entries.push_back(entry);
            changed = true;
        } else if (it->value != entry.value) {
            it->value = entry.value;
            it->source = entry.source;
            changed = true;
        }
    }
    
    return changed;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    if (ext == "json") {
        return saveToJSON(filepath, pretty);
    } else if (ext == "yaml" || ext == "yml") {
        // For now, use JSON for YAML as well
        return saveToJSON(filepath, pretty);
    } else if (ext == "toml") {
        // For now, use JSON for TOML as well
        return saveToJSON(filepath, pretty);
    } else {
        // Simple format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toString(entry.value) << "\n";
        }
        
        return true;
    }
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = entry.value;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        file << json.dump(2);
    } else {
        file << json.dump();
    }
    
    return true;
}

std::string Config::toJSON(bool pretty) const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = entry.value;
    }
    
    if (pretty) {
        return json.dump(2);
    } else {
        return json.dump();
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    // Invalidate cache
    pImpl->cache.erase(key);
    
    // Update performance metrics
    if (pImpl->monitoringEnabled) {
        updatePerformanceMetrics();
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const std::map<std::string, ConfigValue>& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
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
    
    // Invalidate cache
    pImpl->cache.erase(key);
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
    pImpl->cache.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "  Monitoring: " << (pImpl->monitoringEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "  Caching: " << (pImpl->cachingEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "  Validation Rules: " << pImpl->validationRules.size() << "\n";
    oss << "  Presets: " << pImpl->presets.size() << "\n";
    oss << "  Inheritance Sources: " << pImpl->inheritanceSources.size() << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
                oss << "{object}";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

bool Config::addPreset(const ConfigPreset& preset) {
    auto it = pImpl->presets.find(preset.name);
    if (it != pImpl->presets.end()) {
        return false; // Preset already exists
    }
    
    pImpl->presets[preset.name] = preset;
    return true;
}

bool Config::removePreset(const std::string& name) {
    auto it = pImpl->presets.find(name);
    if (it == pImpl->presets.end()) {
        return false; // Preset not found
    }
    
    pImpl->presets.erase(it);
    return true;
}

bool Config::applyPreset(const std::string& name, ConfigSource source) {
    auto it = pImpl->presets.find(name);
    if (it == pImpl->presets.end()) {
        return false; // Preset not found
    }
    
    bool changed = false;
    for (const auto& [key, value] : it->second.settings) {
        auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                      [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (existingIt == pImpl->entries.end()) {
            pImpl->entries.emplace_back(key, value, source);
            changed = true;
        } else if (existingIt->value != value) {
            existingIt->value = value;
            existingIt->source = source;
            changed = true;
        }
    }
    
    // Invalidate cache
    if (changed) {
        pImpl->cache.clear();
    }
    
    return changed;
}

std::vector<std::string> Config::getPresetNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->presets.size());
    for (const auto& preset : pImpl->presets) {
        names.push_back(preset.first);
    }
    return names;
}

ConfigPreset Config::getPreset(const std::string& name) const {
    auto it = pImpl->presets.find(name);
    if (it == pImpl->presets.end()) {
        throw std::runtime_error("Preset not found: " + name);
    }
    return it->second;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& keyPattern) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
                      [&keyPattern](const ValidationRule& rule) {
                          return matchesPattern(rule.keyPattern, keyPattern);
                      }),
        pImpl->validationRules.end()
    );
}

bool Config::validate(ValidationLevel level) const {
    std::vector<std::string> errors = getValidationErrors(level);
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors(ValidationLevel level) const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        bool validated = false;
        
        for (const auto& rule : pImpl->validationRules) {
            if (matchesPattern(entry.key, rule.keyPattern)) {
                validated = true;
                if (!rule.validator(entry.value)) {
                    errors.push_back("Key \"" + entry.key + "\": " + rule.errorMessage);
                }
                
                if (level == ValidationLevel::Strict) {
                    if (!rule.optional && !has(entry.key)) {
                        errors.push_back("Key \"" + entry.key + "\": is required but not found");
                    }
                }
                
                break;
            }
        }
        
        if (!validated && level != ValidationLevel::Minimal) {
            // Only warn for non-optional keys
            for (const auto& rule : pImpl->validationRules) {
                if (!rule.optional && matchesPattern(entry.key, rule.keyPattern)) {
                    errors.push_back("Key \"" + entry.key + "\": no validation rule found");
                    break;
                }
            }
        }
    }
    
    return errors;
}

void Config::addInheritanceSource(const std::string& sourceName, const Config& config) {
    pImpl->inheritanceSources[sourceName] = config;
}

void Config::clearInheritanceSources() {
    pImpl->inheritanceSources.clear();
}

std::vector<std::string> Config::queryKeys(const std::string& pattern) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (matchesPattern(entry.key, pattern)) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<ConfigEntry> Config::queryEntries(const std::string& pattern) const {
    std::vector<ConfigEntry> entries;
    
    for (const auto& entry : pImpl->entries) {
        if (matchesPattern(entry.key, pattern)) {
            entries.push_back(entry);
        }
    }
    
    return entries;
}

std::map<std::string, ConfigValue> Config::query(const std::string& pattern) const {
    std::map<std::string, ConfigValue> result;
    
    for (const auto& entry : pImpl->entries) {
        if (matchesPattern(entry.key, pattern)) {
            result[entry.key] = entry.value;
        }
    }
    
    return result;
}

Config Config::filter(const std::function<bool(const ConfigEntry&)>& predicate) const {
    Config filtered;
    filtered.pImpl->entries = pImpl->entries;
    std::copy_if(filtered.pImpl->entries.begin(), filtered.pImpl->entries.end(),
                std::back_inserter(filtered.pImpl->entries), predicate);
    return filtered;
}

Config Config::filterBySource(ConfigSource source) const {
    Config filtered;
    filtered.pImpl->entries = pImpl->entries;
    std::copy_if(filtered.pImpl->entries.begin(), filtered.pImpl->entries.end(),
                std::back_inserter(filtered.pImpl->entries),
                [source](const ConfigEntry& entry) { return entry.source == source; });
    return filtered;
}

Config Config::filterByKeyPrefix(const std::string& prefix) const {
    Config filtered;
    filtered.pImpl->entries = pImpl->entries;
    std::copy_if(filtered.pImpl->entries.begin(), filtered.pImpl->entries.end(),
                std::back_inserter(filtered.pImpl->entries),
                [&prefix](const ConfigEntry& entry) { return entry.key.find(prefix) == 0; });
    return filtered;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::map<ConfigSource, size_t> Config::getSourceCounts() const {
    std::map<ConfigSource, size_t> counts;
    for (const auto& entry : pImpl->entries) {
        counts[entry.source]++;
    }
    return counts;
}

std::set<std::string> Config::getKeySet() const {
    std::set<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.insert(entry.key);
    }
    return keys;
}

std::vector<std::pair<std::string, ConfigSource>> Config::getSortedEntries() const {
    std::vector<std::pair<std::string, ConfigSource>> sorted;
    sorted.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        sorted.emplace_back(entry.key, entry.source);
    }
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    return sorted;
}

void Config::resetToDefaults() {
    // Reset to development preset
    applyPreset("development", ConfigSource::Default);
}

void Config::snapshot(const std::string& name) {
    pImpl->snapshots[name] = pImpl->entries;
}

void Config::restoreSnapshot(const std::string& name) {
    auto it = pImpl->snapshots.find(name);
    if (it != pImpl->snapshots.end()) {
        pImpl->entries = it->second;
        pImpl->cache.clear();
    }
}

std::vector<std::string> Config::getSnapshotNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->snapshots.size());
    for (const auto& snapshot : pImpl->snapshots) {
        names.push_back(snapshot.first);
    }
    return names;
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries)\n";
    oss << "Source Counts:";
    auto sourceCounts = getSourceCounts();
    for (const auto& [source, count] : sourceCounts) {
        oss << " " << static_cast<int>(source) << "=" << count;
    }
    oss << "\nValidation Status: " << (validate() ? "PASS" : "FAIL") << "\n";
    
    return oss.str();
}

bool Config::fromString(const std::string& str) {
    // Simple parser for string format
    std::istringstream iss(str);
    std::string line;
    
    clear();
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            set(key, value, ConfigSource::Runtime);
        }
    }
    
    return true;
}

std::vector<ConfigEntry> Config::diff(const Config& other) const {
    std::vector<ConfigEntry> diffEntries;
    
    // Check entries in this config not in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            diffEntries.push_back(entry);
        }
    }
    
    // Check entries in other config not in this
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            diffEntries.push_back(otherEntry);
        }
    }
    
    return diffEntries;
}

bool Config::hasChanges(const Config& other) const {
    return !diff(other).empty();
}

void Config::complete(const Config& templateConfig) {
    // Auto-fill missing values with defaults from template
    for (const auto& templateEntry : templateConfig.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == templateEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            // Try to convert template value to appropriate type
            ConfigValue defaultValue;
            try {
                defaultValue = std::get<ConfigValue>(templateEntry.value);
            } catch (...) {
                // Skip if type conversion fails
                continue;
            }
            set(templateEntry.key, defaultValue, ConfigSource::Runtime,
                "Auto-filled from template");
        }
    }
}

void Config::optimizeForPerformance() {
    // Remove unnecessary entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [](const ConfigEntry& entry) {
                          return entry.key.find("temp_") == 0 ||
                                 entry.key.find("debug_") == 0;
                      }),
        pImpl->entries.end()
    );
    
    // Enable caching
    enableCaching(true);
}

void Config::optimizeForMemory() {
    // Reduce memory usage by converting to simpler types
    for (auto& entry : pImpl->entries) {
        // Try to convert complex types to simpler ones
        if (std::holds_alternative<std::vector<double>>(entry.value)) {
            std::vector<double>& vec = std::get<std::vector<double>>(entry.value);
            if (vec.size() <= 10) {
                // Keep as is for small vectors
            }
        }
    }
    
    enableCaching(true);
}

void Config::optimizeForNetwork() {
    // Minimize network usage
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [](const ConfigEntry& entry) {
                          return entry.description.find("network") != std::string::npos;
                      }),
        pImpl->entries.end()
    );
}

void Config::enableMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    if (enable) {
        updatePerformanceMetrics();
    }
}

bool Config::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

std::map<std::string, double> Config::getPerformanceMetrics() const {
    std::map<std::string, double> metrics;
    
    if (pImpl->monitoringEnabled) {
        metrics["entry_count"] = static_cast<double>(pImpl->entries.size());
        metrics["validation_time"] = 0.0; // Would be measured in real implementation
        metrics["memory_usage"] = static_cast<double>(sizeof(ConfigEntry) * pImpl->entries.size());
        
        // Calculate source distribution
        auto sourceCounts = getSourceCounts();
        for (const auto& [source, count] : sourceCounts) {
            metrics["source_" + std::to_string(static_cast<int>(source))] = static_cast<double>(count);
        }
    }
    
    return metrics;
}

void Config::enableCaching(bool enable) {
    pImpl->cachingEnabled = enable;
    if (!enable) {
        pImpl->cache.clear();
    }
}

bool Config::isCachingEnabled() const {
    return pImpl->cachingEnabled;
}

// Static methods for creating validation rules
ValidationRule Config::createRangeValidator(const std::string& keyPattern, 
                                           double min, double max,
                                           const std::string& errorMsg) {
    ValidationRule rule;
    rule.keyPattern = keyPattern;
    rule.validator = [min, max](const ConfigValue& value) {
        try {
            double val;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return false;
            }
            return val >= min && val <= max;
        } catch (...) {
            return false;
        }
    };
    rule.errorMessage = errorMsg;
    rule.numericRange = {min, max};
    return rule;
}

ValidationRule Config::createEnumValidator(const std::string& keyPattern,
                                         const std::vector<std::string>& allowedValues,
                                         const std::string& errorMsg) {
    ValidationRule rule;
    rule.keyPattern = keyPattern;
    rule.validator = [allowedValues](const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            return std::find(allowedValues.begin(), allowedValues.end(), 
                           std::get<std::string>(value)) != allowedValues.end();
        }
        return false;
    };
    rule.errorMessage = errorMsg;
    rule.allowedValues = allowedValues;
    return rule;
}

ValidationRule Config::createTypeValidator(const std::string& keyPattern,
                                         const std::type_index& type,
                                         const std::string& errorMsg) {
    ValidationRule rule;
    rule.keyPattern = keyPattern;
    rule.validator = [type](const ConfigValue& value) {
        return std::type_index(value.type()) == type;
    };
    rule.errorMessage = errorMsg;
    return rule;
}

// Static methods for creating presets
ConfigPreset Config::createDevelopmentPreset() {
    ConfigPreset preset;
    preset.name = "development";
    preset.description = "Development configuration with debugging enabled";
    preset.isSystem = false;
    
    preset.settings = {
        {"neuron_count", 1000},
        {"region_count", 3},
        {"simulation_timestep", 0.001},
        {"learning_strategy", "balanced"},
        {"plasticity_rate", 1.0},
        {"enable_debug_logging", true},
        {"enable_validation", true},
        {"caching_enabled", false}
    };
    
    return preset;
}

ConfigPreset Config::createProductionPreset() {
    ConfigPreset preset;
    preset.name = "production";
    preset.description = "Production configuration optimized for performance";
    preset.isSystem = false;
    
    preset.settings = {
        {"neuron_count", 5000},
        {"region_count", 5},
        {"simulation_timestep", 0.0005},
        {"learning_strategy", "aggressive"},
        {"plasticity_rate", 1.5},
        {"enable_debug_logging", false},
        {"enable_validation", true},
        {"caching_enabled", true}
    };
    
    return preset;
}

ConfigPreset Config::createTestingPreset() {
    ConfigPreset preset;
    preset.name = "testing";
    preset.description = "Testing configuration for unit tests";
    preset.isSystem = false;
    
    preset.settings = {
        {"neuron_count", 100},
        {"region_count", 1},
        {"simulation_timestep", 0.01},
        {"learning_strategy", "conservative"},
        {"plasticity_rate", 0.5},
        {"enable_debug_logging", true},
        {"enable_validation", true},
        {"caching_enabled", false}
    };
    
    return preset;
}

ConfigPreset Config::createBenchmarkPreset() {
    ConfigPreset preset;
    preset.name = "benchmark";
    preset.description = "Benchmark configuration for performance testing";
    preset.isSystem = false;
    
    preset.settings = {
        {"neuron_count", 10000},
        {"region_count", 10},
        {"simulation_timestep", 0.0001},
        {"learning_strategy", "balanced"},
        {"plasticity_rate", 2.0},
        {"enable_debug_logging", false},
        {"enable_validation", false},
        {"caching_enabled", true}
    };
    
    return preset;
}

// Implementation of private helper methods
bool Config::matchesPattern(const std::string& key, const std::string& pattern) {
    if (pattern == "*") {
        return true;
    }
    
    // Simple wildcard matching
    bool match = true;
    size_t keyPos = 0;
    size_t patternPos = 0;
    
    while (keyPos < key.size() && patternPos < pattern.size()) {
        if (pattern[patternPos] == '*') {
            // Skip consecutive wildcards
            while (patternPos + 1 < pattern.size() && pattern[patternPos + 1] == '*') {
                patternPos++;
            }
            
            // If end of pattern, match rest of key
            if (patternPos + 1 == pattern.size()) {
                return true;
            }
            
            // Try to match any remaining key characters
            size_t tempKeyPos = keyPos;
            size_t tempPatternPos = patternPos + 1;
            
            while (tempKeyPos < key.size() && tempPatternPos < pattern.size()) {
                if (key[tempKeyPos] == pattern[tempPatternPos]) {
                    tempKeyPos++;
                    tempPatternPos++;
                } else {
                    tempKeyPos++;
                }
            }
            
            return tempPatternPos == pattern.size();
        } else if (key[keyPos] == pattern[patternPos]) {
            keyPos++;
            patternPos++;
        } else {
            return false;
        }
    }
    
    return keyPos == key.size() && patternPos == pattern.size();
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            oss << "{object}";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::validateValue(const ConfigEntry& entry, ValidationLevel level) const {
    for (const auto& rule : pImpl->validationRules) {
        if (matchesPattern(entry.key, rule.keyPattern)) {
            if (!rule.validator(entry.value)) {
                return false;
            }
            return true;
        }
    }
    
    return level != ValidationLevel::Strict;
}

void Config::updatePerformanceMetrics() const {
    pImpl->performanceMetrics["last_update"] = std::chrono::system_clock::now().time_since_epoch().count();
    pImpl->performanceMetrics["entry_count"] = static_cast<double>(pImpl->entries.size());
}

// Other private helper implementations would go here

} // namespace nlm