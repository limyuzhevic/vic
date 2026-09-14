#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// NeuralAssociativeMemory: Forms and retrieves relationships between neural patterns
// NOT a lookup table - associations are encoded in neural connectivity
//
// Key mechanisms:
// - Co-activating patterns form synaptic-like associations
// - Association strength strengthens with repeated co-activation (Hebbian-like)
// - Can retrieve related patterns given a partial cue
// - Spreading activation across associative chains
// - Neural pattern completion for partial inputs

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity (vectors of neuron activations)
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience (sensory -> action outcome)
    void associateFromExperience(const EpisodicMemoryItem& episode);

    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;

    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;

    // Clear associations
    void clear();

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

    // Process current experience - form associations between sensory patterns
    void processExperience(const std::vector<float>& sensoryPattern,
                          const std::vector<float>& resultingPattern,
                          float reward);

    // Get most active patterns (for concept formation integration)
    std::vector<std::vector<float>> getActivePatterns() const;

    // Apply neuromodulation to associations
    void applyNeuromodulation(float level);

private:
    // Find or create pattern neuron for a neural activity pattern
    NeuronId findOrCreatePatternNeuron(const std::vector<float>& pattern);

    // Compute similarity between two neural activity patterns
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Neural parameters
    float associationDecayRate_;
    float learningRate_;
    float minActivationThreshold_;
};

} // namespace nlm
