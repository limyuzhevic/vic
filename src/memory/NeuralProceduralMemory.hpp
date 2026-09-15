#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>

namespace nlm {

// ProceduralMemory: Learned skills and habits as neural sequences
class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Learn a skill as a neural pattern sequence
    void learnSkill(const std::string& skillName, const std::vector<NeuronId>& sequence, float proficiency = 0.0f);
    
    // Execute skill (retrieve pattern)
    std::vector<NeuronId> executeSkill(const std::string& skillName) const;
    
    // Check if skill exists
    bool hasSkill(const std::string& skillName) const;
    
    // Get all skills
    std::vector<std::string> getAllSkills() const;
    
    // Get skill proficiency
    float getSkillProficiency(const std::string& skillName) const;
    
    // Update proficiency
    void updateSkillProficiency(const std::string& skillName, float delta);
    
    // Get skill neural pattern
    std::vector<NeuronId> getSkillPattern(const std::string& skillName) const;
    
    // Clear all
    void clear();
    
    // Initialize with brain
    void initialize(Brain* brain);
    
    // Update with time (for practice/forgetting)
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
