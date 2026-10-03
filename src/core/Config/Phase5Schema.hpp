// Phase 5 schema implementation
#pragma once

#include "Phase5Schema.hpp"
#include "Phase4Schema.hpp"
#include <string>

namespace nlm {

Phase5Schema::Phase5Schema() : ConfigSchema() {
    // Initialize with Phase 5 specific settings
    setName("Phase 5 Schema");
    setVersion("5.0.0");
}

bool Phase5Schema::validate(const Config& config) const {
    // First validate base Phase 4 schema
    if (!Phase4Schema::validate(config)) {
        return false;
    }
    
    // Phase 5 specific validations
    bool valid = true;
    
    // Validate meta-learning settings
    if (config.has("meta_learning_rate")) {
        auto rate = config.get<double>("meta_learning_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            valid = false;
        }
    }
    
    if (config.has("meta_learning_threshold")) {
        auto threshold = config.get<double>("meta_learning_threshold");
        if (threshold && (*threshold < 0 || *threshold > 1)) {
            valid = false;
        }
    }
    
    // Validate self-organization settings
    if (config.has("self_organization_strength")) {
        auto strength = config.get<double>("self_organization_strength");
        if (strength && (*strength < 0 || *strength > 1)) {
            valid = false;
        }
    }
    
    if (config.has("self_organization_decay_rate")) {
        auto decayRate = config.get<double>("self_organization_decay_rate");
        if (decayRate && (*decayRate < 0 || *decayRate > 1)) {
            valid = false;
        }
    }
    
    // Validate integration settings
    if (config.has("integration_learning_rate")) {
        auto rate = config.get<double>("integration_learning_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            valid = false;
        }
    }
    
    if (config.has("integration_coherence_target")) {
        auto target = config.get<double>("integration_coherence_target");
        if (target && (*target < 0 || *target > 1)) {
            valid = false;
        }
    }
    
    return valid;
}

std::string Phase5Schema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Phase 4 errors
    std::string baseErrors = Phase4Schema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Phase 5 specific validation errors
    if (config.has("meta_learning_rate")) {
        auto rate = config.get<double>("meta_learning_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: meta_learning_rate must be between 0 and 1";
        }
    }
    
    if (config.has("meta_learning_threshold")) {
        auto threshold = config.get<double>("meta_learning_threshold");
        if (threshold && (*threshold < 0 || *threshold > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: meta_learning_threshold must be between 0 and 1";
        }
    }
    
    if (config.has("self_organization_strength")) {
        auto strength = config.get<double>("self_organization_strength");
        if (strength && (*strength < 0 || *strength > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: self_organization_strength must be between 0 and 1";
        }
    }
    
    if (config.has("self_organization_decay_rate")) {
        auto decayRate = config.get<double>("self_organization_decay_rate");
        if (decayRate && (*decayRate < 0 || *decayRate > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: self_organization_decay_rate must be between 0 and 1";
        }
    }
    
    if (config.has("integration_learning_rate")) {
        auto rate = config.get<double>("integration_learning_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: integration_learning_rate must be between 0 and 1";
        }
    }
    
    if (config.has("integration_coherence_target")) {
        auto target = config.get<double>("integration_coherence_target");
        if (target && (*target < 0 || *target > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 5 Validation Error: integration_coherence_target must be between 0 and 1";
        }
    }
    
    return errors;
}

ConfigSchema Phase5Schema::create() {
    ConfigSchema schema = Phase4Schema::create();
    schema.version = "5.0.0";
    schema.filename = "phase5_schema.json";
    
    // Add Phase 5 specific fields
    ConfigSchema::Field field;
    
    // Meta-learning settings
    field.key = "meta_learning_rate";
    field.type = "double";
    field.required = false;
    field.description = "Meta-learning rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    field.key = "meta_learning_threshold";
    field.type = "double";
    field.required = false;
    field.description = "Meta-learning threshold for switching strategies (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Self-organization settings
    field.key = "self_organization_strength";
    field.type = "double";
    field.required = false;
    field.description = "Self-organization strength (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    field.key = "self_organization_decay_rate";
    field.type = "double";
    field.required = false;
    field.description = "Self-organization decay rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Integration settings
    field.key = "integration_learning_rate";
    field.type = "double";
    field.required = false;
    field.description = "Integration learning rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    field.key = "integration_coherence_target";
    field.type = "double";
    field.required = false;
    field.description = "Target system coherence level (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    return schema;
}

} // namespace nlm
