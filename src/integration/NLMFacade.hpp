#pragma once

/**
 * NLM Integration Facade
 * 
 * This facade provides a simplified, unified interface to the NLM brain system,
 * making it easier to:
 * - Add new commands and features
 * - Use advanced brain capabilities
 * - Implement experiments and simulations
 * - Access brain statistics and state
 * 
 * The facade follows the Facade design pattern to hide complexity
 * while providing powerful, high-level access to brain systems.
 */

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/Memory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

/**
 * Facade for easy access to brain systems and advanced commands
 */
class NLMFacade {
public:
    // Constructor and destructor
    NLMFacade(std::shared_ptr<Config> config = nullptr);
    ~NLMFacade();
    
    // Brain lifecycle management
    bool initialize(size_t neuronCount = 1000);
    void reset();
    void step();
    void step(Timestamp time);
    
    // System access (convenience methods)
    
    // Memory systems
    void storeWorkingMemory(const std::string& key, const std::vector<float>& value);
    std::vector<float> retrieveWorkingMemory(const std::string& key) const;
    void storeEpisodic(const std::string& episodeKey, const std::vector<std::string>& events);
    std::vector<std::string> retrieveEpisodic(const std::string& episodeKey) const;
    
    // Neuromodulation commands
    void setReward(float reward);
    void setCuriosity(float curiosity);
    void setNovelty(float novelty);
    float getCurrentDopamineLevel() const;
    
    // Prediction commands
    void predict(const std::vector<float>& sensoryInput);
    std::vector<float> getPrediction() const;
    float getPredictionError() const;
    
    // Cognitive commands
    void planAction(const std::vector<float>& context);
    std::vector<int> getPlannedActions() const;
    void setPlanningDepth(size_t depth);
    
    // Development commands
    void advanceDevelopmentalStage();
    DevelopmentalStage getDevelopmentalStage() const;
    float getPlasticityRate() const;
    
    // Sensory/Motor commands
    void receiveSensoryInput(const std::vector<float>& input);
    std::vector<float> getMotorOutput() const;
    
    // Advanced plasticity commands
    void enableEligibilityTraces(bool enable);
    void setEligibilityTraceDecay(float decayRate);
    void applyExternalReward(float reward, float timeHorizon = 1000.0f);
    
    // Checkpoint and replay commands
    bool saveBrainState(const std::string& filepath) const;
    bool loadBrainState(const std::string& filepath);
    void enableReplay(bool enable);
    void enableCheckpointing(bool enable, const std::string& path = "./checkpoints");
    
    // Analysis and statistics commands
    std::string getBrainStatus() const;
    std::string getSystemMetrics() const;
    std::map<std::string, float> getIntegrationScore() const;
    
    // Experimental commands
    void runExperiment(const std::string& experimentType, const std::map<std::string, std::string>& params);
    void runIntegrationTest();
    void runPlasticityExperiment(size_t trials = 100);
    void runLearningExperiment(float targetAccuracy = 0.9f);
    
    // Configuration commands
    void setConfig(const std::string& key, const std::string& value);
    void setConfig(const std::string& key, float value);
    void setConfig(const std::string& key, int value);
    std::string getConfig(const std::string& key) const;
    
    // Batch processing commands (for performance)
    void processBatch(const std::vector<std::vector<float>>& inputs, 
                     std::vector<std::vector<float>>& outputs);
    void injectCurrentBatch(const std::vector<NeuronId>& neuronIds, 
                           const std::vector<float>& currents);
    
    // Monitoring commands
    void startMonitoring(size_t intervalMs = 1000);
    void stopMonitoring();
    std::vector<std::string> getMonitoringData() const;
    
    // Utility commands
    size_t getNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
    float getExcitatoryInhibitionRatio() const;
    
    // Advanced commands for power users
    void enableRealTimeMode(bool enable);
    void setTargetFPS(float fps);
    void enableDebugOutput(bool enable);
    void addCustomCommand(const std::string& name, std::function<void()> command);
    
    // Learning and adaptation commands
    void triggerExploration();
    void triggerExploitation();
    void updateEnvironmentalModel(const std::vector<float>& model);
    std::vector<float> getEnvironmentalModel() const;
    
    // Social and collaborative commands
    void connectToPeer(const std::string& peerId);
    void disconnectFromPeer(const std::string& peerId);
    void shareBrainState(const std::string& peerId);
    void mergeBrainState(const std::string& peerId, float weight = 0.5f);
    
    // Utility commands
    bool isInitialized() const;
    bool isRunning() const;
    std::string getVersionInfo() const;
    
    // Event-driven commands
    void addSpikeListener(std::function<void(const SpikeEvent&)> listener);
    void addRewardListener(std::function<void(float)> listener);
    void addMemoryListener(std::function<void(const MemoryEvent&)> listener);
    
    // Configuration management
    void loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    void resetConfiguration();
    
    // Performance optimization commands
    void optimizeForSpeed();
    void optimizeForAccuracy();
    void setSimulationSpeed(float multiplier);
    float getSimulationSpeed() const;
    
    // Debug and diagnostic commands
    void enableDebugMode(bool enable);
    std::string getDebugInfo() const;
    std::vector<std::string> getSystemWarnings() const;
    bool hasSystemErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void setupDefaultConfiguration();
    void initializeAdvancedSystems();
    void setupEventHandlers();
    std::shared_ptr<Brain> getBrain() const;
};

} // namespace nlm
