#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <chrono>
#include <atomic>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;
class PerformanceProfiler;

// Neuron performance metrics
struct NeuronPerformanceMetrics {
    // Identity
    NeuronId id{0};
    NeuronType type{NeuronType::Internal};
    
    // Spiking metrics
    size_t spikeCount{0};
    float firingRate{0.0f};
    float averageFiringRate{0.0f};
    float maxFiringRate{0.0f};
    float minFiringRate{0.0f};
    float firingRateVariance{0.0f};
    
    // Membrane potential metrics
    float membranePotential{0.0f};
    float averageMembranePotential{0.0f};
    float membranePotentialMin{0.0f};
    float membranePotentialMax{0.0f};
    float membranePotentialStdDev{0.0f};
    
    // Synaptic metrics
    float synapticInput{0.0f};
    float totalCurrent{0.0f};
    float synapticConductance{0.0f};
    
    // Adaptation metrics
    float lowPassAdaptation{0.0f};
    float highPassAdaptation{0.0f};
    float mTypeCurrent{0.0f};
    float aTypeCurrent{0.0f};
    float adaptationCurrent{0.0f};
    
    // Stochastic metrics
    float membraneNoise{0.0f};
    float noiseIntegrated{0.0f};
    float noiseVariance{0.0f};
    float noiseMean{0.0f};
    
    // Refractory metrics
    uint32_t refractoryRemaining{0};
    uint32_t refractoryRecovery{0};
    
    // STP metrics
    float facilitation{0.0f};
    float depression{0.0f};
    float shortTermPlasticity{0.0f};
    
    // Temporal metrics
    std::chrono::steady_clock::time_point lastSpikeTime;
    float timeSinceLastSpike{0.0f};
    float averageInterSpikeInterval{0.0f};
    float isiCoefficientOfVariation{0.0f};
    
    // State metrics
    float recoveryVariable{0.0f};
    float resetCurrent{0.0f};
    float postInhibitoryCurrent{0.0f};
    
    // Plasticity metrics
    float weightChange{0.0f};
    float weightChangeRate{0.0f};
    float plasticityStrength{0.0f};
    
    // Performance counters
    size_t updateCount{0};
    size_t spikeCountHistory{0};
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Custom metrics for profiling
    std::unordered_map<std::string, double> customMetrics;
    
    // Reset metrics
    void reset() {
        spikeCount = 0;
        firingRate = 0.0f;
        averageFiringRate = 0.0f;
        maxFiringRate = 0.0f;
        minFiringRate = 0.0f;
        firingRateVariance = 0.0f;
        
        membranePotential = 0.0f;
        averageMembranePotential = 0.0f;
        membranePotentialMin = 0.0f;
        membranePotentialMax = 0.0f;
        membranePotentialStdDev = 0.0f;
        
        synapticInput = 0.0f;
        totalCurrent = 0.0f;
        synapticConductance = 0.0f;
        
        lowPassAdaptation = 0.0f;
        highPassAdaptation = 0.0f;
        mTypeCurrent = 0.0f;
        aTypeCurrent = 0.0f;
        adaptationCurrent = 0.0f;
        
        membraneNoise = 0.0f;
        noiseIntegrated = 0.0f;
        noiseVariance = 0.0f;
        noiseMean = 0.0f;
        
        refractoryRemaining = 0;
        refractoryRecovery = 0;
        
        facilitation = 0.0f;
        depression = 0.0f;
        shortTermPlasticity = 0.0f;
        
        timeSinceLastSpike = 0.0f;
        averageInterSpikeInterval = 0.0f;
        isiCoefficientOfVariation = 0.0f;
        
        recoveryVariable = 0.0f;
        resetCurrent = 0.0f;
        postInhibitoryCurrent = 0.0f;
        
        weightChange = 0.0f;
        weightChangeRate = 0.0f;
        plasticityStrength = 0.0f;
        
        updateCount = 0;
        spikeCountHistory = 0;
        customMetrics.clear();
        
        lastUpdateTime = std::chrono::steady_clock::now();
    }
    
