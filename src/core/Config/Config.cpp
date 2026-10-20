Compiled configuration system implementation
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <unordered_set>
#include <ctime>

// Include nlohmann JSON for configuration file support
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<EnhancedConfigEntry> entries;
    std::vector<ConfigHistoryEntry> history;
    std::vector<ConfigChangeEvent> changeEvents;
    
    std::unordered_map<std::string, ConfigMetadata> metadata;
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigMetadata>> categorySchemas;
    
    std::vector<ConfigChangeCallback> changeCallbacks;
    
    std::string currentSessionId;
    std::chrono::system_clock::time_point sessionStartTime;
    
    size_t maxHistoryEntries = 1000;
    std::chrono::system_clock::duration cleanupThreshold;
    
    // Statistics
    size_t totalChanges = 0;
    size_t totalRollbacks = 0;
    
    Impl() : currentSessionId("default"), sessionStartTime(std::chrono::system_clock::now()) {
        cleanupThreshold = std::chrono::hours(24); // Clean up old sessions after 24 hours
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with basic schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Basic configuration schemas
    {
        auto& schema = pImpl->categorySchemas["brain"];
        
        schema["neuron_count"] = ConfigMetadata(
            "Total number of neurons in the brain", 
            "brain", 
            "int", 
            true, 
            static_cast<int>(1000),
            std::make_optional<std::pair<double, double>>(100.0, 10000.0),
            std::nullopt,
            std::nullopt
        );
        
        schema["region_count"] = ConfigMetadata(
            "Number of brain regions", 
            "brain", 
            "int", 
            true,
            static_cast<int>(1),
            std::make_optional<std::pair<double, double>>(1.0, 100.0)
        );
        
        schema["connection_probability"] = ConfigMetadata(
            "Probability of connections between neurons", 
            "brain", 
            "double", 
            true,
            static_cast<double>(0.1),
            std::make_optional<std::pair<double, double>>(0.01, 0.5)
        );
        
        schema["simulation_timestep"] = ConfigMetadata(
            "Simulation timestep in seconds", 
            "brain", 
            "double", 
            true,
            static_cast<double>(0.001),
            std::make_optional<std::pair<double, double>>(0.0001, 0.1)
        );
    }
    
    {
        auto& schema = pImpl->categorySchemas["plasticity"];
        
        schema["stdp_ltp_weight"] = ConfigMetadata(
            "STDP Long-Term Potentiation weight", 
            "plasticity", 
            "double", 
            true,
            static_cast<double>(0.01),
            std::make_optional<std::pair<double, double>>(0.001, 0.1)
        );
        
        schema["stdp_ltd_weight"] = ConfigMetadata(
            "STDP Long-Term Depression weight", 
            "plasticity", 
            "double", 
            true,
            static_cast<double>(0.012),
            std::make_optional<std::pair<double, double>>(0.001, 0.1)
        );
        
        schema["stdp_tau"] = ConfigMetadata(
            "STDP time constant", 
            "plasticity", 
            "double", 
            true,
            static_cast<double>(20.0),
            std::make_optional<std::pair<double, double>>(1.0, 100.0)
        );
        
        schema["synaptogenesis_rate"] = ConfigMetadata(
            "Structural synaptogenesis rate", 
            "plasticity", 
            "double", 
            true,
            static_cast<double>(0.0001),
            std::make_optional<std::pair<double, double>>(0.00001, 0.001)
        );
        
        schema["pruning_rate"] = ConfigMetadata(
            "Structural pruning rate", 
            "plasticity", 
            "double", 
            true,
            static_cast<double>(0.00001),
            std::make_optional<std::pair<double, double>>(0.000001, 0.0001)
        );
    }
    
    {
        auto& schema = pImpl->categorySchemas["development"];
        
        schema["development_enabled"] = ConfigMetadata(
            "Enable development system", 
            "development", 
            "bool", 
            true,
            static_cast<bool>(true)
        );
        
        schema["developmental_stage"] = ConfigMetadata(
            "Current developmental stage", 
            "development", 
            "int", 
            false,
            static_cast<int>(0) // 0 = Initial, 1 = CriticalPeriod, 2 = Maturation, 3 = Adult, 4 = Aging
        );
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, pImpl->currentSessionId);
}

