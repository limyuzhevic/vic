#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <chrono>
#include <stdexcept>
#include "src/core/Types/Types.hpp"

namespace nlm {

class TestFixture {
public:
    virtual ~TestFixture() = default;
    
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual std::string getName() const { return "BaseFixture"; }
    
    void registerResource(const std::string& name, std::shared_ptr<void> resource);
    std::shared_ptr<void> getResource(const std::string& name);
    bool hasResource(const std::string& name) const;
    
    void addSetupHook(std::function<void()> hook);
    void addTeardownHook(std::function<void()> hook);
    
protected:
    std::map<std::string, std::shared_ptr<void>> resources;
    std::vector<std::function<void()>> setupHooks;
    std::vector<std::function<void()>> teardownHooks;
};

class BrainFixture : public TestFixture {
public:
    BrainFixture();
    ~BrainFixture() override;
    
    std::string getName() const override { return "BrainFixture"; }
    
    void setup() override;
    void teardown() override;
    
    Brain& getBrain() { return *brain; }
    const Brain& getBrain() const { return *brain; }
    
    Brain& getBrain(const std::string& regionName);
    const Brain& getBrain(const std::string& regionName) const;
    
    void injectSensoryInput(const class SensoryInput& input);
    void setSimulationStep(SimulationStep step);
    
    // Memory system access
    NeuralWorkingMemory& getWorkingMemory() const { return *workingMemory; }
    NeuralEpisodicMemory& getEpisodicMemory() const { return *episodicMemory; }
    NeuralAssociativeMemory& getAssociativeMemory() const { return *associativeMemory; }
    
    // Plasticity system access
    STDP& getSTDP() const { return *stdp; }
    Hebbian& getHebbian() const { return *hebbian; }
    StructuralPlasticity& getStructuralPlasticity() const { return *structuralPlasticity; }
    
    // Neuromodulation system access
    Dopamine& getDopamine() const { return *dopamine; }
    Curiosity& getCuriosity() const { return *curiosity; }
    Novelty& getNovelty() const { return *novelty; }
    PredictionError& getPredictionError() const { return *predictionError; }
    
    // Development system access
    DevelopmentSystem& getDevelopmentSystem() const { return *developmentSystem; }
    
    // Cognition system access
    NeuralPlanner& getPlanner() const { return *planner; }
    ConceptFormation& getConceptFormation() const { return *conceptFormation; }
    
    // Utility methods for testing
    void resetBrainState() { if (brain) brain->reset(); }
    void stepSimulation(SimulationStep step) { if (brain) brain->step(step); }
    size_t getTotalNeuronCount() const { return brain ? brain->getTotalNeuronCount() : 0; }
    size_t getTotalSynapseCount() const { return brain ? brain->getTotalSynapseCount() : 0; }
    float getAverageFiringRate() const { return brain ? brain->getAverageFiringRate() : 0.0f; }
    
    SimulationStep getCurrentStep() const { return currentStep; }
    
    // Memory management
    void createRegion(const std::string& name = "");
    void createRegionsFromConfig();
    void clearRegions();
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<Brain> brain;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    SimulationStep currentStep;
};

class MemoryFixture : public TestFixture {
public:
    MemoryFixture();
    ~MemoryFixture() override;
    
