#ifndef TEST_MEMORY_SYSTEMS_HPP
#define TEST_MEMORY_SYSTEMS_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestMemorySystems : public TestCase {
public:
    TestMemorySystems() : TestCase("MemorySystems") {}
    
    void run() override {
        // Test NeuralWorkingMemory
        NeuralWorkingMemory::WorkingMemoryConfig wm_config;
        wm_config.capacity = 100.0;
        wm_config.num_items = 10;
        wm_config.decay_rate = 0.01;
        wm_config.competition_enabled = true;
        wm_config.encoding_strength = 0.5;
        wm_config.retrieval_threshold = 0.3;
        
        NeuralWorkingMemory wm(wm_config);
        
        // Test working memory encoding
        std::vector<double> pattern1 = {1.0, 0.5, 0.3, 0.1, 0.0};
        std::vector<double> pattern2 = {0.0, 1.0, 0.2, 0.4, 0.6};
        
        wm.encode(pattern1, 1.0);
        wm.encode(pattern2, 2.0);
        
        ASSERT_TRUE(wm.getNumActiveItems() > 0, "Working memory should have active items");
        
        // Test working memory retrieval
        std::vector<double> query_pattern = {0.9, 0.1, 0.2, 0.4, 0.5};
        double retrieved_item_id = wm.retrieve(query_pattern);
        ASSERT_TRUE(retrieved_item_id >= 0.0, "Should retrieve some item or none");
        
        // Test working memory decay
        double strength_before = wm.getItemStrength(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        wm.step();
        double strength_after = wm.getItemStrength(1);
        ASSERT_TRUE(strength_after <= strength_before, "Item strength should decay over time");
        
        // Test NeuralEpisodicMemory
        NeuralEpisodicMemory::EpisodicMemoryConfig em_config;
        em_config.max_episodes = 1000;
        em_config.encoding_rate = 0.1;
        em_config.consolidation_interval = 100;
        em_config.replay_enabled = true;
        em_config.replay_strength = 0.5;
        
        NeuralEpisodicMemory em(em_config);
        
        // Test episodic memory encoding
        Episode episode;
        episode.id = 1;
        episode.time_step = 0.0;
        episode.pattern = {1.0, 0.5, 0.3, 0.1, 0.0};
        episode.relevance = 0.8;
        episode.metadata = {"source": "test"};
        
        em.encode(episode);
        ASSERT_TRUE(em.getNumEpisodes() > 0, "Episodic memory should store episodes");
        
        // Test episodic memory retrieval
        std::vector<double> query_pattern = {0.9, 0.4, 0.2, 0.3, 0.1};
        std::vector<Episode> retrieved_episodes = em.retrieve(query_pattern, 5);
        ASSERT_TRUE(retrieved_episodes.size() <= 5, "Should retrieve at most 5 episodes");
        
        // Test episodic memory replay
        em.replay();
        
        // Test episodic memory consolidation
        em.consolidate();
        
        // Test memory system integration
        NeuralWorkingMemory wm2(wm_config);
        NeuralEpisodicMemory em2(em_config);
        
        // Encode to working memory first
        wm2.encode(pattern1, 1.0);
        
        // Then consolidate to episodic memory
        em2.consolidateFromWorkingMemory(wm2);
        
        ASSERT_TRUE(em2.getNumEpisodes() > 0, "Consolidation should transfer to episodic memory");
    }
};

#endif // TEST_MEMORY_SYSTEMS_HPP