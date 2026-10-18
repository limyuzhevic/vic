#include "Management.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace nlm {

// ConfigMigrator implementations
ConfigMigrator::ConfigMigrator(const std::string& from_version, const std::string& to_version)
    : from_version(from_version), to_version(to_version) {}

void ConfigMigrator::addMigrationStep(const std::string& version,
                                      const std::function<void(Config&)>& migration_function,
                                      const std::string& description) {
    MigrationStep step;
    step.version = version;
    step.migration_function = migration_function;
    step.description = description;
    steps.push_back(step);
}

bool ConfigMigrator::migrate(Config& config) const {
    for (const auto& step : steps) {
        try {
            step.migration_function(config);
        } catch (const std::exception& e) {
            throw ConfigError("Migration step " + step.version + " failed: " + e.what());
        }
    }
    return true;
}

std::vector<std::string> ConfigMigrator::getMigrationSteps() const {
    std::vector<std::string> versions;
    for (const auto& step : steps) {
        versions.push_back(step.version);
    }
    return versions;
}

// ConfigComparator implementations
ConfigComparator::ConfigComparator(const std::string& comparison_type)
    : comparison_type(comparison_type) {}

std::vector<ConfigComparator::ComparisonResult> ConfigComparator::compare(const Config& old_config, const Config& new_config) const {
    std::vector<ComparisonResult> results;
    
    // Check old config keys
    auto old_keys = old_config.getKeys();
    for (const auto& key : old_keys) {
        ComparisonResult result;
        result.key = key;
        result.old_value = ConfigValue(); // We need to get the actual value
        result.source = ConfigSource::Default;
        results.push_back(result);
    }
    
    // Check new config keys
    auto new_keys = new_config.getKeys();
    for (const auto& key : new_keys) {
        bool found = false;
        for (const auto& result : results) {
            if (result.key == key) {
                found = true;
                break;
            }
        }
        if (!found) {
            ComparisonResult result;
            result.key = key;
            result.new_value = ConfigValue(); // We need to get the actual value
            result.source = ConfigSource::Default;
            results.push_back(result);
        }
    }
    
    return results;
}

double ConfigComparator::getSimilarityScore(const Config& old_config, const Config& new_config) const {
    auto old_keys = old_config.getKeys();
    auto new_keys = new_config.getKeys();
    
    if (old_keys.empty() && new_keys.empty()) {
        return 1.0;
    }
    
    int common_keys = 0;
    for (const auto& key : old_keys) {
        if (std::find(new_keys.begin(), new_keys.end(), key) != new_keys.end()) {
            common_keys++;
        }
    }
    
    int total_keys = std::max(old_keys.size(), new_keys.size());
    return static_cast<double>(common_keys) / total_keys;
}

std::vector<std::string> ConfigComparator::getChangedKeys(const Config& old_config, const Config& new_config) const {
    std::vector<std::string> changed_keys;
    auto old_keys = old_config.getKeys();
    
    for (const auto& key : old_keys) {
        auto old_val = old_config.getHierarchical(key);
        auto new_val = new_config.getHierarchical(key);
        
        if (old_val.has_value() && new_val.has_value() && old_val.value() != new_val.value()) {
            changed_keys.push_back(key);
        } else if (old_val.has_value() && !new_val.has_value()) {
            changed_keys.push_back(key);
        }
    }
    
    return changed_keys;
}

std::vector<std::string> ConfigComparator::getNewKeys(const Config& old_config, const Config& new_config) const {
    std::vector<std::string> new_keys;
    auto old_keys = old_config.getKeys();
    auto new_keys_all = new_config.getKeys();
    
    for (const auto& key : new_keys_all) {
        if (std::find(old_keys.begin(), old_keys.end(), key) == old_keys.end()) {
            new_keys.push_back(key);
        }
    }
    
    return new_keys;
}

std::vector<std::string> ConfigComparator::getRemovedKeys(const Config& old_config, const Config& new_config) const {
    std::vector<std::string> removed_keys;
    auto new_keys = new_config.getKeys();
    auto old_keys_all = old_config.getKeys();
    
    for (const auto& key : old_keys_all) {
        if (std::find(new_keys.begin(), new_keys.end(), key) == new_keys.end()) {
            removed_keys.push_back(key);
        }
    }
    
    return removed_keys;
}

// ConfigValidationReport implementations
void ConfigValidationReport::addIssue(const std::string& key, const std::string& message,
                                      Issue::Severity severity, const std::string& recommendation) {
    issues.emplace_back(key, message, severity, recommendation);
    
    if (severity == Issue::Severity::Error || severity == Issue::Severity::Critical) {
        errors.push_back(message);
    } else if (severity == Issue::Severity::Warning) {
        warnings.push_back(message);
    }
}

void ConfigValidationReport::addWarning(const std::string& message) {
    warnings.push_back(message);
}

void ConfigValidationReport::addError(const std::string& message) {
    errors.push_back(message);
}

