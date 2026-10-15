// ConfigTemplate implementation
#include "config/ConfigTemplate.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

namespace nlm {

// ConfigTemplate implementation
ConfigTemplate::ConfigTemplate(const std::string& name, const std::string& description)
    : name(name), description(description) {}

void ConfigTemplate::setName(const std::string& name) {
    this->name = name;
}

void ConfigTemplate::setDescription(const std::string& description) {
    this->description = description;
}

void ConfigTemplate::addParameter(const std::string& name, const ConfigValue& defaultValue,
                                 const std::string& description, bool required) {
    parameters[name] = std::make_pair(defaultValue, description);
}

void ConfigTemplate::addValidationRule(const std::string& parameter,
                                      std::function<bool(const ConfigValue&, const Config&)> validator) {
    validationRules[parameter].push_back(validator);
}

void ConfigTemplate::addDependency(const std::string& parameter, const std::string& dependsOn) {
    dependencies[parameter] = dependsOn;
}

std::shared_ptr<Config> ConfigTemplate::createConfig() const {
    auto config = std::make_shared<Config>();
    
    for (const auto& [param, valueDesc] : parameters) {
        config->set(param, valueDesc.first);
    }
    
    return config;
}

bool ConfigTemplate::validate(const Config& config) const {
    ValidationResult result = validateWithDetails(config);
    return result.valid;
}

ValidationResult ConfigTemplate::validateWithDetails(const Config& config) const {
    ValidationResult result;
    
    // Check required parameters
    for (const auto& [param, valueDesc] : parameters) {
        if (!config.has(param)) {
            result.addParameterError(param, "Required parameter is missing");
        }
    }
    
    // Run validation rules
    for (const auto& [param, validators] : validationRules) {
        auto it = std::find_if(config.pImpl->entries.begin(), config.pImpl->entries.end(),
                              [&param](const ConfigEntry& e) { return e.key == param; });
        if (it != config.pImpl->entries.end()) {
            for (const auto& validator : validators) {
                if (!validator(it->value, config)) {
                    result.addParameterError(param, "Template validation failed");
                }
            }
        }
    }
    
    return result;
}

std::shared_ptr<Config> ConfigTemplate::applyTemplate(const Config& baseConfig) const {
    auto result = std::make_shared<Config>(baseConfig);
    
    // Override with template values
    for (const auto& [param, valueDesc] : parameters) {
        result->set(param, valueDesc.first);
    }
    
    return result;
}

std::vector<std::string> ConfigTemplate::getParameters() const {
    std::vector<std::string> params;
    params.reserve(parameters.size());
    for (const auto& param : parameters) {
        params.push_back(param.first);
    }
    return params;
}

std::map<std::string, ConfigValue> ConfigTemplate::getDefaultValues() const {
    std::map<std::string, ConfigValue> defaults;
    for (const auto& [param, valueDesc] : parameters) {
        defaults[param] = valueDesc.first;
    }
    return defaults;
}

bool ConfigTemplate::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        json["name"] = name;
        json["description"] = description;
        
        nlohmann::json paramsJson;
        for (const auto& [param, valueDesc] : parameters) {
            // Convert ConfigValue to JSON (simplified)
            paramsJson[param] = "ConfigValue";
        }
        
        json["parameters"] = paramsJson;
        
