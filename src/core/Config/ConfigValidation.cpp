#pragma once

#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace nlm {

// JSON Schema validation functions
namespace JsonSchemaValidation {

    bool validate(const Config& config, const std::string& schemaPath) {
        std::ifstream file(schemaPath);
        if (!file.is_open()) {
            spdlog::error("Could not open schema file: {}", schemaPath);
            return false;
        }
        
        json schema;
        try {
            file >> schema;
        } catch (const std::exception& e) {
            spdlog::error("Error parsing schema file: {}", e.what());
            return false;
        }
        
        // Use the validator to validate the configuration
        ConfigValidator validator;
        validator.setSchema(schema);
        return validator.validate(config, schema);
    }
    
    json createDefaultSchema() {
        json schema;
        schema["type"] = "object";
        schema["properties"] = json::object();
        schema["required"] = json::array();
        
        // Add system-specific properties
        json memoryProps;
        memoryProps["type"] = "object";
        memoryProps["properties"]["workingMemoryCapacity"] = json::object();
        memoryProps["properties"]["workingMemoryCapacity"]["type"] = "number";
        memoryProps["properties"]["workingMemoryCapacity"]["minimum"] = 0.0;
        memoryProps["properties"]["workingMemoryCapacity"]["default"] = 100.0;
        
        memoryProps["properties"]["episodicConsolidationRate"] = json::object();
        memoryProps["properties"]["episodicConsolidationRate"]["type"] = "number";
        memoryProps["properties"]["episodicConsolidationRate"]["minimum"] = 0.0;
        memoryProps["properties"]["episodicConsolidationRate"]["maximum"] = 1.0;
        memoryProps["properties"]["episodicConsolidationRate"]["default"] = 0.1;
        
        schema["properties"]["memory"] = memoryProps;
        schema["required"].push_back("memory");
        
        return schema;
    }
}

// Configuration comparison implementation
namespace ConfigComparison {

    std::vector<ConfigDiff> getDetailedDiff(const Config& config1, const Config& config2) {
        std::vector<ConfigDiff> diffs = config1.getDiff(config2);
        
        // Add detailed information to diffs
        for (auto& diff : diffs) {
            // Add human-readable descriptions
            if (diff.operation == "added") {
                diff.operation = "added: " + (std::holds_alternative<std::monostate>(diff.newValue) ? "default" : "value");
            } else if (diff.operation == "removed") {
                diff.operation = "removed: " + (std::holds_alternative<std::monostate>(diff.oldValue) ? "default" : "value");
            } else if (diff.operation == "modified") {
                diff.operation = "modified: " + configValueToString(diff.oldValue) + " -> " + configValueToString(diff.newValue);
            }
        }
        
        return diffs;
    }
    
    double calculateSimilarity(const Config& config1, const Config& config2) {
        std::vector<ConfigDiff> diffs = config1.getDiff(config2);
        
        if (diffs.empty()) {
            return 1.0; // Identical
        }
        
        // Calculate similarity based on number of matching entries
        std::set<std::string> allKeys;
        for (const auto& entry : config1.getKeys()) {
            allKeys.insert(entry);
        }
        for (const auto& entry : config2.getKeys()) {
            allKeys.insert(entry);
        }
        
        size_t matches = 0;
        for (const auto& key : allKeys) {
            auto val1 = config1.get<ConfigValue>(key);
            auto val2 = config2.get<ConfigValue>(key);
            
            if (val1 && val2) {
                if (*val1 == *val2) {
                    matches++;
                }
            }
        }
        
        return static_cast<double>(matches) / allKeys.size();
    }
}

// Configuration import/export implementation
namespace ConfigImportExport {

    bool importFromYAML(const std::string& filepath, Config& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            spdlog::error("Could not open file for import: {}", filepath);
            return false;
        }
        
