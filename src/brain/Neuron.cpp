// Enhanced Neuron Models for NLM
// Multiple neuron model types beyond LIF
// Hodgkin-Huxley, Izhikevich, and other models

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Forward declarations
class NeuronModel;

// Neuron model factory for creating different model types
class NeuronModelFactory {
public:
    static std::unique_ptr<NeuronModel> createModel(NeuronType type, const NeuronConfig& config);
    static std::vector<std::string> getAvailableModelNames();
};

// Base neuron model interface
class NeuronModel {
public:
    virtual ~NeuronModel() = default;
    
    virtual void initialize(RandomGenerator& rng, const NeuronConfig& config) = 0;
    virtual bool update(double timestep, const std::vector<float>& inputs) = 0;
    virtual void reset() = 0;
    virtual float getMembranePotential() const = 0;
    virtual float getFiringRate() const = 0;
    virtual bool hasSpiked() const = 0;
    virtual float getAdaptationVariable() const = 0;
    virtual void setExternalInput(float input) = 0;
    virtual std::string getModelName() const = 0;
    virtual NeuronModelType getModelType() const = 0;
};

// Neuron configuration structure
struct NeuronConfig {
    // LIF parameters
    float restingPotential = -70.0f;
    float threshold = -55.0f;
    float resetPotential = -70.0f;
    float membraneTimeConstant = 20.0f;
    float refractoryPeriod = 2.0f;
    float adaptationStrength = 0.0f;
    
    // HH model parameters
    float maxSodiumConductance = 120.0f;
    float minPotassiumConductance = 36.0f;
    float leakageConductance = 0.3f;
    float sodiumReversal = 50.0f;
    float potassiumReversal = -77.0f;
    
    // Izhikevich model parameters
    float a = 0.02f;
    float b = 0.2f;
    float c = -65.0f;
    float d = 8.0f;
    
    // Model selection
    NeuronModelType modelType = NeuronModelType::LIF;
    std::string modelName = "LIF";
    
    // Simulation parameters
    float timestep = 0.001f;
    bool useAdaptiveIntegration = false;
    
    // Noise parameters
    float noiseLevel = 0.0f;
    float noiseCorrelation = 0.0f;
};

// LIF (Leaky Integrate-and-Fire) Model
class LIFModel : public NeuronModel {
public:
    LIFModel() = default;
    
    void initialize(RandomGenerator& rng, const NeuronConfig& config) override {
        // Initialize LIF parameters
        v_ = config.restingPotential + rng.uniformReal(-3.0f, 3.0f);  // Small initial noise
        v_reset_ = config.resetPotential;
        threshold_ = config.threshold;
        tau_ = config.membraneTimeConstant;
        refractoryRemaining_ = 0;
        adaptation_ = 0.0f;
        externalInput_ = 0.0f;
        spiked_ = false;
        
        // Set up random number generators
        noise_ = config.noiseLevel;
        if (noise_ > 0.0f) {
            noiseGenerator_.setSeed(rng.getSeed());
        }
    }
    
