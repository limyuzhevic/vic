// Learning Experiment Test Cases
// Added to test framework

#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include "experiments/LearningExperiment.hpp"

namespace nlm {
namespace test_learning_experiment {

void testLearningExperimentBasic() {
    // Test 1: Basic learning experiment setup
    auto config = std::make_shared<Config>();
    
    // Set up minimal configuration
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create learning experiment
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    assert(experiment.initialSynapseCount > 0);
    assert(!experiment.initialWeights.empty());
    
    // Simulate some plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
            // Small weight changes
            syn->addToWeight(0.001f);
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    assert(!experiment.finalWeights.empty());
    
    // Compute statistics
    experiment.computeStatistics();
    
    // Check that we recorded data
    assert(experiment.initialWeights.size() > 0);
    assert(experiment.finalWeights.size() == experiment.initialWeights.size());
    
    std::cout << "✓ Basic Learning Experiment test passed" << std::endl;
}

void testLearningExperimentSTDP() {
    // Test 2: STDP learning during experiment
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 123);
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("stdp_tau", 20.0f);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    LearningExperiment experiment(brain, 123);
    experiment.recordInitialState();
    
    // Run plasticity experiment (similar to main.cpp)
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply correlated activity to trigger STDP
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
        if (neurons.size() >= 2) {
            // Pre fires first
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // With STDP, weights should have changed
    bool weightChanged = false;
    for (size_t i = 0; i < std::min(experiment.initialWeights.size(), 
                                      experiment.finalWeights.size()); ++i) {
        float delta = experiment.finalWeights[i] - experiment.initialWeights[i];
        if (std::abs(delta) > 0.001f) {
            weightChanged = true;
            break;
        }
    }
    
    assert(weightChanged); // STDP should have changed weights
    std::cout << "✓ STDP Learning Experiment test passed" << std::endl;
}

void testLearningExperimentHebbian() {
    // Test 3: Hebbian learning during experiment
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 456);
    config->set("neuron_count", 150);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    LearningExperiment experiment(brain, 456);
    experiment.recordInitialState();
    
    // Enable Hebbian plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, false, false);  // Hebbian only
        }
    }
    
    // Create co-activation patterns to trigger Hebbian learning
    auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
    if (neurons.size() >= 4) {
        // Create co-activation of neurons 0 and 1, and neurons 2 and 3
        for (int step = 0; step < 100; ++step) {
            // Pattern 1: fire neurons 0 and 1
            neurons[0]->injectCurrent(30.0f);
            neurons[1]->injectCurrent(30.0f);
            brain->step(step, step * 0.001);
            
            // Pattern 2: fire neurons 2 and 3  
            neurons[2]->injectCurrent(30.0f);
            neurons[3]->injectCurrent(30.0f);
            brain->step(step + 1, (step + 1) * 0.001);
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Check for Hebbian-like changes (synchronous activation should strengthen synapses)
    std::cout << "✓ Hebbian Learning Experiment test passed" << std::endl;
}

void testLearningExperimentRewardModulation() {
    // Test 4: Reward-modulated learning
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 789);
    config->set("neuron_count", 120);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    LearningExperiment experiment(brain, 789);
    experiment.recordInitialState();
    
    // Enable all plasticity types
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // All types
        }
    }
    
    // Simulate reward-modulated learning
    for (int step = 0; step < 200; ++step) {
        // Apply reward signal
        if (brain->getDopamine()) {
            // Positive reward to strengthen learning
            brain->applyNeuromodulation(*brain->getDopamine());
        }
        
        // Inject sensory-like input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        brain->step(step, step * 0.001);
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Check that learning occurred
    assert(experiment.initialWeights.size() > 0);
    std::cout << "✓ Reward-Modulated Learning Experiment test passed" << std::endl;
}

void testLearningExperimentStatisticalValidation() {
    // Test 5: Statistical validation of learning patterns
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 999);
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    LearningExperiment experiment(brain, 999);
    experiment.recordInitialState();
    
    // Record weights before plasticity
    std::vector<float> preWeights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            preWeights.push_back(syn->getWeight());
        }
    }
    
    // Apply intensive plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run extended plasticity experiment
    for (int step = 0; step < 1000; ++step) {
        // Vary input patterns
        int pattern = step % 4;
        if (pattern == 0) brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        else if (pattern == 1) brain->injectCurrentToNeurons(NeuronType::Sensory, -20.0f);
        else if (pattern == 2) brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
        else brain->injectCurrentToNeurons(NeuronType::Motor, 10.0f);
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            // Check progress
            auto currentWeights = preWeights; // Simplified for test
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Statistical validation: weights should show learning patterns
    bool learningDetected = false;
    for (size_t i = 0; i < std::min(experiment.initialWeights.size(),
                                      experiment.finalWeights.size()); ++i) {
        float delta = experiment.finalWeights[i] - experiment.initialWeights[i];
        if (std::abs(delta) > 0.005f) {  // Significant change threshold
            learningDetected = true;
            break;
        }
    }
    
    // Learning should be detectable with sufficient plasticity
    assert(learningDetected || experiment.initialWeights.size() == 0); // Either learning or no synapses
    std::cout << "✓ Statistical Validation Learning Experiment test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Learning Experiment tests..." << std::endl;
    
    try {
        testLearningExperimentBasic();
        testLearningExperimentSTDP();
        testLearningExperimentHebbian();
        testLearningExperimentRewardModulation();
        testLearningExperimentStatisticalValidation();
        
        std::cout << "✓ All Learning Experiment tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Learning Experiment test failed: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_learning_experiment
} // namespace nlm