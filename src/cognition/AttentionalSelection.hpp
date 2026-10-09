#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// AttentionalSelection: Neural attention through competition and gain modulation
// NOT Transformer attention - uses lateral inhibition, recurrent amplification, and salience-based selection
//
// Key mechanisms:
// - Competitive neural dynamics for winner-take-all selection
// - Recurrent amplification of selected populations
// - Lateral inhibition for surround suppression
// - Salience-based bottom-up attention
// - Top-down goal-directed modulation
// - Dynamic attention switching based on task demands

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference and neural populations
    void initialize(Brain* brain);

    // Process competing neural populations and select attentional focus
    // Uses competitive dynamics with lateral inhibition and recurrent amplification
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                            float globalInhibition = 0.5f,
                                            float timeStep = 1.0f);

    // Apply sustained attention to a specific neural region
    void sustainAttention(RegionId region);

    // Release attention to allow fresh processing (reset competition dynamics)
    void releaseAttention();

    // Get current attentional focus (winners)
    std::vector<NeuronId> getAttentionalFocus() const;

    // Get currently attended regions
    std::vector<RegionId> getAttendedRegions() const;

    // Set attentional parameters
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }
    void setCompetitionThreshold(float threshold) { competitionThreshold_ = threshold; }
    void setAttentionDuration(float duration) { attentionDuration_ = duration; }

    // Get inhibition level for a neuron based on current competition state
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron based on attentional selection
    float getExcitationFor(NeuronId neuron) const;

    // Apply top-down attentional bias based on goals and expectations
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Apply bottom-up attentional bias based on stimulus salience
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Update attentional dynamics (called every simulation step)
    void update(float dt);

    // Check if a neuron is currently receiving attentional resources
    bool isAttended(NeuronId neuron) const;

    // Get attention winners (neurons with strongest recurrent amplification)
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Get overall attentional activation level
    float getAttentionLevel() const;

    // Reset attentional state
    void reset();

    // Get attentional bias for a specific neuron (for integration with other systems)
    float getAttentionalBias(NeuronId neuron) const;

    // Update attentional selection based on cognitive system outputs
    void updateFromCognitiveSystems(const NeuralPlanner* planner,
                                   const ConceptFormation* concepts,
                                   float dt);

    // Switch attention between competing representations
    void switchAttention(float noveltyThreshold = 0.5f);

    // Get attentional stability (how long focus has been maintained)
    float getAttentionalStability() const;

private:
    // Neural competition mechanisms
    void runCompetitiveDynamics(const std::vector<NeuronId>& competitors,
                               float globalInhibition,
                               float dt);

    // Recurrent amplification for selected winners
    void amplifyWinners(const std::vector<NeuronId>& winners, float dt);

    // Lateral inhibition to suppress non-winners
    void applyLateralInhibition(const std::vector<NeuronId>& competitors,
                                float dt);

    // Compute neural activity levels based on inputs and biases
    float computeNeuralActivity(NeuronId neuron, const std::vector<NeuronId>& competitors) const;

    // Normalize attentional weights to sum to 1
    void normalizeWeights(std::vector<float>& weights) const;

    // Find neuron with highest activity (winner)
    NeuronId findWinner(const std::vector<NeuronId>& competitors,
                        const std::vector<float>& activities) const;

    // Update salience and bias maps with decay
    void updateMaps(float dt);

    // Check for attention switching based on novelty or reward
    void checkAttentionSwitching(float dt);

    // Select attention based on task demands
    void selectBasedOnTask(float dt);

    // Integrate with working memory content
    void integrateWithWorkingMemory(float dt);

    // Apply attentional effects to neural populations
    void applyToNeuralPopulations(const std::vector<NeuronId>& winners, float dt);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    float attentionDuration_;

    // Attentional state
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> attentionalWeights_;
    std::vector<float> attentionalSalience_;

    // Bias maps for top-down and bottom-up influences
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;

    // Dynamic attentional parameters
    float currentAttentionLevel_;
    float attentionStability_;
    float attentionSwitchTimer_;
    SimulationStep attentionStartStep_;
    bool isSustainedAttention_;

    // Integration state
    bool integratedWithWorkingMemory_;
    std::vector<NeuronId> workingMemoryNeurons_;
};

} // namespace nlm
