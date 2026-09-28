// tests/test_semantic_memory.cpp
#include "SemanticMemory.hpp"
#include <gtest/gtest.h>

class SemanticMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory = std::make_unique<SemanticMemory>();
    }
    
    std::unique_ptr<SemanticMemory> memory;
};

TEST_F(SemanticMemoryTest, InitialState) {
    EXPECT_TRUE(memory->retrieveFact("test").empty());
    EXPECT_FALSE(memory->hasFact("test"));
}

TEST_F(SemanticMemoryTest, StoreAndRetrieve) {
    memory->storeFact("color", "red");
    EXPECT_TRUE(memory->hasFact("color"));
    EXPECT_EQ(memory->retrieveFact("color"), "red");
}

TEST_F(SemanticMemoryTest, Overwrite) {
    memory->storeFact("color", "red");
    memory->storeFact("color", "blue");
    EXPECT_EQ(memory->retrieveFact("color"), "blue");
}

TEST_F(SemanticMemoryTest, GetAllFacts) {
    memory->storeFact("key1", "value1");
    memory->storeFact("key2", "value2");
    
    auto allFacts = memory->getAllFacts();
    EXPECT_EQ(allFacts.size(), 2);
    
    bool found1 = false, found2 = false;
    for (const auto& fact : allFacts) {
        if (fact.first == "key1" && fact.second == "value1") found1 = true;
        if (fact.first == "key2" && fact.second == "value2") found2 = true;
    }
    EXPECT_TRUE(found1 && found2);
}

TEST_F(SemanticMemoryTest, Clear) {
    memory->storeFact("temp", "value");
    EXPECT_TRUE(memory->hasFact("temp"));
    
    memory->clear();
    EXPECT_FALSE(memory->hasFact("temp"));
    EXPECT_TRUE(memory->retrieveFact("temp").empty());
}