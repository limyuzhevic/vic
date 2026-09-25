#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <thread>
#include <atomic>
#include <shared_mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaField> schema;
    std::map<std::string, ConfigTemplate> templates;
    std::vector<ConfigChangeEvent> changeHistory;
    std::vector<std::function<void(const ConfigChangeEvent&)>> changeListeners;
    std::chrono::milliseconds watchInterval{1000};
    std::atomic<bool> watching{false};
    std::thread watchThread;
    std::string watchFilepath;
    
    mutable std::shared_mutex mutex;
    
    ConfigStatistics stats;
    std::chrono::system_clock::time_point lastWatchCheck;
    
    Impl() : changeHistory(), changeListeners(), watching(false), 
             watchInterval(1000), lastWatchCheck(std::chrono::system_clock::now()) {
        // Initialize statistics
        stats.lastAccessTime = std::chrono::system_clock::now().time_since_epoch().count();
        stats.lastModifiedTime = stats.lastAccessTime;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() {
    stopWatching();
}

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validateSchema) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    // Backup current configuration
    std::vector<ConfigEntry> oldEntries = pImpl->entries;
    
    clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("Failed to open config file: {}", filepath);
        return false;
    }
    
    try {
        json configJson;
        file >> configJson;
        
        // Parse entries from JSON
        if (configJson.contains("entries")) {
            for (const auto& entryJson : configJson["entries"]) {
                ConfigEntry entry = jsonToEntry(entryJson);
                pImpl->entries.push_back(entry);
            }
        }
        
        // Parse schema if present
        if (validateSchema && configJson.contains("schema")) {
            pImpl->schema.clear();
            for (const auto& fieldJson : configJson["schema"]) {
                ConfigSchemaField field;
                field.name = fieldJson.value("name", "");
                field.type = fieldJson.value("type", "");
                field.required = fieldJson.value("required", false);
                field.description = fieldJson.value("description", "");
                field.category = fieldJson.value("category", "");
                
                if (fieldJson.contains("allowedValues")) {
                    for (const auto& val : fieldJson["allowedValues"]) {
                        field.allowedValues.push_back(jsonToConfigValue(val));
                    }
                }
                
                if (fieldJson.contains("minValue")) {
                    field.minValue = fieldJson["minValue"];
                }
                if (fieldJson.contains("maxValue")) {
                    field.maxValue = fieldJson["maxValue"];
                }
                
                pImpl->schema.push_back(field);
            }
        }
        
        // Parse templates if present
        if (configJson.contains("templates")) {
            pImpl->templates.clear();
            for (const auto& [name, templateJson] : configJson["templates"].items()) {
                ConfigTemplate template;
                template.name = name;
                template.description = templateJson.value("description", "");
                template.category = templateJson.value("category", "");
                
                if (templateJson.contains("values")) {
                    for (const auto& [key, valueJson] : templateJson["values"].items()) {
                        template.values[key] = jsonToConfigValue(valueJson);
                    }
                }
                
                pImpl->templates[name] = template;
            }
        }
        
        // Update statistics
        auto now = std::chrono::system_clock::now();
        pImpl->stats.lastModifiedTime = now.time_since_epoch().count();
        pImpl->stats.totalEntries = pImpl->entries.size();
        
    } catch (const std::exception& e) {
        spdlog::error("Error parsing config file {}: {}", filepath, e.what());
        // Restore old configuration
        pImpl->entries = oldEntries;
        return false;
    }
    
    // Trigger change event
    ConfigChangeEvent event;
    event.key = "__config_load__";
    event.oldValue = ConfigValue();
    event.newValue = ConfigValue();
    event.source = ConfigSource::File;
    event.timestamp = std::chrono::system_clock::now();
    
    for (auto& listener : pImpl->changeListeners) {
        listener(event);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validateSchema) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
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
    
    if (validateSchema) {
        validateSchema();
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool includeDefaults) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("Failed to open config file for writing: {}", filepath);
        return false;
    }
    
    json configJson;
    
    // Convert entries to JSON
    json entriesJson = json::array();
    for (const auto& entry : pImpl->entries) {
        entriesJson.push_back(entryToJson(entry));
    }
    configJson["entries"] = entriesJson;
    
    // Convert schema to JSON
    if (!pImpl->schema.empty()) {
        json schemaJson = json::array();
        for (const auto& field : pImpl->schema) {
            json fieldJson;
            fieldJson["name"] = field.name;
            fieldJson["type"] = field.type;
            fieldJson["required"] = field.required;
            fieldJson["description"] = field.description;
            fieldJson["category"] = field.category;
            
            if (!field.allowedValues.empty()) {
                json allowedJson = json::array();
                for (const auto& val : field.allowedValues) {
                    allowedJson.push_back(configValueToJson(val));
                }
                fieldJson["allowedValues"] = allowedJson;
            }
            
            if (field.minValue != std::numeric_limits<double>::lowest()) {
                fieldJson["minValue"] = field.minValue;
            }
            if (field.maxValue != std::numeric_limits<double>::max()) {
                fieldJson["maxValue"] = field.maxValue;
            }
            
            schemaJson.push_back(fieldJson);
        }
        configJson["schema"] = schemaJson;
    }
    
    // Convert templates to JSON
    if (!pImpl->templates.empty()) {
        json templatesJson = json::object();
        for (const auto& [name, template] : pImpl->templates) {
            json templateJson;
            templateJson["description"] = template.description;
            templateJson["category"] = template.category;
            
            if (!template.values.empty()) {
                json valuesJson = json::object();
                for (const auto& [key, value] : template.values) {
                    valuesJson[key] = configValueToJson(value);
                }
                templateJson["values"] = valuesJson;
            }
            
            templatesJson[name] = templateJson;
        }
        configJson["templates"] = templatesJson;
    }
    
    try {
        file << configJson.dump(2);
    } catch (const std::exception& e) {
        spdlog::error("Error writing config file {}: {}", filepath, e.what());
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        auto value = std::get<T>(it->value);
        return value;
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
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    ConfigSource oldSource = ConfigSource::Default;
    bool changed = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        oldSource = it->source;
        
        if (oldValue != value) {
            it->value = value;
            it->source = source;
            it->lastModified = std::chrono::system_clock::now();
            changed = true;
        }
    } else {
        oldValue = ConfigValue();
        pImpl->entries.emplace_back(key, value, source);
        changed = true;
    }
    
    if (changed) {
        auto now = std::chrono::system_clock::now();
        
        // Update statistics
        pImpl->stats.lastAccessTime = now.time_since_epoch().count();
        pImpl->stats.totalEntries = pImpl->entries.size();
        
        // Track source counts
        pImpl->stats.totalEntries = pImpl->entries.size();
        int runtime = 0, file = 0, cmd = 0, def = 0, tpl = 0;
        for (const auto& entry : pImpl->entries) {
            switch (entry.source) {
                case ConfigSource::Runtime: runtime++; break;
                case ConfigSource::File: file++; break;
                case ConfigSource::CommandLine: cmd++; break;
                case ConfigSource::Default: def++; break;
                case ConfigSource::Template: tpl++; break;
            }
        }
        pImpl->stats.runtimeEntries = runtime;
        pImpl->stats.fileEntries = file;
        pImpl->stats.commandLineEntries = cmd;
        pImpl->stats.defaultEntries = def;
        
        // Create change event
        ConfigChangeEvent event;
        event.key = key;
        event.oldValue = oldValue;
        event.newValue = value;
        event.source = source;
        event.timestamp = now;
        
        pImpl->changeHistory.push_back(event);
        
        // Notify listeners
        for (auto& listener : pImpl->changeListeners) {
            listener(event);
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

bool Config::validateSchema() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<ConfigValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                    [&entry](const ConfigSchemaField& f) { return f.name == entry.key; });
        
        if (schemaIt != pImpl->schema.end()) {
            if (!validateValue(entry.value, *schemaIt)) {
                errors.emplace_back(entry.key, "Value does not match schema type or constraints", 
                                   "SCHEMA_TYPE_MISMATCH", true);
            }
        }
    }
    
    // Check for required fields
    for (const auto& field : pImpl->schema) {
        if (field.required) {
            bool hasField = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                                       [&field](const ConfigEntry& e) { return e.key == field.name; });
            if (!hasField) {
                errors.emplace_back(field.name, "Required field is missing", "MISSING_REQUIRED_FIELD", true);
            }
        }
    }
    
    // Update statistics
    size_t totalValid = 0;
    if (!pImpl->schema.empty()) {
        totalValid = pImpl->entries.size();
    }
    
    pImpl->stats.validationPassRate = (pImpl->schema.empty() || totalValid == pImpl->entries.size()) ? 1.0 : 0.0;
    
    // This is a placeholder - errors should be stored for later retrieval
    return errors.empty();
}

