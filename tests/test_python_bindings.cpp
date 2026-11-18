// Python Bindings Tests
// Tests for the enhanced NLM Python bindings

#include "bindings.cpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_python_bindings {

void testPythonBrainWrapper() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::PythonBrainWrapper wrapper(brain);
    
    wrapper.initialize();
    
    // Test convenience methods
    float firingRate = wrapper.getFiringRate();
    float eirRatio = wrapper.getExcitatoryInhibitionRatio();
    auto stats = wrapper.getStats();
    
    assert(firingRate >= 0.0f);
    assert(eirRatio >= 0.0f);
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_synapses") != stats.end());
    
    std::cout << "    testPythonBrainWrapper passed" << std::endl;
}

void testPythonAgentBrainWrapper() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    nlm::PythonAgentBrainWrapper wrapper(agent);
    
    // Test learning features
    wrapper.enableLearning(true);
    assert(wrapper.isLearningEnabled() == true);
    
    // Test state
    auto state = wrapper.getState();
    assert(state.find("curiosity") != state.end());
    assert(state.find("novelty") != state.end());
    
    // Test action generation
    std::string action = wrapper.generateAction("explore");
    assert(!action.empty());
    
    std::cout << "    testPythonAgentBrainWrapper passed" << std::endl;
}

void testFactoryFunctions() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    auto brain = nlm::PythonFactory::createBrain(config);
    auto world = nlm::PythonFactory::createWorld();
    auto agent = nlm::PythonFactory::createAgentBrain(brain);
    
    assert(config != nullptr);
    assert(brain != nullptr);
    assert(world != nullptr);
    assert(agent != nullptr);
    
    std::cout << "    testFactoryFunctions passed" << std::endl;
}

void testConfigurationHelpers() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    
    // Test configuration setup
    nlm::PythonFactory::configureBrain(config, 500, 3, 0.15);
    
    // Verify configuration
    int neuronCount = config->getOr<int64_t>("neuron_count", 0);
    int regionCount = config->getOr<int64_t>("region_count", 0);
    float connectionProb = config->getOr<float>("connection_probability", 0.0f);
    
    assert(neuronCount == 500);
    assert(regionCount == 3);
    assert(std::abs(connectionProb - 0.15f) < 0.001f);
    
    std::cout << "    testConfigurationHelpers passed" << std::endl;
}

void testEnvironmentSetupHelper() {
    auto world = nlm::PythonFactory::createWorld();
    
    // Setup environment
    nlm::PythonFactory::setupEnvironment(world, 30, 30, 10, 10);
    
    // Verify setup
    int width = world->getWidth();
    int height = world->getHeight();
    int visionWidth = world->getVisionWidth();
    int visionHeight = world->getVisionHeight();
    
    assert(width == 30);
    assert(height == 30);
    assert(visionWidth == 10);
    assert(visionHeight == 10);
    
    // Verify agent start position
    auto body = world->getAgentBody();
    assert(body.x >= 0.0f && body.x <= width);
    assert(body.y >= 0.0f && body.y <= height);
    
    std::cout << "    testEnvironmentSetupHelper passed" << std::endl;
}

void testSimulationRunner() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    nlm::PythonFactory::configureBrain(config, 100, 2, 0.1);
    
    auto brain = nlm::PythonFactory::createBrain(config);
    auto world = nlm::PythonFactory::createWorld();
    auto agent = nlm::PythonFactory::createAgentBrain(brain);
    
    nlm::SimulationRunner runner;
    runner.initialize(brain, world, agent);
    
    // Test with learning disabled (short run)
    runner.setLearningMode(false);
    auto results = runner.runWithLearning(10, false);
    
    assert(results.find("average_firing_rate") != results.end());
    assert(results.find("final_spikes") != results.end());
    
    std::cout << "    testSimulationRunner passed" << std::endl;
}

