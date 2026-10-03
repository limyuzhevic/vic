// Environment-specific schema definitions
#pragma once

#include "DevelopmentSchema.hpp"
#include "ProductionSchema.hpp"
#include <string>

namespace nlm {

// Development environment schema
DevelopmentSchema::DevelopmentSchema() : ConfigSchema() {
    // Initialize with development-specific settings
    setName("Development Schema");
    setVersion("dev-1.0.0");
}

bool DevelopmentSchema::validate(const Config& config) const {
    // First validate base Phase 6 schema
    if (!Phase6Schema::validate(config)) {
        return false;
    }
    
    // Development specific validations
    bool valid = true;
    
    // Validate development mode settings
    if (config.has("enable_debug_mode") && config.get<bool>("enable_debug_mode")) {
        // In debug mode, certain settings should be validated
        if (config.has("validation_strategy")) {
            auto strategy = config.get<std::string>("validation_strategy");
            if (strategy && (*strategy != "strict" && *strategy != "lenient" && *strategy != "report")) {
                valid = false;
            }
        }
    }
    
    return valid;
}

std::string DevelopmentSchema::getValidationErrors(const Config& config) const {
    std::string errors;
    
    // Get base Phase 6 errors
    std::string baseErrors = Phase6Schema::getValidationErrors(config);
    if (!baseErrors.empty()) {
        errors += baseErrors + "\n";
    }
    
    // Development specific validation errors
    if (config.has("enable_debug_mode") && config.get<bool>("enable_debug_mode")) {
        if (config.has("validation_strategy")) {
            auto strategy = config.get<std::string>("validation_strategy");
            if (strategy && (*strategy != "strict" && *strategy != "lenient" && *strategy != "report")) {
                if (!errors.empty()) errors += "\n";
                errors += "Development Schema Validation Error: validation_strategy must be one of: strict, lenient, report";
            }
        }
    }
    
    return errors;
}

ConfigSchema DevelopmentSchema::create() {
    ConfigSchema schema = Phase6Schema::create();
    schema.version = "dev-1.0.0";
    schema.filename = "development_schema.json";
    
    // Add development-specific fields
    ConfigSchema::Field field;
    
    field.key = "enable_debug_mode";
    field.type = "bool";
    field.required = false;
    field.description = "Enable debug mode";
    schema.addField(field);
    
    field.key = "enable_validation_detailed";
    field.type = "bool";
    field.required = false;
    field.description = "Enable detailed validation output";
    schema.addField(field);
    
    field.key = "validation_strategy";
    field.type = "string";
    field.required = false;
    field.description = "Validation strategy (strict, lenient, report)";
    field.allowedValues = {"strict", "lenient", "report"};
    schema.addField(field);
    
    field.key = "enable_schema_docs";
    field.type = "bool";
    field.required = false;
    field.description = "Generate schema documentation";
    schema.addField(field);
    
    field.key = "schema_docs_output_dir";
    field.type = "string";
    field.required = false;
    field.description = "Schema documentation output directory";
    schema.addField(field);
    
    return schema;
}

// Production environment schema
ProductionSchema::ProductionSchema() : ConfigSchema() {
    // Initialize with production-specific settings
    setName("Production Schema");
    setVersion("prod-1.0.0");
}

bool ProductionSchema::validate(const Config& config) const {
    // First validate base Phase 6 schema
    if (!Phase6Schema::validate(config)) {
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
    
    // Get base Phase 6 errors
    std::string baseErrors = Phase6Schema::getValidationErrors(config);
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
    ConfigSchema schema = Phase6Schema::create();
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
