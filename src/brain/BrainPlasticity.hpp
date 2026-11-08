namespace nlm {

// Brain Plasticity - Plasticity systems integration (STDP, Hebbian, structural)
class BrainPlasticity {
public:
    BrainPlasticity();
    ~BrainPlasticity();
    
    void initialize(BrainCore* brain);
    void update(size_t currentStep, double timestep);
    
    // Plasticity system accessors
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Plasticity control
    void enableSTDP(bool enable);
    void enableHebbian(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableRewardModulation(bool enable);
    
    // Update plasticity rules
    void applySTDP(size_t currentStep, double timestep);
    void applyHebbian(size_t currentStep, double timestep);
    void applyStructuralPlasticity(size_t currentStep, double timestep, std::mt19937& rng);
    void applyRewardModulatedPlasticity(float reward);
    
    // Integration with neuromodulation
    void setBrainReference(BrainCore* brain) { brain_ = brain; }
    void setNeuromodulationReference(BrainNeuromodulation* neuromodulation) { neuromodulation_ = neuromodulation; }
    
    // Homeostatic mechanisms
    void applyHomeostaticScaling(size_t currentStep);
    void applyMetaplasticity();\nprivate:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BrainCore* brain_ = nullptr;
    BrainNeuromodulation* neuromodulation_ = nullptr;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
};

} // namespace nlm
