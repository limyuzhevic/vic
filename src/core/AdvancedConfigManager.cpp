#include "AdvancedConfigManager.hpp"
#include <unordered_map>

ConfigValueType AdvancedConfigManager::ConfigValueTypeToType(const std::string& typeStr) const {
    static const std::unordered_map<std::string, ConfigValueType> typeMap = {
        {"int", ConfigValueType::Int},
        {"int64", ConfigValueType::Int64},
        {"double", ConfigValueType::Double},
        {"bool", ConfigValueType::Bool},
        {"string", ConfigValueType::String},
        {"vector<int>", ConfigValueType::VectorInt},
        {"vector<double>", ConfigValueType::VectorDouble},
        {"vector<string>", ConfigValueType::VectorString}
    };
    
    auto it = typeMap.find(typeStr);
    if (it != typeMap.end()) {
        return it->second;
    }
    return ConfigValueType::String;
}

namespace nlm {

struct AdvancedConfigManager::Impl {
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, ConfigTemplate> templates;
    std::vector<std::pair<std::string, ConfigUpdateCallback>> updateCallbacks;
    std::chrono::system_clock::time_point lastModified;
    bool autoReloadEnabled = false;
    std::string autoReloadPath;
    std::chrono::milliseconds autoReloadInterval;
    std::chrono::system_clock::time_point lastReloadTime;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> changes;
    std::vector<std::string> validationErrors;
};

AdvancedConfigManager::AdvancedConfigManager() : pImpl(std::make_unique<Impl>()) {}
AdvancedConfigManager::~AdvancedConfigManager() = default;

void AdvancedConfigManager::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

void AdvancedConfigManager::removeSchema(const std::string& key) {
    pImpl->schemas.erase(key);
}

bool AdvancedConfigManager::validateSchema(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->schemas.find(key);
    if (it == pImpl->schemas.end()) {
        return true; // No schema defined, assume valid
    }
    
    return validateValue(it->second, value);
}

std::vector<std::string> AdvancedConfigManager::getValidationErrors() const {
    return pImpl->validationErrors;
}

void AdvancedConfigManager::loadTemplate(const std::string& templateName) {
    // TODO: Implement template loading from files
    ConfigTemplate template;
    template.name = templateName;
    pImpl->templates[templateName] = template;
}

void AdvancedConfigManager::saveTemplate(const std::string& templateName, const std::string& description) {
    ConfigTemplate template;
    template.name = templateName;
    template.description = description;
    pImpl->templates[templateName] = template;
}

bool AdvancedConfigManager::hasTemplate(const std::string& templateName) const {
    return pImpl->templates.find(templateName) != pImpl->templates.end();
}

std::vector<std::string> AdvancedConfigManager::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

void AdvancedConfigManager::validateAll() const {
    pImpl->validationErrors.clear();
    
    // Validate against schemas
    for (const auto& schema : pImpl->schemas) {
        if (has(schema.first)) {
            auto value = get(ConfigValueTypeToType(schema.second.type), schema.first);
            if (value && !validateValue(schema.second, *value)) {
                pImpl->validationErrors.push_back("Schema validation failed for key: " + schema.first);
            }
        }
    }
}

bool AdvancedConfigManager::isValid() const {
    validateAll();
    return pImpl->validationErrors.empty() && Config::isValid();
}

bool AdvancedConfigManager::reloadFromFile(const std::string& filepath) {
    if (loadFromFile(filepath)) {
        validateAll();
        pImpl->lastModified = std::chrono::system_clock::now();
        pImpl->lastReloadTime = pImpl->lastModified;
        return true;
    }
    return false;
}

bool AdvancedConfigManager::enableAutoReload(const std::string& filepath, std::chrono::milliseconds interval) {
    pImpl->autoReloadEnabled = true;
    pImpl->autoReloadPath = filepath;
    pImpl->autoReloadInterval = interval;
    return reloadFromFile(filepath);
}

void AdvancedConfigManager::disableAutoReload() {
    pImpl->autoReloadEnabled = false;
    pImpl->autoReloadPath.clear();
}

bool AdvancedConfigManager::validateWithDetails(std::vector<std::pair<std::string, std::string>>& errors) const {
    errors.clear();
    
    // Validate schema constraints
    for (const auto& schema : pImpl->schemas) {
        if (has(schema.first)) {
            auto value = get(ConfigValueTypeToType(schema.second.type), schema.first);
            if (value && !validateValue(schema.second, *value)) {
                errors.push_back({schema.first, "Schema validation failed"});
            }
        } else if (schema.second.required) {
            errors.push_back({schema.first, "Required key is missing"});
        }
    }
    
    // Validate type-specific constraints
    if (has("neuron_count")) {
        auto neuronCount = get<size_t>("neuron_count");
        if (neuronCount && (*neuronCount == 0 || *neuronCount > 1000000)) {
            errors.push_back({"neuron_count", "Neuron count must be between 1 and 1,000,000"});
        }
    }
    
    if (has("region_count")) {
        auto regionCount = get<size_t>("region_count");
        if (regionCount && (*regionCount == 0 || *regionCount > 1000)) {
            errors.push_back({"region_count", "Region count must be between 1 and 1000"});
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            errors.push_back({"simulation_timestep", "Simulation timestep must be between 0.001 and 1.0"});
        }
    }
    
    // Add config errors
    for (const auto& error : getErrors()) {
        errors.push_back({error.key, error.message});
    }
    
    return errors.empty();
}

std::string AdvancedConfigManager::getValidationReport() const {
    std::stringstream report;
    report << "NLM Configuration Validation Report" << std::endl;
    report << std::string(50, '=') << std::endl << std::endl;
    
    report << "Validation Status: " << (isValid() ? "PASSED" : "FAILED") << std::endl;
    report << "Last Modified: " << std::chrono::system_clock::to_time_t(pImpl->lastModified) << std::endl;
    report << std::endl;
    
    if (!pImpl->validationErrors.empty()) {
        report << "Schema Validation Errors:" << std::endl;
        for (const auto& error : pImpl->validationErrors) {
            report << "  - " << error << std::endl;
        }
        report << std::endl;
    }
    
    auto configErrors = getErrors();
    if (!configErrors.empty()) {
        report << "Configuration Errors:" << std::endl;
        for (const auto& error : configErrors) {
            report << "  - " << error.key << ": " << error.message << std::endl;
        }
        report << std::endl;
    }
    
    report << "Configuration Summary:" << std::endl;
    report << "  Total entries: " << getEntryCount() << std::endl;
    report << "  Valid entries: " << getValidEntryCount() << std::endl;
    report << "  Invalid entries: " << (getEntryCount() - getValidEntryCount()) << std::endl;
    
    return report.str();
}

std::string AdvancedConfigManager::generateDocumentation(const std::string& format) const {
    if (format == "markdown") {
        return generateSchemaDocumentation();
    } else if (format == "json") {
        // TODO: Implement JSON documentation
        return "{\"documentation\": \"JSON format not yet implemented\"}";
    } else if (format == "html") {
        // TODO: Implement HTML documentation
        return "<html><body>HTML documentation not yet implemented</body></html>";
    }
    return generateSchemaDocumentation();
}

std::string AdvancedConfigManager::generateSchemaDocumentation() const {
    std::stringstream doc;
    doc << "# NLM Configuration Documentation" << std::endl;
    doc << std::endl;
    doc << "This document describes all configuration parameters for NLM (Neural Learning Machine)." << std::endl;
    doc << std::endl;
    
    for (const auto& schema : pImpl->schemas) {
        doc << "## " << schema.first << std::endl;
        doc << std::endl;
        doc << "- **Type**: " << schema.second.type << std::endl;
        doc << "- **Description**: " << schema.second.description << std::endl;
        doc << "- **Required**: " << (schema.second.required ? "Yes" : "No") << std::endl;
        
        if (!schema.second.defaultValue.empty()) {
            doc << "- **Default**: " << schema.second.defaultValue << std::endl;
        }
        
        if (!schema.second.allowedValues.empty()) {
            doc << "- **Allowed Values**: ";
            for (size_t i = 0; i < schema.second.allowedValues.size(); ++i) {
                if (i > 0) doc << ", ";
                doc << schema.second.allowedValues[i];
            }
            doc << std::endl;
        }
        
        if (schema.second.minValue != std::numeric_limits<double>::lowest()) {
            doc << "- **Minimum Value**: " << schema.second.minValue << std::endl;
        }
        
        if (schema.second.maxValue != std::numeric_limits<double>::max()) {
            doc << "- **Maximum Value**: " << schema.second.maxValue << std::endl;
        }
        
        doc << std::endl;
    }
    
    return doc.str();
}

std::string AdvancedConfigManager::generateUsageExamples() const {
    std::stringstream examples;
    examples << "# NLM Usage Examples" << std::endl;
    examples << std::endl;
    examples << "## Basic Usage" << std::endl;
    examples << "```bash" << std::endl;
    examples << "nlm run --config=my_config.cfg" << std::endl;
    examples << "```" << std::endl;
    examples << std::endl;
    
    examples << "## Configuration File Generation" << std::endl;
    examples << "```bash" << std::endl;
    examples << "nlm generate-template --output=config_template.cfg" << std::endl;
    examples << "```" << std::endl;
    examples << std::endl;
    
    examples << "## Advanced Configuration" << std::endl;
    examples << "```bash" << std::endl;
    examples << "nlm run --neuron_count=2000 --simulation_timestep=0.0005 \"""""""" --log_level=debug" << std::endl;
    examples << "```" << std::endl;
    examples << std::endl;
    
    examples << "## Benchmark Mode" << std::endl;
    examples << "```bash" << std::endl;
    examples << "nlm benchmark --duration=60 --output=benchmark_results.json --format=json" << std::endl;
    examples << "```" << std::endl;
    examples << std::endl;
    
    return examples.str();
}

void AdvancedConfigManager::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                               bool validate, const std::string& reason) {
    if (validate && !validateSchema(key, value)) {
        pImpl->validationErrors.push_back("Validation failed for key: " + key);
        return;
    }
    
    // Check for changes
    if (has(key)) {
        auto oldValue = get(ConfigValueTypeToType(value), key);
        if (oldValue && *oldValue == value) {
            return; // No change
        }
        pImpl->changes[key] = std::make_pair(*oldValue, value);
    } else {
        pImpl->changes[key] = std::make_pair(ConfigValue(), value);
    }
    
    Config::set(key, value, source);
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Trigger callbacks
    for (const auto& callbackPair : pImpl->updateCallbacks) {
        try {
            callbackPair.second(key, oldValue ? *oldValue : ConfigValue(), value);
        } catch (const std::exception& e) {
            // Log error but don't fail
        }
    }
}

