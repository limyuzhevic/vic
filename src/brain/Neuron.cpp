#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <complex>

namespace nlm {

// Forward declarations for enhanced neuron features
class NeuronModel;
class EnhancedLIFModel;
class IzhikevichModel;
class HindmarshRoseModel;
class NeuronModelFactory;
class TemperatureController;
class EnergyEfficiencyModel;
class EnsembleManager;
class ErrorCorrection;

// Neuron Model Factory - creates different neuron model types
class NeuronModelFactory {
public:
    enum class ModelType {
        BasicLIF,
        EnhancedLIF,
        Izhikevich,
        HindmarshRose,
        HodgkinHuxley,
        AdaptiveExponential
    };
    
    static std::unique_ptr<NeuronModel> createModel(ModelType type, const Config& config);
    static ModelType parseModelType(const std::string& name);
    static std::string modelTypeToString(ModelType type);
    static std::vector<std::string> getAvailableModelNames();
};

// Base neuron model interface
class NeuronModel {
public:
    virtual ~NeuronModel() = default;
    
    // Core dynamics methods
    virtual bool update(Timestamp currentTime, TimestepDuration dt) = 0;
    virtual void reset() = 0;
    virtual void initialize(RandomGenerator& rng) = 0;
    
    // State access
    virtual MembranePotential getMembranePotential() const = 0;
    virtual MembranePotential getThreshold() const = 0;
    virtual float getFiringRate() const = 0;
    virtual FiringState getFiringState() const = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    virtual std::string getModelName() const = 0;
    
    // Statistics and debugging
    virtual void recordSpike(Timestamp timestamp) = 0;
    virtual const std::vector<Timestamp>& getSpikeHistory() const = 0;
    virtual float getAverageFiringRate(Timestamp startTime, Timestamp endTime) const = 0;
    
    // Model-specific methods
    virtual void setParameter(const std::string& name, double value) {}
    virtual double getParameter(const std::string& name) const { return 0.0; }
    
    // Error correction
    virtual void applyErrorCorrection(float voltageError, Timestamp timestamp) {}
    
    // Energy and temperature sensitivity
    virtual void setTemperature(float temp) {}
    virtual void updateEnergyCost() {}
    virtual float getEnergyCost() const { return 0.0f; }
    
    // Plasticity
    virtual void applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {}
    virtual void setPlasticityEnabled(bool enabled) {}
};

// Enhanced LIF Model - improved version of basic LIF
class EnhancedLIFModel : public NeuronModel {
public:
    EnhancedLIFModel();
    ~EnhancedLIFModel() override = default;
    
    bool update(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    void initialize(RandomGenerator& rng) override;
    
    MembranePotential getMembranePotential() const override { return V; }
    MembranePotential getThreshold() const override { return threshold; }
    float getFiringRate() const override { return firingRate; }
    FiringState getFiringState() const override { return firingState; }
    
    void configure(const Config& config) override;
    std::string getModelName() const override { return "EnhancedLIF"; }
    
    void setParameter(const std::string& name, double value) override;
    double getParameter(const std::string& name) const override;
    
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override { return spikeHistory; }
    float getAverageFiringRate(Timestamp startTime, Timestamp endTime) const override;
    
    void applyErrorCorrection(float voltageError, Timestamp timestamp) override;
    void setTemperature(float temp) override;
    void updateEnergyCost() override;
    float getEnergyCost() const override { return energyCost; }
    
    void applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) override;
    void setPlasticityEnabled(bool enabled) override;
    
    // Stochastic dynamics
    void setNoiseLevel(float level) { noiseLevel = level; }
    void setNoiseType(std::string type) { noiseType = type; }
    
private:
    // LIF parameters
    MembranePotential V;                    // Membrane potential
    MembranePotential threshold;            // Firing threshold
    MembranePotential restingPotential;     // Resting potential
    MembranePotential resetPotential;       // Reset potential
    float tau;                              // Membrane time constant
    float C;                                // Membrane capacitance
    float gLeak;                            // Leak conductance
    
    // State variables
    FiringState firingState;                // Current firing state
    float firingRate;                       // Instantaneous firing rate
    float refractoryRemaining;              // Refractory period remaining
    float adaptationVariable;               // Spike-frequency adaptation
    float temperature;                      // Temperature in Celsius
    
    // Stochastic dynamics
    float noiseLevel;                       // Noise intensity
    std::string noiseType;                  // Type of noise: "white", "pink", "brown"
    float noiseAccumulator;                 // Accumulator for colored noise
    
    // Energy and metabolism
    float energyCost;                       // Current energy consumption
    float maxEnergyBudget;                  // Maximum energy budget
    float metabolicRate;                    // Metabolic rate coefficient
    
    // Plasticity
    bool plasticityEnabled;                 // Whether plasticity is active
    float eligibilityTrace;                 // For STDP
    float rewardPredictionError;            // For reward-modulated plasticity
    
    // Error correction
    float voltageErrorHistory;              // History of voltage errors
    float spikeTimingError;                 // Spike timing error for correction
    
    // Simulation
    std::vector<Timestamp> spikeHistory;    // Recent spike timestamps
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;
    
    // Temperature sensitivity coefficients
    float Q10;                             // Temperature coefficient
    float tempBase;                         // Base temperature
    float tempSensitivity;                  // Sensitivity to temperature
    
    // Energy efficiency parameters
    float efficiency;                       // Energy efficiency factor
    float minimumEnergy;                    // Minimum energy requirement
    float optimalFiringRate;                // Firing rate for optimal efficiency
    
    // Configuration
    Config config;
    
    // Thread safety
    mutable std::mutex stateMutex;
    
    // Helper methods
    void applyNoise(float dt);
    void updateFiringRate(Timestamp currentTime);
    void updateTemperatureSensitivity();
    void updateEnergyEfficiency();
    void clampVoltage();
    float calculateNoiseContribution(Timestamp currentTime);
};

// Izhikevich Model - simplified biophysical model
class IzhikevichModel : public NeuronModel {
public:
    IzhikevichModel();
    ~IzhikevichModel() override = default;
    
    bool update(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    void initialize(RandomGenerator& rng) override;
    
    MembranePotential getMembranePotential() const override { return V; }
    MembranePotential getThreshold() const override { return threshold; }
    float getFiringRate() const override { return firingRate; }
    FiringState getFiringState() const override { return firingState; }
    
    void configure(const Config& config) override;
    std::string getModelName() const override { return "Izhikevich"; }
    
    void setParameter(const std::string& name, double value) override;
    double getParameter(const std::string& name) const override;
    
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override { return spikeHistory; }
    float getAverageFiringRate(Timestamp startTime, Timestamp endTime) const override;
    
    void applyErrorCorrection(float voltageError, Timestamp timestamp) override;
    void setTemperature(float temp) override;
    void updateEnergyCost() override;
    float getEnergyCost() const override { return energyCost; }
    
    void applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) override;
    void setPlasticityEnabled(bool enabled) override;
    
private:
    // Izhikevich model variables
    float V;                               // Membrane potential
    float u;                               // Recovery variable
    float threshold;                       // Threshold for spike (usually V > 30)
    
    // Izhikevich parameters
    float a;                              // Recovery time constant
    float b;                              // Sensitivity of recovery variable
    float c;                              // After-spike reset value
    float d;                              // After-spike recovery increment
    float Iinput;                         // External input current
    
    // Stochastic component
    float noise;                          // Noise input
    
    // Temperature sensitivity
    float temperature;                    // Temperature
    
    // Plasticity
    bool plasticityEnabled;
    float eligibilityTrace;
    
    // Energy
    float energyCost;
    
    // State
    FiringState firingState;
    float firingRate;
    std::vector<Timestamp> spikeHistory;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;
};

// Hindmarsh-Rose Model - chaotic dynamics
class HindmarshRoseModel : public NeuronModel {
public:
    HindmarshRoseModel();
    ~HindmarshRoseModel() override = default;
    
    bool update(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    void initialize(RandomGenerator& rng) override;
    
    MembranePotential getMembranePotential() const override { return x; }
    MembranePotential getThreshold() const override { return threshold; }
    float getFiringRate() const override { return firingRate; }
    FiringState getFiringState() const override { return firingState; }
    
    void configure(const Config& config) override;
    std::string getModelName() const override { return "HindmarshRose"; }
    
    void setParameter(const std::string& name, double value) override;
    double getParameter(const std::string& name) const override;
    
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override { return spikeHistory; }
    float getAverageFiringRate(Timestamp startTime, Timestamp endTime) const override;
    
    void applyErrorCorrection(float voltageError, Timestamp timestamp) override;
    void setTemperature(float temp) override;
    void updateEnergyCost() override;
    float getEnergyCost() const override { return energyCost; }
    
    void applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) override;
    void setPlasticityEnabled(bool enabled) override;
    
private:
    // Hindmarsh-Rose variables
    float x;                              // Membrane potential
    float y;                              // Recovery variable
    float z;                              // Slow variable
    