std::string ConfigValidationReport::generateReport() const {
    std::ostringstream oss;
    oss << "Configuration Validation Report: " << name << "\n";
    oss << "Generated at: " << getCurrentTimestamp() << "\n";
    oss << "Valid: " << (is_valid() ? "Yes" : "No") << "\n";
    oss << "Issues found: " << issues.size() << "\n";
    oss << "Warnings: " << warnings.size() << "\n";
    oss << "Errors: " << errors.size() << "\n\n";
    
    for (const auto& issue : issues) {
        oss << "[" << static_cast<int>(issue.severity) << "] " << issue.key << ": " << issue.message << "\n";
        if (!issue.recommendation.empty()) {
            oss << "    Recommendation: " << issue.recommendation << "\n";
        }
    }
    
    return oss.str();
}

std::string ConfigValidationReport::generateJSONReport() const {
    nlohmann::json j;
    j["name"] = name;
    j["valid"] = is_valid();
    j["timestamp"] = getCurrentTimestamp();
    j["issue_count"] = issues.size();
    j["warning_count"] = warnings.size();
    j["error_count"] = errors.size();
    
    nlohmann::json issues_array = nlohmann::json::array();
    for (const auto& issue : issues) {
        nlohmann::json issue_obj;
        issue_obj["key"] = issue.key;
        issue_obj["message"] = issue.message;
        issue_obj["severity"] = static_cast<int>(issue.severity);
        issue_obj["recommendation"] = issue.recommendation;
        issues_array.push_back(issue_obj);
    }
    j["issues"] = issues_array;
    
    return j.dump(2);
}

std::string ConfigValidationReport::generateHTMLReport() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html>\n<head><title>Configuration Validation Report: " << name << "</title></head>\n";
    oss << "<body>\n<h1>Configuration Validation Report: " << name << "</h1>\n";
    oss << "<p><strong>Generated at:</strong> " << getCurrentTimestamp() << "</p>\n";
    oss << "<p><strong>Valid:</strong> " << (is_valid() ? "Yes" : "No") << "</p>\n";
    oss << "<p><strong>Issues found:</strong> " << issues.size() << "</p>\n";
    oss << "<p><strong>Warnings:</strong> " << warnings.size() << "</p>\n";
    oss << "<p><strong>Errors:</strong> " << errors.size() << "</p>\n";
    oss << "<h2>Issues</h2>\n<ul>\n";
    
    for (const auto& issue : issues) {
        oss << "<li><strong>" << issue.key << "</strong>: " << issue.message << " (Severity: " << static_cast<int>(issue.severity) << ")";
        if (!issue.recommendation.empty()) {
            oss << "<br><em>Recommendation: " << issue.recommendation << "</em>";
        }
        oss << "</li>\n";
    }
    
    oss << "</ul>\n</body>\n</html>";
    return oss.str();
}

std::string ConfigValidationReport::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ConfigTemplateManager implementations
ConfigTemplateManager::ConfigTemplateManager() {
    // Initialize built-in templates
}

std::vector<ConfigTemplateManager::Template> ConfigTemplateManager::getBuiltInTemplates() const {
    std::vector<Template> templates;
    
    // Development template
    Template dev_template("development", "Development configuration for testing and debugging");
    dev_template.defaults = {
        {"simulation_timestep", 0.001},
        {"log_level", std::string("DEBUG")},
        {"enable_debug_mode", true},
        {"enable_validation", true},
        {"performance_monitoring", true},
        {"checkpoint_interval", 1000LL},
        {"max_simulation_steps", 10000LL},
        {"neuron_count", 500},
        {"environment_name", std::string("GridWorld")}
    };
    templates.push_back(dev_template);
    
    // Testing template
    Template test_template("testing", "Testing configuration for automated tests");
    test_template.defaults = {
        {"simulation_timestep", 0.01},
        {"log_level", std::string("WARN")},
        {"enable_debug_mode", false},
        {"enable_validation", true},
        {"performance_monitoring", false},
        {"checkpoint_interval", 10000LL},
        {"max_simulation_steps", 1000LL},
        {"neuron_count", 100},
        {"random_seed", 12345LL}
    };
    templates.push_back(test_template);
    
    // Production template
    Template prod_template("production", "Production configuration for deployment");
    prod_template.defaults = {
        {"simulation_timestep", 0.001},
        {"log_level", std::string("INFO")},
        {"enable_debug_mode", false},
        {"enable_validation", true},
        {"performance_monitoring", true},
        {"checkpoint_interval", 100000LL},
        {"max_simulation_steps", 1000000LL},
        {"neuron_count", 10000},
        {"enable_parallel_processing", true},
        {"enable_memory_pools", true}
    };
    templates.push_back(prod_template);
    
    // Minimal template
    Template minimal_template("minimal", "Minimal configuration with only essential settings");
    minimal_template.defaults = {
        {"simulation_timestep", 0.001},
        {"neuron_count", 1000},
        {"environment_name", std::string("GridWorld")}
    };
    templates.push_back(minimal_template);
    
    // Optimized template
    Template optimized_template("optimized", "Optimized configuration for performance");
    optimized_template.defaults = {
        {"simulation_timestep", 0.001},
        {"neuron_count", 1000},
        {"enable_parallel_processing", true},
        {"enable_memory_pools", true},
        {"enable_simd", true},
        {"log_level", std::string("ERROR")},
        {"enable_validation", false},
        {"checkpoint_interval", 0LL}
    };
    templates.push_back(optimized_template);
    
    return templates;
}

