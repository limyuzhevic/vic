// Advanced Neuron Model System
// Support for multiple neuron models with unified interface

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>
#include <any>
#include <optional>

namespace nlm {
namespace neuron {

// Neuron model types
enum class NeuronModelType {
    LIF,              // Leaky Integrate-and-Fire
    Izhikevich,       // Izhikevich model
    HindmarshRose,    // Hindmarsh-Rose model
    MorrisLecar,      // Morris-Lecar model
    HodgkinHuxley,    // Hodgkin-Huxley model
    Exponential,      // Exponential integrate-and-fire
    AdaptiveExponential, // Adaptive exponential integrate-and-fire
    Custom           // User-defined model
};

// Neuron model categories
enum class NeuronModelCategory {
    Simple,           // Computationally efficient, simple dynamics
    Biophysical,      // Biologically detailed models
    Hybrid,           // Combination of simple and complex
    Specialized,      // Designed for specific purposes
    Experimental      // Novel or research models
};

// Neuron state variables
struct NeuronState {
    // Membrane potential variables
    float v = -70.0f;              // Membrane potential
    float u = -14.0f;              // Recovery variable (for adaptive models)
    float w = 0.0f;                // Adaptation variable (for adaptive models)
    float n = 0.0f;                // Sodium gate variable (for Morris-Lecar)
    float h = 0.0f;                // Sodium inactivation variable (for HH model)
    float m = 0.0f;                // Sodium activation variable (for HH model)
    float x = 0.0f;                // Calcium activation variable (for Morris-Lecar)
    
    // Spike variables
    bool isSpiking = false;        // Spike flag
    float lastSpikeTime = -1000.0f; // Last spike time
    float spikeCount = 0.0f;        // Number of spikes
    
    // Synaptic variables
    float totalCurrent = 0.0f;     // Total synaptic current
    float synapticInput = 0.0f;     // Accumulated synaptic input
    
    // Conductance variables (for conductance-based models)
    float gNa = 0.0f;              // Sodium conductance
    float gK = 0.0f;               // Potassium conductance
    float gCa = 0.0f;              // Calcium conductance
    float gL = 0.0f;               // Leak conductance
    
    // Time constants and rates
    float tau_m = 0.1f;            // Membrane time constant
    float tau_h = 1.0f;            // Inactivation time constant
    float tau_n = 0.1f;            // Potassium activation time constant
    
    // Gating variables for HH model
    float alpha_m = 0.0f;
    float beta_m = 0.0f;
    float alpha_h = 0.0f;
    float beta_h = 0.0f;
    float alpha_n = 0.0f;
    float beta_n = 0.0f;
};

// Neuron model parameters
struct NeuronModelParams {
    NeuronModelType type;
    std::string name;
    std::string version;
    std::string description;
    NeuronModelCategory category;
    std::vector<std::string> tags;
    
    // Model-specific parameters
    std::unordered_map<std::string, std::any> parameters;
    
    // Simulation parameters
    float dt = 0.001f;            // Time step
    float vMin = -100.0f;         // Minimum membrane potential
    float vMax = 50.0f;           // Maximum membrane potential
    float threshold = -55.0f;     // Spike threshold
    float restingPotential = -70.0f; // Resting potential
    float refractoryPeriod = 2.0f; // Refractory period (ms)
    
    // Plasticity parameters
    bool supportsPlasticity = true;
    float maxWeightChange = 0.1f;
    float learningRate = 0.01f;
    
    // Performance characteristics
    float computationalCost = 1.0f; // Relative computational cost
    float memoryUsage = 1.0f;       // Relative memory usage
    bool supportsGPU = false;
    bool supportsParallel = false;
};

// Base neuron model interface
class INeuronModel {
public:
    virtual ~INeuronModel() = default;
    
    // Model metadata
    virtual std::string getName() const = 0;
    virtual NeuronModelType getType() const = 0;
    virtual std::string getDescription() const = 0;
    virtual NeuronModelCategory getCategory() const = 0;
    virtual std::vector<std::string> getTags() const = 0;
    virtual NeuronModelParams getParameters() const = 0;
    
    // Model configuration
    virtual bool configure(const NeuronModelParams& params) = 0;
    virtual bool updateParameters(const std::unordered_map<std::string, std::any>& params) = 0;
    virtual bool validateParameters() const = 0;
    
    // State management
    virtual void resetState() = 0;
    virtual NeuronState getState() const = 0;
    virtual void setState(const NeuronState& state) = 0;
    
