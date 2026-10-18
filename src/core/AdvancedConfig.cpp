#include "AdvancedConfig.hpp"
#include <fstream>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace nlm {

// ConfigSchema implementation
ConfigSchema::ConfigSchema(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

ConfigSchema::~ConfigSchema() = default;

std::string ConfigSchema::getName() const {
    return name_;
}

std::string ConfigSchema::getDescription() const {
    return description_;
}

void ConfigSchema::addField(const SchemaField& field) {
    fields_.push_back(field);
}

bool ConfigSchema::validate(const nlohmann::json& config) const {
    if (!config.is_object()) return false;
    
    for (const auto& field : fields_) {
        if (field.required && !config.contains(field.name)) {
            return false;
        }
        
        if (config.contains(field.name)) {
            const auto& value = config[field.name];
            if (!value.is_field() && !value.is_null()) {
                return false;
            }
            
            for (const auto& rule : field.validationRules) {
                if (!rule.validator(value)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigSchema::getErrors(const nlohmann::json& config) const {
    std::vector<std::string> errors;
    
    if (!config.is_object()) {
        errors.push_back("Config must be a JSON object");
        return errors;
    }
    
    for (const auto& field : fields_) {
        if (field.required && !config.contains(field.name)) {
            errors.push_back("Missing required field: " + field.name);
        }
        
        if (config.contains(field.name)) {
            const auto& value = config[field.name];
            if (!value.is_field() && !value.is_null()) {
                errors.push_back("Field '" + field.name + "' has invalid type");
            }
            
            for (const auto& rule : field.validationRules) {
                if (!rule.validator(value)) {
                    errors.push_back(rule.errorMessage);
                }
            }
        }
    }
    
    return errors;
}

void ConfigSchema::addInheritance(const ConfigSchema& parent) {
    inheritance_.push_back(&parent);
}

nlohmann::json ConfigSchema::getDefaultConfig() const {
    nlohmann::json config = nlohmann::json::object();
    
    for (const auto& field : fields_) {
        if (field.defaultValue.is_null()) {
            continue;
        }
        
        config[field.name] = field.defaultValue;
    }
    
    return config;
}

void ConfigSchema::clear() {
    fields_.clear();
}

size_t ConfigSchema::getFieldCount() const {
    return fields_.size();
}

std::vector<std::string> ConfigSchema::getFieldNames() const {
    std::vector<std::string> names;
    for (const auto& field : fields_) {
        names.push_back(field.name);
    }
    return names;
}

// ConfigInheritanceManager implementation
ConfigInheritanceManager::ConfigInheritanceManager() = default;
ConfigInheritanceManager::~ConfigInheritanceManager() = default;

void ConfigInheritanceManager::addNode(const InheritanceNode& node) {
    nodes_[node.name] = node;
    initialized_ = true;
}

void ConfigInheritanceManager::removeNode(const std::string& name) {
    nodes_.erase(name);
}

bool ConfigInheritanceManager::hasNode(const std::string& name) const {
    return nodes_.find(name) != nodes_.end();
}

nlohmann::json ConfigInheritanceManager::resolveInheritance(const std::string& name) const {
    if (!hasNode(name)) {
        throw std::runtime_error("Config node not found: " + name);
    }
    
    const auto& node = nodes_.at(name);
    nlohmann::json resolved = node.config;
    
    // Merge parent configs
    for (const auto& parentName : node.parentNames) {
        if (hasNode(parentName)) {
            nlohmann::json parentConfig = resolveInheritance(parentName);
            resolved = mergeConfigs(resolved, parentConfig);
        }
    }
    
    return resolved;
}

std::vector<std::string> ConfigInheritanceManager::getInheritanceChain(
    const std::string& name) const {
    
    std::vector<std::string> chain;
    if (!hasNode(name)) {
        return chain;
    }
    
    const auto& node = nodes_.at(name);
    chain.push_back(name);
    
    for (const auto& parentName : node.parentNames) {
        if (hasNode(parentName)) {
            auto parentChain = getInheritanceChain(parentName);
            chain.insert(chain.end(), parentChain.begin(), parentChain.end());
        }
    }
    
    return chain;
}

nlohmann::json ConfigInheritanceManager::mergeConfigs(
    const std::vector<nlohmann::json>& configs) const {
    
    nlohmann::json result = nlohmann::json::object();
    for (const auto& config : configs) {
        result = mergeConfigs(result, config);
    }
    
    return result;
}

nlohmann::json ConfigInheritanceManager::mergeConfigs(const nlohmann::json& base,
                                                    const nlohmann::json& override) const {
    
    nlohmann::json result = base;
    
    for (const auto& [key, value] : override.items()) {
        if (value.is_object() && result.contains(key) && result[key].is_object()) {
            result[key] = mergeConfigs(result[key], value);
        } else {
            result[key] = value;
        }
    }
    
    return result;
}

void ConfigInheritanceManager::printInheritanceGraph() const {
    std::cout << "Inheritance Graph:\n";
    for (const auto& [name, node] : nodes_) {
        std::cout << "  " << name << " -> {";
        for (size_t i = 0; i < node.parentNames.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << node.parentNames[i];
        }
        std::cout << "}\n";
    }
}

void ConfigInheritanceManager::exportInheritanceGraph(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "InheritanceGraph {\n";
    for (const auto& [name, node] : nodes_) {
        file << "  " << name << " -> {";
        for (size_t i = 0; i < node.parentNames.size(); ++i) {
            if (i > 0) file << ", ";
            file << node.parentNames[i];
        }
        file << "};\n";
    }
    file << "}\n";
    file.close();
}

// ConfigTemplates implementation
ConfigTemplates::ConfigTemplates() {
    defaultSchema_ = std::make_unique<ConfigSchema>("default", "Default configuration schema");
}

ConfigTemplates::~ConfigTemplates() = default;

void ConfigTemplates::addTemplate(const std::string& name, const nlohmann::json& config,
                                 const std::string& description) {
    TemplateInfo info;
    info.config = config;
    info.description = description;
    
    // Validate with schema if available
    if (validateTemplate(name)) {
        info.schema = nlohmann::json::object();
    } else {
        throw std::runtime_error("Template validation failed: " + name);
    }
    
    templates_[name] = info;
}

void ConfigTemplates::removeTemplate(const std::string& name) {
    templates_.erase(name);
}

bool ConfigTemplates::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

nlohmann::json ConfigTemplates::applyTemplate(const std::string& name,
                                             const nlohmann::json& baseConfig) const {
    if (!hasTemplate(name)) {
        throw std::runtime_error("Template not found: " + name);
    }
    
    nlohmann::json result = baseConfig;
    const auto& templateInfo = templates_.at(name);
    
    // Merge template with base config
    for (const auto& [key, value] : templateInfo.config.items()) {
        if (value.is_object() && result.contains(key) && result[key].is_object()) {
            result[key] = mergeConfigs(result[key], value);
        } else {
            result[key] = value;
        }
    }
    
    return result;
}

std::vector<std::string> ConfigTemplates::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

nlohmann::json ConfigTemplates::getTemplate(const std::string& name) const {
    if (!hasTemplate(name)) {
        throw std::runtime_error("Template not found: " + name);
    }
    
    return templates_.at(name).config;
}

nlohmann::json ConfigTemplates::diffTemplates(const std::string& name1,
                                             const std::string& name2) const {
    if (!hasTemplate(name1) || !hasTemplate(name2)) {
        throw std::runtime_error("One or both templates not found");
    }
    
    nlohmann::json diff = nlohmann::json::object();
    const auto& info1 = templates_.at(name1).config;
    const auto& info2 = templates_.at(name2).config;
    
    // Simple diff implementation
    for (const auto& [key, value] : info1.items()) {
        if (!info2.contains(key)) {
            diff["removed"].push_back(key);
        } else if (info2[key] != value) {
            diff["modified"].push_back({{"key", key}, {"old_value", value}, {"new_value", info2[key]}});
        }
    }
    
    for (const auto& [key, value] : info2.items()) {
        if (!info1.contains(key)) {
            diff["added"].push_back({{"key", key}, {"value", value}});
        }
    }
    
    return diff;
}

bool ConfigTemplates::areTemplatesCompatible(const std::string& name1,
                                             const std::string& name2) const {
    // Simple compatibility check - would be more sophisticated in real implementation
    if (!hasTemplate(name1) || !hasTemplate(name2)) {
        return false;
    }
    
    const auto& info1 = templates_.at(name1).config;
    const auto& info2 = templates_.at(name2).config;
    
    // Check for conflicting keys
    for (const auto& [key, value] : info1.items()) {
        if (info2.contains(key) && !(nlohmann::json::accept(value) && nlohmann::json::accept(info2[key]))) {
            return false;
        }
    }
    
    return true;
}

bool ConfigTemplates::validateTemplate(const std::string& name) const {
    if (!hasTemplate(name)) {
        return false;
    }
    
    const auto& config = templates_.at(name).config;
    // Validation would involve schema checking in real implementation
    return true;
}

std::vector<std::string> ConfigTemplates::getTemplateErrors(const std::string& name) const {
    return std::vector<std::string>(); // Placeholder
}

// DynamicConfigManager implementation
DynamicConfigManager::DynamicConfigManager() {
    currentConfig_ = nlohmann::json::object();
}

DynamicConfigManager::~DynamicConfigManager() = default;

void DynamicConfigManager::updateConfig(const std::string& keyPath, const nlohmann::json& value) {
    ConfigChange change;
    change.keyPath = keyPath;
    change.newValue = value;
    
    // Get old value
    nlohmann::json* current = &currentConfig_;
    std::vector<std::string> pathParts;
    
    // Simple path parsing
    size_t pos = 0;
    std::string token;
    while ((pos = keyPath.find('.')) != std::string::npos) {
        token = keyPath.substr(0, pos);
        pathParts.push_back(token);
        keyPath.erase(0, pos + 1);
    }
    pathParts.push_back(keyPath);
    
    for (const auto& part : pathParts) {
        if (current->contains(part)) {
            if (change.oldValue.is_null()) {
                change.oldValue = *current;
            }
            current = &((*current)[part]);
        } else {
            change.oldValue = nullptr;
            break;
        }
    }
    
    // Update the config
    nlohmann::json* updateTarget = &currentConfig_;
    for (size_t i = 0; i < pathParts.size() - 1; ++i) {
        if (!updateTarget->contains(pathParts[i])) {
            (*updateTarget)[pathParts[i]] = nlohmann::json::object();
        }
        updateTarget = &((*updateTarget)[pathParts[i]]);
    }
    
    (*updateTarget)[pathParts.back()] = value;
    
    change.timestamp = std::chrono::high_resolution_clock::now();
    change.source = "DynamicConfigManager";
    
    changes_.push_back(change);
    
    // Update history
    if (historyEnabled_) {
        history_.push_back(currentConfig_);
        if (history_.size() > maxHistorySize_) {
            history_.erase(history_.begin());
        }
    }
    
    // Notify listeners
    for (const auto& listener : changeListeners_) {
        listener(change);
    }
}

void DynamicConfigManager::updateConfig(const nlohmann::json& config) {
    // Merge the entire config
    currentConfig_ = mergeConfigs(currentConfig_, config);
}

void DynamicConfigManager::resetConfig() {
    nlohmann::json emptyConfig = nlohmann::json::object();
    currentConfig_ = emptyConfig;
    
    ConfigChange change;
    change.keyPath = "root";
    change.oldValue = nlohmann::json(nullptr);
    change.newValue = emptyConfig;
    change.timestamp = std::chrono::high_resolution_clock::now();
    change.source = "reset";
    
    changes_.push_back(change);
}

std::vector<DynamicConfigManager::ConfigChange> DynamicConfigManager::getChanges() const {
    return changes_;
}

std::vector<DynamicConfigManager::ConfigChange> DynamicConfigManager::getChangesSince(
    const auto& timestamp) const {
    
    std::vector<ConfigChange> result;
    for (const auto& change : changes_) {
        if (change.timestamp > timestamp) {
            result.push_back(change);
        }
    }
    
    return result;
}

void DynamicConfigManager::createSnapshot(const std::string& name) {
    snapshots_[name] = currentConfig_;
}

nlohmann::json DynamicConfigManager::restoreSnapshot(const std::string& name) {
    if (snapshots_.find(name) == snapshots_.end()) {
        throw std::runtime_error("Snapshot not found: " + name);
    }
    
    nlohmann::json config = snapshots_[name];
    currentConfig_ = config;
    return config;
}

std::vector<std::string> DynamicConfigManager::getSnapshotNames() const {
    std::vector<std::string> names;
    for (const auto& pair : snapshots_) {
        names.push_back(pair.first);
    }
    return names;
}

void DynamicConfigManager::enableHistory(bool enable, size_t maxHistorySize) {
    historyEnabled_ = enable;
    maxHistorySize_ = maxHistorySize;
}

bool DynamicConfigManager::isHistoryEnabled() const {
    return historyEnabled_;
}

std::vector<nlohmann::json> DynamicConfigManager::getHistory() const {
    return history_;
}

void DynamicConfigManager::addChangeListener(std::function<void(const ConfigChange&)> listener) {
    changeListeners_.push_back(listener);
}

void DynamicConfigManager::removeChangeListener(std::function<void(const ConfigChange&)> listener) {
    changeListeners_.erase(
        std::remove(changeListeners_.begin(), changeListeners_.end(), listener),
        changeListeners_.end()
    );
}

// ConfigDiff implementation
ConfigDiff::ConfigDiff() = default;
ConfigDiff::~ConfigDiff() = default;

std::vector<ConfigDiff::DiffResult> ConfigDiff::computeDiff(const nlohmann::json& config1,
                                                          const nlohmann::json& config2) const {
    std::vector<DiffResult> diff;
    
    // Get all keys from both configs
    std::set<std::string> allKeys;
    for (const auto& [key, value] : config1.items()) {
        allKeys.insert(key);
    }
    for (const auto& [key, value] : config2.items()) {
        allKeys.insert(key);
    }
    
    // Compare each key
    for (const auto& key : allKeys) {
        if (!config1.contains(key) && config2.contains(key)) {
            DiffResult result;
            result.operation = "add";
            result.keyPath = key;
            result.newValue = config2[key];
            result.description = "Added key: " + key;
            diff.push_back(result);
        } else if (config1.contains(key) && !config2.contains(key)) {
            DiffResult result;
            result.operation = "remove";
            result.keyPath = key;
            result.oldValue = config1[key];
            result.description = "Removed key: " + key;
            diff.push_back(result);
        } else if (config1[key] != config2[key]) {
            DiffResult result;
            result.operation = "modify";
            result.keyPath = key;
            result.oldValue = config1[key];
            result.newValue = config2[key];
            result.description = "Modified key: " + key;
            diff.push_back(result);
        }
    }
    
    return diff;
}

std::vector<std::string> ConfigDiff::getConfigKeys(const nlohmann::json& config) const {
    std::vector<std::string> keys;
    for (const auto& [key, value] : config.items()) {
        keys.push_back(key);
    }
    return keys;
}

std::string ConfigDiff::getKeyPath(const std::vector<std::string>& path) const {
    std::string result;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) result += ".";
        result += path[i];
    }
    return result;
}

std::string ConfigDiff::formatDiff(const std::vector<DiffResult>& diff) const {
    std::stringstream ss;
    
    for (const auto& result : diff) {
        ss << getOperationSymbol(result.operation) << " ";
        ss << result.keyPath;
        
        if (result.operation == "add") {
            ss << " = " << result.newValue.dump();
        } else if (result.operation == "remove") {
            ss << " (was " << result.oldValue.dump() << ")";
        } else if (result.operation == "modify") {
            ss << "\n  from: " << result.oldValue.dump() << "\n";
            ss << "  to:   " << result.newValue.dump();
        }
        
        ss << " - " << result.description << "\n";
    }
    
    return ss.str();
}

nlohmann::json ConfigDiff::serializeDiff(const std::vector<DiffResult>& diff) const {
    nlohmann::json result = nlohmann::json::array();
    
    for (const auto& item : diff) {
        nlohmann::json jsonItem;
        jsonItem["operation"] = item.operation;
        jsonItem["keyPath"] = item.keyPath;
        if (!item.oldValue.is_null()) jsonItem["oldValue"] = item.oldValue;
        if (!item.newValue.is_null()) jsonItem["newValue"] = item.newValue;
        jsonItem["description"] = item.description;
        result.push_back(jsonItem);
    }
    
    return result;
}

size_t ConfigDiff::countChanges(const std::vector<DiffResult>& diff) const {
    return diff.size();
}

size_t ConfigDiff::countAdds(const std::vector<DiffResult>& diff) const {
    size_t count = 0;
    for (const auto& result : diff) {
        if (result.operation == "add") count++;
    }
    return count;
}

size_t ConfigDiff::countRemoves(const std::vector<DiffResult>& diff) const {
    size_t count = 0;
    for (const auto& result : diff) {
        if (result.operation == "remove") count++;
    }
    return count;
}

size_t ConfigDiff::countModifications(const std::vector<DiffResult>& diff) const {
    size_t count = 0;
    for (const auto& result : diff) {
        if (result.operation == "modify") count++;
    }
    return count;
}

nlohmann::json ConfigDiff::analyzeMergeConflict(const std::vector<DiffResult>& diff) const {
    nlohmann::json conflict = nlohmann::json::object();
    conflict["hasConflicts"] = false;
    conflict["conflictingKeys"] = nlohmann::json::array();
    
    for (const auto& result : diff) {
        if (result.operation == "modify") {
            // Simple conflict detection - would be more sophisticated
            conflict["hasConflicts"] = true;
            conflict["conflictingKeys"].push_back(result.keyPath);
        }
    }
    
    return conflict;
}

bool ConfigDiff::canMerge(const std::vector<DiffResult>& diff) const {
    return !analyzeMergeConflict(diff)["hasConflicts"]; // .get<bool>();
}

std::string ConfigDiff::normalizeKey(const std::string& key) {
    std::string result = key;
    // Remove special characters, etc.
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](char c) { return !std::isalnum(c) && c != '.' && c != '_'; }),
                 result.end());
    return result;
}

std::string ConfigDiff::getOperationSymbol(const std::string& operation) {
    if (operation == "add") return "+";
    if (operation == "remove") return "-";
    if (operation == "modify") return "*";
    return "?";
}

// ConfigDocumentation implementation
ConfigDocumentation::ConfigDocumentation() {
    initialized_ = true;
}

ConfigDocumentation::~ConfigDocumentation() = default;

std::string ConfigDocumentation::generateMarkdown(const nlohmann::json& config) const {
    std::stringstream ss;
    
    ss << "# Configuration Documentation\n\n";
    ss << "## Configuration Overview\n";
    ss << "```json\n" << config.dump(2) << "\n```\n\n";
    
    // Generate section for each key
    for (const auto& [key, value] : config.items()) {
        ss << "## " << key << "\n";
        ss << "- Type: " << value.type_name() << "\n";
        ss << "- Value: " << value.dump() << "\n\n";
    }
    
    return ss.str();
}

std::string ConfigDocumentation::generateHTML(const nlohmann::json& config) const {
    std::stringstream ss;
    
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Configuration Documentation</title></head>\n";
    ss << "<body>\n<h1>Configuration Documentation</h1>\n";
    ss << "<table border=\"1\">\n<tr><th>Key</th><th>Type</th><th>Value</th></tr>\n";
    
    for (const auto& [key, value] : config.items()) {
        ss << "<tr><td>" << key << "</td><td>" << value.type_name() 
           << "</td><td>" << value.dump() << "</td></tr>\n";
    }
    
    ss << "</table>\n</body>\n</html>";
    
    return ss.str();
}

std::string ConfigDocumentation::generateJSONSchema(const nlohmann::json& config) const {
    nlohmann::json schema = nlohmann::json::object();
    schema["$schema"] = "http://json-schema.org/draft-07/schema#";
    schema["title"] = "Configuration Schema";
    schema["type"] = "object";
    
    nlohmann::json properties = nlohmann::json::object();
    for (const auto& [key, value] : config.items()) {
        nlohmann::json property;
        property["type"] = std::string(value.type_name());
        if (!value.is_null()) {
            property["default"] = value;
        }
        properties[key] = property;
    }
    
    schema["properties"] = properties;
    return schema.dump(2);
}

std::string ConfigDocumentation::generateTypeScriptInterface(const nlohmann::json& config) const {
    std::stringstream ss;
    
    ss << "export interface Configuration {\n";
    
    for (const auto& [key, value] : config.items()) {
        ss << "  " << key << ": ";
        
        if (value.is_string()) {
            ss << "string;\n";
        } else if (value.is_number()) {
            if (value.is_number_integer()) {
                ss << "number;\n";
            } else {
                ss << "number;\n";
            }
        } else if (value.is_boolean()) {
            ss << "boolean;\n";
        } else if (value.is_array()) {
            ss << "any[];\n";
        } else if (value.is_object()) {
            ss << "any;\n";
        } else {
            ss << "any;\n";
        }
    }
    
    ss << "}\n";
    
    return ss.str();
}

void ConfigDocumentation::addSection(const DocumentationSection& section) {
    documentation_.push_back(section);
}

std::vector<ConfigDocumentation::DocumentationSection> ConfigDocumentation::getDocumentation() const {
    return documentation_;
}

nlohmann::json ConfigDocumentation::generateExamples(const std::string& configName) const {
    nlohmann::json examples = nlohmann::json::array();
    
    nlohmann::json example1 = nlohmann::json::object();
    example1["name"] = "Basic " + configName;
    example1["config"] = nlohmann::json::object();
    example1["description"] = "Basic configuration for " + configName;
    examples.push_back(example1);
    
    nlohmann::json example2 = nlohmann::json::object();
    example2["name"] = "Advanced " + configName;
    example2["config"] = nlohmann::json::object();
    example2["description"] = "Advanced configuration for " + configName;
    examples.push_back(example2);
    
    return examples;
}

std::string ConfigDocumentation::generateExampleMarkdown(const nlohmann::json& example) const {
    std::stringstream ss;
    
    ss << "### " << example["name"] << "\n";
    ss << example["description"] << "\n\n";
    ss << "```json\n" << example["config"].dump(2) << "\n```\n";
    
    return ss.str();
}

bool ConfigDocumentation::validateDocumentation() const {
    return initialized_;
}

std::vector<std::string> ConfigDocumentation::getDocumentationErrors() const {
    std::vector<std::string> errors;
    if (!initialized_) {
        errors.push_back("Documentation not initialized");
    }
    return errors;
}

// AdvancedConfig implementation
struct AdvancedConfig::Impl {
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, ConfigTemplates::TemplateInfo> templates;
    ConfigInheritanceManager inheritanceManager;
    DynamicConfigManager dynamicConfig;
    ConfigDiff diffProcessor;
    ConfigDocumentation docGenerator;
    
    std::map<std::string, nlohmann::json> cache;
    bool cachingEnabled = false;
    size_t cacheSize = 100;
    
    ConfigStats stats;
    std::chrono::steady_clock::time_point lastUpdate;
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {
    pImpl->stats.lastModified = std::chrono::high_resolution_clock::now();
}

AdvancedConfig::~AdvancedConfig() = default;

void AdvancedConfig::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filename);
    }
    
    nlohmann::json config;
    file >> config;
    loadConfig(config);
    
    pImpl->stats.lastModified = std::chrono::high_resolution_clock::now();
    pImpl->stats.totalConfigs++;
}

