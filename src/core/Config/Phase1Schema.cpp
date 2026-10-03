// Phase 1 schema implementation
#pragma once

#include "Phase1Schema.hpp"
#include <string>

namespace nlm {

Phase1Schema::Phase1Schema() : ConfigSchema() {
    // Initialize with Phase 1 specific settings
    setName("Phase 1 Schema");
    setVersion("1.0.0");
}

bool Phase1Schema::validate(const Config& config) const {
    bool valid = true;
    
    // Validate Phase 1 specific fields
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

std::string Phase1Schema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Phase 1 specific validation errors
    if (config.has("simulation_timestep")) {
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && (*timestep < 0.000001 || *timestep > 1.0)) {
            errors += "Phase 1 Validation Error: simulation_timestep must be between 0.000001 and 1.0\n";
        }
    }
    
    if (config.has("connection_probability")) {
        auto prob = config.get<double>("connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            errors += "Phase 1 Validation Error: connection_probability must be between 0.0 and 1.0\n";
        }
    }
    
    return errors;
}

ConfigSchema Phase1Schema::create() {
    ConfigSchema schema;
    schema.version = "1.0.0";
    schema.filename = "phase1_schema.json";
    
    // Define Phase 1 core fields
    ConfigSchema::Field field;
    
    field.key = "random_seed";
    field.type = "int";
    field.required = false;
    field.description = "Random seed for reproducible experiments";
    field.min = 0;
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

std::vector<std::string> Phase1Schema::getRequiredFields() const {
    std::vector<std::string> required;
    
    // Add required fields for Phase 1
    required.push_back("neuron_count");
    required.push_back("region_count");
    
    return required;
}

std::string Phase1Schema::getDescription(const std::string& key) const {
    auto it = descriptions_.find(key);
    if (it != descriptions_.end()) {
        return it->second;
    }
    return "Unknown field";
}

std::any Phase1Schema::getDefaultValue(const std::string& key) const {
    auto it = defaultValues_.find(key);
    if (it != defaultValues_.end()) {
        return it->second;
    }
    return std::any();
}

bool Phase1Schema::validateFieldType(const std::string& key, const ConfigValue& value) const {
    const ConfigSchema::Field* field = getField(key);
    if (!field) return false;
    
    // Type checking logic would go here
    return true;
}

bool Phase1Schema::validateFieldRange(const std::string& key, const ConfigValue& value) const {
    const ConfigSchema::Field* field = getField(key);
    if (!field) return false;
    
    // Range checking logic would go here
    return true;
}

bool Phase1Schema::validateFieldFormat(const std::string& key, const ConfigValue& value) const {
    const ConfigSchema::Field* field = getField(key);
    if (!field) return false;
    
    // Format checking logic would go here
    return true;
}

const ConfigSchema::Field* Phase1Schema::getField(const std::string& key) const {
    for (const auto& field : fields_) {
        if (field.key == key) {
            return &field;
        }
    }
    return nullptr;
}

std::string fieldToJSON(const ConfigSchema::Field& field) {
    // JSON serialization implementation
    return "{\"key\": \"" + field.key + "\", \"type\": \"" + field.type + "\"}";
}

ConfigSchema::Field fieldFromJSON(const std::string& jsonStr) {
    // JSON deserialization implementation
    ConfigSchema::Field field;
    // Parse JSON and populate field
    return field;
}

} // namespace nlm
