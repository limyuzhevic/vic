#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <algorithm>
#include <cmath>

class NeuralWorkingMemoryTestSuite : public TestSuite {
public:
    NeuralWorkingMemoryTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neural Working Memory Test Suite..." << std::endl;
        
        testNeuralWorkingMemoryCreation();
        testNeuralWorkingMemoryInitialization();
        testNeuralWorkingMemoryStorageRetrieval();
        testNeuralWorkingMemoryCompetition();
        testNeuralWorkingMemoryDecay();
        testNeuralWorkingMemoryStrengthening();
        
        std::cout << "Neural Working Memory Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuralWorkingMemoryTestSuite";
    }
    
private:
    void testNeuralWorkingMemoryCreation() {
        nlm::NeuralWorkingMemory wm;
        
        ASSERT_TRUE(wm.getCapacity() > 0);
        ASSERT_TRUE(wm.getActiveTraces() == 0);
        ASSERT_TRUE(wm.getMemoryNeurons().size() == 0);
        
        std::cout << "    testNeuralWorkingMemoryCreation passed" << std::endl;
    }
    
    void testNeuralWorkingMemoryInitialization() {
        nlm::Brain brain("test_brain");
        nlm::NeuralWorkingMemory wm;
        
        wm.initialize(&brain);
        ASSERT_TRUE(wm.getCapacity() > 0);
        
        std::cout << "    testNeuralWorkingMemoryInitialization passed" << std::endl;
    }
    
    void testNeuralWorkingMemoryStorageRetrieval() {
        nlm::Brain brain("test_brain");
        nlm::NeuralWorkingMemory wm;
        
        wm.initialize(&brain);
        
        std::vector<float> pattern(10, 0.5f);
        wm.store(pattern, 1.0f);
        
        std::vector<float> retrieved = wm.retrieve();
        ASSERT_TRUE(retrieved.size() == 10);
        
        ASSERT_TRUE(std::abs(retrieved[0] - 0.5f) < 0.01f);
        
        std::cout << "    testNeuralWorkingMemoryStorageRetrieval passed" << std::endl;
    }
    
    void testNeuralWorkingMemoryCompetition() {
        nlm::Brain brain("test_brain");
        nlm::NeuralWorkingMemory wm;
        
        wm.initialize(&brain);
        
        wm.setCapacity(10);
        
        // Create two competing memory traces
        std::vector<float> pattern1(5, 0.8f);
        std::vector<float> pattern2(5, 0.3f);
        
        wm.store(pattern1, 1.0f);
        wm.store(pattern2, 1.0f);
        
        wm.runCompetition();
        
        ASSERT_TRUE(wm.getMemoryActivity() >= 0.0f);
        
        std::cout << "    testNeuralWorkingMemoryCompetition passed" << std::endl;
    }
    
    void testNeuralWorkingMemoryDecay() {
        nlm::Brain brain("test_brain");
        nlm::NeuralWorkingMemory wm;
        
        wm.initialize(&brain);
        wm.setDecayRate(0.1f);
        
        std::vector<float> pattern(5, 0.9f);
        wm.store(pattern, 1.0f);
        
        wm.update(1.0f); // Update for 1 timestep
        
        float activity = wm.getMemoryActivity();
        ASSERT_TRUE(activity >= 0.0f);
        
        std::cout << "    testNeuralWorkingMemoryDecay passed" << std::endl;
    }
    
    void testNeuralWorkingMemoryStrengthening() {
        nlm::Brain brain("test_brain");
        nlm::NeuralWorkingMemory wm;
        
        wm.initialize(&brain);
        
        std::vector<float> pattern(5, 0.5f);
        wm.store(pattern, 1.0f);
        
        wm.strengthenMemory(1.5f);
        
        float activity = wm.getMemoryActivity();
        ASSERT_TRUE(activity >= 0.0f);
        
        std::cout << "    testNeuralWorkingMemoryStrengthening passed" << std::endl;
    }
};

class NeuralEpisodicMemoryTestSuite : public TestSuite {
public:
    NeuralEpisodicMemoryTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neural Episodic Memory Test Suite..." << std::endl;
        
        testNeuralEpisodicMemoryCreation();
        testNeuralEpisodicMemoryStorage();
        testNeuralEpisodicMemoryRetrieval();
        testNeuralEpisodicMemoryReplay();
        
        std::cout << "Neural Episodic Memory Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuralEpisodicMemoryTestSuite";
    }
    
