#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralAssociativeMemory: Pattern association and value learning
// Integrates action-outcome pairs and value associations for pattern completion

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update associative memory with action-outcome pairs
    void update(TimestepDuration dt, const class Action& action, float outcome, float value);
    
    // Learn association between patterns
    void learnAssociation(const std::vector<NeuronId>& pattern1, const std::vector<NeuronId>& pattern2, 
                         float strength, float duration);
    
    // Get pattern completion for given input
    std::vector<NeuronId> completePattern(const std::vector<NeuronId>& inputPattern) const;
    
    // Get value associations for actions
    float getActionValue(const class Action& action) const;
    
    // Update action value based on outcome
    void updateActionValue(const class Action& action, float outcome, float learningRate);
    
    // Get associative memory strength
    float getAssociationStrength(const std::vector<NeuronId>& pattern1, 
                                const std::vector<NeuronId>& pattern2) const;
    
    // Clear all associations
    void clear();
    
    // Get active associations
    const std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>>& getAssociations() const;
    
    // Get statistics
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm