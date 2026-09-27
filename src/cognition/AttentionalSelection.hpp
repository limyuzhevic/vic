#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <set>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralPlanner;

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Core attention mechanisms
    void focusAttention(const std::vector<NeuronId>& neurons);
    void setGoal(const std::vector<NeuronId>& neurons);
    bool isGazingAt(const std::vector<NeuronId>& neurons) const;
    
    // Attention modulation
    void enhanceSignalToNoise(float amount);
    void reduceDistractibility(float amount);
    void adjustAttentionGain(float gain);
    
    // State management
    float getAttentionFocus() const;
    const std::vector<NeuronId>& getCurrentGoal() const;
    const std::set<NeuronId>& getFocusedNeurons() const;
    
    // Integration with other systems
    void updateWorkingMemorySelection(NeuralWorkingMemory* workingMemory);
    void updateEpisodicMemoryEncoding(NeuralEpisodicMemory* episodicMemory);
    void integrateWithNeuralPlanner(NeuralPlanner* planner);
    
    // Attention parameters
    void setAttentionSpan(float span);
    void setSelectionThreshold(float threshold);
    void setDistractibilityThreshold(float threshold);
    
    // Decay and reset
    void decayAttention(float dt);
    void reset();
    
    // Statistics
    size_t getSelectedTracesCount() const;
    size_t getTotalDistractibilityReduced() const;
    float getAverageAttentionFocus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
