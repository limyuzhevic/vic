// Phase 4 schema implementation
#pragma once

#include "Phase4Schema.hpp"
#include "Phase3Schema.hpp"
#include <string>

namespace nlm {

Phase4Schema::Phase4Schema() : ConfigSchema() {
    // Initialize with Phase 4 specific settings
    setName("Phase 4 Schema");
    setVersion("4.0.0");
}

bool Phase4Schema::validate(const Config& config) const {
    // First validate base Phase 3 schema
    if (!Phase3Schema::validate(config)) {
        return false;
    }
    
    // Phase 4 specific validations
    bool valid = true;
    
    // Validate motor system settings
    if (config.has("motor_noise_level")) {
        auto noise = config.get<double>("motor_noise_level");
        if (noise && (*noise < 0 || *noise > 1)) {
            valid = false;
        }
    }
    
    if (config.has("motor_gain")) {
        auto gain = config.get<double>("motor_gain");
        if (gain && (*gain < 0 || *gain > 10)) {
            valid = false;
        }
    }
    
    // Validate action selection settings
    if (config.has("action_selection_temperature")) {
        auto temp = config.get<double>("action_selection_temperature");
        if (temp && (*temp < 0)) {
            valid = false;
        }
    }
    
    if (config.has("action_selection_exploration_rate")) {
        auto rate = config.get<double>("action_selection_exploration_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            valid = false;
        }
    }
    
    // Validate reward prediction settings
    if (config.has("reward_prediction_error_scaling")) {
        auto scaling = config.get<double>("reward_prediction_error_scaling");
        if (scaling && (*scaling < 0 || *scaling > 10)) {
            valid = false;
        }
    }
    
    return valid;
}

std::string Phase4Schema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Phase 3 errors
    std::string baseErrors = Phase3Schema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Phase 4 specific validation errors
    if (config.has("motor_noise_level")) {
        auto noise = config.get<double>("motor_noise_level");
        if (noise && (*noise < 0 || *noise > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 4 Validation Error: motor_noise_level must be between 0 and 1";
        }
    }
    
    if (config.has("motor_gain")) {
        auto gain = config.get<double>("motor_gain");
        if (gain && (*gain < 0 || *gain > 10)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 4 Validation Error: motor_gain must be between 0 and 10";
        }
    }
    
    if (config.has("action_selection_temperature")) {
        auto temp = config.get<double>("action_selection_temperature");
        if (temp && (*temp < 0)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 4 Validation Error: action_selection_temperature must be non-negative";
        }
    }
    
    if (config.has("action_selection_exploration_rate")) {
        auto rate = config.get<double>("action_selection_exploration_rate");
        if (rate && (*rate < 0 || *rate > 1)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 4 Validation Error: action_selection_exploration_rate must be between 0 and 1";
        }
    }
    
    if (config.has("reward_prediction_error_scaling")) {
        auto scaling = config.get<double>("reward_prediction_error_scaling");
        if (scaling && (*scaling < 0 || *scaling > 10)) {
            if (!errors.empty()) errors += "\n";
            errors += "Phase 4 Validation Error: reward_prediction_error_scaling must be between 0 and 10";
        }
    }
    
    return errors;
}

ConfigSchema Phase4Schema::create() {
    ConfigSchema schema = Phase3Schema::create();
    schema.version = "4.0.0";
    schema.filename = "phase4_schema.json";
    
    // Add Phase 4 specific fields
    ConfigSchema::Field field;
    
    // Motor system settings
    field.key = "motor_noise_level";
    field.type = "double";
    field.required = false;
    field.description = "Motor noise level (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    field.key = "motor_gain";
    field.type = "double";
    field.required = false;
    field.description = "Motor gain factor (0-10)";
    field.min = 0.0;
    field.max = 10.0;
    schema.addField(field);
    
    // Action selection settings
    field.key = "action_selection_temperature";
    field.type = "double";
    field.required = false;
    field.description = "Action selection temperature (must be non-negative)";
    field.min = 0.0;
    schema.addField(field);
    
    field.key = "action_selection_exploration_rate";
    field.type = "double";
    field.required = false;
    field.description = "Action selection exploration rate (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    // Reward prediction settings
    field.key = "reward_prediction_error_scaling";
    field.type = "double";
    field.required = false;
    field.description = "Reward prediction error scaling factor (0-10)";
    field.min = 0.0;
    field.max = 10.0;
    schema.addField(field);
    
    // Action outcomes
    field.key = "action_persistence";
    field.type = "double";
    field.required = false;
    field.description = "Action persistence (0-1)";
    field.min = 0.0;
    field.max = 1.0;
    schema.addField(field);
    
    return schema;
}

} // namespace nlm
