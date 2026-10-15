// ConfigManager implementation
#include "config/ConfigManager.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <regex>
#include <iostream>

namespace nlm {

struct ConfigManager::Impl {
    std::map<std::string, std::shared_ptr<Config>> configs;
    std::map<std::string, ConfigTemplate> templates;
    bool historyEnabled = true;
    bool envVarSubEnabled = false;
    bool fileInclusionEnabled = false;
    bool macroSupportEnabled = false;
    bool conditionalEnabled = false;
    bool hotReloadEnabled = false;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::createConfig(const std::string& name, const std::string& description,
                                const std::string& templateName) {
    // Create directory if needed
    std::string configDir = "configs";
    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directory(configDir);
    }
    
    std::string filepath = generateConfigPath(name);
    
    auto config = std::make_shared<Config>();
    config->set("name", name);
    config->set("description", description);
    config->set("timestamp", getCurrentTimestamp());
    
    return saveConfig(name, config);
}

bool ConfigManager::createConfigFromTemplate(const std::string& name, const std::string& templateName) {
    // Load template
    ConfigTemplate template;
    if (!loadTemplate(templateName, template)) {
        return false;
    }
    
    // Create config from template
    auto config = template.createConfig();
    config->set("name", name);
    config->set("template", templateName);
    
    return saveConfig(name, config);
}

bool ConfigManager::validateConfig(const std::string& configName, const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate against all schemas
    ValidationResult result;
    // Would validate against multiple schemas here
    return result.valid;
}

bool ConfigManager::compareConfigs(const std::string& config1, const std::string& config2,
                                   const std::string& outputFile) {
    std::shared_ptr<Config> cfg1, cfg2;
    if (!loadConfig(config1, cfg1) || !loadConfig(config2, cfg2)) {
        return false;
    }
    
    ConfigValidator validator;
    auto differences = validator.getDifferences(*cfg1, *cfg2);
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        json["config1"] = config1;
        json["config2"] = config2;
        json["differences"] = differences;
        
        file << json.dump(2);
    }
    
    return true;
}

bool ConfigManager::mergeConfigs(const std::string& config1, const std::string& config2,
                                 const std::string& outputFile, bool overwrite) {
    std::shared_ptr<Config> cfg1, cfg2;
    if (!loadConfig(config1, cfg1) || !loadConfig(config2, cfg2)) {
        return false;
    }
    
    auto merged = std::make_shared<Config>(*cfg1);
    
    for (const auto& entry : cfg2->pImpl->entries) {
        // Check if parameter already exists
        bool exists = false;
        for (const auto& existing : merged->pImpl->entries) {
            if (existing.key == entry.key) {
                exists = true;
                break;
            }
        }
        
        if (!exists || overwrite) {
            merged->set(entry.key, entry.value);
        }
    }
    
    return saveConfig("merged", merged);
}

bool ConfigManager::exportConfig(const std::string& configName, const std::string& outputFile,
                                 const std::string& format) {
    std::shared_ptr<Config> config;
    if (!loadConfig(configName, config)) {
        return false;
    }
    
    if (format == "json") {
        nlohmann::json json;
        for (const auto& entry : config->pImpl->entries) {
            json[entry.key] = "ConfigValue"; // Simplified
        }
        
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2);
        return true;
    } else if (format == "yaml") {
        // Would implement YAML export
        return false;
    } else if (format == "xml") {
        // Would implement XML export
        return false;
    }
    
    return false;
}

bool ConfigManager::importConfig(const std::string& inputFile, const std::string& format,
                                 const std::string& name) {
    if (format == "json") {
        try {
            std::ifstream file(inputFile);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json json;
            file >> json;
            
            auto config = std::make_shared<Config>();
            
            // Parse JSON (simplified)
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                // Convert value to ConfigValue based on type
                if (it->is_number_integer()) {
                    config->set(key, it->get<int64_t>());
                } else if (it->is_number_float()) {
                    config->set(key, it->get<double>());
                } else if (it->is_boolean()) {
                    config->set(key, it->get<bool>());
                } else if (it->is_string()) {
                    config->set(key, it->get<std::string>());
                }
            }
            
            std::string configName = name.empty() ? "imported" : name;
            return saveConfig(configName, config);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return false;
}

bool ConfigManager::listConfigs(const std::string& pattern) const {
    std::vector<std::string> names;
    if (!getConfigNames(names)) {
        return false;
    }
    
    for (const auto& name : names) {
        std::cout << name << std::endl;
    }
    
    return true;
}

bool ConfigManager::deleteConfig(const std::string& name, bool force) {
    std::string filepath = generateConfigPath(name);
    
    if (!force && std::filesystem::exists(filepath)) {
        std::cout << "Configuration " << name << " exists. Use --force to delete." << std::endl;
        return false;
    }
    
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove(filepath);
    }
    
    return true;
}