Template ConfigTemplateManager::loadTemplate(const std::string& name) const {
    auto templates = getBuiltInTemplates();
    for (const auto& template : templates) {
        if (template.name == name) {
            return template;
        }
    }
    throw ConfigError("Template not found: " + name);
}

bool ConfigTemplateManager::saveTemplate(const Template& template) {
    if (template.name.empty()) {
        return false;
    }
    // Save template to file or database
    return true;
}

bool ConfigTemplateManager::deleteTemplate(const std::string& name) {
    // Implementation for deleting template
    return true;
}

std::vector<std::string> ConfigTemplateManager::getAvailableTemplates() const {
    auto templates = getBuiltInTemplates();
    std::vector<std::string> names;
    for (const auto& template : templates) {
        names.push_back(template.name);
    }
    return names;
}

Config ConfigTemplateManager::loadDevelopmentTemplate(const Config& base_config) {
    Config config = base_config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("log_level", std::string("DEBUG"), ConfigSource::File);
    config.set("enable_debug_mode", true, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", true, ConfigSource::File);
    config.set("checkpoint_interval", 1000LL, ConfigSource::File);
    config.set("max_simulation_steps", 10000LL, ConfigSource::File);
    config.set("neuron_count", 500, ConfigSource::File);
    config.set("environment_name", std::string("GridWorld"), ConfigSource::File);
    return config;
}

Config ConfigTemplateManager::loadTestingTemplate(const Config& base_config) {
    Config config = base_config;
    config.set("simulation_timestep", 0.01, ConfigSource::File);
    config.set("log_level", std::string("WARN"), ConfigSource::File);
    config.set("enable_debug_mode", false, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", false, ConfigSource::File);
    config.set("checkpoint_interval", 10000LL, ConfigSource::File);
    config.set("max_simulation_steps", 1000LL, ConfigSource::File);
    config.set("neuron_count", 100, ConfigSource::File);
    config.set("random_seed", 12345LL, ConfigSource::File);
    return config;
}

Config ConfigTemplateManager::loadProductionTemplate(const Config& base_config) {
    Config config = base_config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("log_level", std::string("INFO"), ConfigSource::File);
    config.set("enable_debug_mode", false, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", true, ConfigSource::File);
    config.set("checkpoint_interval", 100000LL, ConfigSource::File);
    config.set("max_simulation_steps", 1000000LL, ConfigSource::File);
    config.set("neuron_count", 10000, ConfigSource::File);
    config.set("enable_parallel_processing", true, ConfigSource::File);
    config.set("enable_memory_pools", true, ConfigSource::File);
    return config;
}

Config ConfigTemplateManager::loadMinimalTemplate() {
    Config config;
    config.loadFromDefaults();
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("neuron_count", 1000, ConfigSource::File);
    config.set("environment_name", std::string("GridWorld"), ConfigSource::File);
    return config;
}

Config ConfigTemplateManager::loadOptimizedTemplate() {
    Config config;
    config.loadFromDefaults();
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("neuron_count", 1000, ConfigSource::File);
    config.set("enable_parallel_processing", true, ConfigSource::File);
    config.set("enable_memory_pools", true, ConfigSource::File);
    config.set("enable_simd", true, ConfigSource::File);
    config.set("log_level", std::string("ERROR"), ConfigSource::File);
    config.set("enable_validation", false, ConfigSource::File);
    config.set("checkpoint_interval", 0LL, ConfigSource::File);
    return config;
}

// ConfigHistoryManager implementations
ConfigHistoryManager::ConfigHistoryManager(const std::string& history_file) : history_file(history_file) {}

void ConfigHistoryManager::saveSnapshot(const Config& config, const std::string& version,
                                       const std::map<std::string, std::string>& metadata) {
    ConfigSnapshot snapshot(config, version);
    snapshot.metadata = metadata;
    
    snapshots.push_back(snapshot);
    
    if (snapshots.size() > max_history_size) {
        snapshots.erase(snapshots.begin());
    }
    
    if (auto_save) {
        saveHistory();
    }
}

ConfigHistoryManager::ConfigSnapshot ConfigHistoryManager::getSnapshot(size_t index) const {
    if (index < snapshots.size()) {
        return snapshots[index];
    }
    throw ConfigError("Snapshot index out of range: " + std::to_string(index));
}

ConfigHistoryManager::ConfigSnapshot ConfigHistoryManager::getLatestSnapshot() const {
    if (!snapshots.empty()) {
        return snapshots.back();
    }
    throw ConfigError("No snapshots available");
}

Config ConfigHistoryManager::rollbackToSnapshot(size_t index) {
    if (index >= snapshots.size()) {
        throw ConfigError("Snapshot index out of range: " + std::to_string(index));
    }
    return snapshots[index].config;
}

Config ConfigHistoryManager::getDiffBetweenSnapshots(size_t index1, size_t index2) const {
    if (index1 >= snapshots.size() || index2 >= snapshots.size()) {
        throw ConfigError("Snapshot index out of range");
    }
    
    return snapshots[index1].config.diff(snapshots[index2].config);
}

void ConfigHistoryManager::saveHistory() const {
    // Save snapshots to file
    std::ofstream file(history_file);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot save history to file: " + history_file);
    }
    
    nlohmann::json j;
    j["snapshots"] = nlohmann::json::array();
    
    for (const auto& snapshot : snapshots) {
        nlohmann::json snapshot_json;
        snapshot_json["timestamp"] = snapshot.timestamp;
        snapshot_json["version"] = snapshot.version;
        snapshot_json["metadata"] = snapshot.metadata;
        
        // Convert config to JSON
        nlohmann::json config_json;
        for (const auto& entry : snapshot.config.getKeys()) {
            auto value = snapshot.config.getHierarchical(entry);
            if (value.has_value()) {
                config_json[entry] = value.value();
            }
        }
        
        snapshot_json["config"] = config_json;
        j["snapshots"].push_back(snapshot_json);
    }
    
    file << j.dump(2);
}

bool ConfigHistoryManager::loadHistory() {
    if (!std::filesystem::exists(history_file)) {
        return false;
    }
    
    std::ifstream file(history_file);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        throw ConfigParseError("Failed to parse history file: " + std::string(e.what()));
    }
    
    snapshots.clear();
    
    if (!j.contains("snapshots") || !j["snapshots"].is_array()) {
        return false;
    }
    
    for (const auto& snapshot_json : j["snapshots"]) {
        ConfigSnapshot snapshot(Config(), snapshot_json.value("version", "1.0"));
        snapshot.timestamp = snapshot_json.value("timestamp", "");
        snapshot.metadata = snapshot_json.value("metadata", nlohmann::json::object());
        
        if (snapshot_json.contains("config") && snapshot_json["config"].is_object()) {
            // Load config from JSON
            for (const auto& pair : snapshot_json["config"].items()) {
                // Convert JSON value to ConfigValue
                ConfigValue value;
                if (pair.value().is_number_integer()) {
                    value = pair.value().get<int64_t>();
                } else if (pair.value().is_number_float()) {
                    value = pair.value().get<double>();
                } else if (pair.value().is_boolean()) {
                    value = pair.value().get<bool>();
                } else if (pair.value().is_string()) {
                    value = pair.value().get<std::string>();
                } else if (pair.value().is_array()) {
                    // Handle arrays
                    std::vector<std::string> str_array;
                    for (const auto& item : pair.value()) {
                        if (item.is_string()) {
                            str_array.push_back(item.get<std::string>());
                        } else {
                            str_array.push_back(item.dump());
                        }
                    }
                    value = str_array;
                }
                
                snapshot.config.set(pair.key(), value, ConfigSource::File);
            }
        }
        
        snapshots.push_back(snapshot);
    }
    
    return true;
}

void ConfigHistoryManager::clearHistory() {
    snapshots.clear();
    if (std::filesystem::exists(history_file)) {
        std::filesystem::remove(history_file);
    }
}

// ConfigMonitor implementations
void ConfigMonitor::addRule(const std::string& key_pattern,
                            const std::function<bool(const ConfigValue&)>& condition,
                            const std::function<void(const ConfigValue&, ConfigSource)>& callback,
                            const std::string& description) {
    MonitorRule rule;
    rule.key_pattern = key_pattern;
    rule.condition = condition;
    rule.callback = callback;
    rule.description = description;
    rules.push_back(rule);
}

void ConfigMonitor::checkConfig(const Config& config) const {
    if (!active) {
        return;
    }
    
    for (const auto& rule : rules) {
        auto value = config.getHierarchical(rule.key_pattern);
        if (value.has_value() && rule.condition(value.value())) {
            rule.callback(value.value(), ConfigSource::Runtime);
        }
    }
}

void ConfigMonitor::registerConfig(Config& config) {
    // Implementation for monitoring config changes
}

void ConfigMonitor::unregisterConfig() {
    // Implementation for stopping monitoring
}

// ConfigFactory implementations
bool ConfigFactory::addRecipe(const FactoryConfig& recipe) {
    if (recipe.name.empty()) {
        return false;
    }
    
    recipes.push_back(recipe);
    return true;
}

bool ConfigFactory::removeRecipe(const std::string& recipe_name) {
    auto it = std::find_if(recipes.begin(), recipes.end(),
        [&recipe_name](const FactoryConfig& r) { return r.name == recipe_name; });
    
    if (it != recipes.end()) {
        recipes.erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::string> ConfigFactory::getAvailableRecipes() const {
    std::vector<std::string> names;
    for (const auto& recipe : recipes) {
        names.push_back(recipe.name);
    }
    return names;
}

ConfigFactory::FactoryConfig ConfigFactory::getRecipe(const std::string& recipe_name) const {
    auto it = std::find_if(recipes.begin(), recipes.end(),
        [&recipe_name](const FactoryConfig& r) { return r.name == recipe_name; });
    
    if (it != recipes.end()) {
        return *it;
    }
    
    throw ConfigError("Recipe not found: " + recipe_name);
}

Config ConfigFactory::create(const std::string& recipe_name) const {
    auto recipe = getRecipe(recipe_name);
    Config config;
    
    for (const auto& pair : recipe.defaults) {
        config.set(pair.first, pair.second, ConfigSource::File);
    }
    
    return config;
}

// Common factory methods
Config ConfigFactory::createDevelopmentConfig() {
    Config config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("log_level", std::string("DEBUG"), ConfigSource::File);
    config.set("enable_debug_mode", true, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", true, ConfigSource::File);
    config.set("neuron_count", 500, ConfigSource::File);
    config.set("environment_name", std::string("GridWorld"), ConfigSource::File);
    return config;
}

Config ConfigFactory::createTestingConfig() {
    Config config;
    config.set("simulation_timestep", 0.01, ConfigSource::File);
    config.set("log_level", std::string("WARN"), ConfigSource::File);
    config.set("enable_debug_mode", false, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", false, ConfigSource::File);
    config.set("neuron_count", 100, ConfigSource::File);
    config.set("random_seed", 12345LL, ConfigSource::File);
    return config;
}

Config ConfigFactory::createProductionConfig() {
    Config config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("log_level", std::string("INFO"), ConfigSource::File);
    config.set("enable_debug_mode", false, ConfigSource::File);
    config.set("enable_validation", true, ConfigSource::File);
    config.set("performance_monitoring", true, ConfigSource::File);
    config.set("neuron_count", 10000, ConfigSource::File);
    config.set("enable_parallel_processing", true, ConfigSource::File);
    config.set("environment_name", std::string("GridWorld"), ConfigSource::File);
    return config;
}

Config ConfigFactory::createMinimalConfig() {
    Config config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("neuron_count", 1000, ConfigSource::File);
    config.set("environment_name", std::string("GridWorld"), ConfigSource::File);
    return config;
}

Config ConfigFactory::createOptimizedConfig() {
    Config config;
    config.set("simulation_timestep", 0.001, ConfigSource::File);
    config.set("neuron_count", 1000, ConfigSource::File);
    config.set("enable_parallel_processing", true, ConfigSource::File);
    config.set("enable_memory_pools", true, ConfigSource::File);
    config.set("enable_simd", true, ConfigSource::File);
    config.set("log_level", std::string("ERROR"), ConfigSource::File);
    config.set("enable_validation", false, ConfigSource::File);
    return config;
}

// ConfigExporter implementations
std::string ConfigExporter::exportConfig(const Config& config) const {
    switch (format) {
        case ExportFormat::Text:
            return exportToText(config);
        case ExportFormat::JSON:
            return exportToJSON(config);
        case ExportFormat::YAML:
            return exportToYAML(config);
        case ExportFormat::TOML:
            return exportToTOML(config);
        case ExportFormat::XML:
            return exportToXML(config);
        case ExportFormat::CSV:
            return exportToCSV(config);
        case ExportFormat::SQL:
            return exportToSQL(config);
        default:
            return exportToText(config);
    }
}

bool ConfigExporter::exportToFile(const Config& config, const std::string& filepath) const {
    std::string content = exportConfig(config);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return true;
}

std::string ConfigExporter::exportToText(const Config& config) const {
    std::ostringstream oss;
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            oss << key << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, value.value());
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string ConfigExporter::exportToJSON(const Config& config) const {
    nlohmann::json j;
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            // Convert to appropriate JSON type
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[key] = arg;
                }
            }, value.value());
        }
    }
    
    if (pretty_print) {
        return j.dump(2);
    } else {
        return j.dump();
    }
}