bool Config::loadFromFile(const std::string& filepath, const std::string& sessionId) {
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Process JSON data
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (it->is_number_integer()) {
                configValue = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                configValue = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays
                auto arr = it->get<std::vector<nlohmann::json>>();
                if (!arr.empty() && arr[0].is_number_integer()) {
                    configValue = arr.get<std::vector<int>>();
                } else if (!arr.empty() && arr[0].is_number_float()) {
                    configValue = arr.get<std::vector<double>>();
                } else if (!arr.empty() && arr[0].is_boolean()) {
                    configValue = arr.get<std::vector<bool>>();
                } else {
                    configValue = arr.get<std::vector<std::string>>();
                }
            }
            
            set(key, configValue, ConfigSource::File, sessionId);
        }
        
        return true;
        
    } catch (const std::exception&) {
        // Fall back to simple file format
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
                
                set(key, value, ConfigSource::File, sessionId);
            }
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, pImpl->currentSessionId);
}

bool Config::loadFromArgs(int argc, char** argv, const std::string& sessionId) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine, sessionId);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, sessionId);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, false); // Default: no pretty print for backward compatibility
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    if (prettyPrint) {
        file << jsonData.dump(2);
    } else {
        file << jsonData.dump();
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const EnhancedConfigEntry& e) { return e.key == key; });
    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& sessionId) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const EnhancedConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool isNewEntry = (it == pImpl->entries.end());
    
    if (isNewEntry) {
        pImpl->entries.emplace_back(key, value, source);
        it = std::prev(pImpl->entries.end());
        
        // Add default metadata for new entries
        if (pImpl->metadata.find(key) == pImpl->metadata.end()) {
            std::string type = "auto";
            if (std::holds_alternative<int>(value)) type = "int";
            else if (std::holds_alternative<int64_t>(value)) type = "int64_t";
            else if (std::holds_alternative<double>(value)) type = "double";
            else if (std::holds_alternative<bool>(value)) type = "bool";
            else if (std::holds_alternative<std::string>(value)) type = "string";
            else if (std::holds_alternative<std::vector<int>>(value)) type = "int_array";
            else if (std::holds_alternative<std::vector<double>>(value)) type = "double_array";
            else if (std::holds_alternative<std::vector<std::string>>(value)) type = "string_array";
            else if (std::holds_alternative<std::vector<bool>>(value)) type = "bool_array";
            
            pImpl->metadata[key] = ConfigMetadata("", "", type);
        }
    } else {
        oldValue = it->value;
        it->value = value;
        it->source = source;
        it->lastModified = std::chrono::system_clock::now();
        it->lastModifiedSession = sessionId;
    }
    
    // Validate new value
    if (validateValue(key, value)) {
        // Record history
        addHistoryEntry(key, oldValue, value, source, sessionId);
        
        // Notify callbacks
        notifyChangeCallbacks(key, oldValue, value, source);
    } else {
        throw ConfigValidationError("Invalid value for key '" + key + "': " + valueToString(value));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& sessionId) {
    set(key, ConfigValue(value), source, sessionId);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& sessionId) {
    set(key, ConfigValue(value), source, sessionId);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& sessionId) {
    set(key, ConfigValue(value), source, sessionId);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& sessionId) {
    set(key, ConfigValue(value), source, sessionId);
}

bool Config::modify(const std::string& key, const ConfigValue& newValue,
                   const std::string& sessionId, bool validate) {
    if (!has(key)) {
        return false;
    }
    
    if (validate && !validateValue(key, newValue)) {
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const EnhancedConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        it->value = newValue;
        it->source = ConfigSource::Runtime;
        it->lastModified = std::chrono::system_clock::now();
        it->lastModifiedSession = sessionId;
        
        addHistoryEntry(key, oldValue, newValue, ConfigSource::Runtime, sessionId);
        notifyChangeCallbacks(key, oldValue, newValue, ConfigSource::Runtime);
        
        return true;
    }
    
    return false;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const EnhancedConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key, const std::string& sessionId) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const EnhancedConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        pImpl->entries.erase(it);
        
        addHistoryEntry(key, oldValue, ConfigValue(), ConfigSource::Runtime, sessionId);
        notifyChangeCallbacks(key, oldValue, ConfigValue(), ConfigSource::Runtime);
    }
}