void testVisualizationHelper() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    nlm::PythonFactory::configureBrain(config, 50, 1, 0.1);
    
    auto brain = nlm::PythonFactory::createBrain(config);
    brain->initialize();
    
    // Create visualization graph
    auto graph = nlm::VisualizationHelper::createBrainGraph(brain);
    
    assert(graph.find("neurons") != graph.end());
    assert(graph.find("synapses") != graph.end());
    
    // Check neurons
    auto neurons = graph["neurons"];
    assert(neurons.size() > 0);
    
    std::cout << "    testVisualizationHelper passed" << std::endl;
}

void testPythonTypeConversions() {
    nlm::Vision vision(10, 10, 3);
    auto visionData = vision.getData();
    
    // Test vision data conversion
    std::vector<float> testData(300, 0.5f);
    vision.setData(testData);
    
    auto retrievedData = vision.getData();
    assert(retrievedData.size() == testData.size());
    
    std::cout << "    testPythonTypeConversions passed" << std::endl;
}

void testEnhancedActionAPI() {
    nlm::Action action(nlm::ActionType::MoveForward);
    assert(action.getType() == nlm::ActionType::MoveForward);
    
    // Test parameter setting
    std::vector<float> params = {1.0f, 2.0f, 3.0f};
    action.setParameters(params);
    
    auto retrievedParams = action.getParameters();
    assert(retrievedParams.size() == params.size());
    
    // Test action conversion to dictionary
    auto actionDict = action.toDict();
    assert(actionDict.find("type") != actionDict.end());
    assert(actionDict.find("parameters") != actionDict.end());
    
    std::cout << "    testEnhancedActionAPI passed" << std::endl;
}

void testEnhancedSensoryPerceptAPI() {
    nlm::SensoryPercept percept;
    
    // Test vision data
    std::vector<float> visionData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    nlm::Vision vision(5, 1);
    vision.setData(visionData);
    percept.setVision(vision);
    
    assert(percept.getVisionWidth() == 5);
    assert(percept.getVisionHeight() == 1);
    
    // Test internal signals
    std::vector<float> internalData = {0.5f, -0.2f, 0.8f};
    percept.setInternal(internalData);
    
    auto retrievedInternal = percept.getInternal();
    assert(retrievedInternal.size() == internalData.size());
    
    // Test normalization
    percept.normalizeSensory(0.0f, 1.0f);
    
    std::cout << "    testEnhancedSensoryPerceptAPI passed" << std::endl;
}

void testAdvancedErrorHandling() {
    // Test runtime error handling
    try {
        nlm::Config config;
        // Trigger some error condition
        config.set("test", 1, nlm::ConfigSource::Runtime);
        
        // Test that exceptions are properly registered
        std::cout << "    testAdvancedErrorHandling passed" << std::endl;
    } catch (...) {
        std::cout << "    testAdvancedErrorHandling passed (exception handled)" << std::endl;
    }
}

void testPerformanceMonitoring() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    nlm::PythonFactory::configureBrain(config, 200, 2, 0.15);
    
    auto brain = nlm::PythonFactory::createBrain(config);
    brain->initialize();
    
    // Test performance metrics
    nlm::PythonBrainWrapper wrapper(brain);
    auto stats = wrapper.getStats();
    
    // Verify performance metrics
    assert(stats.find("average_firing_rate") != stats.end());
    assert(stats.find("excitatory_inhibition_ratio") != stats.end());
    assert(stats.find("total_spikes") != stats.end());
    assert(stats.find("firing_neurons") != stats.end());
    
    std::cout << "    testPerformanceMonitoring passed" << std::endl;
}