    // Parameters
    float a;                              // Time constant
    float b;                              // Sensitivity
    float c;                              // Current input
    float d;                              // Slope
    
    // Temperature sensitivity
    float temperature;
    
    // Stochastic component
    float noise;
    
    // Plasticity
    bool plasticityEnabled;
    float eligibilityTrace;
    
    // Energy
    float energyCost;
    
    // State
    FiringState firingState;
    float firingRate;
    std::vector<Timestamp> spikeHistory;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;
};

// Temperature Controller - manages temperature effects
class TemperatureController {
public:
    TemperatureController();
    void setTemperature(float temp);
    float getTemperature() const { return currentTemperature; }
    void setBaseTemperature(float temp) { baseTemperature = temp; }
    float getBaseTemperature() const { return baseTemperature; }
    void setTemperatureSensitivity(float sens) { tempSensitivity = sens; }
    
    // Apply temperature effects to model parameters
    template<typename T>
    T applyTemperatureScaling(T value, float Q10 = 2.0f, float referenceTemp = 25.0f) const;
    
    // Get temperature-dependent rate
    float getTemperatureDependentRate(float rate, float Q10 = 2.0f, float referenceTemp = 25.0f) const;
    
private:
    float currentTemperature;
    float baseTemperature;
    float tempSensitivity;
    float ambientTemperature;
    std::chrono::steady_clock::time_point lastUpdate;
};

// Energy Efficiency Model
class EnergyEfficiencyModel {
public:
    EnergyEfficiencyModel();
    
    void update(float firingRate, float voltage, float temperature, float dt);
    float getCurrentEnergyCost() const { return currentEnergyCost; }
    float getEfficiency() const { return efficiency; }
    float getOptimalFiringRate() const { return optimalFiringRate; }
    
    void setEnergyBudget(float budget) { maxEnergyBudget = budget; }
    void setMetabolicRate(float rate) { metabolicRate = rate; }
    
private:
    float currentEnergyCost;
    float maxEnergyBudget;
    float metabolicRate;
    float efficiency;
    float optimalFiringRate;
    float baselineEnergy;
    
    // Efficiency calculation
    void calculateEfficiency(float firingRate, float voltage, float temperature);
};

// Neural Ensemble Manager
class EnsembleManager {
public:
    EnsembleManager();
    
    void addNeuronToEnsemble(NeuronId neuronId, const std::string& ensembleName);
    void removeNeuronFromEnsemble(NeuronId neuronId);
    bool isNeuronInEnsemble(NeuronId neuronId, const std::string& ensembleName) const;
    
    // Ensemble dynamics
    void updateEnsembleCoherence();
    float getEnsembleCoherence(const std::string& ensembleName) const;
    std::vector<NeuronId> getEnsembleNeurons(const std::string& ensembleName) const;
    
    // Group-level dynamics
    void applyGlobalModulation(const std::string& ensembleName, float modulation);
    
private:
    struct Ensemble {
        std::string name;
        std::vector<NeuronId> neuronIds;
        float coherence;                    // 0.0 to 1.0
        float globalModulation;            // Global modulation factor
        float averageFiringRate;
        float voltageVariance;
    };
    
    std::unordered_map<std::string, Ensemble> ensembles;
    mutable std::mutex ensembleMutex;
};

// Error Correction System
class ErrorCorrection {
public:
    ErrorCorrection();
    
    // Voltage error correction
    void applyVoltageCorrection(float& voltage, float targetVoltage, float errorWeight);
    
    // Spike timing error correction
    void applySpikeTimingCorrection(Timestamp spikeTime, float expectedTime, float correctionFactor);
    
    // Reset error accumulators
    void reset() {
        voltageErrorAccum = 0.0f;
        spikeTimingError = 0.0f;
        correctionApplied = 0.0f;
    }
    
    float getVoltageError() const { return voltageError; }
    float getSpikeTimingError() const { return spikeTimingError; }
    float getCorrectionApplied() const { return correctionApplied; }
    
private:
    float voltageError;
    float spikeTimingError;
    float correctionApplied;
    float voltageErrorAccum;
    float maxCorrection;                 // Maximum correction magnitude
    float correctionDecay;               // Rate of decay for correction
};

// Enhanced Neuron implementation with Pimpl pattern
struct Neuron::Impl {
    // Core neuron properties
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    
    // Advanced model system
    std::unique_ptr<NeuronModel> model;      // The actual neuron model
    NeuronModelFactory::ModelType modelType; // Type of model being used
    
    // Temperature and energy systems
    TemperatureController temperatureController;
    EnergyEfficiencyModel energyModel;
    
    // Plasticity and error correction
    float eligibilityTrace;                  // For STDP
    float rewardPredictionError;             // For reward-modulated plasticity
    ErrorCorrection errorCorrection;         // For error correction
    
    // Homeostatic plasticity
    float homeostaticTarget;                // Target firing rate for homeostatic regulation
    float homeostaticLearningRate;          // Learning rate for homeostatic plasticity
    bool homeostaticEnabled;                // Whether homeostatic plasticity is active
    
    // Ensemble management
    EnsembleManager ensembleManager;
    
    // Stochastic dynamics
    float noiseLevel;                        // Noise intensity
    std::string noiseType;                   // Type of noise
    
    // Synaptic dynamics
    float nmdaConductance;                   // NMDA receptor conductance
    float gabaConductance;                   // GABA receptor conductance
    float modulatoryConductance;             // Modulatory synapse conductance
    
    // Synapse type-specific dynamics
    float nmdaMgBlock;                       // Magnesium block for NMDA receptors
    float gabaReversalPotential;             // GABA reversal potential
    float modulatoryReversalPotential;       // Modulatory synapse reversal potential
    
    // NMDA receptor kinetics
    float nmdaRelativeMgBlock;               // Relative magnesium block (0-1)
    float nmdaActivation;                    // NMDA receptor activation
    float nmdaDeactivation;                  // NMDA receptor deactivation
    float nmdaSaturation;                    // NMDA receptor saturation
    
    // GABA kinetics
    float gabaActivation;                    // GABA receptor activation
    float gabaDeactivation;                  // GABA receptor deactivation
    
    // Modulatory synapse kinetics
    float modulatoryActivation;              // Modulatory synapse activation
    float modulatoryDeactivation;            // Modulatory synapse deactivation
    
    // Voltage-gated channels
    float naConductance;                     // Sodium channel conductance
    float kConductance;                      // Potassium channel conductance
    float naActivation;                      // Sodium channel activation
    float kActivation;                       // Potassium channel activation
    
    // Gating variable dynamics
    float naActivationRate;                  // Sodium channel activation rate
    float naInactivationRate;                // Sodium channel inactivation rate
    float naRecoveryRate;                    // Sodium channel recovery rate
    float kActivationRate;                   // Potassium channel activation rate
    float kInactivationRate;                 // Potassium channel inactivation rate
    
    // Channel kinetics
    float naTimeConstant;                    // Sodium channel time constant
    float kTimeConstant;                     // Potassium channel time constant
    
    // Performance and optimization
    uint64_t spikeCount;                     // Total spike count for performance metrics
    float averageFiringRate;                 // Computed average firing rate
    bool plasticityEnabled;                  // Whether plasticity is active
    
    // Thread safety
    mutable std::mutex implMutex;            // For thread-safe operations
    std::atomic<bool> isProcessing;          // Atomic flag for concurrent access
    
    // Configuration
    Config neuronConfig;                     // Neuron-specific configuration
    
    // Debugging and monitoring
    bool debugMode;                          // Enable debug output
    uint64_t debugOutputInterval;            // Debug output interval
    uint64_t debugStepCount;                 // Debug step counter
    
    // Performance tracking
    std::chrono::steady_clock::time_point lastUpdate; // For performance metrics
    std::chrono::duration<float> processingTime;      // Processing time tracking
    
    // Memory management
    size_t maxSpikeHistory;                  // Maximum spike history size
    float memoryEfficiency;                 // Memory efficiency metric
    
    // Validation flags
    bool voltageClamped;                    // Whether voltage was clamped
    bool noiseApplied;                       // Whether noise was applied
    bool temperatureApplied;                 // Whether temperature effects were applied
    