std::vector<ConfigValidationError> Config::getErrors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<ConfigValidationError> errors;
    
    // This is a simplified implementation - in practice you'd store errors from validateSchema()
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.value, ConfigSchemaField())) {
            errors.emplace_back(entry.key, "Value validation failed", "VALUE_INVALID", false);
        }
    }
    
    return errors;
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
                                [&key](const ConfigSchemaField& f) { return f.name == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        return true; // No schema constraint, validation passes
    }
    
    return validateValue(value, *schemaIt);
}

std::vector<ConfigDiff> Config::getDiff(const Config& other) const {
    std::vector<ConfigDiff> diffs;
    
    // Get combined set of all keys
    std::set<std::string> allKeys;
    for (const auto& entry : pImpl->entries) {
        allKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        allKeys.insert(entry.key);
    }
    
    for (const auto& key : allKeys) {
        ConfigSource source1 = ConfigSource::Default;
        ConfigSource source2 = ConfigSource::Default;
        ConfigValue value1;
        ConfigValue value2;
        
        // Find values in this config
        auto it1 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it1 != pImpl->entries.end()) {
            value1 = it1->value;
            source1 = it1->source;
        }
        
        // Find values in other config
        auto it2 = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it2 != other.pImpl->entries.end()) {
            value2 = it2->value;
            source2 = it2->source;
        }
        
        if (it1 == pImpl->entries.end() && it2 == other.pImpl->entries.end()) {
            continue; // Both don't have the key
        } else if (it1 == pImpl->entries.end()) {
            diffs.emplace_back(key, ConfigValue(), value2, ConfigSource::Default, source2, "added");
        } else if (it2 == other.pImpl->entries.end()) {
            diffs.emplace_back(key, value1, ConfigValue(), source1, ConfigSource::Default, "removed");
        } else if (value1 != value2 || source1 != source2) {
            diffs.emplace_back(key, value1, value2, source1, source2, "modified");
        }
    }
    
    return diffs;
}

