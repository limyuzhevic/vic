// Memory System Tests - Modern Google Test Version
#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class MemoryTests : public NLMTestBase {
public:
    void TestWorkingMemoryCreation() {
        NeuralWorkingMemory mem;
        EXPECT_TRUE(mem.isEmpty());
        EXPECT_EQ(mem.getCapacity(), 100);
    }
    
    void TestWorkingMemoryStorage() {
        NeuralWorkingMemory mem;
        
        std::vector<float> data = {1.0f, 2.0f, 3.0f};
        mem.store(data);
        EXPECT_FALSE(mem.isEmpty());
        
        auto retrieved = mem.retrieve();
        EXPECT_EQ(retrieved.size(), 3);
        EXPECT_FLOAT_EQ(retrieved[0], 1.0f);
    }
    
    void TestEpisodicMemoryCreation() {
        NeuralEpisodicMemory mem;
        EXPECT_TRUE(mem.isEmpty());
        EXPECT_EQ(mem.getMaxEpisodes(), 1000);
    }
    
    void TestEpisodicMemoryStorage() {
        NeuralEpisodicMemory mem;
        
        Episode episode;
        episode.event = "test_event";
        episode.timestamp = 0.5;
        episode.context = std::vector<float>{0.1f, 0.2f, 0.3f};
        
        mem.store(episode);
        EXPECT_FALSE(mem.isEmpty());
        
        auto retrieved = mem.recall("test_event");
        EXPECT_EQ(retrieved.size(), 1);
        EXPECT_EQ(retrieved[0].event, "test_event");
        EXPECT_FLOAT_EQ(retrieved[0].timestamp, 0.5f);
    }
};

TEST_F(MemoryTests, TestWorkingMemoryCreation) { TestWorkingMemoryCreation(); }
TEST_F(MemoryTests, TestWorkingMemoryStorage) { TestWorkingMemoryStorage(); }
TEST_F(MemoryTests, TestEpisodicMemoryCreation) { TestEpisodicMemoryCreation(); }
TEST_F(MemoryTests, TestEpisodicMemoryStorage) { TestEpisodicMemoryStorage(); }

} // namespace test
} // namespace nlm