    // Constructor
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             noiseLevel(0.0f), synapticInput(0.0f),
             nmdaConductance(0.0f), gabaConductance(0.0f), modulatoryConductance(0.0f),
             nmdaMgBlock(0.0f), gabaReversalPotential(-80.0f), modulatoryReversalPotential(0.0f),
             nmdaRelativeMgBlock(1.0f), nmdaActivation(0.0f), nmdaDeactivation(0.0f), nmdaSaturation(0.0f),
             gabaActivation(0.0f), gabaDeactivation(0.0f), modulatoryActivation(0.0f), modulatoryDeactivation(0.0f),
             naConductance(120.0f), kConductance(36.0f), naActivation(0.05f), kActivation(0.01f),
             naActivationRate(0.1f), naInactivationRate(0.1f), naRecoveryRate(0.01f),
             kActivationRate(0.1f), kInactivationRate(0.1f),
             naTimeConstant(1.0f), kTimeConstant(1.0f),
             spikeCount(0), averageFiringRate(0.0f),
             plasticityEnabled(true), debugMode(false), debugOutputInterval(1000),
             debugStepCount(0), maxSpikeHistory(1000), memoryEfficiency(1.0f),
             voltageClamped(false), noiseApplied(false), temperatureApplied(false),
             isProcessing(false) {}
};

// NeuronModelFactory implementation
inline std::unique_ptr<NeuronModel> NeuronModelFactory::createModel(ModelType type, const Config& config) {
    switch (type) {
        case ModelType::BasicLIF:
            return std::make_unique<EnhancedLIFModel>();
        case ModelType::EnhancedLIF:
            return std::make_unique<EnhancedLIFModel>();
        case ModelType::Izhikevich:
            return std::make_unique<IzhikevichModel>();
        case ModelType::HindmarshRose:
            return std::make_unique<HindmarshRoseModel>();
        case ModelType::HodgkinHuxley:
            return std::make_unique<EnhancedLIFModel>(); // Placeholder
        case ModelType::AdaptiveExponential:
            return std::make_unique<EnhancedLIFModel>(); // Placeholder
        default:
            return std::make_unique<EnhancedLIFModel>();
    }
}

inline NeuronModelFactory::ModelType NeuronModelFactory::parseModelType(const std::string& name) {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "basiclif" || lowerName == "lif") return ModelType::BasicLIF;
    if (lowerName == "enhancedlif") return ModelType::EnhancedLIF;
    if (lowerName == "izhikevich") return ModelType::Izhikevich;
    if (lowerName == "hindmarshrose") return ModelType::HindmarshRose;
    if (lowerName == "hodgkinhuxley") return ModelType::HodgkinHuxley;
    if (lowerName == "adaptiveexponential") return ModelType::AdaptiveExponential;
    
    return ModelType::EnhancedLIF; // Default
}

inline std::string NeuronModelFactory::modelTypeToString(ModelType type) {
    switch (type) {
        case ModelType::BasicLIF: return "BasicLIF";
        case ModelType::EnhancedLIF: return "EnhancedLIF";
        case ModelType::Izhikevich: return "Izhikevich";
        case ModelType::HindmarshRose: return "HindmarshRose";
        case ModelType::HodgkinHuxley: return "HodgkinHuxley";
        case ModelType::AdaptiveExponential: return "AdaptiveExponential";
        default: return "EnhancedLIF";
    }
}

inline std::vector<std::string> NeuronModelFactory::getAvailableModelNames() {
    return {"BasicLIF", "EnhancedLIF", "Izhikevich", "HindmarshRose", "HodgkinHuxley", "AdaptiveExponential"};
}

// Neuron implementation with enhanced features
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->modelType = NeuronModelFactory::ModelType::EnhancedLIF;
    
    // Create the default model
    pImpl->model = NeuronModelFactory::createModel(pImpl->modelType, pImpl->neuronConfig);
    
    // Initialize default parameters from configuration
    pImpl->neuronConfig.set("model.type", "EnhancedLIF");
    pImpl->neuronConfig.set("model.temperature", 25.0f);
    pImpl->neuronConfig.set("model.noise.level", 0.0f);
    pImpl->neuronConfig.set("model.noise.type", "white");
    pImpl->neuronConfig.set("model.energy.max_budget", 100.0f);
    pImpl->neuronConfig.set("model.energy.metabolic_rate", 1.0f);
    pImpl->neuronConfig.set("model.plasticity.enabled", true);
    pImpl->neuronConfig.set("model.debug.mode", false);
    pImpl->neuronConfig.set("model.debug.interval", 1000);
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->type = type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->model ? pImpl->model->getMembranePotential() : pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("V", static_cast<double>(potential));
    } else {
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("V", pImpl->model->getParameter("V") + delta);
    } else {
        pImpl->state.membranePotential += delta;
    }
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->model ? pImpl->model->getThreshold() : pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("threshold", static_cast<double>(threshold));
    } else {
        pImpl->state.threshold = threshold;
    }
}

bool Neuron::isFiring() const {
    return pImpl->model ? pImpl->model->getFiringState() == FiringState::Active :
           pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->model ? pImpl->model->getFiringState() == FiringState::Refractory :
           pImpl->state.firingState == FiringState::Refractory;
}

void Neuron::setFiringState(FiringState state) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        // Models typically handle their own firing state
        // This is a convenience override
    } else {
        pImpl->state.firingState = state;
    }
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("refractory_period", static_cast<double>(steps));
    } else {
        pImpl->state.refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->model ? static_cast<uint32_t>(pImpl->model->getParameter("refractory_period")) :
           pImpl->state.refractoryPeriod;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->model ? pImpl->model->getFiringRate() : pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("firing_rate", static_cast<double>(rate));
    } else {
        pImpl->state.firingRate = rate;
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("gLeak", static_cast<double>(conductance));
    } else {
        pImpl->state.leakConductance = conductance;
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->model ? static_cast<MembranePotential>(pImpl->model->getParameter("gLeak")) :
           pImpl->state.leakConductance;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("V_rest", static_cast<double>(potential));
    } else {
        pImpl->state.restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->model ? static_cast<MembranePotential>(pImpl->model->getParameter("V_rest")) :
           pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("V_reset", static_cast<double>(potential));
    } else {
        pImpl->state.resetPotential = potential;
    }
}

bool Neuron::checkThreshold() const {
    return pImpl->model ? pImpl->model->getMembranePotential() >= pImpl->model->getThreshold() :
           pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->model ? pImpl->model->getParameter("last_spike_time") : pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("I_input", pImpl->model->getParameter("I_input") + amplitude);
    } else {
        pImpl->synapticInput += amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("I_input", pImpl->model->getParameter("I_input") - amplitude);
    } else {
        pImpl->synapticInput -= amplitude;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("modulatory_input", pImpl->model->getParameter("modulatory_input") + amplitude);
    } else {
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("I_input", pImpl->model->getParameter("I_input") + current);
    } else {
        pImpl->synapticInput += current;
    }
}

void Neuron::receiveNMDAInput(MembranePotential amplitude, float voltage, float MgBlockFactor) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    // NMDA receptor dynamics
    float MgBlock = 1.0f / (1.0f + std::exp(0.062f * (-voltage)) * std::exp(0.062f * MgBlockFactor));
    pImpl->nmdaRelativeMgBlock = MgBlock;
    
    // Voltage-dependent NMDA activation
    float activation = 0.5f * (1.0f + std::tanh((voltage + 60.0f) / 20.0f));
    pImpl->nmdaActivation = activation;
    
    // Saturation of NMDA current
    float saturation = 1.0f / (1.0f + std::exp(-(amplitude - 2.0f) / 0.5f));
    pImpl->nmdaSaturation = saturation;
    
    // Calculate NMDA conductance
    float conductance = amplitude * pImpl->nmdaActivation * pImpl->nmdaRelativeMgBlock * pImpl->nmdaSaturation;
    pImpl->nmdaConductance = conductance;
    
    // Apply NMDA current contribution
    MembranePotential nmdaCurrent = conductance * (voltage - 0.0f); // NMDA reversal potential ~0mV
    pImpl->synapticInput += nmdaCurrent;
    
    pImpl->voltageClamped = false;
}

void Neuron::receiveGABAInput(MembranePotential amplitude, float voltage) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    // GABA receptor activation
    float activation = 0.5f * (1.0f + std::tanh((voltage + 60.0f) / 20.0f));
    pImpl->gabaActivation = activation;
    
    // GABA deactivation
    pImpl->gabaDeactivation = std::exp(-amplitude * 0.01f);
    
    // Calculate GABA conductance
    float conductance = amplitude * pImpl->gabaActivation * pImpl->gabaDeactivation;
    pImpl->gabaConductance = conductance;
    
    // Apply GABA current contribution
    MembranePotential gabaCurrent = conductance * (voltage - pImpl->gabaReversalPotential);
    pImpl->synapticInput += gabaCurrent;
    
    pImpl->voltageClamped = false;
}

void Neuron::receiveModulatoryInputEnhanced(MembranePotential amplitude, Timestamp timestamp) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    // Modulatory synapse activation
    pImpl->modulatoryActivation = std::min(1.0f, pImpl->modulatoryActivation + amplitude * 0.01f);
    
    // Modulatory synapse deactivation
    pImpl->modulatoryDeactivation = std::exp(-amplitude * 0.05f);
    
    // Calculate modulatory conductance
    float conductance = amplitude * pImpl->modulatoryActivation * pImpl->modulatoryDeactivation;
    pImpl->modulatoryConductance = conductance;
    
    // Apply modulatory current contribution
    MembranePotential modulatoryCurrent = conductance * (voltage - pImpl->modulatoryReversalPotential);
    pImpl->synapticInput += modulatoryCurrent;
    
    // Update eligibility trace for plasticity
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + amplitude * 0.001f);
    
    pImpl->voltageClamped = false;
}

void Neuron::clearTotalCurrent() {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->setParameter("I_input", 0.0);
    } else {
        pImpl->synapticInput = 0.0f;
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->recordSpike(timestamp);
    } else {
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > pImpl->maxSpikeHistory) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->model ? pImpl->model->getSpikeHistory() : pImpl->spikeHistory;
}

void Neuron::clearSpikeHistory() {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        // Model-specific implementation
    } else {
        pImpl->spikeHistory.clear();
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
    if (pImpl->model) {
        pImpl->model->setPlasticityEnabled(hebbian || stdp || rewardModulated);
    }
}

void Neuron::setRegionId(RegionId region) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->regionId = region;
    pImpl->ensembleManager.addNeuronToEnsemble(region.value, "region_" + std::to_string(region.value));
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->populationId = population;
    pImpl->ensembleManager.addNeuronToEnsemble(population.value, "population_" + std::to_string(population.value));
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

// Advanced neuron model step methods
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // For backward compatibility, use the enhanced LIF model
    if (!pImpl->model) {
        return step(currentTime);
    }
    
    bool fired = pImpl->model->update(currentTime, dt);
    
    // Update state for backward compatibility
    pImpl->state = NeuronState(); // Reset state for now
    
    // Update ensemble statistics
    pImpl->ensembleManager.updateEnsembleCoherence();
    
    // Update energy efficiency
    pImpl->energyModel.update(pImpl->model->getFiringRate(), 
                            pImpl->model->getMembranePotential(),
                            pImpl->temperatureController.getTemperature(),
                            dt);
    
    // Apply temperature scaling
    pImpl->temperatureController.setTemperature(pImpl->neuronConfig.getOr("model.temperature", 25.0f));
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default step with enhanced LIF model
    TimestepDuration dt = 0.001; // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    if (pImpl->model) {
        pImpl->model->reset();
    }
    
    // Reset state
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Reset energy and error correction
    pImpl->energyModel.update(0.0f, pImpl->state.membranePotential, 
                            pImpl->temperatureController.getTemperature(), 0.001f);
    pImpl->errorCorrection.reset();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    if (pImpl->model) {
        pImpl->model->initialize(rng);
    } else {
        // Original random initialization
        pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
        pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    }
    
    // Initialize advanced features
    pImpl->noiseLevel = pImpl->neuronConfig.getOr("model.noise.level", 0.0f);
    pImpl->noiseType = pImpl->neuronConfig.getOr("model.noise.type", "white");
    
    // Set temperature
    pImpl->temperatureController.setTemperature(pImpl->neuronConfig.getOr("model.temperature", 25.0f));
}

// Enhanced neuron model management
void Neuron::setModelType(NeuronModelFactory::ModelType type) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    pImpl->modelType = type;
    pImpl->model = NeuronModelFactory::createModel(type, pImpl->neuronConfig);
}

NeuronModelFactory::ModelType Neuron::getModelType() const {
    return pImpl->modelType;
}

void Neuron::configureAdvanced(const Config& config) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    pImpl->neuronConfig = config;
    
    // Apply configuration to model
    if (pImpl->model) {
        pImpl->model->configure(config);
    }
    
    // Apply noise configuration
    pImpl->noiseLevel = config.getOr("model.noise.level", 0.0f);
    pImpl->noiseType = config.getOr("model.noise.type", "white");
    
    // Apply temperature configuration
    pImpl->temperatureController.setTemperature(config.getOr("model.temperature", 25.0f));
}

void Neuron::enableDebugMode(bool enable, uint64_t interval) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    pImpl->debugMode = enable;
    pImpl->debugOutputInterval = interval;
    
    if (pImpl->model) {
        pImpl->model->configure(pImpl->neuronConfig);
    }
}

void Neuron::recordDebugInfo() {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    pImpl->debugStepCount++;
    
    if (pImpl->debugMode && pImpl->debugStepCount % pImpl->debugOutputInterval == 0) {
        // Debug output - would typically go to a debug logger
        // For now, we'll just track that debug info was recorded
        pImpl->debugStepCount = 0;
    }
}

float Neuron::getEnergyCost() const {
    return pImpl->energyModel.getCurrentEnergyCost();
}

float Neuron::getEfficiency() const {
    return pImpl->energyModel.getEfficiency();
}

float Neuron::getTemperature() const {
    return pImpl->temperatureController.getTemperature();
}

void Neuron::setTemperature(float temp) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->temperatureController.setTemperature(temp);
    if (pImpl->model) {
        pImpl->model->setTemperature(temp);
    }
}

void Neuron::applyErrorCorrection(float voltageError, Timestamp timestamp) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->errorCorrection.applyVoltageCorrection(
        pImpl->state.membranePotential, voltageError, 0.1f);
    
    if (pImpl->model) {
        pImpl->model->applyErrorCorrection(voltageError, timestamp);
    }
}

void Neuron::updateEnsemble(NeuronId neuronId, const std::string& ensembleName) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->ensembleManager.addNeuronToEnsemble(neuronId, ensembleName);
}

void Neuron::setPlasticity(float eligibility, float rewardError) {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    pImpl->eligibilityTrace = eligibility;
    pImpl->rewardPredictionError = rewardError;
    
    if (pImpl->model) {
        pImpl->model->applyPlasticity(eligibility, rewardError, 0.0);
    }
}

// Thread safety and performance methods
void Neuron::lock() {
    pImpl->implMutex.lock();
    pImpl->isProcessing = true;
}

void Neuron::unlock() {
    pImpl->implMutex.unlock();
    pImpl->isProcessing = false;
}

bool Neuron::isLocked() const {
    return pImpl->isProcessing;
}

// Performance metrics
float Neuron::getProcessingTime() const {
    return pImpl->processingTime.count();
}

uint64_t Neuron::getSpikeCount() const {
    return pImpl->spikeCount;
}

float Neuron::getAverageFiringRate() const {
    return pImpl->averageFiringRate;
}

void Neuron::updatePerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->implMutex);
    
    pImpl->spikeCount = 0;
    pImpl->averageFiringRate = 0.0f;
    
    if (pImpl->model) {
        pImpl->spikeCount = pImpl->spikeHistory.size();
        pImpl->averageFiringRate = pImpl->model->getFiringRate();
    }
}

// Advanced neuron model methods - implementations for EnhancedLIFModel
EnhancedLIFModel::EnhancedLIFModel() {
    reset();
}

bool EnhancedLIFModel::update(Timestamp currentTime, TimestepDuration dt) {
    // Enhanced LIF dynamics with stochastic and temperature effects
    bool fired = false;
    
    // Apply temperature scaling to parameters
    float tempScaledTau = applyTemperatureScaling(tau, 2.0f, 25.0f);
    float tempScaledGLeak = applyTemperatureScaling(gLeak, 2.0f, 25.0f);
    
    // Apply noise
    applyNoise(dt);
    
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    MembranePotential& V = V;
    MembranePotential V_rest = restingPotential;
    MembranePotential V_reset = resetPotential;
    MembranePotential threshold = this->threshold;
    
    // Synaptic input contribution
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tempScaledTau;
    
    // Update membrane potential
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply adaptation
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * 0.01f;
        adaptationVariable *= 0.95f;
    }
    
    // Apply noise
    V += calculateNoiseContribution(currentTime);
    
    // Clamp voltage
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        firingState = FiringState::Active;
        lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        refractoryRemaining = refractoryPeriod;
        firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        adaptationVariable += 1.0f;
    } else {
        firingState = FiringState::Active;
    }
    
    // Update firing rate
    updateFiringRate(currentTime);
    
    // Update temperature sensitivity
    updateTemperatureSensitivity();
    
    // Update energy cost
    updateEnergyCost();
    
    // Clear synaptic input
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void EnhancedLIFModel::reset() {
    V = -70.0f;
    u = 0.0f;
    threshold = -55.0f;
    restingPotential = -70.0f;
    resetPotential = -70.0f;
    tau = 20.0f;
    C = 1.0f;
    gLeak = 10.0f;
    
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    refractoryRemaining = 0;
    adaptationVariable = 0.0f;
    temperature = 25.0f;
    
    noiseLevel = 0.0f;
    noiseType = "white";
    noiseAccumulator = 0.0f;
    
    energyCost = 0.0f;
    maxEnergyBudget = 100.0f;
    metabolicRate = 1.0f;
    efficiency = 1.0f;
    optimalFiringRate = 10.0f;
    minimumEnergy = 1.0f;
    
    plasticityEnabled = true;
    eligibilityTrace = 0.0f;
    rewardPredictionError = 0.0f;
    
    voltageErrorHistory = 0.0f;
    spikeTimingError = 0.0f;
    
    spikeHistory.clear();
    
    Q10 = 2.0f;
    tempBase = 25.0f;
    tempSensitivity = 0.5f;
}

