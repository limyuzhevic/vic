// Test for new AgentBrain advanced functionality
// This tests the new simulation control methods and advanced features

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_advanced_features {

void testCreateSimulationSetup() {
    std::cout << "Testing createSimulationSetup..." << std::endl;
    
    // Test balanced preset
    auto [brain, agent, world] = nlm::AgentBrain::createSimulationSetup("balanced");
    assert(brain != nullptr);
    assert(agent != nullptr);
    assert(world != nullptr);
    
    // Test exploratory preset
    auto [brain2, agent2, world2] = nlm::AgentBrain::createSimulationSetup("exploratory");
    assert(brain2 != nullptr);
    assert(agent2 != nullptr);
    assert(world2 != nullptr);
    
    // Test efficient preset
    auto [brain3, agent3, world3] = nlm::AgentBrain::createSimulationSetup("efficient");
    assert(brain3 != nullptr);
    assert(agent3 != nullptr);
    assert(world3 != nullptr);
    
    std::cout << "    createSimulationSetup tests passed" << std::endl;
}

void testEnableAllSubsystems() {
    std::cout << "Testing enableAllSubsystems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Initially all subsystems should be enabled by default
    assert(agent->isRewardModulationEnabled());
    assert(agent->isStructuralPlasticityEnabled());
    assert(agent->isDevelopmentEnabled());
    assert(agent->isCuriosityEnabled());
    
    // Disable all subsystems
    agent->enableAllSubsystems(false);
    assert(!agent->isRewardModulationEnabled());
    assert(!agent->isStructuralPlasticityEnabled());
    assert(!agent->isDevelopmentEnabled());
    assert(!agent->isCuriosityEnabled());
    
    // Re-enable all subsystems
    agent->enableAllSubsystems(true);
    assert(agent->isRewardModulationEnabled());
    assert(agent->isStructuralPlasticityEnabled());
    assert(agent->isDevelopmentEnabled());
    assert(agent->isCuriosityEnabled());
    
    std::cout << "    enableAllSubsystems tests passed" << std::endl;
}

void testGetEnabledSubsystems() {
    std::cout << "Testing getEnabledSubsystems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Get subsystems when all are enabled
    auto subsystems = agent->getEnabledSubsystems();
    assert(subsystems.size() == 4); // reward_modulation, structural_plasticity, development, curiosity
    
    // Check that all expected subsystems are present
    bool hasRewardModulation = false;
    bool hasStructuralPlasticity = false;
    bool hasDevelopment = false;
    bool hasCuriosity = false;
    
    for (const auto& subsystem : subsystems) {
        if (subsystem == "reward_modulation") hasRewardModulation = true;
        if (subsystem == "structural_plasticity") hasStructuralPlasticity = true;
        if (subsystem == "development") hasDevelopment = true;
        if (subsystem == "curiosity") hasCuriosity = true;
    }
    
    assert(hasRewardModulation);
    assert(hasStructuralPlasticity);
    assert(hasDevelopment);
    assert(hasCuriosity);
    
    std::cout << "    getEnabledSubsystems tests passed" << std::endl;
}

void testSaveLoadWithMetadata() {
    std::cout << "Testing save/load with metadata..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create test directory if needed
    std::string testFile = "test_simulation.pkl";
    
    // Save with metadata
    std::string description = "Test simulation with metadata";
    bool saveSuccess = agent->saveWithMetadata(testFile, description);
    assert(saveSuccess);
    
    // Load with metadata
    std::string loadedDescription;
    bool loadSuccess = agent->loadWithMetadata(testFile, loadedDescription);
    assert(loadSuccess);
    assert(loadedDescription == description);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    save/load with metadata tests passed" << std::endl;
}

void testGetSimulationMetadata() {
    std::cout << "Testing getSimulationMetadata..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Get metadata
    std::string metadata = agent->getSimulationMetadata();
    assert(!metadata.empty());
    
    // Check that metadata contains expected information
    assert(metadata.find("NLM Simulation Metadata") != std::string::npos);
    assert(metadata.find("Brain:") != std::string::npos);
    
    std::cout << "    getSimulationMetadata tests passed" << std::endl;
}

void testNeuromodulationControl() {
    std::cout << "Testing neuromodulation control..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test setting all neuromodulation levels
    agent->setAllNeuromodulation(1.5f, 0.8f, 0.2f);
    
    std::cout << "    neuromodulation control tests passed" << std::endl;
}

void testTaskConfiguration() {
    std::cout << "Testing task configuration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test different task configurations
    agent->configureForTask("exploration");
    agent->configureForTask("exploitation");
    agent->configureForTask("social_learning");
    agent->configureForTask("unknown"); // Should default to balanced
    
    std::cout << "    task configuration tests passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Running Advanced AgentBrain Feature Tests ===" << std::endl;
    
    testCreateSimulationSetup();
    testEnableAllSubsystems();
    testGetEnabledSubsystems();
    testSaveLoadWithMetadata();
    testGetSimulationMetadata();
    testNeuromodulationControl();
    testTaskConfiguration();
    
    std::cout << "=== All Advanced Feature Tests Passed ===" << std::endl;
}

} // namespace test_advanced_features

int main() {
    test_advanced_features::runAllTests();
    return 0;
}
