#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Procedural memory: Neural learning of skills and habits
// IMPLEMENTATION - Phase 2 (real neural procedural memory)
class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn skill/pattern
    // Stored as neural pathways with associated neurons
    void learnSkill(const std::string& skillName, 
                    const std::vector<NeuronId>& neuralPattern,
                    float initialProficiency = 0.0f);
    
    // Retrieve skill
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        float lastUsed;
        float successCount;
        float failureCount;
        
        Skill() : proficiency(0.0f), lastUsed(0.0f), successCount(0.0f), failureCount(0.0f) {}
    };
    
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Execute skill (activate associated neural pattern)
    void executeSkill(const std::string& skillName, float strength = 1.0f);
    
    // Update proficiency based on performance
    void updateProficiency(const std::string& name, float delta);
    
    // Record skill execution outcome
    void recordExecutionOutcome(const std::string& skillName, bool success);
    
    // Get skills available for execution
    std::vector<std::string> getAvailableSkills() const;
    
    // Check if skill exists
    bool hasSkill(const std::string& skillName) const;
    
    // Clear all skills
    void clear();
    
    // Get overall procedural activity
    float getProceduralActivity() const;
    
    // Get skill proficiency
    float getSkillProficiency(const std::string& skillName) const;
    
    // Update procedural memory dynamics
    void update(TimestepDuration dt);
    
    // Apply neuromodulation effects on procedural learning
    void applyNeuromodulation(float dopamineLevel, float curiosityLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Create neural pathway for skill execution
    void createNeuralPathway(const std::vector<NeuronId>& pattern, float strength);
    
    // Consolidate skill (transfer to long-term memory)
    void consolidateSkill(size_t skillIndex);
    
    // Reinforce successful executions
    void reinforceExecution(const std::string& skillName, bool success);
};

} // namespace nlm