std::string ConfigExporter::exportToYAML(const Config& config) const {
    YAML::Emitter out;
    if (pretty_print) {
        out.SetIndent(2);
    }
    
    out << YAML::BeginMap;
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            out << YAML::Key << key;
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    out << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t> ||
                                     std::is_same_v<T, double> ||
                                     std::is_same_v<T, bool>) {
                    out << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    out << YAML::Value << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    out << YAML::Value << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    out << YAML::Value << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                }
            }, value.value());
        }
    }
    
    out << YAML::EndMap;
    return out.c_str();
}

std::string ConfigExporter::exportToTOML(const Config& config) const {
    toml::value v;
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            // Convert to appropriate TOML type
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    v[key] = arg;
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t>) {
                    v[key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    v[key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    v[key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    v[key] = toml::array(arg.begin(), arg.end());
                }
            }, value.value());
        }
    }
    
    std::ostringstream oss;
    oss << v;
    return oss.str();
}

std::string ConfigExporter::exportToXML(const Config& config) const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<configuration>\n";
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            oss << "  <entry key=\"" << key << "\"\>";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    // Escape XML
                    std::string escaped = arg;
                    replace(escaped.begin(), escaped.end(), '&', "&amp;");
                    replace(escaped.begin(), escaped.end(), '<', "&lt;");
                    replace(escaped.begin(), escaped.end(), '>', "&gt;");
                    replace(escaped.begin(), escaped.end(), '"', "&quot;");
                    oss << escaped;
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << value;
                }
            }, value.value());
            oss << "</entry>\n";
        }
    }
    
    oss << "</configuration>";
    return oss.str();
}