void EnhancedLIFModel::initialize(RandomGenerator& rng) {
    // Random initialization with temperature sensitivity
    V = restingPotential + rng.uniformReal(-3.0f, 3.0f);
    threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    resetPotential = restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Temperature-sensitive time constant
    tau = 20.0f * applyTemperatureScaling(1.0f, Q10, tempBase);
    
    // Random refractory period
    refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    refractoryRemaining = 0;
    
    // Initial state
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    adaptationVariable = 0.0f;
    lastSpikeTime = -1.0f;
    
    // Random noise parameters
    noiseLevel = rng.uniformReal(0.0f, 0.5f);
    noiseType = (rng.bernoulli(0.5) ? "white" : "pink");
    
    // Energy parameters
    maxEnergyBudget = rng.uniformReal(50.0f, 200.0f);
    metabolicRate = rng.uniformReal(0.5f, 2.0f);
    
    efficiency = 1.0f;
    optimalFiringRate = rng.uniformReal(5.0f, 20.0f);
    minimumEnergy = rng.uniformReal(0.5f, 2.0f);
    
    spikeHistory.clear();
}

void EnhancedLIFModel::configure(const Config& config) {
    this->config = config;
    
    // Apply configuration parameters
    if (config.has("model.gLeak")) {
        gLeak = config.getOr("model.gLeak", gLeak);
    }
    
    if (config.has("model.tau")) {
        tau = config.getOr("model.tau", tau);
    }
    
    if (config.has("model.C")) {
        C = config.getOr("model.C", C);
    }
    
    if (config.has("model.threshold")) {
        threshold = config.getOr("model.threshold", threshold);
    }
    
    if (config.has("model.restingPotential")) {
        restingPotential = config.getOr("model.restingPotential", restingPotential);
    }
    
    if (config.has("model.resetPotential")) {
        resetPotential = config.getOr("model.resetPotential", resetPotential);
    }
    
    // Temperature sensitivity
    if (config.has("model.temperature")) {
        setTemperature(config.getOr("model.temperature", 25.0f));
    }
    
    // Noise configuration
    if (config.has("model.noise.level")) {
        noiseLevel = config.getOr("model.noise.level", noiseLevel);
    }
    
    if (config.has("model.noise.type")) {
        noiseType = config.getOr("model.noise.type", noiseType);
    }
    
    // Energy configuration
    if (config.has("model.energy.max_budget")) {
        maxEnergyBudget = config.getOr("model.energy.max_budget", maxEnergyBudget);
    }
    
    if (config.has("model.energy.metabolic_rate")) {
        metabolicRate = config.getOr("model.energy.metabolic_rate", metabolicRate);
    }
    
    if (config.has("model.plasticity.enabled")) {
        plasticityEnabled = config.getOr("model.plasticity.enabled", plasticityEnabled);
    }
}

void EnhancedLIFModel::setParameter(const std::string& name, double value) {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "v") {
        V = static_cast<MembranePotential>(value);
    } else if (lowerName == "threshold") {
        threshold = static_cast<MembranePotential>(value);
    } else if (lowerName == "v_rest") {
        restingPotential = static_cast<MembranePotential>(value);
    } else if (lowerName == "v_reset") {
        resetPotential = static_cast<MembranePotential>(value);
    } else if (lowerName == "tau") {
        tau = static_cast<float>(value);
    } else if (lowerName == "c") {
        C = static_cast<float>(value);
    } else if (lowerName == "gleak") {
        gLeak = static_cast<MembranePotential>(value);
    } else if (lowerName == "noise_level") {
        noiseLevel = static_cast<float>(value);
    } else if (lowerName == "temperature") {
        setTemperature(static_cast<float>(value));
    }
}

double EnhancedLIFModel::getParameter(const std::string& name) const {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "v") return V;
    if (lowerName == "threshold") return threshold;
    if (lowerName == "v_rest") return restingPotential;
    if (lowerName == "v_reset") return resetPotential;
    if (lowerName == "tau") return tau;
    if (lowerName == "c") return C;
    if (lowerName == "gleak") return gLeak;
    if (lowerName == "noise_level") return noiseLevel;
    if (lowerName == "temperature") return temperature;
    if (lowerName == "firing_rate") return firingRate;
    if (lowerName == "adaptation_variable") return adaptationVariable;
    if (lowerName == "refractory_remaining") return refractoryRemaining;
    if (lowerName == "last_spike_time") return lastSpikeTime;
    
    return 0.0;
}

void EnhancedLIFModel::recordSpike(Timestamp timestamp) {
    spikeHistory.push_back(timestamp);
    if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
        spikeHistory.erase(spikeHistory.begin());
    }
}

float EnhancedLIFModel::getAverageFiringRate(Timestamp startTime, Timestamp endTime) const {
    if (spikeHistory.empty()) return 0.0f;
    
    size_t count = 0;
    for (const auto& spikeTime : spikeHistory) {
        if (spikeTime >= startTime && spikeTime <= endTime) {
            count++;
        }
    }
    
    float duration = static_cast<float>(endTime - startTime);
    if (duration <= 0.0f) return 0.0f;
    
    return (count * 1000.0f) / duration; // Convert to Hz
}

void EnhancedLIFModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {
    // Apply voltage error correction
    float correction = voltageError * 0.1f; // 10% correction factor
    V += correction;
    
    // Update error history
    voltageErrorHistory = voltageError;
    spikeTimingError = 0.0f; // Reset for this correction
}

void EnhancedLIFModel::setTemperature(float temp) {
    temperature = temp;
    
    // Apply temperature scaling to parameters
    float tempFactor = getTemperatureDependentRate(1.0f, Q10, tempBase);
    
    // Scale time constants and conductances based on temperature
    float scaleFactor = std::pow(Q10, (temperature - tempBase) / 10.0f);
    
    tau *= scaleFactor;
    gLeak *= scaleFactor;
    
    // Update temperature sensitivity
    updateTemperatureSensitivity();
}

void EnhancedLIFModel::updateTemperatureSensitivity() {
    // Update model parameters based on temperature
    float tempDeviation = temperature - tempBase;
    
    // Calculate temperature sensitivity factor
    float tempSensitivityFactor = 1.0f + tempSensitivity * tempDeviation;
    
    // Apply to dynamic properties
    float scaleFactor = std::pow(Q10, tempDeviation / 10.0f);
    
    // Adjust firing rate based on temperature
    if (tempDeviation > 0.0f) {
        // Higher temperatures increase firing rate
        firingRate *= scaleFactor;
    } else {
        // Lower temperatures decrease firing rate
        firingRate /= scaleFactor;
    }
    
    // Update adaptation variable based on temperature
    if (temperature > tempBase + 5.0f) {
        // Warmer temperatures increase adaptation
        adaptationVariable *= 1.05f;
    } else if (temperature < tempBase - 5.0f) {
        // Cooler temperatures decrease adaptation
        adaptationVariable *= 0.95f;
    }
}

void EnhancedLIFModel::updateEnergyCost() {
    // Calculate energy cost based on firing rate and voltage
    float baseCost = 1.0f;
    float firingCost = firingRate * 0.1f;
    float voltageCost = std::abs(V - restingPotential) * 0.01f;
    
    // Temperature affects energy cost
    float tempFactor = getTemperatureDependentRate(1.0f, Q10, tempBase);
    
    energyCost = (baseCost + firingCost + voltageCost) * metabolicRate * tempFactor;
    
    // Update efficiency
    updateEnergyEfficiency();
}

void EnhancedLIFModel::updateEnergyEfficiency() {
    // Calculate efficiency based on firing rate vs optimal
    if (firingRate > 0.0f) {
        efficiency = 1.0f / (1.0f + std::abs(firingRate - optimalFiringRate) / optimalFiringRate);
    }
    
    // Ensure energy cost doesn't exceed budget
    if (energyCost > maxEnergyBudget) {
        energyCost = maxEnergyBudget;
    }
}

void EnhancedLIFModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {
    if (!plasticityEnabled) return;
    
    // STDP-like plasticity
    if (postTrace > preTrace) {
        // Post-synaptic spike occurred after pre-synaptic spike
        // Long-term potentiation
        adaptationVariable += 0.5f * (postTrace - preTrace);
    } else if (preTrace > postTrace) {
        // Pre-synaptic spike occurred after post-synaptic spike
        // Long-term depression
        adaptationVariable -= 0.5f * (preTrace - postTrace);
    }
    
    // Update eligibility trace
    eligibilityTrace = std::max(eligibilityTrace, std::abs(postTrace - preTrace));
}

void EnhancedLIFModel::setPlasticityEnabled(bool enabled) {
    plasticityEnabled = enabled;
}

