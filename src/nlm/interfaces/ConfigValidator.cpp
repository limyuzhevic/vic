// ConfigValidator implementation
#include "interfaces/AdvancedInterface.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}
ConfigValidator::~ConfigValidator() = default;

bool ConfigValidator::loadSchema(const std::string& schemaFile) {
    std::ifstream file(schemaFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return loadSchemaFromString(content);
}

bool ConfigValidator::loadSchemaFromString(const std::string& schemaContent) {
    // Simple schema parsing for now
    // In a real implementation, this would parse JSON/YAML validation schemas
    pImpl->errors.clear();
    
    // For now, just accept any configuration
    // Real implementation would validate against schema rules
    return true;
}

std::vector<std::string> ConfigValidator::validate(const Config& config) const {
    return config.getValidationErrors();
}

std::vector<std::string> ConfigValidator::validateEntries(const std::vector<std::string>& keys, const Config& config) const {
    std::vector<std::string> errors;
    
    for (const auto& key : keys) {
        if (!config.has(key)) {
            errors.push_back("Missing required key: " + key);
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::getErrors() const {
    return pImpl->errors;
}

void ConfigValidator::clearErrors() {
    pImpl->errors.clear();
}

bool ConfigValidator::isValid(const Config& config) const {
    return config.validate();
}

std::string ConfigValidator::getValidationSummary(const Config& config) const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    oss << "  Valid: " << (config.validate() ? "Yes" : "No") << std::endl;
    oss << "  Total Keys: " << config.getKeys().size() << std::endl;
    oss << "  Errors: " << config.getValidationErrors().size() << std::endl;
    
    if (!config.getValidationErrors().empty()) {
        oss << "  Error Details:" << std::endl;
        for (const auto& error : config.getValidationErrors()) {
            oss << "    - " << error << std::endl;
        }
    }
    
    return oss.str();
}

bool ConfigValidator::exportValidationReport(const Config& config, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json report;
    report["valid"] = config.validate();
    report["total_keys"] = config.getKeys().size();
    report["errors"] = config.getValidationErrors();
    
    file << report.dump(2);
    return true;
}

bool ConfigValidator::importValidationSchema(const std::string& schemaFile) {
    return loadSchema(schemaFile);
}

bool ConfigValidator::createDefaultSchema(const std::string& outputFile) const {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["properties"]["random_seed"] = nlohmann::json::object();
    schema["properties"]["random_seed"]["type"] = "integer";
    schema["properties"]["random_seed"]["minimum"] = 1;
    
    schema["properties"]["simulation_timestep"] = nlohmann::json::object();
    schema["properties"]["simulation_timestep"]["type"] = "number";
    schema["properties"]["simulation_timestep"]["minimum"] = 0.0;
    
    schema["properties"]["neuron_count"] = nlohmann::json::object();
    schema["properties"]["neuron_count"]["type"] = "integer";
    schema["properties"]["neuron_count"]["minimum"] = 10;
    schema["properties"]["neuron_count"]["maximum"] = 100000;
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << schema.dump(2);
    return true;
}

std::unordered_map<std::string, double> ConfigValidator::optimize(const Config& config) const {
    std::unordered_map<std::string, double> optimizations;
    
    // Simple optimization logic
    // In a real implementation, this would use optimization algorithms
    
    // Check if neuron_count is too low
    if (config.has("neuron_count")) {
        auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
        if (neuronCount < 100) {
            optimizations["neuron_count"] = 100.0;
        }
    }
    
    // Check if simulation_timestep is too high
    if (config.has("simulation_timestep")) {
        auto timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep > 0.01) {
            optimizations["simulation_timestep"] = 0.001;
        }
    }
    
    return optimizations;
}

std::unordered_map<std::string, std::string> ConfigValidator::getOptimizationSuggestions(const Config& config) const {
    std::unordered_map<std::string, std::string> suggestions;
    
    if (config.has("neuron_count")) {
        auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
        if (neuronCount < 100) {
            suggestions["neuron_count"] = "Consider increasing neuron count for better results";
        }
    }
    
    if (config.has("simulation_timestep")) {
        auto timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep > 0.01) {
            suggestions["simulation_timestep"] = "Consider decreasing simulation timestep for better accuracy";
        }
    }
    
    return suggestions;
}

} // namespace nlm