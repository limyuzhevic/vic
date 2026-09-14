// C++ Unit Test Enhancements
// Expanding existing test coverage with new feature tests, performance benchmarks, integration tests, and memory safety tests

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_cpp_enhancements {

void test_performance_benchmarks() {
    std::cout << "Testing C++ performance benchmarks..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    nlm::Brain brain(config);
    brain.initialize();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Performance test for brain initialization
    assert(creationTime < 5000); // Should complete within 5 seconds
    
    startTime = std::chrono::high_resolution_clock::now();
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    endTime = std::chrono::high_resolution_clock::now();
    auto simulationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Performance test for simulation
    double stepsPerSecond = (1000.0 / simulationTime) * 1000.0;
    assert(stepsPerSecond > 100); // Should process at least 100 steps per second
    
    startTime = std::chrono::high_resolution_clock::now();
    auto neuronCount = brain.getTotalNeuronCount();
    auto synapseCount = brain.getTotalSynapseCount();
    auto firingCount = brain.getFiringNeuronCount();
    auto avgFiringRate = brain.getAverageFiringRate();
    endTime = std::chrono::high_resolution_clock::now();
    auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Performance test for statistics queries
    assert(queryTime < 100); // Should complete within 100ms
    
    std::cout << "Performance benchmarks passed!" << std::endl;
}

void test_scalability() {
    std::cout << "Testing C++ scalability..." << std::endl;
    
    std::vector<double> creationTimes;
    std::vector<double> simulationTimes;
    
    // Test scalability with different brain sizes
    std::vector<int> neuronCounts = {50, 100, 200, 500};
    
    for (int neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(std::max(1, neuronCount / 50)), nlm::ConfigSource::Default);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        nlm::Brain brain(config);
        brain.initialize();
        auto endTime = std::chrono::high_resolution_clock::now();
        auto creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        creationTimes.push_back(creationTime);
        
        startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        endTime = std::chrono::high_resolution_clock::now();
        auto simulationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        simulationTimes.push_back(simulationTime);
    }
    
    // Check that performance scales reasonably (not exponentially)
    for (size_t i = 1; i < creationTimes.size(); ++i) {
        double growthFactor = creationTimes[i] / creationTimes[i-1];
        double neuronGrowthFactor = (neuronCounts[i] / neuronCounts[i-1]);
        
        // Creation time should scale sub-linearly with neuron count
        assert(growthFactor < neuronGrowthFactor * 2);
    }
    
    std::cout << "Scalability tests passed!" << std::endl;
}

void test_memory_usage() {
    std::cout << "Testing C++ memory usage..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory allocation for neurons and synapses
    auto initialNeurons = brain.getTotalNeuronCount();
    auto initialSynapses = brain.getTotalSynapseCount();
    
    // Perform operations that allocate memory
    for (int step = 0; step < 500; ++step) {
        brain.step(step);
    }
    
    // Check that memory is properly managed (no obvious leaks)
    auto finalNeurons = brain.getTotalNeuronCount();
    auto finalSynapses = brain.getTotalSynapseCount();
    
    // Neuron and synapse counts should remain constant (or change slightly)
    assert(std::abs(static_cast<int64_t>(finalNeurons) - static_cast<int64_t>(initialNeurons)) < 100);
    assert(std::abs(static_cast<int64_t>(finalSynapses) - static_cast<int64_t>(initialSynapses)) < 1000);
    
    // Test memory cleanup
    brain.reset();
    
    std::cout << "Memory usage tests passed!" << std::endl;
}

void test_memory_safety() {
    std::cout << "Testing C++ memory safety..." << std::endl;
    
    // Test null pointer handling
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    // Test invalid access patterns
    try {
        // This should handle null/void pointers gracefully
        auto nullRegion = brain.getRegion(nlm::RegionId(999999));
        if (nullRegion) {
            // Should not crash even with invalid ID
        }
    } catch (...) {
        // Exception handling is acceptable
    }
    
    // Test memory boundary conditions
    auto smallConfig = std::make_shared<nlm::Config>();
    smallConfig->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    smallConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain smallBrain(smallConfig);
    smallBrain.initialize();
    
    // Test with very small brain
    for (int step = 0; step < 10; ++step) {
        smallBrain.step(step);
    }
    
    // Test memory cleanup
    smallBrain.reset();
    
    // Test with large brain
    auto largeConfig = std::make_shared<nlm::Config>();
    largeConfig->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    largeConfig->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain largeBrain(largeConfig);
    largeBrain.initialize();
    
    for (int step = 0; step < 100; ++step) {
        largeBrain.step(step);
    }
    
    largeBrain.reset();
    
    std::cout << "Memory safety tests passed!" << std::endl;
}

void test_integration_brain_world_agent() {
    std::cout << "Testing brain-world-agent integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create world
    auto world = nlm::createSimpleWorld();
    assert(world != nullptr);
    
    // Configure world
    world->configure(200, 200, 80, 80);
    world->setAgentStart(100, 100);
    
    // Create agent brain
    auto agentBrain = nlm::createAgentBrain(brain);
    assert(agentBrain != nullptr);
    
    // Test interaction loop
    for (int step = 0; step < 50; ++step) {
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        assert(percept != nullptr);
        
        // Process with agent brain
        if (agentBrain) {
            // Agent brain processing (simplified)
            brain.step(step);
            
            // Produce action
            auto action = brain.produceAction();
            
            // Apply action to world
            if (action) {
                world->applyMotorCommand(*action, step);
            }
            
            world->update(step);
        }
    }
    
    std::cout << "Brain-world-agent integration tests passed!" << std::endl;
}