void Config::clear(const std::string& sessionId) {
    // Store old values for history
    std::vector<std::pair<std::string, ConfigValue>> oldValues;
    for (const auto& entry : pImpl->entries) {
        oldValues.emplace_back(entry.key, entry.value);
    }
    
    pImpl->entries.clear();
    
    // Record history for all cleared entries
    for (const auto& oldVal : oldValues) {
        addHistoryEntry(oldVal.first, oldVal.second, ConfigValue(), ConfigSource::Runtime, sessionId);
        notifyChangeCallbacks(oldVal.first, oldVal.second, ConfigValue(), ConfigSource::Runtime);
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto metaIt = pImpl->metadata.find(key);
    return metaIt != pImpl->metadata.end() ? metaIt->second.description : "";
}

std::string Config::getCategory(const std::string& key) const {
    auto metaIt = pImpl->metadata.find(key);
    return metaIt != pImpl->metadata.end() ? metaIt->second.category : "";
}

std::string Config::getType(const std::string& key) const {
    auto metaIt = pImpl->metadata.find(key);
    return metaIt != pImpl->metadata.end() ? metaIt->second.type : "auto";
}

bool Config::isRequired(const std::string& key) const {
    auto metaIt = pImpl->metadata.find(key);
    return metaIt != pImpl->metadata.end() && metaIt->second.required;
}

void Config::setDefault(const std::string& key, const std::variant<int, int64_t, double, std::string, bool>& defaultValue,
                       const std::string& description, const std::string& category,
                       const std::string& type, bool required) {
    // Create metadata
    ConfigMetadata metadata;
    metadata.description = description;
    metadata.category = category;
    metadata.type = type;
    metadata.required = required;
    metadata.defaultValue = defaultValue;
    
    pImpl->metadata[key] = metadata;
    
    // If key doesn't exist, add it with default value
    if (!has(key)) {
        set(key, defaultValue, ConfigSource::Default, "system");
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Validate against schema
    if (!validateAgainstSchema(key, value)) {
        return false;
    }
    
    // Check metadata-based validation
    auto metaIt = pImpl->metadata.find(key);
    if (metaIt != pImpl->metadata.end()) {
        const auto& metadata = metaIt->second;
        
        // Check required field
        if (metadata.required && isEmptyValue(value)) {
            return false;
        }
        
        // Check type validation
        if (!validateType(metadata.type, value)) {
            return false;
        }
        
        // Check range validation
        if (metadata.range.has_value()) {
            if (!validateRange(metadata.range.value(), value)) {
                return false;
            }
        }
        
        // Check array size validation
        if (metadata.minArraySize.has_value() && isArray(value)) {
            size_t size = getArraySize(value);
            if (size < metadata.minArraySize.value()) {
                return false;
            }
        }
        
        if (metadata.maxArraySize.has_value() && isArray(value)) {
            size_t size = getArraySize(value);
            if (size > metadata.maxArraySize.value()) {
                return false;
            }
        }
        
        // Check custom validator
        if (metadata.validator && !metadata.validator(value)) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateAgainstSchema(const std::string& key, const ConfigValue& value) const {
    // Check category schemas
    for (const auto& [category, schema] : pImpl->categorySchemas) {
        auto keyIt = schema.find(key);
        if (keyIt != schema.end()) {
            // Schema validation logic would go here
            return true;
        }
    }
    
    return true; // No schema found for this key
}

bool Config::isEmptyValue(const ConfigValue& value) const {
    if (std::holds_alternative<std::monostate>(value)) return true;
    if (std::holds_alternative<int>(value) && std::get<int>(value) == 0) return true;
    if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) == 0) return true;
    if (std::holds_alternative<double>(value) && std::get<double>(value) == 0.0) return true;
    if (std::holds_alternative<bool>(value) && !std::get<bool>(value)) return true;
    if (std::holds_alternative<std::string>(value) && std::get<std::string>(value).empty()) return true;
    if (std::holds_alternative<std::vector<int>>(value) && std::get<std::vector<int>>(value).empty()) return true;
    if (std::holds_alternative<std::vector<double>>(value) && std::get<std::vector<double>>(value).empty()) return true;
    if (std::holds_alternative<std::vector<std::string>>(value) && std::get<std::vector<std::string>>(value).empty()) return true;
    if (std::holds_alternative<std::vector<bool>>(value) && std::get<std::vector<bool>>(value).empty()) return true;
    return false;
}

bool Config::validateType(const std::string& type, const ConfigValue& value) const {
    if (type == "auto") return true;
    
    if (type == "int" && !std::holds_alternative<int>(value)) return false;
    if (type == "int64_t" && !std::holds_alternative<int64_t>(value)) return false;
    if (type == "double" && !std::holds_alternative<double>(value)) return false;
    if (type == "bool" && !std::holds_alternative<bool>(value)) return false;
    if (type == "string" && !std::holds_alternative<std::string>(value)) return false;
    if (type == "int_array" && !std::holds_alternative<std::vector<int>>(value)) return false;
    if (type == "double_array" && !std::holds_alternative<std::vector<double>>(value)) return false;
    if (type == "string_array" && !std::holds_alternative<std::vector<std::string>>(value)) return false;
    if (type == "bool_array" && !std::holds_alternative<std::vector<bool>>(value)) return false;
    
    return true;
}

bool Config::validateRange(const std::pair<double, double>& range, const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value)) >= range.first &&
               static_cast<double>(std::get<int>(value)) <= range.second;
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value)) >= range.first &&
               static_cast<double>(std::get<int64_t>(value)) <= range.second;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) >= range.first && std::get<double>(value) <= range.second;
    }
    return false;
}

