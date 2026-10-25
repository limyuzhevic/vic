// AdvancedConfig.cpp implementation
#include "AdvancedConfig.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_set>

namespace nlm {

struct AdvancedConfig::Impl {
    std::shared_ptr<Config> config;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, ConfigSchema> schemaMap;
    std::unordered_map<Category, std::vector<std::string>> categoryMap;
    std::vector<ValidationError> lastValidationErrors;
    bool validationEnabled;
    
    // File watching for hot-reload
    std::filesystem::file_time_type lastModifiedTime;
    bool watchForChanges;
    
    Impl() : config(std::make_shared<Config>()), validationEnabled(true), watchForChanges(false) {}
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {
    initializeSchemas();
}

AdvancedConfig::~AdvancedConfig() = default;

AdvancedConfig::AdvancedConfig(AdvancedConfig&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

AdvancedConfig& AdvancedConfig::operator=(AdvancedConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void AdvancedConfig::initializeSchemas() {
    auto& impl = *pImpl;
    
    // Register all predefined schemas
    auto allSchemas = ConfigSchemaRegistry::getAllSchemas();
    for (const auto& schema : allSchemas) {
        addSchema(schema);
    }
}

void AdvancedConfig::addSchema(const ConfigSchema& schema) {
    auto& impl = *pImpl;
    impl.schemas.push_back(schema);
    impl.schemaMap[schema.key] = schema;
    
    // Add to category map
    Category category = CategoryFromString(schema.category);
    impl.categoryMap[category].push_back(schema.key);
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    // First, validate file exists and is readable
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, use the basic Config class
    if (!pImpl->config->loadFromFile(filepath)) {
        return false;
    }
    
    // Update file modification time for hot-reload
    if (pImpl->watchForChanges) {
        pImpl->lastModifiedTime = std::filesystem::last_write_time(filepath);
    }
    
    return true;
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    return pImpl->config->saveToFile(filepath);
}

void AdvancedConfig::loadFromEnvironment(const std::string& prefix) {
    // Basic implementation - in practice this would parse env vars
    // and set them in the config
    (void)prefix; // Avoid unused parameter warning
}

bool AdvancedConfig::loadFromArgs(int argc, char** argv) {
    // Basic implementation - in practice this would parse command line args
    // and set them in the config
    (void)argc; (void)argv; // Avoid unused parameter warnings
    return true;
}

std::vector<AdvancedConfig::ValidationError> AdvancedConfig::validate() const {
    auto& impl = *pImpl;
    impl.lastValidationErrors.clear();
    
    if (!impl.validationEnabled) {
        return impl.lastValidationErrors;
    }
    
    // Check all required schemas
    for (const auto& schema : impl.schemas) {
        if (schema.required) {
            if (!pImpl->config->has(schema.key)) {
                ValidationError error(schema.key, 
                    "Required configuration key is missing", "");
                impl.lastValidationErrors.push_back(error);
            }
        }
        
        // Validate if key exists
        if (pImpl->config->has(schema.key)) {
            auto value = pImpl->config->get<ConfigValue>(schema.key);
            if (value) {
                if (!schema.validate(*value)) {
                    // This would need a proper validation error message
                    ValidationError error(schema.key, "Configuration value is invalid", "");
                    impl.lastValidationErrors.push_back(error);
                }
            }
        }
    }
    
    return impl.lastValidationErrors;
}

std::vector<std::string> AdvancedConfig::getValidationErrors() const {
    auto errors = validate();
    std::vector<std::string> errorMessages;
    for (const auto& error : errors) {
        errorMessages.push_back(error.key + ": " + error.message);
    }
    return errorMessages;
}

bool AdvancedConfig::isValid() const {
    return validate().empty();
}

bool AdvancedConfig::set(const std::string& key, const ConfigValue& value) {
    auto schema = getSchema(key);
    if (!schema) {
        return false; // Key not in schema
    }
    
    // Transform value through schema transformers
    ConfigValue transformedValue = value;
    for (const auto& transformer : schema->transformers) {
        transformedValue = transformer(transformedValue);
    }
    
    // Validate transformed value
    if (!schema->validate(transformedValue)) {
        return false;
    }
    
    // Set in underlying config
    pImpl->config->set(key, transformedValue);
    return true;
}

const AdvancedConfig::ConfigSchema* AdvancedConfig::getSchema(const std::string& key) const {
    auto it = pImpl->schemaMap.find(key);
    if (it != pImpl->schemaMap.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> AdvancedConfig::getKeysInCategory(Category category) const {
    auto it = pImpl->categoryMap.find(category);
    if (it != pImpl->categoryMap.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

std::string AdvancedConfig::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "====================" << std::endl;
    
    for (const auto& schema : pImpl->schemas) {
        ss << schema.key << " (" << schema.category << "):" << std::endl;
        ss << "  Description: " << schema.description << std::endl;
        
        if (pImpl->config->has(schema.key)) {
            auto value = pImpl->config->get<ConfigValue>(schema.key);
            if (value) {
                ss << "  Value: " << valueToString(*value) << std::endl;
            }
        } else {
            ss << "  Value: [default] " << valueToString(schema.defaultValue) << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

void AdvancedConfig::printValidationErrors(std::ostream& out) const {
    auto errors = getValidationErrors();
    if (errors.empty()) {
        out << "Configuration is valid" << std::endl;
        return;
    }
    
    out << "Configuration validation errors:" << std::endl;
    for (const auto& error : errors) {
        out << "  " << error << std::endl;
    }
}

std::string AdvancedConfig::generateJSONSchema() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"$schema\": \"http://json-schema.org/draft-07/schema#\"," << std::endl;
    ss << "  \"$id\": \"nlm-config-schema\"," << std::endl;
    ss << "  \"title\": \"NLM Neuromorphic Network Configuration\"," << std::endl;
    ss << "  \"description\": \"Advanced configuration for NLM neural network simulation\"" << std::endl;
    ss << "  \"type\": \"object\"," << std::endl;
    ss << "  \"properties\": {" << std::endl;
    
    for (size_t i = 0; i < pImpl->schemas.size(); ++i) {
        const auto& schema = pImpl->schemas[i];
        ss << "    \"" << schema.key << "\": {" << std::endl;
        ss << "      \"type\": \"\" << getTypeNameForSchema(schema) << "\"," << std::endl;
        ss << "      \"description\": \"" << escapeString(schema.description) << "\"," << std::endl;
        ss << "      \"default\": " << valueToJSON(schema.defaultValue) << "," << std::endl;
        if (schema.required) {
            ss << "      \"required\": true," << std::endl;
        }
        ss << "      \"category\": \"" << schema.category << "\"";
        if (i < pImpl->schemas.size() - 1) {
            ss << "," << std::endl;
        } else {
            ss << std::endl;
        }
        ss << "    }";
        if (i < pImpl->schemas.size() - 1) {
            ss << "," << std::endl;
        } else {
            ss << std::endl;
        }
    }
    
    ss << "  },
      \"required\": [" << std::endl;
    
    bool first = true;
    for (const auto& schema : pImpl->schemas) {
        if (schema.required) {
            if (!first) ss << ",\n        ";
            ss << "      \"" << schema.key << "\"";
            first = false;
        }
    }
    ss << "]," << std::endl;
    
    ss << "  \"categories\": {" << std::endl;
    // Add category definitions
    ss << "    \"Core\": {\"description\": \"Core system parameters\"}," << std::endl;
    ss << "    \"Memory\": {\"description\": \"Memory system parameters\"}," << std::endl;
    ss << "    \"Plasticity\": {\"description\": \"Synaptic plasticity parameters\"}," << std::endl;
    ss << "    \"Neuromodulation\": {\"description\": \"Neuromodulation system parameters\"}," << std::endl;
    ss << "    \"Cognition\": {\"description\": \"Cognitive system parameters\"}," << std::endl;
    ss << "    \"Development\": {\"description\": \"Development system parameters\"}," << std::endl;
    ss << "    \"Performance\": {\"description\": \"Performance optimization parameters\"}," << std::endl;
    ss << "    \"Debugging\": {\"description\": \"Debugging and profiling parameters\"}," << std::endl;
    ss << "    \"Experimental\": {\"description\": \"Experimental and research parameters\"}" << std::endl;
    
    ss << "  }" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::shared_ptr<AdvancedConfig> AdvancedConfig::createDefaultConfig() {
    auto config = std::make_shared<AdvancedConfig>();
    return config;
}

void AdvancedConfig::merge(const AdvancedConfig& other) {
    // Merge other config into this one
    for (const auto& schema : other.pImpl->schemas) {
        if (pImpl->config->has(schema.key)) {
            // Keep existing value
            continue;
        }
        
        if (std::holds_alternative<std::string>(schema.defaultValue)) {
            std::string value = std::get<std::string>(schema.defaultValue);
            if (!value.empty()) {
                pImpl->config->set(schema.key, value);
            }
        }
    }
}

void AdvancedConfig::resetToDefaults() {
    // Reset to factory defaults by clearing and re-initializing
    pImpl->config->clear();
    initializeSchemas();
}

bool AdvancedConfig::hotReload(const std::string& filepath) {
    if (!pImpl->watchForChanges) {
        return false;
    }
    
    std::filesystem::file_time_type currentTime = 
        std::filesystem::last_write_time(filepath);
    
    if (currentTime > pImpl->lastModifiedTime) {
        // File has been modified, reload it
        if (loadFromFile(filepath)) {
            pImpl->lastModifiedTime = currentTime;
            return true;
        }
    }
    
    return false;
}

// Helper functions
std::string AdvancedConfig::valueToString(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            ss << v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            ss << v;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << v;
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << v << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << v[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << v[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << v[i] << "\"";
            }
            ss << "]";
        } else {
            ss << "[unknown type]";
        }
    }, value);
    return ss.str();
}

std::string AdvancedConfig::valueToJSON(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int> || 
                      std::is_same_v<T, int64_t> || 
                      std::is_same_v<T, double>) {
            ss << v;
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << escapeString(v) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << v[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << v[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << escapeString(v[i]) << "\"";
            }
            ss << "]";
        } else {
            ss << "null";
        }
    }, value);
    return ss.str();
}

std::string AdvancedConfig::escapeString(const std::string& str) const {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '\"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string AdvancedConfig::getTypeNameForSchema(const ConfigSchema& schema) const {
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return "integer";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return "integer";
        } else if constexpr (std::is_same_v<T, double>) {
            return "number";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "boolean";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "array";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "array";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "array";
        }
    }, schema.defaultValue);
    return "string"; // default fallback
}

AdvancedConfig::Category AdvancedConfig::CategoryFromString(const std::string& category) const {
    static const std::unordered_map<std::string, Category> categoryMap = {
        {"Core", Category::Core},
        {"Memory", Category::Memory},
        {"Plasticity", Category::Plasticity},
        {"Neuromodulation", Category::Neuromodulation},
        {"Cognition", Category::Cognition},
        {"Development", Category::Development},
        {"Performance", Category::Performance},
        {"Debugging", Category::Debugging},
        {"Experimental", Category::Experimental}
    };
    
    auto it = categoryMap.find(category);
    if (it != categoryMap.end()) {
        return it->second;
    }
    return Category::Core;
}

bool AdvancedConfig::ConfigSchema::validate(const ConfigValue& value) const {
    for (const auto& validator : validators) {
        if (!validator(value)) {
            return false;
        }
    }
    return true;
}

ConfigValue AdvancedConfig::ConfigSchema::transform(const ConfigValue& value) const {
    ConfigValue result = value;
    for (const auto& transformer : transformers) {
        result = transformer(result);
    }
    return result;
}

std::string AdvancedConfig::ConfigSchema::getTypeName() const {
    return valueToString(defaultValue);
}

// ConfigSchemaRegistry implementation
AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createCoreSchemas() {
    ConfigSchema schema("neuron_count", "Number of neurons in the network", ConfigValue(1000), true, "Core");
    schema.required = true;
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createMemorySchemas() {
    ConfigSchema schema("working_memory_capacity", 
        "Capacity of working memory in number of traces", ConfigValue(100), false, "Memory");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createPlasticitySchemas() {
    ConfigSchema schema("stdp_ltp_weight", "STDP LTP weight", ConfigValue(0.01f), false, "Plasticity");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createNeuromodulationSchemas() {
    ConfigSchema schema("dopamine_baseline", "Baseline dopamine level", ConfigValue(0.1f), false, "Neuromodulation");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createCognitionSchemas() {
    ConfigSchema schema("planning_depth", "Planning horizon depth", ConfigValue(3), false, "Cognition");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createDevelopmentSchemas() {
    ConfigSchema schema("enable_development", "Enable developmental processes", ConfigValue(true), false, "Development");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createPerformanceSchemas() {
    ConfigSchema schema("enable_multithreading", "Enable multithreaded processing", ConfigValue(true), false, "Performance");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createDebuggingSchemas() {
    ConfigSchema schema("enable_debug_logging", "Enable debug logging", ConfigValue(false), false, "Debugging");
    return schema;
}

AdvancedConfig::ConfigSchema ConfigSchemaRegistry::createExperimentalSchemas() {
    ConfigSchema schema("experimental_mode", "Enable experimental features", ConfigValue(false), false, "Experimental");
    return schema;
}

std::vector<AdvancedConfig::ConfigSchema> ConfigSchemaRegistry::getAllSchemas() {
    std::vector<ConfigSchema> schemas;
    
    // Add schemas from each category
    schemas.push_back(createCoreSchemas());
    schemas.push_back(createMemorySchemas());
    schemas.push_back(createPlasticitySchemas());
    schemas.push_back(createNeuromodulationSchemas());
    schemas.push_back(createCognitionSchemas());
    schemas.push_back(createDevelopmentSchemas());
    schemas.push_back(createPerformanceSchemas());
    schemas.push_back(createDebuggingSchemas());
    schemas.push_back(createExperimentalSchemas());
    
    // Add more schemas (simplified for space)
    schemas.push_back(ConfigSchema("region_count", "Number of neural regions", ConfigValue(1), false, "Core"));
    schemas.push_back(ConfigSchema("connection_probability", "Probability of synaptic connections", ConfigValue(0.1f), false, "Core"));
    schemas.push_back(ConfigSchema("simulation_timestep", "Simulation timestep in milliseconds", ConfigValue(0.001f), false, "Core"));
    
    return schemas;
}

} // namespace nlm