    // Simulation
    virtual bool step(float currentTime) = 0;
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    
    // Event handling
    virtual void onPreSpike() {}
    virtual void onPostSpike() {}
    virtual void onInput(float current, float conductance = 1.0f) {}
    
    // Plasticity support
    virtual bool supportsPlasticity() const = 0;
    virtual float computePlasticityChange(const std::unordered_map<std::string, std::any>& context) = 0;
    virtual void updatePlasticity(float timestep) {}
    
    // Statistics
    virtual size_t getSpikeCount() const = 0;
    virtual float getFiringRate() const = 0;
    virtual float getAverageMembranePotential() const = 0;
    virtual std::unordered_map<std::string, std::any> getStatistics() const = 0;
    
    // Performance
    virtual float getComputationTime() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual void optimizeForPerformance() {}
};

// Typed neuron model template
template<typename T>
class NeuronModelBase : public INeuronModel {
public:
    NeuronModelBase(const std::string& name, NeuronModelType type, const std::string& description)
        : name_(name), type_(type), description_(description) {}
    
    std::string getName() const override { return name_; }
    NeuronModelType getType() const override { return type_; }
    std::string getDescription() const override { return description_; }
    
    bool configure(const NeuronModelParams& params) override {
        params_ = params;
        currentState_ = NeuronState();  // Reset state
        return true;
    }
    
    bool updateParameters(const std::unordered_map<std::string, std::any>& params) override {
        for (const auto& pair : params) {
            params_.parameters[pair.first] = pair.second;
        }
        return validateParameters();
    }
    
    virtual bool validateParameters() const override {
        // Basic validation
        if (params_.dt <= 0.0f) return false;
        if (params_.vMin >= params_.vMax) return false;
        if (params_.threshold <= params_.vMin || params_.threshold >= params_.vMax) return false;
        return true;
    }
    
    void resetState() override {
        currentState_ = NeuronState();
        currentState_.restingPotential = params_.restingPotential;
    }
    
    NeuronState getState() const override { return currentState_; }
    void setState(const NeuronState& state) override { currentState_ = state; }
    
    size_t getSpikeCount() const override { return static_cast<size_t>(currentState_.spikeCount); }
    float getFiringRate() const override {
        if (params_.dt > 0.0f) {
            return currentState_.spikeCount / params_.dt;
        }
        return 0.0f;
    }
    
    float getAverageMembranePotential() const override { return currentState_.v; }
    
    NeuronModelParams getParameters() const override { return params_; }
    
protected:
    std::string name_;
    NeuronModelType type_;
    std::string description_;
    NeuronModelParams params_;
    NeuronState currentState_;
    mutable std::mutex mutex_;
};

// LIF (Leaky Integrate-and-Fire) Model
class LIFNeuronModel : public NeuronModelBase<LIFNeuronModel> {
public:
    LIFNeuronModel() : NeuronModelBase<LIFNeuronModel>(
        "LIF Neuron",
        NeuronModelType::LIF,
        "Leaky Integrate-and-Fire neuron model"
    ) {
        // Set default parameters
        params_.name = "LIF Neuron";
        params_.version = "1.0.0";
        params_.category = NeuronModelCategory::Simple;
        params_.tags = {"lif", "simple", "efficient", "integrate_and_fire"};
        params_.dt = 0.001f;
        params_.vMin = -100.0f;
        params_.vMax = 50.0f;
        params_.threshold = -55.0f;
        params_.restingPotential = -70.0f;
        params_.refractoryPeriod = 2.0f;
        params_.supportsPlasticity = true;
        params_.maxWeightChange = 0.1f;
        params_.learningRate = 0.01f;
        params_.computationalCost = 1.0f;
        params_.memoryUsage = 1.0f;
        params_.supportsGPU = false;
        params_.supportsParallel = true;
        
        // Set default parameter values
        params_.parameters["tau"] = 20.0f;  // Membrane time constant (ms)
        params_.parameters["reset_potential"] = -75.0f; // Reset potential
        params_.parameters["leak_conductance"] = 0.1f; // Leak conductance
    }
    
    bool initialize() override {
        // Initialize model-specific variables
        return true;
    }
    
    bool shutdown() override {
        // Clean up resources
        return true;
    }
    
