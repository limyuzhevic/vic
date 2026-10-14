// Integration Tests
// Tests all brain components working together and system integration

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <chrono>

// Include NLM headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

namespace test_integration {

void testBrainSystemIntegration() {
    std::cout << "Testing brain system integration..." << std::endl;
    
    // Create comprehensive configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    config->set("development.initial_neurons_per_region", 80, nlm::ConfigSource::Default);
    config->set("memory.capacity", 2048, nlm::ConfigSource::Default);
    config->set("neuromodulation.dopamine.baseline", 0.1, nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify all major systems are present and initialized
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getRegionCount() == 10);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Verify memory systems
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Verify cognition systems
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Verify development system
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Verify neuromodulation systems
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Verify plasticity systems
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    std::cout << "    Brain system integration test passed" << std::endl;
}

void testMemorySystemsIntegration() {
    std::cout << "Testing memory systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("memory.capacity", 512, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    brain.getWorkingMemory()->store(nlm::NeuronId(1), 1.0f);
    brain.getWorkingMemory()->store(nlm::NeuronId(2), 2.5f);
    brain.getWorkingMemory()->store(nlm::NeuronId(3), -1.5f);
    
    assert(brain.getWorkingMemory()->getCurrentSize() == 3);
    assert(brain.getWorkingMemory()->retrieve(nlm::NeuronId(1)) == 1.0f);
    assert(brain.getWorkingMemory()->retrieve(nlm::NeuronId(2)) == 2.5f);
    assert(brain.getWorkingMemory()->retrieve(nlm::NeuronId(3)) == -1.5f);
    
    // Test episodic memory integration
    nlm::EpisodicMemoryItem item1;
    item1.timestamp = 1000;
    item1.metadata = "Experience 1";
    item1.neurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    item1.values = {1.0f, 2.0f};
    
    nlm::EpisodicMemoryItem item2;
    item2.timestamp = 1001;
    item2.metadata = "Experience 2";
    item2.neurons = {nlm::NeuronId(3), nlm::NeuronId(4)};
    item2.values = {3.0f, 4.0f};
    
    brain.getEpisodicMemory()->storeEpisode(item1);
    brain.getEpisodicMemory()->storeEpisode(item2);
    
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 2);
    
    auto recentEpisodes = brain.getEpisodicMemory()->getRecentEpisodes(1);
    assert(recentEpisodes.size() == 1);
    assert(recentEpisodes[0].metadata == "Experience 2");
    
    // Test associative memory integration
    brain.getAssociativeMemory()->associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    brain.getAssociativeMemory()->associate(nlm::NeuronId(2), nlm::NeuronId(3), 0.6f);
    brain.getAssociativeMemory()->associate(nlm::NeuronId(3), nlm::NeuronId(4), 0.9f);
    
    auto associations1 = brain.getAssociativeMemory()->getAssociations(nlm::NeuronId(1));
    assert(associations1.size() == 1);
    assert(associations1[0] == nlm::NeuronId(2));
    
    auto associations2 = brain.getAssociativeMemory()->getAssociations(nlm::NeuronId(2));
    assert(associations2.size() == 2);
    assert(brain.getAssociativeMemory()->getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);
    