void EnhancedLIFModel::applyNoise(float dt) {
    // Apply different types of noise
    if (noiseLevel <= 0.0f) return;
    
    if (noiseType == "white") {
        // White Gaussian noise
        float noise = static_cast<float>(std::normal_distribution<double>(0.0, noiseLevel)());
        noiseAccumulator = noise;
    } else if (noiseType == "pink") {
        // Pink (1/f) noise approximation
        noiseAccumulator = noiseAccumulator * 0.99f + static_cast<float>(std::normal_distribution<double>(0.0, noiseLevel)()) * 0.01f;
    } else if (noiseType == "brown") {
        // Brown noise (random walk)
        noiseAccumulator += static_cast<float>(std::normal_distribution<double>(0.0, noiseLevel)()) * dt;
        noiseAccumulator *= 0.99f;
    }
    
    // Clamp noise accumulator
    noiseAccumulator = std::clamp(noiseAccumulator, -10.0f, 10.0f);
}

float EnhancedLIFModel::calculateNoiseContribution(Timestamp currentTime) {
    // Calculate noise contribution to membrane potential
    float noiseAmplitude = noiseLevel * noiseAccumulator;
    
    // Temperature affects noise
    float tempFactor = getTemperatureDependentRate(1.0f, Q10, tempBase);
    
    return noiseAmplitude * tempFactor;
}

void EnhancedLIFModel::updateFiringRate(Timestamp currentTime) {
    // Update firing rate based on recent spike history
    if (!spikeHistory.empty()) {
        // Use last spike time to estimate instantaneous firing rate
        float lastSpike = spikeHistory.back();
        float timeSinceLastSpike = static_cast<float>(currentTime - lastSpike);
        
        if (timeSinceLastSpike > 0.0f) {
            firingRate = 1000.0f / timeSinceLastSpike; // Convert to Hz
        } else {
            firingRate = 1000.0f; // Maximum reasonable rate
        }
        
        // Smooth firing rate
        static float alpha = 0.1f; // Smoothing factor
        static float smoothedFiringRate = 0.0f;
        smoothedFiringRate = alpha * firingRate + (1.0f - alpha) * smoothedFiringRate;
        firingRate = smoothedFiringRate;
    } else {
        firingRate = 0.0f;
    }
}

void EnhancedLIFModel::clampVoltage() {
    // Clamp voltage to prevent numerical instability
    V = std::clamp(V, -100.0f, 50.0f);
}

// Izhikevich Model implementation
IzhikevichModel::IzhikevichModel() {
    reset();
}

bool IzhikevichModel::update(Timestamp currentTime, TimestepDuration dt) {
    // Izhikevich model: dV/dt = 0.04V^2 + 5V + 140 - u + I
    // du/dt = a(bV - u)
    
    bool fired = false;
    
    // Temperature effects
    float tempFactor = getTemperatureDependentRate(1.0f, 2.0f, 25.0f);
    
    // Update membrane potential
    V += (0.04f * V * V + 5.0f * V + 140.0f - u + Iinput) * dt * tempFactor;
    
    // Update recovery variable
    u += a * (b * V - u) * dt * tempFactor;
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        firingState = FiringState::Active;
        lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset
        V = c;
        u += d;
        
        // Update firing rate
        updateFiringRate(currentTime);
    } else {
        firingState = FiringState::Active;
    }
    
    // Apply noise
    float noise = static_cast<float>(std::normal_distribution<double>(0.0, noiseLevel)());
    V += noise * tempFactor;
    
    // Clamp voltage
    V = std::clamp(V, -100.0f, 50.0f);
    u = std::clamp(u, -100.0f, 100.0f);
    
    // Update energy cost
    updateEnergyCost();
    
    return fired;
}

void IzhikevichModel::reset() {
    V = -70.0f;
    u = -14.0f;
    threshold = 30.0f;
    
    // Izhikevich parameters
    a = 0.02f;
    b = 0.2f;
    c = -65.0f;
    d = 8.0f;
    Iinput = 0.0f;
    
    temperature = 25.0f;
    noise = 0.0f;
    
    plasticityEnabled = true;
    eligibilityTrace = 0.0f;
    
    energyCost = 0.0f;
    
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    spikeHistory.clear();
}

void IzhikevichModel::initialize(RandomGenerator& rng) {
    // Random initialization
    V = rng.uniformReal(-70.0f, -60.0f);
    u = rng.uniformReal(-15.0f, -10.0f);
    
    // Random Izhikevich parameters
    a = rng.uniformReal(0.01f, 0.05f);
    b = rng.uniformReal(0.1f, 0.5f);
    c = rng.uniformReal(-80.0f, -60.0f);
    d = rng.uniformReal(2.0f, 10.0f);
    
    temperature = rng.uniformReal(20.0f, 30.0f);
    noise = rng.uniformReal(0.0f, 0.1f);
    
    plasticityEnabled = true;
    Iinput = 0.0f;
    
    energyCost = 0.0f;
    
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    spikeHistory.clear();
}

void IzhikevichModel::configure(const Config& config) {
    // Apply configuration parameters
    if (config.has("model.izhikevich.a")) {
        a = config.getOr("model.izhikevich.a", a);
    }
    
    if (config.has("model.izhikevich.b")) {
        b = config.getOr("model.izhikevich.b", b);
    }
    
    if (config.has("model.izhikevich.c")) {
        c = config.getOr("model.izhikevich.c", c);
    }
    
    if (config.has("model.izhikevich.d")) {
        d = config.getOr("model.izhikevich.d", d);
    }
    
    if (config.has("model.noise.level")) {
        noise = config.getOr("model.noise.level", noise);
    }
}

void IzhikevichModel::setParameter(const std::string& name, double value) {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "v") V = static_cast<float>(value);
    else if (lowerName == "u") u = static_cast<float>(value);
    else if (lowerName == "a") a = static_cast<float>(value);
    else if (lowerName == "b") b = static_cast<float>(value);
    else if (lowerName == "c") c = static_cast<float>(value);
    else if (lowerName == "d") d = static_cast<float>(value);
    else if (lowerName == "Iinput") Iinput = static_cast<float>(value);
    else if (lowerName == "noise") noise = static_cast<float>(value);
    else if (lowerName == "temperature") setTemperature(static_cast<float>(value));
}

double IzhikevichModel::getParameter(const std::string& name) const {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "v") return V;
    if (lowerName == "u") return u;
    if (lowerName == "a") return a;
    if (lowerName == "b") return b;
    if (lowerName == "c") return c;
    if (lowerName == "d") return d;
    if (lowerName == "Iinput") return Iinput;
    if (lowerName == "noise") return noise;
    if (lowerName == "temperature") return temperature;
    if (lowerName == "firing_rate") return firingRate;
    if (lowerName == "last_spike_time") return lastSpikeTime;
    
    return 0.0;
}

void IzhikevichModel::recordSpike(Timestamp timestamp) {
    spikeHistory.push_back(timestamp);
    if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
        spikeHistory.erase(spikeHistory.begin());
    }
}

float IzhikevichModel::getAverageFiringRate(Timestamp startTime, Timestamp endTime) const {
    if (spikeHistory.empty()) return 0.0f;
    
    size_t count = 0;
    for (const auto& spikeTime : spikeHistory) {
        if (spikeTime >= startTime && spikeTime <= endTime) {
            count++;
        }
    }
    
    float duration = static_cast<float>(endTime - startTime);
    if (duration <= 0.0f) return 0.0f;
    
    return (count * 1000.0f) / duration;
}

void IzhikevichModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {
    // Apply voltage error correction
    float correction = voltageError * 0.05f; // Smaller correction for Izhikevich
    V += correction;
    
    // Update error history
    voltageErrorHistory = voltageError;
}

void IzhikevichModel::setTemperature(float temp) {
    temperature = temp;
}

void IzhikevichModel::updateEnergyCost() {
    // Izhikevich energy cost
    float baseCost = 1.0f;
    float firingCost = firingRate * 0.05f;
    float voltageCost = std::abs(V - c) * 0.005f;
    
    energyCost = (baseCost + firingCost + voltageCost) * std::pow(2.0f, (temperature - 25.0f) / 10.0f);
}

void IzhikevichModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {
    if (!plasticityEnabled) return;
    
    // STDP-like plasticity
    if (postTrace > preTrace) {
        // Post-synaptic spike occurred after pre-synaptic spike
        // Long-term potentiation
        d += 0.1f * (postTrace - preTrace);
    } else if (preTrace > postTrace) {
        // Pre-synaptic spike occurred after post-synaptic spike
        // Long-term depression
        d -= 0.1f * (preTrace - postTrace);
    }
    
    // Update eligibility trace
    eligibilityTrace = std::max(eligibilityTrace, std::abs(postTrace - preTrace));
}

void IzhikevichModel::setPlasticityEnabled(bool enabled) {
    plasticityEnabled = enabled;
}

void IzhikevichModel::updateFiringRate(Timestamp currentTime) {
    // Update firing rate based on spike history
    if (!spikeHistory.empty()) {
        float lastSpike = spikeHistory.back();
        float timeSinceLastSpike = static_cast<float>(currentTime - lastSpike);
        
        if (timeSinceLastSpike > 0.0f) {
            firingRate = 1000.0f / timeSinceLastSpike;
        } else {
            firingRate = 1000.0f;
        }
    } else {
        firingRate = 0.0f;
    }
}