void AdvancedConfig::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file for writing: " + filename);
    }
    
    nlohmann::json config = exportConfig();
    file << std::setw(2) << config;
}

void AdvancedConfig::loadConfig(const nlohmann::json& config) {
    // Validate against schemas
    bool isValid = true;
    std::vector<std::string> errors;
    
    // Clear cache
    if (pImpl->cachingEnabled) {
        pImpl->cache.clear();
    }
    
    // Process config
    for (const auto& [key, schema] : pImpl->schemas) {
        if (config.contains(key)) {
            if (!schema.validate(config[key])) {
                auto validationErrors = schema.getErrors(config[key]);
                errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
            }
        }
    }
    
    if (!errors.empty()) {
        pImpl->stats.invalidConfigs++;
        throw std::runtime_error("Config validation failed");
    }
    
    // Set dynamic config
    pImpl->dynamicConfig.updateConfig(config);
    
    pImpl->stats.lastModified = std::chrono::high_resolution_clock::now();
    if (isValid) {
        pImpl->stats.validConfigs++;
    }
}

nlohmann::json AdvancedConfig::exportConfig() const {
    return pImpl->dynamicConfig.getCachedValue("/");
}

void AdvancedConfig::registerSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.getName()] = schema;
}

void AdvancedConfig::unregisterSchema(const std::string& name) {
    pImpl->schemas.erase(name);
}

