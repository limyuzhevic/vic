// Simple test to verify enhanced configuration system
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Enhanced Configuration System" << std::endl;
    std::cout << "=================================" << std::endl;
    
    nlm::Config config;
    
    // Test basic configuration operations
    config.set("neuron_count", 1000);
    config.set("connection_probability", 0.1);
    config.set("plasticity_learning_rate", 0.01);
    config.set("max_memory", 1024);
    
    std::cout << "1. Basic configuration set: " << (config.has("neuron_count") ? "OK" : "FAILED") << std::endl;
    
    // Test get operations
    auto neuron_count = config.get<int64_t>("neuron_count");
    std::cout << "2. Get operation: " << (neuron_count.has_value() ? "OK" : "FAILED") << std::endl;
    
    // Test validation
    std::cout << "3. Schema validation: " << (config.validate() ? "OK" : "FAILED") << std::endl;
    
    // Test cross-parameter validation
    bool cross_valid = config.validateCrossParameters();
    std::cout << "4. Cross-parameter validation: " << (cross_valid ? "OK" : "FAILED") << std::endl;
    
    // Test optimization analysis
    auto optimizations = config.analyzeOptimizations();
    std::cout << "5. Optimization analysis: " << (optimizations.empty() ? "OK" : "FAILED") << std::endl;
    
    // Test serialization
    std::string json_output = config.exportToString();
    std::cout << "6. JSON serialization: " << (!json_output.empty() ? "OK" : "FAILED") << std::endl;
    
    // Test deserialization
    nlm::Config config2;
    bool import_success = config2.importFromString(json_output);
    std::cout << "7. JSON deserialization: " << (import_success ? "OK" : "FAILED") << std::endl;
    
    // Test template creation
    nlm::ConfigTemplate template;
    template.name = "test_template";
    template.values["neuron_count"] = "500";
    bool template_success = config.createTemplate(template);
    std::cout << "8. Template creation: " << (template_success ? "OK" : "FAILED") << std::endl;
    
    // Test validation history
    config.enableValidationHistory(true);
    auto history = config.getValidationHistory();
    std::cout << "9. Validation history: " << (history.empty() ? "OK" : "FAILED") << std::endl;
    
    // Test monitoring
    config.enableMonitoring(true);
    auto monitoring = config.getMonitoringData();
    std::cout << "10. Monitoring: " << (!monitoring.empty() ? "OK" : "FAILED") << std::endl;
    
    // Test configuration comparison
    auto diff = config.diff(config2);
    std::cout << "11. Configuration diffing: " << (diff.size() > 0 ? "OK" : "FAILED") << std::endl;
    
    std::cout << "\nEnhanced Configuration System Test Complete!" << std::endl;
    return 0;
}
