// Python Bindings Tests
// Tests for the new convenience functions and enhanced Python API

#include "python/bindings.cpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <map>

namespace test_python_bindings {

void testConfigurationPresets() {
    // Test configuration presets
    auto defaultConfig = nlm::createDefaultConfig();
    assert(defaultConfig != nullptr);
    
    auto developmentConfig = nlm::createDevelopmentConfig();
    assert(developmentConfig != nullptr);
    
    auto standardConfig = nlm::createStandardConfig();
    assert(standardConfig != nullptr);
    
    auto highPerformanceConfig = nlm::createHighPerformanceConfig();
    assert(highPerformanceConfig != nullptr);
    
    auto minimalConfig = nlm::createMinimalConfig();
    assert(minimalConfig != nullptr);
    
    std::cout << "    testConfigurationPresets passed" << std::endl;
}

void testCompleteSimulationSetup() {
    // Test complete simulation setup
    auto config = nlm::createDevelopmentConfig();
    assert(config != nullptr);
    
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    
    auto world = nlm::createSimpleWorld();
    assert(world != nullptr);
    
    auto agent = nlm::createAgentBrain(brain);
    assert(agent != nullptr);
    
    // Initialize systems
    assert(brain->initialize());
    world->configure(10, 10, 5, 5);
    world->reset();
    agent->initialize(world);
    
    std::cout << "    testCompleteSimulationSetup passed" << std::endl;
}

void testConvenienceSimulationLoop() {
    // Test convenience simulation loop
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto world = nlm::createSimpleWorld();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(world);
    
    // Run a few steps
    for (int step = 0; step < 10; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    // Verify simulation ran
    assert(brain->getTotalNeuronCount() > 0);
    assert(world->getSimulationTime() > 0.0);
    
    std::cout << "    testConvenienceSimulationLoop passed" << std::endl;
}

void testErrorHandlingAndValidation() {
    // Test error handling and validation
    auto config = nlm::createDefaultConfig();
    
    // Test configuration validation
    auto validation = nlm::validateConfig(config);
    assert(std::get<0>(validation).empty()); // No errors
    
    // Test safe brain step
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    bool stepSuccess = nlm::safe_brain_step(brain, 0);
    assert(stepSuccess); // Should succeed
    
    std::cout << "    testErrorHandlingAndValidation passed" << std::endl;
}

void testPerformanceMonitoring() {
    // Test performance monitoring
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Profile simulation
    auto profile = nlm::profile_brain_simulation(brain, 100);
    assert(profile.find("total_time") != profile.end());
    assert(profile.find("steps_per_second") != profile.end());
    assert(profile.find("average_step_time_ms") != profile.end());
    
    assert(profile["steps_per_second"] > 0);
    assert(profile["average_step_time_ms"] > 0);
    
    std::cout << "    testPerformanceMonitoring passed" << std::endl;
}

void testAdvancedNeuralAccess() {
    // Test advanced neural access and analysis
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Test neural connectivity matrix
    auto connectivity = nlm::analyze_connectivity(brain);
    assert(connectivity.find("total_neurons") != connectivity.end());
    assert(connectivity.find("total_synapses") != connectivity.end());
    assert(connectivity.find("average_connections_per_neuron") != connectivity.end());
    
    // Test neural activity inspection
    auto activity = nlm::inspect_neural_activity(brain, 0.5f);
    assert(activity.find("active_neurons") != activity.end());
    assert(activity.find("firing_neurons") != activity.end());
    assert(activity.find("threshold_used") != activity.end());
    
    std::cout << "    testAdvancedNeuralAccess passed" << std::endl;
}

void testDevelopmentAnalysis() {
    // Test development stage analysis
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto world = nlm::createSimpleWorld();
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(world);
    
    auto developmentSummary = nlm::get_development_summary(brain, agent);
    assert(developmentSummary.find("brain_stage") != developmentSummary.end());
    assert(developmentSummary.find("neuromodulation_level") != developmentSummary.end());
    assert(developmentSummary.find("curiosity_level") != developmentSummary.end());
    
    std::cout << "    testDevelopmentAnalysis passed" << std::endl;
}

void testDataExportAndImport() {
    // Test data export and import
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Save brain state
    assert(nlm::save_brain_state(brain, "test_checkpoint.bin"));
    
    // Create new brain and load state
    auto config2 = nlm::createDefaultConfig();
    auto brain2 = nlm::createBrain(config2);
    assert(nlm::load_brain_state(brain2, "test_checkpoint.bin"));
    
    std::cout << "    testDataExportAndImport passed" << std::endl;
}

void testSimulationStatistics() {
    // Test simulation statistics
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto world = nlm::createSimpleWorld();
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(world);
    
    // Run a few steps
    for (int step = 0; step < 50; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
    }
    
    // Get statistics
    auto stats = nlm::get_simulation_stats(brain, world);
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_synapses") != stats.end());
    assert(stats.find("firing_neurons") != stats.end());
    assert(stats.find("average_firing_rate") != stats.end());
    assert(stats.find("excitatory_inhibition_ratio") != stats.end());
    assert(stats.find("total_spikes") != stats.end());
    assert(stats.find("simulation_time") != stats.end());
    assert(stats.find("world_width") != stats.end());
    assert(stats.find("world_height") != stats.end());
    
    std::cout << "    testSimulationStatistics passed" << std::endl;
}

void testAdvancedPlasticyConfiguration() {
    // Test advanced plasticity configuration
    auto config = nlm::createDevelopmentConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    std::map<std::string, float> hebbianParams;
    hebbianParams["learning_rate"] = 0.001f;
    hebbianParams["structural_rate"] = 0.0001f;
    
    std::map<std::string, float> stdpParams;
    stdpParams["learning_rate"] = 0.001f;
    stdpParams["tau_plus"] = 20.0f;
    stdpParams["tau_minus"] = 20.0f;
    
    auto result = nlm::configure_plasticity_rules(brain, hebbianParams, stdpParams);
    assert(result.find("Plasticity rules configured") != std::string::npos);
    
    std::cout << "    testAdvancedPlasticyConfiguration passed" << std::endl;
}

void testMultiAgentSupport() {
    // Test multi-agent simulation support
    auto config = nlm::createDevelopmentConfig();
    
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    std::vector<std::shared_ptr<nlm::SimpleWorld>> worlds;
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents;
    
    // Create multiple agents
    for (int i = 0; i < 3; ++i) {
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(15, 15, 6, 6);
        world->reset();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(world);
        
        brains.push_back(brain);
        worlds.push_back(world);
        agents.push_back(agent);
    }
    
    // Test group creation
    auto groupInfo = nlm::create_agent_group(brains, worlds, agents);
    assert(groupInfo.find("agents") != groupInfo.end());
    assert(groupInfo["total_agents"] == 3);
    
    std::cout << "    testMultiAgentSupport passed" << std::endl;
}

void runAll() {
    std::cout << "=== Python Bindings Tests ===" << std::endl;
    testConfigurationPresets();
    testCompleteSimulationSetup();
    testConvenienceSimulationLoop();
    testErrorHandlingAndValidation();
    testPerformanceMonitoring();
    testAdvancedNeuralAccess();
    testDevelopmentAnalysis();
    testDataExportAndImport();
    testSimulationStatistics();
    testAdvancedPlasticyConfiguration();
    testMultiAgentSupport();
    std::cout << "=== All Python Bindings Tests Passed ===" << std::endl;
}

} // namespace test_python_bindings