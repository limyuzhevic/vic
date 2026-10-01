#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <queue>

namespace nlm {

// AttentionalSelection: Competitive selection of neural representations
// NOT human attention - computational model of competitive selection
//
// Key mechanisms:
// - Competitive inhibition between neural populations
// - Salience weighting based on novelty/reward
// - Gating of information flow to working memory
// - Resource allocation across brain regions
// - Dynamic prioritization based on task demands

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competition among neural representations
    // Selects the most salient/active representation
    size_t processCompetition(const std::vector<NeuronId>& competitors,
                            const std::vector<float>& activationLevels);
    
    // Get the current attentional focus
    size_t getAttentionalFocus() const { return attentionalFocus_; }
    
    // Get attention strength (0-1)
    float getAttentionStrength() const { return attentionStrength_; }
    
    // Get inhibition strength (0-1, higher = more inhibition)
    float getInhibitionStrength() const { return inhibitionStrength_; }
    
    // Get excitation strength (0-1, higher = more excitation)
    float getExcitationStrength() const { return excitationStrength_; }
    
    // Apply attention modulation to working memory
    void applyToWorkingMemory(NeuralWorkingMemory* workingMemory);
    
    // Update attentional priority based on novelty and reward
    void updateAttentionalPriority(float novelty, float reward, SimulationStep currentTime);
    
    // Get all active attentional weights
    const std::vector<float>& getAttentionalWeights() const { return attentionalWeights_; }
    
    // Get attentional history for analysis
    const std::vector<size_t>& getAttentionalHistory() const { return attentionalHistory_; }
    
    // Set attentional parameters
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }
    void setAttentionDecayRate(float rate) { attentionDecayRate_ = rate; }
    
    // Clear attentional state
    void clear() {
        attentionalFocus_ = 0;
        attentionStrength_ = 0.0f;
        attentionalWeights_.assign(attentionalWeights_.size(), 0.0f);
        attentionalHistory_.clear();
        noveltyHistory_.clear();
        rewardHistory_.clear();
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Current attentional state
    size_t attentionalFocus_;
    float attentionStrength_;
    float inhibitionStrength_;
    float excitationStrength_;
    
    // Parameters
    float attentionDecayRate_;
    float noveltyBoostFactor_;
    float rewardBoostFactor_;
    
    // History for analysis
    std::vector<size_t> attentionalHistory_;
    std::vector<float> noveltyHistory_;
    std::vector<float> rewardHistory_;
    std::vector<SimulationStep> historySteps_;
};

} // namespace nlm