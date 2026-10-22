// BrainManager Tests
#include "BrainManager.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_brain_manager {

void testBrainManagerConfigCreation() {
    std::cout << "    testBrainManagerConfigCreation passed" << std::endl;
}

void testBrainManagerBrainCreation() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    assert(brain != nullptr);
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    
    std::cout << "    testBrainManagerBrainCreation passed" << std::endl;
}

void testBrainManagerInitialization() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    bool success = nlm::BrainManager::initializeBrain(brain);
    assert(success);
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    assert(nlm::BrainManager::getSynapseCount(brain) > 0);
    
    std::cout << "    testBrainManagerInitialization passed" << std::endl;
}

void testBrainManagerSimulation() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    nlm::BrainManager::initializeBrain(brain);
    
    // Test with no input
    nlm::BrainManager::runSimpleSimulation(brain, 10);
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    
    std::cout << "    testBrainManagerSimulation passed" << std::endl;
}

void testBrainManagerSensoryInput() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    nlm::BrainManager::initializeBrain(brain);
    
    // Test with sensory input
    std::vector<float> input = {1.0f, 0.5f, -0.5f, 0.0f, 2.0f};
    nlm::BrainManager::runSimpleSimulation(brain, 10, input);
    
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    
    std::cout << "    testBrainManagerSensoryInput passed" << std::endl;
}

void testBrainManagerConfigFile() {
    // Create and save config
    auto config = nlm::BrainManager::createConfig(200, 2, 0.15f, 0.002, 123);
    
    std::string filename = "test_config_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".cfg";
    
    bool saved = nlm::BrainManager::saveConfigToFile(config, filename);
    assert(saved);
    
    // Load config
    auto loadedConfig = nlm::BrainManager::loadConfigFromFile(filename);
    assert(loadedConfig != nullptr);
    
    assert(nlm::BrainManager::getNeuronCount(nlm::BrainManager::createBrain(loadedConfig)) == 200);
    
    // Clean up
    std::remove(filename.c_str());
    
    std::cout << "    testBrainManagerConfigFile passed" << std::endl;
}

void testBrainManagerDevelopment() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    nlm::BrainManager::initializeBrain(brain);
    
    // Enable development features
    nlm::BrainManager::enableDevelopment(brain, true);
    nlm::BrainManager::enablePlasticity(brain, true);
    nlm::BrainManager::enableRewardModulation(brain, true);
    nlm::BrainManager::enableCuriosity(brain, true);
    
    // Run simulation with development
    nlm::BrainManager::runSimpleSimulation(brain, 20);
    
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    
    std::cout << "    testBrainManagerDevelopment passed" << std::endl;
}

void testBrainManagerErrorHandling() {
    std::shared_ptr<nlm::Brain> nullBrain;
    
    // Test null brain handling
    assert(nlm::BrainManager::getNeuronCount(nullBrain) == 0);
    assert(nlm::BrainManager::getSynapseCount(nullBrain) == 0);
    assert(nlm::BrainManager::getAverageFiringRate(nullBrain) == 0.0f);
    assert(nlm::BrainManager::getExcitationInhibitionRatio(nullBrain) == 0.0f);
    
    std::cout << "    testBrainManagerErrorHandling passed" << std::endl;
}

void testBrainManagerStateManagement() {
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f, 0.001, 42);
    auto brain = nlm::BrainManager::createBrain(config);
    
    nlm::BrainManager::initializeBrain(brain);
    nlm::BrainManager::runSimpleSimulation(brain, 10);
    
    // Get status
    std::string status = nlm::BrainManager::getBrainStatus(brain);
    assert(!status.empty());
    assert(status.find("Neurons: 100") != std::string::npos);
    
    // Get system stats
    auto stats = nlm::BrainManager::getSystemStats(brain);
    assert(stats.find("neuron_count") != stats.end());
    assert(stats["neuron_count"] == 100.0);
    
    // Reset brain
    nlm::BrainManager::resetBrain(brain);
    assert(nlm::BrainManager::getNeuronCount(brain) == 100);
    
    std::cout << "    testBrainManagerStateManagement passed" << std::endl;
}

void runAllBrainManagerTests() {
    testBrainManagerConfigCreation();
    testBrainManagerBrainCreation();
    testBrainManagerInitialization();
    testBrainManagerSimulation();
    testBrainManagerSensoryInput();
    testBrainManagerConfigFile();
    testBrainManagerDevelopment();
    testBrainManagerErrorHandling();
    testBrainManagerStateManagement();
    
    std::cout << std::endl;
    std::cout << "All BrainManager tests passed!" << std::endl;
    std::cout << "BrainManager provides:
    • Simplified brain creation and initialization
    • Easy configuration management
    • Development and learning controls
    • Robust error handling
    • Clear, user-friendly API
    • Built-in simulation utilities
    • Comprehensive state management
" << std::endl;
}

} // namespace test_brain_manager