    // Update metrics
    void updateSpikingMetrics() {
        updateCount++;
        if (spikeCount > spikeCountHistory) {
            float deltaRate = static_cast<float>(spikeCount - spikeCountHistory) / (1.0f);  // 1ms assumption
            firingRate = deltaRate;
            
            if (averageFiringRate == 0.0f) {
                averageFiringRate = firingRate;
            } else {
                averageFiringRate = (averageFiringRate + firingRate) / 2.0f;
            }
            
            maxFiringRate = std::max(maxFiringRate, firingRate);
            minFiringRate = std::min(minFiringRate, firingRate);
            
            if (spikeCount > 1) {
                firingRateVariance = (firingRateVariance + (firingRate - averageFiringRate) * (firingRate - averageFiringRate)) / static_cast<float>(spikeCount - 1);
            }
            
            spikeCountHistory = spikeCount;
        }
    }
    
    void updateMembranePotentialMetrics(float potential) {
        membranePotential = potential;
        averageMembranePotential = (averageMembranePotential + potential) / 2.0f;
        membranePotentialMin = std::min(membranePotentialMin, potential);
        membranePotentialMax = std::max(membranePotentialMax, potential);
        
        if (membranePotentialStdDev == 0.0f) {
            membranePotentialStdDev = 0.0f;
        } else {
            membranePotentialStdDev = std::sqrt((membranePotentialStdDev * membranePotentialStdDev * (updateCount - 1) + (potential - averageMembranePotential) * (potential - averageMembranePotential)) / static_cast<float>(updateCount));
        }
    }
    
    void updateAdaptationMetrics() {
        adaptationCurrent = lowPassAdaptation + highPassAdaptation + mTypeCurrent + aTypeCurrent;
    }
    
    void updatePlasticityMetrics(float change, float strength) {
        weightChange = change;
        weightChangeRate = std::abs(change);
        plasticityStrength = strength;
    }
    