void AdvancedConfigManager::set(const std::string& key, const std::string& value, ConfigSource source,
                               bool validate, const std::string& reason) {
    set(key, ConfigValue(value), source, validate, reason);
}

void AdvancedConfigManager::set(const std::string& key, int value, ConfigSource source,
                               bool validate, const std::string& reason) {
    set(key, ConfigValue(value), source, validate, reason);
}

void AdvancedConfigManager::set(const std::string& key, double value, ConfigSource source,
                               bool validate, const std::string& reason) {
    set(key, ConfigValue(value), source, validate, reason);
}

void AdvancedConfigManager::set(const std::string& key, bool value, ConfigSource source,
                               bool validate, const std::string& reason) {
    set(key, ConfigValue(value), source, validate, reason);
}

void AdvancedConfigManager::addUpdateCallback(const ConfigUpdateCallback& callback) {
    static int callbackId = 0;
    std::string callbackIdStr = "callback_" + std::to_string(++callbackId);
    pImpl->updateCallbacks.push_back(std::make_pair(callbackIdStr, callback));
}

void AdvancedConfigManager::removeUpdateCallback(const std::string& callbackId) {
    pImpl->updateCallbacks.erase(
        std::remove_if(pImpl->updateCallbacks.begin(), pImpl->updateCallbacks.end(),
            [&callbackId](const auto& pair) { return pair.first == callbackId; }),
        pImpl->updateCallbacks.end()
    );
}

