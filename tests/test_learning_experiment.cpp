// LearningExperiment Tests
// Tests for the advanced LearningExperiment class in main.cpp

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_learning_experiment {

void testLearningExperimentCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    LearningExperiment experiment(brain, 42);
    
    // Verify experiment was created
    assert(experiment.brain_ == &brain);
    assert(experiment.seed_ == 42);
    assert(experiment.initialSynapseCount_ == 0);
    assert(experiment.initialWeights_.empty());
    assert(experiment.finalWeights_.empty());
    assert(experiment.mostActiveNeurons_.empty());
    
    std::cout << "    testLearningExperimentCreation passed" << std::endl;
}

void testLearningExperimentRecordInitialState() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    LearningExperiment experiment(brain, 123);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Verify initial state recorded
    assert(experiment.initialSynapseCount_ == brain.getTotalSynapseCount());
    assert(!experiment.initialWeights_.empty());
    
    // Record final state after some steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Should have recorded some synapses
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testLearningExperimentRecordInitialState passed" << std::endl;
}

void testLearningExperimentRecordFinalState() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    LearningExperiment experiment(brain, 456);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Inject current to create activity
    for (size_t i = 0; i < 10 && i < brain.getTotalNeuronCount(); ++i) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
    }
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Verify final state recorded
    assert(experiment.initialSynapseCount_ == brain.getTotalSynapseCount());
    assert(!experiment.finalWeights_.empty());
    assert(experiment.mostActiveNeurons_.size() > 0);
    
    std::cout << "    testLearningExperimentRecordFinalState passed" << std::endl;
}

void testLearningExperimentComputeStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    LearningExperiment experiment(brain, 789);
    
    // Record initial and final states
    experiment.recordInitialState();
    
    // Apply some learning
    for (auto& syn : brain.getRegion(nlm::RegionId(1))->getSynapses()) {
        syn->enablePlasticity(true, true, false);
    }
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        brain.step(step);
    }
    
    experiment.recordFinalState();
    
    // Compute statistics - should not crash
    experiment.computeStatistics();
    
    // Verify results
    assert(experiment.getResults().totalSpikes > 0);
    assert(!experiment.getResults().learningOccurred || 
           experiment.getResults().weightChange != 0.0f);
    
    std::cout << "    testLearningExperimentComputeStatistics passed" << std::endl;
}

void testLearningExperimentResults() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    LearningExperiment experiment(brain, 999);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Simulate learning
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 25.0f);
        brain.step(step);
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Get results
    LearningExperiment::ExperimentResults results = experiment.getResults();
    
    // Verify results structure
    assert(results.totalSpikes >= 0);
    assert(results.mostActiveNeurons >= 0);
    assert(results.weightChange == results.finalMeanWeight - results.initialMeanWeight);
    
    // Verify learning detection logic
    bool expectedLearning = (std::abs(results.weightChange) > 0.001f) || 
                          (results.strengthened > 0 || results.weakened > 0) ||
                          (results.totalSpikes > 0);
    
    assert(results.learningOccurred == expectedLearning);
    
    std::cout << "    testLearningExperimentResults passed" << std::endl;
}

void testLearningExperimentErrorHandling() {
    // Test with null brain pointer
    LearningExperiment experiment(nullptr, 111);
    
    // These should not crash and should log errors
    experiment.recordInitialState();
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    LearningExperiment::ExperimentResults results = experiment.getResults();
    assert(results.totalSpikes == 0);
    assert(results.mostActiveNeurons == 0);
    assert(results.learningOccurred == false);
    
    std::cout << "    testLearningExperimentErrorHandling passed" << std::endl;
}

void testLearningExperimentComplexScenario() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    LearningExperiment experiment(brain, 222);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Apply complex learning scenario with multiple regions
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Inject current to both regions
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 15.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
        
        brain.step(step);
        
        // Every 50 steps, record intermediate state
        if (step % 50 == 0) {
            LearningExperiment intermediate(brain, step);
            intermediate.recordInitialState();
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute statistics
    experiment.computeStatistics();
    
    // Verify complex scenario worked
    LearningExperiment::ExperimentResults results = experiment.getResults();n    assert(results.totalSpikes > 0);
    assert(results.mostActiveNeurons > 0);
    assert(results.strengthened + results.weakened + results.unchanged > 0);
    
    std::cout << "    testLearningExperimentComplexScenario passed" << std::endl;
}

void runAll() {
    testLearningExperimentCreation();
    testLearningExperimentRecordInitialState();
    testLearningExperimentRecordFinalState();
    testLearningExperimentComputeStatistics();
    testLearningExperimentResults();
    testLearningExperimentErrorHandling();
    testLearningExperimentComplexScenario();
}

} // namespace test_learning_experiment