std::vector<ConfigDiff> Config::getSimilar(const Config& other, double threshold) const {
    std::vector<ConfigDiff> diffs;
    
    // Calculate similarity based on value equality
    double similarCount = 0;
    double totalCount = 0;
    
    std::set<std::string> allKeys;
    for (const auto& entry : pImpl->entries) {
        allKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        allKeys.insert(entry.key);
    }
    
    for (const auto& key : allKeys) {
        totalCount++;
        
        ConfigValue value1, value2;
        bool has1 = false, has2 = false;
        
        auto it1 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it1 != pImpl->entries.end()) {
            value1 = it1->value;
            has1 = true;
        }
        
        auto it2 = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it2 != other.pImpl->entries.end()) {
            value2 = it2->value;
            has2 = true;
        }
        
        if (has1 && has2 && value1 == value2) {
            similarCount++;
        }
    }
    
    double similarity = totalCount > 0 ? (similarCount / totalCount) : 0.0;
    
    if (similarity >= threshold) {
        // Return a single diff showing they are similar
        ConfigDiff diff;
        diff.key = "__similarity__";
        diff.oldValue = ConfigValue(similarity);
        diff.newValue = ConfigValue();
        diff.operation = "similar";
        diffs.push_back(diff);
    }
    
    return diffs;
}

