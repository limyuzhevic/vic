#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn a skill (procedural knowledge)
    void learnSkill(const std::string& name, const std::vector<NeuronId>& neuralPattern);
    
    // Retrieve a skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update skill proficiency based on performance
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Update skill effectiveness based on execution
    void updateSkillExecution(const std::string& name, float effectiveness);
    
    // Get skills ordered by proficiency
    std::vector<Skill*> getSkillsOrderedByProficiency() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
