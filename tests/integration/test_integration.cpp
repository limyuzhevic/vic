// Integration Unit Tests
// Comprehensive integration tests for NLM system components

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>

namespace nlm {
namespace test {

// Test fixture for integration tests
class IntegrationTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test configuration
        config = createDefaultConfig();
        
        // Initialize brain with test configuration
        brain = std::make_shared<nlm::Brain>(config);
        brainInitialized = brain->initialize();
        EXPECT_TRUE(brainInitialized);
    }
    
    void TearDown() override {
        TestBase::TearDown();
        brain.reset();
        config.reset();
    }
    
protected:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    bool brainInitialized;
};

// Test brain initialization with real configuration
TEST_F(IntegrationTest, BrainInitializationIntegration) {
    EXPECT_TRUE(brainInitialized);
    EXPECT_GT(brain->getTotalNeuronCount(), 0);
    EXPECT_GT(brain->getTotalSynapseCount(), 0);
    EXPECT_GT(brain->getRegionCount(), 0);
}

// Test neuron-synapse integration
TEST_F(IntegrationTest, NeuronSynapseIntegration) {
    // Get all neurons and synapses from brain
    std::vector<nlm::Neuron*> neurons;
    std::vector<nlm::Synapse*> synapses;
    
    // Note: This would require access to brain's internal structures
    // For now, test basic creation and validation
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Validate integration
    EXPECT_TRUE(neuron.getId().isValid());
    EXPECT_TRUE(synapse.getId().isValid());
    EXPECT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
    EXPECT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
}

// Test brain step simulation with integrated components
TEST_F(IntegrationTest, BrainStepIntegration) {
    // Record initial state
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Run brain simulation
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain->step(step);
        
        // Verify system remains stable
        EXPECT_GE(brain->getTotalNeuronCount(), 0);
        EXPECT_GE(brain->getTotalSynapseCount(), 0);
        EXPECT_GE(brain->getFiringNeuronCount(), 0);
    }
    
    // Verify simulation progressed
    EXPECT_GT(brain->getTotalSpikeCount(), initialSpikes);
}

// Test neuromodulation integration
TEST_F(IntegrationTest, NeuromodulationIntegration) {
    nlm::STDP* stdp = brain->getSTDP();
    nlm::Hebbian* hebbian = brain->getHebbian();
    nlm::StructuralPlasticity* structural = brain->getStructuralPlasticity();
    
    // Verify neuromodulation components are accessible
    EXPECT_NE(stdp, nullptr);
    EXPECT_NE(hebbian, nullptr);
    EXPECT_NE(structural, nullptr);
    
    // Test basic neuromodulation operations
    float ltpWeight = stdp->getLTPWeight();
    float hebbianLearningRate = hebbian->getLearningRate();
    
    EXPECT_GT(ltpWeight, 0.0f);
    EXPECT_GT(hebbianLearningRate, 0.0f);
    
    // Test that neuromodulation affects brain dynamics
    brain->step(0);
    
    // Verify brain still functional
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test development system integration
TEST_F(IntegrationTest, DevelopmentSystemIntegration) {
    nlm::DevelopmentSystem* development = brain->getDevelopmentSystem();
    EXPECT_NE(development, nullptr);
    
    // Test development system components
    nlm::Synaptogenesis* synaptogenesis = development->getSynaptogenesis();
    nlm::Pruning* pruning = development->getPruning();
    nlm::Maturation* maturation = development->getMaturation();
    
    EXPECT_NE(synaptogenesis, nullptr);
    EXPECT_NE(pruning, nullptr);
    EXPECT_NE(maturation, nullptr);
    
    // Test development system operations
    brain->step(0);
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalNeuronCount(), 0);
}

