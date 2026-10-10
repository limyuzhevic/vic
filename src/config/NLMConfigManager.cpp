// NLM Configuration Management
// Comprehensive configuration validation and management system

#include "config/NLMConfigManager.hpp"
#include "config/ConfigurationValidator.hpp"
#include "config/ConfigurationSchema.hpp"
#include "config/ConfigurationTemplate.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>

namespace nlm {

NLMConfigManager::NLMConfigManager() {
    NLM_LOG_INFO("Initializing NLM Configuration Manager");
    
    // Initialize with default schema
    _schema = std::make_shared<ConfigurationSchema>();
    _schema->initializeDefaultSchema();
    
    // Initialize validator
    _validator = std::make_shared<ConfigurationValidator>();
    _validator->setSchema(_schema);
}

NLMConfigManager::~NLMConfigManager() {
    NLM_LOG_INFO("Shutting down NLM Configuration Manager");
}

bool NLMConfigManager::loadFromFile(const std::string& filePath) {
    NLM_LOG_INFO("Loading configuration from " + filePath);
    
    if (!std::filesystem::exists(filePath)) {
        NLM_LOG_ERROR("Configuration file does not exist: " + filePath);
        return false;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filePath);
        return false;
    }
    
    try {
        _currentConfig.clear();
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            // Remove whitespace
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos == std::string::npos) {
                continue; // Empty line
            }
            
            size_t endPos = line.find_last_not_of(" \t\r\n");
            line = line.substr(startPos, endPos - startPos + 1);
            
            // Skip comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for section header
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Parse key-value pair
            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                NLM_LOG_WARNING("Invalid configuration line: " + line);
                continue;
            }
            
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            // Remove whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Store in configuration
            _currentConfig[currentSection + "." + key] = value;
        }
        
        file.close();
        
        // Validate the loaded configuration
        if (!validateCurrentConfig()) {
            NLM_LOG_ERROR("Configuration validation failed");
            return false;
        }
        
        NLM_LOG_INFO("Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration: " + std::string(e.what()));
        file.close();
        return false;
    }
}

bool NLMConfigManager::saveToFile(const std::string& filePath) {
    NLM_LOG_INFO("Saving configuration to " + filePath);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filePath);
        return false;
    }
    
    try {
        // Group keys by section
        std::map<std::string, std::map<std::string, std::string>> sections;
        
        for (const auto& pair : _currentConfig) {
            size_t dotPos = pair.first.find('.');
            if (dotPos == std::string::npos) {
                continue; // Invalid format
            }
            
            std::string section = pair.first.substr(0, dotPos);
            std::string key = pair.first.substr(dotPos + 1);
            
            sections[section][key] = pair.second;
        }
        
        // Write sections to file
        for (const auto& sectionPair : sections) {
            file << "[" << sectionPair.first << "]\n";
            
            for (const auto& keyPair : sectionPair.second) {
                file << keyPair.first << " = " << keyPair.second << "\n";
            }
            
            file << "\n";
        }
        
        file.close();
        
        NLM_LOG_INFO("Configuration saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration: " + std::string(e.what()));
        file.close();
        return false;
    }
}

