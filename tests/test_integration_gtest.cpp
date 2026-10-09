#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "development/DevelopmentSystem.hpp"
#include <memory>
#include <vector>
#include <cmath>

namespace test_integration {

class SystemIntegrationTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create all system components
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        
        brain = std::make_unique<nlm::Brain>(config);
        brain->initialize();
        
        dopamine = std::make_unique<nlm::Dopamine>();
        acetylcholine = std::make_unique<nlm::Acetylcholine>();
        
        workingMemory = std::make_unique<nlm::WorkingMemory>(config);
        episodicMemory = std::make_unique<nlm::EpisodicMemory>(config);
        
        stdp = std::make_unique<nlm::STDP>();
        hebbian = std::make_unique<nlm::HebbianRule>();
        
        developmentSystem = std::make_unique<nlm::DevelopmentSystem>();
    }

    void TearDown() override {
        developmentSystem.reset();
        hebbian.reset();
        stdp.reset();
        episodicMemory.reset();
        workingMemory.reset();
        acetylcholine.reset();
        dopamine.reset();
        brain.reset();
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::Dopamine> dopamine;
    std::unique_ptr<nlm::Acetylcholine> acetylcholine;
    std::unique_ptr<nlm::WorkingMemory> workingMemory;
    std::unique_ptr<nlm::EpisodicMemory> episodicMemory;
    std::unique_ptr<nlm::STDP> stdp;
    std::unique_ptr<nlm::HebbianRule> hebbian;
    std::unique_ptr<nlm::DevelopmentSystem> developmentSystem;
};

TEST_F(SystemIntegrationTestFixture, AllComponentsInitialization) {
    EXPECT_TRUE(brain->isInitialized());
    EXPECT_TRUE(dopamine->isInitialized());
    EXPECT_TRUE(acetylcholine->isInitialized());
    EXPECT_TRUE(workingMemory->isInitialized());
    EXPECT_TRUE(episodicMemory->isInitialized());
    EXPECT_TRUE(stdp->isEnabled());
    EXPECT_TRUE(hebbian->isEnabled());
}

TEST_F(SystemIntegrationTestFixture, BrainWithNeuromodulation) {
    nlm::Timestamp ts(1000.0f);
    dopamine->signalReward(0.8f);
    acetylcholine->signalAttentionShift(0.9f);
    
    EXPECT_TRUE(std::isfinite(dopamine->getLevel()));
    EXPECT_TRUE(std::isfinite(acetylcholine->getLevel()));
}

TEST_F(SystemIntegrationTestFixture, MemoryWithBrainInteraction) {
    workingMemory->store("test_event", 42.0f);
    
    nlm::Timestamp ts(1000.0f);
    episodicMemory->record("brain_event", "test_type", 0.5f, ts);
    
    EXPECT_TRUE(workingMemory->retrieve("test_event").has_value());
    EXPECT_TRUE(episodicMemory->retrieveEvent("brain_event").has_value());
}

TEST_F(SystemIntegrationTestFixture, PlasticityWithBrain) {
    auto* preNeuron = new nlm::Neuron(nlm::NeuronId(1));
    auto* postNeuron = new nlm::Neuron(nlm::NeuronId(2));
    
    preNeuron->setType(nlm::NeuronType::Excitatory);
    postNeuron->setType(nlm::NeuronType::Excitatory);
    
    auto synapse = std::make_unique<nlm::Synapse>(preNeuron, postNeuron, 0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {nlm::Timestamp(0.0f)};
    std::vector<nlm::Timestamp> postSpikes = {nlm::Timestamp(10.0f)};
    
    stdp->update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    EXPECT_TRUE(std::isfinite(synapse->getWeight()));
    
    delete preNeuron;
    delete postNeuron;
}

TEST_F(SystemIntegrationTestFixture, DevelopmentSystemIntegration) {
    EXPECT_TRUE(developmentSystem->getStage() != nlm::DevelopmentalStage::NotDefined);
    
    nlm::DevelopmentalStage initialStage = developmentSystem->getStage();
    developmentSystem->advanceStage();
    
    EXPECT_NE(initialStage, developmentSystem->getStage());
}

TEST_F(SystemIntegrationTestFixture, CompleteWorkflow) {
    // Simulate a complete neural system workflow
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step);
        dopamine->update(0.001f);
        acetylcholine->update(0.001f);
        workingMemory->update(0.001f);
        episodicMemory->update(0.001f);
        
        if (step % 5 == 0) {
            developmentSystem->advanceStage();
        }
    }
    
    EXPECT_TRUE(brain->isInitialized());
    EXPECT_TRUE(std::isfinite(dopamine->getLevel()));
    EXPECT_TRUE(std::isfinite(acetylcholine->getLevel()));
}

