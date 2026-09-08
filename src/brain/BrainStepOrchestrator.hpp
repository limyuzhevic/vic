namespace nlm {

class BrainStepOrchestrator {
private:
    Brain* brain_;

public:
    explicit BrainStepOrchestrator(Brain* brain);
    
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime);
    void processSpikes(SimulationStep currentStep);
    void updateWorkingMemory(TimestepDuration dt);
    void applyNeuromodulation();
    void applyPlasticity(TimestepDuration dt);
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updatePredictionSystem();
    void updateAttention(TimestepDuration dt);
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep, RandomGenerator& rng);
    void replayMemories(SimulationStep currentStep);
    void applyDevelopment(SimulationStep currentStep, RandomGenerator& rng, TimestepDuration dt);
    void consolidateMemories(SimulationStep currentStep);
    void checkpointManager(SimulationStep currentStep, Timestamp currentTime);
    
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm