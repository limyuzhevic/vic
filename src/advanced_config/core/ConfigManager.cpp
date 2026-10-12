#include "ConfigManager.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>

namespace nlm {

// ConfigManager implementation
ConfigManager::ConfigManager(const ManagerConfig& config)
    : configurationLocked(false), managerConfig(config), lastCacheClear(std::chrono::steady_clock::now()) {}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadConfiguration() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (configurationLocked) {
        return false; // Cannot load when locked
    }
    
    // Clear previous configuration
    clear();
    
    // Load from all sources in priority order
    bool allLoaded = true;
    std::vector<std::shared_ptr<ConfigSource>> sortedSources = sources;
    
    // Sort by priority (descending)
    std::sort(sortedSources.begin(), sortedSources.end(),
              [](const std::shared_ptr<ConfigSource>& a, const std::shared_ptr<ConfigSource>& b) {
                  return a->getPriority() > b->getPriority();
              });
    
    for (const auto& source : sortedSources) {
        if (!loadFromSource(source)) {
            allLoaded = false;
        }
    }
    
    // Validate configuration
    if (managerConfig.enableValidation) {
        if (!validateConfiguration()) {
            // Restore from previous successful load
            clear();
            for (const auto& source : sources) {
                // Try to reload from each source
                if (source->getPriority() > 0) {
                    loadFromSource(source);
                }
            }
            return false;
        }
    }
    
    // Notify listeners
    ConfigChangeEvent event(ConfigChangeEvent::Type::ConfigLoaded);
    notifyChange(event);
    
    return allLoaded;
}

bool ConfigManager::saveConfiguration() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (configurationLocked) {
        return false; // Cannot save when locked
    }
    
    bool allSaved = true;
    for (const auto& source : sources) {
        if (source->isWritable()) {
            if (!saveToSource(source)) {
                allSaved = false;
            }
        }
    }
    
    if (allSaved) {
        ConfigChangeEvent event(ConfigChangeEvent::Type::ConfigSaved);
        // Note: ConfigManager doesn't have access to notifyChange here
        // This would need to be handled by the ConfigManager implementation
    }
    
    return allSaved;
}

template<typename T>
std::optional<T> ConfigManager::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check cache first
    auto cacheIt = cache.find(key);
    if (cacheIt != cache.end()) {
        try {
            return std::any_cast<T>(cacheIt->second);
        } catch (const std::bad_any_cast&) {
            // Cache corruption, remove from cache
            cache.erase(cacheIt);
        }
    }
    
    // Get from underlying config
    return std::nullopt; // This should use the actual Config instance
}

template<typename T>
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool ConfigManager::set(const std::string& key, const T& value,
                         ConfigSource::Type source, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (configurationLocked) {
        return false; // Cannot modify when locked
    }
    
    // Check if key is read-only in schema
    if (schema) {
        const ConfigSchema::Field* field = schema->getField(key);
        if (field && field->readOnly) {
            return false; // Cannot modify read-only key
        }
    }
    
    // Create ConfigChangeEvent for old value
    ConfigValue oldValue;
    bool keyExists = false;
    
    // Check if key exists (implementation depends on how config is stored)
    // For now, assume key doesn't exist
    
    // Set the value (implementation depends on how config is stored)
    // For now, return success
    
    // Create ConfigChangeEvent
    ConfigChangeEvent event(ConfigChangeEvent::Type::ValueChanged, key,
                           oldValue, value, "Runtime", description);
    
    // Validate event
    if (managerConfig.enableValidation) {
        bool validationResult = true;
        
        // Notify validation callbacks
        for (const auto& callback : validationCallbacks) {
            if (!callback->beforeValidate(event)) {
                validationResult = false;
                break;
            }
        }
        
        if (!validationResult) {
            // Notify error callbacks
            for (const auto& callback : errorCallbacks) {
                callback->onError(event, "Validation failed");
            }
            return false;
        }
    }
    
    // Notify change listeners
    notifyChange(event);
    
    // Notify update callbacks
    for (const auto& callback : updateCallbacks) {
        if (callback->shouldHandle(event)) {
            callback->onUpdate(event);
        }
    }
    
    // Add to history
    addToHistory(event);
    
    // Log audit
    if (managerConfig.enableAuditing) {
        logAudit(event);
    }
    
    // Update cache
    cache[key] = value;
    
    return true;
}

