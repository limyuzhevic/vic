// Default schema definition
#pragma once

#include "DefaultSchema.hpp"
#include "Phase1Schema.hpp"
#include <string>

namespace nlm {

DefaultSchema::DefaultSchema() : ConfigSchema() {
    // Initialize with default settings
    setName("Default Schema");
    setVersion("1.0.0");
}

bool DefaultSchema::validate(const Config& config) const {
    // First validate base Phase 1 schema
    if (!Phase1Schema::validate(config)) {
        return false;
    }
    
    // Default specific validations
    bool valid = true;
    
    // Validate default configuration settings
    if (config.has("simulation_timestep")) {
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && (*timestep < 0.000001 || *timestep > 1.0)) {
            valid = false;
        }
    }
    
    if (config.has("connection_probability")) {
        auto prob = config.get<double>("connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            valid = false;
        }
    }
    
    return valid;
}

std::string DefaultSchema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Phase 1 errors
    std::string baseErrors = Phase1Schema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Default specific validation errors
    if (config.has("simulation_timestep")) {
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && (*timestep < 0.000001 || *timestep > 1.0)) {
            if (!errors.empty()) errors += "\n";
            errors += "Default Schema Validation Error: simulation_timestep must be between 0.000001 and 1.0";
        }
    }
    
    if (config.has("connection_probability")) {
        auto prob = config.get<double>("connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            if (!errors.empty()) errors += "\n";
            errors += "Default Schema Validation Error: connection_probability must be between 0.0 and 1.0";
        }
    }
    
    return errors;
}

ConfigSchema DefaultSchema::create() {
    ConfigSchema schema = Phase1Schema::create();
    schema.version = "1.0.0";
    schema.filename = "default_schema.json";
    
    // Add default-specific fields
    ConfigSchema::Field field;
    
    field.key = "random_seed";
    field.type = "int";
    field.required = false;
    field.description = "Random seed for reproducible experiments";
    field.min = 0;
    field.max = 2147483647;  // 32-bit signed int range
    schema.addField(field);
    
    field.key = "simulation_timestep";
    field.type = "double";
    field.required = false;
    field.description = "Simulation timestep in seconds (1e-6 to 1.0)";
    field.min = std::variant<int, double>(0.000001);
    field.max = std::variant<int, double>(1.0);
    schema.addField(field);
    
    field.key = "neuron_count";
    field.type = "int";
    field.required = false;
    field.description = "Number of neurons in the brain (10 to 1000000)";
    field.min = 10;
    field.max = 1000000;
    schema.addField(field);
    
    field.key = "region_count";
    field.type = "int";
    field.required = false;
    field.description = "Number of brain regions (1 to 100)";
    field.min = 1;
    field.max = 100;
    schema.addField(field);
    
    field.key = "connection_probability";
    field.type = "double";
    field.required = false;
    field.description = "Connection probability (0.0 to 1.0)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    return schema;
}

} // namespace nlm
