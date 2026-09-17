// NeuralAssociativeMemory: Implements neural-based pattern associations and memory connections
// NOT a simple hash table - uses distributed neural patterns for robust association
//
// Key mechanisms:
// - Neural pattern encoding (patterns stored as distributed neuron groups)
// - Synaptic associations between patterns (strengthened via Hebbian learning)
// - Spreading activation for associative recall
// - Experience-based association formation

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

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;

    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};