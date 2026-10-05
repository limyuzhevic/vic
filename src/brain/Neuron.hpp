#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <optional>
#include <unordered_map>
#include <string>
#include <cassert>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron template for advanced initialization
struct NeuronTemplate {
    NeuronType type;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    uint32_t refractoryPeriod;
    float adaptationRate;
    bool hasPlasticity;
    float initialEnergy;
    float maxEnergy;
    
    NeuronTemplate()
        : type(NeuronType::Internal)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , refractoryPeriod(5)
        , adaptationRate(0.01f)
        , hasPlasticity(true)
        , initialEnergy(1.0f)
        , maxEnergy(10.0f) {}
};

// Initialization strategy enumeration
enum class InitializationStrategy : uint8_t {
    Random,
    TypeSpecific,
    DevelopmentStage,
    PopulationSpecific,
    SynapticContext,
    EnergyBudget,
    TemplateBased,
    ActivityDependent,
    AgeDependent
};

// Population statistics for population-specific initialization
struct PopulationStats {
    float avgRestingPotential;
    float avgThreshold;
    float avgResetPotential;
    float stdDevRestingPotential;
    float stdDevThreshold;
    float stdDevResetPotential;
    float firingRate;
    float energyConsumption;
    float synapticDensity;
    
    PopulationStats()
        : avgRestingPotential(-70.0f)
        , avgThreshold(-55.0f)
        , avgResetPotential(-70.0f)
        , stdDevRestingPotential(2.0f)
        , stdDevThreshold(2.0f)
        , stdDevResetPotential(2.0f)
        , firingRate(10.0f)
        , energyConsumption(1.0f)
        , synapticDensity(1.0f) {}
};

// Global batch processing configuration
BatchProcessingConfig BatchProcessor::globalConfig;

// Thread-safe batch processing manager
class BatchProcessor {
public:
    BatchProcessor() {
        config.enableParallel = false;
        config.enableAsync = false;
    }
    
    // Set batch processing configuration
    void setConfig(const BatchProcessingConfig& c) {
        std::lock_guard<std::mutex> lock(mutex);
        config = c;
    }
    
    // Get current configuration
    BatchProcessingConfig getConfig() const {
        std::lock_guard<std::mutex> lock(mutex);
        return config;
    }
    
    // Record batch operation metrics
    void recordOperation(const BatchProcessingMetrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex);
        accumulatedMetrics.neuronsProcessed += metrics.neuronsProcessed;
        accumulatedMetrics.totalSteps += metrics.totalSteps;
        accumulatedMetrics.parallelProcessed += metrics.parallelProcessed;
        accumulatedMetrics.asyncProcessed += metrics.asyncProcessed;
        accumulatedMetrics.numOperations++;
        if (config.enablePerformanceMonitoring) {
            double stepTime = (metrics.neuronsProcessed > 0) ? 
                (metrics.totalSteps / static_cast<double>(metrics.neuronsProcessed)) : 0.0;
            accumulatedMetrics.averageStepTime += stepTime;
            if (stepTime > accumulatedMetrics.maxStepTime) accumulatedMetrics.maxStepTime = stepTime;
            if (stepTime < accumulatedMetrics.minStepTime || accumulatedMetrics.minStepTime == 0.0) 
                accumulatedMetrics.minStepTime = stepTime;
        }
    }
    
    // Get accumulated metrics
    BatchProcessingMetrics getAccumulatedMetrics() const {
        std::lock_guard<std::mutex> lock(mutex);
        return accumulatedMetrics;
    }
    
    // Reset metrics
    void resetMetrics() {
        std::lock_guard<std::mutex> lock(mutex);
        accumulatedMetrics.reset();
    }
    
    // Global configuration
    static BatchProcessingConfig globalConfig;
    
private:
    BatchProcessingConfig config;
    BatchProcessingMetrics accumulatedMetrics;
    mutable std::mutex mutex;
};

// Batch processing configuration
struct BatchProcessingConfig {
    bool enableParallel{false};
    bool enableAsync{false};
    bool enablePerformanceMonitoring{true};
    uint32_t maxBatchSize{100};
    uint32_t numThreads{std::thread::hardware_concurrency()};
    std::chrono::milliseconds timeout{1000};
    
    BatchProcessingConfig() = default;
    
    void reset() {
        enableParallel = false;
        enableAsync = false;
        enablePerformanceMonitoring = true;
        maxBatchSize = 100;
        numThreads = std::thread::hardware_concurrency();
        timeout = std::chrono::milliseconds{1000};
    }
};

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Stability metrics structure
struct NeuronStabilityMetrics {
    float avgVoltageVelocity;           // Average dV/dt (mV/ms)
    float maxVoltageVelocity;           // Maximum dV/dt (mV/ms)
    float avgVoltageAcceleration;        // Average d²V/dt² (mV/ms²)
    float maxVoltageAcceleration;        // Maximum d²V/dt² (mV/ms²)
    float minStableVoltage;              // Minimum stable voltage observed (mV)
    float maxStableVoltage;              // Maximum stable voltage observed (mV)
    uint32_t stableFramesCount;         // Number of stable frames
    uint32_t totalSteps;                // Total steps taken
    float stabilityScore;               // 0.0 to 1.0, higher = more stable
    float numericalStability;           // Numerical stability indicator
    bool hasNumericalIssues;            // Flag for numerical problems
    
    NeuronStabilityMetrics()
        : avgVoltageVelocity(0.0f), maxVoltageVelocity(0.0f),
          avgVoltageAcceleration(0.0f), maxVoltageAcceleration(0.0f),
          minStableVoltage(0.0f), maxStableVoltage(0.0f),
          stableFramesCount(0), totalSteps(0), stabilityScore(1.0f),
          numericalStability(1.0f), hasNumericalIssues(false) {}
};

