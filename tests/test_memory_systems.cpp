#include "gtest/gtest.h"
#include "src/memory/WorkingMemory.hpp"
#include "src/memory/SemanticMemory.hpp"
#include "src/memory/ProceduralMemory.hpp"
#include "src/memory/AssociativeMemory.hpp"
#include "src/core/Types/Types.hpp"

// Mock brain for testing
class MockBrain {
public:
    MockBrain() : currentStep_(0) {}
    
    size_t getTotalNeuronCount() const { return 256; }
    size_t getFiringNeuronCount() const { return 128; }
    size_t getTotalSpikeCount() const { return 64; }
    float getAverageFiringRate() const { return 0.5f; }
    float getCurrentStep() const { return currentStep_; }
    SimulationStep getCurrentStepTyped() const { return SimulationStep(currentStep_); }
    
    void step(int step) { currentStep_ = step; }
    
    // Mock regions and populations for memory testing
    struct MockPopulation {
        bool containsNeuron(NeuronId neuron) const { return neuron < 100; }
        const std::vector<Neuron*>& getNeurons() const { return neurons; }
        std::vector<Neuron*> neurons;
    };
    
    struct MockRegion {
        const std::vector<MockPopulation*>& getPopulations() const { return populations; }
        std::vector<MockPopulation*> populations;
    };
    
    const std::vector<MockRegion*>& getRegions() const { return regions; }
    std::vector<MockRegion*> regions;
    
    SimulationClock* getSimulationClock() { return nullptr; }
    
private:
    SimulationStep currentStep_;
    std::vector<MockRegion*> regions;
};

// Mock neuron for testing
class MockNeuron {
public:
    MockNeuron(float potential = 0.0f, float resting = -70.0f) 
        : membranePotential_(potential), restingPotential_(resting) {}
    
    float getState() const { return membranePotential_; }
    float getStateMembranePotential() const { return membranePotential_; }
    float getStateRestingPotential() const { return restingPotential_; }
    
private:
    float membranePotential_;
    float restingPotential_;
};

TEST(WorkingMemoryTest, BasicStorageAndRetrieval) {
    WorkingMemory wm;
    
    // Test initial state
    EXPECT_EQ(wm.getCurrentSize(), 0);
    EXPECT_EQ(wm.getCapacity(), 100);
    
    // Store an item
    NeuronId neuron1 = NeuronId(1);
    wm.store(neuron1, 0.5f);
    
    // Retrieve the item
    EXPECT_EQ(wm.retrieve(neuron1), 0.5f);
    EXPECT_TRUE(wm.contains(neuron1));
    
    // Test size tracking
    EXPECT_EQ(wm.getCurrentSize(), 1);
    
    // Clear memory
    wm.clear();
    EXPECT_EQ(wm.getCurrentSize(), 0);
    EXPECT_FALSE(wm.contains(neuron1));
}

TEST(WorkingMemoryTest, CapacityManagement) {
    WorkingMemory wm;
    wm.setCapacity(5);
    EXPECT_EQ(wm.getCapacity(), 5);
    
    // Store more items than capacity
    for (int i = 0; i < 10; ++i) {
        wm.store(NeuronId(i), 1.0f);
    }
    
    // Memory should not exceed capacity
    EXPECT_LE(wm.getCurrentSize(), 5);
}

TEST(WorkingMemoryTest, DecayFunction) {
    WorkingMemory wm;
    NeuronId neuron1 = NeuronId(1);
    wm.store(neuron1, 1.0f);
    
    // Decay memory
    wm.decay(0.5f);
    EXPECT_LT(wm.retrieve(neuron1), 1.0f);
}

TEST(SemanticMemoryTest, BasicFactStorage) {
    SemanticMemory sm;
    
    // Store facts
    sm.storeFact("temperature", "warm");
    sm.storeFact("color", "red");
    sm.storeFact("size", "large");
    
    // Retrieve facts
    EXPECT_EQ(sm.retrieveFact("temperature"), "warm");
    EXPECT_EQ(sm.retrieveFact("color"), "red");
    EXPECT_EQ(sm.retrieveFact("size"), "large");
    
    // Check existence
    EXPECT_TRUE(sm.hasFact("temperature"));
    EXPECT_TRUE(sm.hasFact("color"));
    EXPECT_TRUE(sm.hasFact("size"));
    EXPECT_FALSE(sm.hasFact("nonexistent"));
    
    // Get all facts
    auto allFacts = sm.getAllFacts();
    EXPECT_EQ(allFacts.size(), 3);
    
    // Test update
    sm.updateFact("temperature", "hot", 0.5f);
    EXPECT_EQ(sm.retrieveFact("temperature"), "hot");
}

