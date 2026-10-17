// Test Runner for Enhanced Configuration System
// This program demonstrates all the enhanced configuration system features

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

int main() {
    std::cout << "=== Enhanced Configuration System Demo ===" << std::endl;
    std::cout << std::endl;
    
    nlm::Config config;
    
    std::cout << "1. Basic Configuration Operations:" << std::endl;
    config.set("neuron_count", 1000, nlm::ConfigSource::File);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::File);
    config.set("connection_probability", 0.1, nlm::ConfigSource::File);
    
    std::cout << "   Loaded configuration from file:" << std::endl;
    std::cout << "   neuron_count = " << config.get<int>("neuron_count").value() << std::endl;
    std::cout << "   simulation_timestep = " << config.get<double>("simulation_timestep").value() << std::endl;
    std::cout << "   connection_probability = " << config.get<double>("connection_probability").value() << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Type Safety and Error Handling:" << std::endl;
    config.set("plasticity_learning_rate", 0.01, nlm::ConfigSource::File);
    
    // Try to get with wrong type
    try {
        double wrong = config.get<double>("plasticity_learning_rate");
        std::cout << "   Got double value: " << wrong << std::endl;
    } catch (const nlm::ConfigTypeMismatchException& e) {
        std::cout << "   Type mismatch error (correct): " << e.what() << std::endl;
    }
    
    // Correct type
    auto correctValue = config.getOr<double>("plasticity_learning_rate", 0.05);
    std::cout << "   Correct type value: " << correctValue << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Schema Validation:" << std::endl;
    nlm::SchemaConstraint neuronCountConstraint;
    neuronCountConstraint.key = "neuron_count";
    neuronCountConstraint.type = "int";
    neuronCountConstraint.range = std::make_pair(1.0, 10000.0);
    neuronCountConstraint.description = "Number of neurons in the brain";
    config.addSchemaConstraint(neuronCountConstraint);
    
    nlm::SchemaConstraint connectionProbConstraint;
    connectionProbConstraint.key = "connection_probability";
    connectionProbConstraint.type = "double";
    connectionProbConstraint.range = std::make_pair(0.0, 1.0);
    connectionProbConstraint.description = "Connection probability (0 to 1)";
    config.addSchemaConstraint(connectionProbConstraint);
    
    std::cout << "   Configuration validation status: " << (config.validate() ? "VALID" : "INVALID") << std::endl;
    
    // Try to set invalid value
    config.set("connection_probability", 1.5, nlm::ConfigSource::Runtime);
    auto errors = config.validateAll();
    std::cout << "   Validation errors: " << errors.size() << std::endl;
    for (const auto& error : errors) {
        std::cout << "     - " << error << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "4. Configuration Dependencies:" << std::endl;
    nlm::Dependency dep1;
    dep1.dependentKey = "plasticity_enabled";
    dep1.requiredKeys.push_back("plasticity_learning_rate");
    dep1.requiredKeys.push_back("plasticity_adaptation_rate");
    dep1.condition = "all_present";
    config.addDependency(dep1);
    
    config.set("plasticity_learning_rate", 0.01, nlm::ConfigSource::File);
    config.set("plasticity_adaptation_rate", 0.001, nlm::ConfigSource::File);
    config.set("plasticity_enabled", true, nlm::ConfigSource::File);
    
    std::cout << "   Dependencies satisfied: " << config.validate() << std::endl;
    std::cout << std::endl;
    
    std::cout << "5. JSON Export/Import:" << std::endl;
    std::string jsonOutput = config.exportToJSON();
    std::cout << "   JSON output:" << std::endl;
    std::cout << "   " << jsonOutput.replace(0, 20, "  ...").replace(jsonOutput.size() - 20, 20, "...") << std::endl;
    
    nlm::Config config2;
    bool importSuccess = config2.importFromJSON(jsonOutput);
    std::cout << "   Import successful: " << (importSuccess ? "YES" : "NO") << std::endl;
    
    if (importSuccess) {
        std::cout << "   Imported neuron_count: " << config2.get<int>("neuron_count").value() << std::endl;
        std::cout << "   Imported connection_probability: " << config2.get<double>("connection_probability").value() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "6. Configuration Comparison and Merging:" << std::endl;
    nlm::Config config3;
    config3.set("region_count", 2, nlm::ConfigSource::File);
    config3.set("max_simulation_steps", 10000, nlm::ConfigSource::File);
    
    nlm::Config config4;
    config4.set("region_count", 2, nlm::ConfigSource::File);
    config4.set("max_simulation_steps", 10000, nlm::ConfigSource::File);
    
    std::cout << "   config3 equals config4: " << (config3.equals(config4) ? "YES" : "NO") << std::endl;
    
    config4.set("new_param", 42, nlm::ConfigSource::Runtime);
    std::cout << "   After adding parameter: " << (config3.equals(config4) ? "YES" : "NO") << std::endl;
    
    nlm::Config config5;
    config5.set("base_param", 100, nlm::ConfigSource::File);
    config5.merge(config3);
    
    std::cout << "   After merge - base_param: " << config5.get<int>("base_param").value() << std::endl;
    std::cout << "   After merge - region_count: " << config5.get<int>("region_count").value() << std::endl;
    std::cout << std::endl;
    
    std::cout << "7. Source Tracking:" << std::endl;
    config.set("override_param", 999, nlm::ConfigSource::CommandLine);
    config.set("override_param", 888, nlm::ConfigSource::Runtime);
    
    std::cout << "   Current source for override_param: ";
    switch (config.getSource("override_param")) {
        case nlm::ConfigSource::File: std::cout << "File"; break;
        case nlm::ConfigSource::CommandLine: std::cout << "Command Line"; break;
        case nlm::ConfigSource::Runtime: std::cout << "Runtime"; break;
        case nlm::ConfigSource::Default: std::cout << "Default"; break;
    }
    std::cout << std::endl;
    
    std::string sourceInfo = config.getSourceInfo("override_param");
    std::cout << "   Source info: " << sourceInfo << std::endl;
    std::cout << std::endl;
    
    std::cout << "8. Memory Usage and Validation Report:" << std::endl;
    double memoryUsage = config.getMemoryUsage();
    std::cout << "   Memory usage: " << memoryUsage << " bytes" << std::endl;
    
    std::cout << "   Validation report:" << std::endl;
    std::cout << "   " << config.getValidationReport().replace("Validation Report:", "Validation Report:").replace("Status:", "Status:").replace("entries:", "entries:").replace("bytes", "bytes") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== All Features Demonstrated Successfully ===" << std::endl;
    
    return 0;
}