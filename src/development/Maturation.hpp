#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>

namespace nlm {

// Forward declarations
class Brain;
class RandomGenerator;
class Neuron;
class Synapse;
class NeuralRegion;
class STDP;
class Hebbian;
class Dopamine;
class Reward;
class Curiosity;
class Novelty;
class NeuralEpisodicMemory;
class NeuralWorkingMemory;

// Neural maturation: development of adult-level properties
// REAL IMPLEMENTATION - Phase 2 complete with biologically plausible mechanisms

class Maturation {
public:
    Maturation();
    ~Maturation();
    
    // Get maturation progress (0-1)
    float getProgress() const;
    void setProgress(float progress);
    
    // Update maturation
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update with timestep for compatibility
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
        update(brain, static_cast<SimulationStep>(dt * 1000.0));
    }
    
    // Get mature properties
    MembranePotential getMatureThreshold() const;
    MembranePotential getMatureRestingPotential() const;
    float getMatureTimeConstant() const;
    
    // Get developmental stage multiplier
    float getStageMultiplier(DevelopmentalStage stage) const;
    
    // Get stage-specific progress
    float getStageProgress(DevelopmentalStage stage) const;
    
    // Get current developmental age
    double getDevelopmentalAge() const { return pImpl->developmentalClockAge; }
    
    // Get network activity metrics
    float getAverageFiringRate() const { return pImpl->averageFiringRate; }
    float getActivityCoherence() const { return pImpl->activityCoherence; }
    float getBurstPatternStrength() const { return pImpl->burstPatternStrength; }
    
    // Get molecular mechanism status
    float getMolecularScaffoldingProgress() const { return pImpl->molecularScaffoldingProgress; }
    float getGeneExpressionLevel() const { return pImpl->geneExpressionLevel; }
    float getRejuvenationPotential() const { return pImpl->rejuvenationPotential; }
    
    // Get adult property development status
    float getMatureFiringPatternCoherence() const { return pImpl->matureFiringPatternCoherence; }
    float getMatureConnectivityOptimization() const { return pImpl->matureConnectivityOptimization; }
    float getMatureExcitatoryInhibitoryBalance() const { return pImpl->matureExcitatoryInhibitoryBalance; }
    
    // Get aging status
    float getAgingRate() const { return pImpl->agingRate; }
    float getNeuronalDecline() const { return pImpl->declineFactors[0]; }
    float getSynapticDecline() const { return pImpl->declineFactors[1]; }
    float getMetabolicDecline() const { return pImpl->declineFactors[2]; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper methods for internal implementation
    void updateNeuromodulationInfluences(Brain* brain);
    void updateActivityDependentMaturation(Brain* brain);
    void updateMolecularMechanisms(Brain* brain, float maturationRate);
    void applyStageEffects(Brain* brain, float maturationRate);
    void updateExperienceDependentMaturation(Brain* brain);
    void updateNeuralPatternDevelopment(Brain* brain, float maturationRate);
    void applyAdultPropertyDevelopment(Brain* brain, float maturationRate);
    void applyAgingMaturationEffects(Brain* brain, float maturationRate);
    
    // Neural activity calculation methods
    float calculateActivityCoherence(Brain* brain) const;
    float calculateBurstPatternStrength(Brain* brain) const;
    float calculateMolecularStability(Brain* brain) const;
    float calculateProteinSynthesisRate(Brain* brain) const;
    float calculateGeneExpressionRate(Brain* brain, float maturationRate) const;
    float calculateExperienceSignal(Brain* brain) const;
    float calculateRegionalPatternStrength(NeuralRegion* region) const;
    
    // Experience and neuron calculation methods
    float calculateNeuronExperience(Neuron* neuron) const;
    float calculateFiringHistoryExperience(Neuron* neuron) const;
    float calculateMembranePotentialExperience(Neuron* neuron) const;
    float calculateSynapticHistoryExperience(Neuron* neuron) const;
    float calculateNeuronActivity(Neuron* neuron) const;
    float calculateSynapseActivityHistory(Synapse* synapse) const;
    
    // Synapse and neuron tracking methods
    size_t findNeuronIndex(NeuronId neuronId) const;
    size_t findSynapseIndex(SynapseId synapseId) const;
    float getSynapseAge(SynapseId synapseId) const;
    float calculateSynapseMolecularStability(Synapse* synapse) const;
    
    // Molecular and developmental calculation methods
    float calculateAgingRate() const;
    float calculateAgingImpact() const;
    float calculateExperienceMaturation(Brain* brain) const;
    float calculatePatternBasedMaturation(Brain* brain, float patternStrength, float maturationRate) const;
    
    // Helper calculation methods
    void updateSynapticStabilizationFactors(Brain* brain, float maturationRate);
    void applyMolecularMaturationToNeurons(Brain* brain, float maturationRate);
    void updateAdultProperties(Neuron* neuron, float maturationRate);
    void optimizeAdultNetworkProperties(Brain* brain, float maturationRate);
    void enhanceExperienceBasedConnections(Brain* brain, Neuron* neuron, float experience);
    void applyPatternBasedMaturation(Brain* brain, float patternStrength, float maturationRate);
    void applySynapticRemodelingRejuvenation(NeuralRegion* region, float maturationRate);
    void updateNeuronPropertiesFromMolecularMaturation(Neuron* neuron, float maturationRate);
    void applyRejuvenationMechanisms(Brain* brain, float maturationRate);
    bool remodelSynapticConnection(NeuralRegion* region, Synapse* oldSynapse);
    void calculateMaturationProgress(Brain* brain);
};

} // namespace nlm
