#include <gtest/gtest.h>
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../motor/MotorSystem.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../memory/Memory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        rng.seed(42);
    }
    
    nlm::RandomGenerator rng{42};
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::Dopamine> dopamine;
    std::unique_ptr<nlm::PredictionSystem> predictionSystem;
    std::unique_ptr<nlm::MotorSystem> motorSystem;
    std::unique_ptr<nlm::Synaptogenesis> synaptogenesis;
    std::unique_ptr<nlm::Maturation> maturation;
    std::unique_ptr<nlm::Pruning> pruning;
    std::unique_ptr<nlm::WorkingMemory> workingMemory;
    std::unique_ptr<nlm::EpisodicMemory> episodicMemory;
    std::unique_ptr<nlm::SemanticMemory> semanticMemory;
    std::unique_ptr<nlm::ProceduralMemory> proceduralMemory;
    std::unique_ptr<nlm::AssociativeMemory> associativeMemory;
};

TEST_F(IntegrationTest, AllSystemsInitialized) {
    // Create and initialize all systems
    dopamine = std::make_unique<nlm::Dopamine>();
    predictionSystem = std::make_unique<nlm::PredictionSystem>();
    motorSystem = std::make_unique<nlm::MotorSystem>();
    synaptogenesis = std::make_unique<nlm::Synaptogenesis>();
    maturation = std::make_unique<nlm::Maturation>();
    pruning = std::make_unique<nlm::Pruning>();
    workingMemory = std::make_unique<nlm::WorkingMemory>();
    episodicMemory = std::make_unique<nlm::EpisodicMemory>();
    semanticMemory = std::make_unique<nlm::SemanticMemory>();
    proceduralMemory = std::make_unique<nlm::ProceduralMemory>();
    associativeMemory = std::make_unique<nlm::AssociativeMemory>();
    
    // Test that all systems are functional
    EXPECT_NE(dopamine, nullptr);
    EXPECT_NE(predictionSystem, nullptr);
    EXPECT_NE(motorSystem, nullptr);
    EXPECT_NE(synaptogenesis, nullptr);
    EXPECT_NE(maturation, nullptr);
    EXPECT_NE(pruning, nullptr);
    EXPECT_NE(workingMemory, nullptr);
    EXPECT_NE(episodicMemory, nullptr);
    EXPECT_NE(semanticMemory, nullptr);
    EXPECT_NE(proceduralMemory, nullptr);
    EXPECT_NE(associativeMemory, nullptr);
}

TEST_F(IntegrationTest, NeuromodulationIntegration) {
    dopamine = std::make_unique<nlm::Dopamine>();
    
    // Test dopamine dynamics
    dopamine->setLevel(0.5f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.5f);
    
    float initialPlasticity = dopamine->getPlasticityFactor();
    dopamine->signalReward(1.0f);
    EXPECT_GE(dopamine->getLevel(), initialPlasticity);
}

TEST_F(IntegrationTest, PredictionAndMotorIntegration) {
    predictionSystem = std::make_unique<nlm::PredictionSystem>();
    motorSystem = std::make_unique<nlm::MotorSystem>();
    
    // Create dummy sensory inputs for testing
    nlm::Vision vision;
    std::vector<float> visionData = {1.0f, 2.0f, 3.0f};
    vision.setData(visionData);
    
    // Test prediction
    auto prediction = predictionSystem->predictNextState(vision);
    EXPECT_NE(prediction, nullptr);
    
    // Test action selection (with empty neural region)
    nlm::NeuralRegion emptyRegion;
    auto action = motorSystem->selectAction(emptyRegion);
    EXPECT_NE(action, nullptr);
}

TEST_F(IntegrationTest, DevelopmentIntegration) {
    synaptogenesis = std::make_unique<nlm::Synaptogenesis>();
    maturation = std::make_unique<nlm::Maturation>();
    pruning = std::make_unique<nlm::Pruning>();
    
    // Test development system parameters
    synaptogenesis->setFormationRate(0.001f);
    maturation->setProgress(0.5f);
    pruning->setThreshold(0.01f);
    
    EXPECT_FLOAT_EQ(synaptogenesis->getFormationRate(), 0.001f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 0.5f);
    EXPECT_FLOAT_EQ(pruning->getThreshold(), 0.01f);
}

TEST_F(IntegrationTest, MemoryIntegration) {
    workingMemory = std::make_unique<nlm::WorkingMemory>();
    episodicMemory = std::make_unique<nlm::EpisodicMemory>();
    semanticMemory = std::make_unique<nlm::SemanticMemory>();
    proceduralMemory = std::make_unique<nlm::ProceduralMemory>();
    associativeMemory = std::make_unique<nlm::AssociativeMemory>();
    
    // Test working memory
    workingMemory->store(1, 0.75f);
    EXPECT_TRUE(workingMemory->contains(1));
    EXPECT_FLOAT_EQ(workingMemory->retrieve(1), 0.75f);
    
    // Test episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.neurons.push_back(1);
    episode.values.push_back(0.5f);
    episodicMemory->storeEpisode(episode);
    EXPECT_EQ(episodicMemory->getEpisodeCount(), 1U);
    
    // Test semantic memory
    semanticMemory->storeFact("test", "value");
    EXPECT_TRUE(semanticMemory->hasFact("test"));
    EXPECT_EQ(semanticMemory->retrieveFact("test"), "value");
    
    // Test procedural memory
    proceduralMemory->learnSkill("testSkill", std::vector<nlm::NeuronId>{1, 2, 3});
    EXPECT_NE(proceduralMemory->getSkill("testSkill"), nullptr);
    
    // Test associative memory
    associativeMemory->associate(1, 2, 0.8f);
    auto associations = associativeMemory->getAssociations(1);
    EXPECT_GT(associations.size(), 0U);
}

TEST_F(IntegrationTest, SystemCooperation) {
    // Test that different systems can work together
    dopamine = std::make_unique<nlm::Dopamine>();
    predictionSystem = std::make_unique<nlm::PredictionSystem>();
    
    // Dopamine influences prediction system
    dopamine->setLevel(0.5f);
    float dopamineLevel = dopamine->getLevel();
    
    nlm::Vision vision;
    std::vector<float> visionData = {1.0f, 2.0f};
    vision.setData(visionData);
    
    auto prediction = predictionSystem->predictNextState(vision);
    EXPECT_NE(prediction, nullptr);
    
    // Check that prediction error is updated
    nlm::Vision actual;
    actual.setData(visionData);
    predictionSystem->updatePredictions(vision, actual);
    
    EXPECT_GT(predictionSystem->getPredictionError(), 0.0f);
}

TEST_F(IntegrationTest, CompleteSystemFlow) {
    // Simulate a complete system flow
    dopamine = std::make_unique<nlm::Dopamine>();
    workingMemory = std::make_unique<nlm::WorkingMemory>();
    episodicMemory = std::make_unique<nlm::EpisodicMemory>();
    
    // 1. Store experience in working memory
    workingMemory->store(1, 0.8f);
    
    // 2. Record as episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.neurons.push_back(1);
    episode.values.push_back(0.8f);
    episodicMemory->storeEpisode(episode);
    
    // 3. Apply neuromodulation to reinforce learning
    dopamine->signalReward(0.5f);
    
    // 4. Consolidate memory
    episodicMemory->consolidate(0.5f);
    
    // Verify system state
    EXPECT_TRUE(workingMemory->contains(1));
    EXPECT_EQ(episodicMemory->getEpisodeCount(), 1U);
    EXPECT_GT(dopamine->getLevel(), 0.0f);
}