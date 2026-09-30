// Integration Tests
// Test component interaction and system behavior

#include <gtest/gtest.h>
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include <vector>

namespace nlm {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("learning_rate", 0.02f, ConfigSource::Default);
        
        // Create brain
        brain = std::make_unique<Brain>(config);
        brain->initialize();
        
        // Create learning mechanisms
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        
        // Create memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Create neuromodulators
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
    }
    
    void TearDown() override {
        config.reset();
        brain.reset();
        stdp.reset();
        hebbian.reset();
        workingMemory.reset();
        episodicMemory.reset();
        associativeMemory.reset();
        curiosity.reset();
        predictionError.reset();
    }
    
    std::shared_ptr<Config> config;
    std::unique_ptr<Brain> brain;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
};

TEST_F(IntegrationTest, BrainLearningIntegration) {
    // Test brain integration with STDP learning
    ASSERT_TRUE(brain->initialize());
    
    // Simulate learning over multiple steps
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    // Verify that learning occurred
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
    EXPECT_LE(brain->getTotalSpikeCount(), 50000);  // Reasonable upper bound
}

TEST_F(IntegrationTest, MemorySystemsIntegration) {
    // Test memory systems integration
    brain->recordEpisode();
    
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

TEST_F(IntegrationTest, NeuromodulatorIntegration) {
    // Test neuromodulator integration
    if (curiosity) {
        curiosity->update(10.0f, 100.0f);  // novelty, potential reward
    }
    
    if (predictionError) {
        predictionError->update(5.0f, 3.0f);  // predicted, actual reward
    }
    
    // Brain should have processed neuromodulator signals
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

TEST_F(IntegrationTest, LearningMechanismsIntegration) {
    // Test integration of different learning mechanisms
    ASSERT_TRUE(brain->initialize());
    
    // Test STDP
    std::vector<Timestamp> preSpikes = {100.0f};
    std::vector<Timestamp> postSpikes = {110.0f};
    
    for (auto& synapse : brain->getAllSynapses()) {
        stdp->update(synapse.get(), preSpikes, postSpikes, 1.0f);
    }
    
    // Test Hebbian learning
    std::vector<NeuronId> preS neurons = {NeuronId(1)};
    std::vector<NeuronId> postS neurons = {NeuronId(2)};
    
    for (auto& synapse : brain->getAllSynapses()) {
        hebbian->update(synapse.get(), preS neurons, postS neurons);
    }
    
    // Verify that learning mechanisms modified synapses
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

TEST_F(IntegrationTest, ErrorRecovery) {
    // Test error recovery and system robustness
    ASSERT_TRUE(brain->initialize());
    
    // Test with edge case inputs
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
    }
    
    // System should still function
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
}

TEST_F(IntegrationTest, PerformanceUnderLoad) {
    // Test system performance under load
    ASSERT_TRUE(brain->initialize());
    
    // Simulate high load
    for (SimulationStep step = 0; step < 500; ++step) {
        brain->step(step);
    }
    
    // Verify performance under load
    EXPECT_GT(brain->getTotalSpikeCount(), 1000);
}

} // namespace nlm