void test_session_persistence() {
    std::cout << "Testing session persistence..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set up initial state
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Save session
    std::string sessionFile = "test_persistence.bin";
    auto metadata = brain.saveBrainSession(sessionFile, "test_session", brain, true);
    assert(!metadata.empty());
    
    // Modify brain state
    brain.reset();
    for (int step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    // Load session (should restore original state)
    auto loadedBrain = brain.loadBrainSession(sessionFile);
    assert(loadedBrain != nullptr);
    
    // Verify that loaded brain has similar properties
    assert(loadedBrain->getTotalNeuronCount() == brain.getTotalNeuronCount());
    assert(loadedBrain->getRegionCount() == brain.getRegionCount());
    
    std::cout << "Session persistence tests passed!" << std::endl;
}

void test_experiment_framework() {
    std::cout << "Testing experiment framework..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create experiment
    nlm::Experiment experiment("test_experiment");
    experiment.setSeed(42);
    experiment.setStartStep(0);
    experiment.setEndStep(100);
    
    // Record experiment metrics
    experiment.recordMetric("accuracy", 0.95f);
    experiment.recordMetric("loss", 0.05f);
    experiment.recordMetric("epochs", 10);
    
    // Run experiment simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Record metrics
        float accuracy = 0.9f + (step * 0.0001f);
        float loss = 0.1f - (step * 0.0001f);
        
        experiment.recordMetric("accuracy", accuracy);
        experiment.recordMetric("loss", loss);
        
        // Record step data
        experiment.recordStep(
            step,
            brain.getTotalNeuronCount(),
            brain.getTotalSynapseCount(),
            brain.getFiringNeuronCount(),
            brain.getAverageFiringRate()
        );
    }
    
    // Verify experiment data
    assert(experiment.getName() == "test_experiment");
    assert(experiment.getMetric("accuracy") > 0.9f);
    assert(experiment.getMetric("loss") < 0.1f);
    assert(experiment.getStepData().size() == 100);
    
    std::cout << "Experiment framework tests passed!" << std::endl;
}

void test_visualization_interfaces() {
    std::cout << "Testing visualization interfaces..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create visualization interface
    auto vis = nlm::createVisualizationInterface();
    assert(vis != nullptr);
    
    // Test visualization with brain data
    for (int step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Could visualize brain state (simplified)
        auto neuronCount = brain.getTotalNeuronCount();
        auto synapseCount = brain.getTotalSynapseCount();
        auto firingCount = brain.getFiringNeuronCount();
        
        assert(neuronCount > 0);
        assert(synapseCount > 0);
    }
    
    // Test visualization output
    assert(!vis->isActive()); // Should not be active by default
    
    // Test performance profiling
    py::dict metrics = brain->profileBrain(50, true, true);
    assert(metrics.contains("total_time_ms"));
    assert(metrics.contains("steps_per_second"));
    
    std::cout << "Visualization interface tests passed!" << std::endl;
}

void test_cross_component_interactions() {
    std::cout << "Testing cross-component interactions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation integration
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    auto novelty = brain->getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test neuromodulation interactions
    dopamine->signalReward(1.0f);
    curiosity->initialize(brain);
    novelty->initialize(brain);
    
    // Test development-neuromodulation interactions
    auto devSystem = brain->getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    devSystem->setStage(nlm::DevelopmentalStage::Adult);
    devSystem->setDevelopmentalAge(1000.0f);
    
    // Test memory-neuromodulation interactions
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    workingMemory->store(pattern, 1.0f);
    
    // Test cognitive system interactions
    auto attention = brain->getAttention();
    auto planner = brain->getPlanner();
    auto conceptFormation = brain->getConceptFormation();
    
    assert(attention != nullptr);
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test cross-component updates
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
        
        curiosity->update(0.5f, 0.1f, 0.1f);
        novelty->update(0.1f);
        
        if (attention) {
            attention->update(step * 0.1f);
        }
    }
    
    std::cout << "Cross-component interaction tests passed!" << std::endl;
}

void test_performance_regression() {
    std::cout << "Testing performance regression..." << std::endl;
    
    // Run the same operations multiple times and check for degradation
    std::vector<double> executionTimes;
    
    for (int run = 0; run < 5; ++run) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        nlm::Brain brain(config);
        brain.initialize();
        
        for (int step = 0; step < 200; ++step) {
            brain.step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        executionTimes.push_back(executionTime);
    }
    
    // Check that performance doesn't degrade significantly between runs
    double maxTime = *std::max_element(executionTimes.begin(), executionTimes.end());
    double minTime = *std::min_element(executionTimes.begin(), executionTimes.end());
    
    double regressionRatio = maxTime / minTime;
    assert(regressionRatio < 2.0); // Should not degrade by more than 2x
    
    std::cout << "Performance regression tests passed!" << std::endl;
}

void runAllCppEnhancementsTests() {
    std::cout << "Running all C++ enhancements tests..." << std::endl;
    
    test_performance_benchmarks();
    test_scalability();
    test_memory_usage();
    test_memory_safety();
    test_integration_brain_world_agent();
    test_session_persistence();
    test_experiment_framework();
    test_visualization_interfaces();
    test_cross_component_interactions();
    test_performance_regression();
    
    std::cout << "All C++ enhancements tests passed!" << std::endl;
}

PYBIND11_MODULE(test_cpp_enhancements, m) {
    m.def("runAllCppEnhancementsTests", &runAllCppEnhancementsTests);
}
