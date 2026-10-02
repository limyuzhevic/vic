#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// AttentionalSelection: Selects which sensory information gets processed
// Uses competitive dynamics and reward-based learning to focus attention
//
// Key mechanisms:
// - Competitive neural dynamics for winner-take-all selection
// - Reward-modulated plasticity to learn what to attend to
// - Gating of information flow based on attentional focus
// - Modulation of sensory processing gain

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process sensory signals through attentional filter
    // Returns selected focus (which neurons were attended to)
    std::vector<NeuronId> processSensoryInput(const std::vector<float>& sensorySignals,
                                            const std::vector<NeuronId>& availableNeurons);

    // Update attentional state based on reward prediction
    void updateBasedOnReward(float reward, const std::vector<NeuronId>& selectedNeurons);

    // Get current attentional focus
    std::vector<NeuronId> getCurrentFocus() const;

    // Get attentional strength for each neuron (gain modulation)
    float getAttentionGain(NeuronId neuron) const;

    // Set attentional parameters
    void setCompetitionStrength(float strength) { competitionStrength_ = strength; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setSelectionThreshold(float threshold) { selectionThreshold_ = threshold; }

    // Check if something deserves attention
    bool deservesAttention(float novelty, float urgency, float relevance) const;

    // Get attentional statistics
    float getAverageAttentionalFocus() const;
    size_t getActiveAttentionalNeurons() const;

    // Clear attentional state
    void clear();

    // Learn to attend to rewarding stimuli
    void learnFromReward(float reward, const std::vector<NeuronId>& neurons,
                        const std::vector<float>& attentionValues);

    // Update attention based on prediction error
    void updateBasedOnPredictionError(float error, const std::vector<NeuronId>& currentFocus);

    // Get neural correlates of attention
    std::vector<float> getNeuralAttentionPattern() const;

private:
    // Competitive selection: winner-take-all among competing neurons
    std::vector<NeuronId> competitiveSelection(const std::vector<float>& signals,
                                              const std::vector<NeuronId>& neurons);

    // Compute attentional gain based on attention history and reward
    float computeAttentionGain(NeuronId neuron, float recentReward,
                              const std::vector<float>& attentionHistory) const;

    // Normalize attentional weights
    void normalizeAttentionWeights();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Current attentional state
    std::vector<NeuronId> currentFocus_;
    std::unordered_map<NeuronId, float> attentionWeights_;
    std::unordered_map<NeuronId, std::vector<float>> attentionHistory_;

    // Parameters
    float competitionStrength_;
    float learningRate_;
    float selectionThreshold_;
    float noveltyWeight_;
    float urgencyWeight_;
    float relevanceWeight_;
};