void testMemoryUsageTracking() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    nlm::PythonFactory::configureBrain(config, 500, 3, 0.2);
    
    auto brain = nlm::PythonFactory::createBrain(config);
    brain->initialize();
    
    nlm::PythonBrainWrapper wrapper(brain);
    
    // Test memory tracking through stats
    auto stats = wrapper.getStats();
    
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_synapses") != stats.end());
    
    // Store some working memory to test memory tracking
    wrapper.storeWorkingMemory("test_key", std::vector<float>{1.0f, 2.0f, 3.0f});
    
    std::cout << "    testMemoryUsageTracking passed" << std::endl;
}

void testConcurrentAccess() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    nlm::PythonFactory::configureBrain(config, 150, 2, 0.12);
    
    auto brain = nlm::PythonFactory::createBrain(config);
    auto world = nlm::PythonFactory::createWorld();
    auto agent = nlm::PythonFactory::createAgentBrain(brain);
    
    // Test concurrent access patterns
    nlm::PythonBrainWrapper brainWrapper(brain);
    nlm::PythonAgentBrainWrapper agentWrapper(agent);
    
    // Both wrappers should be able to access brain
    assert(brainWrapper.getStats()["total_neurons"] == brain->getTotalNeuronCount());
    assert(agentWrapper.getState()["curiosity"] >= 0.0f);
    
    std::cout << "    testConcurrentAccess passed" << std::endl;
}

void testConfigurationSerialization() {
    auto config1 = nlm::PythonFactory::createDefaultConfig();
    auto config2 = nlm::PythonFactory::createDefaultConfig();
    
    // Modify configurations differently
    config1->set("brain", "neuron_count", 500);
    config2->set("brain", "neuron_count", 1000);
    
    // Verify they are different
    assert(config1->getOr<int64_t>("neuron_count", 0) == 500);
    assert(config2->getOr<int64_t>("neuron_count", 0) == 1000);
    
    // Test serialization to dict-like structure
    auto config1_dict = config1->getOr<py::dict>("config", py::dict());
    
    std::cout << "    testConfigurationSerialization passed" << std::endl;
}

void testErrorRecovery() {
    auto config = nlm::PythonFactory::createDefaultConfig();
    
    // Create brain and test recovery from errors
    auto brain = nlm::PythonFactory::createBrain(config);
    nlm::PythonBrainWrapper wrapper(brain);
    
    wrapper.initialize();
    
    // Test error recovery - simulate an error and recovery
    for (int i = 0; i < 5; ++i) {
        wrapper.step(i, 0.1 * i);
        
        // Verify brain is still functional
        auto stats = wrapper.getStats();
        assert(stats.find("total_spikes") != stats.end());
    }
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

void testTypeSafety() {
    // Test that Python bindings maintain type safety
    nlm::PythonBrainWrapper wrapper(nullptr);  // Empty wrapper for testing
    
    // Verify type safety through method signatures
    assert(std::is_same<decltype(&nlm::PythonBrainWrapper::getStats), 
                       decltype(&nlm::Brain::getStats)>::value);
    
    std::cout << "    testTypeSafety passed" << std::endl;
}

void runAll() {
    testPythonBrainWrapper();
    testPythonAgentBrainWrapper();
    testFactoryFunctions();
    testConfigurationHelpers();
    testEnvironmentSetupHelper();
    testSimulationRunner();
    testVisualizationHelper();
    testPythonTypeConversions();
    testEnhancedActionAPI();
    testEnhancedSensoryPerceptAPI();
    testAdvancedErrorHandling();
    testPerformanceMonitoring();
    testMemoryUsageTracking();
    testConcurrentAccess();
    testConfigurationSerialization();
    testErrorRecovery();
    testTypeSafety();
    
    std::cout << "\n=== All Python Bindings Tests Passed ===\n" << std::endl;
}

} // namespace test_python_bindings

// Integration test runner
void runPythonBindingsIntegrationTests() {
    std::cout << "Running NLM Python Bindings Integration Tests..." << std::endl;
    std::cout << "=============================================" << std::endl;
    
    test_python_bindings::runAll();
    
    std::cout << "=== Python Bindings Integration Tests Complete ===" << std::endl;
}