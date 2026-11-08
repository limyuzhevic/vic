namespace nlm {

// Brain Simulation - Simulation loop and step processing
class BrainSimulation {
public:
    BrainSimulation();
    ~BrainSimulation();
    
    void initialize(BrainCore* brain);
    void updateStep(size_t currentStep, double timestep);
    
    // Simulation control
    void setTimestep(double timestep) { timestep_ = timestep; }
    double getTimestep() const { return timestep_; }
    
    // Simulation state
    size_t getCurrentStep() const { return currentStep_; }
    double getCurrentTime() const { return currentTime_; }
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep_; }
    size_t getTotalSpikesTotal() const { return totalSpikesTotal_; }
    
    // Integration with other systems
    void setBrainReference(BrainCore* brain) { brain_ = brain; }
    void setMemoryReference(BrainMemory* memory) { memory_ = memory; }
    void setNeuromodulationReference(BrainNeuromodulation* neuromodulation) { neuromodulation_ = neuromodulation; }
    void setPlasticityReference(BrainPlasticity* plasticity) { plasticity_ = plasticity; }
    void setDevelopmentReference(BrainDevelopment* development) { development_ = development; }
    void setCognitionReference(BrainCognition* cognition) { cognition_ = cognition; }
    
    // Simulation phases (for debugging and analysis)
    enum class SimulationPhase {
        ProcessDelayedSpikes,
        UpdateNeurons,
        DetectSpikes,
        UpdateWorkingMemory,
        ApplyNeuromodulation,
        ApplyPlasticity,
        UpdateEpisodicMemory,
        UpdatePredictionSystem,
        UpdateAttention,
        UpdateConceptFormation,
        ApplyStructuralPlasticity,
        ReplayMemories,
        ApplyDevelopment,
        ConsolidateMemory,
        UpdateCheckpoints
    };
    
    // Simulation statistics
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BrainCore* brain_ = nullptr;
    BrainMemory* memory_ = nullptr;
    BrainNeuromodulation* neuromodulation_ = nullptr;
    BrainPlasticity* plasticity_ = nullptr;
    BrainDevelopment* development_ = nullptr;
    BrainCognition* cognition_ = nullptr;
    
    double timestep_ = 0.001;
    size_t currentStep_ = 0;
    double currentTime_ = 0.0;
    size_t totalSpikesThisStep_ = 0;
    size_t totalSpikesTotal_ = 0;
};

} // namespace nlm
