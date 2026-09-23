// Learning Experiment Tests
#include "nlm/LearningExperiment.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_learning {

void testLearningExperimentCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::LearningExperiment experiment(brain, 42);
    
    // Verify experiment was created
    assert(experiment.brain != nullptr);
    assert(experiment.seed == 42);
    assert(experiment.initialSynapseCount == 0);
    assert(experiment.initialWeights.empty());
    assert(experiment.finalWeights.empty());
    assert(experiment.mostActiveNeurons.empty());
    
    std::cout << "    testLearningExperimentCreation passed" << std::endl;
}

void testLearningExperimentRecording() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool success = brain->initialize();
    assert(success);
    
    nlm::LearningExperiment experiment(brain, 123);
    
    // Record initial state
    experiment.recordInitialState();
    assert(experiment.initialSynapseCount > 0);
    assert(!experiment.initialWeights.empty());
    
    // Simulate some steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Record final state
    experiment.recordFinalState();
    assert(!experiment.finalWeights.empty());
    assert(!experiment.mostActiveNeurons.empty());
    
    // Compute statistics
    experiment.computeStatistics();
    
    std::cout << "    testLearningExperimentRecording passed" << std::endl;
}

void testLearningExperimentStatistics() {
    nlm::LearningExperiment experiment(nullptr, 456);
    
    // Test with empty weights (should handle gracefully)
    experiment.initialWeights = {1.0f, 2.0f, 3.0f};
    experiment.finalWeights = {1.1f, 2.1f, 3.1f};
    
    // Should not crash with valid data
    experiment.computeStatistics();
    
    // Test with empty weights (should print error)
    experiment.initialWeights.clear();
    experiment.finalWeights.clear();
    
    // Should handle gracefully without crashing
    experiment.computeStatistics();
    
    std::cout << "    testLearningExperimentStatistics passed" << std::endl;
}

void runAll() {
    testLearningExperimentCreation();
    testLearningExperimentRecording();
    testLearningExperimentStatistics();
    
    std::cout << std::endl;
    std::cout << "=== Learning Experiment Tests PASSED ===" << std::endl;
}

} // namespace test_learning