    bool update(double timestep, const std::vector<float>& inputs) override {
        if (refractoryRemaining_ > 0) {
            // During refractory period, hold at reset potential
            refractoryRemaining_ -= static_cast<uint32_t>(timestep * 1000.0);  // Convert ms to s
            if (refractoryRemaining_ <= 0) {
                refractoryRemaining_ = 0;
                // Start integrating again
                float I_total = externalInput_;
                if (!inputs.empty()) {
                    for (float input : inputs) {
                        I_total += input;
                    }
                }
                // LIF integration with Euler method
                float dv = (config_.restingPotential - v_) / tau_ + I_total / C_;
                v_ += dv * static_cast<float>(timestep);
                
                // Apply spike-frequency adaptation
                if (config_.adaptationStrength > 0.0f) {
                    adaptation_ += config_.adaptationStrength;
                    v_ -= adaptation_ * 0.01f;
                    adaptation_ *= 0.95f;  // Decay adaptation
                }
                
                // Clamp membrane potential
                v_ = std::clamp(v_, -100.0f, 50.0f);
                
                // Check for spike
                if (v_ >= threshold_) {
                    spiked_ = true;
                    v_ = v_reset_;
                    refractoryRemaining_ = static_cast<uint32_t>(config_.refractoryPeriod * 1000.0);
                    return true;
                }
            }
            return false;
        }
        
        // Normal integration phase
        float I_total = externalInput_;
        if (!inputs.empty()) {
            for (float input : inputs) {
                I_total += input;
            }
        }
        
        // LIF integration with Euler method
        float dv = (config_.restingPotential - v_) / tau_ + I_total / C_;
        v_ += dv * static_cast<float>(timestep);
        
        // Add noise if enabled
        if (noise_ > 0.0f) {
            float noise = noiseGenerator_.uniformReal(-noise_, noise_);
            v_ += noise * static_cast<float>(timestep);
        }
        
        // Apply spike-frequency adaptation
        if (config_.adaptationStrength > 0.0f) {
            adaptation_ += config_.adaptationStrength;
            v_ -= adaptation_ * 0.01f;
            adaptation_ *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential
        v_ = std::clamp(v_, -100.0f, 50.0f);
        
        // Check for spike
        if (v_ >= threshold_) {
            spiked_ = true;
            v_ = v_reset_;
            refractoryRemaining_ = static_cast<uint32_t>(config_.refractoryPeriod * 1000.0);
            return true;
        }
        
        return false;
    }
    
    void reset() override {
        v_ = config_.restingPotential;
        refractoryRemaining_ = 0;
        adaptation_ = 0.0f;
        externalInput_ = 0.0f;
        spiked_ = false;
    }
    
    float getMembranePotential() const override { return v_; }
    float getFiringRate() const override { return firingRate_; }
    bool hasSpiked() const override { return spiked_; }
    float getAdaptationVariable() const override { return adaptation_; }
    void setExternalInput(float input) override { externalInput_ = input; }
    std::string getModelName() const override { return "LIF"; }
    NeuronModelType getModelType() const override { return NeuronModelType::LIF; }
    
    void setConfig(const NeuronConfig& config) { config_ = config; }
    
private:
    // LIF parameters
    float v_ = -70.0f;                    // Membrane potential
    float v_reset_ = -70.0f;              // Reset potential
    float threshold_ = -55.0f;            // Spike threshold
    float tau_ = 20.0f;                   // Membrane time constant
    float C_ = 1.0f;                      // Membrane capacitance
    uint32_t refractoryRemaining_ = 0;   // Remaining refractory time
    float adaptation_ = 0.0f;             // Spike-frequency adaptation variable
    float externalInput_ = 0.0f;          // External input current
    bool spiked_ = false;                 // Spike flag for current timestep
    float firingRate_ = 0.0f;             // Firing rate
    
    // Noise parameters
    float noise_ = 0.0f;
    RandomGenerator noiseGenerator_;
    
    // Configuration
    NeuronConfig config_;
};

// Hodgkin-Huxley (HH) Model
class HHModel : public NeuronModel {
public:
    HHModel() = default;
    
    void initialize(RandomGenerator& rng, const NeuronConfig& config) override {
        // Initialize HH parameters
        v_ = config.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        
        // Ion channel gating variables
        m_ = calculateAlphaM(v_) / (calculateAlphaM(v_) + calculateBetaM(v_));
        h_ = calculateAlphaH(v_) / (calculateAlphaH(v_) + calculateBetaH(v_));
        n_ = calculateAlphaN(v_) / (calculateAlphaN(v_) + calculateBetaN(v_));
        
        // Time derivatives
        dm_dt_ = 0.0f;
        dh_dt_ = 0.0f;
        dn_dt_ = 0.0f;
        
        // Reset flags
        spiked_ = false;
        externalInput_ = 0.0f;
        
        // Set up random noise
        noise_ = config.noiseLevel;
        if (noise_ > 0.0f) {
            noiseGenerator_.setSeed(rng.getSeed());
        }
        
        config_ = config;
    }
    