bool ConfigManager::backupConfig(const std::string& name, const std::string& outputDir) {
    std::shared_ptr<Config> config;
    if (!loadConfig(name, config)) {
        return false;
    }
    
    std::string backupDir = outputDir.empty() ? "backups" : outputDir;
    if (!std::filesystem::exists(backupDir)) {
        std::filesystem::create_directory(backupDir);
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string backupFile = backupDir + "/" + name + "_" + timestamp + ".cfg";
    
    return config->saveToFile(backupFile);
}

bool ConfigManager::restoreConfig(const std::string& name, const std::string& backupFile,
                                  bool overwrite) {
    auto config = std::make_shared<Config>();
    
    if (!backupFile.empty()) {
        if (!config->loadFromFile(backupFile)) {
            return false;
        }
    } else {
        std::string defaultBackup = "backups/" + name + "_latest.cfg";
        if (!config->loadFromFile(defaultBackup)) {
            return false;
        }
    }
    
    return saveConfig(name, config);
}

bool ConfigManager::saveConfig(const std::string& name, const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    std::string filepath = generateConfigPath(name);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write config in simple format
        file << "# Configuration: " << name << std::endl;
        file << "# Generated: " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        for (const auto& entry : config->pImpl->entries) {
            file << "# " << entry.description << std::endl;
            // Write value based on type
            if (std::holds_alternative<std::string>(entry.value)) {
                file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"" << std::endl;
            } else if (std::holds_alternative<int>(entry.value)) {
                file << entry.key << " = " << std::get<int>(entry.value) << std::endl;
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << entry.key << " = " << std::get<int64_t>(entry.value) << std::endl;
            } else if (std::holds_alternative<double>(entry.value)) {
                file << entry.key << " = " << std::get<double>(entry.value) << std::endl;
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false") << std::endl;
            } else {
                file << entry.key << " = " << "ConfigValue" << std::endl;
            }
            file << std::endl;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigManager::loadConfig(const std::string& name, std::shared_ptr<Config>& config) {
    std::string filepath = generateConfigPath(name);
    
    config = std::make_shared<Config>();
    return config->loadFromFile(filepath);
}

bool ConfigManager::updateConfig(const std::string& name, const std::map<std::string, ConfigValue>& updates) {
    std::shared_ptr<Config> config;
    if (!loadConfig(name, config)) {
        return false;
    }
    
    for (const auto& [key, value] : updates) {
        config->set(key, value);
    }
    
    return saveConfig(name, config);
}

bool ConfigManager::getConfigNames(std::vector<std::string>& names) const {
    names.clear();
    
    std::string configDir = "configs";
    if (!std::filesystem::exists(configDir)) {
        return true;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
            names.push_back(entry.path().stem().string());
        }
    }
    
    return true;
}

bool ConfigManager::getConfigMetadata(const std::string& name, std::map<std::string, std::string>& metadata) const {
    std::shared_ptr<Config> config;
    if (!loadConfig(name, config)) {
        return false;
    }
    
    metadata["name"] = name;
    metadata["timestamp"] = getCurrentTimestamp();
    metadata["entry_count"] = std::to_string(config->pImpl->entries.size());
    
    return true;
}

bool ConfigManager::createTemplate(const std::string& name, const ConfigTemplate& template) {
    return template.saveToFile(generateTemplatePath(name));
}

bool ConfigManager::loadTemplate(const std::string& name, ConfigTemplate& template) {
    return template.loadFromFile(generateTemplatePath(name));
}

bool ConfigManager::listTemplates() const {
    std::string templateDir = "config/templates";
    if (!std::filesystem::exists(templateDir)) {
        std::cout << "No templates directory found." << std::endl;
        return true;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(templateDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::cout << entry.path().stem().string() << std::endl;
        }
    }
    
    return true;
}

bool ConfigManager::deleteTemplate(const std::string& name) {
    std::string filepath = generateTemplatePath(name);
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove(filepath);
        return true;
    }
    return false;
}

bool ConfigManager::enableHistory(bool enable) {
    // Would enable/disable history tracking
    return true;
}

bool ConfigManager::addToHistory(const std::shared_ptr<Config>& config, const std::string& action,
                                const std::string& user, const std::string& description) {
    // Would add to history if enabled
    return true;
}

bool ConfigManager::getHistoryEntries(std::vector<std::shared_ptr<ConfigHistoryEntry>>& entries) const {
    // Would retrieve history entries
    return true;
}

bool ConfigManager::restoreFromHistory(const std::string& entryId, std::shared_ptr<Config>& config) {
    // Would restore from history
    return true;
}

std::string ConfigManager::generateConfigPath(const std::string& name) const {
    return "configs/" + name + ".cfg";
}

std::string ConfigManager::generateTemplatePath(const std::string& name) const {
    return "config/templates/" + name + ".json";
}

std::string ConfigManager::generateBackupPath(const std::string& name, const std::string& timestamp) const {
    return "backups/" + name + "_" + timestamp + ".cfg";
}

std::string ConfigManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace nlm
