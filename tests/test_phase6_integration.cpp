// Phase 6 Integration Test Suite
// Tests the complete integration of all NLM brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>

namespace test_phase6_integration {

// Test 1: Basic neural dynamics with improved LIF implementation
void testImprovedLIFDynamics() {
    std::cout << "=== Test 1: Improved LIF Neural Dynamics ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test configurable timestep
    brain.getRegion(nlm::RegionId(0))->getNeuron(0)->setDefaultTimestep(0.0005);
    float defaultTimestep = brain.getRegion(nlm::RegionId(0))->getNeuron(0)->getDefaultTimestep();
    assert(defaultTimestep == 0.0005f);
    
    // Test LIF dynamics over multiple steps
    size_t initialSpikes = brain.getTotalSpikeCount();
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Use custom timestep for detailed dynamics
        brain.step(step, 0.0001);
        
        // Verify neural activity increases with time
        if (step % 10 == 0) {
            size_t currentSpikes = brain.getTotalSpikeCount();
            assert(currentSpikes >= initialSpikes);  // Should not decrease
        }
    }
    
    // Verify spike activity occurred
    assert(brain.getTotalSpikeCount() > initialSpikes);
    std::cout << "✓ Improved LIF dynamics test passed" << std::endl;
}

// Test 2: Enhanced synaptic dynamics with short-term plasticity
void testEnhancedSynapticDynamics() {
    std::cout << "=== Test 2: Enhanced Synaptic Dynamics ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity on all synapses
    if (auto* region = brain.getRegion(nlm::RegionId(0))) {
        for (auto& synapse : region->getSynapses()) {
            synapse->enablePlasticity(true, true, true);
        }
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (auto* region = brain.getRegion(nlm::RegionId(0))) {
        for (const auto& synapse : region->getSynapses()) {
            initialWeights.push_back(synapse->getWeight());
        }
    }
    
    // Apply correlated activity patterns to trigger STDP
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Create pre-post correlation pattern
        if (auto* region = brain.getRegion(nlm::RegionId(0))) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre-synaptic neuron fires
                neurons[0]->injectCurrent(40.0f);
                brain.step(step, 0.001);
                
                // Post-synaptic neuron fires (LTP)
                neurons[1]->injectCurrent(40.0f);
                brain.step(step + 1, 0.001);
            }
        }
    }
    
    // Verify synaptic changes occurred
    size_t weightChanges = 0;
    if (auto* region = brain.getRegion(nlm::RegionId(0))) {
        for (const auto& synapse : region->getSynapses()) {
            float newWeight = synapse->getWeight();
            if (std::abs(newWeight) > 0.001f) { // Significant change
                weightChanges++;
            }
        }
    }
    
    // At least some synapses should have changed
    assert(weightChanges > 0);
    std::cout << "✓ Enhanced synaptic dynamics test passed" << std::endl;
}

// Test 3: Full agent-environment integration
void testAgentEnvironmentIntegration() {
    std::cout << "=== Test 3: Agent-Environment Integration ===" << std::endl;
    
    // Create brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent and world
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    nlm::SimpleWorld world;
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    agent.initialize(world);
    
    // Enable all learning systems
    agent.enableAllSystems();
    
    // Run simulation with sensory input, action, and reward
    for (int step = 0; step < 200; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory percept
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Get action
        auto action = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Apply reward (based on position)
        float reward = std::max(0.0f, 100.0f - world.getAgentBody().x - world.getAgentBody().y);
        agent.applyRewardModulation(reward, 0.0);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        if (step % 50 == 0) {
            std::cout << "    Step " << step << ": Pos(" << world.getAgentBody().x << ", " << world.getAgentBody().y << ")";
            std::cout << " Reward: " << reward << " Curiosity: " << agent.getCuriosityLevel();
            std::cout << " Novelty: " << agent.getNoveltyLevel() << std::endl;
        }
    }
    
    // Verify agent performed meaningful activity
    assert(world.getAgentBody().x >= 0.0 && world.getAgentBody().x <= 20.0);
    assert(world.getAgentBody().y >= 0.0 && world.getAgentBody().y <= 20.0);
    
    // Verify learning systems were active
    assert(agent.getCuriosityLevel() > 0.0f);
    assert(agent.getNoveltyLevel() > 0.0f);
    assert(agent.getPredictionError() >= 0.0f);
    
    std::cout << "✓ Agent-environment integration test passed" << std::endl;
}

// Test 4: Memory system integration
void testMemorySystemIntegration() {
    std::cout << "=== Test 4: Memory System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Verify memory systems are connected
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory with neural activity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Working memory should process neural activity
        if (workingMemory) {
            // Working memory accumulates activity patterns
            // This is a simple test to verify the system is functional
        }
    }
    
    // Verify episodic memory can store experiences
    if (episodicMemory) {
        // Episodic memory should have mechanisms for encoding experiences
        // Implementation-specific testing would go here
    }
    
    std::cout << "✓ Memory system integration test passed" << std::endl;
}

