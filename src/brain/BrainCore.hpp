namespace nlm {

// Brain Core - Basic brain management and initialization
class BrainCore {
public:
    BrainCore(std::shared_ptr<Config> config);
    ~BrainCore();
    
    bool initialize();
    void reset();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Basic accessors
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getExcitationInhibitionRatio() const;
    std::unique_ptr<Action> produceAction();
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // State management
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // System access (for integration with other subsystems)
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    
    // Statistics and logging
    void logStatus() const;
    void save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Development
    void develop();
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    
    // Helper methods
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    Synapse* getSynapse(SynapseId id);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get most active neurons (for monitoring)
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
private:
    // Private implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