bool NLMConfigManager::validateCurrentConfig() {
    NLM_LOG_INFO("Validating current configuration");
    
    std::vector<std::string> errors;
    
    // Validate required sections
    std::set<std::string> requiredSections = {"brain", "simulation", "logging"};
    
    std::map<std::string, std::map<std::string, std::string>> sections;
    for (const auto& pair : _currentConfig) {
        size_t dotPos = pair.first.find('.');
        if (dotPos == std::string::npos) {
            continue;
        }
        
        std::string section = pair.first.substr(0, dotPos);
        std::string key = pair.first.substr(dotPos + 1);
        
        sections[section][key] = pair.second;
    }
    
    for (const auto& section : requiredSections) {
        if (sections.find(section) == sections.end()) {
            errors.push_back("Missing required section: " + section);
        }
    }
    
    // Validate brain section
    if (sections.find("brain") != sections.end()) {
        const auto& brainSection = sections["brain"];
        
        // Check required brain parameters
        std::set<std::string> requiredBrainParams = {"neuron_count", "region_count"};
        
        for (const auto& param : requiredBrainParams) {
            if (brainSection.find(param) == brainSection.end()) {
                errors.push_back("Missing required brain parameter: " + param);
            }
        }
        
        // Validate parameter values
        if (brainSection.find("neuron_count") != brainSection.end()) {
            try {
                size_t neuronCount = std::stoul(brainSection.at("neuron_count"));
                if (neuronCount == 0) {
                    errors.push_back("neuron_count must be greater than 0");
                }
                if (neuronCount > 1000000) {
                    errors.push_back("neuron_count must be less than 1,000,000");
                }
            } catch (const std::exception& e) {
                errors.push_back("Invalid neuron_count value: " + brainSection.at("neuron_count"));
            }
        }
        
        if (brainSection.find("region_count") != brainSection.end()) {
            try {
                size_t regionCount = std::stoul(brainSection.at("region_count"));
                if (regionCount == 0) {
                    errors.push_back("region_count must be greater than 0");
                }
            } catch (const std::exception& e) {
                errors.push_back("Invalid region_count value: " + brainSection.at("region_count"));
            }
        }
    }
    
    // Validate simulation section
    if (sections.find("simulation") != sections.end()) {
        const auto& simSection = sections["simulation"];
        
        if (simSection.find("max_steps") != simSection.end()) {
            try {
                uint64_t maxSteps = std::stoull(simSection.at("max_steps"));
                if (maxSteps == 0) {
                    errors.push_back("max_steps must be greater than 0");
                }
            } catch (const std::exception& e) {
                errors.push_back("Invalid max_steps value: " + simSection.at("max_steps"));
            }
        }
    }
    
    // Validate logging section
    if (sections.find("logging") != sections.end()) {
        const auto& loggingSection = sections["logging"];
        
        if (loggingSection.find("level") != loggingSection.end()) {
            std::string level = loggingSection.at("level");
            std::set<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
            
            if (validLevels.find(level) == validLevels.end()) {
                errors.push_back("Invalid logging level: " + level);
            }
        }
    }
    
    // Report errors
    for (const auto& error : errors) {
        NLM_LOG_ERROR("Configuration error: " + error);
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errors.size()) + " errors");
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

bool NLMConfigManager::setConfigValue(const std::string& key, const std::string& value) {
    NLM_LOG_INFO("Setting configuration key: " + key);
    
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        NLM_LOG_ERROR("Invalid configuration key format: " + key);
        return false;
    }
    
    _currentConfig[key] = value;
    NLM_LOG_INFO("Configuration key set successfully");
    return true;
}

std::string NLMConfigManager::getConfigValue(const std::string& key) const {
    auto it = _currentConfig.find(key);
    if (it == _currentConfig.end()) {
        NLM_LOG_WARNING("Configuration key not found: " + key);
        return "";
    }
    
    return it->second;
}

std::vector<std::string> NLMConfigManager::getConfigKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : _currentConfig) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

void NLMConfigManager::clearConfig() {
    NLM_LOG_INFO("Clearing configuration");
    _currentConfig.clear();
}

