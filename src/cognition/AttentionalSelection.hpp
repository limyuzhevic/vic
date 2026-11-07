#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

// Attentional selection system for competitive focus
// PLACEHOLDER - Phase 3 will implement real attentional selection
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize the attentional system
    bool initialize(class Brain* brain);
    
    // Process competition between selected items
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Get the currently selected item
    NeuronId getSelectedItem() const;
    
    // Get attention strength
    float getAttentionStrength() const;
    void setAttentionStrength(float strength);
    
    // Update attentional state
    void update(TimestepDuration dt);
    
    // Set inhibition/excitation balance
    void setInhibitionStrength(float strength);
    float getInhibitionStrength() const;
    void setExcitationStrength(float strength);
    float getExcitationStrength() const;
    
    // Clear current selection
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm