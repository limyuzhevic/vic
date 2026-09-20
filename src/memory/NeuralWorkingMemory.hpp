// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};

// NeuralEpisodicMemory: Storage of events and experiences
// Implements episode storage with replay capability

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);

    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;

    // Get episode count
    size_t getEpisodeCount() const;

    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;

    // Clear all
    void clear();

    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);

    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;

    // Replay episode
    void replayEpisode(const EpisodicMemoryItem* episode);

    // Get max episodes
    void setMaxEpisodes(size_t max) { maxEpisodes_ = max; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t maxEpisodes_;
    std::vector<EpisodicMemoryItem> episodes_;
};

// NeuralAssociativeMemory: Relationships between representations
// Implements Hebbian and activity-dependent associations

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association
    void associate(NeuronId a, NeuronId b, float strength);

    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;

    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;

    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);

    // Clear all
    void clear();

    // Get all associations
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& getAllAssociations() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

// NeuralEpisodicMemory for replay support
void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!brain_ || !episode) return;
    
    // Restore neural activity patterns from episode
    for (size_t i = 0; i < episode->neurons.size() && i < episode->values.size(); ++i) {
        NeuronId neuronId = episode->neurons[i];
        float activation = episode->values[i];
        
        // Find the neuron and set its membrane potential to reflect episode activity
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Set membrane potential proportional to activation
                        neuron->setMembranePotential(activation * neuron->getThreshold());
                        break;
                    }
                }
            }
        }
    }
}

// NeuralAssociativeMemory association helper
void NeuralAssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association already exists
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    associations_.emplace_back(a, b, strength);
}

const std::vector<std::tuple<NeuronId, NeuronId, float>>& NeuralAssociativeMemory::getAllAssociations() const {
    return associations_;
}

} // namespace nlm