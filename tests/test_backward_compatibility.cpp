// Backward compatibility tests for NLM
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <algorithm>

namespace test_backward_compatibility {

void testConfigBackwardCompatibility() {
    std::cout << "    Testing Config backward compatibility..." << std::endl;
    
    // Test old-style config initialization
    nlm::Config config;
    
    // Test old key formats
    config.set("neuron_count", 1000);
    config.set("learning_rate", 0.01f);
    config.set("world.width", 20);
    config.set("world.height", 20);
    config.set("brain.excitatory_ratio", 0.8f);
    
    // Verify keys are accessible
    assert(config.has("neuron_count"));
    assert(config.has("learning_rate"));
    assert(config.has("world.width"));
    assert(config.has("world.height"));
    assert(config.has("brain.excitatory_ratio"));
    
    // Test old getter methods
    int neuronCount = config.getOr<int>("neuron_count", 500);
    assert(neuronCount == 1000);
    
    float learningRate = config.getOr<float>("learning_rate", 0.001f);
    assert(learningRate == 0.01f);
    
    int worldWidth = config.getOr<int>("world.width", 10);
    assert(worldWidth == 20);
    
    // Test default value loading compatibility
    int missingValue = config.getOr<int>("missing.key", 42);
    assert(missingValue == 42);
    
    std::cout << "    ✓ Config backward compatibility test passed" << std::endl;
}

void testBrainBackwardCompatibility() {
    std::cout << "    Testing Brain backward compatibility..." << std::endl;
    
    // Test old-style brain initialization
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain != nullptr);
    
    // Test old-style initialization method
    brain->initialize();
    
    // Test old-style step method
    auto oldStep = nlm::SimulationStep{0, 0.0f};
    brain->step(oldStep);
    
    // Test old-style action production
    auto action = brain->produceAction();
    
    // Test old-style reset
    brain->reset();
    assert(brain->getTotalSpikeCount() == 0);
    
    // Test old-style state access
    size_t totalNeurons = brain->getTotalNeuronCount();
    assert(totalNeurons == 500);
    
    size_t activeNeurons = brain->getActiveNeuronCount();
    assert(activeNeurons >= 0);
    
    size_t firingNeurons = brain->getFiringNeuronCount();
    assert(firingNeurons >= 0);
    
    float firingRate = brain->getAverageFiringRate();
    assert(firingRate >= 0.0f);
    
    std::cout << "    ✓ Brain backward compatibility test passed" << std::endl;
}

void testWorldBackwardCompatibility() {
    std::cout << "    Testing World backward compatibility..." << std::endl;
    
    // Test old-style world initialization
    auto world = std::make_shared<nlm::SimpleWorld>();
    assert(world != nullptr);
    
    // Test old-style configuration
    world->configure(15, 15, 6, 6);
    assert(world->getWidth() == 15);
    assert(world->getHeight() == 15);
    
    // Test old-style agent placement
    world->setAgentStart(7, 7);
    assert(world->getAgentBody().x == 7);
    assert(world->getAgentBody().y == 7);
    
    // Test old-style update
    world->update(0.1f);
    
    // Test old-style action application
    auto action = nlm::Action(nlm::ActionType::MoveForward);
    world->applyMotorCommand(action, 0.0f);
    
    // Test old-style reset
    world->reset();
    assert(world->getSimulationTime() == 0.0f);
    
    std::cout << "    ✓ World backward compatibility test passed" << std::endl;
}

void testAgentBrainBackwardCompatibility() {
    std::cout << "    Testing AgentBrain backward compatibility..." << std::endl;
    
    // Create brain and world
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 5, 5);
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    assert(agentBrain != nullptr);
    
    // Test old-style initialization
    agentBrain->initialize(*world);
    
    // Test old-style sensory processing
    auto percept = world->getSensoryPercept();
    agentBrain->processSensoryInput(*percept);
    
    // Test old-style motor command decoding
    auto action = agentBrain->decodeMotorCommand();
    
    // Test old-style reset
    agentBrain->reset();
    
    // Test old-style feature access
    assert(agentBrain->getSensoryInputSize() == 25); // 5x5 vision
    assert(agentBrain->getMotorOutputSize() == 18); // Standard motor commands
    
    std::cout << "    ✓ AgentBrain backward compatibility test passed" << std::endl;
}

void testDataTypeBackwardCompatibility() {
    std::cout << "    Testing Data Type backward compatibility..." << std::endl;
    
    // Test old-style NeuronId usage
    nlm::NeuronId neuronId;
    neuronId.value = 123;
    assert(neuronId.value == 123);
    assert(neuronId.index() == 123);
    
    // Test old-style ActionType usage
    auto action = nlm::Action(nlm::ActionType::MoveForward);
    assert(action.getType() == nlm::ActionType::MoveForward);
    
    // Test old-style Vision usage
    auto vision = nlm::Vision(10, 10, 3);
    assert(vision.getWidth() == 10);
    assert(vision.getHeight() == 10);
    assert(vision.getChannels() == 3);
    
    // Test old-style Audio usage
    auto audio = nlm::Audio(44100, 1024);
    assert(audio.getSampleRate() == 44100);
    assert(audio.getNumSamples() == 1024);
    
    // Test old-style InternalSignals usage
    auto internal = nlm::InternalSignals();
    internal.addSignal(0.5f);
    internal.addSignal(1.0f);
    assert(internal.getAllSignals().size() == 2);
    
    std::cout << "    ✓ Data Type backward compatibility test passed" << std::endl;
}

