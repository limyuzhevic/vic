// BrainManager.hpp
// Brain lifecycle management for NLM

#ifndef NLM_BRAINMANAGER_HPP
#define NLM_BRAINMANAGER_HPP

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace nlm {

class Brain;
class Config;

// Brain manager class for handling brain lifecycle
class BrainManager {
public:
    BrainManager() = default;
    ~BrainManager() = default;
    
    // Initialize brain with configuration
    bool initializeBrain(const std::shared_ptr<Config>& config);
    
    // Get the brain instance
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Check if brain is initialized
    bool isInitialized() const { return brain != nullptr && initialized; }
    
    // Reset brain to initial state
    bool resetBrain();
    
    // Reinitialize brain (reset + initialize)
    bool reinitializeBrain(const std::shared_ptr<Config>& config);
    
    // Log brain status
    void logStatus() const;
    
    // Get brain statistics
    struct BrainStats {
        uint64_t totalNeurons = 0;
        uint64_t totalSynapses = 0;
        uint64_t totalSpikes = 0;
        float averageFiringRate = 0.0f;
        uint64_t firingNeuronCount = 0;
        uint64_t regionCount = 0;
        double initializationTime = 0.0;  // seconds
        bool plasticityEnabled = false;
        bool developmentEnabled = false;
    };
    
    BrainStats getStats() const;
    
    // Get simulation clock time
    double getSimulationTime() const { return currentSimulationTime; }
    
    // Step the simulation
    bool stepSimulation(uint64_t step, double timestep);
    
    // Inject current into specific neurons
    bool injectCurrentToNeurons(NeuronType neuronType, float current, uint64_t count = 0);
    
    // Get neurons by type
    std::vector<Neuron*> getNeuronsByType(NeuronType type) const;
    
    // Get synapses by type
    std::vector<Synapse*> getSynapsesByType(SynapseType type) const;
    
    // Enable/disable plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    
    // Enable/disable development
    void enableDevelopment(bool enable, float synaptogenesisRate = 0.0001f, 
                           float pruningRate = 0.00001f, float maturationRate = 0.00005f);
    
    // Get most active neurons
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Check if brain systems are properly integrated
    bool verifyIntegration() const;
    
    // Save brain state to file
    bool saveState(const std::string& filename) const;
    
    // Load brain state from file
    bool loadState(const std::string& filename);
    
    // Get brain configuration
    std::shared_ptr<Config> getConfig() const { return brainConfig; }
    
    // Set configuration
    void setConfig(const std::shared_ptr<Config>& config) { brainConfig = config; }
    
    // Check if development system exists
    bool hasDevelopmentSystem() const;
    
    // Check if memory systems exist
    bool hasMemorySystems() const;
    
    // Check if neuromodulation systems exist
    bool hasNeuromodulationSystems() const;
    
    // Check if prediction system exists
    bool hasPredictionSystem() const;
    
    // Check if cognition systems exist
    bool hasCognitionSystems() const;
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> brainConfig;
    bool initialized = false;
    double currentSimulationTime = 0.0;
    std::chrono::high_resolution_clock::time_point initializationStartTime;
    
    // Helper methods
    void resetStats();
    void updateStats();
    
    // Check if we can perform an operation (brain must be initialized)
    bool checkInitialized(const std::string& operationName) const;
    
    // Validate operation parameters
    bool validateParameters(const std::string& operationName, 
                           uint64_t param1 = 0, float param2 = 0.0f) const;
};

} // namespace nlm

#endif // NLM_BRAINMANAGER_HPP