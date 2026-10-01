#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

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

// Inter-regional connection (long-range connectivity)
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
};

// Comprehensive brain statistics
struct BrainStatistics {
    // Basic counts
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    
    // Activity metrics
    size_t totalSpikes;
    size_t pendingSpikes;
    float averageFiringRate;
    float excitationInhibitionRatio;
    
    // Regional statistics
    std::vector<std::string> regionNames;
    std::vector<size_t> neuronCountsPerRegion;
    std::vector<size_t> firingCountsPerRegion;
    
    // Plasticity statistics
    float totalWeightChange;
    std::vector<std::string> plasticityMethodsActive;
    
    // Memory statistics
    size_t workingMemoryCapacity;
    size_t episodicMemoryUsage;
    size_t conceptCount;
    
    // Development statistics
    float developmentalAge;
    float plasticityRate;
    DevelopmentalStage developmentalStage;
    
    // Performance statistics
    double lastStepDuration;
    size_t stepsExecuted;
    
    BrainStatistics()
        : totalNeurons(0), totalSynapses(0), activeNeurons(0), firingNeurons(0),
          totalSpikes(0), pendingSpikes(0), averageFiringRate(0.0f),
          excitationInhibitionRatio(0.0f), totalWeightChange(0.0f),
          workingMemoryCapacity(0), episodicMemoryUsage(0), conceptCount(0),
          developmentalAge(0.0f), plasticityRate(0.0f), developmentalStage(DevelopmentalStage::Initial),
          lastStepDuration(0.0), stepsExecuted(0) {}
};

// Brain configuration presets
struct BrainPresets {
    static nlohmann::json createStandardBrain() {
        nlohmann::json config;
        config["random_seed"] = 42;
        config["simulation_timestep"] = 0.001;
        config["neuron_count"] = 500;
        config["region_count"] = 1;
        config["connection_probability"] = 0.15;
        config["stdp_ltp_weight"] = 0.01;
        config["stdp_ltd_weight"] = 0.012;
        config["stdp_time_constant"] = 20.0;
        config["synaptogenesis_rate"] = 0.0001;
        config["pruning_rate"] = 0.00001;
        config["enable_reward_modulation"] = true;
        config["enable_curiosity"] = true;
        config["enable_development"] = true;
        return config;
    }
    
    static nlohmann::json createResearchBrain() {
        nlohmann::json config;
        config["random_seed"] = 12345;
        config["simulation_timestep"] = 0.001;
        config["neuron_count"] = 2000;
        config["region_count"] = 3;
        config["connection_probability"] = 0.08;
        config["stdp_ltp_weight"] = 0.02;
        config["stdp_ltd_weight"] = 0.015;
        config["stdp_time_constant"] = 25.0;
        config["synaptogenesis_rate"] = 0.0005;
        config["pruning_rate"] = 0.0002;
        config["enable_reward_modulation"] = false;
        config["enable_curiosity"] = true;
        config["enable_development"] = true;
        config["enable_structural_plasticity"] = true;
        config["memory_capacity"] = 10000;
        return config;
    }
    
    static nlohmann::json createMinimalBrain() {
        nlohmann::json config;
        config["random_seed"] = 42;
        config["simulation_timestep"] = 0.001;
        config["neuron_count"] = 100;
        config["region_count"] = 1;
        config["connection_probability"] = 0.2;
        config["stdp_ltp_weight"] = 0.01;
        config["stdp_ltd_weight"] = 0.012;
        config["stdp_time_constant"] = 20.0;
        return config;
    }
};

// Brain: The central coordinator of the neural system
// Enhanced with better API design, convenience methods, and comprehensive error handling
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Enhanced initialization with configuration loading
    bool initializeFromFile(const std::string& configFile);
    bool initializeFromConfig(const nlohmann::json& config);
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Convenience step method
    void stepWithTime(Timestamp currentTime, double timestep = 0.001) {
        step(static_cast<SimulationStep>(currentTime / timestep), currentTime);
    }
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Convenience methods for common sensory input types
    void receiveVisionInput(const std::vector<float>& visionData, size_t width, size_t height);
    void receiveAudioInput(const std::vector<float>& audioData, size_t sampleRate);
    void receiveInternalInput(const std::vector<float>& internalSignals);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Convenience current injection methods
    void injectExcitatoryInput(MembranePotential current);
    void injectInhibitoryInput(MembranePotential current);
    void injectSensoryInput(MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction and cognition systems
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Enhanced statistics methods
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getTotalWeightChange() const;
    double getCurrentTime() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Enhanced action production
    std::unique_ptr<class Action> produceActionWithContext(const std::string& context);
    std::unique_ptr<class Action> produceActionBasedOnPrediction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Enhanced neuromodulation
    void applyRewardModulation(float reward);
    void applyCuriosityModulation(float novelty);
    void applyPredictionErrorModulation(float error);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Enhanced developmental control
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
    float getDevelopmentalProgress() const;
    
    // Reset brain state
    void reset();
    
    // Enhanced reset with cleanup
    void resetAllSystems();
    void clearMemory();
    void resetPlasticity();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Enhanced save/load with compression support
    bool saveWithMetadata(const std::string& filepath, const std::string& metadata = "");
    bool load(const std::string& filepath);
    bool loadWithMetadata(std::string& metadataOut);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Enhanced region management
    RegionId addRegionWithConfig(const std::string& name, const nlohmann::json& config);
    void removeRegion(RegionId id);
    std::vector<std::string> getRegionNames() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Brain configuration
    std::shared_ptr<const Config> getConfig() const;
    void updateConfig(const nlohmann::json& config);
    nlohmann::json getConfigAsJson() const;
    
    // Random generator access
    RandomGenerator* getRandomGenerator();
    
    // Logging and monitoring
    void logStatus() const;
    void logDetailedStatus() const;
    std::string getStatusReport() const;
    
    // Convenience diagnostic methods
    void printBrainState() const;
    void printPerformanceMetrics() const;
    void printPlasticitySummary() const;
    
    // Enhanced error handling
    bool isValid() const;
    std::vector<std::string> validateConfiguration() const;
    void handleError(const std::string& error, const std::string& context = "");
    
    // Statistics and analytics
    BrainStatistics getStatistics() const;
    void collectStatistics(BrainStatistics& stats) const;
    
    // Simulation control
    void enableDebugMode(bool enable);
    void setTimeStep(double timestep);
    double getTimeStep() const;
    Timestamp getCurrentTime() const;
    
    // Performance optimization
    void enableOptimizations(bool enableSIMD, bool enableParallel, bool enableCaching);
    bool areOptimizationsEnabled() const;
    
    // Specialized brain creation methods
    static std::shared_ptr<Brain> createStandardBrain();
    static std::shared_ptr<Brain> createResearchBrain();
    static std::shared_ptr<Brain> createMinimalBrain(const nlohmann::json& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void initializeSystems();
    void validateBrainState() const;
    void updateStatistics();
    void cleanupExpiredData();
    
    // Configuration parsing helpers
    void parseConfig(const nlohmann::json& config);
    void applyConfigToSystems(const nlohmann::json& config);
    
    // Error handling helpers
    void logAndThrow(const std::string& error) const;
    void logWarning(const std::string& warning) const;
};

} // namespace nlm