bool Config::exportToFile(const std::string& filepath) const {
    return saveToFile(filepath, false);
}

bool Config::importFromFile(const std::string& filepath, bool merge) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    if (!loadFromFile(filepath, true)) {
        return false;
    }
    
    if (!merge) {
        // Remove entries not in file
        std::vector<std::string> keys = getKeys();
        for (const auto& key : keys) {
            bool existsInFile = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                                          [&key](const ConfigEntry& e) { return e.key == key; });
            if (!existsInFile) {
                remove(key);
            }
        }
    }
    
    return true;
}

ConfigStatistics Config::getStatistics() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    ConfigStatistics stats = pImpl->stats;
    
    // Recalculate counts
    size_t runtime = 0, file = 0, cmd = 0, def = 0, tpl = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Runtime: runtime++; break;
            case ConfigSource::File: file++; break;
            case ConfigSource::CommandLine: cmd++; break;
            case ConfigSource::Default: def++; break;
            case ConfigSource::Template: tpl++; break;
        }
    }
    
    stats.runtimeEntries = runtime;
    stats.fileEntries = file;
    stats.commandLineEntries = cmd;
    stats.defaultEntries = def;
    
    return stats;
}

std::map<std::string, double> Config::getMetrics() const {
    std::map<std::string, double> metrics;
    
    ConfigStatistics stats = getStatistics();
    
    metrics["total_entries"] = stats.totalEntries;
    metrics["runtime_percentage"] = stats.totalEntries > 0 ? (stats.runtimeEntries / stats.totalEntries) : 0.0;
    metrics["file_percentage"] = stats.totalEntries > 0 ? (stats.fileEntries / stats.totalEntries) : 0.0;
    metrics["validation_pass_rate"] = stats.validationPassRate;
    metrics["change_history_size"] = pImpl->changeHistory.size();
    
    return metrics;
}

bool Config::reloadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, true);
}

void Config::watchForChanges(std::function<void(const std::vector<ConfigDiff>&)> callback, 
                             std::chrono::milliseconds interval) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    stopWatching();
    
    pImpl->watchFilepath = filepath;
    pImpl->watchInterval = interval;
    pImpl->watching = true;
    pImpl->lastWatchCheck = std::chrono::system_clock::now();
    
    pImpl->watchThread = std::thread([this, callback]() {
        while (pImpl->watching) {
            std::this_thread::sleep_for(pImpl->watchInterval);
            
            std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
            auto now = std::chrono::system_clock::now();
            
            if (pImpl->lastWatchCheck.time_since_epoch().count() == 0 ||
                std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastWatchCheck).count() >= 
                pImpl->watchInterval.count()) {
                
                // Check file modification time
                if (std::filesystem::exists(pImpl->watchFilepath)) {
                    auto fileTime = std::filesystem::last_write_time(pImpl->watchFilepath);
                    auto fileTimeSinceEpoch = std::chrono::time_point_cast<std::chrono::milliseconds>(fileTime).time_since_epoch();
                    
                    if (fileTimeSinceEpoch > pImpl->lastWatchCheck.time_since_epoch()) {
                        Config currentConfig;
                        if (currentConfig.loadFromFile(pImpl->watchFilepath, true)) {
                            std::vector<ConfigDiff> diffs = getDiff(currentConfig);
                            if (!diffs.empty()) {
                                callback(diffs);
                            }
                        }
                    }
                }
                
                pImpl->lastWatchCheck = now;
            }
        }
    });
}

void Config::stopWatching() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->watching = false;
    if (pImpl->watchThread.joinable()) {
        pImpl->watchThread.join();
    }
}

void Config::setupMemoryConfig(double workingMemoryCapacity, 
                               double episodicConsolidationRate,
                               double associativeStrengthDecay,
                               double episodicDecayRate) {
    set("memory.workingMemoryCapacity", workingMemoryCapacity);
    set("memory.episodicConsolidationRate", episodicConsolidationRate);
    set("memory.associativeStrengthDecay", associativeStrengthDecay);
    set("memory.episodicDecayRate", episodicDecayRate);
}

