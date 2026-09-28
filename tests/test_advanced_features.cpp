// NLM Advanced Features Tests
// Test new core features: Event System, Plasticity, Multi-Region Coordination, Replay System, Development System

#include "performance/EventQueue.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Reward.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>

namespace test_advanced_features {

void testEnhancedEventSystem() {
    std::cout << "Testing Enhanced Event System..." << std::endl;
    
    // Test EventQueue - lock-free spike processing
    nlm::EventQueue eventQueue;
    assert(eventQueue.empty());
    
    // Create and push a spike event
    nlm::QueuedSpikeEvent spikeEvent(1, 2, 3, 0.5f, true);
    bool pushResult = eventQueue.push(spikeEvent);
    assert(pushResult);
    assert(!eventQueue.empty());
    
    // Pop the spike event
    auto poppedEvent = eventQueue.pop();
    assert(poppedEvent.has_value());
    assert(poppedEvent->sourceNeuron == 1);
    assert(poppedEvent->destNeuron == 2);
    assert(poppedEvent->weight == 0.5f);
    assert(poppedEvent->isExcitatory);
    
    assert(eventQueue.empty());
    
    // Test delayed spike queue
    nlm::DelayedSpikeQueue delayedQueue(1024, 100);
    delayedQueue.schedule(1, 2, 3, 0.5f, true, 10);
    
    assert(delayedQueue.pendingCount() > 0);
    
    // Test priority event queue
    nlm::PriorityEventQueue priorityQueue;
    priorityQueue.push(100, 0, 1, nullptr); // Highest priority
    priorityQueue.push(200, 1, 2, nullptr); // Lower priority
    
    assert(priorityQueue.size() == 2);
    
    auto event1 = priorityQueue.pop();
    assert(event1.has_value());
    assert(event1->priority == 0); // Higher priority
    
    auto event2 = priorityQueue.pop();
    assert(event2.has_value());
    assert(event2->priority == 1);
    
    assert(priorityQueue.empty());
    
    std::cout << "    testEnhancedEventSystem passed" << std::endl;
}

void testHomeostaticPlasticity() {
    std::cout << "Testing Homeostatic Plasticity..." << std::endl;
    
    // Test weight balance
    nlm::Brain brain;
    
    // Set up neurons and synapses
    auto neuron1 = brain.addNeuron(nlm::NeuronType::Excitatory);
    auto neuron2 = brain.addNeuron(nlm::NeuronType::Inhibitory);
    auto synapse = brain.addSynapse(neuron1->getId(), neuron2->getId());
    
    // Test target weight
    synapse->setTargetWeight(0.7f);
    assert(synapse->getTargetWeight() == 0.7f);
    
    // Test weight balance rate
    synapse->setBalanceRate(0.01f);
    assert(synapse->getBalanceRate() == 0.01f);
    
    // Test homeostatic scaling
    brain.setHomeostaticTarget(0.5f);
    assert(brain.getHomeostaticTarget() == 0.5f);
    
    brain.setHomeostaticRate(0.001f);
    assert(brain.getHomeostaticRate() == 0.001f);
    
    std::cout << "    testHomeostaticPlasticity passed" << std::endl;
}

void testRewardModulatedPlasticity() {
    std::cout << "Testing Reward-Modulated Plasticity..." << std::endl;
    
    // Test reward signals
    nlm::Reward reward;
    reward.setValue(1.0f);
    assert(reward.getValue() == 1.0f);
    
    reward.add(0.5f);
    assert(reward.getValue() == 1.5f);
    
    // Test eligibility traces
    nlm::Brain brain;
    auto neuron1 = brain.addNeuron(nlm::NeuronType::Excitatory);
    auto neuron2 = brain.addNeuron(nlm::NeuronType::Inhibitory);
    auto synapse = brain.addSynapse(neuron1->getId(), neuron2->getId());
    
    // Set eligibility trace
    synapse->setEligibilityTrace(0.8f);
    assert(synapse->getEligibilityTrace() == 0.8f);
    
    // Update eligibility based on reward
    float reward = 0.9f;
    float delta = synapse->getEligibilityTrace() * reward;
    synapse->updateWeight(delta);
    
    // Test reward-modulated STDP
    nlm::STDP stdp;
    stdp.setLearningRate(0.01f);
    assert(stdp.getLearningRate() == 0.01f);
    
    // Simulate reward-modulated plasticity
    stdp.setRewardSignal(0.7f);
    assert(stdp.getRewardSignal() == 0.7f);
    
    std::cout << "    testRewardModulatedPlasticity passed" << std::endl;
}

void testMultiRegionCoordination() {
    std::cout << "Testing Multi-Region Coordination..." << std::endl;
    
    // Test region interactions
    nlm::Brain brain;
    
    // Create multiple regions
    auto region1 = brain.addRegion();
    auto region2 = brain.addRegion();
    auto region3 = brain.addRegion();
    
    assert(region1->getId() != region2->getId());
    assert(region2->getId() != region3->getId());
    
    // Test region synchronization
    brain.setSynchronizationMode(nlm::SynchronizationMode::CrossRegion);
    assert(brain.getSynchronizationMode() == nlm::SynchronizationMode::CrossRegion);
    
    // Test inter-region connectivity
    auto neuron1 = brain.addNeuron(nlm::NeuronType::Excitatory, region1->getId());
    auto neuron2 = brain.addNeuron(nlm::NeuronType::Inhibitory, region2->getId());
    auto synapse = brain.addSynapse(neuron1->getId(), neuron2->getId());
    
    // Test region coupling
    region1->addCoupling(region2->getId(), 0.5f);
    assert(region1->getCoupling(region2->getId()) == 0.5f);
    
    std::cout << "    testMultiRegionCoordination passed" << std::endl;
}

void testEnhancedReplaySystem() {
    std::cout << "Testing Enhanced Replay System..." << std::endl;
    
    // Test replay strategies
    nlm::Brain brain;
    
    // Test offline replay
    brain.setReplayStrategy(nlm::ReplayStrategy::OffineReversal);
    assert(brain.getReplayStrategy() == nlm::ReplayStrategy::OffineReversal);
    
    // Test consolidation
    brain.setConsolidationMode(nlm::ConsolidationMode::Incremental);
    assert(brain.getConsolidationMode() == nlm::ConsolidationMode::Incremental);
    
    // Test replay timing
    brain.setReplayRate(0.1f); // 10% of normal speed
    assert(brain.getReplayRate() == 0.1f);
    
    // Test replay priority
    brain.setReplayPriority(nlm::ReplayPriority::High);
    assert(brain.getReplayPriority() == nlm::ReplayPriority::High);
    
    std::cout << "    testEnhancedReplaySystem passed" << std::endl;
}

void testAdvancedDevelopmentSystem() {
    std::cout << "Testing Advanced Development System..." << std::endl;
    
    // Test developmental stages
    nlm::DevelopmentSystem development;
    
    development.setStage(nlm::DevelopmentStage::Early); // Start in early stage
    assert(development.getStage() == nlm::DevelopmentStage::Early);
    
    development.setStage(nlm::DevelopmentStage::Middle);
    assert(development.getStage() == nlm::DevelopmentStage::Middle);
    
    development.setStage(nlm::DevelopmentStage::Late);
    assert(development.getStage() == nlm::DevelopmentStage::Late);
    
    // Test developmental rates
    development.setMaturationRate(0.01f);
    assert(development.getMaturationRate() == 0.01f);
    
    development.setSynaptogenesisRate(0.001f);
    assert(development.getSynaptogenesisRate() == 0.001f);
    
    development.setPruningRate(0.0001f);
    assert(development.getPruningRate() == 0.0001f);
    
    // Test developmental parameters
    development.setTargetSize(1000);
    assert(development.getTargetSize() == 1000);
    
    development.setOptimalConnectivity(0.15f);
    assert(development.getOptimalConnectivity() == 0.15f);
    
    std::cout << "    testAdvancedDevelopmentSystem passed" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Features Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test Enhanced Event System
    std::cout << "Running Enhanced Event System tests..." << std::endl;
    try {
        testEnhancedEventSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Homeostatic Plasticity
    std::cout << "Running Homeostatic Plasticity tests..." << std::endl;
    try {
        testHomeostaticPlasticity();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Reward-Modulated Plasticity
    std::cout << "Running Reward-Modulated Plasticity tests..." << std::endl;
    try {
        testRewardModulatedPlasticity();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Multi-Region Coordination
    std::cout << "Running Multi-Region Coordination tests..." << std::endl;
    try {
        testMultiRegionCoordination();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Enhanced Replay System
    std::cout << "Running Enhanced Replay System tests..." << std::endl;
    try {
        testEnhancedReplaySystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Advanced Development System
    std::cout << "Running Advanced Development System tests..." << std::endl;
    try {
        testAdvancedDevelopmentSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All Advanced Features Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Advanced Features Tests FAILED ===" << std::endl;
    }
}

} // namespace test_advanced_features