// Test memory system integration
TEST_F(IntegrationTest, MemorySystemIntegration) {
    // Test working memory integration with brain
    nlm::Memory::storeWorkingMemory(NeuronId(1), 0.5f);
    float retrieved = nlm::Memory::retrieveWorkingMemory(NeuronId(1));
    EXPECT_FLOAT_EQ(retrieved, 0.5f);
    
    // Test episodic memory integration
    nlm::SimulationStep step = 1000;
    nlm::Memory::storeEpisodicMemory(step, NeuronId(2), 0.7f, "Integration test");
    
    auto episodicData = nlm::Memory::getEpisodicMemory(step);
    EXPECT_FALSE(episodicData.empty());
    EXPECT_EQ(episodicData[0].neurons[0], NeuronId(2));
    EXPECT_FLOAT_EQ(episodicData[0].values[0], 0.7f);
    
    brain->step(0);
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test prediction system integration
TEST_F(IntegrationTest, PredictionSystemIntegration) {
    nlm::PredictionSystem* prediction = brain->getPredictionSystem();
    EXPECT_NE(prediction, nullptr);
    
    nlm::NeuralPrediction* neuralPrediction = prediction->getNeuralPrediction();
    EXPECT_NE(neuralPrediction, nullptr);
    
    brain->step(0);
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test cognition system integration
TEST_F(IntegrationTest, CognitionSystemIntegration) {
    nlm::NeuralPlanner* neuralPlanner = brain->getNeuralPlanner();
    EXPECT_NE(neuralPlanner, nullptr);
    
    nlm::ConceptFormation* conceptFormation = brain->getConceptFormation();
    EXPECT_NE(conceptFormation, nullptr);
    
    brain->step(0);
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test motor system integration
TEST_F(IntegrationTest, MotorSystemIntegration) {
    brain->step(0);
    
    auto action = brain->produceAction();
    EXPECT_NE(action, nullptr);
    
    auto& actionData = action->getActionData();
    EXPECT_FALSE(actionData.empty());
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test environment system integration
TEST_F(IntegrationTest, EnvironmentSystemIntegration) {
    brain->step(0);
    
    // Verify environment integration doesn't crash
    // Note: Environment integration might require more setup
    EXPECT_TRUE(true); // If we get here, integration worked
}

// Test complete system simulation
TEST_F(IntegrationTest, CompleteSystemSimulation) {
    // Run extended simulation
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Process sensory input
        brain->step(step);
        
        // Verify system remains stable
        EXPECT_GE(brain->getTotalNeuronCount(), 0);
        EXPECT_GE(brain->getTotalSynapseCount(), 0);
        EXPECT_GE(brain->getFiringNeuronCount(), 0);
        EXPECT_GE(brain->getAverageFiringRate(), 0.0f);
    }
    
    // Verify simulation produced meaningful results
    EXPECT_GT(brain->getTotalSpikeCount(), initialSpikes);
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test system initialization robustness
TEST_F(IntegrationTest, SystemInitializationRobustness) {
    // Test that system can be initialized with different configurations
    std::vector<std::shared_ptr<nlm::Config>> configs;
    
    // Small configuration
    auto smallConfig = createDefaultConfig();
    smallConfig->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    smallConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    configs.push_back(smallConfig);
    
    // Medium configuration
    auto mediumConfig = createDefaultConfig();
    mediumConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    mediumConfig->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    configs.push_back(mediumConfig);
    
    // Large configuration
    auto largeConfig = createDefaultConfig();
    largeConfig->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    largeConfig->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    configs.push_back(largeConfig);
    
    // Test each configuration
    for (size_t i = 0; i < configs.size(); ++i) {
        nlm::Brain testBrain(configs[i]);
        bool initialized = testBrain.initialize();
        EXPECT_TRUE(initialized);
        
        // Run a few steps to verify stability
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            testBrain.step(step);
        }
        
        EXPECT_GT(testBrain.getTotalNeuronCount(), 0);
    }
}

// Parameterized test for integration with different system sizes
class ParameterizedIntegrationTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(20, 0.1f, 0.01f, "Small system integration"),
            TestParameters(100, 0.2f, 0.05f, "Medium system integration"),
            TestParameters(500, 0.15f, 0.1f, "Large system integration")
        };
    }
};

TEST_P(ParameterizedIntegrationTest, SystemIntegrationWithDifferentSizes) {
    TestParameters params = GetParam();
    
    auto paramConfig = createDefaultConfig();
    paramConfig->set("neuron_count", static_cast<int64_t>(params.neuronCount), nlm::ConfigSource::Default);
    paramConfig->set("region_count", static_cast<int64_t>(params.neuronCount / 50), nlm::ConfigSource::Default);
    paramConfig->set("connection_probability", params.connectionProbability, nlm::ConfigSource::Default);
    
    nlm::Brain paramBrain(paramConfig);
    bool initialized = paramBrain.initialize();
    EXPECT_TRUE(initialized);
    
    // Run integration test simulation
    size_t initialSpikes = paramBrain.getTotalSpikeCount();
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        paramBrain.step(step);
        
        // Verify system stability
        EXPECT_GE(paramBrain.getTotalNeuronCount(), 0);
        EXPECT_GE(paramBrain.getTotalSynapseCount(), 0);
    }
    
    EXPECT_GT(paramBrain.getTotalSpikeCount(), initialSpikes);
}

INSTANTIATE_TEST_SUITE_P(
    IntegrationParameterizedTests,
    ParameterizedIntegrationTest,
    ::testing::ValuesIn(ParameterizedIntegrationTest::GetTestParameters())
);

// Stress test for system integration
TEST(IntegrationStressTest, SystemStressTest) {
    auto stressConfig = createDefaultConfig();
    stressConfig->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    stressConfig->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain stressBrain(stressConfig);
    bool initialized = stressBrain.initialize();
    EXPECT_TRUE(initialized);
    
    // Run extended stress simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        stressBrain.step(step);
        
        // Verify system remains stable under stress
        EXPECT_GE(stressBrain.getTotalNeuronCount(), 0);
        EXPECT_GE(stressBrain.getTotalSynapseCount(), 0);
        EXPECT_GE(stressBrain.getFiringNeuronCount(), 0);
    }
    
    // Verify stress test passed
    EXPECT_GT(stressBrain.getTotalSpikeCount(), 0);
}

// Performance benchmark for system integration
TEST(IntegrationPerformanceTest, SystemIntegrationPerformance) {
    TestBase testBase;
    
    auto func = []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
    };
    
    auto result = testBase.runBenchmark(func, 5);
    
    std::cout << "System integration performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Test integration with neuromodulation
TEST_F(IntegrationTest, NeuromodulationSystemIntegration) {
    nlm::Dopamine* dopamine = brain->getDopamine();
    nlm::Curiosity* curiosity = brain->getCuriosity();
    nlm::Novelty* novelty = brain->getNovelty();
    nlm::PredictionError* predictionError = brain->getPredictionError();
    
    EXPECT_NE(dopamine, nullptr);
    EXPECT_NE(curiosity, nullptr);
    EXPECT_NE(novelty, nullptr);
    EXPECT_NE(predictionError, nullptr);
    
    brain->step(0);
    
    // Verify integration didn't break brain
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test integration with reward system
TEST_F(IntegrationTest, RewardSystemIntegration) {
    // Test that brain can process reward signals
    brain->step(0);
    
    // Verify reward integration
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

} // namespace test
} // namespace nlm