void Config::setupNeuromodulationConfig(double dopamineBaseline,
                                       double dopamineNoise,
                                       double noveltyThreshold,
                                       double curiosityLevel,
                                       double explorationRate) {
    set("neuromodulation.dopamineBaseline", dopamineBaseline);
    set("neuromodulation.dopamineNoise", dopamineNoise);
    set("neuromodulation.noveltyThreshold", noveltyThreshold);
    set("neuromodulation.curiosityLevel", curiosityLevel);
    set("neuromodulation.explorationRate", explorationRate);
}

void Config::setupCognitiveConfig(double planningDepth,
                                 double conceptFormationThreshold,
                                 double attentionDecayRate,
                                 double workingMemoryDecayRate,
                                 double activationThreshold) {
    set("cognitive.planningDepth", planningDepth);
    set("cognitive.conceptFormationThreshold", conceptFormationThreshold);
    set("cognitive.attentionDecayRate", attentionDecayRate);
    set("cognitive.workingMemoryDecayRate", workingMemoryDecayRate);
    set("cognitive.activationThreshold", activationThreshold);
}

void Config::setupPredictionConfig(double confidenceThreshold,
                                  double learningRate,
                                  double predictionDecayRate,
                                  double predictionNoise) {
    set("prediction.confidenceThreshold", confidenceThreshold);
    set("prediction.learningRate", learningRate);
    set("prediction.decayRate", predictionDecayRate);
    set("prediction.noise", predictionNoise);
}

void Config::setupDevelopmentConfig(double plasticityRate,
                                   double developmentalStage,
                                   double explorationPhaseDuration,
                                   double learningPhaseDuration,
                                   bool enablePhaseTransitions) {
    set("development.plasticityRate", plasticityRate);
    set("development.stage", developmentalStage);
    set("development.explorationPhaseDuration", explorationPhaseDuration);
    set("development.learningPhaseDuration", learningPhaseDuration);
    set("development.enablePhaseTransitions", enablePhaseTransitions);
}

void Config::setupIntegrationConfig(double corticalCorticalCoupling,
                                   double corticalSubcorticalCoupling,
                                   double feedbackStrength,
                                   double predictionErrorGain,
                                   double learningRate) {
    set("integration.corticalCorticalCoupling", corticalCorticalCoupling);
    set("integration.corticalSubcorticalCoupling", corticalSubcorticalCoupling);
    set("integration.feedbackStrength", feedbackStrength);
    set("integration.predictionErrorGain", predictionErrorGain);
    set("integration.learningRate", learningRate);
}

bool Config::applyTemplate(const ConfigTemplate& template) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    bool applied = false;
    for (const auto& [key, value] : template.values) {
        set(key, value, ConfigSource::Template);
        applied = true;
    }
    
    return applied;
}

bool Config::applyTemplate(const std::string& templateName) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    return applyTemplate(it->second);
}

bool Config::registerTemplate(const ConfigTemplate& template) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    pImpl->templates[template.name] = template;
    return true;
}

bool Config::unregisterTemplate(const std::string& name) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        pImpl->templates.erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::string> Config::getAvailableTemplates() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<std::string> names;
    for (const auto& [name, template] : pImpl->templates) {
        names.push_back(name);
    }
    
    return names;
}

ConfigTemplate Config::getTemplate(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    
    return ConfigTemplate();
}

ConfigTemplate Config::createDevelopmentTemplate() {
    ConfigTemplate template;
    template.name = "development";
    template.description = "Fast development configuration with moderate settings";
    template.category = "development";
    
    template.values["debug.enabled"] = true;
    template.values["performance.optimizationLevel"] = 1;
    template.values["neuromodulation.dopamineBaseline"] = 0.5;
    template.values["neuromodulation.explorationRate"] = 0.3;
    template.values["cognitive.planningDepth"] = 2;
    template.values["prediction.learningRate"] = 0.05;
    template.values["development.plasticityRate"] = 0.5;
    template.values["development.stage"] = 0.5;
    template.values["integration.feedbackStrength"] = 0.7;
    
    return template;
}

