#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <memory>
#include <queue>

namespace nlm {

// NeuralProceduralMemory: Stores learned action sequences and skills using neural patterns
// NOT traditional symbolic procedures - neural pattern-based skill representation
//
// Key mechanisms:
// - Neural pattern storage for skills
// - Sequence learning and reinforcement
// - Developmental skill acquisition
// - Pattern similarity-based transfer learning

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn skill from neural pattern and action sequence
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern,
                   const std::vector<ActionType>& sequence,
                   float initialProficiency = 0.5f);

    // Get skill by name (returns pointer to internal skill - use with care)
    Skill* getSkill(const std::string& name);

    // Get all learned skills
    std::vector<Skill*> getAllSkills();

    // Update skill proficiency based on performance
    void updateProficiency(const std::string& name, float delta);

    // Update procedural memory with experienced actions and rewards
    void update(const std::vector<ActionType>& actions, float reward);

    // Decay skill weaknesses over time
    void decay(float decayRate);

    // Create procedural rules connecting skills
    void createProceduralRules(size_t skillIndex);

    // Convert neuron pattern to neural representation
    void convertNeuronPatternToNeuralPattern(const std::vector<NeuronId>& neuronPattern,
                                            std::vector<float>& neuralPattern);

    // Convert neural pattern back to neuron pattern (for demonstration)
    std::vector<NeuronId> convertNeuralPatternToNeuronPattern(const std::vector<float>& neuralPattern);

    // Find most similar skill sequence to actions
    size_t findMostSimilarSequence(const std::vector<ActionType>& actions) const;

    // Update neural representation based on proficiency change
    void updateSkillNeuralRepresentation(size_t skillIndex, float delta);

    // Update skill sequence based on performance
    void updateSkillSequenceNeuralRepresentation(size_t skillIndex, const std::vector<ActionType>& actions);

    // Calculate pattern similarity between neural patterns
    static float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                           const std::vector<float>& pattern2);

    // Calculate sequence similarity between action sequences
    static float calculateSequenceSimilarity(const std::vector<ActionType>& seq1,
                                            const std::vector<ActionType>& seq2);

    // Set base learning rate
    void setBaseLearningRate(float rate);

    // Set developmental multiplier
    void setDevelopmentalMultiplier(float multiplier);

    // Get base learning rate
    float getBaseLearningRate() const;

    // Get developmental multiplier
    float getDevelopmentalMultiplier() const;

    // Activate skill to increase proficiency
    void activateSkill(const std::string& name);

    // Get list of executable skills
    std::vector<std::string> getExecutableSkills() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
};

} // namespace nlm