// Batch processing configuration
struct BatchProcessingConfig {
    bool enableParallel{false};
    bool enableAsync{false};
    bool enablePerformanceMonitoring{true};
    uint32_t maxBatchSize{100};
    uint32_t numThreads{std::thread::hardware_concurrency()};
    std::chrono::milliseconds timeout{1000};
    
    BatchProcessingConfig() = default;
    
    void reset() {
        enableParallel = false;
        enableAsync = false;
        enablePerformanceMonitoring = true;
        maxBatchSize = 100;
        numThreads = std::thread::hardware_concurrency();
        timeout = std::chrono::milliseconds{1000};
    }
};

// Error handling structures
enum class ErrorType : uint8_t {
    ParameterValidation,
    StateValidation,
    SynapticInputValidation,
    CurrentInjectionValidation,
    SpikeRecordValidation,
    SynapseOperationValidation,
    MemoryAllocation,
    StateTransitionValidation,
    BoundaryCondition,
    RefractoryPeriod,
    Overflow,
    NumericalStability
};

struct ErrorInfo {
    ErrorType type;
    std::string message;
    std::string file;
    int line;
    std::string function;
    Timestamp timestamp;
    bool isWarning;
    
    ErrorInfo() : type(ErrorType::ParameterValidation), message(""), file(""), line(0), 
                  function(""), timestamp(0.0), isWarning(false) {}
    
    ErrorInfo(ErrorType t, const std::string& msg, const std::string& file, int line, 
              const std::string& func, bool warn = false) 
        : type(t), message(msg), file(file), line(line), function(func), 
          timestamp(std::chrono::duration_cast<std::chrono::duration<double>>(
              std::chrono::system_clock::now().time_since_epoch()).count()), 
          isWarning(warn) {}
};

struct ErrorHandlingResult {
    bool success;
    std::string message;
    std::vector<ErrorInfo> errors;
    std::vector<ErrorInfo> warnings;
    
    ErrorHandlingResult() : success(true), message("Success") {}
    
    ErrorHandlingResult(bool s, const std::string& msg) : success(s), message(msg) {}
    
    void addError(ErrorType type, const std::string& msg, const std::string& file = "", 
                  int line = 0, const std::string& function = "", bool isWarning = false) {
        ErrorInfo error(type, msg, file, line, function, isWarning);
        if (isWarning) {
            warnings.push_back(error);
            NLM_LOG_WARNING("[" + std::to_string(static_cast<int>(type)) + "] " + msg);
        } else {
            errors.push_back(error);
            NLM_LOG_ERROR("[" + std::to_string(static_cast<int>(type)) + "] " + msg);
        }
    }
    
    size_t getErrorCount(ErrorType type) const {
        size_t count = 0;
        for (const auto& error : errors) {
            if (error.type == type) count++;
        }
        return count;
    }
    
    void clear() {
        errors.clear();
        warnings.clear();
    }
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Static advanced initialization methods
    // Type-specific biological initialization
    static void initializeBiologically(Neuron* neuron, NeuronType type, RandomGenerator& rng);
    
    // Development stage-dependent initialization
    static void initializeByDevelopmentStage(Neuron* neuron, DevelopmentalStage stage, RandomGenerator& rng);
    
    // Population-specific initialization using population statistics
    static void initializeForPopulation(Neuron* neuron, PopulationId population, RandomGenerator& rng);
    
    // Synaptic context initialization based on connection count
    static void initializeWithSynapticContext(Neuron* neuron, int synapticConnections, RandomGenerator& rng);
    
    // Validate initialized parameters are within biological ranges
    static bool validateInitializedParameters(const Neuron* neuron);
    
    // Energy/budget-constrained initialization
    static void initializeWithEnergyBudget(Neuron* neuron, float maxEnergy, RandomGenerator& rng);
    
    // Template-based initialization for consistency
    static void initializeFromTemplate(Neuron* neuron, const NeuronTemplate& template, RandomGenerator& rng);
    
    // Batch advanced initialization with multiple strategies
    static void batchInitializeAdvanced(std::vector<Neuron*>& neurons, InitializationStrategy strategy, RandomGenerator& rng);
    
    // Type-specific population statistics cache
    static std::unordered_map<PopulationId, PopulationStats> populationStats;
    
    // Thread-safe access to population statistics
    static std::mutex populationStatsMutex;
    
    // Get population statistics for a given population ID
    static PopulationStats getPopulationStats(PopulationId population);
    
    // Set population statistics for a given population ID
    static void setPopulationStats(PopulationId population, const PopulationStats& stats);
    
    // Biological parameter ranges and validation constants
    static constexpr float MIN_BIOLOGICAL_POTENTIAL = -100.0f;      // mV
    static constexpr float MAX_BIOLOGICAL_POTENTIAL = 100.0f;       // mV
    static constexpr float MIN_RESTING_POTENTIAL = -90.0f;          // mV
    static constexpr float MAX_RESTING_POTENTIAL = -50.0f;          // mV
    static constexpr float MIN_THRESHOLD = -60.0f;                  // mV
    static constexpr float MAX_THRESHOLD = -30.0f;                   // mV
    static constexpr float MIN_RESET_POTENTIAL = -80.0f;            // mV
    static constexpr float MAX_RESET_POTENTIAL = -60.0f;            // mV
    static constexpr float MIN_LEAK_CONDUCTANCE = 1.0f;             // nS
    static constexpr float MAX_LEAK_CONDUCTANCE = 100.0f;            // nS
    static constexpr float MIN_REFECTORY_PERIOD = 1;                // ms
    static constexpr float MAX_REFECTORY_PERIOD = 100;               // ms
};

} // namespace nlm