std::string ConfigExporter::exportToCSV(const Config& config) const {
    std::ostringstream oss;
    oss << "key,value,type\n";
    
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            oss << key << ",";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    // Escape quotes
                    std::string quoted = "\"" + arg + "\"";
                    replace(quoted.begin(), quoted.end(), '"', '""');
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
                oss << ",";
                
                if constexpr (std::is_same_v<T, int> || 
                              std::is_same_v<T, int64_t>) {
                    oss << "integer";
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << "float";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << "boolean";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "int_array";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "float_array";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "string_array";
                }
            }, value.value());
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string ConfigExporter::exportToSQL(const Config& config) const {
    std::ostringstream oss;
    oss << "CREATE TABLE config (key TEXT PRIMARY KEY, value TEXT, type TEXT);\n\nINSERT INTO config (key, value, type) VALUES\n";
    
    bool first = true;
    for (const auto& key : config.getHierarchicalKeys()) {
        auto value = config.getHierarchical(key);
        if (value.has_value()) {
            if (!first) {
                oss << ",\n";
            }
            first = false;
            
            oss << "  ('" << key << "', ";
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    std::string escaped = arg;
                    replace(escaped.begin(), escaped.end(), "'", "''");
                    oss << "'" << escaped << "', '";
                    oss << "string";
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t>) {
                    oss << std::to_string(arg) << "', 'integer'";
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << std::to_string(arg) << "', 'float'";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false") << "', 'boolean'";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    std::string array_str = "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) array_str += ", ";
                        std::string escaped = arg[i];
                        replace(escaped.begin(), escaped.end(), "'", "''");
                        array_str += "'" + escaped + "'";
                    }
                    array_str += "]";
                    oss << "'" << array_str << "', 'string_array'";
                }
            }, value.value());
        }
    }
    
    return oss.str();
}

