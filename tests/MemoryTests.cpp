// MemoryTests.cpp - Comprehensive memory system tests
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace nlm {

// Memory system unit tests
class MemoryUnits {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Working memory tests
        TEST_CASE(WorkingMemoryTests, "Working Memory Initialization");
        TEST_CASE(WorkingMemoryTests, "Working Memory Store/Retrieve");
        TEST_CASE(WorkingMemoryTests, "Working Memory Capacity");
        TEST_CASE(WorkingMemoryTests, "Working Memory Decay");
        
        // Episodic memory tests
        TEST_CASE(EpisodicMemoryTests, "Episodic Memory Initialization");
        TEST_CASE(EpisodicMemoryTests, "Episodic Memory Storage");
        TEST_CASE(EpisodicMemoryTests, "Episodic Memory Retrieval");
        TEST_CASE(EpisodicMemoryTests, "Episodic Memory Consolidation");
        
        // Associative memory tests
        TEST_CASE(AssociativeMemoryTests, "Associative Memory Initialization");
        TEST_CASE(AssociativeMemoryTests, "Associative Memory Creation");
        TEST_CASE(AssociativeMemoryTests, "Associative Memory Strength");
        
        // Memory integration tests
        TEST_CASE(MemoryIntegrationTests, "Working Memory Integration");
        TEST_CASE(MemoryIntegrationTests, "Episodic Memory Integration");
        TEST_CASE(MemoryIntegrationTests, "Memory Replay System");
        
        std::cout << "=== Memory System Tests ===" << std::endl;
        runner.runAll();
    }
};

// Working memory tests
namespace WorkingMemoryTests {
    bool testInitialization() {
        WorkingMemory wm;
        ASSERT_EQ(wm.getCapacity(), 100);  // Default capacity
        ASSERT_EQ(wm.getCurrentSize(), 0);  // Initially empty
        std::cout << "PASS: Working memory initialization successful" << std::endl;
        return true;
    }
    
    bool testStoreRetrieve() {
        WorkingMemory wm;
        NeuronId neuron = 123;
        float value = 0.75f;
        
        wm.store(neuron, value);
        ASSERT_TRUE(wm.contains(neuron));
        
        float retrieved = wm.retrieve(neuron);
        ASSERT_FLOAT_EQ(retrieved, value, 0.001f);
        std::cout << "PASS: Working memory store/retrieve successful" << std::endl;
        return true;
    }
    
    bool testCapacity() {
        WorkingMemory wm;
        size_t capacity = wm.getCapacity();
        ASSERT_GT(capacity, 0);  // Should have some capacity
        std::cout << "PASS: Working memory capacity: " << capacity << std::endl;
        return true;
    }
    
    bool testDecay() {
        WorkingMemory wm;
        NeuronId neuron1 = 1;
        NeuronId neuron2 = 2;
        float value1 = 0.8f;
        float value2 = 0.6f;
        
        wm.store(neuron1, value1);
        wm.store(neuron2, value2);
        
        // Apply decay
        wm.decay(0.1f);  // 10% decay per timestep
        
        // Values should be reduced
        float retrieved1 = wm.retrieve(neuron1);
        float retrieved2 = wm.retrieve(neuron2);
        
        ASSERT_LT(retrieved1, value1);
        ASSERT_LT(retrieved2, value2);
        std::cout << "PASS: Working memory decay successful. "
                 << "Before: " << value1 << ", After: " << retrieved1 << std::endl;
        return true;
    }
}}

// Episodic memory tests
namespace EpisodicMemoryTests {
    bool testInitialization() {
        EpisodicMemory em;
        ASSERT_EQ(em.getEpisodeCount(), 0);  // Initially empty
        std::cout << "PASS: Episodic memory initialization successful" << std::endl;
        return true;
    }
    
    bool testStorage() {
        EpisodicMemory em;
        
        // Create a test episode
        EpisodicMemoryItem episode;
        episode.timestamp = 1000;
        episode.metadata = "Test episode";
        episode.conceptIds.push_back(1);
        episode.conceptIds.push_back(2);
        
        em.storeEpisode(episode);
        
        ASSERT_EQ(em.getEpisodeCount(), 1);
        
        // Retrieve the episode
        EpisodicMemoryItem retrieved = em.retrieveEpisode(0);
        ASSERT_EQ(retrieved.timestamp, 1000);
        ASSERT_EQ(retrieved.metadata, "Test episode");
        ASSERT_EQ(retrieved.conceptIds.size(), 2);
        
        std::cout << "PASS: Episodic memory storage/retrieval successful" << std::endl;
        return true;
    }
    
    bool testConsolidation() {
        EpisodicMemory em;
        
        // Create multiple episodes
        for (size_t i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i * 100;
            episode.metadata = "Episode " + std::to_string(i);
            episode.conceptIds.push_back(i % 3);  // Cycle through 3 concepts
            em.storeEpisode(episode);
        }
        
        ASSERT_EQ(em.getEpisodeCount(), 10);
        
        // Apply consolidation with threshold
        em.consolidate(0.5f);  // Consolidate episodes with relevance >= 0.5
        
        // Should still have all episodes (consolidation doesn't delete yet)
        ASSERT_EQ(em.getEpisodeCount(), 10);
        
        std::cout << "PASS: Episodic memory consolidation successful" << std::endl;
        return true;
    }
}}

