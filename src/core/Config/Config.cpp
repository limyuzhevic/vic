#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <regex>
#include <iomanip>
#include <thread>
#include <chrono>
#include <unordered_set>

// JSON and YAML support (would need external libraries in real implementation)
// For now, we'll provide stub implementations

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // For schema and metadata management
    std::unordered_map<std::string, ParameterMetadata> parameterRegistry;
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, std::string> groupToSchema;
    std::unordered_map<std::string, std::vector<std::string>> groupParameters;
    
    // For environment variable loading
    std::string envVarPrefix;
    
    // For templates
    std::vector<std::string> templateNames;
    std::unordered_map<std::string, std::vector<ConfigEntry>> templates;
    
    // For change tracking
    std::chrono::system_clock::time_point lastModifiedTime;
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> changeHistory;
    
    // For monitoring
    bool monitoringEnabled = false;
    std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)> changeCallback;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    validationEnabled_ = true;
    monitoringEnabled_ = false;
    hotSwappingEnabled_ = false;
    envVarPrefix_ = "NLM_";
    changeCallback_ = nullptr;
    lastModifiedTime_ = std::chrono::system_clock::now();
    
    // Setup default validation schemas
    setupDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setupDefaultSchemas() {
    // Neural parameters schema
    ConfigSchema neuralSchema("neural", ParameterCategory::Neural);
    neuralSchema.addParameter(ParameterMetadata("neuron_count", "Number of neurons in the brain", ParameterCategory::Neural));
    neuralSchema.addParameter(ParameterMetadata("region_count", "Number of brain regions", ParameterCategory::Neural));
    neuralSchema.addParameter(ParameterMetadata("connection_probability", "Connection probability for random connectivity", ParameterCategory::Neural));
    neuralSchema.addParameter(ParameterMetadata("sensory_neuron_count", "Number of sensory neurons", ParameterCategory::Neural));
    neuralSchema.addParameter(ParameterMetadata("motor_neuron_count", "Number of motor neurons", ParameterCategory::Neural));
    pImpl->schemas["neural"] = neuralSchema;
    pImpl->groupToSchema["neural"] = "neural";
    
    // Plasticity parameters schema
    ConfigSchema plasticitySchema("plasticity", ParameterCategory::Plasticity);
    plasticitySchema.addParameter(ParameterMetadata("plasticity_learning_rate", "Plasticity learning rate", ParameterCategory::Plasticity));
    plasticitySchema.addParameter(ParameterMetadata("stdp_ltp_weight", "STDP LTP weight", ParameterCategory::Plasticity));
    plasticitySchema.addParameter(ParameterMetadata("stdp_ltd_weight", "STDP LTD weight", ParameterCategory::Plasticity));
    plasticitySchema.addParameter(ParameterMetadata("stdp_time_constant", "STDP time constant", ParameterCategory::Plasticity));
    plasticitySchema.addParameter(ParameterMetadata("plasticity_modifier", "Plasticity modifier factor", ParameterCategory::Plasticity));
    pImpl->schemas["plasticity"] = plasticitySchema;
    pImpl->groupToSchema["plasticity"] = "plasticity";
    
    // Neuromodulation parameters schema
    ConfigSchema neuromodulationSchema("neuromodulation", ParameterCategory::Neuromodulation);
    neuromodulationSchema.addParameter(ParameterMetadata("dopamine_baseline", "Dopamine baseline level", ParameterCategory::Neuromodulation));
    neuromodulationSchema.addParameter(ParameterMetadata("curiosity_strength", "Curiosity strength", ParameterCategory::Neuromodulation));
    neuromodulationSchema.addParameter(ParameterMetadata("novelty_sensitivity", "Novelty sensitivity", ParameterCategory::Neuromodulation));
    neuromodulationSchema.addParameter(ParameterMetadata("reward_prediction_error", "Reward prediction error scale", ParameterCategory::Neuromodulation));
    pImpl->schemas["neuromodulation"] = neuromodulationSchema;
    pImpl->groupToSchema["neuromodulation"] = "neuromodulation";
    
    // Development parameters schema
    ConfigSchema developmentSchema("development", ParameterCategory::Development);
    developmentSchema.addParameter(ParameterMetadata("development_synaptogenesis_rate", "Synaptogenesis rate", ParameterCategory::Development));
    developmentSchema.addParameter(ParameterMetadata("development_pruning_rate", "Pruning rate", ParameterCategory::Development));
    developmentSchema.addParameter(ParameterMetadata("plasticity_modifier", "Plasticity modifier factor", ParameterCategory::Development));
    pImpl->schemas["development"] = developmentSchema;
    pImpl->groupToSchema["development"] = "development";
    
    // Simulation parameters schema
    ConfigSchema simulationSchema("simulation", ParameterCategory::Simulation);
    simulationSchema.addParameter(ParameterMetadata("simulation_timestep", "Simulation timestep in seconds", ParameterCategory::Simulation));
    simulationSchema.addParameter(ParameterMetadata("max_simulation_steps", "Maximum simulation steps", ParameterCategory::Simulation));
    simulationSchema.addParameter(ParameterMetadata("simulation_time_limit", "Simulation time limit", ParameterCategory::Simulation));
    pImpl->schemas["simulation"] = simulationSchema;
    pImpl->groupToSchema["simulation"] = "simulation";
}