std::string ConfigExporter::exportFormatToString(ExportFormat format) {
    switch (format) {
        case ExportFormat::Text: return "text";
        case ExportFormat::JSON: return "json";
        case ExportFormat::YAML: return "yaml";
        case ExportFormat::TOML: return "toml";
        case ExportFormat::XML: return "xml";
        case ExportFormat::CSV: return "csv";
        case ExportFormat::SQL: return "sql";
        default: return "text";
    }
}

ConfigExporter::ExportFormat ConfigExporter::parseFormatFromString(const std::string& format_str) {
    std::string lower = format_str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "json") return ExportFormat::JSON;
    if (lower == "yaml" || lower == "yml") return ExportFormat::YAML;
    if (lower == "toml") return ExportFormat::TOML;
    if (lower == "xml") return ExportFormat::XML;
    if (lower == "csv") return ExportFormat::CSV;
    if (lower == "sql") return ExportFormat::SQL;
    return ExportFormat::Text;
}

// ConfigImporter implementations
Config ConfigImporter::importFromFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file: " + filepath);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    return importFromText(content);
}

Config ConfigImporter::importFromText(const std::string& text) const {
    ExportFormat format = detectFormat(text);
    
    switch (format) {
        case ExportFormat::JSON:
            return importFromJSON(text);
        case ExportFormat::YAML:
            return importFromYAML(text);
        case ExportFormat::TOML:
            return importFromTOML(text);
        case ExportFormat::Text:
            // Parse simple key=value format
            Config config;
            std::istringstream stream(text);
            std::string line;
            
            while (std::getline(stream, line)) {
                line = Config::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = Config::trim(line.substr(0, pos));
                    std::string value = Config::trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config.set(key, value, options.source);
                }
            }
            
            return config;
        default:
            throw ConfigParseError("Unsupported format for import: " + exportFormatToString(format));
    }
}

