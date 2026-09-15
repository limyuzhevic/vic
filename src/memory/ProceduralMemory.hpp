#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Procedural memory: learned skills and habits with proficiency and chaining
// Real implementation with skill learning, reinforcement, and context-dependent execution

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn skill with pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    const Skill* getSkill(const std::string& name) const;
    
    // Get all skills
    std::vector<const Skill*> getAllSkills() const;
    
    // Update proficiency based on outcome
    void updateProficiency(const std::string& name, float delta, bool success = true);
    
    // Execute skill if conditions are met
    bool executeSkill(const std::string& name, const std::vector<float>& context);
    
    // Clear all skills
    void clear();
    
    // Consolidate skills into automaticity
    void consolidate();
    
    // Get skills available for context
    std::vector<const Skill*> getAvailableSkills(const std::vector<float>& context) const;
    
    // Set skill context
    void setSkillContext(const std::string& skillName, const std::vector<NeuronId>& context);
    
    // Clear skill context
    void clearSkillContext(const std::string& skillName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm