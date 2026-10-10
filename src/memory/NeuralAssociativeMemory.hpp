#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// AssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall
//
// Associative Memory Architecture:
// ================================
// NeuralAssociativeMemory implements pattern completion and recall through
// neural association mechanisms. Unlike traditional associative memory systems
// that store patterns as vectors, NLM encodes associations directly in synaptic
// connectivity, making it biologically plausible and scalable.
//
// Core Principles:
// 1. **Neurally-Grounded**: Associations are formed between neural activity patterns
// 2. **Hebbian Learning**: Co-activation strengthens connections (neurons that fire together, wire together)
// 3. **Pattern Completion**: Partial activation can retrieve complete associated patterns
// 4. **Spreading Activation**: Activation propagates through association chains
// 5. **Memory Consolidation**: Repeated associations become more stable
//
// Biological Inspiration:
// ---------------------
// - Hippocampal-cortical interactions for memory formation
// - Neocortical associations for schema and concept formation
// - Hebbian plasticity for synaptic strengthening
// - Pattern completion in cortical networks
//
// Memory Encoding:
// ---------------
// During experience execution, when patterns of neural activity co-activate,
// the brain forms associations between them. These associations are:
//
// 1. **Temporary**: Initially weak, strengthen with repetition
// 2. **Context-Dependent**: Associations include timing and context information
// 3. **Reciprocal**: Bidirectional associations (A→B and B→A)
// 4. **Threshold-Based**: Only form if activation exceeds threshold
//
// Memory Retrieval:
// ----------------
// Given a cue pattern (partial activation), the system can:
//
// 1. **Direct Retrieval**: Find patterns strongly associated with the cue
// 2. **Pattern Completion**: Use spreading activation to find related patterns
// 3. **Context Retrieval**: Retrieve the context surrounding the cue
// 4. **Sequence Recall**: Retrieve sequences of associated patterns
//
// Usage Examples:
// ---------------
// Example 1: Basic association formation
// std::vector<float> pattern1 = {1.0f, 0.5f, 0.0f};
// std::vector<float> pattern2 = {0.5f, 1.0f, 0.5f};
// associativeMemory.associate(pattern1, pattern2, 0.8f);
//
// Example 2: Retrieval with partial cue
// std::vector<float> cue = {0.9f, 0.1f, 0.0f};
// auto results = associativeMemory.retrieve(cue, 3);
// for (const auto& pattern : results) {
//     // Process retrieved pattern
// }
//
// Example 3: Spreading activation
// std::vector<float> seed = {1.0f, 0.0f, 0.0f};
// auto extended = associativeMemory.spreadActivation(seed, 2);
// // Extended includes direct and indirect associations
//
// Performance Characteristics:
// -----------------------------
// - **Scalability**: Efficient for thousands to millions of associations
// - **Retrieval Speed**: O(k) for direct retrieval, O(k²) for spreading activation
// - **Memory Efficiency**: Associations stored in synaptic weights, not separate memory arrays
// - **Parallelization**: Independent associations can be processed in parallel
//
// Limitations and Considerations:
// ------------------------------
// 1. **Precision**: Neural patterns are approximate, not exact
// 2. **Capacity**: Limited by synaptic resources, not just storage
// 3. ** Interference**: Similar patterns may interfere with each other
// 4. **Noise**: Neural activity is inherently noisy
// 5. **Decay**: Associations weaken without reinforcement
//
// Integration with Other Systems:
// ------------------------------
// - **EpisodicMemory**: Source of pattern associations
// - **ConceptFormation**: Forms higher-level associations between concepts
// - **PredictionSystem**: Predicts future pattern activations
// - **Neuromodulation**: Reward signals strengthen important associations
// - **Plasticity**: STDP and Hebbian mechanisms update association strengths
//
// Configuration Parameters:
// -------------------------
// - **AssociationStrength**: Initial strength of new associations
// - **SpreadSteps**: Number of steps for spreading activation
// - **Threshold**: Minimum similarity for considering patterns related
// - **DecayRate**: How quickly associations weaken without reinforcement
// - **MaxAssociations**: Maximum number of associations to maintain

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience
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

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm