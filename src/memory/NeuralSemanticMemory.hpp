#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Simple Semantic Memory: Stores facts and knowledge
// Simplified version for Phase 6 integration
class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Store fact with strength and expiration
    void storeFact(const std::string& key, const std::string& value, float strength = 1.0f, float decayRate = 0.001f);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Update fact strength
    void updateFactStrength(const std::string& key, float delta);
    
    // Decay all facts
    void decay(float decayRate);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simple Procedural Memory: Stores skills and habits
// Simplified version for Phase 6 integration  
class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Learn skill
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        float lastUsedTime;
        float decayRate;
        
        Skill() : proficiency(0.0f), lastUsedTime(0.0f), decayRate(0.0001f) {}
    };
    
    // Store skill
    void storeSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency = 0.0f);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency through practice
    void practiceSkill(const std::string& name, float improvementDelta);
    
    // Decay unused skills
    void decayUnused(float currentTime, float decayRate);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm