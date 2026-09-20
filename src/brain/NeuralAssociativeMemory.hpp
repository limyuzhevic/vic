#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// NeuralAssociativeMemory: Forms and uses associations between neural representations
// NOT just Hebbian learning - uses contextual modulation, prediction error, and higher-order associations
//
// Key mechanisms:
// - Context-aware association formation
// - Pattern completion using associations
// - Prediction error-modulated strengthening
// - Hierarchical (compound) associations

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Form association between two neural patterns in a specific context
    void associate(NeuronId a, NeuronId b, const std::vector<float>& context,
                  float strength = 1.0f, float predictionError = 0.0f);

    // Get associations for a neuron
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;

    // Get association strength with context
    float getAssociationStrength(NeuronId a, NeuronId b,
                                 const std::vector<float>& context) const;

    // Update association strength (learning)
    void updateAssociation(NeuronId a, NeuronId b, float delta,
                          const std::vector<float>& context);

    // Retrieve associated patterns with similarity search
    std::vector<NeuronId> retrieveSimilar(NeuronId seed,
                                        const std::vector<float>& context,
                                        size_t maxResults = 5) const;

    // Pattern completion - fill in missing patterns based on associations
    void patternCompletion(const std::vector<NeuronId>& partialPattern,
                          std::vector<NeuronId>& completedPattern) const;

    // Get all associations (for statistics)
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>>
        getAllAssociations() const;

    // Clear all associations
    void clear();

    // Update associations based on neural activity
    void update(Brain* brain, TimestepDuration dt);

    // Get number of active associations
    size_t getActiveAssociations() const { return activeAssociations_.size(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    
    // Strong associations (quick recall)
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> strongAssociations_;
    
    // Weak associations (slow recall)
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> weakAssociations_;
    
    // Active associations (recently used)
    std::unordered_set<std::tuple<NeuronId, NeuronId, std::vector<float>>> activeAssociations_;
    
    // Context modulation weights
    std::unordered_map<NeuronId, float> contextWeights_;
};

} // namespace nlm