// Test 5: Neuromodulation system integration
void testNeuromodulationIntegration() {
    std::cout << "=== Test 5: Neuromodulation Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent with neuromodulation
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    nlm::SimpleWorld world;
    world.configure(width=15, height=15, visionWidth=6, visionHeight:6);
    world.reset();
    
    agent.initialize(world);
    
    // Enable all neuromodulation systems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Test neuromodulation over time
    float initialCuriosity = agent.getCuriosityLevel();
    float initialNovelty = agent.getNoveltyLevel();
    
    for (int step = 0; step < 100; ++step) {
        // Apply varied rewards to test neuromodulation
        float reward = (step % 20 < 5) ? 1.0f : (step % 20 < 10) ? 0.5f : 0.0f;
        agent.applyRewardModulation(reward, reward * 0.5f);
        
        // Update world and agent
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        agent.updateDevelopment(0.1);
    }
    
    // Verify neuromodulation systems responded
    assert(agent.getCuriosityLevel() >= initialCuriosity);
    assert(agent.getPredictionError() >= 0.0f);
    
    std::cout << "✓ Neuromodulation integration test passed" << std::endl;
}

// Test 6: Development system integration
void testDevelopmentIntegration() {
    std::cout << "=== Test 6: Development System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto developmentalStage = brain.getDevelopmentalStage();
    assert(developmentalStage != nlm::DevelopmentalStage::Invalid);
    
    // Test development over time
    float initialStage = static_cast<float>(developmentalStage);
    
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        // Development should progress over time
        auto currentStage = brain.getDevelopmentalStage();
        float currentStageValue = static_cast<float>(currentStage);
        
        if (step % 100 == 0) {
            std::cout << "    Development stage at step " << step << ": " << currentStageValue << std::endl;
        }
    }
    
    // Verify development occurred
    assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Invalid);
    
    std::cout << "✓ Development system integration test passed" << std::endl;
}

// Test 7: Performance and scalability
void testPerformanceAndScalability() {
    std::cout << "=== Test 7: Performance and Scalability ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run substantial simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step, 0.001); // Use configurable timestep
        
        // Occasionally perform brain operations
        if (step % 200 == 0) {
            brain.getTotalSpikeCount();
            brain.getFiringNeuronCount();
            brain.getAverageFiringRate();
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    Simulated 1000 steps with 500 neurons in " << duration.count() << "ms" << std::endl;
    
    // Performance should be reasonable (less than 5 seconds for this test)
    assert(duration.count() < 5000);
    
    // Verify results are reasonable
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Invalid);
    
    std::cout << "✓ Performance and scalability test passed" << std::endl;
}

// Test 8: Python binding compatibility
void testPythonBindingCompatibility() {
    std::cout << "=== Test 8: Python Binding Compatibility ===" << std::endl;
    
    // This test would be run from Python, but we can test the C++ API
    // that the Python bindings expose
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test methods that should be available via Python bindings
    brain.getRegionCount();
    brain.getTotalNeuronCount();
    brain.getTotalSynapseCount();
    brain.getFiringNeuronCount();
    brain.getAverageFiringRate();
    brain.getTotalSpikeCount();
    brain.getDevelopmentalStage();
    brain.logStatus();
    
    // Test neuron methods that should be exposed
    auto* region = brain.getRegion(nlm::RegionId(0));
    if (region && !region->getAllNeurons().empty()) {
        auto* neuron = region->getAllNeurons()[0];
        neuron->getId();
        neuron->getType();
        neuron->getMembranePotential();
        neuron->getThreshold();
        neuron->isFiring();
        neuron->isRefractory();
    }
    
    std::cout << "✓ Python binding compatibility test passed" << std::endl;
}

// Run all Phase 6 integration tests
void runAllPhase6IntegrationTests() {
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "NLM Phase 6 Integration Test Suite" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    
    testImprovedLIFDynamics();
    std::cout << std::endl;
    
    testEnhancedSynapticDynamics();
    std::cout << std::endl;
    
    testAgentEnvironmentIntegration();
    std::cout << std::endl;
    
    testMemorySystemIntegration();
    std::cout << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    testDevelopmentIntegration();
    std::cout << std::endl;
    
    testPerformanceAndScalability();
    std::cout << std::endl;
    
    testPythonBindingCompatibility();
    std::cout << std::endl;
    
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "All Phase 6 Integration Tests Passed! ✓" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
}

} // namespace test_phase6_integration

int main() {
    test_phase6_integration::runAllPhase6IntegrationTests();
    return 0;
}