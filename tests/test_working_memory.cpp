// tests/test_working_memory.cpp
#include "WorkingMemory.hpp"
#include <gtest/gtest.h>

class WorkingMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory = std::make_unique<WorkingMemory>();
    }
    
    std::unique_ptr<WorkingMemory> memory;
};

TEST_F(WorkingMemoryTest, InitialState) {
    EXPECT_EQ(memory->getCapacity(), 100);
    EXPECT_EQ(memory->getCurrentSize(), 0);
    EXPECT_FLOAT_EQ(memory->retrieve(1), 0.0f);
    EXPECT_FALSE(memory->contains(1));
}

TEST_F(WorkingMemoryTest, StoreAndRetrieve) {
    memory->store(1, 0.5f);
    EXPECT_TRUE(memory->contains(1));
    EXPECT_FLOAT_EQ(memory->retrieve(1), 0.5f);
}

TEST_F(WorkingMemoryTest, CapacityLimits) {
    // Test capacity limits by filling memory
    for (size_t i = 0; i < 150; ++i) {
        memory->store(i, 1.0f);
    }
    EXPECT_EQ(memory->getCurrentSize(), 100); // Should not exceed capacity
    EXPECT_TRUE(memory->contains(149)); // Most recent items should remain
    EXPECT_FALSE(memory->contains(0)); // Oldest items should be replaced
}

TEST_F(WorkingMemoryTest, WeakestItemReplacement) {
    // Fill with items having different values
    memory->store(1, 1.0f);
    memory->store(2, 0.5f);
    memory->store(3, 0.1f);
    
    EXPECT_EQ(memory->getCurrentSize(), 3);
    
    // Add another item - should replace the weakest (3, 0.1f)
    memory->store(4, 0.9f);
    
    EXPECT_FALSE(memory->contains(3)); // Should be replaced
    EXPECT_TRUE(memory->contains(4)); // New item should be there
    EXPECT_FLOAT_EQ(memory->retrieve(4), 0.9f);
}

TEST_F(WorkingMemoryTest, Decay) {
    memory->store(1, 1.0f);
    memory->store(2, 0.8f);
    
    memory->decay(0.1f); // 10% decay
    
    EXPECT_FLOAT_EQ(memory->retrieve(1), 0.9f);
    EXPECT_FLOAT_EQ(memory->retrieve(2), 0.72f);
}

TEST_F(WorkingMemoryTest, Overwrite) {
    memory->store(1, 0.5f);
    EXPECT_FLOAT_EQ(memory->retrieve(1), 0.5f);
    
    memory->store(1, 0.8f);
    EXPECT_FLOAT_EQ(memory->retrieve(1), 0.8f);
}