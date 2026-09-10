#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Procedural Memory: Learned skills and habits
// NOT stored in a simple database - skills are encoded as neural pathways
//
// Key mechanisms:
// - Skills stored as neural patterns (recurrent connections)
// - Practice strengthens synaptic connections (long-term potentiation)
// - Proficiency develops through reinforcement learning
// - Skill transfer between related tasks

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    float lastPracticed;
    float successRate;
    int practiceCount;
    
    Skill() : proficiency(0.0f), lastPracticed(0), successRate(0.5f), practiceCount(0) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);

    // Execute skill
    bool executeSkill(const std::string& name, const std::vector<NeuronId>& inputPattern) const;

    // Get skill
    Skill* getSkill(const std::string& name);

    // Get all skills
    std::vector<Skill*> getAllSkills();

    // Get skills by proficiency range
    std::vector<Skill*> getSkillsByProficiency(float minProficiency, float maxProficiency) const;

    // Get all skills sorted by proficiency
    std::vector<Skill*> getAllSkillsSortedByProficiency() const;

    // Update proficiency
    void updateProficiency(const std::string& name, float delta);

    // Record successful execution
    void recordSuccess(const std::string& name, float score = 1.0f);

    // Practice skill
    void practiceSkill(const std::string& name, TimestepDuration dt);

    // Clear all skills
    void clear();

    // Get number of skills
    size_t getSkillCount() const { return skills_.size(); }

    // Get total proficiency
    float getTotalProficiency() const;

    // Get most practiced skill
    std::string getMostPracticedSkill() const;

    // Decay skills that aren't practiced
    void decayUnusedSkills(TimestepDuration dt);

private:
    // Create skill activation pattern
    std::vector<float> createSkillActivation(const Skill& skill, const std::vector<NeuronId>& inputPattern) const;

    // Find pattern similarity
    float computePatternSimilarity(const std::vector<NeuronId>& pattern1, const std::vector<NeuronId>& pattern2) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<Skill> skills_;
    float decayRate_;
};

} // namespace nlm