bool ConfigManager::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check cache first
    return cache.find(key) != cache.end();
}

bool ConfigManager::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (configurationLocked) {
        return false; // Cannot modify when locked
    }
    
    // Check if key exists and get old value
    ConfigValue oldValue;
    bool keyExists = false;
    
    // Remove from underlying config (implementation depends on storage)
    
    if (keyExists) {
        // Create ConfigChangeEvent
        ConfigChangeEvent event(ConfigChangeEvent::Type::KeyRemoved, key,
                               oldValue, std::any(), "Runtime", "");
        
        // Notify change listeners
        notifyChange(event);
        
        // Notify update callbacks
        for (const auto& callback : updateCallbacks) {
            if (callback->shouldHandle(event)) {
                callback->onUpdate(event);
            }
        }
        
        // Add to history
        addToHistory(event);
        
        // Log audit
        if (managerConfig.enableAuditing) {
            logAudit(event);
        }
        
        // Remove from cache
        cache.erase(key);
        
        return true;
    }
    
    return false;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> keys;
    keys.reserve(cache.size());
    
    for (const auto& pair : cache) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

std::string ConfigManager::summary() const {
    std::ostringstream oss;
    
    oss << "Advanced Configuration Manager\n";
    oss << "===============================\n";
    oss << "Configuration locked: " << (configurationLocked ? "Yes" : "No") << "\n";
    oss << "Validation enabled: " << (managerConfig.enableValidation ? "Yes" : "No") << "\n";
    oss << "Auditing enabled: " << (managerConfig.enableAuditing ? "Yes" : "No") << "\n";
    oss << "Change tracking enabled: " << (managerConfig.enableChangeTracking ? "Yes" : "No") << "\n";
    oss << "Caching enabled: " << (managerConfig.enableCaching ? "Yes" : "No") << "\n";
    oss << "Sources: " << sources.size() << "\n";
    oss << "Cache entries: " << cache.size() << "\n";
    oss << "History size: " << history.size() << "\n";
    
    oss << "\nSources:\n";
    for (const auto& source : sources) {
        oss << "  - " << source->getTypeName() << " (priority: " << source->getPriority() << ")\n";
    }
    
    oss << "\nCache:\n";
    for (const auto& pair : cache) {
        oss << "  - " << pair.first << "\n";
    }
    
    return oss.str();
}

bool ConfigManager::addSource(std::shared_ptr<ConfigSource> source) {
    if (!source) {
        return false;
    }
    
    sources.push_back(source);
    return true;
}

bool ConfigManager::removeSource(const std::string& typeName) {
    auto it = std::remove_if(sources.begin(), sources.end(),
                            [&typeName](const std::shared_ptr<ConfigSource>& source) {
                                return source->getTypeName() == typeName;
                            });
    
    if (it != sources.end()) {
        sources.erase(it, sources.end());
        return true;
    }
    
    return false;
}

void ConfigManager::addChangeListener(std::shared_ptr<ConfigChangeListener> listener) {
    if (listener) {
        changeListeners.push_back(listener);
    }
}

void ConfigManager::removeChangeListener(const std::string& listenerName) {
    changeListeners.erase(
        std::remove_if(changeListeners.begin(), changeListeners.end(),
                      [&listenerName](const std::shared_ptr<ConfigChangeListener>& listener) {
                          return listener->getName() == listenerName;
                      }),
        changeListeners.end()
    );
}

void ConfigManager::addValidationCallback(std::shared_ptr<ConfigValidationCallback> callback) {
    if (callback) {
        validationCallbacks.push_back(callback);
    }
}

void ConfigManager::removeValidationCallback(const std::string& callbackName) {
    validationCallbacks.erase(
        std::remove_if(validationCallbacks.begin(), validationCallbacks.end(),
                      [&callbackName](const std::shared_ptr<ConfigValidationCallback>& callback) {
                          return callback->getName() == callbackName;
                      }),
        validationCallbacks.end()
    );
}

