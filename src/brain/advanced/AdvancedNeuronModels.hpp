#pragma once

#include "../Neuron.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Advanced neuron model types
enum class AdvancedNeuronModelType : uint8_t {
    LIF,                      // Leaky Integrate-and-Fire (baseline)
    HodgkinHuxley,           // HH model with ion channel dynamics
    Izhikevich,              // Izhikevich adaptive exponential
    AdaptiveExponential,     // Adaptive exponential integrate-and-fire
    Theta,                   // Theta neuron model
    MorrisLecar,             // Morris-Lecar two-variable model
    MultipleUnit             // Multiple coupled neurons
};

// Hodgkin-Huxley specific parameters
struct HodgkinHuxleyParameters {
    // Membrane capacitance
    MembranePotential C_m;  // μF/cm²
    
    // Maximal conductances
    float g_Na_max;         // mS/cm² - Sodium
    float g_K_max;          // mS/cm² - Potassium  
    float g_L_max;          // mS/cm² - Leak
    
    // Reversal potentials
    float E_Na;             // mV - Sodium reversal potential
    float E_K;              // mV - Potassium reversal potential
    float E_L;              // mV - Leak reversal potential
    
    // Threshold and reset
    MembranePotential V_th; // mV - Spike threshold
    float V_reset;           // mV - Reset potential
    
    // Simulation parameters
    float dt;               // ms - Time step
    float T_max;            // ms - Maximum simulation time
};

// Izhikevich specific parameters
struct IzhikevichParameters {
    // Membrane dynamics
    float a;                // Recovery time constant
    float b;                // Sensitivity of recovery variable
    float c;                // After-spike reset value
    float d;                // After-spike recovery
    
    // Threshold
    float V_th;             // mV - Spike threshold
    
    // Initial conditions
    float V_init;           // mV - Initial membrane potential
    float u_init;           // mV - Initial recovery variable
};

// Adaptive exponential integrate-and-fire parameters
struct AdaptiveExponentialParameters {
    // Integration parameters
    float V_th;             // mV - Spike threshold
    float V_reset;           // mV - Reset potential
    float delta_T;           // mV - Slope factor
    
    // Adaptation parameters
    float tau_w;            // ms - Adaptation time constant
    float a;                // Adaptation strength
    float b;                // Adaptation threshold sensitivity
    
    // Simulation
    float dt;               // ms - Time step
};

// Theta neuron parameters
struct ThetaNeuronParameters {
    // Membrane properties
    float V_th;             // mV - Threshold
    float V_reset;           // mV - Reset
    
    // Firing rate parameters
    float omega_0;          // Hz - Natural frequency
    float delta;            // Hz - Frequency current dependence
    
    // Simulation
    float dt;               // ms - Time step
};

// Morris-Lecar parameters
struct MorrisLecarParameters {
    // Membrane parameters
    float C_m;              // μF/cm² - Membrane capacitance
    float g_L;              // mS/cm² - Leak conductance
    float g_Ca;             // mS/cm² - Calcium conductance
    float g_K;              // mS/cm² - Potassium conductance
    
    // Reversal potentials
    float E_L;              // mV - Leak reversal
    float E_Ca;             // mV - Calcium reversal
    float E_K;              // mV - Potassium reversal
    
    // Steady-state and time constants
    float phi;              // Scaling factor
    float V1;               // mV - Half-activation voltage (Ca)
    float V2;               // mV - Half-activation voltage (Ca)
    float V3;               // mV - Half-activation voltage (K)
    float V4;               // mV - Half-activation voltage (K)
    
    // Steady-state conductances
    float g_L_inf;          // mS/cm² - Steady-state leak
    float g_Ca_inf;         // mS/cm² - Steady-state calcium
    float g_K_inf;          // mS/cm² - Steady-state potassium
    
    // Time constants
    float tau_L;            // ms - Time constant (leak)
    float tau_Ca;           // ms - Time constant (calcium)
    float tau_K;            // ms - Time constant (potassium)
    
    // Threshold
    float V_th;             // mV - Spike threshold
    
    // Reset values
    float V_reset;          // mV - Reset potential
    
    // Simulation
    float dt;               // ms - Time step
};

// Base class for advanced neuron models
class AdvancedNeuron {
public:
    virtual ~AdvancedNeuron() = default;
    
    // Core simulation method
    virtual bool step(Timestamp currentTime) = 0;
    
    // Get membrane potential
    virtual MembranePotential getMembranePotential() const = 0;
    
    // Get firing state
    virtual bool isFiring() const = 0;
    
    // Get last spike time
    virtual float getLastSpikeTime() const = 0;
    
    // Reset neuron to initial state
    virtual void reset() = 0;
    
    // Get model type
    virtual AdvancedNeuronModelType getModelType() const = 0;
    
    // Get model-specific parameters
    virtual const std::any& getParameters() const = 0;
    
    // Inject external current
    virtual void injectCurrent(MembranePotential current) = 0;
    
    // Receive synaptic input
    virtual void receiveSynapticInput(MembranePotential amplitude) = 0;
    
    // Record a spike
    virtual void recordSpike(Timestamp timestamp) = 0;
    
    // Get spike history
    virtual const std::vector<Timestamp>& getSpikeHistory() const = 0;
};

// Factory function for creating advanced neurons
std::unique_ptr<AdvancedNeuron> createAdvancedNeuron(
    AdvancedNeuronModelType modelType,
    NeuronId id,
    const std::any& parameters,
    const NeuronState& initialState = NeuronState()
);

} // namespace nlm