    bool update(double timestep, const std::vector<float>& inputs) override {
        if (refractoryRemaining_ > 0) {
            // During refractory period
            refractoryRemaining_ -= static_cast<uint32_t>(timestep * 1000.0);
            if (refractoryRemaining_ <= 0) {
                refractoryRemaining_ = 0;
            }
            return false;
        }
        
        // Calculate ion channel currents
        float I_Na = config_.maxSodiumConductance * m_ * m_ * m_ * h_ * (v_ - config_.sodiumReversal);
        float I_K = config_.minPotassiumConductance * n_ * n_ * n_ * n_ * (v_ - config_.potassiumReversal);
        float I_L = config_.leakageConductance * (v_ - config_.restingPotential);
        
        // Calculate total input current
        float I_total = externalInput_;
        if (!inputs.empty()) {
            for (float input : inputs) {
                I_total += input;
            }
        }
        
        // Calculate time derivatives
        float I_ion = I_Na + I_K + I_L;
        float dv_dt = (I_total - I_ion) / C_;
        
        dm_dt_ = alphaM(v_) * (1.0f - m_) - betaM(v_) * m_;
        dh_dt_ = alphaH(v_) * (1.0f - h_) - betaH(v_) * h_;
        dn_dt_ = alphaN(v_) * (1.0f - n_) - betaN(v_) * n_;
        
        // Update gating variables
        float dt = static_cast<float>(timestep) * 1000.0f;  // Convert to ms
        m_ += dm_dt_ * dt;
        h_ += dh_dt_ * dt;
        n_ += dn_dt_ * dt;
        
        // Clamp gating variables
        m_ = std::clamp(m_, 0.0f, 1.0f);
        h_ = std::clamp(h_, 0.0f, 1.0f);
        n_ = std::clamp(n_, 0.0f, 1.0f);
        
        // Update membrane potential
        v_ += dv_dt * dt;
        
        // Add noise if enabled
        if (noise_ > 0.0f) {
            float noise = noiseGenerator_.uniformReal(-noise_, noise_);
            v_ += noise * static_cast<float>(timestep);
        }
        
        // Check for spike threshold
        if (v_ >= config_.threshold) {
            spiked_ = true;
            v_ = config_.resetPotential;
            refractoryRemaining_ = static_cast<uint32_t>(config_.refractoryPeriod * 1000.0);
            return true;
        }
        
        return false;
    }
    
    void reset() override {
        v_ = config_.restingPotential;
        m_ = calculateAlphaM(v_) / (calculateAlphaM(v_) + calculateBetaM(v_));
        h_ = calculateAlphaH(v_) / (calculateAlphaH(v_) + calculateBetaH(v_));
        n_ = calculateAlphaN(v_) / (calculateAlphaN(v_) + calculateBetaN(v_));
        dm_dt_ = 0.0f;
        dh_dt_ = 0.0f;
        dn_dt_ = 0.0f;
        refractoryRemaining_ = 0;
        externalInput_ = 0.0f;
        spiked_ = false;
    }
    
    float getMembranePotential() const override { return v_; }
    float getFiringRate() const override { return firingRate_; }
    bool hasSpiked() const override { return spiked_; }
    float getAdaptationVariable() const override { return 0.0f; }  // HH doesn't have adaptation
    void setExternalInput(float input) override { externalInput_ = input; }
    std::string getModelName() const override { return "Hodgkin-Huxley"; }
    NeuronModelType getModelType() const override { return NeuronModelType::HodgkinHuxley; }
    
    void setConfig(const NeuronConfig& config) { config_ = config; }
    
private:
    // HH parameters
    float v_ = -70.0f;                    // Membrane potential
    float m_ = 0.05f;                     // Sodium activation
    float h_ = 0.6f;                      // Sodium inactivation
    float n_ = 0.32f;                     // Potassium activation
    float dm_dt_ = 0.0f;                  // Time derivative of m
    float dh_dt_ = 0.0f;                  // Time derivative of h
    float dn_dt_ = 0.0f;                  // Time derivative of n
    uint32_t refractoryRemaining_ = 0;   // Remaining refractory time
    float externalInput_ = 0.0f;          // External input current
    bool spiked_ = false;                 // Spike flag
    float firingRate_ = 0.0f;             // Firing rate
    
    // Noise parameters
    float noise_ = 0.0f;
    RandomGenerator noiseGenerator_;
    
    // Membrane capacitance
    static constexpr float C_ = 1.0f;      // μF/cm²
    
