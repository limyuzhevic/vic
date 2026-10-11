#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace nlm {

// Semantic memory: storage of facts, concepts, and knowledge
// Implements a key-value store for learned facts and associations

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a fact (key-value pair)
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve a fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all facts
    void clear();
    
    // Get fact count
    size_t getFactCount() const;
    
    // Update fact with new value
    void updateFact(const std::string& key, const std::string& newValue);
    
    // Remove fact
    void removeFact(const std::string& key);
    
    // Search facts containing keyword
    std::vector<std::pair<std::string, std::string>> search(const std::string& keyword) const;
    
    // Get facts modified since timestamp
    std::vector<std::pair<std::string, std::string>> getRecentFacts(size_t maxAge) const;
    
    // Get confidence in fact
    float getFactConfidence(const std::string& key) const;
    
    // Update fact confidence
    void updateFactConfidence(const std::string& key, float delta);
    
    // Get fact creation timestamp (simplified)
    SimulationStep getFactTimestamp(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

// Procedural memory: storage of skills and habits
// Implements learned behavioral patterns and skills

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn a skill (pattern of neurons)
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill by name
    const Skill* getSkill(const std::string& name) const;
    
    // Get all learned skills
    std::vector<Skill*> getAllSkills();
    
    // Execute skill (neural activation)
    bool executeSkill(const std::string& name);
    
    // Update skill proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Check if skill exists
    bool hasSkill(const std::string& name) const;
    
    // Clear all skills
    void clear();
    
    // Get skill count
    size_t getSkillCount() const;
    
    // Get most practiced skills
    std::vector<std::pair<std::string, float>> getMostPracticedSkills(size_t count) const;
    
    // Associate neurons to skill
    void associateNeuronsToSkill(const std::string& name, const std::vector<NeuronId>& neurons);
    
    // Get neurons associated with skill
    std::vector<NeuronId> getSkillNeurons(const std::string& name) const;
    
    // Skill state update
    void updateSkillState(float dt);
    
    // Get skill execution probability
    float getSkillExecutionProbability(const std::string& name) const;
    
    // Increase skill execution
    void recordSkillExecution(const std::string& name);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