    bool step(float currentTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuronState& state = currentState_;
        
        // Handle refractory period
        if (state.refractoryRemaining > 0) {
            state.refractoryRemaining--;
            state.totalCurrent = 0.0f;  // No input during refractory period
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // LIF dynamics: dv/dt = (-v + I)/tau
        float tau = params_.parameters["tau"];
        float resetPotential = params_.parameters["reset_potential"];
        
        // Update membrane potential
        float dv = (-state.v + state.synapticInput) / tau;
        state.v += params_.dt * dv * 1000.0f;  // Convert to ms time scale
        
        // Check for spike
        if (state.v >= params_.threshold) {
            // Enter refractory period
            state.refractoryRemaining = params_.refractoryPeriod;
            state.firingState = FiringState::Active;
            
            // Record spike
            state.spikeCount++;
            state.lastSpikeTime = currentTime;
            
            // Reset membrane potential
            state.v = resetPotential;
            
            // Call pre-spike callback
            onPreSpike();
            
            return true;  // Spike occurred
        }
        
        // Call input callback
        onInput(state.synapticInput);
        
        // Clear synaptic input for next step
        state.synapticInput = 0.0f;
        
        return false;  // No spike
    }
    
    float computePlasticityChange(const std::unordered_map<std::string, std::any>& context) override {
        // Simple plasticity rule for LIF model
        float totalChange = 0.0f;
        
        // Can be overridden for specific plasticity rules
        if (context.find("reward") != context.end()) {
            float reward = std::any_cast<float>(context.at("reward"));
            totalChange = reward * params_.learningRate;
        }
        
        return totalChange * params_.maxWeightChange;
    }
    
    std::unordered_map<std::string, std::any> getStatistics() const override {
        std::unordered_map<std::string, std::any> stats = NeuronModelBase::getStatistics();
        stats["model_type"] = "LIF";
        stats["membrane_time_constant"] = params_.parameters["tau"];
        stats["refractory_period"] = params_.refractoryPeriod;
        return stats;
    }
};

// Izhikevich Neuron Model
class IzhikevichNeuronModel : public NeuronModelBase<IzhikevichNeuronModel> {
public:
    IzhikevichNeuronModel() : NeuronModelBase<IzhikevichNeuronModel>(
        "Izhikevich Neuron",
        NeuronModelType::Izhikevich,
        "Biophysically realistic Izhikevich neuron model"
    ) {
        // Set default parameters
        params_.name = "Izhikevich Neuron";
        params_.version = "1.0.0";
        params_.category = NeuronModelCategory::Biophysical;
        params_.tags = {"izhikevich", "biophysical", "detailed", "adaptive"};
        params_.dt = 0.005f;  // Smaller time step for stability
        params_.vMin = -100.0f;
        params_.vMax = 50.0f;
        params_.threshold = 30.0f;
        params_.restingPotential = -70.0f;
        params_.refractoryPeriod = 5.0f;
        params_.supportsPlasticity = true;
        params_.maxWeightChange = 0.05f;
        params_.learningRate = 0.01f;
        params_.computationalCost = 5.0f;
        params_.memoryUsage = 3.0f;
        params_.supportsGPU = true;
        params_.supportsParallel = true;
        
        // Izhikevich parameters
        params_.parameters["a"] = 0.02f;   // Recovery variable time constant
        params_.parameters["b"] = 0.2f;    // Recovery variable sensitivity
        params_.parameters["c"] = -65.0f;  // After-spike reset of membrane potential
        params_.parameters["d"] = 8.0f;   // After-spike reset of recovery variable
    }
    
    bool initialize() override {
        return true;
    }
    
    bool shutdown() override {
        return true;
    }
    
