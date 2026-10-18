#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Configuration Validation System" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    nlm::Config config;
    
    // Test basic configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    std::cout << "1. Basic configuration set" << std::endl;
    
    // Test validation
    bool valid = config.validateConfig();
    std::cout << "2. Configuration validation: " << (valid ? "PASSED" : "FAILED") << std::endl;
    
    // Test validation report
    auto report = config.getValidationReport();
    std::cout << "3. Validation report: " << report.getSummary() << std::endl;
    
    // Test schema validation
    bool schemaValid = config.validateAgainstSchema();
    std::cout << "4. Schema validation: " << (schemaValid ? "PASSED" : "FAILED") << std::endl;
    
    // Test parameter validation
    std::cout << "5. Parameter validation:" << std::endl;
    std::cout << "   - Neuron count: " << (config.validateNeuronCount() ? "VALID" : "INVALID") << std::endl;
    std::cout << "   - Synapse density: " << (config.validateSynapseDensity() ? "VALID" : "INVALID") << std::endl;
    std::cout << "   - Time parameters: " << (config.validateTimeParameters() ? "VALID" : "INVALID") << std::endl;
    
    // Test type safety
    auto neuronCount = config.get<size_t>("neuron_count");
    std::cout << "6. Type safety: " << (neuronCount.has_value() ? "PASSED" : "FAILED") << std::endl;
    
    // Test schema export
    std::string schema = config.exportSchema();
    std::cout << "7. Schema export: " << (schema.empty() ? "FAILED" : "PASSED") << std::endl;
    
    // Test parameter info
    auto paramInfo = config.getParameterInfo("neuron_count");
    std::cout << "8. Parameter info: " << (paramInfo.has_value() ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << std::endl;
    std::cout << "All tests completed!" << std::endl;
    
    return 0;
}