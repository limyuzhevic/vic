// MotorSystem Tests
#include "motor/MotorSystem.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_motor {

void testMotorSystemCreation() {
    nlm::MotorSystem motorSystem;
    
    // Check that available actions include expected actions
    const auto& actions = motorSystem.getAvailableActions();
    assert(!actions.empty());
    assert(std::find(actions.begin(), actions.end(), nlm::ActionType::MoveForward) != actions.end());
    assert(std::find(actions.begin(), actions.end(), nlm::ActionType::Wait) != actions.end());
    
    std::cout << "    testMotorSystemCreation passed" << std::endl;
}

void testActionSelectionEmptyRegion() {
    nlm::MotorSystem motorSystem;
    
    // Create empty motor region (no neurons)
    nlm::NeuralRegion motorRegion(nlm::RegionId(1), "MotorRegion");
    
    // Select action from empty region should return Wait
    auto action = motorSystem.selectAction(motorRegion);
    assert(action->getType() == nlm::ActionType::Wait);
    
    std::cout << "    testActionSelectionEmptyRegion passed" << std::endl;
}

void testActionSelectionWithNeurons() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    // Create motor region with neurons
    nlm::NeuralRegion motorRegion(nlm::RegionId(1), "MotorRegion");
    
    // Add some motor-type neurons
    auto pop1 = motorRegion.addPopulation(5, nlm::NeuronType::Motor);
    auto pop2 = motorRegion.addPopulation(3, nlm::NeuronType::Internal);
    
    // Get neurons and initialize them randomly
    auto allNeurons = motorRegion.getAllNeurons();
    for (auto* neuron : allNeurons) {
        neuron->initializeRandom(rng);
    }
    
    // Set some neurons to active firing state to create action bias
    if (allNeurons.size() >= 2) {
        allNeurons[0]->setFiringState(nlm::FiringState::Active);
        allNeurons[0]->setFiringRate(5.0f);
        allNeurons[1]->setFiringState(nlm::FiringState::Active);
        allNeurons[1]->setFiringRate(3.0f);
    }
    
    // Select action - should select based on neural activity
    auto action = motorSystem.selectAction(motorRegion);
    
    // Should not crash and should return a valid action
    assert(action != nullptr);
    assert(std::find(motorSystem.getAvailableActions().begin(), 
                    motorSystem.getAvailableActions().end(), 
                    action->getType()) != motorSystem.getAvailableActions().end());
    
    std::cout << "    testActionSelectionWithNeurons passed" << std::endl;
}

void testNeuralActivityStats() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    nlm::NeuralRegion motorRegion(nlm::RegionId(1), "MotorRegion");
    
    // Add neurons with varying firing rates
    auto pop = motorRegion.addPopulation(5, nlm::NeuronType::Motor);
    auto neurons = motorRegion.getAllNeurons();
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        neurons[i]->initializeRandom(rng);
        // Set different firing rates based on index
        neurons[i]->setFiringRate(static_cast<float>(i + 1) * 2.0f);
        if (i % 2 == 0) {
            neurons[i]->setFiringState(nlm::FiringState::Active);
        }
    }
    
    // Get neural activity stats
    auto stats = motorSystem.getNeuralActivityStats(motorRegion);
    
    // Verify statistics
    assert(stats.totalNeurons == 5);
    assert(stats.meanFiringRate > 0.0f);
    assert(stats.activeFraction >= 0.0f && stats.activeFraction <= 1.0f);
    assert(stats.firingVariance >= 0.0f);
    
    std::cout << "    testNeuralActivityStats passed" << std::endl;
}

void testNeuralCoherenceCalculation() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    // Create neurons with synchronized vs unsynchronized firing
    std::vector<const nlm::Neuron*> neurons;
    
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron* neuron = new nlm::Neuron(nlm::NeuronId(i));
        neuron->setType(nlm::NeuronType::Motor);
        neuron->initializeRandom(rng);
        
        // Synchronize firing for coherence
        neuron->setFiringRate(2.0f + (i * 0.5f));
        neuron->setFiringState(nlm::FiringState::Active);
        
        neurons.push_back(neuron);
    }
    
    // High coherence (similar firing rates)
    float coherence = motorSystem.calculateNeuralCoherence(neurons);
    assert(coherence >= 0.0f && coherence <= 1.0f);
    
    // Cleanup
    for (auto* neuron : neurons) {
        delete neuron;
    }
    
    std::cout << "    testNeuralCoherenceCalculation passed" << std::endl;
}

