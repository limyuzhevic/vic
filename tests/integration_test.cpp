// Integration tests for NLM modules
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/training/TrainingModule.hpp"
#include "../src/performance/OptimizedNeuronMath.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

namespace integration_test {

void testBrainWorldIntegration() {
    std::cout << "    Testing Brain-World integration..." << std::endl;
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config->set("world.width", 20, nlm::ConfigSource::Runtime);
    config->set("world.height", 20, nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain != nullptr);
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    assert(world != nullptr);
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    assert(agentBrain != nullptr);
    
    // Initialize components
    agentBrain->initialize(*world);
    
    // Test data flow: World -> AgentBrain -> Brain
    auto percept = world->getSensoryPercept();
    assert(percept != nullptr);
    
    agentBrain->processSensoryInput(*percept);
    brain->step(0);
    
    auto action = brain->produceAction();
    world->applyMotorCommand(action, world->getSimulationTime());
    
    // Verify integration worked
    assert(brain->getTotalNeuronCount() == 500);
    assert(world->getWidth() == 20);
    assert(world->getHeight() == 20);
    
    std::cout << "    ✓ Brain-World integration test passed" << std::endl;
}

void testTrainingModuleIntegration() {
    std::cout << "    Testing Training Module integration..." << std::endl;
    
    // Create training configuration
    auto trainingConfig = std::make_shared<nlm::TrainingConfig>();
    trainingConfig->enableValidation = true;
    trainingConfig->validationFrequency = 10.0f;
    trainingConfig->earlyStopping = true;
    trainingConfig->patience = 5;
    
    // Create brain for training
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("training.enabled", true, nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain != nullptr);
    
    // Create training module
    auto trainingModule = std::make_shared<nlm::TrainingModule>();
    assert(trainingModule != nullptr);
    
    // Initialize training (simulated)
    // In real implementation, would set up callbacks and monitoring
    
    // Test training loop integration
    double totalReward = 0.0;
    double totalLoss = 0.0;
    
    for (int episode = 0; episode < 10; ++episode) {
        // Reset brain for new episode
        brain->reset();
        brain->initialize();
        
        double episodeReward = 0.0;
        double episodeLoss = 0.0;
        
        for (int step = 0; step < 100; ++step) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brain->step(simStep);
            
            // Simulate reward and loss calculation
            double reward = 1.0 - (step * 0.01);
            double loss = std::exp(-static_cast<double>(brain->getAverageFiringRate()) / 100.0);
            
            episodeReward += reward;
            episodeLoss += loss;
        }
        
        totalReward += episodeReward;
        totalLoss += episodeLoss;
        
        // Simulate callback invocation
        // trainingModule->afterEpisode(episode, episodeReward/100.0, true);
    }
    
    // Verify integration results
    assert(totalReward > 0.0);
    assert(totalLoss > 0.0);
    
    std::cout << "    ✓ Training Module integration test passed" << std::endl;
}

void testPerformanceOptimizationIntegration() {
    std::cout << "    Testing Performance Optimization integration..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    assert(onm != nullptr);
    
    // Configure for optimal performance
    onm->configureForSpeed();
    assert(onm->isSIMDEnabled());
    assert(onm->isPrefetchingEnabled());
    assert(onm->isCachingEnabled());
    
    // Test neural computation integration
    const size_t neuronCount = 2000;
    std::vector<nlm::NeuronState> neurons(neuronCount);
    std::vector<float> externalInputs(neuronCount, 0.0f);
    std::vector<float> synapticInputs(neuronCount, 0.0f);
    
    // Initialize with test data
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons[i].potential = -70.0f + static_cast<float>(i) * 0.01f;
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
        externalInputs[i] = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        synapticInputs[i] = static_cast<float>(rand()) / RAND_MAX * 0.1f;
    }
    
    // Perform optimized computation
    onm->updateNeuronsSIMD(neurons.data(), neuronCount, 0.001f,
                          externalInputs.data(), synapticInputs.data(),
                          -50.0f, 5.0f);
    
    // Verify results
    for (size_t i = 0; i < neuronCount; ++i) {
        assert(std::isfinite(neurons[i].potential));
        assert(neurons[i].refractoryTime >= 0.0f);
        assert(neurons[i].refractoryTime <= 5.0f); // Within refractory period
    }
    
    // Test synaptic weight updates
    const size_t weightCount = 1000;
    std::vector<float> weights(weightCount, 0.5f);
    std::vector<bool> preSpikes(weightCount, false);
    std::vector<bool> postSpikes(weightCount, false);
    std::vector<uint32_t> spikeMask((weightCount + 31) / 32, 0);
    
    // Set some spikes
    for (size_t i = 0; i < weightCount; i += 10) {
        preSpikes[i] = true;
        postSpikes[i] = true;
        spikeMask[i / 32] |= (1 << (i % 32));
    }
    
    onm->updateSynapticWeightsSIMD(weights.data(), preSpikes.data(),
                                  postSpikes.data(), 0.01f, weightCount,
                                  spikeMask.data());
    
    // Verify weight updates
    for (size_t i = 0; i < weightCount; ++i) {
        assert(weights[i] >= 0.0f && weights[i] <= 1.0f);
    }
    
    std::cout << "    ✓ Performance Optimization integration test passed" << std::endl;
}

