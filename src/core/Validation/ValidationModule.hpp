// Validation Module Header
// This file includes all validation framework headers

#pragma once

// Core validation framework headers
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"
#include "core/Validation/ValidationIntegration.hpp"

// Validation result types
#include "core/Validation/ValidationResult.hpp"

// Validation configuration
#include "core/Validation/ValidationConfig.hpp"

// Validation rules
#include "core/Validation/ValidationRules.hpp"

// Integration with existing configuration system
#include "core/Validation/ValidationIntegration.hpp"

// Export validation functionality
namespace nlm {
    // Main validation framework instance
    extern ValidationFramework gValidationFramework;
    
    // Initialize validation framework
    bool initializeValidationFramework(const std::string& configFile = "configs/validation.cfg");
    
    // Validate configuration
    bool validateConfiguration(const Config& config);
    
    // Get validation results
    const std::vector<ValidationResultDetail>& getValidationResults();
    
    // Save validation results
    bool saveValidationResults(const std::string& filepath, const std::string& format = "text");
    
    // Load validation configuration
    bool loadValidationConfiguration(const std::string& filepath);
    
    // Get validation framework statistics
    std::string getValidationStatistics();
}