TEST_F(SystemIntegrationTestFixture, SystemStateConsistency) {
    float initialDA = dopamine->getLevel();
    float initialACh = acetylcholine->getLevel();
    
    // Apply various system changes
    dopamine->signalReward(0.7f);
    acetylcholine->signalAttentionShift(0.8f);
    workingMemory->store("consistency_test", 123.0f);
    
    float finalDA = dopamine->getLevel();
    float finalACh = acetylcholine->getLevel();
    
    EXPECT_TRUE(std::isfinite(finalDA));
    EXPECT_TRUE(std::isfinite(finalACh));
}

TEST_F(SystemIntegrationTestFixture, IntegrationWithPythonBindings) {
    // Test that C++ systems can be accessed from Python
    // This would be tested in Python tests
    EXPECT_TRUE(true);  // Placeholder
}

TEST_F(SystemIntegrationTestFixture, CrossSystemCompatibility) {
    // Test that different system components work together
    std::vector<float> features = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    workingMemory->store("features", 1.0f);
    episodicMemory->record("integration_test", "features", 0.5f, nlm::Timestamp(1000.0f));
    
    EXPECT_TRUE(workingMemory->retrieve("features").has_value());
    EXPECT_TRUE(episodicMemory->retrieveEvent("integration_test").has_value());
}

TEST_F(SystemIntegrationTestFixture, SystemLoadHandling) {
    // Test system under load
    for (int i = 0; i < 50; ++i) {
        brain->step(i % 100);
        dopamine->update(0.001f);
        acetylcholine->update(0.001f);
    }
    
    EXPECT_TRUE(brain->isInitialized());
    EXPECT_TRUE(std::isfinite(dopamine->getLevel()));
}

TEST_F(SystemIntegrationTestFixture, MemoryDecayIntegration) {
    workingMemory->store("decay_test", 100.0f);
    
    float initialValue = workingMemory->retrieve("decay_test").value();
    
    for (int i = 0; i < 100; ++i) {
        workingMemory->update(0.001f);
    }
    
    auto finalValue = workingMemory->retrieve("decay_test");
    if (finalValue.has_value()) {
        EXPECT_LT(finalValue.value(), initialValue);
    }
}

TEST_F(SystemIntegrationTestFixture, SystemReset) {
    // Test system reset functionality
    nlm::SimulationStep step = 10;
    brain->step(step);
    
    brain->reset();
    EXPECT_TRUE(brain->isInitialized());
}

TEST_F(SystemIntegrationTestFixture, DevelopmentalPlasticityCoupling) {
    // Test coupling between development and plasticity
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    
    stdp->setLearningRate(plasticityModifier * 0.01f);
    EXPECT_GT(stdp->getLearningRate(), 0.0f);
}

TEST_F(SystemIntegrationTestFixture, NeuromodulationAttention) {
    // Test neuromodulation's effect on attention
    acetylcholine->signalAttentionShift(0.8f);
    
    float attentionMod = acetylcholine->getAttentionModulation();
    EXPECT_TRUE(std::isfinite(attentionMod));
    EXPECT_GT(attentionMod, 0.0f);
}

TEST_F(SystemIntegrationTestFixture, MemoryEpisodicWorking) {
    // Test episodic and working memory interaction
    workingMemory->store("short_term", 1.0f);
    episodicMemory->record("long_term", "episodic", 0.9f, nlm::Timestamp(1000.0f));
    
    auto shortTerm = workingMemory->retrieve("short_term");
    auto longTerm = episodicMemory->retrieveEvent("long_term");
    
    EXPECT_TRUE(shortTerm.has_value());
    EXPECT_TRUE(longTerm.has_value());
}

TEST_F(SystemIntegrationTestFixture, SystemRobustness) {
    // Test system robustness to edge cases
    for (int i = 0; i < 20; ++i) {
        // Reset system state
        dopamine->reset();
        acetylcholine->reset();
        
        // Apply random operations
        float randomAction = static_cast<float>(rand()) / RAND_MAX;
        
        if (randomAction < 0.3f) {
            dopamine->signalReward(randomAction);
        } else if (randomAction < 0.6f) {
            acetylcholine->signalAttentionShift(randomAction);
        } else {
            workingMemory->store("random", randomAction);
        }
    }
    
    EXPECT_TRUE(true);  // Should not crash
}

} // namespace test_integration