bool AdvancedConfig::hasSchema(const std::string& name) const {
    return pImpl->schemas.find(name) != pImpl->schemas.end();
}

void AdvancedConfig::setupInheritance(const std::string& child, const std::string& parent) {
    pImpl->inheritanceManager.addNode(
        ConfigInheritanceManager::InheritanceNode{child, nlohmann::json::object(), {parent}, 1.0f}
    );
}

void AdvancedConfig::removeInheritance(const std::string& child) {
    pImpl->inheritanceManager.removeNode(child);
}

std::vector<std::string> AdvancedConfig::getInheritanceChain(const std::string& name) const {
    return pImpl->inheritanceManager.getInheritanceChain(name);
}

void AdvancedConfig::registerTemplate(const std::string& name, const nlohmann::json& config,
                                     const std::string& description) {
    pImpl->templates[name] = {
        config,
        description,
        nlohmann::json::object() // Placeholder schema
    };
}

nlohmann::json AdvancedConfig::applyTemplate(const std::string& name) const {
    return pImpl->templates.at(name).config;
}

void AdvancedConfig::enableDynamicUpdates(bool enable) {
    pImpl->dynamicConfig.enableHistory(enable);
}

void AdvancedConfig::updateConfig(const std::string& keyPath, const nlohmann::json& value) {
    pImpl->dynamicConfig.updateConfig(keyPath, value);
}

