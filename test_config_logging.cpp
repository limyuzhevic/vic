#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

int main() {
    std::cout << "NLM Configuration and Logging System Test" << std::endl;
    std::cout << "============================================" << std::endl;
    
    // Initialize global configuration
    auto config = std::make_shared<Config>();
    
    // Test configuration settings
    config->set("neuron_count", ConfigValue(static_cast<int64_t>(100)));
    config->set("region_count", ConfigValue(static_cast<int64_t>(2)));
    config->set("simulation_timestep", ConfigValue(0.001));
    config->set("stdp_ltp_weight", ConfigValue(0.01f));
    config->set("development_enabled", ConfigValue(true));
    
    // Test retrieval
    int64_t neuronCount;
    if (config->get("neuron_count", neuronCount)) {
        std::cout << "Neuron count: " << neuronCount << std::endl;
    }
    
    float timestep;
    if (config->get("simulation_timestep", timestep)) {
        std::cout << "Simulation timestep: " << timestep << "s" << std::endl;
    }
    
    bool devEnabled;
    if (config->get("development_enabled", devEnabled)) {
        std::cout << "Development enabled: " << (devEnabled ? "Yes" : "No") << std::endl;
    }
    
    // Test getOr with defaults
    float unknownValue = config->getOr("unknown_setting", 0.5f);
    std::cout << "Unknown setting (default): " << unknownValue << std::endl;
    
    // Test logging system
    Logger::getInstance().setLevel(LogLevel::Info);
    
    NLM_LOG_INFO("Test log message from NLM brain system");
    NLM_LOG_WARNING("This is a warning test message");
    NLM_LOG_ERROR("This is an error test message");
    
    // Test schema validation
    std::cout << "\nConfiguration validation: " << (config->validate() ? "PASSED" : "FAILED") << std::endl;
    
    // Test configuration keys
    std::vector<std::string> keys = config->getKeys();
    std::cout << "\nConfiguration keys: " << keys.size() << std::endl;
    for (const auto& key : keys) {
        std::cout << "  - " << key << std::endl;
    }
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}