#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// ProceduralMemory: Stores skills, habits, and behavioral patterns
// NOT just a list of skills - skills are encoded as neural pathways
//
// Key mechanisms:
// - Skills encoded as neural activation patterns and synaptic connections
// - Procedural learning through repetition and reinforcement
// - Skill transfer between similar tasks
// - Hierarchical skill organization (subskills -> complex skills)
// - Automaticity through repeated practice

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a new skill
    // Skill is a sequence of actions/neural patterns
    void learnSkill(const std::string& name,
                   const std::vector<NeuronId>& neuralPattern,
                   float initialProficiency = 1.0f);

    // Get skill by name
    Skill* getSkill(const std::string& name);

    // Get all skills
    std::vector<Skill*> getAllSkills();

    // Update skill proficiency
    void updateProficiency(const std::string& name, float delta);

    // Execute skill (retrieve neural pattern)
    std::vector<NeuronId> executeSkill(const std::string& name);

    // Check if skill exists
    bool hasSkill(const std::string& name) const;

    // Clear all skills
    void clear();

    // Get average proficiency across skills
    float getAverageProficiency() const;

    // Find similar skills (pattern similarity)
    std::vector<std::string> findSimilarSkills(const std::vector<NeuronId>& pattern,
                                               size_t maxResults = 5) const;

    // Transfer skill to similar task
    void transferSkill(const std::string& sourceSkill,
                      const std::string& targetSkill,
                      float transferStrength = 0.5f);

    // Get skills by proficiency level
    std::vector<std::string> getSkillsByProficiency(float minProficiency) const;

    // Practice skill to improve proficiency
    void practiceSkill(const std::string& name, float practiceQuality = 1.0f);

    // Get skill execution count
    size_t getSkillExecutions(const std::string& name) const;

private:
    // Skill execution statistics
    struct SkillStats {
        size_t executions;
        SimulationStep lastExecuted;
        float totalPracticeQuality;
        
        SkillStats() : executions(0), lastExecuted(0), totalPracticeQuality(0.0f) {}
    };

    // Neural pattern similarity computation
    float computePatternSimilarity(const std::vector<NeuronId>& patternA,
                                   const std::vector<NeuronId>& patternB) const;

    // Normalize neural pattern for comparison
    std::vector<float> normalizePattern(const std::vector<NeuronId>& pattern) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, Skill> skills_;
    std::unordered_map<std::string, SkillStats> skillStats_;
    size_t nextSkillId_;
};

} // namespace nlm
