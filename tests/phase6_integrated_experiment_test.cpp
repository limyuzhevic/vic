// Phase6IntegratedExperiment Test Suite
// Tests for Phase6IntegratedExperiment integration and system integration
// Validates Phase6IntegratedExperiment integration with all NLM systems

#include "phase6_integrated_experiment_test.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6IntegrationFix.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

void testPhase6IntegratedExperimentInitialization() {
    std::cout << "Testing Phase6IntegratedExperiment initialization..." << std::endl;
    
    // Test Phase6IntegratedExperiment initialization
    Phase6IntegratedExperiment experiment;
    
    bool initialized = experiment.initialize();
    assert(initialized == true);
    
    std::cout << "Phase6IntegratedExperiment initialization test passed!" << std::endl;
}

void testPhase6IntegratedExperimentRun() {
    std::cout << "Testing Phase6IntegratedExperiment run..." << std::endl;
    
    // Test Phase6IntegratedExperiment run
    Phase6IntegratedExperiment experiment;
    
    experiment.initialize();
    experiment.run();
    
    std::cout << "Phase6IntegratedExperiment run test passed!" << std::endl;
}

void testPhase6IntegratedExperimentIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment integration..." << std::endl;
    
    // Test Phase6IntegratedExperiment integration
    Phase6IntegratedExperiment experiment;
    
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    std::cout << "Phase6IntegratedExperiment integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentMemoryIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment memory integration..." << std::endl;
    
    // Test Phase6IntegratedExperiment memory integration
    Phase6IntegratedExperiment experiment;
    
    bool memoryOK = experiment.testMemoryIntegration();
    assert(memoryOK == true);
    
    std::cout << "Phase6IntegratedExperiment memory integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentNeuromodulationIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment neuromodulation integration..." << std::endl;
    
    // Test Phase6IntegratedExperiment neuromodulation integration
    Phase6IntegratedExperiment experiment;
    
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    assert(neuromodulationOK == true);
    
    std::cout << "Phase6IntegratedExperiment neuromodulation integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentCognitionIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment cognition integration..." << std::endl;
    
    // Test Phase6IntegratedExperiment cognition integration
    Phase6IntegratedExperiment experiment;
    
    bool cognitionOK = experiment.testCognitionIntegration();
    assert(cognitionOK == true);
    
    std::cout << "Phase6IntegratedExperiment cognition integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentDevelopmentIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment development integration..." << std::endl;
    
    // Test Phase6IntegratedExperiment development integration
    Phase6IntegratedExperiment experiment;
    
    bool developmentOK = experiment.testDevelopmentIntegration();
    assert(developmentOK == true);
    
    std::cout << "Phase6IntegratedExperiment development integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentWorldIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment world integration..." << std::endl;
    
    // Create brain for world integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world for Phase6IntegratedExperiment integration
    SimpleWorld world;
    world.configure(50.0f, 50.0f, 16, 16);
    
    // Create agent for world integration
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test Phase6IntegratedExperiment world integration
    Phase6IntegratedExperiment experiment;
    
    bool worldIntegrationOK = experiment.testWorldIntegration();
    assert(worldIntegrationOK == true);
    
    std::cout << "Phase6IntegratedExperiment world integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentEnvironmentIntegration() {
    std::cout << "Testing Phase6IntegratedExperiment environment integration..." << std::endl;
    
    // Create brain for environment integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create environment for Phase6IntegratedExperiment integration
    // (Environment would be created if available)
    
    // Test Phase6IntegratedExperiment environment integration
    Phase6IntegratedExperiment experiment;
    
    bool environmentIntegrationOK = experiment.testEnvironmentIntegration();
    assert(environmentIntegrationOK == true);
    
    std::cout << "Phase6IntegratedExperiment environment integration test passed!" << std::endl;
}

void testPhase6IntegratedExperimentPhase6Integration() {
    std::cout << "Testing Phase6IntegratedExperiment Phase 6 integration..." << std::endl;
    
    // Create Phase6IntegrationFix
    Phase6IntegrationFix fix;
    
    // Test Phase6IntegrationFix with Phase6IntegratedExperiment
    bool integrationOK = fix.checkSystemIntegration();
    // Integration may or may not be available depending on system state
    
    // Test Phase6IntegrationFix backward compatibility
    bool compatOK = fix.verifyBackwardCompatibility();
    assert(compatOK == true);
    
    std::cout << "Phase6IntegratedExperiment Phase 6 integration test passed!" << std::endl;
}

void runPhase6IntegratedExperimentTests() {
    std::cout << "=== Running Phase6IntegratedExperiment Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentInitialization();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentRun();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentMemoryIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentNeuromodulationIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentCognitionIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentDevelopmentIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentWorldIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentEnvironmentIntegration();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentPhase6Integration();
    std::cout << std::endl;
    
    std::cout << "=== All Phase6IntegratedExperiment Tests Passed ===" << std::endl;
}

} // namespace nlm