void ConfigManager::addUpdateCallback(std::shared_ptr<ConfigUpdateCallback> callback) {
    if (callback) {
        updateCallbacks.push_back(callback);
    }
}

void ConfigManager::removeUpdateCallback(const std::string& callbackName) {
    updateCallbacks.erase(
        std::remove_if(updateCallbacks.begin(), updateCallbacks.end(),
                      [&callbackName](const std::shared_ptr<ConfigUpdateCallback>& callback) {
                          return callback->getName() == callbackName;
                      }),
        updateCallbacks.end()
    );
}

void ConfigManager::addErrorCallback(std::shared_ptr<ConfigErrorCallback> callback) {
    if (callback) {
        errorCallbacks.push_back(callback);
    }
}

void ConfigManager::removeErrorCallback(const std::string& callbackName) {
    errorCallbacks.erase(
        std::remove_if(errorCallbacks.begin(), errorCallbacks.end(),
                      [&callbackName](const std::shared_ptr<ConfigErrorCallback>& callback) {
                          return callback->getName() == callbackName;
                      }),
        errorCallbacks.end()
    );
}

bool ConfigManager::validateConfiguration() const {
    // This would validate the configuration against the schema
    // For now, return true
    return true;
}

void ConfigManager::notifyChange(const ConfigChangeEvent& event) {
    for (const auto& listener : changeListeners) {
        listener->onConfigChanged(event);
    }
}

void ConfigManager::logAudit(const ConfigChangeEvent& event) const {
    // Log audit trail to file
    std::ofstream auditFile(managerConfig.auditLogFile, std::ios::app);
    if (auditFile.is_open()) {
        auditFile << "[" << std::chrono::system_clock::now() << "] "
                  << event.toString() << "\n";
    }
}

void ConfigManager::addToHistory(const ConfigChangeEvent& event) {
    if (managerConfig.enableChangeTracking) {
        history.push_back(event);
        
        // Maintain history size limit
        while (history.size() > managerConfig.maxHistorySize) {
            history.erase(history.begin());
        }
    }
}

void ConfigManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Clear underlying config
    
    // Clear cache
    cache.clear();
    
    // Clear history
    history.clear();
}

bool ConfigManager::exportToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        nlohmann::json json;
        
        // Export configuration to JSON
        for (const auto& entry : getKeys()) {
            std::optional<ConfigValue> valueOpt = get<ConfigValue>(entry);
            if (valueOpt) {
                const ConfigValue& value = *valueOpt;
                
                if (const int64_t* intVal = std::get_if<int64_t>(&value)) {
                    json[entry] = *intVal;
                } else if (const double* doubleVal = std::get_if<double>(&value)) {
                    json[entry] = *doubleVal;
                } else if (const bool* boolVal = std::get_if<bool>(&value)) {
                    json[entry] = *boolVal;
                } else if (const std::string* stringVal = std::get_if<std::string>(&value)) {
                    json[entry] = *stringVal;
                }
            }
        }
        
        file << json.dump(2);
    } else if (format == "ini") {
        // Export as INI format
        for (const auto& entry : getKeys()) {
            std::optional<ConfigValue> valueOpt = get<ConfigValue>(entry);
            if (valueOpt) {
                const ConfigValue& value = *valueOpt;
                
                file << entry << " = ";
                
                if (const int64_t* intVal = std::get_if<int64_t>(&value)) {
                    file << *intVal;
                } else if (const double* doubleVal = std::get_if<double>(&value)) {
                    file << *doubleVal;
                } else if (const bool* boolVal = std::get_if<bool>(&value)) {
                    file << (*boolVal ? "true" : "false");
                } else if (const std::string* stringVal = std::get_if<std::string>(&value)) {
                    file << "\"" << *stringVal << "\"";
                }
                
                file << "\n";
            }
        }
    }
    
    return true;
}

