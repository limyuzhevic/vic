// Configuration management utilities and enhanced features
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <fstream>
#include <cctype>

#include "ValidationExceptions.hpp"
#include "Validators.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration backup/restore utility
class ConfigurationManager {
public:
    // Create a backup of the current configuration
    static bool backupConfig(const std::shared_ptr<Config>& config, const std::string& backupPath) {
        try {
            std::ofstream file(backupPath);
            if (!file.is_open()) {
                return false;
            }
            
            file << "# Configuration Backup\n";
            file << "# Created: " << getCurrentTimestamp() << "\n";
            file << "# Source: " << backupPath << "\n\n";
            
            // Write all configuration entries
            auto keys = config->getKeys();
            file << "# Configuration entries: " << keys.size() << "\n\n";
            
            for (const auto& key : keys) {
                auto valueOpt = config->get<std::string>(key);
                if (valueOpt) {
                    file << key << " = \"" << *valueOpt << "\"\n";
                } else {
                    // Try other types
                    auto intVal = config->get<int>(key);
                    if (intVal) {
                        file << key << " = " << *intVal << "\n";
                    } else {
                        auto doubleVal = config->get<double>(key);
                        if (doubleVal) {
                            file << key << " = " << *doubleVal << "\n";
                        } else {
                            auto boolVal = config->get<bool>(key);
                            if (boolVal) {
                                file << key << " = " << (*boolVal ? "true" : "false") << "\n";
                            }
                        }
                    }
                }
            }
            
            file << "\n# End of backup\n";
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Restore configuration from backup
    static bool restoreConfig(std::shared_ptr<Config>& config, const std::string& backupPath) {
        try {
            // Clear existing config
            config->clear();
            
            std::ifstream file(backupPath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '\"' && value.back() == '\"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config->set(key, value, ConfigSource::File);
                }
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Compare two configurations
    static std::string compareConfigs(const std::shared_ptr<Config>& config1,
                                      const std::shared_ptr<Config>& config2) {
        std::stringstream ss;
        ss << "=== Configuration Comparison ===\n";
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Keys only in config1
        std::set<std::string> set1(keys1.begin(), keys1.end());
        std::set<std::string> set2(keys2.begin(), keys2.end());
        
        std::set<std::string> onlyIn1, onlyIn2, inBoth;
        std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                              std::inserter(inBoth, inBoth.begin()));
        std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
                            std::inserter(onlyIn1, onlyIn1.begin()));
        std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(),
                            std::inserter(onlyIn2, onlyIn2.begin()));
        
        ss << "Keys only in config1: " << onlyIn1.size() << "\n";
        for (const auto& key : onlyIn1) {
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            ss << "  " << key << " = \"" << (val1 ? *val1 : "") << "\"\n";
        }
        
        ss << "\nKeys only in config2: " << onlyIn2.size() << "\n";
        for (const auto& key : onlyIn2) {
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            ss << "  " << key << " = \"" << (val2 ? *val2 : "") << "\"\n";
        }
        
        ss << "\nKeys in both configs: " << inBoth.size() << "\n";
        for (const auto& key : inBoth) {
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            if (val1 != val2) {
                ss << "  " << key << " - DIFFERENT:\n";
                ss << "    Config1: \"" << (val1 ? *val1 : "") << "\"\n";
                ss << "    Config2: \"" << (val2 ? *val2 : "") << "\"\n";
            }
        }
        
        return ss.str();
    }
    
    // Generate configuration diff
    static std::string generateConfigDiff(const std::shared_ptr<Config>& oldConfig,
                                          const std::shared_ptr<Config>& newConfig,
                                          const std::string& contextPrefix = "") {
        std::stringstream ss;
        ss << contextPrefix << "=== Configuration Diff ===\n";
        
        auto oldKeys = oldConfig->getKeys();
        auto newKeys = newConfig->getKeys();
        
        std::set<std::string> oldSet(oldKeys.begin(), oldKeys.end());
        std::set<std::string> newSet(newKeys.begin(), newKeys.end());
        
        // Changes in oldConfig
        std::set<std::string> removed;
        std::set_difference(oldSet.begin(), oldSet.end(), newSet.begin(), newSet.end(),
                            std::inserter(removed, removed.begin()));
        
        if (!removed.empty()) {
            ss << contextPrefix << "Removed keys: ";
            for (const auto& key : removed) {
                ss << key << " ";
            }
            ss << "\n";
        }
        
        // Changes in newConfig
        std::set<std::string> added;
        std::set_difference(newSet.begin(), newSet.end(), oldSet.begin(), oldSet.end(),
                            std::inserter(added, added.begin()));
        
        if (!added.empty()) {
            ss << contextPrefix << "Added keys: ";
            for (const auto& key : added) {
                ss << key << " ";
            }
            ss << "\n";
        }
        
        // Modified keys
        std::set<std::string> common;
        std::set_intersection(oldSet.begin(), oldSet.end(), newSet.begin(), newSet.end(),
                              std::inserter(common, common.begin()));
        
        for (const auto& key : common) {
            auto oldVal = oldConfig->get<std::string>(key);
            auto newVal = newConfig->get<std::string>(key);
            if (oldVal != newVal) {
                ss << contextPrefix << "Modified key: " << key << "\n";
                ss << contextPrefix << "  Old: \"" << (oldVal ? *oldVal : "") << "\"\n";
                ss << contextPrefix << "  New: \"" << (newVal ? *newVal : "") << "\"\n";
            }
        }
        
        return ss.str();
    }
    
    // Generate configuration documentation
    static std::string generateDocumentation(const std::shared_ptr<Config>& config,
                                             const std::string& title = "Configuration Documentation") {
        std::stringstream ss;
        ss << "# " << title << "\n\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        auto keys = config->getKeys();
        ss << "## Configuration Parameters (" << keys.size() << " total)\n\n";
        
        for (const auto& key : keys) {
            ss << "### " << key << "\n";
            
            // Try to get value with different types
            auto valueOpt = config->get<std::string>(key);
            std::string valueStr;
            if (valueOpt) {
                valueStr = *valueOpt;
            } else {
                auto intVal = config->get<int>(key);
                if (intVal) {
                    valueStr = std::to_string(*intVal);
                } else {
                    auto doubleVal = config->get<double>(key);
                    if (doubleVal) {
                        valueStr = std::to_string(*doubleVal);
                    } else {
                        auto boolVal = config->get<bool>(key);
                        if (boolVal) {
                            valueStr = *boolVal ? "true" : "false";
                        }
                    }
                }
            }
            
            ss << "- **Value**: `" << valueStr << "`\n";
            ss << "- **Type**: " << (valueOpt ? "string" : 
                                intVal ? "int" :
                                doubleVal ? "double" :
                                boolVal ? "bool" : "unknown") << "\n";
            ss << "- **Description**: TODO - Add description for " << key << "\n\n";
        }
        
        return ss.str();
    }
    
    // Get current timestamp as string
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Trim whitespace from string
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

// Enhanced configuration schema with inheritance and environment support
class EnhancedConfigSchema {
public:
    struct SchemaGroup {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        
        SchemaGroup() = default;
        SchemaGroup(const std::string& name, const std::string& description = "")
            : name(name), description(description) {}
    };
    
    EnhancedConfigSchema() = default;
    
    void addGroup(const SchemaGroup& group) {
        groups_.push_back(group);
        // Also add individual parameters to the parameter schema
        for (const auto& param : group.parameters) {
            parameterSchema_.addParameter(ConfigSchema::ParameterInfo(
                param.first, "string", false, "Parameter in group " + group.name
            ));
        }
    }
    
    const std::vector<SchemaGroup>& getGroups() const { return groups_; }
    const ConfigSchema& getParameterSchema() const { return parameterSchema_; }
    
    // Load configuration from environment variables
    static bool loadFromEnvironment(std::shared_ptr<Config>& config, const std::string& prefix = "NLM_") {
        bool success = true;
        
        // Read all environment variables
        // Note: In a real implementation, you'd need to access the actual environment
        // For now, we'll simulate with a placeholder
        
        return success;
    }
    
    // Merge with another schema (inheritance)
    void merge(const EnhancedConfigSchema& other) {
        // Merge groups
        for (const auto& group : other.getGroups()) {
            bool found = false;
            for (auto& existingGroup : groups_) {
                if (existingGroup.name == group.name) {
                    // Merge parameters
                    for (const auto& param : group.parameters) {
                        existingGroup.parameters[param.first] = param.second;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                groups_.push_back(group);
            }
        }
        
        // Merge parameter schema would go here
    }
    
private:
    std::vector<SchemaGroup> groups_;
    ConfigSchema parameterSchema_;
};

} // namespace nlm