// Hindmarsh-Rose Model implementation
HindmarshRoseModel::HindmarshRoseModel() {
    reset();
}

bool HindmarshRoseModel::update(Timestamp currentTime, TimestepDuration dt) {
    // Hindmarsh-Rose model: dx/dt = y - ax^3 + bx^2 - zx + I
    // dy/dt = cx - dy - 2y^2 + x
    // dz/dt = r(s(x + x_r) - z)
    
    bool fired = false;
    
    // Temperature effects
    float tempFactor = getTemperatureDependentRate(1.0f, 2.0f, 25.0f);
    
    // Update variables
    x += (y - a * x * x * x + b * x * x - z * x + Iinput) * dt * tempFactor;
    y += (c * x - d * y - 2.0f * y * y + x) * dt * tempFactor;
    z += r * (s * (x + x_r) - z) * dt * tempFactor;
    
    // Check for spike (based on threshold crossing)
    if (x >= threshold) {
        fired = true;
        firingState = FiringState::Active;
        lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Update firing rate
        updateFiringRate(currentTime);
    } else {
        firingState = FiringState::Active;
    }
    
    // Apply noise
    float noise = static_cast<float>(std::normal_distribution<double>(0.0, noiseLevel)());
    x += noise * tempFactor;
    
    // Clamp variables
    x = std::clamp(x, -100.0f, 100.0f);
    y = std::clamp(y, -100.0f, 100.0f);
    z = std::clamp(z, -100.0f, 100.0f);
    
    // Update energy cost
    updateEnergyCost();
    
    return fired;
}

void HindmarshRoseModel::reset() {
    x = -1.6f;
    y = -1.0f;
    z = 0.0f;
    
    // Hindmarsh-Rose parameters
    a = 1.0f;
    b = 3.0f;
    c = 1.0f;
    d = 5.0f;
    r = 0.006f;
    s = 4.0f;
    x_r = 0.7f;
    
    Iinput = 0.0f;
    
    temperature = 25.0f;
    noise = 0.0f;
    
    plasticityEnabled = true;
    eligibilityTrace = 0.0f;
    
    energyCost = 0.0f;
    
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    spikeHistory.clear();
}

void HindmarshRoseModel::initialize(RandomGenerator& rng) {
    // Random initialization
    x = rng.uniformReal(-2.0f, 2.0f);
    y = rng.uniformReal(-2.0f, 2.0f);
    z = rng.uniformReal(-1.0f, 1.0f);
    
    // Random Hindmarsh-Rose parameters
    a = rng.uniformReal(0.5f, 2.0f);
    b = rng.uniformReal(2.0f, 5.0f);
    c = rng.uniformReal(0.5f, 2.0f);
    d = rng.uniformReal(3.0f, 7.0f);
    r = rng.uniformReal(0.001f, 0.01f);
    s = rng.uniformReal(3.0f, 6.0f);
    x_r = rng.uniformReal(0.5f, 1.0f);
    
    temperature = rng.uniformReal(20.0f, 30.0f);
    noise = rng.uniformReal(0.0f, 0.1f);
    
    plasticityEnabled = true;
    Iinput = 0.0f;
    
    energyCost = 0.0f;
    
    firingState = FiringState::Resting;
    firingRate = 0.0f;
    spikeHistory.clear();
}

void HindmarshRoseModel::configure(const Config& config) {
    // Apply configuration parameters
    if (config.has("model.hindmarshrose.a")) {
        a = config.getOr("model.hindmarshrose.a", a);
    }
    
    if (config.has("model.hindmarshrose.b")) {
        b = config.getOr("model.hindmarshrose.b", b);
    }
    
    if (config.has("model.hindmarshrose.c")) {
        c = config.getOr("model.hindmarshrose.c", c);
    }
    
    if (config.has("model.hindmarshrose.d")) {
        d = config.getOr("model.hindmarshrose.d", d);
    }
    
    if (config.has("model.hindmarshrose.r")) {
        r = config.getOr("model.hindmarshrose.r", r);
    }
    
    if (config.has("model.hindmarshrose.s")) {
        s = config.getOr("model.hindmarshrose.s", s);
    }
    
    if (config.has("model.hindmarshrose.xr")) {
        x_r = config.getOr("model.hindmarshrose.xr", x_r);
    }
    
    if (config.has("model.noise.level")) {
        noise = config.getOr("model.noise.level", noise);
    }
}

void HindmarshRoseModel::setParameter(const std::string& name, double value) {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "x") x = static_cast<float>(value);
    else if (lowerName == "y") y = static_cast<float>(value);
    else if (lowerName == "z") z = static_cast<float>(value);
    else if (lowerName == "a") a = static_cast<float>(value);
    else if (lowerName == "b") b = static_cast<float>(value);
    else if (lowerName == "c") c = static_cast<float>(value);
    else if (lowerName == "d") d = static_cast<float>(value);
    else if (lowerName == "r") r = static_cast<float>(value);
    else if (lowerName == "s") s = static_cast<float>(value);
    else if (lowerName == "xr") x_r = static_cast<float>(value);
    else if (lowerName == "Iinput") Iinput = static_cast<float>(value);
    else if (lowerName == "noise") noise = static_cast<float>(value);
    else if (lowerName == "temperature") setTemperature(static_cast<float>(value));
}

double HindmarshRoseModel::getParameter(const std::string& name) const {
    std::string lowerName = name;
    for (char& c : lowerName) c = std::tolower(c);
    
    if (lowerName == "x") return x;
    if (lowerName == "y") return y;
    if (lowerName == "z") return z;
    if (lowerName == "a") return a;
    if (lowerName == "b") return b;
    if (lowerName == "c") return c;
    if (lowerName == "d") return d;
    if (lowerName == "r") return r;
    if (lowerName == "s") return s;
    if (lowerName == "xr") return x_r;
    if (lowerName == "Iinput") return Iinput;
    if (lowerName == "noise") return noise;
    if (lowerName == "temperature") return temperature;
    if (lowerName == "firing_rate") return firingRate;
    if (lowerName == "last_spike_time") return lastSpikeTime;
    
    return 0.0;
}

void HindmarshRoseModel::recordSpike(Timestamp timestamp) {
    spikeHistory.push_back(timestamp);
    if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
        spikeHistory.erase(spikeHistory.begin());
    }
}

float HindmarshRoseModel::getAverageFiringRate(Timestamp startTime, Timestamp endTime) const {
    if (spikeHistory.empty()) return 0.0f;
    
    size_t count = 0;
    for (const auto& spikeTime : spikeHistory) {
        if (spikeTime >= startTime && spikeTime <= endTime) {
            count++;
        }
    }
    
    float duration = static_cast<float>(endTime - startTime);
    if (duration <= 0.0f) return 0.0f;
    
    return (count * 1000.0f) / duration;
}

void HindmarshRoseModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {
    // Apply voltage error correction
    float correction = voltageError * 0.08f;
    x += correction;
    
    // Update error history
    voltageErrorHistory = voltageError;
}

void HindmarshRoseModel::setTemperature(float temp) {
    temperature = temp;
}

void HindmarshRoseModel::updateEnergyCost() {
    // Hindmarsh-Rose energy cost
    float baseCost = 1.0f;
    float firingCost = firingRate * 0.03f;
    float voltageCost = std::abs(x) * 0.001f;
    
    energyCost = (baseCost + firingCost + voltageCost) * std::pow(2.0f, (temperature - 25.0f) / 10.0f);
}

void HindmarshRoseModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {
    if (!plasticityEnabled) return;
    
    // STDP-like plasticity for Hindmarsh-Rose
    if (postTrace > preTrace) {
        // Post-synaptic spike occurred after pre-synaptic spike
        // Long-term potentiation
        c += 0.05f * (postTrace - preTrace);
    } else if (preTrace > postTrace) {
        // Pre-synaptic spike occurred after post-synaptic spike
        // Long-term depression
        c -= 0.05f * (preTrace - postTrace);
    }
    
    // Update eligibility trace
    eligibilityTrace = std::max(eligibilityTrace, std::abs(postTrace - preTrace));
}

void HindmarshRoseModel::setPlasticityEnabled(bool enabled) {
    plasticityEnabled = enabled;
}

void HindmarshRoseModel::updateFiringRate(Timestamp currentTime) {
    // Update firing rate based on spike history
    if (!spikeHistory.empty()) {
        float lastSpike = spikeHistory.back();
        float timeSinceLastSpike = static_cast<float>(currentTime - lastSpike);
        
        if (timeSinceLastSpike > 0.0f) {
            firingRate = 1000.0f / timeSinceLastSpike;
        } else {
            firingRate = 1000.0f;
        }
    } else {
        firingRate = 0.0f;
    }
}

// Helper template for temperature scaling