    bool step(float currentTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuronState& state = currentState_;
        
        // Handle refractory period
        if (state.refractoryRemaining > 0) {
            state.refractoryRemaining--;
            state.totalCurrent = 0.0f;
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // Get Izhikevich parameters
        float a = params_.parameters["a"];
        float b = params_.parameters["b"];
        float c = params_.parameters["c"];
        float d = params_.parameters["d"];
        
        // Izhikevich equations:
        // dv/dt = 0.04*v^2 + 5*v + 140 - u + I
        // du/dt = a*(b*v - u)
        
        // Calculate derivatives
        float dv = 0.04f * state.v * state.v + 5.0f * state.v + 140.0f - state.u + state.synapticInput;
        float du = a * (b * state.v - state.u);
        
        // Update state
        state.v += dv * params_.dt * 1000.0f;
        state.u += du * params_.dt * 1000.0f;
        
        // Check for spike
        if (state.v >= params_.threshold) {
            // Enter refractory period
            state.refractoryRemaining = params_.refractoryPeriod;
            state.firingState = FiringState::Active;
            
            // Record spike
            state.spikeCount++;
            state.lastSpikeTime = currentTime;
            
            // Reset state
            state.v = c;
            state.u += d;
            
            // Call pre-spike callback
            onPreSpike();
            
            return true;  // Spike occurred
        }
        
        // Call input callback
        onInput(state.synapticInput);
        
        // Clear synaptic input
        state.synapticInput = 0.0f;
        
        return false;  // No spike
    }
    
    float computePlasticityChange(const std::unordered_map<std::string, std::any>& context) override {
        // Adaptive plasticity rule for Izhikevich model
        float totalChange = 0.0f;
        
        // Activity-dependent plasticity
        float firingRate = getFiringRate();
        
        if (firingRate > 0.0f) {
            float plasticity = 1.0f / (1.0f + std::exp(-2.0f * (firingRate - 10.0f)));
            totalChange = plasticity * params_.learningRate;
        }
        
        return totalChange * params_.maxWeightChange;
    }
    
    std::unordered_map<std::string, std::any> getStatistics() const override {
        std::unordered_map<std::string, std::any> stats = NeuronModelBase::getStatistics();
        stats["model_type"] = "Izhikevich";
        stats["a"] = params_.parameters["a"];
        stats["b"] = params_.parameters["b"];
        stats["c"] = params_.parameters["c"];
        stats["d"] = params_.parameters["d"];
        return stats;
    }
};

// Hindmarsh-Rose Model
class HindmarshRoseNeuronModel : public NeuronModelBase<HindmarshRoseNeuronModel> {
public:
    HindmarshRoseNeuronModel() : NeuronModelBase<HindmarshRoseNeuronModel>(
        "Hindmarsh-Rose Neuron",
        NeuronModelType::HindmarshRose,
        "Complex oscillatory Hindmarsh-Rose neuron model"
    ) {
        // Set default parameters
        params_.name = "Hindmarsh-Rose Neuron";
        params_.version = "1.0.0";
        params_.category = NeuronModelCategory::Biophysical;
        params_.tags = {"hindmarsh_rose", "complex", "oscillatory", "bursting"};
        params_.dt = 0.005f;
        params_.vMin = -100.0f;
        params_.vMax = 50.0f;
        params_.threshold = -30.0f;
        params_.restingPotential = -1.6f;
        params_.refractoryPeriod = 10.0f;
        params_.supportsPlasticity = true;
        params_.maxWeightChange = 0.03f;
        params_.learningRate = 0.005f;
        params_.computationalCost = 8.0f;
        params_.memoryUsage = 5.0f;
        params_.supportsGPU = false;
        params_.supportsParallel = false;
        
        // Hindmarsh-Rose parameters
        params_.parameters["a"] = 1.0f;   // Time constant
        params_.parameters["b"] = 3.0f;   // Sensitivity
        params_.parameters["c"] = 1.0f;   // Behavior parameter
        params_.parameters["I"] = 3.5f;   // External current
    }
    
    bool initialize() override {
        return true;
    }
    
    bool shutdown() override {
        return true;
    }
    
