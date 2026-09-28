// tests/test_procedural_memory.cpp
#include "ProceduralMemory.hpp"
#include <gtest/gtest.h>

class ProceduralMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory = std::make_unique<ProceduralMemory>();
    }
    
    std::unique_ptr<ProceduralMemory> memory;
};

TEST_F(ProceduralMemoryTest, InitialState) {
    EXPECT_EQ(memory->getAllSkills().size(), 0);
}

TEST_F(ProceduralMemoryTest, LearnSkill) {
    std::vector<NeuronId> pattern = {1, 2, 3, 4, 5};
    memory->learnSkill("walk", pattern);
    
    auto skills = memory->getAllSkills();
    ASSERT_EQ(skills.size(), 1);
    EXPECT_EQ(skills[0]->name, "walk");
    EXPECT_EQ(skills[0]->proficiency, 1.0f);
}

TEST_F(ProceduralMemoryTest, GetSkill) {
    std::vector<NeuronId> pattern = {1, 2, 3};
    memory->learnSkill("jump", pattern);
    
    Skill* skill = memory->getSkill("jump");
    ASSERT_NE(skill, nullptr);
    EXPECT_EQ(skill->name, "jump");
}

TEST_F(ProceduralMemoryTest, GetSkillNotFound) {
    EXPECT_EQ(memory->getSkill("nonexistent"), nullptr);
}

TEST_F(ProceduralMemoryTest, UpdateProficiency) {
    std::vector<NeuronId> pattern = {1, 2, 3};
    memory->learnSkill("run", pattern);
    
    memory->updateProficiency("run", 0.3f);
    
    auto skills = memory->getAllSkills();
    EXPECT_EQ(skills[0]->proficiency, 1.3f); // 1.0 + 0.3
}

TEST_F(ProceduralMemoryTest, ProficientLimit) {
    std::vector<NeuronId> pattern = {1, 2, 3};
    memory->learnSkill("swim", pattern);
    
    // Update to max proficiency
    memory->updateProficiency("swim", 1.0f);
    EXPECT_EQ(memory->getSkill("swim")->proficiency, 1.0f);
    
    // Try to update beyond max
    memory->updateProficiency("swim", 0.5f);
    EXPECT_EQ(memory->getSkill("swim")->proficiency, 1.0f);
}

TEST_F(ProceduralMemoryTest, Clear) {
    std::vector<NeuronId> pattern = {1, 2, 3};
    memory->learnSkill("test", pattern);
    
    memory->clear();
    EXPECT_EQ(memory->getAllSkills().size(), 0);
}