Config ConfigImporter::importFromJSON(const std::string& json_text) const {
    try {
        nlohmann::json j = nlohmann::json::parse(json_text);
        Config config;
        
        for (const auto& pair : j.items()) {
            ConfigValue value;
            
            if (pair.value().is_array()) {
                if (pair.value().empty()) {
                    value = std::vector<std::string>{};
                } else if (pair.value()[0].is_number_integer()) {
                    std::vector<int> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<int>());
                    }
                    value = vec;
                } else if (pair.value()[0].is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                } else {
                    std::vector<std::string> vec;
                    for (const auto& item : pair.value()) {
                        vec.push_back(item.get<std::string>());
                    }
                    value = vec;
                }
            } else if (pair.value().is_boolean()) {
                value = pair.value().get<bool>();
            } else if (pair.value().is_number_integer()) {
                value = pair.value().get<int64_t>();
            } else if (pair.value().is_number_float()) {
                value = pair.value().get<double>();
            } else {
                value = pair.value().get<std::string>();
            }
            
            config.set(pair.key(), value, options.source);
        }
        
        return config;
    } catch (const std::exception& e) {
        throw ConfigParseError("Failed to import JSON: " + std::string(e.what()));
    }
}

Config ConfigImporter::importFromYAML(const std::string& yaml_text) const {
    try {
        YAML::Node root = YAML::Load(yaml_text);
        Config config;
        
        for (const auto& pair : root) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (pair.second.IsSequence()) {
                std::vector<std::string> vec;
                for (const auto& item : pair.second) {
                    if (item.IsScalar()) {
                        vec.push_back(item.as<std::string>());
                    } else {
                        vec.push_back(item.As<std::string>());
                    }
                }
                value = vec;
            } else if (pair.second.IsScalar()) {
                std::string str_val = pair.second.as<std::string>();
                
                // Try to infer type
                try {
                    int int_val = std::stoi(str_val);
                    value = int_val;
                } catch (...) {
                    try {
                        double double_val = std::stod(str_val);
                        value = double_val;
                    } catch (...) {
                        try {
                            if (str_val == "true" || str_val == "false") {
                                bool bool_val = (str_val == "true");
                                value = bool_val;
                            } else {
                                value = str_val;
                            }
                        } catch (...) {
                            value = str_val;
                        }
                    }
                }
            }
            
            config.set(key, value, options.source);
        }
        
        return config;
    } catch (const YAML::Exception& e) {
        throw ConfigParseError("Failed to import YAML: " + std::string(e.what()));
    }
}

Config ConfigImporter::importFromTOML(const std::string& toml_text) const {
    try {
        auto data = toml::parse(toml_text);
        Config config;
        
        for (const auto& pair : data) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (auto* p = std::get_if<toml::integer>(&pair.second)) {
                value = static_cast<int64_t>(*p);
            } else if (auto* p = std::get_if<toml::floating_point>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::boolean>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::string>(&pair.second)) {
                value = *p;
            } else if (auto* p = std::get_if<toml::array>(&pair.second)) {
                // Convert array to appropriate vector type
                std::vector<std::string> str_vec;
                for (const auto& item : **p) {
                    if (auto* s = std::get_if<std::string>(&item)) {
                        str_vec.push_back(*s);
                    } else if (auto* i = std::get_if<toml::integer>(&item)) {
                        str_vec.push_back(std::to_string(static_cast<int64_t>(*i)));
                    } else if (auto* f = std::get_if<toml::floating_point>(&item)) {
                        str_vec.push_back(std::to_string(*f));
                    } else if (auto* b = std::get_if<toml::boolean>(&item)) {
                        str_vec.push_back(*b ? "true" : "false");
                    }
                }
                value = str_vec;
            }
            
            config.set(key, value, options.source);
        }
        
        return config;
    } catch (const std::exception& e) {
        throw ConfigParseError("Failed to import TOML: " + std::string(e.what()));
    }
}

ConfigExporter::ExportFormat ConfigImporter::detectFormat(const std::string& content) {
    // Remove leading whitespace
    size_t start = content.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return ExportFormat::Text;
    
    std::string first_chars = content.substr(start, std::min<size_t>(50, content.size() - start));
    
    // Check for JSON
    if (first_chars.find("{") != std::string::npos || first_chars.find("[") != std::string::npos) {
        try {
            nlohmann::json::parse(content);
            return ExportFormat::JSON;
        } catch (...) {
            // Not JSON
        }
    }
    
    // Check for YAML
    if (first_chars.find(":") != std::string::npos && (first_chars.find("-") != std::string::npos || 
         first_chars.find("  ") != std::string::npos)) {
        try {
            YAML::Load(content);
            return ExportFormat::YAML;
        } catch (...) {
            // Not YAML
        }
    }
    
    // Check for TOML
    if (first_chars.find("[") != std::string::npos && first_chars.find("=") != std::string::npos) {
        try {
            toml::parse(content);
            return ExportFormat::TOML;
        } catch (...) {
            // Not TOML
        }
    }
    
    return ExportFormat::Text;
}