ConfigTemplate Config::createResearchTemplate() {
    ConfigTemplate template;
    template.name = "research";
    template.description = "Research configuration with high variability for exploration";
    template.category = "research";
    
    template.values["debug.enabled"] = true;
    template.values["performance.optimizationLevel"] = 0;
    template.values["neuromodulation.dopamineBaseline"] = 0.5;
    template.values["neuromodulation.dopamineNoise"] = 0.3;
    template.values["neuromodulation.noveltyThreshold"] = 0.8;
    template.values["neuromodulation.curiosityLevel"] = 1.0;
    template.values["neuromodulation.explorationRate"] = 0.5;
    template.values["cognitive.planningDepth"] = 5;
    template.values["cognitive.conceptFormationThreshold"] = 0.7;
    template.values["prediction.learningRate"] = 0.1;
    template.values["prediction.noise"] = 0.2;
    template.values["development.plasticityRate"] = 0.8;
    template.values["development.stage"] = 0.8;
    template.values["integration.feedbackStrength"] = 0.9;
    
    return template;
}

ConfigTemplate Config::createPerformanceTemplate() {
    ConfigTemplate template;
    template.name = "performance";
    template.description = "High-performance configuration with optimized settings";
    template.category = "performance";
    
    template.values["debug.enabled"] = false;
    template.values["performance.optimizationLevel"] = 3;
    template.values["memory.workingMemoryCapacity"] = 200.0;
    template.values["memory.associativeStrengthDecay"] = 0.9;
    template.values["memory.episodicDecayRate"] = 0.95;
    template.values["neuromodulation.dopamineBaseline"] = 0.7;
    template.values["neuromodulation.dopamineNoise"] = 0.05;
    template.values["neuromodulation.noveltyThreshold"] = 0.6;
    template.values["cognitive.planningDepth"] = 1;
    template.values["cognitive.attentionDecayRate"] = 0.99;
    template.values["prediction.learningRate"] = 0.02;
    template.values["prediction.decayRate"] = 0.99;
    template.values["development.plasticityRate"] = 0.3;
    template.values["integration.corticalCorticalCoupling"] = 0.9;
    
    return template;
}

ConfigTemplate Config::createRoboticsTemplate() {
    ConfigTemplate template;
    template.name = "robotics";
    template.description = "Robotics application configuration with real-time constraints";
    template.category = "application";
    
    template.values["debug.enabled"] = false;
    template.values["performance.optimizationLevel"] = 2;
    template.values["performance.realtimeConstraint"] = true;
    template.values["performance.maxLatency"] = 100.0;
    template.values["memory.workingMemoryCapacity"] = 150.0;
    template.values["neuromodulation.dopamineBaseline"] = 0.6;
    template.values["neuromodulation.dopamineNoise"] = 0.1;
    template.values["neuromodulation.noveltyThreshold"] = 0.5;
    template.values["neuromodulation.explorationRate"] = 0.4;
    template.values["cognitive.planningDepth"] = 3;
    template.values["prediction.learningRate"] = 0.03;
    template.values["prediction.confidenceThreshold"] = 0.9;
    template.values["development.stage"] = 1.0;
    
    return template;
}

ConfigTemplate Config::createExplorationTemplate() {
    ConfigTemplate template;
    template.name = "exploration";
    template.description = "Exploration-focused configuration with high curiosity";
    template.category = "application";
    
    template.values["debug.enabled"] = true;
    template.values["performance.optimizationLevel"] = 1;
    template.values["neuromodulation.dopamineBaseline"] = 0.5;
    template.values["neuromodulation.dopamineNoise"] = 0.2;
    template.values["neuromodulation.noveltyThreshold"] = 0.9;
    template.values["neuromodulation.curiosityLevel"] = 1.0;
    template.values["neuromodulation.explorationRate"] = 0.7;
    template.values["cognitive.planningDepth"] = 4;
    template.values["cognitive.conceptFormationThreshold"] = 0.8;
    template.values["prediction.learningRate"] = 0.15;
    template.values["prediction.noise"] = 0.3;
    template.values["development.plasticityRate"] = 0.9;
    template.values["development.stage"] = 0.7;
    
    return template;
}

