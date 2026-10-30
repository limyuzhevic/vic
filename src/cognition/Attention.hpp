#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>

namespace nlm {

// AttentionalSelection: Competitive selection of information for processing
// Implements selective attention through competitive dynamics
// NOT like Transformers - biological inspiration from basal ganglia and cortex
//
// Key mechanisms:
// - Competitive selection among active working memory traces
// - Inhibition of non-selected traces
// - Enhancement of selected traces
// - Priority-based selection based on reward prediction

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update attentional selection based on working memory competition
    void update(TimestepDuration dt);
    
    // Process competition among memory traces
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Get currently attended memory trace
    NeuronId getAttendedNeuron() const { return attendedNeuron_; }
    float getAttentionStrength() const { return attentionStrength_; }
    
    // Get attention state
    bool isAttending() const { return isAttending_; }
    float getInhibitionStrength() const { return inhibitionStrength_; }
    float getExcitationStrength() const { return excitationStrength_; }
    
    // Set attentional parameters
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }
    void setAttentionThreshold(float threshold) { attentionThreshold_ = threshold; }
    
    // Get attentional resources
    size_t getAttentionalResources() const { return attentionalResources_; }
    void setAttentionalResources(size_t resources) { attentionalResources_ = resources; }
    
    // Update attention based on neuromodulation
    void updateFromNeuromodulation(float novelty, float predictionError, float curiosity);
    
    // Reset attentional state
    void reset();
    
private:
    // Competition between memory traces
    void performCompetitiveSelection(const std::vector<NeuronId>& competitors);
    
    // Enhance attended trace
    void enhanceAttendedTrace(NeuronId neuronId, float strength);
    
    // Inhibit non-attended traces
    void inhibitNonAttendedTraces(const std::vector<NeuronId>& attended, const std::vector<NeuronId>& allNeurons);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Current attentional state
    NeuronId attendedNeuron_;
    float attentionStrength_;
    bool isAttending_;
    
    // Competitive dynamics parameters
    float inhibitionStrength_;
    float excitationStrength_;
    float attentionThreshold_;
    
    // Attentional resources
    size_t attentionalResources_;
    
    // Competition history
    std::deque<NeuronId> attentionHistory_;
    
    // Novelty-driven attention modulation
    float noveltyAttentionModulation_;
    float predictionErrorAttentionModulation_;
    float curiosityAttentionModulation_;
    
    // Stability and decay
    float attentionStability_;
    float attentionDecayRate_;
    
    // Priority scoring
    std::unordered_map<NeuronId, float> priorityScores_;
    
    // Reward prediction based attention
    float rewardPrediction_;
    float actionSelectionValue_;
    
    // Meta-attentional processes
    bool isMetaAttentionActive_;
    float metaAttentionWeight_;
    
    // Cortical-thalamic loop simulation
    float thalamicInputGain_;
    float corticalFeedbackStrength_;
};

} // namespace nlm