        try {
            // For now, delegate to JSON import
            // In a full implementation, you'd use a YAML parser like YAML-cpp
            return config.importFromFile(filepath, true);
        } catch (const std::exception& e) {
            spdlog::error("Error importing config: {}", e.what());
            return false;
        }
    }
    
    bool exportToYAML(const std::string& filepath, const Config& config) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            spdlog::error("Could not open file for export: {}", filepath);
            return false;
        }
        
        // For now, delegate to JSON export
        // In a full implementation, you'd use a YAML formatter
        return config.exportToFile(filepath);
    }
    
    std::string generateConfigReport(const Config& config) {
        std::ostringstream oss;
        oss << "Configuration Report\n";
        oss << "===================\n\n";
        
        auto stats = config.getStatistics();
        oss << "Statistics:\n";
        oss << "  Total entries: " << stats.totalEntries << "\n";
        oss << "  Runtime entries: " << stats.runtimeEntries << "\n";
        oss << "  File entries: " << stats.fileEntries << "\n";
        oss << "  Command line entries: " << stats.commandLineEntries << "\n";
        oss << "  Default entries: " << stats.defaultEntries << "\n";
        oss << "  Validation pass rate: " << (stats.validationPassRate * 100) << "%\n\n";
        
        oss << "Entries by source:\n";
        for (const auto& key : config.getKeys()) {
            auto source = config.get<ConfigSource>(key);
            if (source) {
                oss << "  " << key << " (source: " << static_cast<int>(*source) << ")\n";
            }
        }
        
        return oss.str();
    }
}

// Configuration monitoring implementation
namespace ConfigMonitoring {

    std::map<std::string, double> collectMetrics(const Config& config) {
        std::map<std::string, double> metrics;
        
        auto stats = config.getStatistics();
        
        metrics["total_entries"] = static_cast<double>(stats.totalEntries);
        metrics["runtime_percentage"] = stats.totalEntries > 0 ? 
            (static_cast<double>(stats.runtimeEntries) / stats.totalEntries) : 0.0;
        metrics["file_percentage"] = stats.totalEntries > 0 ? 
            (static_cast<double>(stats.fileEntries) / stats.totalEntries) : 0.0;
        metrics["validation_rate"] = stats.validationPassRate;
        
        // Change frequency
        // Note: This would require getChangeHistory() to be implemented in Config
        // metrics["changes_per_hour"] = calculateChangeFrequency(config);
        
        return metrics;
    }
    
    std::vector<ConfigChangeEvent> getChangeHistory(const Config& config, size_t limit) {
        // This would need to be exposed in Config.hpp
        // For now, return empty vector
        return std::vector<ConfigChangeEvent>();
    }
    
    double calculateChangeFrequency(const Config& config) {
        auto history = getChangeHistory(config);
        if (history.size() < 2) {
            return 0.0;
        }
        
        auto first = history.front().timestamp;
        auto last = history.back().timestamp;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(last - first).count();
        
        return duration > 0 ? (history.size() / duration) : 0.0;
    }
}

// Configuration template management implementation
namespace ConfigTemplateManagement {

    bool applyTemplateToConfig(Config& config, const ConfigTemplate& template) {
        bool applied = false;
        for (const auto& [key, value] : template.values) {
            config.set(key, value, ConfigSource::Template);
            applied = true;
        }
        return applied;
    }
    
    bool validateTemplate(const ConfigTemplate& template, const std::vector<ConfigSchemaField>& schema) {
        bool valid = true;
        
        for (const auto& [key, value] : template.values) {
            auto fieldIt = std::find_if(schema.begin(), schema.end(),
                                        [&key](const ConfigSchemaField& f) { return f.name == key; });
            
            if (fieldIt != schema.end()) {
                // Use the ConfigValidator to validate the field
                ConfigValidator validator;
                json fieldSchema;
                fieldSchema["type"] = fieldIt->type;
                fieldSchema["required"] = fieldIt->required;
                
                if (!validator.validateField(key, value, fieldSchema)) {
                    valid = false;
                }
            }
        }
        
        return valid;
    }
    
    json templateToJson(const ConfigTemplate& template) {
        json j;
        j["name"] = template.name;
        j["description"] = template.description;
        j["category"] = template.category;
        
        if (!template.values.empty()) {
            json valuesJson = json::object();
            for (const auto& [key, value] : template.values) {
                valuesJson[key] = configValueToJson(value);
            }
            j["values"] = valuesJson;
        }
        
        return j;
    }
    
    ConfigTemplate jsonToTemplate(const json& j) {
        ConfigTemplate template;
        
        template.name = j.value("name", "");
        template.description = j.value("description", "");
        template.category = j.value("category", "");
        
        if (j.contains("values")) {
            for (const auto& [key, valueJson] : j["values"].items()) {
                template.values[key] = jsonToConfigValue(valueJson);
            }
        }
        
        return template;
    }
}

} // namespace nlm
