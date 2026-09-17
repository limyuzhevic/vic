#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm>

namespace nlm {

// NeuralProceduralMemory: Stores learned skills and behavioral patterns
// NOT a simple procedure list - patterns are encoded in synaptic weights and neural pathways
//
// Key mechanisms:
// - Skills encoded as neural activity patterns and recurrent circuits
// - Procedural learning through reinforcement and error signals
// - Pattern completion for skill retrieval
// - Adaptation through practice and feedback
// - Hierarchical organization of skills (simple → complex)

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    std::vector<float> patternActivations;
    float proficiency;
    SimulationStep lastPracticed;
    float successRate;
    
    Skill() : proficiency(0.0f), lastPracticed(0), successRate(0.0f) {}
};

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a new skill
    // The skill is encoded as neural patterns and connections
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);

    // Retrieve skill neural pattern
    // Uses pattern completion to retrieve the full skill pattern
    std::vector<float> retrieveSkill(const std::string& name) const;

    // Check if skill exists
    bool hasSkill(const std::string& name) const;

    // Get all skills
    std::vector<Skill> getAllSkills() const;

    // Update skill proficiency (practice)
    void practiceSkill(const std::string& name, bool success);

    // Get skill proficiency level
    float getSkillProficiency(const std::string& name) const;

    // Get skill neural pattern neurons
    std::vector<NeuronId> getSkillNeurons(const std::string& name) const;

    // Delete skill
    void deleteSkill(const std::string& name);

    // Get number of skills
    size_t getSkillCount() const { return skills_.size(); }

    // Get skill by name
    const Skill* getSkill(const std::string& name) const;

    // Update procedural memory (adaptation and consolidation)
    void update(TimestepDuration dt);

    // Consolidate skills (move to long-term storage)
    void consolidate();

    // Clear all skills
    void clear();

    // Get skills that have been practiced recently
    std::vector<std::string> getRecentlyPracticedSkills(size_t count) const;

    // Get skills with low proficiency that need practice
    std::vector<std::string> getSkillsNeedingPractice(float minProficiency) const;

    // Create recurrent connections for skill maintenance
    void createSkillRecurrentConnections();

    // Get average proficiency across all skills
    float getAverageProficiency() const;

private:
    // Create neural pattern for a skill
    void createSkillNeuralPattern(const std::string& name, const std::vector<NeuronId>& pattern);

    // Create recurrent connections for skill maintenance
    void createSkillRecurrentConnection(const std::string& name);

    // Decay unused skills
    void decayUnusedSkills();

    // Adapt skills based on practice outcomes
    void adaptSkillsBasedOnPractice();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;

    // Skills storage
    std::unordered_map<std::string, Skill> skills_;

    // Recurrent connections for skill maintenance
    std::vector<std::pair<NeuronId, NeuronId>> skillConnections_;

    // Practice history
    std::vector<std::pair<std::string, SimulationStep>> practiceHistory_;

    // Skill interference tracking
    std::vector<std::pair<std::string, std::string>> skillInterference_;
};

} // namespace nlm