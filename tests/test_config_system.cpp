#include <iostream>
#include <cassert>
#include <filesystem>

// Include the configuration system
#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"

void testConfigInitialization() {
    std::cout << "Testing Config initialization..." << std::endl;
    
    // Test Config class initialization
    nlm::Config config;
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    
    std::cout << "  ✓ Config initialization passed" << std::endl;
}

void testParameterAccess() {
    std::cout << "Testing parameter access..." << std::endl;
    
    nlm::Config config;
    
    // Test getOr for various types
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    int neurons = config.getOr<int>("neuron_count", 1000);
    float connectionProb = config.getOr<float>("connection_probability", 0.1f);
    bool devEnabled = config.getOr<bool>("development_enabled", true);
    
    assert(timestep == 0.001);
    assert(neurons == 1000);
    assert(connectionProb == 0.1f);
    assert(devEnabled == true);
    
    // Test get with missing parameter
    auto missing = config.get<double>("nonexistent_parameter");
    assert(!missing.has_value());
    
    std::cout << "  ✓ Parameter access passed" << std::endl;
}

void testParameterSetting() {
    std::cout << "Testing parameter setting..." << std::endl;
    
    nlm::Config config;
    
    // Set parameters using Config methods
    config.set("custom_int", 42);
    config.set("custom_double", 3.14159);
    config.set("custom_string", std::string("test"));
    config.set("custom_bool", true);
    
    assert(config.has("custom_int"));
    assert(config.has("custom_double"));
    assert(config.has("custom_string"));
    assert(config.has("custom_bool"));
    
    // Verify values
    assert(config.getOr<int>("custom_int", 0) == 42);
    assert(config.getOr<double>("custom_double", 0.0) == 3.14159);
    assert(config.getOr<std::string>("custom_string", "") == "test");
    assert(config.getOr<bool>("custom_bool", false) == true);
    
    std::cout << "  ✓ Parameter setting passed" << std::endl;
}

void testConfigFileOperations() {
    std::cout << "Testing configuration file operations..." << std::endl;
    
    nlm::Config config;
    
    // Set some parameters
    config.set("test_param_1", 100);
    config.set("test_param_2", std::string("hello"));
    config.set("test_param_3", 3.14159);
    config.set("test_param_4", true);
    
    // Save to file
    bool saveSuccess = config.saveToFile("test_config.cfg");
    assert(saveSuccess);
    
    // Verify file was created
    assert(std::filesystem::exists("test_config.cfg"));
    
    // Load back
    bool loadSuccess = config.loadFromFile("test_config.cfg");
    assert(loadSuccess);
    
    // Verify parameters are still there
    assert(config.getOr<int>("test_param_1", 0) == 100);
    assert(config.getOr<std::string>("test_param_2", "") == "hello");
    assert(config.getOr<double>("test_param_3", 0.0) == 3.14159);
    assert(config.getOr<bool>("test_param_4", false) == true);
    
    // Clean up
    std::filesystem::remove("test_config.cfg");
    
    std::cout << "  ✓ Configuration file operations passed" << std::endl;
}

void testParameterGroups() {
    std::cout << "Testing parameter groups..." << std::endl;
    
    // Initialize ConfigManager
    nlm::initializeConfigManager();
    nlm::ConfigManager& configManager = nlm::getConfigManager();
    
    // Check parameter groups
    auto simulationParams = configManager.getParameterNamesByGroup("simulation");
    auto neuralParams = configManager.getParameterNamesByGroup("neural");
    auto plasticityParams = configManager.getParameterNamesByGroup("plasticity");
    auto memoryParams = configManager.getParameterNamesByGroup("memory");
    auto developmentParams = configManager.getParameterNamesByGroup("development");
    auto neuromodulationParams = configManager.getParameterNamesByGroup("neuromodulation");
    auto generalParams = configManager.getParameterNamesByGroup("general");
    
    assert(!simulationParams.empty());
    assert(!neuralParams.empty());
    assert(!plasticityParams.empty());
    assert(!memoryParams.empty());
    assert(!developmentParams.empty());
    assert(!neuromodulationParams.empty());
    assert(!generalParams.empty());
    
    // Check specific parameters
    assert(configManager.hasParameter("simulation_timestep"));
    assert(configManager.hasParameter("neuron_count"));
    assert(configManager.hasParameter("membrane_potential_threshold"));
    assert(configManager.hasParameter("stdp_ltp_weight"));
    assert(configManager.hasParameter("working_memory_capacity"));
    assert(configManager.hasParameter("development_enabled"));
    assert(configManager.hasParameter("dopamine_baseline"));
    assert(configManager.hasParameter("random_seed"));
    
    std::cout << "  ✓ Parameter groups passed" << std::endl;
}