bool Config::isArray(const ConfigValue& value) const {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value) ||
           std::holds_alternative<std::vector<bool>>(value);
}

 size_t Config::getArraySize(const ConfigValue& value) const {
    if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value).size();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value).size();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value).size();
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        return std::get<std::vector<bool>>(value).size();
    }
    return 0;
}

void Config::addHistoryEntry(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue,
                           ConfigSource source, const std::string& sessionId) {
    pImpl->history.emplace_back(key, oldValue, newValue, source, sessionId);
    pImpl->totalChanges++;
    
    // Record change event
    pImpl->changeEvents.emplace_back(key, newValue, source, std::chrono::system_clock::now(), sessionId);
    
    // Clean up old history if needed
    if (pImpl->history.size() > pImpl->maxHistoryEntries) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (pImpl->changeEvents.size() > pImpl->maxHistoryEntries) {
        pImpl->changeEvents.erase(pImpl->changeEvents.begin());
    }
    
    // Cleanup old sessions
    cleanupOldSessions();
}

void Config::notifyChangeCallbacks(const std::string& key, const ConfigValue& oldValue,
                                   const ConfigValue& newValue, ConfigSource source) {
    for (const auto& callback : pImpl->changeCallbacks) {
        try {
            callback(key, oldValue, newValue, source);
        } catch (const std::exception&) {
            // Ignore callback errors to prevent crashes
        }
    }
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

void Config::clearChangeCallbacks() {
    pImpl->changeCallbacks.clear();
}

void Config::setSessionId(const std::string& sessionId) {
    pImpl->currentSessionId = sessionId;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
}

std::string Config::getSessionId() const {
    return pImpl->currentSessionId;
}

std::vector<std::string> Config::getActiveSessions() const {
    std::vector<std::string> sessions;
    for (const auto& entry : pImpl->entries) {
        if (std::find(sessions.begin(), sessions.end(), entry.lastModifiedSession) == sessions.end()) {
            sessions.push_back(entry.lastModifiedSession);
        }
    }
    
    // Add current session
    if (std::find(sessions.begin(), sessions.end(), pImpl->currentSessionId) == sessions.end()) {
        sessions.push_back(pImpl->currentSessionId);
    }
    
    return sessions;
}

void Config::cleanupSession(const std::string& sessionId) {
    if (sessionId == "system" || sessionId == pImpl->currentSessionId) {
        return; // Never cleanup system or current session
    }
    
    // Mark entries from this session for cleanup
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sessionId](const EnhancedConfigEntry& e) {
            return e.lastModifiedSession == sessionId && e.source != ConfigSource::Default;
        });
    
    pImpl->entries.erase(it, pImpl->entries.end());
    
    // Remove history from this session
    pImpl->history.erase(
        std::remove_if(pImpl->history.begin(), pImpl->history.end(),
            [&sessionId](const ConfigHistoryEntry& e) { return e.sessionId == sessionId; }),
        pImpl->history.end()
    );
}