        file << json.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigTemplate::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        name = json.value("name", "");
        description = json.value("description", "");
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::shared_ptr<ConfigTemplate> ConfigTemplate::createPhase2Template() {
    auto template = std::make_shared<ConfigTemplate>("Phase 2 Standard", "Standard configuration for Phase 2 neural simulation");
    
    template->addParameter("neuron_count", static_cast<int64_t>(500), "Number of neurons", true);
    template->addParameter("region_count", static_cast<int64_t>(1), "Number of brain regions", true);
    template->addParameter("connection_probability", 0.15, "Connection probability for random connectivity", true);
    template->addParameter("simulation_timestep", 0.001, "Simulation timestep in seconds", true);
    template->addParameter("stdp_ltp_weight", 0.02, "STDP LTP weight", true);
    template->addParameter("stdp_ltd_weight", 0.015, "STDP LTD weight", true);
    template->addParameter("stdp_time_constant", 20.0, "STDP time constant", true);
    template->addParameter("synaptogenesis_rate", 0.0001, "Synaptogenesis rate", true);
    template->addParameter("pruning_rate", 0.00001, "Pruning rate", true);
    
    return template;
}

std::shared_ptr<ConfigTemplate> ConfigTemplate::createPhase4Template() {
    auto template = std::make_shared<ConfigTemplate>("Phase 4 Standard", "Standard configuration for Phase 4 cognitive development");
    
    template->addParameter("neuron_count", static_cast<int64_t>(1000), "Number of neurons", true);
    template->addParameter("region_count", static_cast<int64_t>(3), "Number of brain regions", true);
    template->addParameter("connection_probability", 0.1, "Connection probability for random connectivity", true);
    template->addParameter("simulation_timestep", 0.001, "Simulation timestep in seconds", true);
    template->addParameter("plasticity_learning_rate", 0.1, "Plasticity learning rate", true);
    template->addParameter("dopamine_scale", 1.0, "Dopamine scale factor", true);
    template->addParameter("curiosity_threshold", 0.5, "Curiosity threshold", true);
    template->addParameter("development_stages", static_cast<int64_t>(10), "Number of developmental stages", true);
    template->addParameter("plasticity_rate", 0.5, "Overall plasticity rate", true);
    
    return template;
}

std::shared_ptr<ConfigTemplate> ConfigTemplate::createPhase6Template() {
    auto template = std::make_shared<ConfigTemplate>("Phase 6 Standard", "Standard configuration for Phase 6 advanced learning");
    
    template->addParameter("neuron_count", static_cast<int64_t>(2000), "Number of neurons", true);
    template->addParameter("region_count", static_cast<int64_t>(5), "Number of brain regions", true);
    template->addParameter("connection_probability", 0.08, "Connection probability for random connectivity", true);
    template->addParameter("simulation_timestep", 0.0005, "Simulation timestep in seconds", true);
    template->addParameter("stdp_ltp_weight", 0.05, "STDP LTP weight", true);
    template->addParameter("stdp_ltd_weight", 0.04, "STDP LTD weight", true);
    template->addParameter("stdp_time_constant", 30.0, "STDP time constant", true);
    template->addParameter("plasticity_learning_rate", 0.2, "Plasticity learning rate", true);
    template->addParameter("dopamine_scale", 2.0, "Dopamine scale factor", true);
    template->addParameter("curiosity_threshold", 0.7, "Curiosity threshold", true);
    template->addParameter("development_stages", static_cast<int64_t>(20), "Number of developmental stages", true);
    template->addParameter("plasticity_rate", 0.8, "Overall plasticity rate", true);
    template->addParameter("working_memory_capacity", 100, "Working memory capacity", true);
    template->addParameter("episodic_memory_size", 1000, "Episodic memory size", true);
    template->addParameter("prediction_horizon", 50, "Prediction horizon", true);
    template->addParameter("prediction_error_threshold", 0.1, "Prediction error threshold", true);
    
    return template;
}

std::shared_ptr<ConfigTemplate> ConfigTemplate::createCustomTemplate(const std::string& name) {
    return std::make_shared<ConfigTemplate>(name, "Custom configuration template");
}

// ConfigHistory implementation
ConfigHistoryEntry::ConfigHistoryEntry(const std::string& aid, const std::string& atimestamp,
                                     const std::string& aaction, std::shared_ptr<Config> aconfig,
                                     const std::string& auser, const std::string& adescription)
    : id(aid), timestamp(atimestamp), action(aaction), configSnapshot(aconfig),
      user(auser), description(adescription) {}

struct ConfigHistory::Impl {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> entries;
    bool enabled = true;
    size_t maxEntries = 1000;
};

ConfigHistory::ConfigHistory() : pImpl(std::make_unique<Impl>()) {}

ConfigHistory::~ConfigHistory() = default;

std::string ConfigHistory::recordChange(const std::string& action, const std::shared_ptr<Config>& config,
                                        const std::string& user, const std::string& description) {
    if (!pImpl->enabled || !config) {
        return "";
    }
    
    // Generate unique ID
    std::string id = "entry_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
    std::string timestampStr = ss.str();
    
    // Create entry
    auto entry = std::make_shared<ConfigHistoryEntry>(id, timestampStr, action, config, user, description);
    
    // Add to history
    pImpl->entries.push_back(entry);
    
    // Trim if too many entries
    if (pImpl->entries.size() > pImpl->maxEntries) {
        pImpl->entries.erase(pImpl->entries.begin());
    }
    
    return id;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistory() const {
    return pImpl->entries;
}

std::optional<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getEntry(const std::string& id) const {
    for (const auto& entry : pImpl->entries) {
        if (entry->id == id) {
            return entry;
        }
    }
    return std::nullopt;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getRecentEntries(size_t count) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> recent;
    if (count > pImpl->entries.size()) {
        recent = pImpl->entries;
    } else {
        recent = std::vector<std::shared_ptr<ConfigHistoryEntry>>(
            pImpl->entries.end() - count, pImpl->entries.end());
    }
    return recent;
}

bool ConfigHistory::restoreFromHistory(const std::string& id, std::shared_ptr<Config>& restoredConfig) {
    auto entryOpt = getEntry(id);
    if (!entryOpt) {
        return false;
    }
    
    restoredConfig = entryOpt->configSnapshot;
    return true;
}

bool ConfigHistory::rollback(const std::string& id, std::shared_ptr<Config>& rolledBackConfig) {
    return restoreFromHistory(id, rolledBackConfig);
}

bool ConfigHistory::deleteEntry(const std::string& id) {
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ++it) {
        if ((*it)->id == id) {
            pImpl->entries.erase(it);
            return true;
        }
    }
    return false;
}

bool ConfigHistory::clearHistory() {
    pImpl->entries.clear();
    return true;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByAction(const std::string& action) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->action == action) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByUser(const std::string& user) const {
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->user == user) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigHistoryEntry>> ConfigHistory::getHistoryByTimeRange(
    const std::string& startTime, const std::string& endTime) const {
    // Simplified time range filtering (would need proper date parsing)
    std::vector<std::shared_ptr<ConfigHistoryEntry>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry->timestamp >= startTime && entry->timestamp <= endTime) {
            result.push_back(entry);
        }
    }
    return result;
}

bool ConfigHistory::exportHistory(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        json["entries"] = nlohmann::json::array();
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json entryJson;
            entryJson["id"] = entry->id;
            entryJson["timestamp"] = entry->timestamp;
            entryJson["action"] = entry->action;
            entryJson["user"] = entry->user;
            entryJson["description"] = entry->description;
            
            // Simplified config serialization
            nlohmann::json configJson;
            for (const auto& configEntry : entry->configSnapshot->pImpl->entries) {
                configJson[configEntry.key] = "ConfigValue"; // Simplified
            }
            
            entryJson["config"] = configJson;
            json["entries"].push_back(entryJson);
        }
        
        file << json.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigHistory::importHistory(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        pImpl->entries.clear();
        
        for (const auto& entryJson : json["entries"]) {
            std::string id = entryJson.value("id", "");
            std::string timestamp = entryJson.value("timestamp", "");
            std::string action = entryJson.value("action", "");
            std::string user = entryJson.value("user", "");
            std::string description = entryJson.value("description", "");
            
            // Simplified config deserialization
            auto config = std::make_shared<Config>();
            
            // Create entry
            auto entry = std::make_shared<ConfigHistoryEntry>(id, timestamp, action, config, user, description);
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nlm
