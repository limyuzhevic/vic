#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>
#include <memory>

namespace nlm {

// ActionConsequencePredictor: Learns action -> consequence mappings
// Uses the neural substrate to store and retrieve action consequences
//
// Key principles:
// - Separates action prediction from general temporal prediction
// - Can be used independently of full NeuralPrediction system
// - Focuses specifically on action-consequence relationships
class ActionConsequencePredictor {
public:
    ActionConsequencePredictor();
    ~ActionConsequencePredictor();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record that taking an action in a state led to a specific consequence
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);

    // Predict consequence of taking an action in a given state
    std::vector<float> predictConsequence(ActionType action,
                                          const std::vector<float>& currentState);

    // Get prediction confidence for an action
    float getConsequenceConfidence(ActionType action,
                                  const std::vector<float>& currentState) const;

    // Update based on actual observed consequence
    void updatePrediction(ActionType action,
                         const std::vector<float>& predicted,
                         const std::vector<float>& actual,
                         float reward);

    // Get learned action quality (average expected reward)
    float getActionQuality(ActionType action) const;

    // Clear learned associations
    void clear();

    // Access to brain for neural integration
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }

private:
    // Find or create neuron for pattern representation
    NeuronId findOrCreatePatternNeuron(const std::vector<float>& pattern);

    // Compute similarity between patterns
    float computePatternSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> actionConsequences_;
    std::vector<std::tuple<ActionType, std::vector<float>, float>> actionQuality_;
};

} // namespace nlm