void Config::cleanupOldSessions() {
    auto now = std::chrono::system_clock::now();
    
    // Remove history entries older than threshold
    auto historyIt = std::remove_if(pImpl->history.begin(), pImpl->history.end(),
        [this, now](const ConfigHistoryEntry& e) {
            return (now - e.timestamp) > pImpl->cleanupThreshold;
        });
    
    pImpl->history.erase(historyIt, pImpl->history.end());
    
    // Remove old sessions from entries
    auto entriesIt = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [this, now](const EnhancedConfigEntry& e) {
            if (e.lastModifiedSession == "system" || e.lastModifiedSession == pImpl->currentSessionId) {
                return false;
            }
            
            return (now - e.lastModified) > pImpl->cleanupThreshold;
        });
    
    pImpl->entries.erase(entriesIt, pImpl->entries.end());
}

std::vector<ConfigHistoryEntry> Config::getHistory(const std::string& key, size_t limit) const {
    std::vector<ConfigHistoryEntry> result;
    for (auto it = pImpl->history.rbegin(); it != pImpl->history.rend() && result.size() < limit; ++it) {
        if (it->key == key) {
            result.push_back(*it);
        }
    }
    return result;
}

std::vector<ConfigChangeEvent> Config::getRecentChanges(size_t limit) const {
    if (limit > pImpl->changeEvents.size()) {
        limit = pImpl->changeEvents.size();
    }
    
    std::vector<ConfigChangeEvent> result(pImpl->changeEvents.end() - limit, pImpl->changeEvents.end());
    return result;
}

size_t Config::getChangeCount() const {
    return pImpl->totalChanges;
}

std::string Config::summary() const {
    return summary(false);
}

std::string Config::summary(bool includeHistory) const {
    std::ostringstream oss;
    oss << "Configuration Summary (Session: " << pImpl->currentSessionId << ")\n";
    oss << "Entries: " << pImpl->entries.size() << "\n";
    oss << "Total Changes: " << pImpl->totalChanges << "\n";
    oss << "Active Sessions: " << getActiveSessions().size() << "\n";
    
    oss << "\nConfiguration Entries:\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ", " << entry.lastModifiedSession << ")\n";
    }
    
    if (includeHistory) {
        oss << "\nRecent Changes:\n";
        for (size_t i = 0; i < std::min(pImpl->changeEvents.size(), size_t(10)); ++i) {
            const auto& event = pImpl->changeEvents[i];
            oss << "  " << event.timestamp << " " << event.key << " = " << valueToString(event.value) << " (" << event.source << ")\n";
        }
    }
    
    return oss.str();
}

std::string Config::summary(const std::string& sessionId) const {
    std::ostringstream oss;
    oss << "Configuration Summary (Session: " << sessionId << ")\n";
    
    size_t sessionEntries = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.lastModifiedSession == sessionId) {
            sessionEntries++;
        }
    }
    
    oss << "Entries in this session: " << sessionEntries << "\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.lastModifiedSession == sessionId) {
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
            oss << "]\n";
        }
    }
    
    return oss.str();
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

void Config::registerConfigSchema(const std::string& category, const std::unordered_map<std::string, ConfigMetadata>& schema) {
    pImpl->categorySchemas[category] = schema;
}

bool Config::validateConfigSchema(const std::string& category, const Config& config) {
    auto it = config.pImpl->categorySchemas.find(category);
    if (it == config.pImpl->categorySchemas.end()) {
        return true; // No schema for this category
    }
    
    const auto& schema = it->second;
    
    for (const auto& [key, metadata] : schema) {
        if (metadata.required && !config.has(key)) {
            return false;
        }
        
        if (config.has(key)) {
            auto value = config.get<ConfigValue>(key);
            if (value && !config.validateValue(key, *value)) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getAllCategories() {
    std::vector<std::string> categories;
    for (const auto& [category, schema] : std::unordered_map<std::string, std::unordered_map<std::string, ConfigMetadata>>()) {
        categories.push_back(category);
    }
    return categories;
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
