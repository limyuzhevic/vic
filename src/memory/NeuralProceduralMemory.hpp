#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

// ProceduralMemory: Stores learned skills and habits
// NOT a skill database - procedural memory is embedded in neural connectivity
//
// Key mechanisms:
// - Skills stored as neural activation patterns
// - Proficiency affects strength of connections
// - Procedural memory interacts with working memory
// - Skills can be transferred to episodic memory

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn skill from neural pattern
    // Skills form through repeated experience
    void learnSkill(const std::string& name,
                   const std::vector<float>& pattern,
                   float initialProficiency = 0.5f);

    // Retrieve skill by name
    const std::vector<float>* getSkill(const std::string& name) const;

    // Execute skill given current state
    std::vector<float> executeSkill(const std::string& name,
                                   const std::vector<float>& inputState);

    // Get skill proficiency
    float getSkillProficiency(const std::string& name) const;

    // Update skill proficiency (practice effect)
    void updateProficiency(const std::string& name, float delta);

    // Get all learned skills
    std::vector<std::string> getAllSkills() const;

    // Get skill similarity to current state
    float getSkillSimilarity(const std::string& name,
                           const std::vector<float>& currentState) const;

    // Transfer skill to working memory for execution
    void prepareSkill(const std::string& name);

    // Get skills sorted by proficiency
    std::vector<std::pair<std::string, float>> getSkillsByProficiency() const;

    // Clear all skills
    void clear();

    // Export skills for persistence
    std::vector<std::pair<std::string, std::vector<float>>> exportSkills() const;

    // Import skills from previous session
    void importSkills(const std::vector<std::pair<std::string, std::vector<float>>>& skills);

private:
    // Find skill neuron or create one
    NeuronId findSkillNeuron(const std::vector<float>& pattern);

    // Compute procedural similarity between skills
    float computeSkillSimilarity(const std::vector<float>& a,
                               const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Procedural memory storage
    std::unordered_map<std::string, std::vector<float>> skills_;
    std::unordered_map<std::string, float> proficiency_;
    std::unordered_map<std::string, NeuronId> skillNeurons_;
};
