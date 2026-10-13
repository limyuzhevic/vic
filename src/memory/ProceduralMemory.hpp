#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Skill structure for procedural memory
struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn a skill with neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill by name
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update skill proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all skills
    void clear();
    
    // Get number of skills
    size_t getSkillCount() const { return skills_.size(); }
    
    // Find best matching skill for pattern
    Skill* findBestSkill(const std::vector<NeuronId>& pattern) const;
    
    // Execute skill (neural activation)
    void executeSkill(const std::string& name, class Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal storage
    std::unordered_map<std::string, Skill> skills_;
};

} // namespace nlm