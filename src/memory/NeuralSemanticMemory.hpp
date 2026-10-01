#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Semantic memory: Gradual knowledge acquisition
// IMPLEMENTATION - Phase 2 (real neural semantic memory)
class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store concept/fact
    // Implemented as neural activation patterns
    void storeConcept(const std::string& concept, const std::vector<float>& pattern);
    
    // Retrieve concept
    std::vector<float> retrieveConcept(const std::string& concept) const;
    
    // Check if concept exists
    bool hasConcept(const std::string& concept) const;
    
    // Get all concepts
    std::vector<std::string> getAllConcepts() const;
    
    // Clear all concepts
    void clear();
    
    // Update concept strength based on reinforcement
    void reinforceConcept(const std::string& concept, float strength);
    
    // Get concept activation level
    float getConceptActivation(const std::string& concept) const;
    
    // Get overall semantic activity
    float getSemanticActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

// Procedural memory: Learned skills and habits
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
        
        Skill() : proficiency(0.0f), lastUsed(0.0f) {}
    };
    
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Execute skill (activate associated neural pattern)
    void executeSkill(const std::string& skillName, float strength = 1.0f);
    
    // Update proficiency based on performance
    void updateProficiency(const std::string& name, float delta);
    
    // Mark skill as used recently
    void markSkillUsed(const std::string& name, float timestamp);
    
    // Get skills available for execution
    std::vector<std::string> getAvailableSkills() const;
    
    // Clear all skills
    void clear();
    
    // Get overall procedural activity
    float getProceduralActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm