// Python Bindings Comprehensive Tests
// Tests for all new helper functions, error handling, memory systems access, neuromodulation APIs, and advanced simulation control

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

namespace py = pybind11;
namespace nlm {

// Forward declarations for factory functions
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
std::shared_ptr<SimpleWorld> createSimpleWorld();
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);
std::shared_ptr<VisualizationInterface> createVisualizationInterface();
std::shared_ptr<ExperimentRunner> createExperimentRunner();

void test_pybindings_helper_functions() {
    std::cout << "Testing Python helper functions..." << std::endl;
    
    // Test factory functions
    auto config1 = createDefaultConfig();
    auto config2 = createDefaultConfig();
    assert(config1 != config2);
    
    config1->setConfigValue("test_key", "test_value");
    assert(config1->has("test_key"));
    assert(config1->getConfigValue("test_key") == "test_value");
    
    // Test config operations
    config1->saveToFile("test_config.json");
    auto config3 = createDefaultConfig();
    config3->loadFromFile("test_config.json");
    assert(config3->has("test_key"));
    
    // Test brain creation and initialization
    auto brain = createBrain(config1);
    assert(brain != nullptr);
    assert(brain->initialize());
    
    // Test step simulation
    for (int step = 0; step < 5; ++step) {
        brain->step(step);
    }
    
    // Test action production
    auto action = brain->produceAction();
    assert(action != nullptr);
    
    // Test memory systems
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    workingMemory->store(pattern, 1.0f);
    auto retrieved = workingMemory->retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Test episodic memory
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test development system
    auto devSystem = brain->getDevelopmentSystem();
    assert(devSystem != nullptr);
    assert(devSystem->getStageName() == "Initial");
    
    // Test neuromodulation
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    auto predError = brain->getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Test prediction system
    auto predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test cognitive systems
    auto planner = brain->getPlanner();
    assert(planner != nullptr);
    
    auto conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    auto attention = brain->getAttention();
    assert(attention != nullptr);
    
    std::cout << "All helper function tests passed!" << std::endl;
}

void test_pybindings_error_handling() {
    std::cout << "Testing error handling and validation..." << std::endl;
    
    // Test null pointer handling
    auto config = createDefaultConfig();
    auto brain = createBrain(config);
    
    // Test invalid arguments
    try {
        brain->injectCurrent(nullptr, 1.0f);  // Should throw error for null neuron
        assert(false && "Should have thrown exception");
    } catch (const std::exception& e) {
        assert(std::string(e.what()).find("null") != std::string::npos ||
               std::string(e.what()).find("invalid") != std::string::npos);
    }
    
    // Test invalid configuration values
    auto badConfig = createDefaultConfig();
    badConfig->setConfigValue("neuron_count", "invalid_string"); // This might fail or be validated
    
    // Test boundary conditions
    auto world = createSimpleWorld();
    world->configure(-10, -10, 0, 0); // Invalid dimensions
    assert(world->getWidth() > 0); // Should handle gracefully
    
    // Test memory access violations (simulate)
    auto workingMemory = brain->getWorkingMemory();
    std::vector<float> largePattern(10000, 1.0f); // Large pattern
    workingMemory->store(largePattern, 1.0f); // Should handle large memory
    
    std::cout << "All error handling tests passed!" << std::endl;
}

void test_pybindings_memory_systems() {
    std::cout << "Testing memory systems access..." << std::endl;
    
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = createBrain(config);
    brain->initialize();
    
    // Test NeuralWorkingMemory
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storage and retrieval
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    workingMemory->store(pattern1, 0.5f);
    
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
    workingMemory->store(pattern2, 1.5f);
    
    auto activity = workingMemory->retrieve();
    assert(activity.size() == 3);
    
    // Test neuron-specific storage
    auto neuronId = nlm::NeuronId(0);
    workingMemory->storeToNeuron(neuronId, 0.8f);
    assert(workingMemory->contains(neuronId));
    assert(workingMemory->getNeuronActivation(neuronId) == 0.8f);
    
    // Test memory updates and decay
    workingMemory->update(0.1f); // Time delta
    workingMemory->strengthenMemory(1.1f); // Strengthen factor
    
    // Test capacity limits
    workingMemory->setCapacity(100);
    assert(workingMemory->getCapacity() == 100);
    
    workingMemory->setDecayRate(0.9f);
    assert(workingMemory->getDecayRate() == 0.9f);
    
    // Test competition
    workingMemory->runCompetition();
    workingMemory->isWinning(neuronId);
    
    // Test NeuralEpisodicMemory
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test NeuralAssociativeMemory
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << "All memory systems tests passed!" << std::endl;
}

void test_pybindings_neuromodulation() {
    std::cout << "Testing neuromodulation APIs..." << std::endl;
    
    auto config = createDefaultConfig();
    auto brain = createBrain(config);
    brain->initialize();
    
    // Test Dopamine system
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test reward signaling
    dopamine->signalReward(1.0f);
    dopamine->signalRewardPredictionError(0.1f);
    
    // Test Curiosity system
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test novelty detection
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    std::vector<float> current = {0.5f, 0.6f, 0.7f};
    std::vector<float> previous = {0.4f, 0.5f, 0.6f};
    novelty->detectNovelty(current, previous);
    
    // Test Prediction Error system
    auto predError = brain->getPredictionErrorSignal();
    assert(predError != nullptr);
    
    float predicted = 0.8f;
    float actual = 1.2f;
    predError->computeError(predicted, actual);
    predError->updatePrediction(1.0f);
    
    // Test system interactions
    curiosity->initialize(brain);
    novelty->initialize(brain);
    predError->initialize(brain);
    
    // Test updates with neuromodulators
    curiosity->update(0.5f, 0.1f, 0.1f);
    novelty->update(0.1f);
    
    // Test state queries
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    assert(predError->getError() >= 0.0f);
    
    std::cout << "All neuromodulation tests passed!" << std::endl;
}

void test_pybindings_advanced_simulation() {
    std::cout << "Testing advanced simulation control..." << std::endl;
    
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = createBrain(config);
    brain->initialize();
    
    // Test visualization interface
    auto vis = createVisualizationInterface();
    assert(vis != nullptr);
    
    // Test profiling
    py::dict metrics = brain->profileBrain(100, true, true);
    assert(metrics.contains("total_time_ms"));
    assert(metrics.contains("steps_per_second"));
    assert(metrics.contains("total_spikes"));
    
    // Test network metrics
    py::dict networkMetrics = brain->computeNetworkMetrics();
    assert(networkMetrics.contains("neuron_count"));
    assert(networkMetrics.contains("synapse_count"));
    assert(networkMetrics.contains("average_firing_rate"));
    
    // Test brain topography analysis
    py::dict topography = brain->analyzeBrainTopography();
    assert(topography.contains("topography"));
    assert(topography.contains("network_metrics"));
    
    // Test development control
    auto devSystem = brain->getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test stage progression
    assert(devSystem->getStageName() == "Initial");
    devSystem->setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem->getStageName() == "Adult");
    
    devSystem->advanceStage();
    assert(devSystem->getStageName() == "Aging");
    
    // Test development metrics
    devSystem->setDevelopmentalAge(1000.0f);
    assert(devSystem->getDevelopmentalAge() == 1000.0f);
    
    assert(devSystem->getPlasticityModifier() >= 0.0f);
    assert(devSystem->isCriticalPeriod() == false);
    
    // Test session management
    std::string sessionFile = "test_session.bin";
    auto metadata = brain->saveBrainSession(sessionFile, "test_session", brain, true);
    assert(metadata.contains("session_name"));
    assert(metadata.contains("timestamp"));
    
    // Test loading sessions
    auto loadedBrain = brain->loadBrainSession(sessionFile);
    assert(loadedBrain != nullptr);
    assert(loadedBrain->getTotalNeuronCount() == brain->getTotalNeuronCount());
    
    // Test experiment framework
    auto experimentRunner = createExperimentRunner();
    assert(experimentRunner != nullptr);
    
    auto experiment = experimentRunner->createExperiment("test_exp");
    assert(experiment != nullptr);
    assert(experiment->getName() == "test_exp");
    
    // Test experiment metrics recording
    experiment->recordMetric("accuracy", 0.95f);
    experiment->recordMetric("loss", 0.05f);
    experiment->recordMetric("epoch", 10);
    
    assert(experiment->hasMetric("accuracy"));
    assert(experiment->getMetric("accuracy") == 0.95f);
    
    // Test step data recording
    experiment->recordStep(0, 50, 200, 10, 0.2f);
    experiment->setStartStep(0);
    experiment->setEndStep(100);
    experiment->addNote("Test note");
    
    auto stepData = experiment->getStepData();
    assert(stepData.size() > 0);
    
    // Test advanced features
    brain->exportPerformanceReport("performance_report.txt", 50);
    
    // Test learning experiment simulation
    py::dict envConfig;
    envConfig["reward_decay"] = 0.9;
    
    py::list experimentResults = brain->runLearningExperiment(config, envConfig, 5, 20);
    assert(experimentResults.size() == 5);
    
    std::cout << "All advanced simulation tests passed!" << std::endl;
}

void test_pybindings_memory_safety() {
    std::cout << "Testing memory safety..." << << std::endl;
    
    auto config = createDefaultConfig();
    
    // Test memory leak prevention with multiple brain instances
    for (int i = 0; i < 10; ++i) {
        auto brain = createBrain(config);
        assert(brain != nullptr);
        brain->initialize();
        
        // Perform operations
        for (int step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->clear();
        }
    }
    
    // Test null pointer safety
    auto nullBrain = std::shared_ptr<Brain>(nullptr);
    try {
        // This should throw an exception
        if (nullBrain) {
            nullBrain->step(0);
        }
        assert(false && "Should have thrown exception for null brain");
    } catch (...) {
        // Expected
    }
    
    // Test boundary conditions
    auto world = createSimpleWorld();
    
    // Very large world (stress test)
    world->configure(1000, 1000, 500, 500);
    assert(world->getWidth() == 1000);
    assert(world->getHeight() == 1000);
    
    // Very small world
    world->configure(1, 1, 0, 0);
    assert(world->getWidth() == 1);
    
    // Test memory cleanup
    auto vis = createVisualizationInterface();
    vis->clear(); // Should not crash
    
    std::cout << "All memory safety tests passed!" << std::endl;
}

void test_pybindings_performance() {
    std::cout << "Testing performance..." << std::endl;
    
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto brain = createBrain(config);
    brain->initialize();
    
    // Performance test for simulation steps
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete 1000 steps in reasonable time (< 10 seconds)
    assert(duration.count() < 10000);
    
    // Performance test for memory operations
    startTime = std::chrono::high_resolution_clock::now();
    
    auto workingMemory = brain->getWorkingMemory();
    for (int i = 0; i < 100; ++i) {
        std::vector<float> pattern(100, 0.5f);
        workingMemory->store(pattern, 1.0f);
        workingMemory->retrieve();
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Memory operations should be fast
    assert(duration.count() < 1000);
    
    // Performance test for neural operations
    startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        auto neuronId = nlm::NeuronId(i % 50);
        brain->injectCurrent(neuronId, 1.0f);
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    assert(duration.count() < 1000);
    
    std::cout << "All performance tests passed!" << std::endl;
}

void runAllPythonBindingsTests() {
    std::cout << "Running all Python bindings tests..." << std::endl;
    
    test_pybindings_helper_functions();
    test_pybindings_error_handling();
    test_pybindings_memory_systems();
    test_pybindings_neuromodulation();
    test_pybindings_advanced_simulation();
    test_pybindings_memory_safety();
    test_pybindings_performance();
    
    std::cout << "All Python bindings tests passed!" << std::endl;
}

PYBIND11_MODULE(test_pybindings, m) {
    m.def("test_pybindings_helper_functions", &test_pybindings_helper_functions);
    m.def("test_pybindings_error_handling", &test_pybindings_error_handling);
    m.def("test_pybindings_memory_systems", &test_pybindings_memory_systems);
    m.def("test_pybindings_neuromodulation", &test_pybindings_neuromodulation);
    m.def("test_pybindings_advanced_simulation", &test_pybindings_advanced_simulation);
    m.def("test_pybindings_memory_safety", &test_pybindings_memory_safety);
    m.def("test_pybindings_performance", &test_pybindings_performance);
    m.def("runAllPythonBindingsTests", &runAllPythonBindingsTests);
}
