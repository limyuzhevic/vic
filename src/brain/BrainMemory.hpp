namespace nlm {

// Brain Memory - Memory systems integration
class BrainMemory {
public:
    BrainMemory();
    ~BrainMemory();
    
    void initialize(BrainCore* brain);
    void update(size_t currentStep);
    
    // Working memory
    void storeToNeuron(NeuronId neuronId, float activation);
    bool isInWorkingMemory(NeuronId neuronId) const;
    float getWorkingMemoryActivation(NeuronId neuronId) const;
    std::vector<NeuronId> getWorkingMemoryNeurons() const;
    void updateWorkingMemory(double timestep);
    void setWorkingMemoryCapacity(size_t capacity);
    
    // Episodic memory
    void storeEpisode(const EpisodicMemoryItem& episode);
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    void setMaxEpisodes(size_t maxEpisodes);
    void consolidate(double threshold);
    void clearEpisodes();
    
    // Associative memory
    void storeAssociation(const NeuralPattern& pattern1, const NeuralPattern& pattern2, float strength);
    float recallAssociation(const NeuralPattern& pattern1, const NeuralPattern& pattern2) const;
    void updateAssociations(const NeuralPattern& currentPattern);
    void clearAssociations();
    
    // Memory statistics
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemorySize() const;
    size_t getAssociationCount() const;
    float getEpisodicReplayProbability() const;
    
    // Integration
    void setBrainReference(BrainCore* brain) { brain_ = brain; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BrainCore* brain_ = nullptr;
};

} // namespace nlm
