namespace nlm {

// Brain Cognition - Cognition systems integration
class BrainCognition {
public:
    BrainCognition();
    ~BrainCognition();
    
    void initialize(BrainCore* brain);
    void update(double timestep);
    
    // Cognition system accessors
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Cognitive processes
    void updatePlanning(const std::vector<float>& sensoryInput);
    void updateConceptFormation(const std::vector<float>& sensoryPattern, float reward);
    void updateAttention(const std::vector<NeuronId>& competitors);
    void updateSocialLearning(const MotorCommand& observedAction, const std::vector<float>& beforeVision, const std::vector<float>& afterVision);
    
    // Integration with other systems
    void setBrainReference(BrainCore* brain) { brain_ = brain; }
    void setMemoryReference(BrainMemory* memory) { memory_ = memory; }
    void setNeuromodulationReference(BrainNeuromodulation* neuromodulation) { neuromodulation_ = neuromodulation; }
    
    // Cognitive statistics
    size_t getPlannerDecisions() const;
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    size_t getSocialObservationCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BrainCore* brain_ = nullptr;
    BrainMemory* memory_ = nullptr;
    BrainNeuromodulation* neuromodulation_ = nullptr;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
};

} // namespace nlm
