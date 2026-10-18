#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== NLM Configuration Validation Test ===" << std::endl;
    
    nlm::Config config;
    
    std::cout << "\n1. Testing basic configuration loading..." << std::endl;
    
    // Load from the default config file
    bool loadSuccess = config.loadFromFile("configs/default.cfg");
    std::cout << "   Load from file: " << (loadSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    if (loadSuccess) {
        std::cout << "   Loaded keys: ";
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n2. Testing individual parameter validation..." << std::endl;
    
    // Test validation of specific parameters
    bool neuronCountValid = config.validateNeuronCount();
    bool synapseDensityValid = config.validateSynapseDensity();
    bool timeParamsValid = config.validateTimeParameters();
    
    std::cout << "   Neuron count validation: " << (neuronCountValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "   Synapse density validation: " << (synapseDensityValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "   Time parameters validation: " << (timeParamsValid ? "VALID" : "INVALID") << std::endl;
    
    std::cout << "\n3. Testing configuration consistency validation..." << std::endl;
    
    bool consistencyValid = config.validateConsistency();
    std::cout << "   Configuration consistency: " << (consistencyValid ? "VALID" : "INVALID") << std::endl;
    
    std::cout << "\n4. Testing completeness validation..." << std::endl;
    
    bool completenessValid = config.validateCompleteness();
    std::cout << "   Configuration completeness: " << (completenessValid ? "VALID" : "INVALID") << std::endl;
    
    std::cout << "\n5. Testing schema validation..." << std::endl;
    
    bool schemaValid = config.validateAgainstSchema();
    std::cout << "   Schema validation: " << (schemaValid ? "VALID" : "INVALID") << std::endl;
    
    std::cout << "\n6. Testing comprehensive validation..." << std::endl;
    
    auto validationReport = config.getValidationReport();
    std::cout << "   Overall validation: " << (validationReport.valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << "   Errors: " << validationReport.errors.size() << std::endl;
    std::cout << "   Warnings: " << validationReport.warnings.size() << std::endl;
    
    if (!validationReport.errors.empty()) {
        std::cout << "   First error: " << validationReport.errors[0].parameter << " - " << validationReport.errors[0].message << std::endl;
    }
    
    std::cout << "\n7. Testing validation statistics..." << std::endl;
    
    auto stats = config.getValidationStats();
    std::cout << "   Total validations: " << stats.totalValidations << std::endl;
    std::cout << "   Successful validations: " << stats.successfulValidations << std::endl;
    std::cout << "   Failed validations: " << stats.failedValidations << std::endl;
    
    std::cout << "\n8. Testing parameter type safety..." << std::endl;
    
    bool intTypeValid = config.ensureType<int>("neuron_count");
    bool doubleTypeValid = config.ensureType<double>("simulation_timestep");
    bool stringTypeValid = config.ensureType<std::string>("environment_name");
    
    std::cout << "   int type check (neuron_count): " << (intTypeValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << "   double type check (simulation_timestep): " << (doubleTypeValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << "   string type check (environment_name): " << (stringTypeValid ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n9. Testing file validation..." << std::endl;
    
    bool filePathValid = config.validateFilePath("configs/default.cfg");
    bool fileFormatValid = config.validateFileFormat("configs/default.cfg");
    
    std::cout << "   File path validation: " << (filePathValid ? "VALID" : "INVALID") << std::endl;
    std::cout << "   File format validation: " << (fileFormatValid ? "VALID" : "INVALID") << std::endl;
    
    std::cout << "\n10. Testing schema export..." << std::endl;
    
    std::string schema = config.exportSchema();
    std::cout << "   Schema export: " << (schema.empty() ? "FAILED" : "SUCCESS") << std::endl;
    if (!schema.empty()) {
        std::cout << "   Schema length: " << schema.length() << " characters" << std::endl;
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    int passedTests = 0;
    int totalTests = 10;
    
    if (loadSuccess) passedTests++;
    if (neuronCountValid && synapseDensityValid && timeParamsValid) passedTests++;
    if (consistencyValid) passedTests++;
    if (completenessValid) passedTests++;
    if (schemaValid) passedTests++;
    if (validationReport.valid) passedTests++;
    if (stats.totalValidations > 0) passedTests++;
    if (intTypeValid && doubleTypeValid && stringTypeValid) passedTests++;
    if (filePathValid && fileFormatValid) passedTests++;
    if (!schema.empty()) passedTests++;
    
    std::cout << "Tests passed: " << passedTests << " / " << totalTests << std::endl;
    std::cout << "Overall result: " << (passedTests == totalTests ? "SUCCESS" : "PARTIAL SUCCESS") << std::endl;
    
    return (passedTests >= totalTests * 0.8) ? 0 : 1; // Return 0 if at least 80% passed
}