    bool step(float currentTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuronState& state = currentState_;
        
        // Handle refractory period
        if (state.refractoryRemaining > 0) {
            state.refractoryRemaining--;
            state.totalCurrent = 0.0f;
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // Get parameters
        float a = params_.parameters["a"];
        float b = params_.parameters["b"];
        float c = params_.parameters["c"];
        float I = params_.parameters["I"];
        
        // Hindmarsh-Rose equations:
        // dv/dt = y - a*v^2 + b*v + I
        // dy/dt = c - d*v - y
        
        float y = state.u;  // Using u as the y variable
        
        float dv = y - a * state.v * state.v + b * state.v + I;
        float dy = c - params_.parameters["d"] * state.v - y;
        
        // Update state
        state.v += dv * params_.dt * 1000.0f;
        state.u += dy * params_.dt * 1000.0f;
        
        // Check for spike
        if (state.v >= params_.threshold) {
            // Enter refractory period
            state.refractoryRemaining = params_.refractoryPeriod;
            state.firingState = FiringState::Active;
            
            // Record spike
            state.spikeCount++;
            state.lastSpikeTime = currentTime;
            
            // Reset state (optional)
            // state.v = c;
            
            // Call pre-spike callback
            onPreSpike();
            
            return true;  // Spike occurred
        }
        
        // Call input callback
        onInput(state.synapticInput);
        
        // Clear synaptic input
        state.synapticInput = 0.0f;
        
        return false;  // No spike
    }
    
    float computePlasticityChange(const std::unordered_map<std::string, std::any>& context) override {
        // Burst-dependent plasticity for Hindmarsh-Rose model
        float totalChange = 0.0f;
        
        // Calculate burst properties
        float firingRate = getFiringRate();
        float burstFrequency = 0.0f;
        
        // Simple burst detection
        if (currentState_.lastSpikeTime > 0.0f) {
            burstFrequency = 1000.0f / (currentTime - currentState_.lastSpikeTime);
        }
        
        // Burst-dependent plasticity
        if (burstFrequency > 50.0f) {  // High burst frequency
            totalChange = params_.learningRate * 0.1f;
        } else if (burstFrequency > 10.0f) {  // Medium burst frequency
            totalChange = params_.learningRate * 0.05f;
        } else {  // Low burst frequency
            totalChange = params_.learningRate * 0.01f;
        }
        
        return totalChange * params_.maxWeightChange;
    }
    
    std::unordered_map<std::string, std::any> getStatistics() const override {
        std::unordered_map<std::string, std::any> stats = NeuronModelBase::getStatistics();
        stats["model_type"] = "HindmarshRose";
        stats["a"] = params_.parameters["a"];
        stats["b"] = params_.parameters["b"];
        stats["c"] = params_.parameters["c"];
        stats["d"] = params_.parameters["d"];
        return stats;
    }
};

// Neuron Model Registry
class NeuronModelRegistry {
public:
    static NeuronModelRegistry& getInstance();
    
    // Model registration
    bool registerModel(std::unique_ptr<INeuronModel> model);
    bool unregisterModel(const std::string& name);
    
    // Model queries
    std::shared_ptr<INeuronModel> getModel(const std::string& name) const;
    std::vector<std::string> getModelNames() const;
    std::vector<std::string> getModelNamesByType(NeuronModelType type) const;
    std::vector<std::string> getModelNamesByCategory(NeuronModelCategory category) const;
    
    // Model discovery
    std::vector<std::string> discoverAvailableModels() const;
    bool loadModelFromFile(const std::string& filename);
    
    // Registry management
    void clear();
    size_t getModelCount() const;
    
    // Model creation by type
    std::unique_ptr<INeuronModel> createModel(NeuronModelType type,
                                               const std::unordered_map<std::string, std::any>& config = {});
    std::vector<std::unique_ptr<INeuronModel>> createModels(
        const std::vector<NeuronModelType>& types,
        const std::unordered_map<std::string, std::any>& config = {});
    
private:
    NeuronModelRegistry() = default;
    ~NeuronModelRegistry() = default;
    
    // Prevent copying
    NeuronModelRegistry(const NeuronModelRegistry&) = delete;
    NeuronModelRegistry& operator=(const NeuronModelRegistry&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuron Model Factory
class NeuronModelFactory {
public:
    static NeuronModelFactory& getInstance();
    
    // Factory methods
    std::unique_ptr<INeuronModel> createLIFModel(const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<INeuronModel> createIzhikevichModel(const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<INeuronModel> createHindmarshRoseModel(const std::unordered_map<std::string, std::any>& config = {});
    std::unique_ptr<INeuronModel> createCustomModel(const std::string& name,
                                                      NeuronModelType type,
                                                      const std::unordered_map<std::string, std::any>& config = {});
    
    // Model discovery
    std::vector<NeuronModelType> getAvailableModelTypes() const;
    NeuronModelParams getModelParameters(NeuronModelType type) const;
    std::vector<NeuronModelParams> getAllModelParameters() const;
    
    // Configuration utilities
    static NeuronModelParams createDefaultParams(NeuronModelType type);
    static bool validateParams(const NeuronModelParams& params);
    
private:
    NeuronModelFactory() = default;
    ~NeuronModelFactory() = default;
    
    // Prevent copying
    NeuronModelFactory(const NeuronModelFactory&) = delete;
    NeuronModelFactory& operator=(const NeuronModelFactory&) = delete;
};

// Neuron Model Utilities
namespace utils {
    // Model validation and comparison
    bool validateNeuronModel(const INeuronModel& model);
    bool compareModels(const INeuronModel& model1, const INeuronModel& model2);
    
    // Model simulation utilities
    std::vector<float> simulateModel(INeuronModel& model, float duration, float timestep = 0.001f);
    std::vector<std::vector<float>> simulateMultipleModels(
        const std::vector<std::unique_ptr<INeuronModel>>& models,
        float duration, float timestep = 0.001f);
    
    // Model analysis
    float calculateFiringRate(const std::vector<float>& membranePotentials, float timestep);
    float calculateBurstFrequency(const std::vector<float>& spikeTimes);
    std::vector<float> extractSpikeTimes(const std::vector<float>& membranePotentials, float threshold);
    
    // Model optimization
    void optimizeModelParameters(INeuronModel& model, const std::string& objective);
    
    // Thread-safe utilities
    template<typename Func>
    auto threadSafeModelOperation(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
    
    // Model serialization
    std::string serializeModel(const INeuronModel& model);
    bool deserializeModel(INeuronModel& model, const std::string& serialized);
}

} // namespace neuron
} // namespace nlm

// Neuron model registration macros
#define REGISTER_NEURON_MODEL(ModelClass, ModelName) \
    class ModelName##_registrar { \
    public: \
        ModelName##_registrar() { \
            nlm::neuron::NeuronModelRegistry::getInstance().registerModel(std::make_unique<ModelClass>()); \
        } \
    }; \
    static ModelName##_registrar ModelName##_instance;

namespace nlm {
namespace neuron {
    // Global registry and factory instances
    extern NeuronModelRegistry& getNeuronModelRegistry();
    extern NeuronModelFactory& getNeuronModelFactory();
    