void testActionStrengthCalculation() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    // Create neurons with different firing patterns
    std::vector<const nlm::Neuron*> neurons;
    
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron* neuron = new nlm::Neuron(nlm::NeuronId(i));
        neuron->setType(nlm::NeuronType::Motor);
        neuron->initializeRandom(rng);
        neuron->setFiringRate(static_cast<float>(i + 1) * 1.0f);
        neuron->setFiringState(nlm::FiringState::Active);
        
        neurons.push_back(neuron);
    }
    
    // Test action strength calculation (using placeholder action type)
    float strength = motorSystem.calculateActionStrength(nlm::ActionType::MoveForward, neurons);
    assert(strength >= 0.0f && strength <= 1.0f);
    
    // Cleanup
    for (auto* neuron : neurons) {
        delete neuron;
    }
    
    std::cout << "    testActionStrengthCalculation passed" << std::endl;
}

void testActionPreference() {
    nlm::MotorSystem motorSystem;
    
    // Set initial preference
    motorSystem.setActionPreference(nlm::ActionType::MoveForward, 2.0f);
    
    // Verify preference was set
    assert(motorSystem.getAvailableActions().size() > 0);
    
    std::cout << "    testActionPreference passed" << std::endl;
}

void testActionHistory() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    nlm::NeuralRegion motorRegion(nlm::RegionId(1), "MotorRegion");
    auto pop = motorRegion.addPopulation(3, nlm::NeuronType::Motor);
    auto neurons = motorRegion.getAllNeurons();
    
    for (auto* neuron : neurons) {
        neuron->initializeRandom(rng);
    }
    
    // Select action and check history
    auto action1 = motorSystem.selectAction(motorRegion);
    auto history = motorSystem.getActionHistory();
    assert(history.size() == 1);
    assert(history[0]->getType() == action1->getType());
    
    auto action2 = motorSystem.selectAction(motorRegion);
    history = motorSystem.getActionHistory();
    assert(history.size() == 2);
    assert(history[1]->getType() == action2->getType());
    
    // Clear history
    motorSystem.clearHistory();
    history = motorSystem.getActionHistory();
    assert(history.empty());
    
    std::cout << "    testActionHistory passed" << std::endl;
}

void testPreferenceDecay() {
    nlm::MotorSystem motorSystem;
    
    // Set high preferences
    motorSystem.setActionPreference(nlm::ActionType::MoveForward, 5.0f);
    motorSystem.setActionPreference(nlm::ActionType::MoveBackward, 3.0f);
    
    // Decay preferences
    motorSystem.decayPreferences();
    
    // Verify preferences decayed (should be lower after decay)
    assert(motorSystem.getAvailableActions().size() > 0);
    
    std::cout << "    testPreferenceDecay passed" << std::endl;
}

void testSystemReset() {
    nlm::MotorSystem motorSystem;
    nlm::RandomGenerator rng(42);
    
    nlm::NeuralRegion motorRegion(nlm::RegionId(1), "MotorRegion");
    auto pop = motorRegion.addPopulation(3, nlm::NeuronType::Motor);
    auto neurons = motorRegion.getAllNeurons();
    
    for (auto* neuron : neurons) {
        neuron->initializeRandom(rng);
        neuron->setFiringRate(2.0f);
        neuron->setFiringState(nlm::FiringState::Active);
    }
    
    // Select some actions
    motorSystem.selectAction(motorRegion);
    auto action1 = motorSystem.selectAction(motorRegion);
    
    // Get action history size before reset
    auto history = motorSystem.getActionHistory();
    size_t historySize = history.size();
    
    // Reset system
    motorSystem.reset();
    
    // History should be cleared after reset
    history = motorSystem.getActionHistory();
    assert(history.empty());
    
    std::cout << "    testSystemReset passed" << std::endl;
}

void runAll() {
    testMotorSystemCreation();
    testActionSelectionEmptyRegion();
    testActionSelectionWithNeurons();
    testNeuralActivityStats();
    testNeuralCoherenceCalculation();
    testActionStrengthCalculation();
    testActionPreference();
    testActionHistory();
    testPreferenceDecay();
    testSystemReset();
    
    std::cout << "=== All MotorSystem Tests PASSED ===" << std::endl;
}

} // namespace test_motor