nlohmann::json AdvancedConfig::getConfig(const std::string& keyPath) const {
    return pImpl->dynamicConfig.getCachedValue(keyPath);
}

bool AdvancedConfig::validateConfig() const {
    nlohmann::json config = exportConfig();
    
    bool isValid = true;
    for (const auto& [key, schema] : pImpl->schemas) {
        if (config.contains(key)) {
            if (!schema.validate(config[key])) {
                isValid = false;
                break;
            }
        }
    }
    
    return isValid;
}

std::vector<std::string> AdvancedConfig::getConfigErrors() const {
    std::vector<std::string> errors;
    nlohmann::json config = exportConfig();
    
    for (const auto& [key, schema] : pImpl->schemas) {
        if (config.contains(key)) {
            auto schemaErrors = schema.getErrors(config[key]);
            errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
        }
    }
    
    return errors;
}

std::vector<ConfigDiff::DiffResult> AdvancedConfig::diffConfig(const nlohmann::json& other) const {
    return pImpl->diffProcessor.computeDiff(exportConfig(), other);
}

nlohmann::json AdvancedConfig::mergeConfig(const nlohmann::json& other) const {
    return pImpl->inheritanceManager.mergeConfigs(exportConfig(), other);
}

void AdvancedConfig::enableDocumentation(bool enable) {
    // Documentation enable/disable logic
}

std::string AdvancedConfig::generateDocumentation() const {
    return pImpl->docGenerator.generateMarkdown(exportConfig());
}

AdvancedConfig::ConfigStats AdvancedConfig::getStats() const {
    return pImpl->stats;
}

void AdvancedConfig::enableCaching(bool enable, size_t cacheSize) {
    pImpl->cachingEnabled = enable;
    pImpl->cacheSize = cacheSize;
}

void AdvancedConfig::clearCache() {
    pImpl->cache.clear();
}

bool AdvancedConfig::isCached(const std::string& key) const {
    return pImpl->cache.find(key) != pImpl->cache.end();
}

nlohmann::json AdvancedConfig::getCachedValue(const std::string& key) const {
    if (isCached(key)) {
        return pImpl->cache[key];
    }
    return nlohmann::json(nullptr);
}

} // namespace nlm