ConfigTemplate Config::createDebugTemplate() {
    ConfigTemplate template;
    template.name = "debug";
    template.description = "Debug configuration with extensive logging and slow operation";
    template.category = "debug";
    
    template.values["debug.enabled"] = true;
    template.values["debug.level"] = "verbose";
    template.values["performance.optimizationLevel"] = 0;
    template.values["logging.level"] = "debug";
    template.values["logging.toFile"] = true;
    template.values["logging.filePath"] = "nlm_debug.log";
    template.values["neuromodulation.dopamineNoise"] = 0.5;
    template.values["neuromodulation.explorationRate"] = 0.2;
    template.values["prediction.learningRate"] = 0.01;
    template.values["prediction.noise"] = 0.5;
    template.values["development.plasticityRate"] = 0.2;
    template.values["development.stage"] = 0.3;
    
    return template;
}

bool Config::unregisterTemplate(std::function<void(const ConfigChangeEvent&)> listener) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find(pImpl->changeListeners.begin(), pImpl->changeListeners.end(), listener);
    if (it != pImpl->changeListeners.end()) {
        pImpl->changeListeners.erase(it);
        return true;
    }
    
    return false;
}

void Config::clearChangeListeners() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->changeListeners.clear();
}

bool Config::has(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue value = it->value;
        ConfigSource source = it->source;
        
        pImpl->entries.erase(it);
        
        // Create change event for removal
        ConfigChangeEvent event;
        event.key = key;
        event.oldValue = value;
        event.newValue = ConfigValue();
        event.source = source;
        event.timestamp = std::chrono::system_clock::now();
        
        for (auto& listener : pImpl->changeListeners) {
            listener(event);
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
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

std::string Config::summaryByCategory(const std::string& category) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries in category '" << category << "'):\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "]\n";
        }
    }
    
    return oss.str();
}

void Config::setSchema(const std::vector<ConfigSchemaField>& schema) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->schema = schema;
}

std::vector<ConfigSchemaField> Config::getSchema() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->schema;
}

bool Config::isValidSchema() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return !pImpl->schema.empty();
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

ConfigValue Config::parseValue(const std::string& str) {
    // Try different types in order of specificity
    if (str == "true") return true;
    if (str == "false") return false;
    
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) return intVal;
        
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) return int64Val;
        
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) return doubleVal;
    } catch (const std::exception&) {}
    
    // Try vector parsing (e.g., "[1,2,3]" or "[a,b,c]")
    if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<ConfigValue> values;
        
        size_t pos = 0;
        while (pos < inner.size()) {
            size_t commaPos = inner.find(',', pos);
            if (commaPos == std::string::npos) {
                commaPos = inner.size();
            }
            
            std::string token = inner.substr(pos, commaPos - pos);
            // Remove quotes
            token = trim(token);
            if ((token.front() == '\"' && token.back() == '\"') ||
                (token.front() == '\'' && token.back() == '\'')) {
                token = token.substr(1, token.size() - 2);
            }
            
            values.push_back(parseValue(token));
            pos = commaPos + 1;
            while (pos < inner.size() && (inner[pos] == ' ' || inner[pos] == '\t')) {
                pos++;
            }
        }
        
        // Determine vector type
        if (!values.empty()) {
            if (std::holds_alternative<int>(values[0])) {
                std::vector<int> intVec;
                for (const auto& val : values) {
                    if (std::holds_alternative<int>(val)) {
                        intVec.push_back(std::get<int>(val));
                    }
                }
                return intVec;
            } else if (std::holds_alternative<double>(values[0])) {
                std::vector<double> doubleVec;
                for (const auto& val : values) {
                    if (std::holds_alternative<double>(val)) {
                        doubleVec.push_back(std::get<double>(val));
                    }
                }
                return doubleVec;
            } else if (std::holds_alternative<std::string>(values[0])) {
                std::vector<std::string> stringVec;
                for (const auto& val : values) {
                    if (std::holds_alternative<std::string>(val)) {
                        stringVec.push_back(std::get<std::string>(val));
                    }
                }
                return stringVec;
            }
        }
    }
    
    // Default to string
    return str;
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
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

