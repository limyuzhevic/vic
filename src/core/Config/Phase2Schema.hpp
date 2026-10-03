// Phase 2 schema implementation
#pragma once

#include "Phase2Schema.hpp"
#include "Phase1Schema.hpp"
#include <string>

namespace nlm {

Phase2Schema::Phase2Schema() : ConfigSchema() {
    // Initialize with Phase 2 specific settings
    setName("Phase 2 Schema");
    setVersion("2.0.0");
}

bool Phase2Schema::validate(const Config& config) const {
    // First validate base Phase 1 schema
    if (!Phase1Schema::validate(config)) {
        return false;
    }
    
    // Phase 2 specific validations
    bool valid = true;
    
    // Validate neuromodulation fields
    if (config.has("dopamine_max")) {
        auto dopamineMax = config.get<double>("dopamine_max");
        if (dopamineMax && (*dopamineMax < 0 || *dopamineMax > 10)) {
            valid = false;
        }
    }
    
    if (config.has("dopamine_decay_rate")) {
        auto decayRate = config.get<double>("dopamine_decay_rate");
        if (decayRate && (*decayRate < 0 || *decayRate > 1)) {
            valid = false;
        }
    }
    
    // Validate memory settings
    if (config.has("working_memory_size")) {
        auto memSize = config.get<int>("working_memory_size");
        if (memSize && (*memSize < 1 || *memSize > 10000)) {
            valid = false;
        }
    }
    
    // Validate episodic memory settings
    if (config.has("episodic_memory_size")) {
        auto memSize = config.get<int>("episodic_memory_size");
        if (memSize && (*memSize < 1 || *memSize > 100000)) {
            valid = false;
        }
    }
    
    return valid;
}

std::string Phase2Schema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Phase 1 errors
    std::string baseErrors = Phase1Schema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Phase 2 specific validation errors
    if (config.has("dopamine_max")) {
        auto dopamineMax = config.get<double>("dopamine_max");
        if (dopamineMax && (*dopamineMax < 0 || *dopamineMax > 10)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 2 Validation Error: dopamine_max must be between 0 and 10";
        }
    }
    
    if (config.has("dopamine_decay_rate")) {
        auto decayRate = config.get<double>("dopamine_decay_rate");
        if (decayRate && (*decayRate < 0 || *decayRate > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 2 Validation Error: dopamine_decay_rate must be between 0 and 1";
        }
    }
    
    if (config.has("working_memory_size")) {
        auto memSize = config.get<int>("working_memory_size");
        if (memSize && (*memSize < 1 || *memSize > 10000)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 2 Validation Error: working_memory_size must be between 1 and 10000";
        }
    }
    
    if (config.has("episodic_memory_size")) {
        auto memSize = config.get<int>("episodic_memory_size");
        if (memSize && (*memSize < 1 || *memSize > 100000)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 2 Validation Error: episodic_memory_size must be between 1 and 100000";
        }
    }
    
    return errors;
}

ConfigSchema Phase2Schema::create() {
    ConfigSchema schema = Phase1Schema::create();
    schema.version = "2.0.0";
    schema.filename = "phase2_schema.json";
    
    // Add Phase 2 specific fields
    ConfigSchema::Field field;
    
    // Neuromodulation settings
    field.key = "dopamine_max";
    field.type = "double";
    field.required = false;
    field.description = "Maximum dopamine level (0-10)";
    field.min = 0.0;
    field.max = 10.0;
    schema.addField(field);
    
    field.key = "dopamine_decay_rate";
    field.type = "double";
    field.required = false;
    field.description = "Dopamine decay rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Working memory settings
    field.key = "working_memory_size";
    field.type = "int";
    field.required = false;
    field.description = "Working memory capacity (1-10000 items)";
    field.min = 1;
    field.max = 10000;
    schema.addField(field);
    
    field.key = "working_memory_decay_rate";
    field.type = "double";
    field.required = false;
    field.description = "Working memory decay rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Episodic memory settings
    field.key = "episodic_memory_size";
    field.type = "int";
    field.required = false;
    field.description = "Episodic memory capacity (1-100000 episodes)";
    field.min = 1;
    field.max = 100000;
    schema.addField(field);
    
    field.key = "episodic_memory_similarity_threshold";
    field.type = "double";
    field.required = false;
    field.description = "Episodic memory similarity threshold (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Semantic memory settings
    field.key = "semantic_memory_learning_rate";
    field.type = "double";
    field.required = false;
    field.description = "Semantic memory learning rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Prediction error settings
    field.key = "prediction_error_decay_rate";
    field.type = "double";
    field.required = false;
    field.description = "Prediction error decay rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Novelty settings
    field.key = "novelty_threshold";
    field.type = "double";
    field.required = false;
    field.description = "Novelty detection threshold (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Curiosity settings
    field.key = "curiosity_strength";
    field.type = "double";
    field.required = false;
    field.description = "Curiosity modulation strength (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    return schema;
}

} // namespace nlm