void testParameterValidation() {
    std::cout << "Testing parameter validation..." << std::endl;
    
    nlm::initializeConfigManager();
    nlm::ConfigManager& configManager = nlm::getConfigManager();
    
    // Validate that all parameters are within bounds
    bool allValid = configManager.validateAllParameters();
    assert(allValid);
    
    // Test individual parameter validation
    bool timestepValid = configManager.validateParameter("simulation_timestep");
    bool neuronCountValid = configManager.validateParameter("neuron_count");
    bool connectionProbValid = configManager.validateParameter("connection_probability");
    
    assert(timestepValid);
    assert(neuronCountValid);
    assert(connectionProbValid);
    
    std::cout << "  ✓ Parameter validation passed" << std::endl;
}

void testCommandLineInterface() {
    std::cout << "Testing command line interface..." << std::endl;
    
    nlm::initializeConfigManager();
    nlm::ConfigManager& configManager = nlm::getConfigManager();
    
    // Simulate command line arguments
    const char* argv[] = {
        "test_program",
        "--simulation_timestep", "0.002",
        "--neuron_count", "2000",
        "--dopamine_baseline", "0.5",
        "--development_enabled", "false"
    };
    int argc = 9; // Program name + 4 key-value pairs
    
    bool success = configManager.loadFromCommandLine(argc, const_cast<char**>(argv));
    assert(success);
    
    // Check that command line parameters were set
    assert(configManager.getParameter<double>("simulation_timestep").value() == 0.002);
    assert(configManager.getParameter<int>("neuron_count").value() == 2000);
    assert(configManager.getParameter<double>("dopamine_baseline").value() == 0.5);
    assert(configManager.getParameter<bool>("development_enabled").value() == false);
    
    std::cout << "  ✓ Command line interface passed" << std::endl;
}

void testConfigurationSummary() {
    std::cout << "Testing configuration summary..." << std::endl;
    
    nlm::initializeConfigManager();
    nlm::ConfigManager& configManager = nlm::getConfigManager();
    
    // Get configuration summary
    std::string summary = configManager.getConfigurationSummary();
    assert(!summary.empty());
    assert(summary.find("Configuration Parameters by Group:") != std::string::npos);
    assert(summary.find("Simulation Parameters") != std::string::npos);
    assert(summary.find("Neural Parameters") != std::string::npos);
    
    std::cout << "  ✓ Configuration summary passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    // Test that existing Config API still works
    nlm::Config config;
    
    // Test old-style set/get operations
    config.set("test_key", 123);
    assert(config.has("test_key"));
    
    auto value = config.get<int>("test_key");
    assert(value.has_value());
    assert(value.value() == 123);
    
    // Test getOr
    int defaultValue = config.getOr<int>("nonexistent_key", 999);
    assert(defaultValue == 999);
    
    // Test remove
    config.remove("test_key");
    assert(!config.has("test_key"));
    
    // Test clear
    config.set("temp_key", 456);
    config.clear();
    assert(!config.has("temp_key"));
    
    std::cout << "  ✓ Backward compatibility passed" << std::endl;
}

void testParameterReset() {
    std::cout << "Testing parameter reset..." << std::endl;
    
    nlm::initializeConfigManager();
    nlm::ConfigManager& configManager = nlm::getConfigManager();
    
    // Get original value
    auto originalValue = configManager.getParameter<double>("simulation_timestep");
    assert(originalValue.has_value());
    
    // Change parameter
    configManager.setParameter<double>("simulation_timestep", 0.005);
    auto newValue = configManager.getParameter<double>("simulation_timestep");
    assert(newValue.has_value());
    assert(newValue.value() == 0.005);
    
    // Reset parameter
    configManager.resetParameter("simulation_timestep");
    auto resetValue = configManager.getParameter<double>("simulation_timestep");
    assert(resetValue.has_value());
    assert(resetValue.value() == originalValue.value());
    
    std::cout << "  ✓ Parameter reset passed" << std::endl;
}

void runAllTests() {
    std::cout << "==================================================" << std::endl;
    std::cout << "Running NLM Configuration System Tests" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    
    testConfigInitialization();
    std::cout << std::endl;
    
    testParameterAccess();
    std::cout << std::endl;
    
    testParameterSetting();
    std::cout << std::endl;
    
    testConfigFileOperations();
    std::cout << std::endl;
    
    testParameterGroups();
    std::cout << std::endl;
    
    testParameterValidation();
    std::cout << std::endl;
    
    testCommandLineInterface();
    std::cout << std::endl;
    
    testConfigurationSummary();
    std::cout << std::endl;
    
    testBackwardCompatibility();
    std::cout << std::endl;
    
    testParameterReset();
    std::cout << std::endl;
    
    std::cout << "==================================================" << std::endl;
    std::cout << "All tests passed! Configuration system is working correctly." << std::endl;
    std::cout << "==================================================" << std::endl;
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}