bool ConfigManager::importFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        try {
            nlohmann::json json = nlohmann::json::parse(file);
            
            // Import configuration from JSON
            for (const auto& item : json.items()) {
                const std::string& key = item.key();
                const nlohmann::json& value = item.value();
                
                if (value.is_number_integer()) {
                    set(key, value.get<int64_t>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            return false;
        }
    } else if (format == "ini") {
        // Import from INI format
        // This would require a proper INI parser
        return false;
    }
    
    return true;
}
bool ConfigManager::addTemplate(std::unique_ptr<ConfigTemplate> template) {
    if (!template) {
        return false;
    }
    
    templates.push_back(std::move(template));
    return true;
}

bool ConfigManager::addProfile(std::unique_ptr<ConfigProfile> profile) {
    if (!profile) {
        return false;
    }
    
    profiles.push_back(std::move(profile));
    return true;
}

bool ConfigManager::applyProfile(const std::string& profileName) {
    for (const auto& profile : profiles) {
        if (profile->getInfo().name == profileName) {
            profile->applyTo(Config::getInstance());
            return true;
        }
    }
    return false;
}

bool ConfigManager::applyTemplate(const std::string& templateName) {
    for (const auto& template : templates) {
        if (template->getInfo().name == templateName) {
            template->applyTo(Config::getInstance());
            return true;
        }
    }
    return false;
}

// ConfigWatcher implementation
ConfigWatcher::ConfigWatcher(std::shared_ptr<ConfigManager> manager)
    : manager(manager), running(false) {}

ConfigWatcher::~ConfigWatcher() {
    stop();
}

bool ConfigWatcher::start() {
    if (running) {
        return false; // Already running
    }
    
    running = true;
    watchThread = std::thread(&ConfigWatcher::watchLoop, this);
    return true;
}

void ConfigWatcher::stop() {
    if (!running) {
        return;
    }
    
    running = false;
    if (watchThread.joinable()) {
        watchThread.join();
    }
}

void ConfigWatcher::watchLoop() {
    while (running) {
        std::this_thread::sleep_for(watchInterval);
        
        // Check for configuration changes
        // This is a simplified implementation
        // In a real implementation, you would monitor the actual configuration
    }
}

// ConfigHistory implementation
ConfigHistory::ConfigHistory(size_t maxSize) : maxSize(maxSize) {}

void ConfigHistory::addEvent(const ConfigChangeEvent& event) {
    std::lock_guard<std::mutex> lock(mutex);
    
    events.push_back(event);
    
    // Maintain history size limit
    while (events.size() > maxSize) {
        events.erase(events.begin());
    }
}

std::vector<ConfigChangeEvent> ConfigHistory::getRecentEvents(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    size_t startPos = (count > events.size()) ? 0 : events.size() - count;
    return std::vector<ConfigChangeEvent>(events.begin() + startPos, events.end());
}

std::vector<ConfigChangeEvent> ConfigHistory::getEventsByType(ConfigChangeEvent::Type type) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<ConfigChangeEvent> filteredEvents;
    for (const auto& event : events) {
        if (event.getType() == type) {
            filteredEvents.push_back(event);
        }
    }
    
    return filteredEvents;
}

std::vector<ConfigChangeEvent> ConfigHistory::getEventsByKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<ConfigChangeEvent> filteredEvents;
    for (const auto& event : events) {
        if (event.getKey() == key) {
            filteredEvents.push_back(event);
        }
    }
    
    return filteredEvents;
}

// ConfigOptimizer implementation
void ConfigOptimizer::optimizeLoading(ConfigManager& manager) {
    // Optimize configuration loading by preloading frequently accessed values
    // This is a simplified implementation
}

void ConfigOptimizer::optimizeAccess(ConfigManager& manager) {
    // Optimize configuration access by caching values
    // This is a simplified implementation
}

std::string ConfigOptimizer::compress(const Config& config) {
    // Compress configuration
    // This is a simplified implementation
    return "";
}

bool ConfigOptimizer::decompress(Config& config, const std::string& compressed) {
    // Decompress configuration
    // This is a simplified implementation
    return false;
}

std::vector<std::string> ConfigOptimizer::suggestOptimizations(const ConfigManager& manager) {
    // Suggest configuration optimizations
    // This is a simplified implementation
    return {};
}

} // namespace nlm