    // Convenience functions
    template<typename... Args>
    std::unique_ptr<INeuronModel> createNeuronModel(NeuronModelType type, Args&&... args);
    
    std::vector<std::unique_ptr<INeuronModel>> createMultipleNeuronModels(
        const std::vector<NeuronModelType>& types);
}
} // namespace nlm

// Neuron model implementation
namespace nlm {
namespace neuron {

struct NeuronModelRegistry::Impl {
    std::unordered_map<std::string, std::shared_ptr<INeuronModel>> models;
    std::mutex mutex;
    bool initialized = false;
};

NeuronModelRegistry& NeuronModelRegistry::getInstance() {
    static NeuronModelRegistry instance;
    return instance;
}

bool NeuronModelRegistry::registerModel(std::unique_ptr<INeuronModel> model) {
    if (!model) {
        return false;
    }
    
    std::string name = model->getName();
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if model already exists
    if (pImpl->models.find(name) != pImpl->models.end()) {
        return false;  // Model already registered
    }
    
    // Initialize model
    if (!model->initialize()) {
        return false;
    }
    
    pImpl->models[name] = std::move(model);
    return true;
}

bool NeuronModelRegistry::unregisterModel(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->models.find(name);
    if (it == pImpl->models.end()) {
        return false;  // Model not found
    }
    
    // Shutdown model
    if (!it->second->shutdown()) {
        return false;
    }
    
    pImpl->models.erase(it);
    return true;
}

std::shared_ptr<INeuronModel> NeuronModelRegistry::getModel(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->models.find(name);
    return it != pImpl->models.end() ? it->second : nullptr;
}

std::vector<std::string> NeuronModelRegistry::getModelNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> names;
    names.reserve(pImpl->models.size());
    