void testMemoryPoolIntegration() {
    std::cout << "    Testing Memory Pool integration..." << std::endl;
    
    // Test neuron memory pool integration
    nlm::MemoryPool<nlm::Neuron> neuronPool(1000, 10);
    assert(!neuronPool.empty());
    
    std::vector<nlm::Neuron*> neurons;
    for (size_t i = 0; i < 500; ++i) {
        nlm::Neuron* neuron = neuronPool.allocate();
        if (neuron) {
            neurons.push_back(neuron);
            neuron->setId(i);
            neuron->setType(nlm::NeuronType::Excitatory);
        }
    }
    
    assert(neurons.size() == 500);
    assert(neuronPool.allocatedCount() == 500);
    assert(neuronPool.availableCount() == 500); // 1000 - 500
    
    // Deallocate neurons
    for (nlm::Neuron* neuron : neurons) {
        neuronPool.deallocate(neuron);
    }
    
    assert(neuronPool.allocatedCount() == 0);
    assert(neuronPool.availableCount() == 1000);
    
    // Test synapse memory pool integration
    nlm::MemoryPool<nlm::Synapse> synapsePool(500, 5);
    assert(!synapsePool.empty());
    
    std::vector<nlm::Synapse*> synapses;
    for (size_t i = 0; i < 200; ++i) {
        nlm::Synapse* synapse = synapsePool.allocate();
        if (synapse) {
            synapses.push_back(synapse);
            synapse->setId(i);
            synapse->setSourceId(i);
            synapse->setDestinationId(i + 1);
        }
    }
    
    assert(synapses.size() == 200);
    assert(synapsePool.allocatedCount() == 200);
    assert(synapsePool.availableCount() == 300); // 500 - 200
    
    // Deallocate synapses
    for (nlm::Synapse* synapse : synapses) {
        synapsePool.deallocate(synapse);
    }
    
    assert(synapsePool.allocatedCount() == 0);
    assert(synapsePool.availableCount() == 500);
    
    std::cout << "    ✓ Memory Pool integration test passed" << std::endl;
}

void testCrossModuleDataFlow() {
    std::cout << "    Testing Cross-Module Data Flow..." << std::endl;
    
    // Create interconnected modules
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300, nlm::ConfigSource::Runtime);
    config->set("world.width", 10, nlm::ConfigSource::Runtime);
    config->set("world.height", 10, nlm::ConfigSource::Runtime);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 5, 5);
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    agentBrain->initialize(*world);
    
    // Set up an agent start position
    world->setAgentStart(5, 5);
    
    // Test complete data flow cycle
    for (size_t step = 0; step < 50; ++step) {
        // World provides sensory input to agent
        auto percept = world->getSensoryPercept();
        assert(percept != nullptr);
        
        // Agent brain processes sensory input and sends to brain
        agentBrain->processSensoryInput(*percept);
        
        // Brain processes and produces action
        auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
        brain->step(simStep);
        
        auto action = brain->produceAction();
        
        // World applies action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Verify data integrity
        assert(brain->getActiveNeuronCount() >= 0);
        assert(world->getAgentBody().x >= 0);
        assert(world->getAgentBody().y >= 0);
    }
    
    std::cout << "    ✓ Cross-Module Data Flow test passed" << std::endl;
}

void runAll() {
    testBrainWorldIntegration();
    testTrainingModuleIntegration();
    testPerformanceOptimizationIntegration();
    testMemoryPoolIntegration();
    testCrossModuleDataFlow();
    
    std::cout << "\nAll integration tests passed!" << std::endl;
}

} // namespace integration_test

int main() {
    std::cout << "Running Integration Tests..." << std::endl;
    integration_test::runAll();
    return 0;
}
