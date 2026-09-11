namespace nlm {

// Advanced development and developmental tracking system
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Update development with brain reference
    void update(class Brain* brain, SimulationStep currentStep);
    void update(class Brain* brain, class RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get maturation parameters
    float getMaturationRate() const;
    float getSynaptogenesisRate() const;
    float getPruningThreshold() const;
    
    // Track developmental progress
    float getDevelopmentalProgress() const; // 0.0 to 1.0
    
    // Access developmental parameters
    float getDevelopmentalParameter(const std::string& parameter) const;
    void setDevelopmentalParameter(const std::string& parameter, float value);
    
    // Get developmental metrics
    struct DevelopmentalMetrics {
        double age;                    // Age in simulation time
        float progress;                // Progress through development (0.0-1.0)
        float plasticityFactor;        // Current plasticity level
        float learningRate;            // Current learning rate
        float memoryConsolidation;     // Memory consolidation strength
        float noveltySensitivity;      // Sensitivity to novel stimuli
        float attentionSpan;           // Attention span in time units
        float curiosityLevel;          // Curiosity drive (0.0-1.0)
        float explorationBias;         // Bias toward exploration vs exploitation
    };
    
    DevelopmentalMetrics getMetrics() const;
    
    // Stage-specific development methods
    void updateCriticalPeriod(Brain* brain);
    void updateMaturation(Brain* brain, SimulationStep currentStep);
    void updateSynaptogenesis(Brain* brain, RandomGenerator& rng);
    void updatePruning(Brain* brain, RandomGenerator& rng);
    
    // Get developmental stage information
    struct StageInfo {
        DevelopmentalStage stage;
        const char* name;
        double duration;               // Typical duration in simulation time
        float plasticityMultiplier;    // Plasticity modifier for this stage
        float learningMultiplier;      // Learning rate modifier
        float noveltySensitivity;      // Novelty sensitivity
        bool isCritical;               // Is critical period
        std::vector<std::string> milestones; // Development milestones
    };
    
    StageInfo getStageInfo(DevelopmentalStage stage) const;
    StageInfo getCurrentStageInfo() const;
    
    // Get current stage name as string
    const char* getCurrentStageName() const;
    
    // Get time spent in current stage
    double getTimeInCurrentStage() const;
    
    // Get developmental progress towards next stage
    float getProgressToNextStage() const;
    
    // Complete development
    void completeDevelopment();
    
    // Reset development
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