    // Helper functions for HH gating variables
    float alphaM(float v) const { return (0.1f * (v + 40.0f)) / (1.0f - exp(-(v + 40.0f) / 10.0f)); }
    float betaM(float v) const { return 4.0f * exp(-(v + 65.0f) / 18.0f); }
    float alphaH(float v) const { return 0.07f * exp(-(v + 65.0f) / 20.0f); }
    float betaH(float v) const { return 1.0f / (exp(-(v + 35.0f) / 10.0f) + 1.0f); }
    float alphaN(float v) const { return (0.01f * (v + 55.0f)) / (1.0f - exp(-(v + 55.0f) / 10.0f)); }
    float betaN(float v) const { return 0.125f * exp(-(v + 65.0f) / 80.0f); }
    
    float exp(float x) const { return std::exp(x); }
    
    // Configuration
    NeuronConfig config_;
};

// Izhikevich Model
class IzhikevichModel : public NeuronModel {
public:
    IzhikevichModel() = default;
    
    void initialize(RandomGenerator& rng, const NeuronConfig& config) override {
        // Initialize Izhikevich parameters
        v_ = config.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        u_ = config_.b * v_;  // Initialize adaptation variable
        
        // Set model-specific parameters
        a_ = config.a;
        b_ = config.b;
        c_ = config.c;
        d_ = config.d;
        
        // Reset flags
        spiked_ = false;
        externalInput_ = 0.0f;
        
        // Set up random noise
        noise_ = config.noiseLevel;
        if (noise_ > 0.0f) {
            noiseGenerator_.setSeed(rng.getSeed());
        }
        
        config_ = config;
    }
    
    bool update(double timestep, const std::vector<float>& inputs) override {
        if (refractoryRemaining_ > 0) {
            // During refractory period
            refractoryRemaining_ -= static_cast<uint32_t>(timestep * 1000.0);
            if (refractoryRemaining_ <= 0) {
                refractoryRemaining_ = 0;
            }
            return false;
        }
        
        // Izhikevich model equations
        float I_total = externalInput_;
        if (!inputs.empty()) {
            for (float input : inputs) {
                I_total += input;
            }
        }
        
        // Add noise if enabled
        if (noise_ > 0.0f) {
            float noise = noiseGenerator_.uniformReal(-noise_, noise_);
            I_total += noise;
        }
        
        // Update membrane potential and adaptation variable
        float dv = v_ * (1.0f - a_) * v_ - u_ + b_ + I_total;
        float du = d_ * (a_ * v_ - u_);
        
        v_ += dv * static_cast<float>(timestep) * 1000.0f;
        u_ += du * static_cast<float>(timestep) * 1000.0f;
        
        // Check for spike
        if (v_ >= config_.threshold) {
            spiked_ = true;
            v_ = c_;  // Reset potential
            u_ = u_ + d_;  // Update adaptation variable
            refractoryRemaining_ = static_cast<uint32_t>(config_.refractoryPeriod * 1000.0);
            return true;
        }
        
        return false;
    }
    
    void reset() override {
        v_ = config_.restingPotential;
        u_ = config_.b * v_;
        spiked_ = false;
        externalInput_ = 0.0f;
        refractoryRemaining_ = 0;
    }
    
    float getMembranePotential() const override { return v_; }
    float getFiringRate() const override { return firingRate_; }
    bool hasSpiked() const override { return spiked_; }
    float getAdaptationVariable() const override { return u_; }
    void setExternalInput(float input) override { externalInput_ = input; }
    std::string getModelName() const override { return "Izhikevich"; }
    NeuronModelType getModelType() const override { return NeuronModelType::Izhikevich; }
    
    void setConfig(const NeuronConfig& config) { config_ = config; }
    
private:
    // Izhikevich parameters
    float v_ = -65.0f;                    // Membrane potential
    float u_ = 0.0f;                      // Adaptation variable
    float a_ = 0.02f;                     // Sensitivity to subthreshold fluctuations
    float b_ = 0.2f;                      // Neural adaptation
    float c_ = -65.0f;                    // After-spike reset
    float d_ = 8.0f;                      // After-spike adaptation
    uint32_t refractoryRemaining_ = 0;   // Remaining refractory time
    float externalInput_ = 0.0f;          // External input current
    bool spiked_ = false;                 // Spike flag
    float firingRate_ = 0.0f;             // Firing rate
    
    // Noise parameters
    float noise_ = 0.0f;
    RandomGenerator noiseGenerator_;
    
    // Configuration
    NeuronConfig config_;
};

// Neuron Model Factory Implementation
std::unique_ptr<NeuronModel> NeuronModelFactory::createModel(NeuronType type, const NeuronConfig& config) {
    switch (type) {
        case NeuronType::Excitatory:
            // For excitatory neurons, use different models based on config
            switch (config.modelType) {
                case NeuronModelType::Izhikevich:
                    return std::make_unique<IzhikevichModel>();
                case NeuronModelType::HodgkinHuxley:
                    return std::make_unique<HHModel>();
                case NeuronModelType::LIF:
                default:
                    return std::make_unique<LIFModel>();
            }
        case NeuronType::Inhibitory:
            // For inhibitory neurons, use different models
            switch (config.modelType) {
                case NeuronModelType::HodgkinHuxley:
                    return std::make_unique<HHModel>();
                case NeuronModelType::Izhikevich:
                    return std::make_unique<IzhikevichModel>();
                case NeuronModelType::LIF:
                default:
                    return std::make_unique<LIFModel>();
            }
        case NeuronType::Modulatory:
            // Modulatory neurons often use Izhikevich model
            return std::make_unique<IzhikevichModel>();
        case NeuronType::Sensory:
            // Sensory neurons use LIF for efficiency
            return std::make_unique<LIFModel>();
        case NeuronType::Motor:
            // Motor neurons use LIF for real-time performance
            return std::make_unique<LIFModel>();
        case NeuronType::Internal:
        default:
            // Default to LIF for internal neurons
            return std::make_unique<LIFModel>();
    }
}

std::vector<std::string> NeuronModelFactory::getAvailableModelNames() {
    return {"LIF", "Hodgkin-Huxley", "Izhikevich"};
}

// Neuron implementation with enhanced model support
class Neuron::Impl {
public:
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Enhanced neuron model support
    std::unique_ptr<NeuronModel> neuronModel;
    NeuronConfig neuronConfig;
    
    // LIF parameters (for backward compatibility)
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
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

void Neuron::setModelType(NeuronType type) {
    pImpl->type = type;
    // Reinitialize model with new type if model exists
    if (pImpl->neuronModel && pImpl->pImpl->pImpl) {
        pImpl->pImpl->pImpl->neuronModel = NeuronModelFactory::createModel(type, pImpl->pImpl->pImpl->neuronConfig);
    }
}

void Neuron::setModelParameters(const NeuronConfig& config) {
    pImpl->neuronConfig = config;
    // Reinitialize model with new parameters if model exists
    if (pImpl->neuronModel && pImpl->pImpl->pImpl) {
        pImpl->pImpl->pImpl->neuronModel = NeuronModelFactory::createModel(pImpl->type, config);
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        pImpl->spikeHistory.push_back(currentTime);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update firing rate
        pImpl->state.firingRate = 1000.0f / dt;  // spikes per second
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::stepLIFWithModel(Timestamp currentTime, TimestepDuration dt, const std::vector<float>& inputs) {
    // Use enhanced neuron model if available
    if (pImpl->neuronModel) {
        bool spiked = pImpl->neuronModel->update(dt, inputs);
        if (spiked) {
            pImpl->spikeHistory.push_back(currentTime);
            if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
                pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
            }
            pImpl->state.firingRate = 1000.0f / dt;  // spikes per second
        }
    } else {
        // Fallback to LIF implementation
        stepLIF(currentTime, dt);
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

MembranePotential Neuron::getMembranePotential() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->getMembranePotential();
    }
    return pImpl->state.membranePotential;
}

FiringRate Neuron::getFiringRate() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->getFiringRate();
    }
    return pImpl->state.firingRate;
}

bool Neuron::hasSpiked() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->hasSpiked();
    }
    return (pImpl->state.firingState == FiringState::Active);
}

float Neuron::getAdaptationVariable() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->getAdaptationVariable();
    }
    return pImpl->state.adaptationVariable;
}

void Neuron::setExternalInput(float input) {
    if (pImpl->neuronModel) {
        pImpl->neuronModel->setExternalInput(input);
    }
}

std::string Neuron::getModelName() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->getModelName();
    }
    return "LIF";
}

NeuronModelType Neuron::getModelType() const {
    if (pImpl->neuronModel) {
        return pImpl->neuronModel->getModelType();
    }
    return NeuronModelType::LIF;
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->totalCurrent += current;
    pImpl->state.membranePotential += current / Impl::MEMBRANE_CAPACITANCE;
}

