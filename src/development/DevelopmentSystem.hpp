#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;
class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

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
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // New development system integration methods
    
    // Modulate working memory based on developmental stage
    void modulateWorkingMemory(NeuralWorkingMemory* workingMemory);
    
    // Modulate episodic memory based on developmental stage
    void modulateEpisodicMemory(NeuralEpisodicMemory* episodicMemory);
    
    // Modulate associative memory based on developmental stage
    void modulateAssociativeMemory(NeuralAssociativeMemory* associativeMemory);
    
    // Modulate prediction system based on developmental stage
    void modulatePredictionSystem(PredictionSystem* predictionSystem);
    
    // Modulate neural planner based on developmental stage
    void modulateNeuralPlanner(NeuralPlanner* planner);
    
    // Modulate concept formation based on developmental stage
    void modulateConceptFormation(ConceptFormation* conceptFormation);
    
    // Modulate attentional selection based on developmental stage
    void modulateAttentionalSelection(AttentionalSelection* attention);
    
    // Modulate dopamine system based on developmental stage
    void modulateDopamine(Dopamine* dopamine);
    
    // Modulate acetylcholine system based on developmental stage
    void modulateAcetylcholine(Acetylcholine* acetylcholine);
    
    // Modulate norepinephrine system based on developmental stage
    void modulateNorepinephrine(Norepinephrine* norepinephrine);
    
    // Modulate serotonin system based on developmental stage
    void modulateSerotonin(Serotonin* serotonin);
    
    // Modulate STDP based on developmental stage
    void modulateSTDP(STDP* stdp);
    
    // Modulate Hebbian learning based on developmental stage
    void modulateHebbian(Hebbian* hebbian);
    
    // Modulate structural plasticity based on developmental stage
    void modulateStructuralPlasticity(StructuralPlasticity* structuralPlasticity);
    
    // Apply development modulation to all brain systems
    void modulateAllSystems(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