void Config::registerParameter(const ParameterMetadata& metadata) {
    pImpl->parameterRegistry[metadata.key] = metadata;
}

void Config::registerSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.name] = schema;
    for (const auto& param : schema.parameters) {
        pImpl->parameterRegistry[param.key] = param;
    }
}

void Config::setParameterGroup(const std::string& group, const ConfigSchema& schema) {
    pImpl->groupToSchema[group] = schema.name;
    registerSchema(schema);
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& group) {
    // Check if parameter exists in registry
    auto it = pImpl->parameterRegistry.find(key);
    
    ConfigEntry entry(key, value, source);
    if (it != pImpl->parameterRegistry.end()) {
        entry.metadata = it->second;
        entry.description = it->second.description;
    }
    
    // Find existing entry
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool isNew = existingIt == pImpl->entries.end();
    
    if (isNew) {
        pImpl->entries.push_back(entry);
        oldValue = ConfigValue(); // Default empty variant
    } else {
        oldValue = existingIt->value;
        *existingIt = entry;
    }
    
    // Update tracking
    entry.timestamp = std::chrono::system_clock::now();
    entry.modified = true;
    
    if (monitoringEnabled_ && changeCallback_ && !std::holds_alternative<std::monostate>(oldValue) && oldValue != value) {
        changeCallback_(key, oldValue, value);
    }
    
    lastModifiedTime_ = std::chrono::system_clock::now();
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source, const std::string& group) {
    set(key, ConfigValue(value), source, group);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source, const std::string& group) {
    set(key, ConfigValue(value), source, group);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source, const std::string& group) {
    set(key, ConfigValue(value), source, group);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source, const std::string& group) {
    set(key, ConfigValue(value), source, group);
}