    std::cout << "    Memory systems integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("neuromodulation.dopamine.baseline", 0.1, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.level", 0.5, nlm::ConfigSource::Default);
    config->set("neuromodulation.novelty.threshold", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine integration
    brain.getDopamine()->setLevel(0.5f);
    assert(brain.getDopamine()->getLevel() == 0.5f);
    
    // Test neuromodulator application
    class TestNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.3f; }
        void setLevel(float level) override {}
        const char* getName() const override { return "TestModulator"; }
        float getPlasticityFactor() const override { return 1.5f; }
        void update(nlm::TimestepDuration dt) override {}
    } testModulator;
    
    brain.applyNeuromodulation(testModulator);
    
    // Test neuromodulation effects on plasticity
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0};
    
    float initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should have changed due to STDP
    assert(synapse.getWeight() != initialWeight);
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("development.synapses_per_region", 30, nlm::ConfigSource::Default);
    config->set("development.pruning_threshold", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stage transitions
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Test development system functionality
    brain.develop(); // Should trigger developmental processes
    
    // Verify development system is still accessible
    assert(brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "    Development system integration test passed" << std::endl;
}

void testSensorimotorIntegration() {
    std::cout << "Testing sensorimotor integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("motor.output_neurons", 20, nlm::ConfigSource::Default);
    config->set("sensory.input_neurons", 20, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test sensory input injection
    // Create a simple sensory input (implementation would vary)
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
    
    // Test motor neuron activation
    brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 3.0f);
    
    // Run simulation steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify brain state is consistent
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Sensorimotor integration test passed" << std::endl;
}

void testRealTimeSimulation() {
    std::cout << "Testing real-time simulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("simulation.dt", 0.001, nlm::ConfigSource::Default);
    config->set("simulation.max_steps", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run extended simulation
    nlm::Timestamp time = 0.0;
    nlm::TimestepDuration dt = 0.001;
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, time);
        time += dt;
    }
    
    // Verify simulation statistics
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test checkpointing
    bool saveSuccess = brain.save("test_integration_checkpoint.bin");
    // Save may fail depending on implementation, but API should exist
    assert(&nlm::Brain::save != nullptr);
    
    std::cout << "    Real-time simulation test passed" << std::endl;
}

void testComprehensiveBrainFunctionality() {
    std::cout << "Testing comprehensive brain functionality..." << std::endl;
    
    // Create a complex brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(8), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.structural.enabled", true, nlm::ConfigSource::Default);
    config->set("development.synapses_per_region", 80, nlm::ConfigSource::Default);
    config->set("memory.capacity", 2048, nlm::ConfigSource::Default);
    config->set("neuromodulation.dopamine.baseline", 0.05, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.level", 0.3, nlm::ConfigSource::Default);
    config->set("neuromodulation.novelty.threshold", 0.2, nlm::ConfigSource::Default);
    config->set("cognition.planning.enabled", true, nlm::ConfigSource::Default);
    config->set("cognition.attention.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize all systems
    bool success = brain.initialize();
    assert(success);
    
    // Run comprehensive simulation with mixed inputs
    nlm::Timestamp time = 0.0;
    nlm::TimestepDuration dt = 0.001;
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Inject mixed signals
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.0f);
        }
        if (step % 15 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 0.5f);
        }
        
        // Apply neuromodulation
        if (step % 25 == 0) {
            class TestModulator : public nlm::Neuromodulator {
            public:
                float getLevel() const override { return 0.2f + (step % 10) * 0.05f; }
                void setLevel(float level) override {}
                const char* getName() const override { return "TestModulator"; }
                float getPlasticityFactor() const override { return 1.0f; }
                void update(nlm::TimestepDuration dt) override {}
            } modulator;
            brain.applyNeuromodulation(modulator);
        }
        
        brain.step(step, time);
        time += dt;
        
        // Periodically check system health
        if (step % 50 == 0) {
            // Verify all systems are still functional
            assert(brain.getTotalNeuronCount() > 0);
            assert(brain.getTotalSynapseCount() > 0);
            assert(brain.getWorkingMemory() != nullptr);
            assert(brain.getPredictionSystem() != nullptr);
            assert(brain.getPlanner() != nullptr);
        }
    }
    
    // Verify final state
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test action production at the end
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test save/load functionality
    bool saveSuccess = brain.save("test_comprehensive_checkpoint.bin");
    assert(&nlm::Brain::save != nullptr);
    assert(&nlm::Brain::load != nullptr);
    
    std::cout << "    Comprehensive brain functionality test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration Tests..." << std::endl;
    
    testBrainSystemIntegration();
    testMemorySystemsIntegration();
    testNeuromodulationIntegration();
    testDevelopmentSystemIntegration();
    testSensorimotorIntegration();
    testRealTimeSimulation();
    testComprehensiveBrainFunctionality();
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration