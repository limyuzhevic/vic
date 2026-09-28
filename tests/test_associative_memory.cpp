// tests/test_associative_memory.cpp
#include "AssociativeMemory.hpp"
#include <gtest/gtest.h>

class AssociativeMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory = std::make_unique<AssociativeMemory>();
    }
    
    std::unique_ptr<AssociativeMemory> memory;
};

TEST_F(AssociativeMemoryTest, InitialState) {
    EXPECT_EQ(memory->getAssociations(1).size(), 0);
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(1, 2), 0.0f);
}

TEST_F(AssociativeMemoryTest, CreateAssociation) {
    memory->associate(1, 2, 0.7f);
    
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(1, 2), 0.7f);
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(2, 1), 0.7f); // Should be symmetric
}

TEST_F(AssociativeMemoryTest, GetAssociations) {
    memory->associate(1, 2, 0.5f);
    memory->associate(1, 3, 0.8f);
    
    auto associations = memory->getAssociations(1);
    EXPECT_EQ(associations.size(), 2);
    EXPECT_TRUE((std::find(associations.begin(), associations.end(), 2) != associations.end()));
    EXPECT_TRUE((std::find(associations.begin(), associations.end(), 3) != associations.end()));
}

TEST_F(AssociativeMemoryTest, UpdateAssociation) {
    memory->associate(1, 2, 0.3f);
    memory->updateAssociation(1, 2, 0.2f);
    
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(1, 2), 0.5f);
    
    // Test clamping at 0.0f and 1.0f
    memory->updateAssociation(1, 2, 0.8f); // 0.5 + 0.8 = 1.3, should clamp to 1.0
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(1, 2), 1.0f);
    
    memory->updateAssociation(1, 2, -1.5f); // 1.0 - 1.5 = -0.5, should clamp to 0.0
    EXPECT_FLOAT_EQ(memory->getAssociationStrength(1, 2), 0.0f);
}

TEST_F(AssociativeMemoryTest, Clear) {
    memory->associate(1, 2, 0.5f);
    memory->associate(3, 4, 0.8f);
    
    EXPECT_EQ(memory->getAssociations(1).size(), 1);
    EXPECT_EQ(memory->getAssociations(3).size(), 1);
    
    memory->clear();
    EXPECT_EQ(memory->getAssociations(1).size(), 0);
    EXPECT_EQ(memory->getAssociations(3).size(), 0);
}