    for (const auto& pair : pImpl->models) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> NeuronModelRegistry::getModelNamesByType(NeuronModelType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> names;
    
    for (const auto& pair : pImpl->models) {
        if (pair.second && pair.second->getType() == type) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

std::vector<std::string> NeuronModelRegistry::getModelNamesByCategory(NeuronModelCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> names;
    
    for (const auto& pair : pImpl->models) {
        if (pair.second && pair.second->getCategory() == category) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

std::vector<std::string> NeuronModelRegistry::discoverAvailableModels() const {
    std::vector<std::string> models;
    
    // Check registry for built-in models
    auto& registry = NeuronModelRegistry::getInstance();
    models = registry.getModelNames();
    
    // Could also scan filesystem for external models
    // For now, just return built-in models
    
    return models;
}

bool NeuronModelRegistry::loadModelFromFile(const std::string& filename) {
    // For now, placeholder implementation
    // Would need to implement model file loading from serialized format
    return false;
}

void NeuronModelRegistry::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->models) {
        if (pair.second) {
            pair.second->shutdown();
        }
    }
    
    pImpl->models.clear();
}

size_t NeuronModelRegistry::getModelCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->models.size();
}

std::unique_ptr<INeuronModel> NeuronModelRegistry::createModel(NeuronModelType type,
                                                                const std::unordered_map<std::string, std::any>& config) {
    // This would need to access the factory to create models
    // Placeholder implementation
    return nullptr;
}

std::vector<std::unique_ptr<INeuronModel>> NeuronModelRegistry::createModels(
    const std::vector<NeuronModelType>& types,
    const std::unordered_map<std::string, std::any>& config) {
    std::vector<std::unique_ptr<INeuronModel>> models;
    models.reserve(types.size());
    
    for (const auto& type : types) {
        auto model = createModel(type, config);
        if (model) {
            models.push_back(std::move(model));
        }
    }
    
    return models;
}

NeuronModelFactory& NeuronModelFactory::getInstance() {
    static NeuronModelFactory instance;
    return instance;
}

std::unique_ptr<INeuronModel> NeuronModelFactory::createLIFModel(const std::unordered_map<std::string, std::any>& config) {
    auto model = std::make_unique<LIFNeuronModel>();
    model->configure(createDefaultParams(NeuronModelType::LIF));
    return model;
}

std::unique_ptr<INeuronModel> NeuronModelFactory::createIzhikevichModel(const std::unordered_map<std::string, std::any>& config) {
    auto model = std::make_unique<IzhikevichNeuronModel>();
    model->configure(createDefaultParams(NeuronModelType::Izhikevich));
    return model;
}

std::unique_ptr<INeuronModel> NeuronModelFactory::createHindmarshRoseModel(const std::unordered_map<std::string, std::any>& config) {
    auto model = std::make_unique<HindmarshRoseNeuronModel>();
    model->configure(createDefaultParams(NeuronModelType::HindmarshRose));
    return model;
}

std::unique_ptr<INeuronModel> NeuronModelFactory::createCustomModel(const std::string& name,
                                                                   NeuronModelType type,
                                                                   const std::unordered_map<std::string, std::any>& config) {
    // Create custom model - this would need a factory system
    // For now, return a placeholder
    return nullptr;
}

std::vector<NeuronModelType> NeuronModelFactory::getAvailableModelTypes() const {
    return {NeuronModelType::LIF, NeuronModelType::Izhikevich, NeuronModelType::HindmarshRose};
}

NeuronModelParams NeuronModelFactory::getModelParameters(NeuronModelType type) const {
    return createDefaultParams(type);
}

std::vector<NeuronModelParams> NeuronModelFactory::getAllModelParameters() const {
    std::vector<NeuronModelParams> params;
    for (const auto& type : getAvailableModelTypes()) {
        params.push_back(createDefaultParams(type));
    }
    return params;
}

NeuronModelParams NeuronModelFactory::createDefaultParams(NeuronModelType type) {
    NeuronModelParams params;
    params.type = type;
    
    switch (type) {
        case NeuronModelType::LIF:
            params.name = "LIF Neuron";
            params.version = "1.0.0";
            params.description = "Leaky Integrate-and-Fire neuron model";
            params.category = NeuronModelCategory::Simple;
            params.tags = {"lif", "simple", "efficient"};
            params.dt = 0.001f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -55.0f;
            params.restingPotential = -70.0f;
            params.refractoryPeriod = 2.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.1f;
            params.learningRate = 0.01f;
            break;
            
        case NeuronModelType::Izhikevich:
            params.name = "Izhikevich Neuron";
            params.version = "1.0.0";
            params.description = "Biophysically realistic Izhikevich neuron model";
            params.category = NeuronModelCategory::Biophysical;
            params.tags = {"izhikevich", "biophysical", "adaptive"};
            params.dt = 0.005f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = 30.0f;
            params.restingPotential = -70.0f;
            params.refractoryPeriod = 5.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.05f;
            params.learningRate = 0.01f;
            break;
            
        case NeuronModelType::HindmarshRose:
            params.name = "Hindmarsh-Rose Neuron";
            params.version = "1.0.0";
            params.description = "Complex oscillatory Hindmarsh-Rose neuron model";
            params.category = NeuronModelCategory::Biophysical;
            params.tags = {"hindmarsh_rose", "complex", "oscillatory"};
            params.dt = 0.005f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -30.0f;
            params.restingPotential = -1.6f;
            params.refractoryPeriod = 10.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.03f;
            params.learningRate = 0.005f;
            break;
            
        case NeuronModelType::MorrisLecar:
            params.name = "Morris-Lecar Neuron";
            params.version = "1.0.0";
            params.description = "Morris-Lecar conductance-based neuron model";
            params.category = NeuronModelCategory::Biophysical;
            params.tags = {"morris_lecar", "conductance", "galvanic"};
            params.dt = 0.005f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -40.0f;
            params.restingPotential = -60.0f;
            params.refractoryPeriod = 8.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.04f;
            params.learningRate = 0.008f;
            break;
            
        case NeuronModelType::HodgkinHuxley:
            params.name = "Hodgkin-Huxley Neuron";
            params.version = "1.0.0";
            params.description = "Biophysical Hodgkin-Huxley neuron model";
            params.category = NeuronModelCategory::Biophysical;
            params.tags = {"hodgkin_huxley", "ion_channels", "conductance"};
            params.dt = 0.01f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -55.0f;
            params.restingPotential = -65.0f;
            params.refractoryPeriod = 5.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.02f;
            params.learningRate = 0.002f;
            break;
            
        case NeuronModelType::Exponential:
            params.name = "Exponential Neuron";
            params.version = "1.0.0";
            params.description = "Exponential integrate-and-fire neuron model";
            params.category = NeuronModelCategory::Simple;
            params.tags = {"exponential", "integrate_and_fire", "adaptive"};
            params.dt = 0.001f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -50.0f;
            params.restingPotential = -70.0f;
            params.refractoryPeriod = 3.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.08f;
            params.learningRate = 0.015f;
            break;
            
        case NeuronModelType::AdaptiveExponential:
            params.name = "Adaptive Exponential Neuron";
            params.version = "1.0.0";
            params.description = "Adaptive exponential integrate-and-fire neuron model";
            params.category = NeuronModelCategory::Hybrid;
            params.tags = {"adaptive_exponential", "integrate_and_fire", "adaptive"};
            params.dt = 0.001f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -45.0f;
            params.restingPotential = -70.0f;
            params.refractoryPeriod = 4.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.06f;
            params.learningRate = 0.012f;
            break;
            
        case NeuronModelType::Custom:
            params.name = "Custom Neuron";
            params.version = "1.0.0";
            params.description = "User-defined custom neuron model";
            params.category = NeuronModelCategory::Specialized;
            params.tags = {"custom", "user_defined", "flexible"};
            params.dt = 0.001f;
            params.vMin = -100.0f;
            params.vMax = 50.0f;
            params.threshold = -55.0f;
            params.restingPotential = -70.0f;
            params.refractoryPeriod = 2.0f;
            params.supportsPlasticity = true;
            params.maxWeightChange = 0.1f;
            params.learningRate = 0.01f;
            break;
    }
    
    return params;
}

bool NeuronModelFactory::validateParams(const NeuronModelParams& params) {
    return params.dt > 0.0f && params.vMin < params.vMax && params.threshold > params.vMin && params.threshold < params.vMax;
}

NeuronModelRegistry& getNeuronModelRegistry() {
    return NeuronModelRegistry::getInstance();
}

NeuronModelFactory& getNeuronModelFactory() {
    return NeuronModelFactory::getInstance();
}

template<typename... Args>
std::unique_ptr<INeuronModel> createNeuronModel(NeuronModelType type, Args&&... args) {
    // This would need to implement a generic model creation system
    // For now, return a placeholder
    return nullptr;
}

std::vector<std::unique_ptr<INeuronModel>> createMultipleNeuronModels(
    const std::vector<NeuronModelType>& types) {
    std::vector<std::unique_ptr<INeuronModel>> models;
    models.reserve(types.size());
    
    for (const auto& type : types) {
        auto model = NeuronModelFactory::getInstance().createLIFModel(); // Placeholder
        if (model) {
            models.push_back(std::move(model));
        }
    }
    
    return models;
}

} // namespace neuron
} // namespace nlm

// Neuron model registration
namespace nlm {
namespace neuron {
    // Register the built-in models
    REGISTER_NEURON_MODEL(LIFNeuronModel, LIFModel);
    REGISTER_NEURON_MODEL(IzhikevichNeuronModel, IzhikevichModel);
    REGISTER_NEURON_MODEL(HindmarshRoseNeuronModel, HindmarshRoseModel);
}
} // namespace nlm

// Neuron model convenience functions
namespace nlm {
namespace neuron {
    template<typename... Args>
    std::unique_ptr<INeuronModel> createNeuronModel(NeuronModelType type, Args&&... args) {
        return NeuronModelFactory::getInstance().createLIFModel(); // Placeholder
    }
}
} // namespace nlm