// Associative memory tests
namespace AssociativeMemoryTests {
    bool testInitialization() {
        AssociativeMemory am;
        std::cout << "PASS: Associative memory initialization successful" << std::endl;
        return true;
    }
    
    bool testCreation() {
        AssociativeMemory am;
        NeuronId a = 1;
        NeuronId b = 2;
        float strength = 0.7f;
        
        am.associate(a, b, strength);
        
        std::vector<NeuronId> associations = am.getAssociations(a);
        ASSERT_TRUE(std::find(associations.begin(), associations.end(), b) != associations.end());
        
        float retrievedStrength = am.getAssociationStrength(a, b);
        ASSERT_FLOAT_EQ(retrievedStrength, strength, 0.001f);
        
        std::cout << "PASS: Associative memory creation successful" << std::endl;
        return true;
    }
    
    bool testStrength() {
        AssociativeMemory am;
        NeuronId a = 1;
        NeuronId b = 2;
        
        // Create initial association
        am.associate(a, b, 0.5f);
        
        // Update strength
        am.updateAssociation(a, b, 0.3f);
        
        float strength = am.getAssociationStrength(a, b);
        ASSERT_FLOAT_EQ(strength, 0.8f, 0.001f);  // 0.5 + 0.3
        
        std::cout << "PASS: Associative memory strength update successful" << std::endl;
        return true;
    }
}}

// Memory integration tests
namespace MemoryIntegrationTests {
    bool testWorkingMemoryIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Check if working memory is accessible
        auto* wm = brain->getWorkingMemory();
        ASSERT_TRUE(wm != nullptr);
        
        std::cout << "PASS: Working memory integration successful" << std::endl;
        return true;
    }
    
    bool testEpisodicMemoryIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Check if episodic memory is accessible
        auto* em = brain->getEpisodicMemory();
        ASSERT_TRUE(em != nullptr);
        
        std::cout << "PASS: Episodic memory integration successful" << std::endl;
        return true;
    }
    
    bool testReplaySystem() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Run some steps to generate episodes
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check if replay system is accessible
        auto* em = brain->getEpisodicMemory();
        if (em != nullptr) {
            size_t episodeCount = em->getEpisodeCount();
            std::cout << "PASS: Replay system accessible. "
                     << "Episodes stored: " << episodeCount << std::endl;
        } else {
            std::cout << "INFO: No episodic memory system available" << std::endl;
        }
        
        return true;
    }
}}

// Phase 6 memory integration test
namespace Phase6MemoryIntegration {
    bool testPhase6MemoryIntegration() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 1000;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Check memory integration
        bool memoryIntegrated = result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated;
        
        if (memoryIntegrated) {
            std::cout << "PASS: Phase 6 memory integration successful. "
                     << "WM: " << result.memoryWorkingMemoryIntegrated 
                     << ", EM: " << result.memoryEpisodicMemoryIntegrated << std::endl;
        } else {
            std::cout << "FAIL: Phase 6 memory integration failed. "
                     << "WM: " << result.memoryWorkingMemoryIntegrated 
                     << ", EM: " << result.memoryEpisodicMemoryIntegrated << std::endl;
            return false;
        }
        
        return true;
    }
}}

// Test registration functions
void register_memory_tests(TestRunner& runner) {
    TEST_CASE(MemoryUnits, "Working Memory Initialization");
    TEST_CASE(MemoryUnits, "Working Memory Store/Retrieve");
    TEST_CASE(MemoryUnits, "Working Memory Capacity");
    TEST_CASE(MemoryUnits, "Working Memory Decay");
    
    TEST_CASE(EpisodicMemoryTests, "Episodic Memory Initialization");
    TEST_CASE(EpisodicMemoryTests, "Episodic Memory Storage");
    TEST_CASE(EpisodicMemoryTests, "Episodic Memory Retrieval");
    TEST_CASE(EpisodicMemoryTests, "Episodic Memory Consolidation");
    
    TEST_CASE(AssociativeMemoryTests, "Associative Memory Initialization");
    TEST_CASE(AssociativeMemoryTests, "Associative Memory Creation");
    TEST_CASE(AssociativeMemoryTests, "Associative Memory Strength");
    
    TEST_CASE(MemoryIntegrationTests, "Working Memory Integration");
    TEST_CASE(MemoryIntegrationTests, "Episodic Memory Integration");
    TEST_CASE(MemoryIntegrationTests, "Memory Replay System");
    
    TEST_CASE(Phase6MemoryIntegration, "Phase 6 Memory Integration");
}

// Helper test functions
void setupMemoryTests() {
    std::cout << "Setting up memory system tests..." << std::endl;
}

void teardownMemoryTests() {
    std::cout << "Cleaning up memory system tests..." << std::endl;
}

} // namespace nlm
