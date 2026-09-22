#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// NeuralProceduralMemory: Skills and habits storage
// Represents learned behaviors, skills, and action sequences
class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn a new skill/pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve a skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update skill proficiency based on performance
    void updateProficiency(const std::string& name, float delta);
    
    // Execute a skill (generate output)
    std::vector<float> executeSkill(const std::string& name, const std::vector<float>& input);
    
    // Clear all skills
    void clear();
    
    // Get statistics
    size_t getSkillCount() const { return skills_.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Legacy interface for Brain.cpp
    friend class Brain;
    
    Brain* brain_;
    
    // Legacy data structures for compatibility
    std::vector<Skill> skills_;
    float skillLimit_;
};

} // namespace nlm