void AdvancedConfigManager::clearUpdateCallbacks() {
    pImpl->updateCallbacks.clear();
}

bool AdvancedConfigManager::exportToJSON(const std::string& filepath, bool includeDefaults) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        if (includeDefaults || entry.source != ConfigSource::Default) {
            jsonData[entry.key] = configValueToString(entry.value);
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << jsonData << std::endl;
    return true;
}

bool AdvancedConfigManager::importFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        return false;
    }
    
    clear();
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        set(it.key(), ConfigValue(it.value().get<std::string>()), ConfigSource::File);
    }
    
    validateAll();
    return true;
}

bool AdvancedConfigManager::hasChanges() const {
    return !pImpl->changes.empty();
}

std::map<std::string, std::pair<ConfigValue, ConfigValue>> AdvancedConfigManager::getChanges() const {
    return pImpl->changes;
}

void AdvancedConfigManager::resetChangeTracking() {
    pImpl->changes.clear();
}

size_t AdvancedConfigManager::getEntryCount() const {
    return Config::getEntryCount();
}

size_t AdvancedConfigManager::getValidEntryCount() const {
    size_t validCount = 0;
    validateAll();
    return getEntryCount() - pImpl->validationErrors.size();
}

std::chrono::system_clock::time_point AdvancedConfigManager::getLastModified() const {
    return pImpl->lastModified;
}

