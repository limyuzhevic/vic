#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update attentional selection
    void update(TimestepDuration dt);
    
    // Process competition among working memory traces
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Get selected focus
    NeuronId getSelectedFocus() const { return selectedFocus_; }
    
    // Get attention strength
    float getAttentionStrength() const { return attentionStrength_; }
    void setAttentionStrength(float strength) { attentionStrength_ = strength; }
    
    // Get inhibition strength
    float getInhibitionStrength() const { return inhibitionStrength_; }
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    
    // Get excitation strength
    float getExcitationStrength() const { return excitationStrength_; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }
    
    // Set attention gain
    void setAttentionGain(float gain) { attentionGain_ = gain; }
    
    // Select action based on attentional state
    ActionType selectAction(ActionType defaultAction) const;
    
    // Get working memory winners
    const std::vector<NeuronId>& getWorkingMemoryWinners() const { return workingMemoryWinners_; }
    void setWorkingMemoryWinners(const std::vector<NeuronId>& winners) { workingMemoryWinners_ = winners; }
    
    // Reset attentional state
    void reset();
    
    // Update attentional state based on neuromodulation
    void updateWithNeuromodulation(float acetylcholineLevel, 
                                   float norepinephrineLevel, 
                                   float serotoninLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    NeuronId selectedFocus_;
    float attentionStrength_;
    float inhibitionStrength_;
    float excitationStrength_;
    float attentionGain_;
    std::vector<NeuronId> workingMemoryWinners_;
    std::vector<float> attentionWeights_;
    std::vector<float> competitionScores_;
    SimulationStep lastUpdateTime_;
};

} // namespace nlm
