// Test program demonstrating the improved NLM configuration system
// This shows how to use the enhanced configuration features

#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

void demonstrateBasicConfiguration() {
    std::cout << "=== Basic Configuration Demo ===\n" << std::endl;
    
    nlm::Config config;
    
    // Set basic values
    config.set("simulation.timestep", 0.001);
    config.set("simulation.neuronCount", 1000);
    config.set("simulation.regionCount", 1);
    config.set("plasticity.stdpEnabled", true);
    config.set("plasticity.hebbianEnabled", false);
    
    // Get values with type safety
    double timestep = config.getOr<double>("simulation.timestep", 0.001);
    int neuronCount = config.getOr<int>("simulation.neuronCount", 1000);
    bool stdpEnabled = config.getOr<bool>("plasticity.stdpEnabled", true);
    
    std::cout << "Timestep: " << timestep << "s" << std::endl;
    std::cout << "Neuron count: " << neuronCount << std::endl;
    std::cout << "STDP enabled: " << (stdpEnabled ? "yes" : "no") << std::endl;
    std::cout << std::endl;
}

void demonstrateAdvancedConfiguration() {
    std::cout << "=== Advanced Configuration Demo ===\n" << std::endl;
    
    nlm::Config config;
    
    // Create configuration schema
    nlm::ConfigSchema schema;
    
    // Add schema elements
    schema.addElement(nlm::ConfigSchemaElement("simulation.timestep", nlm::ConfigDataType::DOUBLE, true, 0.001, "Simulation timestep"));
    schema.addElement(nlm::ConfigSchemaElement("simulation.neuronCount", nlm::ConfigDataType::UINT64, true, 1000ull, "Number of neurons"));
    schema.addElement(nlm::ConfigSchemaElement("plasticity.stdpWeight", nlm::ConfigDataType::FLOAT, false, 0.01f, "STDP learning rate"));
    schema.addElement(nlm::ConfigSchemaElement("plasticity.hebbianWeight", nlm::ConfigDataType::FLOAT, false, 0.005f, "Hebbian learning rate"));
    
    // Set schema
    config.setSchema(schema);
    
    // Add array values
    config.set("simulation.sensoryNeurons", std::vector<int>{10, 20, 30, 40, 50});
    config.set("plasticity.plasticityList", std::vector<std::vector<double>>{{0.1, 0.2}, {0.3, 0.4}});
    
    // Validate configuration
    bool isValid = config.isValid();
    std::cout << "Configuration valid: " << (isValid ? "yes" : "no") << std::endl;
    
    if (!isValid) {
        std::cout << "Validation errors:" << std::endl;
        auto errors = config.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Get stats
    auto stats = config.getStats();
    std::cout << "\nConfiguration statistics:" << std::endl;
    std::cout << "  Total entries: " << stats.totalEntries << std::endl;
    std::cout << "  Valid entries: " << stats.validEntries << std::endl;
    std::cout << "  Invalid entries: " << stats.invalidEntries << std::endl;
    
    std::cout << std::endl;
}

void demonstrateConfigurationComparison() {
    std::cout << "=== Configuration Comparison Demo ===\n" << std::endl;
    
    nlm::Config config1;
    nlm::Config config2;
    
    // Set same values in both configs
    config1.set("simulation.timestep", 0.001);
    config1.set("simulation.neuronCount", 1000);
    config1.set("plasticity.stdpEnabled", true);
    
    config2.set("simulation.timestep", 0.001);
    config2.set("simulation.neuronCount", 1000);
    config2.set("plasticity.stdpEnabled", true);
    
    // Compare configs
    if (config1 == config2) {
        std::cout << "Configs are identical" << std::endl;
    } else {
        std::cout << "Configs differ" << std::endl;
    }
    
    // Modify one config
    config2.set("plasticity.stdpEnabled", false);
    
    if (config1 == config2) {
        std::cout << "Configs are still identical (unexpected)" << std::endl;
    } else {
        std::cout << "Configs now differ (expected)" << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateCommandLineConfiguration() {
    std::cout << "=== Command Line Configuration Demo ===\n" << std::endl;
    
    // Simulate command-line arguments
    int argc = 7;
    const char* argv[] = {
        "nlm_demo",
        "--simulation.timestep=0.001",
        "--simulation.neuronCount=500",
        "-plasticity.stdpEnabled=true",
        "-plasticity.hebbianWeight=0.005",
        "--sim\.debug=false"
    };
    
    nlm::Config config;
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    std::cout << "Configuration from command line:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "NLM Enhanced Configuration System Demo" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    
    demonstrateBasicConfiguration();
    demonstrateAdvancedConfiguration();
    demonstrateConfigurationComparison();
    demonstrateCommandLineConfiguration();
    
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}