bool AdvancedConfigManager::validateValue(const ConfigSchema& schema, const ConfigValue& value) const {
    // Validate based on type
    try {
        switch (getConfigValueTypeFromString(schema.type)) {
            case ConfigValueType::Int:
                if (!std::holds_alternative<int>(value)) return false;
                break;
            case ConfigValueType::Int64:
                if (!std::holds_alternative<int64_t>(value)) return false;
                break;
            case ConfigValueType::Double:
                if (!std::holds_alternative<double>(value)) return false;
                break;
            case ConfigValueType::Bool:
                if (!std::holds_alternative<bool>(value)) return false;
                break;
            case ConfigValueType::String:
                if (!std::holds_alternative<std::string>(value)) return false;
                break;
            case ConfigValueType::VectorInt:
                if (!std::holds_alternative<std::vector<int>>(value)) return false;
                break;
            case ConfigValueType::VectorDouble:
                if (!std::holds_alternative<std::vector<double>>(value)) return false;
                break;
            case ConfigValueType::VectorString:
                if (!std::holds_alternative<std::vector<std::string>>(value)) return false;
                break;
            default:
                return false;
        }
        
        // Additional validation based on schema constraints
        if (schema.isArray) {
            auto it = std::get_if<std::vector<int>>(&value);
            if (it) {
                if (it->size() < schema.minSize || it->size() > schema.maxSize) return false;
            }
        }
        
    } catch (const std::exception&) {
        return false;
    }
    
    return true;
}

std::string AdvancedConfigManager::configValueToString(const ConfigValue& value) const {
    std::stringstream ss;
    
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
    
    return ss.str();
}

std::string AdvancedConfigManager::getConfigValueDescription(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    if (it != pImpl->schemas.end()) {
        return it->second.description;
    }
    return "";
}

ConfigValueType AdvancedConfigManager::getConfigValueTypeFromString(const std::string& typeStr) const {
    static const std::map<std::string, ConfigValueType> typeMap = {
        {"int", ConfigValueType::Int},
        {"int64", ConfigValueType::Int64},
        {"double", ConfigValueType::Double},
        {"bool", ConfigValueType::Bool},
        {"string", ConfigValueType::String},
        {"vector<int>", ConfigValueType::VectorInt},
        {"vector<double>", ConfigValueType::VectorDouble},
        {"vector<string>", ConfigValueType::VectorString}
    };
    
    auto it = typeMap.find(typeStr);
    if (it != typeMap.end()) {
        return it->second;
    }
    return ConfigValueType::String;
}

} // namespace nlm
