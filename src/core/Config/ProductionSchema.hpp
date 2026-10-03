// Production environment schema implementation
#pragma once

#include "ProductionSchema.hpp"
#include "DevelopmentSchema.hpp"
#include <string>

namespace nlm {

ProductionSchema::ProductionSchema() : ConfigSchema() {
    // Initialize with production-specific settings
    setName("Production Schema");
    setVersion("prod-1.0.0");
}

bool ProductionSchema::validate(const Config& config) const {
    // First validate base Development schema
    if (!DevelopmentSchema::validate(config)) {
        return false;
    }
    
    // Production specific validations
    bool valid = true;
    
    // Validate production mode settings
    if (config.has("enable_hot_reloading")) {
        auto hotReload = config.get<bool>("enable_hot_reloading");
        if (hotReload && hotReload) {
            // If hot reloading is enabled, check for performance monitoring
            if (config.has("performance_sample_rate")) {
                auto sampleRate = config.get<int>("performance_sample_rate");
                if (sampleRate && (*sampleRate < 1 || *sampleRate > 1000)) {
                    valid = false;
                }
            }
        }
    }
    
    return valid;
}

std::string ProductionSchema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Development schema errors
    std::string baseErrors = DevelopmentSchema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Production specific validation errors
    if (config.has("enable_hot_reloading")) {
        auto hotReload = config.get<bool>("enable_hot_reloading");
        if (hotReload && hotReload) {
            if (config.has("performance_sample_rate")) {
                auto sampleRate = config.get<int>("performance_sample_rate");
                if (sampleRate && (*sampleRate < 1 || *sampleRate > 1000)) {
                    if (!errors.empty()) errors += "\n";
                    errors += "Production Schema Validation Error: performance_sample_rate must be between 1 and 1000 when hot reloading is enabled";
                }
            }
        }
    }
    
    return errors;
}

ConfigSchema ProductionSchema::create() {
    ConfigSchema schema = DevelopmentSchema::create();
    schema.version = "prod-1.0.0";
    schema.filename = "production_schema.json";
    
    // Add production-specific fields
    ConfigSchema::Field field;
    
    field.key = "performance_monitoring";
    field.type = "bool";
    field.required = false;
    field.description = "Enable performance monitoring";
    schema.addField(field);
    
    field.key = "performance_sample_rate";
    field.type = "int";
    field.required = false;
    field.description = "Performance metrics sampling rate (1-1000 Hz)";
    field.min = 1;
    field.max = 1000;
    schema.addField(field);
    
    field.key = "error_reporting_enabled";
    field.type = "bool";
    field.required = false;
    field.description = "Enable error reporting";
    schema.addField(field);
    
    field.key = "error_reporting_endpoint";
    field.type = "string";
    field.required = false;
    field.description = "Error reporting endpoint URL";
    schema.addField(field);
    
    field.key = "log_retention_days";
    field.type = "int";
    field.required = false;
    field.description = "Log retention period in days (1-365)";
    field.min = 1;
    field.max = 365;
    schema.addField(field);
    
    field.key = "enable_hot_reloading";
    field.type = "bool";
    field.required = false;
    field.description = "Enable configuration hot-reloading";
    schema.addField(field);
    
    field.key = "hot_reload_check_interval";
    field.type = "double";
    field.required = false;
    field.description = "Hot-reload check interval in seconds (0.1-60.0)";
    field.min = 0.1;
    field.max = 60.0;
    schema.addField(field);
    
    return schema;
}

} // namespace nlm
