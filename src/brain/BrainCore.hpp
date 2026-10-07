//=============================================================
// BRAIN CORE INTERFACE - Brain.cpp Refactoring (Phase 7)
// Abstract base class for core brain functionality
//=============================================================

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class BrainCore {
public:
    virtual ~BrainCore() = default;
    
    // Virtual initialization
    virtual bool initialize() = 0;
    
    // Core simulation methods
    virtual void step(SimulationStep currentStep) = 0;
    virtual void step(SimulationStep currentStep, Timestamp currentTime) = 0;
    
    // Sensory and motor I/O
    virtual void receiveSensoryInput(const class SensoryInput& input) = 0;
    virtual void injectCurrent(NeuronId neuron, MembranePotential current) = 0;
    virtual void injectCurrentToNeurons(NeuronType type, MembranePotential current) = 0;
    
    // Statistics and queries
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getTotalSynapseCount() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual float getAverageFiringRate() const = 0;
    virtual float getExcitationInhibitionRatio() const = 0;
    virtual size_t getTotalSpikeCount() const = 0;
    virtual size_t getPendingSpikeEventCount() const = 0;
    
    // Action production
    virtual std::unique_ptr<class Action> produceAction() = 0;
    
    // Plasticity and development
    virtual void applyNeuromodulation(const class Neuromodulator& signal) = 0;
    virtual void updatePlasticity() = 0;
    virtual void develop() = 0;
    
    // State management
    virtual void reset() = 0;
    virtual bool save(const std::string& filepath) const = 0;
    virtual bool load(const std::string& filepath) = 0;
    
    // Region management
    virtual RegionId addRegion(const std::string& name = "") = 0;
    virtual NeuralRegion* getRegion(RegionId id) = 0;
    virtual const NeuralRegion* getRegion(RegionId id) const = 0;
    virtual size_t getRegionCount() const = 0;
    virtual std::vector<RegionId> getRegionIds() const = 0;
    virtual const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const = 0;
    
    // Inter-region connections
    virtual void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) = 0;
    virtual void removeInterRegionConnection(RegionId source, RegionId target) = 0;
    
    // System access
    virtual SpikeSystem* getSpikeSystem() = 0;
    virtual const SpikeSystem* getSpikeSystem() const = 0;
    virtual STDP* getSTDP() = 0;
    virtual Hebbian* getHebbian() = 0;
    virtual StructuralPlasticity* getStructuralPlasticity() = 0;
    
    // Configuration access
    virtual std::shared_ptr<const Config> getConfig() const = 0;
    virtual RandomGenerator* getRandomGenerator() = 0;
    
    // Logging
    virtual void logStatus() const = 0;
    
    // State tracking
    virtual SimulationStep getCurrentStep() const = 0;
    virtual Timestamp getCurrentTime() const = 0;
    virtual void setCurrentStep(SimulationStep step) = 0;
    virtual void setCurrentTime(Timestamp time) = 0;
};

} // namespace nlm