    std::unordered_map<std::string, double> getPerformanceSummary() const {
        std::unordered_map<std::string, double> summary;
        
        summary["spike_count"] = static_cast<double>(spikeCount);
        summary["firing_rate_hz"] = static_cast<double>(firingRate);
        summary["average_firing_rate_hz"] = static_cast<double>(averageFiringRate);
        summary["firing_rate_variance"] = static_cast<double>(firingRateVariance);
        summary["membrane_potential_mv"] = static_cast<double>(membranePotential);
        summary["membrane_potential_average_mv"] = static_cast<double>(averageMembranePotential);
        summary["membrane_potential_variance_mv2"] = static_cast<double>(membranePotentialStdDev * membranePotentialStdDev);
        summary["adaptation_current_na"] = static_cast<double>(adaptationCurrent);
        summary["plasticity_strength"] = static_cast<double>(plasticityStrength);
        summary["weight_change"] = static_cast<double>(weightChange);
        summary["weight_change_rate"] = static_cast<double>(weightChangeRate);
        summary["isi_coefficient_of_variation"] = static_cast<double>(isiCoefficientOfVariation);
        summary["spike_count_history"] = static_cast<double>(spikeCountHistory);
        
        return summary;
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
    
    // Configuration and validation
    void setMembraneCapacitance(float capacitance);  // nF
    float getMembraneCapacitance() const;
    void setTimeConstant(float tau);                // ms
    float getTimeConstant() const;
    void setRefractoryProperties(uint32_t absolute, uint32_t relative);
    void setAdaptationParameters(float lowPassTC, float highPassTC, 
                                 float lowPassHalf, float highPassHalf);
    void setStochasticProperties(float variance, float mean, float tau);
    void setSynapticProperties(float ampaTC, float nmdaTC, float gabaATC, float gabaBTC);
    
    // Performance monitoring
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    NeuronPerformanceMetrics getPerformanceMetrics() const;
    void recordPerformanceMetrics();
    std::unordered_map<std::string, double> getPerformanceSummary() const;
    
    // Advanced dynamics
    void updateAdaptationCurrents(float voltage);           // Update low-pass/high-pass/M-type/A-type currents
    void updateSynapticConductances(float dt);              // Update AMPA/NMDA/GABA conductances
    void updateStochasticDynamics(float dt, RandomGenerator& rng); // Ornstein-Uhlenbeck noise
    void updateRefractoryRecovery(float dt);                 // Exponential recovery from refractory period
    void updateShortTermPlasticity(float dt);                // STP facilitation/depression
    void updateResetCurrents();                              // Post-spike reset currents
    void updatePostInhibitoryCurrents();                     // PING currents
    
    // Biological model selection
    void setSFA(ModelType model);                             // Spike-frequency adaptation model
    void setMembraneModel(ModelType model);                   // Membrane model (leaky integrate-and-fire, etc.)
    void setSynapticModel(ModelType model);                   // Synaptic model
    
    // State queries for advanced dynamics
    float getAdaptationCurrents() const;                      // Combined adaptation current (nA)
    float getSynapticInput() const;                           // Total synaptic input (nA)
    float getNoiseInput() const;                             // Current noise input (mV)
    float getRefractoryRecovery() const;                      // Current recovery from refractory (0-1)
    float getSTPValue() const;                                // Current STP value
    
    // Statistical properties
    float getMembranePotentialVariance() const;
    float getFiringRateVariability() const;
    float getAdaptationIndex() const;                        // Measure of SFA strength
    
    // Backward compatibility
    float getMembraneCapacitanceLegacy() const;              // Returns 1.0f (for compatibility)
    float getTimeConstantLegacy() const;                     // Returns 20.0f (for compatibility)
    void setMembranePropertiesLegacy(float capacitance, float timeConstant); // For compatibility
    
    // Internal configuration
    struct NeuronConfig {
        // Membrane properties
        float membraneCapacitance = 1.0f;      // nF
        float timeConstant = 20.0f;           // ms
        float leakConductance = 10.0f;        // nS
        float membraneVoltageClampMin = -100.0f;  // mV
        float membraneVoltageClampMax = 50.0f;   // mV
        
        // Refractory properties
        uint32_t absoluteRefractorySteps = 3;  // steps
        uint32_t relativeRefractorySteps = 2;  // steps
        
        // Adaptation properties
        float lowPassAdaptationTimeConstant = 100.0f;  // ms
        float highPassAdaptationTimeConstant = 10.0f;   // ms
        float mTypeTimeConstant = 50.0f;      // ms
        float aTypeTimeConstant = 20.0f;      // ms
        float lowPassHalfActivation = -60.0f;  // mV
        float highPassHalfActivation = -50.0f; // mV
        
        // Synaptic properties
        float ampaTimeConstant = 5.0f;       // ms
        float nmdaTimeConstant = 100.0f;     // ms
        float gabaATimeConstant = 10.0f;     // ms
        float gabaBTimeConstant = 100.0f;    // ms
        
        // Stochastic properties
        float noiseVariance = 1.0f;          // mV^2
        float noiseMean = 0.0f;              // mV
        float noiseTimeConstant = 5.0f;      // ms
        
        // Reset and aftercurrents
        float resetCurrentAmplitude = 0.0f;  // nA
        float postInhibitoryCurrentAmplitude = 0.0f; // nA
        float PINGAmplitude = 0.0f;          // nA
        
        // STP properties
        float STPTimeConstant = 500.0f;      // ms
        float facilitationIncrement = 0.01f;
        float depressionDecrement = 0.01f;
        
        // Type-specific defaults
        float excitatoryMembraneCapacitance = 1.0f;      // nF
        float inhibitoryMembraneCapacitance = 0.8f;       // nF
        float modulatoryMembraneCapacitance = 1.2f;       // nF
    };
    
    static NeuronConfig createExcitatoryNeuronConfig();
    static NeuronConfig createInhibitoryNeuronConfig();
    static NeuronConfig createSensoryNeuronConfig();
    static NeuronConfig createMotorNeuronConfig();
    static NeuronConfig createInternalNeuronConfig();
    
    // Configuration validation
    static bool validateConfig(const NeuronConfig& config);
    static NeuronConfig createDefaultConfig(NeuronType type);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance monitoring integration
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable NeuronPerformanceMetrics perfMetrics;
};

} // namespace nlm
