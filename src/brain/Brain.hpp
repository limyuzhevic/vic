#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <format>
#include <span>
#include <ranges>
#include <expected>
#include <optional>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Enhanced inter-regional connection with C++20 features
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
    
    // Enhanced comparison operators for better safety
    bool operator==(const InterRegionConnection& other) const {
        return sourceRegion == other.sourceRegion && 
               targetRegion == other.targetRegion && 
               delay == other.delay;
    }
    
    bool operator!=(const InterRegionConnection& other) const {
        return !(*this == other);
    }
    
    // Helper function to check if connection is valid
    bool isValid() const {
        return sourceRegion.is_valid() && targetRegion.is_valid() && 
               delay > 0 && weight >= -1.0f && weight <= 1.0f;
    }
    
    // String representation using std::format (C++20)
    std::string toString() const {
        return std::format("Connection {} -> {} (weight: {}, delay: {})",
                         sourceRegion.index(), targetRegion.index(),
                         weight, delay);
    }
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// Enhanced with C++20 features for improved performance and type safety

class Brain {
public:
    // Create brain with configuration using smart pointers
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving with noexcept
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step with optional error reporting
    std::expected<void, std::string> step(SimulationStep currentStep);
    
    // Simulation time step with both step and time parameters
    std::expected<void, std::string> step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    std::expected<bool, std::string> injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Enhanced spike system access with optional return
    std::optional<SpikeSystem*> getSpikeSystem() const;
    std::optional<const SpikeSystem*> getSpikeSystemPtr() const;
    
    // Enhanced plasticity system access
    std::optional<STDP*> getSTDP();
    std::optional<Hebbian*> getHebbian();
    std::optional<StructuralPlasticity*> getStructuralPlasticity();
    
    // Enhanced statistics with better type safety
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state with enhanced error reporting
    std::expected<bool, std::string> reset();
    
    // Save brain state to file (checkpointing) with error reporting
    std::expected<bool, std::string> save(const std::string& filepath) const;
    
    // Load brain state from file with error reporting
    std::expected<bool, std::string> load(const std::string& filepath);
    
    // Enhanced region management with better type safety
    RegionId addRegion(const std::string& name = "");
    std::optional<NeuralRegion*> getRegion(RegionId id);
    std::optional<const NeuralRegion*> getRegionConst(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions using span for efficient access
    std::span<const std::unique_ptr<NeuralRegion>> getRegions() const;
    
    // Enhanced inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    std::expected<bool, std::string> removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics with enhanced type safety
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Enhanced memory system accessors
    std::optional<NeuralWorkingMemory*> getWorkingMemory();
    std::optional<NeuralEpisodicMemory*> getEpisodicMemory();
    std::optional<NeuralAssociativeMemory*> getAssociativeMemory();
    
    // Enhanced prediction system accessor
    std::optional<PredictionSystem*> getPredictionSystem();
    
    // Enhanced cognition system accessors
    std::optional<NeuralPlanner*> getPlanner();
    std::optional<ConceptFormation*> getConceptFormation();
    std::optional<AttentionalSelection*> getAttention();
    
    // Enhanced development system
    std::optional<DevelopmentSystem*> getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Enhanced neuromodulation systems
    std::optional<Dopamine*> getDopamine();
    std::optional<Curiosity*> getCuriosity();
    std::optional<Novelty*> getNovelty();
    std::optional<PredictionError*> getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    std::optional<RandomGenerator*> getRandomGenerator();
    
    // Enhanced logging with modern formatting
    void logStatus() const;
    std::string getDebugInfo() const;
    
    // Utility functions for enhanced C++20 integration
    
    // Check if brain is initialized and ready
    bool isInitialized() const;
    
    // Get brain system health status
    std::string getSystemHealth() const;
    
    // Enhanced structured bindings helper
    auto getSystemStats() const {
        return std::tie(
            getRegionCount(),
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getTotalSpikeCount()
        );
    }
    
    // C++20 range-based algorithms for brain operations
    template<typename Function>
    void forEachNeuron(Function&& func) const {
        for (const auto& region : getRegions()) {
            for (const auto& population : region->getPopulations()) {
                for (const auto* neuron : population->getNeurons()) {
                    func(*neuron);
                }
            }
        }
    }
    
    template<typename Predicate>
    size_t countNeuronsIf(Predicate&& pred) const {
        size_t count = 0;
        forEachNeuron([&](const Neuron& neuron) {
            if (pred(neuron)) count++;
        });
        return count;
    }
    
    // C++20 views and algorithms for enhanced data processing
    auto getAllNeurons() const {
        std::vector<const Neuron*> neurons;
        forEachNeuron([&](const Neuron& neuron) {
            neurons.push_back(&neuron);
        });
        return neurons;
    }
    
    auto getExcitatoryNeurons() const {
        return getAllNeurons() | std::views::filter(
            [](const Neuron* neuron) { return neuron->getType() == NeuronType::Excitatory; }
        );
    }
    
    auto getInhibitoryNeurons() const {
        return getAllNeurons() | std::views::filter(
            [](const Neuron* neuron) { return neuron->getType() == NeuronType::Inhibitory; }
        );
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Enhanced initialization state tracking
    bool initialized_ = false;
    
    // Helper functions for enhanced C++20 integration
    static bool validateNeuronId(NeuronId id);
    static bool validateRegionId(RegionId id);
    static bool validatePopulationId(PopulationId id);
};

} // namespace nlm