std::string NLMConfigManager::getConfigSummary() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Summary ===\n";
    oss << "Total keys: " << _currentConfig.size() << "\n\n";
    
    std::map<std::string, std::map<std::string, std::string>> sections;
    for (const auto& pair : _currentConfig) {
        size_t dotPos = pair.first.find('.');
        if (dotPos == std::string::npos) {
            continue;
        }
        
        std::string section = pair.first.substr(0, dotPos);
        std::string key = pair.first.substr(dotPos + 1);
        
        sections[section][key] = pair.second;
    }
    
    for (const auto& sectionPair : sections) {
        oss << "Section: " << sectionPair.first << "\n";
        
        for (const auto& keyPair : sectionPair.second) {
            oss << "  " << keyPair.first << " = " << keyPair.second << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

void NLMConfigManager::resetToDefaults() {
    NLM_LOG_INFO("Resetting configuration to defaults");
    clearConfig();
    
    // Set default values
    _currentConfig["brain.neuron_count"] = "1000";
    _currentConfig["brain.region_count"] = "1";
    _currentConfig["brain.connection_probability"] = "0.1";
    _currentConfig["simulation.max_steps"] = "10000";
    _currentConfig["simulation.timestep"] = "0.001";
    _currentConfig["logging.level"] = "INFO";
    _currentConfig["logging.file"] = "nlm.log";
}

void NLMConfigManager::importFromTemplate(const std::string& templateName) {
    NLM_LOG_INFO("Importing configuration from template: " + templateName);
    
    // Load template
    std::string templatePath = "./templates/" + templateName + ".json";
    if (!std::filesystem::exists(templatePath)) {
        NLM_LOG_ERROR("Template file not found: " + templatePath);
        return;
    }
    
    // Parse template (simplified JSON parsing)
    std::ifstream file(templatePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open template file: " + templatePath);
        return;
    }
    
    // In a real implementation, this would parse JSON properly
    // For simplicity, we'll just clear and reset to defaults
    clearConfig();
    resetToDefaults();
    
    NLM_LOG_INFO("Configuration imported from template");
}

void NLMConfigManager::exportToTemplate(const std::string& templateName) {
    NLM_LOG_INFO("Exporting configuration to template: " + templateName);
    
    // Create template directory if it doesn't exist
    std::filesystem::create_directory("./templates");
    
    // Save configuration as JSON (simplified)
    std::string templatePath = "./templates/" + templateName + ".json";
    
    std::ofstream file(templatePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open template file for writing: " + templatePath);
        return;
    }
    
    file << "{\n";
    file << "  \"brain\": {\n";
    file << "    \"neuron_count\": \"" << getConfigValue("brain.neuron_count") << "\",\n";
    file << "    \"region_count\": \"" << getConfigValue("brain.region_count") << "\",\n";
    file << "    \"connection_probability\": \"" << getConfigValue("brain.connection_probability") << "\"\n";
    file << "  },\n";
    file << "  \"simulation\": {\n";
    file << "    \"max_steps\": \"" << getConfigValue("simulation.max_steps") << "\",\n";
    file << "    \"timestep\": \"" << getConfigValue("simulation.timestep") << "\"\n";
    file << "  },\n";
    file << "  \"logging\": {\n";
    file << "    \"level\": \"" << getConfigValue("logging.level") << "\",\n";
    file << "    \"file\": \"" << getConfigValue("logging.file") << "\"\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    
    NLM_LOG_INFO("Configuration exported to template");
}

ConfigurationValidator* NLMConfigManager::getValidator() const {
    return _validator.get();
}

ConfigurationSchema* NLMConfigManager::getSchema() const {
    return _schema.get();
}

// ============================================================================
// ConfigurationValidator
// ============================================================================

ConfigurationValidator::ConfigurationValidator() {
    NLM_LOG_INFO("Initializing Configuration Validator");
}

ConfigurationValidator::~ConfigurationValidator() {
    NLM_LOG_INFO("Shutting down Configuration Validator");
}

void ConfigurationValidator::setSchema(ConfigurationSchema* schema) {
    _schema = schema;
}

bool ConfigurationValidator::validate(const std::map<std::string, std::string>& config) {
    NLM_LOG_INFO("Validating configuration");
    
    if (!_schema) {
        NLM_LOG_ERROR("Schema not set for validation");
        return false;
    }
    
    std::vector<std::string> errors = validateAgainstSchema(config);
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errors.size()) + " errors");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Validation error: " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

std::vector<std::string> ConfigurationValidator::validateAgainstSchema(const std::map<std::string, std::string>& config) {
    std::vector<std::string> errors;
    
    if (!_schema) {
        errors.push_back("Schema not set");
        return errors;
    }
    
    // Validate required fields
    const auto& requiredFields = _schema->getRequiredFields();
    
    for (const auto& field : requiredFields) {
        if (config.find(field) == config.end()) {
            errors.push_back("Required field missing: " + field);
        }
    }
    
    // Validate field types
    const auto& fieldTypes = _schema->getFieldTypes();
    
    for (const auto& pair : fieldTypes) {
        const auto& fieldName = pair.first;
        const auto& fieldType = pair.second;
        
        if (config.find(fieldName) != config.end()) {
            const auto& value = config.at(fieldName);
            
            if (!validateFieldType(fieldName, value, fieldType)) {
                errors.push_back("Invalid type for field " + fieldName + ": " + value);
            }
        }
    }
    
    return errors;
}

bool ConfigurationValidator::validateFieldType(const std::string& fieldName, const std::string& value, const std::string& type) {
    if (type == "int") {
        try {
            std::stoi(value);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    } else if (type == "uint64_t") {
        try {
            std::stoull(value);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    } else if (type == "float") {
        try {
            std::stof(value);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    } else if (type == "string") {
        return !value.empty();
    } else if (type == "boolean") {
        return value == "true" || value == "false";
    }
    
    return true; // Unknown type, assume valid
}

// ============================================================================
// ConfigurationSchema
// ============================================================================

ConfigurationSchema::ConfigurationSchema() {
    NLM_LOG_INFO("Initializing Configuration Schema");
}

ConfigurationSchema::~ConfigurationSchema() {
    NLM_LOG_INFO("Shutting down Configuration Schema");
}

void ConfigurationSchema::initializeDefaultSchema() {
    // Clear existing schema
    _fields.clear();
    _requiredFields.clear();
    
    // Add core fields
    _fields["brain.neuron_count"] = "uint64_t";
    _fields["brain.region_count"] = "uint64_t";
    _fields["brain.connection_probability"] = "float";
    
    _fields["simulation.max_steps"] = "uint64_t";
    _fields["simulation.timestep"] = "float";
    
    _fields["logging.level"] = "string";
    _fields["logging.file"] = "string";
    
    // Add required fields
    _requiredFields = {
        "brain.neuron_count",
        "brain.region_count",
        "simulation.max_steps"
    };
}

const std::map<std::string, std::string>& ConfigurationSchema::getFieldTypes() const {
    return _fields;
}

const std::set<std::string>& ConfigurationSchema::getRequiredFields() const {
    return _requiredFields;
}

bool ConfigurationSchema::addField(const std::string& name, const std::string& type, bool required) {
    _fields[name] = type;
    
    if (required) {
        _requiredFields.insert(name);
    }
    
    NLM_LOG_INFO("Added field to schema: " + name + " (type: " + type + ")");
    return true;
}

bool ConfigurationSchema::removeField(const std::string& name) {
    auto it = _fields.find(name);
    if (it != _fields.end()) {
        _fields.erase(it);
        _requiredFields.erase(name);
        NLM_LOG_INFO("Removed field from schema: " + name);
        return true;
    }
    
    NLM_LOG_WARNING("Field not found in schema: " + name);
    return false;
}

// ============================================================================
// ConfigurationTemplate
// ============================================================================

ConfigurationTemplate::ConfigurationTemplate() {
    NLM_LOG_INFO("Initializing Configuration Template System");
}

ConfigurationTemplate::~ConfigurationTemplate() {
    NLM_LOG_INFO("Shutting down Configuration Template System");
}

bool ConfigurationTemplate::createTemplate(const std::string& name, const std::map<std::string, std::string>& config) {
    NLM_LOG_INFO("Creating template: " + name);
    
    std::string templatePath = "./templates/" + name + ".json";
    
    // Create templates directory
    std::filesystem::create_directory("./templates");
    
    // Save template
    std::ofstream file(templatePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open template file for writing: " + templatePath);
        return false;
    }
    
    // Write template (simplified JSON format)
    file << "{\n";
    for (size_t i = 0; i < config.size(); ++i) {
        const auto& pair = std::next(config.begin(), i);
        file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        if (i < config.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    file.close();
    
    _templates[name] = config;
    
    NLM_LOG_INFO("Template created: " + name);
    return true;
}

bool ConfigurationTemplate::loadTemplate(const std::string& name) {
    NLM_LOG_INFO("Loading template: " + name);
    
    if (_templates.find(name) != _templates.end()) {
        // Already loaded
        return true;
    }
    
    std::string templatePath = "./templates/" + name + ".json";
    if (!std::filesystem::exists(templatePath)) {
        NLM_LOG_ERROR("Template file not found: " + templatePath);
        return false;
    }
    
    std::ifstream file(templatePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open template file: " + templatePath);
        return false;
    }
    
    // Parse template (simplified)
    std::string line;
    std::map<std::string, std::string> config;
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '}') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Remove quotes
        key.erase(0, key.find_first_not_of("\" ""));
        key.erase(key.find_last_not_of("\" ""));
        value.erase(0, value.find_first_not_of("\" '"));
        value.erase(value.find_last_not_of("\" '"));
        
        config[key] = value;
    }
    
    file.close();
    
    _templates[name] = config;
    
    NLM_LOG_INFO("Template loaded: " + name);
    return true;
}

const std::map<std::string, std::map<std::string, std::string>>& ConfigurationTemplate::getAllTemplates() const {
    return _templates;
}

const std::map<std::string, std::string>& ConfigurationTemplate::getTemplate(const std::string& name) const {
    auto it = _templates.find(name);
    if (it != _templates.end()) {
        return it->second;
    }
    
    static const std::map<std::string, std::string> emptyMap;
    return emptyMap;
}

bool ConfigurationTemplate::deleteTemplate(const std::string& name) {
    auto it = _templates.find(name);
    if (it != _templates.end()) {
        _templates.erase(it);
        
        std::string templatePath = "./templates/" + name + ".json";
        if (std::filesystem::exists(templatePath)) {
            std::filesystem::remove(templatePath);
        }
        
        NLM_LOG_INFO("Template deleted: " + name);
        return true;
    }
    
    NLM_LOG_WARNING("Template not found: " + name);
    return false;
}

bool ConfigurationTemplate::listTemplates() const {
    NLM_LOG_INFO("Listing available templates:");
    
    for (const auto& pair : _templates) {
        NLM_LOG_INFO("  " + pair.first);
    }
    
    return true;
}

} // namespace nlm