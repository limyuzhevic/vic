// Test program to verify configuration improvements
#include <iostream>
#include <memory>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

int main() {
    std::cout << "=== NLM Configuration Test ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Test 1: Create default config
    std::cout << "\n1. Testing default configuration..." << std::endl;
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", int64_t(1000));
    config->set("brain.connection_probability", 0.1f);
    config->set("plasticity.stdp.enable", true);
    config->set("neuromodulation.curiosity.enable", false);
    config->set("learning_rate", 0.01);
    
    std::cout << "Default config created with " << config->getKeys().size() << " entries" << std::endl;
    
    // Test 2: Test JSON loading (if file exists)
    std::cout << "\n2. Testing JSON configuration..." << std::endl;
    auto config2 = std::make_shared<nlm::Config>();
    
    // Try to load from the enhanced config file
    if (config2->loadFromFile("configs/enhanced_config.json")) {
        std::cout << "JSON configuration loaded successfully!" << std::endl;
        std::cout << "Number of entries: " << config2->getKeys().size() << std::endl;
        
        // Test getting values
        auto neuronCount = config2->getOr<int64_t>("brain.neuron_count", 0);
        std::cout << "Neuron count: " << neuronCount << std::endl;
        
        auto plasticityEnabled = config2->getOr<bool>("plasticity.stdp.enable", false);
        std::cout << "Plasticity enabled: " << (plasticityEnabled ? "true" : "false") << std::endl;
        
        auto curiosityEnabled = config2->getOr<bool>("neuromodulation.curiosity.enable", false);
        std::cout << "Curiosity enabled: " << (curiosityEnabled ? "true" : "false") << std::endl;
    } else {
        std::cout << "JSON configuration file not found or failed to load" << std::endl;
    }
    
    // Test 3: Test command line argument parsing
    std::cout << "\n3. Testing command line argument parsing..." << std::endl;
    auto config3 = std::make_shared<nlm::Config>();
    
    // Simulate command line arguments
    int argc = 7;
    char* argv[] = {
        (char*)"test",
        (char*)"--neuron_count=512",
        (char*)"--connection_probability=0.2",
        (char*)"-plasticity_learning_rate=0.05",
        (char*)"--enable_curiosity=true"
    };
    
    config3->loadFromArgs(argc, argv);
    std::cout << "Command line config loaded with " << config3->getKeys().size() << " entries" << std::endl;
    
    // Test getting values from command line
    auto neuronsFromArgs = config3->getOr<int64_t>("neuron_count", 0);
    std::cout << "Neurons from args: " << neuronsFromArgs << std::endl;
    
    auto curiosityFromArgs = config3->getOr<bool>("enable_curiosity", false);
    std::cout << "Curiosity from args: " << (curiosityFromArgs ? "true" : "false") << std::endl;
    
    // Test 4: Test config validation
    std::cout << "\n4. Testing configuration validation..." << std::endl;
    
    // Define validation rules
    auto validationRule = [](const nlm::Config& cfg) -> bool {
        auto neurons = cfg.getOr<int64_t>("brain.neuron_count", 0);
        if (neurons == 0) {
            return false;
        }
        return neurons > 0 && neurons <= 100000;
    };
    
    // Add validation rule
    // Note: This would require adding validation infrastructure to Config class
    // For now, we'll just demonstrate the concept
    std::cout << "Validation concept demonstrated" << std::endl;
    
    // Test 5: Test config summary
    std::cout << "\n5. Configuration summary..." << std::endl;
    std::cout << "=== Config Summary ===" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Test 6: Test save to simple format
    std::cout << "\n6. Testing save to simple format..." << std::endl;
    auto config4 = std::make_shared<nlm::Config>();
    config4->set("test_key", "test_value");
    config4->set("test_number", 42);
    config4->set("test_boolean", true);
    config4->set("test_array", std::vector<double>{1.0, 2.0, 3.0});
    
    if (config4->saveToFile("test_config.cfg")) {
        std::cout << "Configuration saved to test_config.cfg" << std::endl;
    } else {
        std::cout << "Failed to save configuration" << std::endl;
    }
    
    std::cout << "\n=== Configuration tests completed ===" << std::endl;
    return 0;
}