template<typename T>
std::optional<T> Config::get(const std::string& key, const std::string& group) const {
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
T Config::getOr(const std::string& key, const T& defaultValue, 
              const std::string& group) const {
    auto val = get<T>(key, group);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::has(const std::string& key, const std::string& group) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

std::optional<ParameterMetadata> Config::getMetadata(const std::string& key, 
                                                     const std::string& group) const {
    auto it = pImpl->parameterRegistry.find(key);
    if (it != pImpl->parameterRegistry.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Config::remove(const std::string& key, const std::string& group) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::string Config::getInheritedValue(const std::string& key, 
                                      const std::string& group) const {
    // For now, just return the current value
    // In a real implementation, this would check inheritance chains
    return "";
}

std::vector<std::string> Config::getGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : pImpl->groupToSchema) {
        groups.push_back(pair.first);
    }
    return groups;
}

std::vector<ParameterMetadata> Config::getParameters(const std::string& group) const {
    std::vector<ParameterMetadata> params;
    
    if (!group.empty()) {
        auto schemaIt = pImpl->groupToSchema.find(group);
        if (schemaIt != pImpl->groupToSchema.end()) {
            auto schemaIt2 = pImpl->schemas.find(schemaIt->second);
            if (schemaIt2 != pImpl->schemas.end()) {
                return schemaIt2->second.parameters;
            }
        }
    }
    
    // Return all registered parameters
    for (const auto& pair : pImpl->parameterRegistry) {
        params.push_back(pair.second);
    }
    
    return params;
}

std::vector<ParameterCategory> Config::getCategories() const {
    std::vector<ParameterCategory> categories;
    std::unordered_set<ParameterCategory> uniqueCats;
    
    for (const auto& pair : pImpl->parameterRegistry) {
        ParameterCategory cat = pair.second.category;
        if (uniqueCats.find(cat) == uniqueCats.end()) {
            uniqueCats.insert(cat);
            categories.push_back(cat);
        }
    }
    
    return categories;
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::clear(const std::string& group) {
    auto schemaIt = pImpl->groupToSchema.find(group);
    if (schemaIt != pImpl->groupToSchema.end()) {
        auto schemaIt2 = pImpl->schemas.find(schemaIt->second);
        if (schemaIt2 != pImpl->schemas.end()) {
            // Clear only parameters in this group
            for (const auto& param : schemaIt2->second.parameters) {
                remove(param.key, group);
            }
        }
    }
}

std::vector<std::string> Config::getKeys(const std::string& group) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        // If group is specified, check if entry belongs to that group
        if (group.empty()) {
            keys.push_back(entry.key);
        } else {
            auto schemaIt = pImpl->groupToSchema.find(group);
            if (schemaIt != pImpl->groupToSchema.end()) {
                auto schemaIt2 = pImpl->schemas.find(schemaIt->second);
                if (schemaIt2 != pImpl->schemas.end()) {
                    for (const auto& param : schemaIt2->second.parameters) {
                        if (param.key == entry.key) {
                            keys.push_back(entry.key);
                            break;
                        }
                    }
                }
            }
        }
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries(const std::string& group) const {
    std::vector<ConfigEntry> entries;
    
    if (group.empty()) {
        entries = pImpl->entries;
    } else {
        auto schemaIt = pImpl->groupToSchema.find(group);
        if (schemaIt != pImpl->groupToSchema.end()) {
            auto schemaIt2 = pImpl->schemas.find(schemaIt->second);
            if (schemaIt2 != pImpl->schemas.end()) {
                for (const auto& param : schemaIt2->second.parameters) {
                    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                              [&param](const ConfigEntry& e) { return e.key == param.key; });
                    if (entryIt != pImpl->entries.end()) {
                        entries.push_back(*entryIt);
                    }
                }
            }
        }
    }
    
    return entries;
}

bool Config::hasSchema(const std::string& schemaName) const {
    return pImpl->schemas.find(schemaName) != pImpl->schemas.end();
}

std::optional<ConfigSchema> Config::getSchema(const std::string& schemaName) const {
    auto it = pImpl->schemas.find(schemaName);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> Config::getSchemaNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->schemas) {
        names.push_back(pair.first);
    }
    return names;
}

bool Config::loadFromEnvironment() {
    bool loaded = false;
    
    for (const auto& pair : pImpl->parameterRegistry) {
        const std::string& key = pair.first;
        const ParameterMetadata& meta = pair.second;
        
        // Construct environment variable name
        std::string envVar = envVarPrefix_ + key;
        for (char& c : envVar) {
            c = std::toupper(c);
        }
        
        const char* envValue = std::getenv(envVar.c_str());
        if (envValue) {
            std::string valueStr(envValue);
            ConfigValue value = parseValue(valueStr, "");
            if (value.index() != std::variant_npos) {
                set(key, value, ConfigSource::Environment, "");
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::hasChangedSince(const std::chrono::system_clock::time_point& time) const {
    return lastModifiedTime_ > time;
}

std::chrono::system_clock::time_point Config::getLastModifiedTime() const {
    return lastModifiedTime_;
}

bool Config::loadFromFile(const std::string& filepath, const std::string& group) {
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
        
        // Parse key=value pairs
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
            
            ConfigValue parsedValue = parseValue(value, group);
            if (parsedValue.index() != std::variant_npos) {
                set(key, parsedValue, ConfigSource::File, group);
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath, const std::string& group) {
    // Simplified JSON parsing - would use actual JSON library in production
    // For now, fall back to key=value format
    return loadFromFile(filepath, group);
}

bool Config::loadFromYAML(const std::string& filepath, const std::string& group) {
    // Simplified YAML parsing - would use actual YAML library in production
    // For now, fall back to key=value format
    return loadFromFile(filepath, group);
}

bool Config::saveToFile(const std::string& filepath, const std::string& group) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto entries = getEntries(group);
    for (const auto& entry : entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath, const std::string& group) const {
    // Simplified JSON writing - would use actual JSON library in production
    return saveToFile(filepath, group);
}

bool Config::saveToYAML(const std::string& filepath, const std::string& group) const {
    // Simplified YAML writing - would use actual YAML library in production
    return saveToFile(filepath, group);
}

bool Config::loadTemplate(const std::string& templateName) {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    clear();
    for (const auto& entry : it->second) {
        pImpl->entries.push_back(entry);
    }
    
    return true;
}

std::vector<std::string> Config::getTemplateNames() const {
    return pImpl->templateNames;
}

bool Config::saveTemplate(const std::string& templateName) const {
    pImpl->templates[templateName] = pImpl->entries;
    pImpl->templateNames.push_back(templateName);
    return true;
}

std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> errors;
    
    if (!validationEnabled_) {
        return errors;
    }
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        std::vector<ValidationError> entryErrors = validateEntry(entry);
        errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
    }
    
    return errors;
}

std::vector<ValidationError> Config::validate(const std::string& group) const {
    std::vector<ValidationError> errors;
    
    if (!validationEnabled_) {
        return errors;
    }
    
    auto entries = getEntries(group);
    for (const auto& entry : entries) {
        std::vector<ValidationError> entryErrors = validateEntry(entry);
        errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
    }
    
    return errors;
}

bool Config::validateParameter(const std::string& key, const std::string& group) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    std::vector<ValidationError> errors = validateEntry(*it);
    return errors.empty();
}

std::vector<ValidationError> Config::validateEntry(const ConfigEntry& entry) const {
    std::vector<ValidationError> errors;
    
    // Check if parameter exists in registry
    auto metaIt = pImpl->parameterRegistry.find(entry.key);
    if (metaIt != pImpl->parameterRegistry.end()) {
        const ParameterMetadata& meta = metaIt->second;
        
        // Check if value is valid
        if (!validateValue(entry.value, meta.constraint)) {
            errors.emplace_back(entry.key, 
                                "Invalid value type or value out of range",
                                ValidationSeverity::Error);
        }
        
        // Check if required parameter is set
        if (meta.required && entry.source == ConfigSource::Default) {
            errors.emplace_back(entry.key, 
                                "Required parameter is not set",
                                ValidationSeverity::Warning);
        }
    }
    
    return errors;
}

bool Config::validateValue(const ConfigValue& value, 
                           const ParameterConstraint& constraint) const {
    // Check if value has correct type
    bool hasCorrectType = false;
    
    switch (constraint.type) {
        case ParameterType::Integer:
            hasCorrectType = std::holds_alternative<int>(value) || 
                           std::holds_alternative<int64_t>(value);
            break;
        case ParameterType::Float:
            hasCorrectType = std::holds_alternative<double>(value) ||
                           std::holds_alternative<int>(value);
            break;
        case ParameterType::Boolean:
            hasCorrectType = std::holds_alternative<bool>(value);
            break;
        case ParameterType::String:
            hasCorrectType = std::holds_alternative<std::string>(value);
            break;
        case ParameterType::IntegerArray:
            hasCorrectType = std::holds_alternative<std::vector<int>>(value);
            break;
        case ParameterType::FloatArray:
            hasCorrectType = std::holds_alternative<std::vector<double>>(value);
            break;
        case ParameterType::StringArray:
            hasCorrectType = std::holds_alternative<std::vector<std::string>>(value);
            break;
        case ParameterType::Object:
            // Object validation would be more complex
            hasCorrectType = true;
            break;
    }
    
    if (!hasCorrectType) {
        return false;
    }
    
    // Additional validation based on constraint
    bool isValid = true;
    
    // Range validation
    if (constraint.minValue || constraint.maxValue) {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (constraint.minValue && val < *constraint.minValue) isValid = false;
            if (constraint.maxValue && val > *constraint.maxValue) isValid = false;
        } else if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (constraint.minValue && static_cast<double>(val) < *constraint.minValue) isValid = false;
            if (constraint.maxValue && static_cast<double>(val) > *constraint.maxValue) isValid = false;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (constraint.minValue && static_cast<double>(val) < *constraint.minValue) isValid = false;
            if (constraint.maxValue && static_cast<double>(val) > *constraint.maxValue) isValid = false;
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            auto& arr = std::get<std::vector<int>>(value);
            if (constraint.minValue) {
                if (std::any_of(arr.begin(), arr.end(), 
                               [&constraint](int v) { return static_cast<double>(v) < *constraint.minValue; })) {
                    isValid = false;
                }
            }
            if (constraint.maxValue && isValid) {
                if (std::any_of(arr.begin(), arr.end(),
                               [&constraint](int v) { return static_cast<double>(v) > *constraint.maxValue; })) {
                    isValid = false;
                }
            }
        }
    }
    
    // Length validation
    if (constraint.minLength || constraint.maxLength) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            auto& arr = std::get<std::vector<int>>(value);
            if (constraint.minLength && arr.size() < *constraint.minLength) isValid = false;
            if (constraint.maxLength && arr.size() > *constraint.maxLength) isValid = false;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            auto& arr = std::get<std::vector<double>>(value);
            if (constraint.minLength && arr.size() < *constraint.minLength) isValid = false;
            if (constraint.maxLength && arr.size() > *constraint.maxLength) isValid = false;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            auto& arr = std::get<std::vector<std::string>>(value);
            if (constraint.minLength && arr.size() < *constraint.minLength) isValid = false;
            if (constraint.maxLength && arr.size() > *constraint.maxLength) isValid = false;
        }
    }
    
    // Allowed values validation
    if (!constraint.allowedValues.empty() && isValid) {
        bool found = false;
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            found = std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(),
                            std::to_string(val)) != constraint.allowedValues.end();
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            found = std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(),
                            std::to_string(val)) != constraint.allowedValues.end();
        } else if (std::holds_alternative<bool>(value)) {
            bool val = std::get<bool>(value);
            found = std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(),
                            (val ? "true" : "false")) != constraint.allowedValues.end();
        } else if (std::holds_alternative<std::string>(value)) {
            std::string val = std::get<std::string>(value);
            found = std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(), val) != constraint.allowedValues.end();
        }
        
        if (!found) isValid = false;
    }
    
    // Custom validator
    if (constraint.customValidator && isValid) {
        isValid = constraint.customValidator(value);
    }
    
    return isValid;
}

void Config::emitChangeEvent(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    pImpl->changeHistory.push_back({key, {oldValue, newValue}});
    
    if (monitoringEnabled_ && changeCallback_) {
        changeCallback_(key, oldValue, newValue);
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "yes" || lower == "1" || lower == "on";
}

ConfigValue Config::parseValue(const std::string& str, const std::string& group) {
    if (str.empty()) {
        return ConfigValue(); // Default variant (empty)
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        return stringToBool(str);
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as int64_t
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

std::string Config::summary(const std::string& group) const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    auto entries = getEntries(group);
    for (const auto& entry : entries) {
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

std::string Config::schemaSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schemas:\n";
    
    for (const auto& pair : pImpl->schemas) {
        const ConfigSchema& schema = pair.second;
        oss << "  Schema: " << schema.name << " (Category: " << static_cast<int>(schema.category) << ")\n";
        oss << "    Parameters: " << schema.parameters.size() << "\n";
        oss << "    Description: " << schema.description << "\n";
    }
    
    return oss.str();
}

} // namespace nlm