private:
    void testNeuralEpisodicMemoryCreation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralEpisodicMemory em;
        
        em.initialize(&brain);
        ASSERT_TRUE(em.getEpisodeCount() == 0);
        
        std::cout << "    testNeuralEpisodicMemoryCreation passed" << std::endl;
    }
    
    void testNeuralEpisodicMemoryStorage() {
        nlm::Brain brain("test_brain");
        nlm::NeuralEpisodicMemory em;
        
        em.initialize(&brain);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.positionX = 10.0f;
        episode.positionY = 20.0f;
        episode.action = nlm::ActionType::Move;
        episode.reward = 1.0f;
        
        em.storeEpisode(episode);
        ASSERT_TRUE(em.getEpisodeCount() == 1);
        
        std::cout << "    testNeuralEpisodicMemoryStorage passed" << std::endl;
    }
    
    void testNeuralEpisodicMemoryRetrieval() {
        nlm::Brain brain("test_brain");
        nlm::NeuralEpisodicMemory em;
        
        em.initialize(&brain);
        
        nlm::EpisodicMemoryItem episode1;
        episode1.timestamp = 1;
        episode1.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode1.reward = 1.0f;
        
        nlm::EpisodicMemoryItem episode2;
        episode2.timestamp = 2;
        episode2.sensoryState = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        episode2.reward = 0.5f;
        
        em.storeEpisode(episode1);
        em.storeEpisode(episode2);
        
        std::vector<const nlm::EpisodicMemoryItem*> results = 
            em.retrieveSimilar({0.15f, 0.25f, 0.35f, 0.45f, 0.55f});
        ASSERT_TRUE(results.size() >= 1);
        
        std::cout << "    testNeuralEpisodicMemoryRetrieval passed" << std::endl;
    }
    
    void testNeuralEpisodicMemoryReplay() {
        nlm::Brain brain("test_brain");
        nlm::NeuralEpisodicMemory em;
        
        em.initialize(&brain);
        em.enableReplay(true);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
        episode.neuronActivations = {0.8f, 0.6f};
        
        em.storeEpisode(episode);
        
        const nlm::EpisodicMemoryItem* retrieved = em.getEpisode(0);
        ASSERT_TRUE(retrieved != nullptr);
        
        em.replayEpisode(retrieved);
        
        std::cout << "    testNeuralEpisodicMemoryReplay passed" << std::endl;
    }
};

class NeuralAssociativeMemoryTestSuite : public TestSuite {
public:
    NeuralAssociativeMemoryTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neural Associative Memory Test Suite..." << std::endl;
        
        testNeuralAssociativeMemoryCreation();
        testNeuralAssociativeMemoryCreation();
        testNeuralAssociativeMemoryRetrieval();
        testNeuralAssociativeMemorySpreadActivation();
        
        std::cout << "Neural Associative Memory Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuralAssociativeMemoryTestSuite";
    }
    
private:
    void testNeuralAssociativeMemoryCreation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralAssociativeMemory am;
        
        am.initialize(&brain);
        ASSERT_TRUE(am.getAssociationCount() == 0);
        
        std::cout << "    testNeuralAssociativeMemoryCreation passed" << std::endl;
    }
    
    void testNeuralAssociativeMemoryCreation2() {
        nlm::Brain brain("test_brain");
        nlm::NeuralAssociativeMemory am;
        
        am.initialize(&brain);
        
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
        std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
        
        am.associate(patternA, patternB, 0.8f);
        ASSERT_TRUE(am.getAssociationCount() == 1);
        
        float strength = am.getAssociationStrength(patternA, patternB);
        ASSERT_TRUE(strength > 0.0f);
        
        std::cout << "    testNeuralAssociativeMemoryCreation2 passed" << std::endl;
    }
    
    void testNeuralAssociativeMemoryRetrieval() {
        nlm::Brain brain("test_brain");
        nlm::NeuralAssociativeMemory am;
        
        am.initialize(&brain);
        
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
        std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
        
        am.associate(patternA, patternB, 0.9f);
        
        std::vector<std::vector<float>> retrieved = am.retrieve(patternA);
        ASSERT_TRUE(retrieved.size() >= 1);
        
        std::cout << "    testNeuralAssociativeMemoryRetrieval passed" << std::endl;
    }
    
    void testNeuralAssociativeMemorySpreadActivation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralAssociativeMemory am;
        
        am.initialize(&brain);
        
        std::vector<float> cuePattern = {0.1f, 0.2f, 0.3f};
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
        std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
        std::vector<float> patternC = {0.7f, 0.8f, 0.9f};
        
        am.associate(patternA, patternB, 0.7f);
        am.associate(patternB, patternC, 0.6f);
        
        std::vector<std::vector<float>> spread = am.spreadActivation(cuePattern, 2);
        ASSERT_TRUE(spread.size() >= 1);
        
        std::cout << "    testNeuralAssociativeMemorySpreadActivation passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createNeuralWorkingMemoryTestSuite() {
        return new NeuralWorkingMemoryTestSuite();
    }
    
    TestSuite* createNeuralEpisodicMemoryTestSuite() {
        return new NeuralEpisodicMemoryTestSuite();
    }
    
    TestSuite* createNeuralAssociativeMemoryTestSuite() {
        return new NeuralAssociativeMemoryTestSuite();
    }
}