void Neuron::receiveExcitatoryInput(MembranePotential input) {
    pImpl->synapticInput += input;
}

void Neuron::receiveInhibitoryInput(MembranePotential input) {
    pImpl->synapticInput -= input;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

FiringState Neuron::getFiringState() const {
    return pImpl->state.firingState;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
    if (pImpl->neuronModel) {
        pImpl->neuronModel->setExternalInput(0.0f);  // Reset external input
    }
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->totalCurrent = 0.0f;
    
    // Reset neuron model if it exists
    if (pImpl->neuronModel) {
        pImpl->neuronModel->reset();
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // LIF parameters
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->totalCurrent = 0.0f;
    
    // Initialize neuron model if it exists
    if (pImpl->neuronModel) {
        pImpl->neuronModel->initialize(rng, pImpl->neuronConfig);
    }
}

// Python bindings for neuron models
PYBIND11_MODULE(pynlm_neurons, m) {
    m.doc() = R"pbdoc(
        NLM Neuron Models Module
        ------------------------
        Advanced neuron models including LIF, Hodgkin-Huxley, and Izhikevich.
        Provides enhanced neuron dynamics and computational neuroscience capabilities.
    )pbdoc";
    
    py::enum_<NeuronModelType>(m, "NeuronModelType", R"pbdoc(Available neuron model types)pbdoc")
        .value("LIF", NeuronModelType::LIF)
        .value("HodgkinHuxley", NeuronModelType::HodgkinHuxley)
        .value("Izhikevich", NeuronModelType::Izhikevich)
        .export_values();
    
    py::class_<NeuronModel, std::unique_ptr<NeuronModel>>(m, "NeuronModel", R"pbdoc(Base neuron model class)pbdoc")
        .def("get_model_name", &NeuronModel::getModelName, R"pbdoc(Get model name)pbdoc")
        .def("get_model_type", &NeuronModel::getModelType, R"pbdoc(Get model type)pbdoc")
        .def("get_membrane_potential", &NeuronModel::getMembranePotential, R"pbdoc(Get membrane potential)pbdoc")
        .def("get_firing_rate", &NeuronModel::getFiringRate, R"pbdoc(Get firing rate)pbdoc")
        .def("has_spiked", &NeuronModel::hasSpiked, R"pbdoc(Check if neuron spiked)pbdoc")
        .def("get_adaptation_variable", &NeuronModel::getAdaptationVariable, R"pbdoc(Get adaptation variable)pbdoc")
        .def("set_external_input", &NeuronModel::setExternalInput, py::arg("input"), R"pbdoc(Set external input)pbdoc");
    
    py::class_<NeuronConfig>(m, "NeuronConfig", R"pbdoc(Neuron model configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("resting_potential", &NeuronConfig::restingPotential)
        .def_readwrite("threshold", &NeuronConfig::threshold)
        .def_readwrite("reset_potential", &NeuronConfig::resetPotential)
        .def_readwrite("membrane_time_constant", &NeuronConfig::membraneTimeConstant)
        .def_readwrite("refractory_period", &NeuronConfig::refractoryPeriod)
        .def_readwrite("adaptation_strength", &NeuronConfig::adaptationStrength)
        .def_readwrite("model_type", &NeuronConfig::modelType)
        .def_readwrite("model_name", &NeuronConfig::modelName);
    
    py::class_<NeuronModelFactory>(m, "NeuronModelFactory", R"pbdoc(Factory for creating neuron models)pbdoc")
        .def_static("create_model", &NeuronModelFactory::createModel, py::arg("type"), py::arg("config"), R"pbdoc(Create neuron model)pbdoc")
        .def_static("get_available_model_names", &NeuronModelFactory::getAvailableModelNames, R"pbdoc(Get available model names)pbdoc");
    
    // Convenience functions for creating models
    m.def("create_lif_model", []() {
        return std::make_unique<LIFModel>();
    }, R"pbdoc(Create LIF model)pbdoc");
    
    m.def("create_hh_model", []() {
        return std::make_unique<HHModel>();
    }, R"pbdoc(Create Hodgkin-Huxley model)pbdoc");
    
    m.def("create_izhikevich_model", []() {
        return std::make_unique<IzhikevichModel>();
    }, R"pbdoc(Create Izhikevich model)pbdoc");
}

} // namespace nlm