bool ConfigImporter::isJSON(const std::string& content) {
    try {
        nlohmann::json::parse(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigImporter::isYAML(const std::string& content) {
    try {
        YAML::Load(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigImporter::isTOML(const std::string& content) {
    try {
        toml::parse(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigImporter::isText(const std::string& content) {
    return true; // Default to text format
}

// ConfigDiffTool implementations
ConfigDiffTool::ConfigDiffTool(const std::string& diff_name) : name(diff_name) {}

std::vector<std::pair<std::string, std::string>> ConfigDiffTool::diff(const Config& config1, const Config& config2) const {
    std::vector<std::pair<std::string, std::string>> diffs;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    std::set<std::string> all_keys;
    for (const auto& key : keys1) all_keys.insert(key);
    for (const auto& key : keys2) all_keys.insert(key);
    
    for (const auto& key : all_keys) {
        auto val1 = config1.getHierarchical(key);
        auto val2 = config2.getHierarchical(key);
        
        std::string val1_str, val2_str;
        
        if (val1.has_value()) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    val1_str = "\"" + arg + "\"";
                } else {
                    val1_str = std::to_string(arg);
                }
            }, val1.value());
        } else {
            val1_str = "(null)";
        }
        
        if (val2.has_value()) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    val2_str = "\"" + arg + "\"";
                } else {
                    val2_str = std::to_string(arg);
                }
            }, val2.value());
        } else {
            val2_str = "(null)";
        }
        
        diffs.emplace_back(val1_str, val2_str);
    }
    
    return diffs;
}

Config ConfigDiffTool::unifiedDiff(const Config& config1, const Config& config2) const {
    Config diff_config;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    std::set<std::string> all_keys;
    for (const auto& key : keys1) all_keys.insert(key);
    for (const auto& key : keys2) all_keys.insert(key);
    
    for (const auto& key : all_keys) {
        auto val1 = config1.getHierarchical(key);
        auto val2 = config2.getHierarchical(key);
        
        if (!val1.has_value() && !val2.has_value()) {
            continue;
        }
        
        ConfigValue new_value;
        if (val2.has_value()) {
            new_value = val2.value();
        }
        
        diff_config.set(key, new_value, ConfigSource::File);
    }
    
    return diff_config;
}

double ConfigDiffTool::similarity(const Config& config1, const Config& config2) const {
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    if (keys1.empty() && keys2.empty()) {
        return 1.0;
    }
    
    int common_keys = 0;
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
            common_keys++;
        }
    }
    
    int total_keys = std::max(keys1.size(), keys2.size());
    return static_cast<double>(common_keys) / total_keys;
}

std::vector<std::string> ConfigDiffTool::findBreakingChanges(const Config& old_config, const Config& new_config) {
    std::vector<std::string> breaking_changes;
    
    auto old_keys = old_config.getKeys();
    
    for (const auto& key : old_keys) {
        auto old_val = old_config.getHierarchical(key);
        auto new_val = new_config.getHierarchical(key);
        
        if (old_val.has_value() && !new_val.has_value()) {
            breaking_changes.push_back("Removed key: " + key);
        } else if (old_val.has_value() && new_val.has_value()) {
            // Check for incompatible type changes
            auto old_type = getConfigValueType(old_val.value());
            auto new_type = getConfigValueType(new_val.value());
            
            if (old_type != new_type) {
                breaking_changes.push_back("Type changed for key " + key + ": " + 
                                          old_type + " -> " + new_type);
            } else if (old_val.value() != new_val.value()) {
                breaking_changes.push_back("Value changed for key " + key + ": " +
                                          valueToString(old_val.value()) + " -> " +
                                          valueToString(new_val.value()));
            }
        }
    }
    
    return breaking_changes;
}

std::vector<std::string> ConfigDiffTool::findDeprecatedKeys(const Config& old_config, const Config& new_config) {
    std::vector<std::string> deprecated_keys;
    
    // This would be populated from a deprecation registry
    // For now, return empty
    return deprecated_keys;
}

std::vector<std::string> ConfigDiffTool::findNewKeys(const Config& old_config, const Config& new_config) {
    std::vector<std::string> new_keys;
    auto old_keys = old_config.getKeys();
    auto new_keys_all = new_config.getKeys();
    
    for (const auto& key : new_keys_all) {
        if (std::find(old_keys.begin(), old_keys.end(), key) == old_keys.end()) {
            new_keys.push_back(key);
        }
    }
    
    return new_keys;
}

std::vector<std::string> ConfigDiffTool::findRemovedKeys(const Config& old_config, const Config& new_config) {
    std::vector<std::string> removed_keys;
    auto new_keys = new_config.getKeys();
    auto old_keys_all = old_config.getKeys();
    
    for (const auto& key : old_keys_all) {
        if (std::find(new_keys.begin(), new_keys.end(), key) == new_keys.end()) {
            removed_keys.push_back(key);
        }
    }
    
    return removed_keys;
}

// Helper functions
std::string ConfigDiffTool::getConfigValueType(const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || 
                      std::is_same_v<T, int64_t>) {
            return "int";
        } else if constexpr (std::is_same_v<T, double>) {
            return "float";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "int_array";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "float_array";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "string_array";
        }
    });
    return "unknown";
}

std::string ConfigDiffTool::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

} // namespace nlm