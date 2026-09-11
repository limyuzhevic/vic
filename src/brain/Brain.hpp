#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

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
    
    // Context manager for simulation
    class SimulationSession {
    public:
        SimulationSession(Brain* brain, double timestep, SimulationStep startStep = 0);
        ~SimulationSession();
        
        // Run simulation for specified number of steps
        bool runSteps(size_t steps);
        
        // Run simulation until time limit
        bool runUntil(double timeLimit);
        
        // Run single step
        bool step();
        
        // Get current simulation state
        SimulationStep getCurrentStep() const;
        double getCurrentTime() const;
        
        // Check if session is active
        bool isActive() const;
        
    private:
        Brain* brain_;
        double timestep_;
        SimulationStep startStep_;
        SimulationStep currentStep_;
        double currentTime_;
        bool active_;
    };
    
    // Context manager interface for Python
    SimulationSession createSession(double timestep, SimulationStep startStep = 0) {
        return SimulationSession(this, timestep, startStep);
    }
    
    // Enhanced initialization with validation
    bool initializeWithConfig(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& config);
    
    // Simulation helper methods
    bool runSimulationSteps(SimulationStep steps, double timestep = 0.001);
    bool runSimulationTime(double duration, double timestep = 0.001);
    bool runEpisode(double maxDuration = 60.0, double timestep = 0.001);
    
    // Statistical analysis methods
    double getStatisticalMean(const std::vector<float>& data) const;
    double getStatisticalStdDev(const std::vector<float>& data, double mean) const;
    double getStatisticalVariance(const std::vector<float>& data) const;
    std::pair<double, double> getStatisticalBounds(const std::vector<float>& data) const;
    
    // Neural activity analysis
    std::vector<float> getNeutronActivityPattern(RegionId regionId) const;
    double getRegionExcitabilityRatio(RegionId regionId) const;
    std::vector<std::pair<NeuronId, float>> getTopActiveNeurons(size_t topN = 10) const;
    
    // Memory system analysis
    size_t getWorkingMemoryUsage() const;
    size_t getEpisodicMemoryCount() const;
    double getMemoryConsolidationScore() const;
    
    // Development and learning metrics
    float getPlasticityIndex() const;
    float getLearningRate() const;
    float getExplorationRate() const;
    
    // Experimental mode helpers
    bool enableExperimentalMode();
    bool enableResearchMode();
    bool disableAllModulators();
    
    // Debugging and profiling methods
    void startProfiler();
    void stopProfiler();
    bool isProfilerActive() const;
    std::string getProfilerReport() const;
    
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    void enableLoggingToFile(const std::string& filepath);
    void disableLoggingToFile();
    bool isLoggingToFile() const;
    
    // Performance monitoring
    double getSimulationSpeed() const;
    double getAverageFiringRatePerStep() const;
    size_t getMemoryFootprint() const;
    
    // Simulation state management
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // Checkpoint and recovery
    bool createCheckpoint(const std::string& prefix = "checkpoint");
    bool restoreFromCheckpoint(const std::string& filepath);
    std::vector<std::string> getAvailableCheckpoints() const;
    
    // Configuration validation
    bool validateConfiguration(std::vector<std::string>& errors) const;
    bool isConfigurationValid() const;
    
    // Convenience methods for common patterns
    void setupForControlTask(float targetReward = 1.0f, float explorationBonus = 0.1f);
    void setupForExplorationTask(float noveltyThreshold = 0.5f, float curiosityFactor = 1.0f);
    void setupForMemoryTask(float memoryCapacity = 1000.0f, float consolidationRate = 0.01f);
    
    // Export and visualization helpers
    std::string generateActivityReport() const;
    std::string generateConnectivityMatrix(RegionId regionId) const;
    void exportNeutronActivity(const std::string& filepath, RegionId regionId) const;
    void exportSynapseWeights(const std::string& filepath) const;
    
    // Statistical analysis helpers
    struct StatisticalSummary {
        double mean;
        double variance;
        double stddev;
        double min;
        double max;
        size_t count;
        
        StatisticalSummary() : mean(0.0), variance(0.0), stddev(0.0), min(0.0), max(0.0), count(0) {}
    };
    
    StatisticalSummary computeStatisticalSummary(const std::vector<float>& data) const;
    std::vector<StatisticalSummary> computeStatisticalSummaryByRegion() const;
    
    // Debugging helpers
    void setNeuronDebugFlag(NeuronId id, bool enabled);
    bool getNeuronDebugFlag(NeuronId id) const;
    void clearAllDebugFlags();
    
    // Error handling and recovery
    class SimulationError : public std::runtime_error {
    public:
        SimulationError(const std::string& message) : std::runtime_error(message) {}
    };
    
    // Exception-safe simulation execution
    bool executeSafely(std::function<bool()> simulationFunc);
    
    // Timing and synchronization
    void setTimeStep(double timestep);
    double getTimeStep() const;
    
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const;
    
    // Real-time control
    void setSimulationSpeed(double speed);
    double getSimulationSpeed() const;
    
    // Advanced simulation control
    void enableFixedTimeStep(bool enabled);
    bool isFixedTimeStepEnabled() const;
    
    void setMaxSimulationSteps(size_t maxSteps);
    size_t getMaxSimulationSteps() const;
    
    void enableGracefulShutdown(bool enabled);
    bool isGracefulShutdownEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