template<typename T>
T TemperatureController::applyTemperatureScaling(T value, float Q10, float referenceTemp) const {
    float tempFactor = getTemperatureDependentRate(1.0f, Q10, referenceTemp);
    return static_cast<T>(value * tempFactor);
}

float TemperatureController::getTemperatureDependentRate(float rate, float Q10, float referenceTemp) const {
    float tempDeviation = currentTemperature - referenceTemp;
    return rate * std::pow(Q10, tempDeviation / 10.0f);
}

// Energy Efficiency Model implementations
EnergyEfficiencyModel::EnergyEfficiencyModel() {
    currentEnergyCost = 0.0f;
    maxEnergyBudget = 100.0f;
    metabolicRate = 1.0f;
    efficiency = 1.0f;
    optimalFiringRate = 10.0f;
    baselineEnergy = 1.0f;
}

void EnergyEfficiencyModel::update(float firingRate, float voltage, float temperature, float dt) {
    // Calculate energy cost
    float baseCost = baselineEnergy;
    float firingCost = firingRate * 0.1f;
    float voltageCost = std::abs(voltage) * 0.01f;
    
    // Temperature affects metabolic cost
    float tempFactor = std::pow(2.0f, (temperature - 25.0f) / 10.0f);
    
    currentEnergyCost = (baseCost + firingCost + voltageCost) * metabolicRate * tempFactor;
    
    // Calculate efficiency
    calculateEfficiency(firingRate, voltage, temperature);
}

void EnergyEfficiencyModel::calculateEfficiency(float firingRate, float voltage, float temperature) {
    // Efficiency based on firing rate vs optimal
    float rateEfficiency = 1.0f / (1.0f + std::abs(firingRate - optimalFiringRate) / optimalFiringRate);
    
    // Voltage efficiency
    float voltageEfficiency = 1.0f / (1.0f + std::abs(voltage) / 100.0f);
    
    // Temperature efficiency
    float tempDeviation = std::abs(temperature - 25.0f);
    float tempEfficiency = 1.0f / (1.0f + tempDeviation / 10.0f);
    
    // Overall efficiency
    efficiency = rateEfficiency * voltageEfficiency * tempEfficiency;
}

// Temperature Controller implementation
TemperatureController::TemperatureController() {
    currentTemperature = 25.0f;
    baseTemperature = 25.0f;
    tempSensitivity = 0.5f;
    ambientTemperature = 25.0f;
    lastUpdate = std::chrono::steady_clock::now();
}

void TemperatureController::setTemperature(float temp) {
    currentTemperature = temp;
    lastUpdate = std::chrono::steady_clock::now();
}

// Ensemble Manager implementation
EnsembleManager::EnsembleManager() {
    // Initialize with default ensembles
    Ensemble regionEnsemble;
    regionEnsemble.name = "default_region";
    regionEnsemble.coherence = 0.5f;
    regionEnsemble.globalModulation = 1.0f;
    regionEnsemble.averageFiringRate = 0.0f;
    regionEnsemble.voltageVariance = 0.0f;
    ensembles["default_region"] = regionEnsemble;
}

void EnsembleManager::addNeuronToEnsemble(NeuronId neuronId, const std::string& ensembleName) {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    if (ensembles.find(ensembleName) == ensembles.end()) {
        Ensemble ensemble;
        ensemble.name = ensembleName;
        ensemble.coherence = 0.5f;
        ensemble.globalModulation = 1.0f;
        ensemble.averageFiringRate = 0.0f;
        ensemble.voltageVariance = 0.0f;
        ensembles[ensembleName] = ensemble;
    }
    
    ensembles[ensembleName].neuronIds.push_back(neuronId);
}

void EnsembleManager::removeNeuronFromEnsemble(NeuronId neuronId) {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    for (auto& pair : ensembles) {
        auto& ensemble = pair.second;
        ensemble.neuronIds.erase(
            std::remove(ensemble.neuronIds.begin(), ensemble.neuronIds.end(), neuronId),
            ensemble.neuronIds.end()
        );
    }
}

bool EnsembleManager::isNeuronInEnsemble(NeuronId neuronId, const std::string& ensembleName) const {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    if (ensembles.find(ensembleName) == ensembles.end()) {
        return false;
    }
    
    const auto& ensemble = ensembles.at(ensembleName);
    return std::find(ensemble.neuronIds.begin(), ensemble.neuronIds.end(), neuronId) != ensemble.neuronIds.end();
}

void EnsembleManager::updateEnsembleCoherence() {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    for (auto& pair : ensembles) {
        auto& ensemble = pair.second;
        
        if (ensemble.neuronIds.empty()) {
            ensemble.coherence = 0.0f;
            continue;
        }
        
        // Calculate coherence based on firing rate variance and voltage variance
        float firingRateVariance = 0.0f; // Would need actual neuron access
        float voltageVariance = ensemble.voltageVariance;
        
        ensemble.coherence = 1.0f / (1.0f + firingRateVariance + voltageVariance);
        
        // Update average firing rate (placeholder)
        ensemble.averageFiringRate = 10.0f; // Placeholder
    }
}

float EnsembleManager::getEnsembleCoherence(const std::string& ensembleName) const {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    if (ensembles.find(ensembleName) == ensembles.end()) {
        return 0.0f;
    }
    
    return ensembles.at(ensembleName).coherence;
}

std::vector<NeuronId> EnsembleManager::getEnsembleNeurons(const std::string& ensembleName) const {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    if (ensembles.find(ensembleName) == ensembles.end()) {
        return std::vector<NeuronId>();
    }
    
    return ensembles.at(ensembleName).neuronIds;
}

void EnsembleManager::applyGlobalModulation(const std::string& ensembleName, float modulation) {
    std::lock_guard<std::mutex> lock(ensembleMutex);
    
    if (ensembles.find(ensembleName) == ensembles.end()) {
        return;
    }
    
    ensembles[ensembleName].globalModulation *= modulation;
}

// Error Correction implementation
ErrorCorrection::ErrorCorrection() {
    voltageError = 0.0f;
    spikeTimingError = 0.0f;
    correctionApplied = 0.0f;
    voltageErrorAccum = 0.0f;
    maxCorrection = 5.0f;
    correctionDecay = 0.95f;
}

void ErrorCorrection::applyVoltageCorrection(float& voltage, float targetVoltage, float errorWeight) {
    float error = targetVoltage - voltage;
    
    // Accumulate error
    voltageErrorAccum += error * errorWeight;
    
    // Apply correction
    float correction = error * errorWeight;
    if (correction > maxCorrection) correction = maxCorrection;
    if (correction < -maxCorrection) correction = -maxCorrection;
    
    voltage += correction;
    correctionApplied += std::abs(correction);
    
    // Decay error
    voltageErrorAccum *= correctionDecay;
    
    // Update error metrics
    voltageError = error;
}

// Placeholder for missing methods and includes
// Additional implementations would continue here...

} // namespace nlm

// Additional includes and implementations would be added here...

// For now, provide placeholder implementations for missing methods
namespace nlm {

// Placeholder implementations for missing model methods
void NeuronModel::recordSpike(Timestamp timestamp) {}
const std::vector<Timestamp>& NeuronModel::getSpikeHistory() const { static const std::vector<Timestamp> empty; return empty; }
float NeuronModel::getAverageFiringRate(Timestamp startTime, Timestamp endTime) const { return 0.0f; }

// Additional placeholder implementations
void EnhancedLIFModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {}
void EnhancedLIFModel::setPlasticityEnabled(bool enabled) {}
void EnhancedLIFModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {}
void EnhancedLIFModel::setTemperature(float temp) {}
void EnhancedLIFModel::updateEnergyCost() {}
void EnhancedLIFModel::updateTemperatureSensitivity() {}
void EnhancedLIFModel::updateEnergyEfficiency() {}
void EnhancedLIFModel::clampVoltage() {}
void EnhancedLIFModel::applyNoise(float dt) {}
void EnhancedLIFModel::calculateNoiseContribution(Timestamp currentTime) {}
void EnhancedLIFModel::updateFiringRate(Timestamp currentTime) {}

void IzhikevichModel::updateFiringRate(Timestamp currentTime) {}
void IzhikevichModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {}
void IzhikevichModel::setPlasticityEnabled(bool enabled) {}
void IzhikevichModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {}
void IzhikevichModel::setTemperature(float temp) {}
void IzhikevichModel::updateEnergyCost() {}

void HindmarshRoseModel::updateFiringRate(Timestamp currentTime) {}
void HindmarshRoseModel::applyPlasticity(float preTrace, float postTrace, Timestamp timestamp) {}
void HindmarshRoseModel::setPlasticityEnabled(bool enabled) {}
void HindmarshRoseModel::applyErrorCorrection(float voltageError, Timestamp timestamp) {}
void HindmarshRoseModel::setTemperature(float temp) {}
void HindmarshRoseModel::updateEnergyCost() {}

} // namespace nlm