json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(arg);
        } else {
            return json();
        }
    }, value);
}

ConfigValue Config::jsonToConfigValue(const json& j, const std::string& typeHint) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            // Determine array type based on first element
            const auto& first = j[0];
            if (first.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                return vec;
            } else if (first.is_number()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                return vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                return vec;
            }
        }
        return std::vector<int>();
    }
    
    return std::string();
}

json Config::entryToJson(const ConfigEntry& entry) const {
    json j;
    j["key"] = entry.key;
    j["value"] = configValueToJson(entry.value);
    j["source"] = static_cast<int>(entry.source);
    j["description"] = entry.description;
    j["category"] = entry.category;
    j["isReadOnly"] = entry.isReadOnly;
    j["lastModified"] = std::chrono::system_clock::to_time_t(entry.lastModified);
    
    return j;
}

ConfigEntry Config::jsonToEntry(const json& j) const {
    ConfigEntry entry;
    
    entry.key = j.value("key", "");
    entry.value = jsonToConfigValue(j.value("value", json()));
    entry.source = static_cast<ConfigSource>(j.value("source", 0));
    entry.description = j.value("description", "");
    entry.category = j.value("category", "");
    entry.isReadOnly = j.value("isReadOnly", false);
    
    if (j.contains("lastModified")) {
        auto timePoint = std::chrono::system_clock::from_time_t(j["lastModified"]);
        entry.lastModified = timePoint;
    }
    
    return entry;
}

bool Config::validateValue(const ConfigValue& value, const ConfigSchemaField& field) const {
    if (field.required && std::holds_alternative<std::monostate>(value)) {
        return false;
    }
    
    if (std::holds_alternative<std::monostate>(value)) {
        return !field.required;
    }
    
    if (!field.type.empty()) {
        std::string type = field.type;
        bool isMatch = false;
        
        if (type == "int" && (std::holds_alternative<int>(value) || 
                              std::holds_alternative<int64_t>(value))) {
            isMatch = true;
        } else if (type == "double" && (std::holds_alternative<int>(value) || 
                                         std::holds_alternative<int64_t>(value) || 
                                         std::holds_alternative<double>(value))) {
            isMatch = true;
        } else if (type == "bool" && std::holds_alternative<bool>(value)) {
            isMatch = true;
        } else if (type == "string" && std::holds_alternative<std::string>(value)) {
            isMatch = true;
        } else if (type == "int[]" && std::holds_alternative<std::vector<int>>(value)) {
            isMatch = true;
        } else if (type == "double[]" && (std::holds_alternative<std::vector<int>>(value) || 
                                           std::holds_alternative<std::vector<double>>(value))) {
            isMatch = true;
        } else if (type == "string[]" && std::holds_alternative<std::vector<std::string>>(value)) {
            isMatch = true;
        }
        
        if (!isMatch) {
            return false;
        }
    }
    
    if (!field.allowedValues.empty()) {
        bool found = false;
        for (const auto& allowed : field.allowedValues) {
            if (value == allowed) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return validateConstraints(value, field);
}

bool Config::validateConstraints(const ConfigValue& value, const ConfigSchemaField& field) const {
    if (std::holds_alternative<int>(value)) {
        double val = std::get<int>(value);
        if (val < field.minValue || val > field.maxValue) {
            return false;
        }
    } else if (std::holds_alternative<int64_t>(value)) {
        double val = std::get<int64_t>(value);
        if (val < field.minValue || val > field.maxValue) {
            return false;
        }
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < field.minValue || val > field.maxValue) {
            return false;
        }
    }
    
    return true;
}

// Helper functions to get numeric limits
double Config::getNumericMax() {
    return std::numeric_limits<double>::max();
}

double Config::getNumericMin() {
    return std::numeric_limits<double>::lowest();
},
}