void testPerformanceBackwardCompatibility() {
    std::cout << "    Testing Performance Optimization backward compatibility..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    assert(onm != nullptr);
    
    // Test old-style configuration
    onm->configureOptimal();
    assert(onm->isSIMDEnabled());
    assert(onm->isPrefetchingEnabled());
    assert(onm->isCachingEnabled());
    
    // Test old-style configuration for memory
    onm->configureForMemory();
    assert(onm->isSIMDEnabled());
    assert(onm->isCachingEnabled());
    // Prefetching should be disabled for memory optimization
    assert(!onm->isPrefetchingEnabled());
    
    // Test old-style configuration for speed
    onm->configureForSpeed();
    assert(onm->isSIMDEnabled());
    assert(onm->isPrefetchingEnabled());
    assert(onm->isCachingEnabled());
    
    // Test old-style configuration for latency
    onm->configureForLatency();
    assert(onm->isSIMDEnabled());
    assert(onm->isCachingEnabled());
    // Prefetching should be disabled for latency optimization
    assert(!onm->isPrefetchingEnabled());
    
    // Test old-style benchmarking
    onm->benchmarkNeuronalUpdates(1000, 10);
    
    std::cout << "    ✓ Performance Optimization backward compatibility test passed" << std::endl;
}

void testMemoryPoolBackwardCompatibility() {
    std::cout << "    Testing Memory Pool backward compatibility..." << std::endl;
    
    // Test old-style NeuronPool (using MemoryPool<Neuron>)
    nlm::MemoryPool<nlm::Neuron> neuronPool(100, 5);
    assert(!neuronPool.empty());
    
    // Test old-style allocation pattern
    std::vector<nlm::Neuron*> neurons;
    for (size_t i = 0; i < 50; ++i) {
        nlm::Neuron* neuron = neuronPool.allocate();
        if (neuron) {
            neurons.push_back(neuron);
            neuron->setId(i);
        }
    }
    
    assert(neurons.size() == 50);
    
    // Test old-style deallocation pattern
    for (nlm::Neuron* neuron : neurons) {
        neuronPool.deallocate(neuron);
    }
    
    assert(neuronPool.allocatedCount() == 0);
    
    // Test old-style SynapsePool (using MemoryPool<Synapse>)
    nlm::MemoryPool<nlm::Synapse> synapsePool(50, 3);
    assert(!synapsePool.empty());
    
    std::vector<nlm::Synapse*> synapses;
    for (size_t i = 0; i < 25; ++i) {
        nlm::Synapse* synapse = synapsePool.allocate();
        if (synapse) {
            synapses.push_back(synapse);
            synapse->setId(i);
        }
    }
    
    assert(synapses.size() == 25);
    
    // Test old-style deallocation pattern
    for (nlm::Synapse* synapse : synapses) {
        synapsePool.deallocate(synapse);
    }
    
    assert(synapsePool.allocatedCount() == 0);
    
    std::cout << "    ✓ Memory Pool backward compatibility test passed" << std::endl;
}

void testErrorHandlingBackwardCompatibility() {
    std::cout << "    Testing Error Handling backward compatibility..." << std::endl;
    
    // Test old-style error conditions
    auto config = std::make_shared<nlm::Config>();
    
    // Test with potentially invalid values (should handle gracefully)
    config->set("neuron_count", 0, nlm::ConfigSource::Runtime); // Zero neurons
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize(); // Should handle or validate
    
    // Test with invalid world size
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(0, 0, 8, 8); // Invalid size
    
    // Should handle gracefully
    world->reset();
    
    // Test old-style exception handling (simulated)
    bool exceptionHandled = false;
    try {
        // Simulate old-style operation that might fail
        auto action = nlm::Action(); // Default constructor
        // Should not throw
        exceptionHandled = true;
    } catch (...) {
        exceptionHandled = false;
    }
    
    assert(exceptionHandled);
    
    std::cout << "    ✓ Error Handling backward compatibility test passed" << std::endl;
}

void runAll() {
    testConfigBackwardCompatibility();
    testBrainBackwardCompatibility();
    testWorldBackwardCompatibility();
    testAgentBrainBackwardCompatibility();
    testDataTypeBackwardCompatibility();
    testPerformanceBackwardCompatibility();
    testMemoryPoolBackwardCompatibility();
    testErrorHandlingBackwardCompatibility();
    
    std::cout << "\nAll backward compatibility tests passed!" << std::endl;
}

} // namespace test_backward_compatibility

int main() {
    std::cout << "Running Backward Compatibility Tests..." << std::endl;
    test_backward_compatibility::runAll();
    return 0;
}