    std::string getName() const override { return "MemoryFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // Working memory tests
    void testWorkingMemoryCapacity();
    void testWorkingMemoryDecay();
    void testWorkingMemoryPatternCompletion();
    
    // Episodic memory tests
    void testEpisodicMemoryStorage();
    void testEpisodicMemoryRetrieval();
    void testEpisodicMemoryTemporalSequence();
    
    // Associative memory tests
    void testAssociativeMemoryHebbianLearning();
    void testAssociativeMemoryPatternCompletion();
    void testAssociativeMemoryForgetting();
    
    // Cross-memory integration tests
    void testWorkingEpisodicIntegration();
    void testEpisodicAssociativeIntegration();
    
    NeuralWorkingMemory& getWorkingMemory() { return *workingMemory; }
    NeuralEpisodicMemory& getEpisodicMemory() { return *episodicMemory; }
    NeuralAssociativeMemory& getAssociativeMemory() { return *associativeMemory; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
};

class PlasticityFixture : public TestFixture {
public:
    PlasticityFixture();
    ~PlasticityFixture() override;
    
    std::string getName() const override { return "PlasticityFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // STDP tests
    void testSTDPLongTermPotentiation();
    void testSTDPShortTermDepression();
    void testSTDPTemporalVirialance();
    
    // Hebbian tests
    void testHebbianCoactivation();
    void testHebbianWeightUpdate();
    
    // Structural plasticity tests
    void testStructuralGrowth();
    void testStructuralPruning();
    void testStructuralHomeostasis();
    
    STDP& getSTDP() { return *stdp; }
    Hebbian& getHebbian() { return *hebbian; }
    StructuralPlasticity& getStructuralPlasticity() { return *structuralPlasticity; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
};

class NeuromodulationFixture : public TestFixture {
public:
    NeuromodulationFixture();
    ~NeuromodulationFixture() override;
    
    std::string getName() const override { return "NeuromodulationFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // Dopamine tests
    void testDopamineRewardSignaling();
    void testDopaminePredictionError();
    void testDopamineTemporalDifference();
    
    // Curiosity tests
    void testCuriosityNoveltyDetection();
    void testCuriosityExplorationDrive();
    void testCuriosityInformationGain();
    
    // Novelty tests
    void testNoveltyChangeDetection();
    void testNoveltyArousal();
    
    // Prediction error tests
    void testPredictionErrorLearning();
    void testPredictionErrorPrecision();
    
    Dopamine& getDopamine() { return *dopamine; }
    Curiosity& getCuriosity() { return *curiosity; }
    Novelty& getNovelty() { return *novelty; }
    PredictionError& getPredictionError() { return *predictionError; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
};

class DevelopmentFixture : public TestFixture {
public:
    DevelopmentFixture();
    ~DevelopmentFixture() override;
    
    std::string getName() const override { return "DevelopmentFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // Synaptogenesis tests
    void testSynaptogenesisGrowthPhase();
    void testSynaptogenesisPruningPhase();
    void testSynaptogenesisMaturationPhase();
    
    // Maturation tests
    void testMaturationExcitatoryInhibitionBalance();
    void testMaturationNeuralTopology();
    void testMaturationFunctionalProperties();
    
    // Pruning tests
    void testPruningActivityDependent();
    void testPruningCompetitive();
    void testPruningHomeostatic();
    
    Synaptogenesis& getSynaptogenesis() { return *synaptogenesis; }
    Maturation& getMaturation() { return *maturation; }
    Pruning& getPruning() { return *pruning; }
    DevelopmentSystem& getDevelopmentSystem() { return *developmentSystem; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<Synaptogenesis> synaptogenesis;
    std::unique_ptr<Maturation> maturation;
    std::unique_ptr<Pruning> pruning;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
};

class CognitionFixture : public TestFixture {
public:
    CognitionFixture();
    ~CognitionFixture() override;
    
    std::string getName() const override { return "CognitionFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // Planning tests
    void testPlanningHierarchicalActions();
    void testPlanningTemporalSequences();
    void testPlanningGoalConflicts();
    
    // Concept formation tests
    void testConceptFormationPatternDiscovery();
    void testConceptFormationAbstraction();
    void testConceptFormationGeneralization();
    
    // Attention tests
    void testAttentionSelectiveAttention();
    void testAttentionGating();
    void testAttentionResourceAllocation();
    
    NeuralPlanner& getPlanner() { return *planner; }
    ConceptFormation& getConceptFormation() { return *conceptFormation; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
};

class IntegrationFixture : public TestFixture {
public:
    IntegrationFixture();
    ~IntegrationFixture() override;
    
    std::string getName() const override { return "IntegrationFixture"; }
    
    void setup() override;
    void teardown() override;
    
    // Component integration tests
    void testBrainMemoryIntegration();
    void testBrainPlasticityIntegration();
    void testBrainNeuromodulationIntegration();
    void testBrainCognitionIntegration();
    
    // Cross-system dynamics
    void testEmergentBehaviors();
    void testSystemStability();
    void testAdaptationCapacity();
    
    Brain& getBrain() { return *brain; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::unique_ptr<Brain> brain;
};

} // namespace nlm