TEST(SemanticMemoryTest, SearchAndCompletion) {
    SemanticMemory sm;
    sm.storeFact("temperature", "warm");
    sm.storeFact("color", "red");
    sm.storeFact("weather", "sunny");
    sm.storeFact("size", "large");
    
    // Search functionality
    auto results = sm.search("temp");
    EXPECT_GE(results.size(), 1);
    
    auto results2 = sm.search("color");
    EXPECT_GE(results2.size(), 1);
    
    // Pattern completion
    std::string completion = sm.completePattern("temp");
    EXPECT_FALSE(completion.empty());
}

TEST(ProceduralMemoryTest, SkillLearning) {
    ProceduralMemory pm;
    
    // Create a mock neural pattern
    std::vector<NeuronId> pattern;
    for (int i = 0; i < 10; ++i) {
        pattern.push_back(NeuronId(i));
    }
    
    // Learn a skill
    pm.learnSkill("move_forward", pattern);
    
    // Retrieve skill
    Skill* skill = pm.getSkill("move_forward");
    ASSERT_NE(skill, nullptr);
    EXPECT_EQ(skill->name, "move_forward");
    EXPECT_EQ(skill->proficiency, 0.0f);
    EXPECT_EQ(skill->neuralPattern.size(), 10);
    
    // Get all skills
    auto allSkills = pm.getAllSkills();
    EXPECT_GE(allSkills.size(), 1);
    
    // Update proficiency
    pm.updateProficiency("move_forward", 0.5f);
    EXPECT_FLOAT_EQ(skill->proficiency, 0.5f);
}

TEST(ProceduralMemoryTest, SkillExecution) {
    ProceduralMemory pm;
    
    // Create and learn a skill
    std::vector<NeuronId> pattern;
    pattern.push_back(NeuronId(1));
    pattern.push_back(NeuronId(2));
    pm.learnSkill("test_skill", pattern);
    
    // Execute skill
    auto executedPattern = pm.executeSkill("test_skill", 1.0f);
    EXPECT_FALSE(executedPattern.empty());
}

TEST(AssociativeMemoryTest, PatternAssociation) {
    AssociativeMemory am;
    
    // Create test patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> pattern2 = {0.5f, 0.6f, 0.7f, 0.8f};
    
    // Create association
    am.associate(pattern1, pattern2, 0.8f);
    
    // Test retrieval
    auto retrieved = am.retrieve(pattern1, 5);
    EXPECT_FALSE(retrieved.empty());
    
    // Test association strength
    float strength = am.getAssociationStrength(pattern1, pattern2);
    EXPECT_FLOAT_EQ(strength, 0.8f);
    
    // Test spread activation
    auto spread = am.spreadActivation(pattern1, 2);
    EXPECT_FALSE(spread.empty());
}

TEST(AssociativeMemoryTest, ExperienceBasedAssociation) {
    AssociativeMemory am;
    
    // Create a mock episodic memory item
    EpisodicMemoryItem episode;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.action = ActionType::MoveForward;
    episode.resultingSensoryState = {0.4f, 0.5f, 0.6f};
    episode.reward = 1.0f;
    episode.energy = 0.8f;
    episode.novelty = 0.5f;
    episode.resultingReward = 0.9f;
    
    // Create association from experience
    am.associateFromExperience(episode);
    
    // Should have created some associations
    EXPECT_GT(am.getAssociationCount(), 0);
}

TEST(MemoryIntegrationTest, AllMemorySystems) {
    // Test that all memory systems can be initialized and work together
    MockBrain mockBrain;
    
    WorkingMemory wm;
    wm.initialize(&mockBrain);
    EXPECT_NO_THROW(wm.store(NeuronId(1), 0.5f));
    
    SemanticMemory sm;
    sm.initialize(&mockBrain);
    EXPECT_NO_THROW(sm.storeFact("test", "value"));
    
    ProceduralMemory pm;
    pm.initialize(&mockBrain);
    EXPECT_NO_THROW(pm.learnSkill("test", {NeuronId(1)}));
    
    AssociativeMemory am;
    am.initialize(&mockBrain);
    EXPECT_NO_THROW(am.associate({0.1f}, {0.2f}));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
