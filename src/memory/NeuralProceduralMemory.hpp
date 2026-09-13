#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Procedural memory: learned skills and habits
class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Learn skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    class Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<class Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
    // Initialize with brain reference
    void initialize(class Brain* brain) {}
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm