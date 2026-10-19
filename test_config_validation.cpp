#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

void testConfigValidation() {
    std::cout << "=== Testing Configuration Validation ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test 1: Valid configuration
    std::cout << "\n1. Testing valid configuration...";
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
    config->set("region_count", static_cast<int64_t>(5), ConfigSource::File);
    config->set("connection_probability", 0.1f, ConfigSource::File);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::File);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::File);
    config->set("stdp_tau", 20.0f, ConfigSource::File);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::File);
    config->set("pruning_rate", 0.00001f, ConfigSource::File);
    config->set("simulation_timestep", 0.001, ConfigSource::File);
    config->set("replay_interval", 100, ConfigSource::File);
    config->set("consolidation_interval", 1000, ConfigSource::File);
    
    if (config->validate()) {
        std::cout << " PASSED" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " FAILED" << std::endl;
    }
    
    // Test 2: Invalid configuration (neuron_count out of range)
    std::cout << "\n2. Testing invalid configuration (neuron_count > 1,000,000)...";
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(2000000), ConfigSource::File);
    
    if (!config->validate()) {
        std::cout << " Correctly rejected" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " Incorrectly accepted" << std::endl;
    }
    
    // Test 3: Invalid configuration (connection_probability > 1.0)
    std::cout << "\n3. Testing invalid configuration (connection_probability > 1.0)...";
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
    config->set("connection_probability", 1.5f, ConfigSource::File);
    
    if (!config->validate()) {
        std::cout << " Correctly rejected" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " Incorrectly accepted" << std::endl;
    }
    
    // Test 4: Missing parameter dependency
    std::cout << "\n4. Testing missing parameter dependency...";
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
    
    if (!config->validate()) {
        std::cout << " Correctly rejected" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " Incorrectly accepted" << std::endl;
    }
    
    // Test 5: File loading with validation
    std::cout << "\n5. Testing file loading with validation...";
    std::string testConfigFile = "test_config.txt";
    
    // Create a test configuration file
    std::ofstream testFile(testConfigFile);
    testFile << "# Test Configuration File\n";
    testFile << "neuron_count = 500\n";
    testFile << "region_count = 2\n";
    testFile << "connection_probability = 0.2\n";
    testFile << "stdp_ltp_weight = 0.02\n";
    testFile << "stdp_ltd_weight = 0.025\n";
    testFile << "stdp_tau = 25.0\n";
    testFile << "synaptogenesis_rate = 0.0002\n";
    testFile << "pruning_rate = 0.00002\n";
    testFile << "simulation_timestep = 0.002\n";
    testFile << "replay_interval = 200\n";
    testFile << "consolidation_interval = 2000\n";
    testFile.close();
    
    config->clear();
    bool loadSuccess = config->loadFromFile(testConfigFile);
    if (loadSuccess && config->validate()) {
        std::cout << " PASSED" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " FAILED" << std::endl;
    }
    
    // Test 6: Command line argument validation
    std::cout << "\n6. Testing command line argument validation...";
    const char* argv[] = {"test", "--neuron_count=100", "--region_count=10", "--connection_probability=0.5"};
    int argc = 4;
    
    config->clear();
    bool argsSuccess = config->loadFromArgs(argc, const_cast<char**>(argv));
    if (argsSuccess && config->validate()) {
        std::cout << " PASSED" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << " FAILED" << std::endl;
    }
    
    // Clean up test file
    std::remove(testConfigFile.c_str());
    
    std::cout << "\n=== Configuration Validation Tests Complete ===" << std::endl;
}

int main() {
    // Initialize logger for testing
    Logger::getInstance().initialize("config_test.log", "INFO");
    
    testConfigValidation();
    
    return 0;
}