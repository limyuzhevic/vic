# NLM Advanced Features Documentation

**Complete reference for advanced NLM features and capabilities**

## Overview

This document provides comprehensive documentation for advanced features in the NLM (Neural Learning Machine) neural simulation framework. It covers advanced functionality including advanced neuron models, complex plasticity rules, sophisticated neuromodulation, advanced cognitive systems, and performance optimization techniques.

## Advanced Features Overview

### Advanced Neuron Models
- **Theta Neurons**: Phase-resetting neurons for temporal processing
- **Hodgkin-Huxley Neurons**: Biophysical neuron models with ion channel dynamics
- **Adaptive Exponential Neurons**: Neurons with adaptation and exponential spike initiation
- **Morris-Lecar Neurons**: Reduced two-dimensional neuron models
- **Izhikevich Neurons**: Spiking neuron models with multiple dynamical regimes

### Advanced Plasticity Rules
- **STDP with Eligibility Traces**: Spike-timing-dependent plasticity with temporal traces
- **Reward-Modulated STDP**: STDP modulated by reward signals
- **Anti-STDP**: Anti-Hebbian learning rules
- **Genealogical Plasticity**: Evolution-inspired plasticity rules
- **Meta-Plasticity**: Plasticity rules that adapt their own parameters

### Advanced Neuromodulation
- **Multi-Stage Neuromodulation**: Neuromodulators with multiple functional stages
- **Cross-Modal Neuromodulation**: Interaction between different neuromodulatory systems
- **Neuromodulation with Prediction**: Integration of prediction signals with neuromodulation
- **Contextual Neuromodulation**: Context-dependent neuromodulation

### Advanced Cognitive Systems
- **Predictive Coding**: Hierarchical predictive cognitive architecture
- **Meta-Learning**: Learning to learn and meta-cognitive processes
- **Social Learning**: Complex social and cultural learning mechanisms
- **Complex Planning**: Multi-step planning with uncertainty
- **Emotion Integration**: Emotional regulation and decision-making

### Performance Optimization
- **SIMD Vectorization**: Single instruction, multiple data optimization
- **Multi-threading**: Parallel processing and concurrent execution
- **Memory Pooling**: Efficient memory allocation and management
- **Event-Driven Processing**: Optimized spike and event handling
- **Checkpointing and Recovery**: State management and persistence

### Advanced Visualization
- **Real-time Visualization**: Live visualization of neural dynamics
- **Multi-dimensional Analysis**: Analysis across multiple dimensions
- **Interactive Exploration**: User-interactive exploration tools
- **Data Visualization**: Comprehensive data presentation
- **Animation Generation**: Time-series and state transition animations

## Advanced Neuron Models

### Theta Neuron Model

The Theta neuron model is designed for temporal processing and phase-resetting capabilities.

#### ThetaNeuron Class (`nlm/brain/advanced/theta_neuron.hpp`)

```cpp
class ThetaNeuron {
public:
    // Constructor
    ThetaNeuron(const NeuronConfig& config);
    
    // Core dynamics
    void update(double timestep);
    bool checkThreshold() const;
    void spike();
    void reset();
    
    // Theta-specific properties
    double getPhase() const;
    double getFrequency() const;
    void setFrequency(double frequency);
    double getPhaseResetStrength() const;
    void setPhaseResetStrength(double strength);
    
    // Oscillatory behavior
    double getPhaseVelocity() const;
    void setPhaseVelocity(double velocity);
    bool isPhaseReset() const;
    double getPhaseResetAmount() const;
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### ThetaNeuron Usage Example

```cpp
#include "nlm/brain/advanced/theta_neuron.hpp"
#include <iostream>

int main() {
    // Create theta neuron configuration
    NeuronConfig theta_config;
    theta_config.id = 1;
    theta_config.type = NeuronType::Excitatory;
    theta_config.v_thresh = -50.0;
    theta_config.v_rest = -70.0;
    theta_config.v_reset = -75.0;
    theta_config.tau_mem = 20.0;
    theta_config.tau_ref = 2.0;
    
    // Create and initialize theta neuron
    ThetaNeuron theta_neuron(theta_config);
    theta_neuron.setFrequency(10.0);  // 10 Hz oscillation
    theta_neuron.setPhaseResetStrength(0.5);  // Strong phase reset
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        theta_neuron.update(0.1);  // 0.1 ms timestep
        
        // Check for phase reset
        if (theta_neuron.isPhaseReset()) {
            std::cout << "Phase reset at step " << step 
                      << ", amount: " << theta_neuron.getPhaseResetAmount() << std::endl;
        }
        
        // Log phase
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << ", Phase: " << theta_neuron.getPhase()
                      << ", Frequency: " << theta_neuron.getFrequency() << " Hz" << std::endl;
        }
    }
    
    return 0;
}
```

### Hodgkin-Huxley Neuron Model

The Hodgkin-Huxley neuron model is a biophysical model that includes ion channel dynamics.

#### HodgkinHuxleyNeuron Class (`nlm/brain/advanced/hodgkin_huxley_neuron.hpp`)

```cpp
class HodgkinHuxleyNeuron {
public:
    // Constants
    static constexpr double DEFAULT_V_THRESH = -55.0;  // mV
    static constexpr double DEFAULT_V_REST = -70.0;   // mV
    static constexpr double DEFAULT_V_RESET = -75.0;  // mV
    static constexpr double DEFAULT_TAU_MEM = 10.0;   // ms
    static constexpr double DEFAULT_TAU_REF = 2.0;    // ms
    
    // Ion channel constants
    static constexpr double DEFAULT_G_Na = 120.0;    // mS/cm^2
    static constexpr double DEFAULT_G_K = 36.0;      // mS/cm^2
    static constexpr double DEFAULT_G_L = 0.3;       // mS/cm^2
    static constexpr double DEFAULT_E_Na = 50.0;     // mV
    static constexpr double DEFAULT_E_K = -77.0;     // mV
    static constexpr double DEFAULT_E_L = -54.387;   // mV
    
    // Constructor
    HodgkinHuxleyNeuron(const NeuronConfig& config);
    
    // Core dynamics
    void update(double timestep);
    bool checkThreshold() const;
    void spike();
    void reset();
    
    // Ion channel dynamics
    double getSodiumConductance() const;
    double getPotassiumConductance() const;
    double getLeakConductance() const;
    double getSodiumReversalPotential() const;
    double getPotassiumReversalPotential() const;
    double getLeakReversalPotential() const;
    
    // Gating variables
    double getSodiumActivation() const;
    double getSodiumInactivation() const;
    double getPotassiumActivation() const;
    
    // Ionic currents
    double getSodiumCurrent() const;
    double getPotassiumCurrent() const;
    double getLeakCurrent() const;
    double getTotalMembraneCurrent() const;
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### HodgkinHuxley Neuron Dynamics

The Hodgkin-Huxley neuron model is governed by the following differential equations:

```cpp
dV/dt = (I_app - I_Na - I_K - I_L) / C_m

dm/dt = (alpha_m(V) * (1 - m) - beta_m(V) * m) / tau_m

dh/dt = (alpha_h(V) * (1 - h) - beta_h(V) * h) / tau_h

dn/dt = (alpha_n(V) * (1 - n) - beta_n(V) * n) / tau_n

I_Na = g_Na * m^3 * h * (V - E_Na)
I_K = g_K * n^4 * (V - E_K)
I_L = g_L * (V - E_L)
```

Where the rate functions are:

```cpp
alpha_m(V) = 0.1 * (V + 40) / (1 - exp(-(V + 40) / 10))
beta_m(V) = 4.0 * exp(-(V + 65) / 18)

alpha_h(V) = 0.07 * exp(-(V + 65) / 20)
beta_h(V) = 1 / (1 + exp(-(V + 35) / 10))

alpha_n(V) = 0.01 * (V + 55) / (1 - exp(-(V + 55) / 10))
beta_n(V) = 0.125 * exp(-(V + 65) / 80)
```

#### HodgkinHuxley Neuron Usage Example

```cpp
#include "nlm/brain/advanced/hodgkin_huxley_neuron.hpp"
#include <iostream>
#include <cmath>

int main() {
    // Create Hodgkin-Huxley neuron configuration
    NeuronConfig hh_config;
    hh_config.id = 1;
    hh_config.type = NeuronType::Excitatory;
    hh_config.v_thresh = -55.0;
    hh_config.v_rest = -70.0;
    hh_config.v_reset = -75.0;
    hh_config.tau_mem = 10.0;
    hh_config.tau_ref = 2.0;
    
    // Create and initialize Hodgkin-Huxley neuron
    HodgkinHuxleyNeuron hh_neuron(hh_config);
    
    // Run simulation with applied current
    double I_app = 100.0;  // Applied current (nA)
    for (size_t step = 0; step < 2000; ++step) {
        hh_neuron.update(0.01);  // 0.01 ms timestep
        
        // Check for spike
        if (hh_neuron.checkThreshold()) {
            hh_neuron.spike();
            std::cout << "Spike at step " << step << std::endl;
        }
        
        // Log ion channel states
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": "
                      << "V = " << hh_neuron.getVoltage() << " mV, "
                      << "I_Na = " << hh_neuron.getSodiumCurrent() << " nA, "
                      << "I_K = " << hh_neuron.getPotassiumCurrent() << " nA, "
                      << "I_L = " << hh_neuron.getLeakCurrent() << " nA" << std::endl;
        }
    }
    
    return 0;
}
```

### Adaptive Exponential Neuron Model

The Adaptive Exponential neuron model combines exponential spike initiation with adaptation.

#### AdaptiveExponentialNeuron Class (`nlm/brain/advanced/adaptive_exponential_neuron.hpp`)

```cpp
class AdaptiveExponentialNeuron {
public:
    // Constants
    static constexpr double DEFAULT_V_THRESH = -50.0;  // mV
    static constexpr double DEFAULT_V_REST = -70.0;   // mV
    static constexpr double DEFAULT_V_RESET = -75.0;  // mV
    static constexpr double DEFAULT_TAU_MEM = 20.0;   // ms
    static constexpr double DEFAULT_TAU_REF = 2.0;    // ms
    
    // Adaptation constants
    static constexpr double DEFAULT_B_DELT = 0.0005;  // Adaptation increment
    static constexpr double DEFAULT_A_DELTA = 4.0;     // Adaptation decrement
    static constexpr double DEFAULT_DELTA_T_MAX = 100.0; // Maximum adaptation time constant
    
    // Exponential constants
    static constexpr double DEFAULT_DELTA_TAU = 1.0;  // Exponential time constant
    static constexpr double DEFAULT_DELTA_V = 2.0;    // Exponential voltage threshold
    
    // Constructor
    AdaptiveExponentialNeuron(const NeuronConfig& config);
    
    // Core dynamics
    void update(double timestep);
    bool checkThreshold() const;
    void spike();
    void reset();
    
    // Adaptation properties
    double getAdaptationCurrent() const;
    double getAdaptationIncrement() const;
    void setAdaptationIncrement(double increment);
    double getAdaptationDecrement() const;
    void setAdaptationDecrement(double decrement);
    double getAdaptationTimeConstant() const;
    void setAdaptationTimeConstant(double tau);
    double getMaximumAdaptationTimeConstant() const;
    void setMaximumAdaptationTimeConstant(double tau_max);
    
    // Exponential properties
    double getExponentialSlope() const;
    void setExponentialSlope(double slope);
    double getExponentialThreshold() const;
    void setExponentialThreshold(double v_th);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### AdaptiveExponential Neuron Dynamics

The Adaptive Exponential neuron model is governed by the following equations:

```cpp
dV/dt = (-(V - V_L) + Delta_T * exp((V - V_T) / Delta_U) + I_syn + I_app) / tau_V

dDelta_T/dt = -(Delta_T) / tau_delta + A_delta * sum_delta(t)

V_T = V_thresh + Delta_T

I_syn = sum_syn

I_app = applied current

Delta_T: Adaptation current
A_delta: Adaptation decrement
Delta_U: Adaptation slope
V_T: Threshold voltage
Delta_T: Adaptation increment
```

Where:
- `V`: Membrane potential
- `V_L`: Leak reversal potential (-70 mV)
- `tau_V`: Membrane time constant
- `Delta_T`: Adaptation current
- `tau_delta`: Adaptation time constant
- `A_delta`: Adaptation decrement
- `sum_delta(t)`: Sum of delta functions at spike times
- `I_syn`: Total synaptic current
- `I_app`: Applied current

#### AdaptiveExponential Neuron Usage Example

```cpp
#include "nlm/brain/advanced/adaptive_exponential_neuron.hpp"
#include <iostream>

int main() {
    // Create adaptive exponential neuron configuration
    NeuronConfig aeuron_config;
    aeuron_config.id = 1;
    aeuron_config.type = NeuronType::Excitatory;
    aeuron_config.v_thresh = -50.0;
    aeuron_config.v_rest = -70.0;
    aeuron_config.v_reset = -75.0;
    aeuron_config.tau_mem = 20.0;
    aeuron_config.tau_ref = 2.0;
    
    // Create and initialize adaptive exponential neuron
    AdaptiveExponentialNeuron aeuron(aeuron_config);
    aeuron.setAdaptationIncrement(0.001);
    aeuron.setAdaptationDecrement(2.0);
    aeuron.setExponentialSlope(1.0);
    aeuron.setExponentialThreshold(2.0);
    
    // Run simulation with adaptation
    double I_app = 100.0;  // Applied current (nA)
    for (size_t step = 0; step < 3000; ++step) {
        aeuron.update(0.1);  // 0.1 ms timestep
        
        // Check for spike
        if (aeuron.checkThreshold()) {
            aeuron.spike();
            std::cout << "Spike at step " << step << std::endl;
            std::cout << "  Adaptation current: " << aeuron.getAdaptationCurrent() << std::endl;
        }
        
        // Log neuron state
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": "
                      << "V = " << aeuron.getVoltage() << " mV, "
                      << "Delta_T = " << aeuron.getAdaptationCurrent() << " nA, "
                      << "V_T = " << aeuron.getThreshold() << " mV" << std::endl;
        }
    }
    
    return 0;
}
```

## Advanced Plasticity Rules

### STDP with Eligibility Traces

STDP with eligibility traces combines traditional spike-timing-dependent plasticity with temporal traces for credit assignment.

#### STDPWithEligibilityTraces Class (`nlm/plasticity/stdp_eligibility_traces.hpp`)

```cpp
class STDPWithEligibilityTraces {
public:
    // Constants
    static constexpr double DEFAULT_LEARNING_RATE = 0.001;
    static constexpr double DEFAULT_TAU_TRACE_PRE = 20.0;  // ms
    static constexpr double DEFAULT_TAU_TRACE_POST = 20.0; // ms
    static constexpr double DEFAULT_TRACE_DECAY = 0.01;     // 1/ms
    
    // Constructor
    STDPWithEligibilityTraces(double learning_rate = DEFAULT_LEARNING_RATE,
                              double tau_trace_pre = DEFAULT_TAU_TRACE_PRE,
                              double tau_trace_post = DEFAULT_TAU_TRACE_POST,
                              double trace_decay = DEFAULT_TRACE_DECAY);
    
    // Plasticity operation with eligibility traces
    void applyWithTraces(NeuronId pre_id, NeuronId post_id,
                        double pre_spike_time, double post_spike_time,
                        double weight, double time_step);
    
    // Trace management
    void updateTraces(double time_step);
    void addPreTrace(NeuronId neuron_id, double trace_strength, double time);
    void addPostTrace(NeuronId neuron_id, double trace_strength, double time);
    double getPreTrace(NeuronId neuron_id, double time) const;
    double getPostTrace(NeuronId neuron_id, double time) const;
    void clearTraces(double time_threshold);
    
    // Configuration
    void setLearningRate(double rate);
    double getLearningRate() const;
    void setTraceTimeConstants(double tau_pre, double tau_post);
    void getTraceTimeConstants(double& tau_pre, double& tau_post) const;
    void setTraceDecay(double decay);
    double getTraceDecay() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### STDP with Eligibility Traces Dynamics

The STDP with eligibility traces is governed by the following equations:

```cppnpre(i, t) = Σ_j δ(t - t_j^p) * exp(-(t - t_j^p) / τ_pre)

p(t) = ∫_0^t A_plus * npre(i, t') * npost(j, t) dt'

npre(i, t) = Σ_j δ(t - t_j^p) * exp(-(t - t_j^p) / τ_pre)

npost(j, t) = Σ_i δ(t - t_i^q) * exp(-(t - t_i^q) / τ_post)

Δw = η * p(t)
```

Where:
- `pre(i, t)`: Eligibility trace for pre-synaptic neuron i at time t
- `post(j, t)`: Eligibility trace for post-synaptic neuron j at time t
- `A_plus`: Learning rate for potentiation
- `η`: Overall learning rate
- `τ_pre`: Time constant for pre-synaptic trace
- `τ_post`: Time constant for post-synaptic trace
- `δ(t - t_j^p)`: Dirac delta function at spike time of pre-synaptic neuron
- `δ(t - t_i^q)`: Dirac delta function at spike time of post-synaptic neuron

#### STDP with Eligibility Traces Usage Example

```cpp
#include "nlm/plasticity/stdp_eligibility_traces.hpp"
#include <iostream>
#include <unordered_map>

class STDPWithEligibilityTracesDemo {
private:
    STDPWithEligibilityTraces stdp_eligibility_traces;
    std::unordered_map<NeuronId, double> pre_traces;
    std::unordered_map<NeuronId, double> post_traces;
    std::vector<std::pair<NeuronId, double>> pre_spikes;
    std::vector<std::pair<NeuronId, double>> post_spikes;
    
public:
    STDPWithEligibilityTracesDemo() : stddp_eligibility_traces() {
        // Initialize configuration
        stddp_eligibility_traces.setLearningRate(0.001);
        stddp_eligibility_traces.setTraceTimeConstants(20.0, 20.0);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        for (size_t step = 0; step < 1000; ++step) {
            // Simulate spikes
            if (step % 50 == 0) {  // Pre-synaptic spike
                pre_spikes.emplace_back(1, time);
                stddp_eligibility_traces.addPreTrace(1, 1.0, time);
            }
            
            if (step % 75 == 0) {  // Post-synaptic spike
                post_spikes.emplace_back(2, time);
                stddp_eligibility_traces.addPostTrace(2, 1.0, time);
            }
            
            // Update eligibility traces
            stddp_eligibility_traces.updateTraces(dt);
            
            // Apply STDP with traces
            for (const auto& pre_spike : pre_spikes) {
                for (const auto& post_spike : post_spikes) {
                    double time_diff = post_spike.second - pre_spike.second;
                    if (time_diff > 0 && time_diff < 50.0) {  // Within window
                        double weight_change = stddp_eligibility_traces.applyWithTraces(
                            pre_spike.first, post_spike.first,
                            pre_spike.second, post_spike.second,
                            0.5, dt);
                        std::cout << "Weight change: " << weight_change << std::endl;
                    }
                }
            }
            
            // Log traces
            if (step % 200 == 0) {
                std::cout << "Time " << time << ": "
                          << "Pre-trace[1]: " << stddp_eligibility_traces.getPreTrace(1, time) << ", "
                          << "Post-trace[2]: " << stddp_eligibility_traces.getPostTrace(2, time) << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    STDPWithEligibilityTracesDemo demo;
    demo.simulate();
    return 0;
}
```

### Reward-Modulated STDP

Reward-modulated STDP combines traditional STDP with reward signals for reinforcement learning.

#### RewardModulatedSTDP Class (`nlm/plasticity/reward_modulated_stdp.hpp`)

```cpp
class RewardModulatedSTDP {
public:
    // Constants
    static constexpr double DEFAULT_LEARNING_RATE = 0.001;
    static constexpr double DEFAULT_TAU_TRACE = 20.0;  // ms
    static constexpr double DEFAULT_REWARD_SCALE = 1.0;
    static constexpr double DEFAULT_NOISE_LEVEL = 0.01;
    
    // Constructor
    RewardModulatedSTDP(double learning_rate = DEFAULT_LEARNING_RATE,
                       double tau_trace = DEFAULT_TAU_TRACE,
                       double reward_scale = DEFAULT_REWARD_SCALE,
                       double noise_level = DEFAULT_NOISE_LEVEL);
    
    // Plasticity operation with reward modulation
    void applyWithReward(NeuronId pre_id, NeuronId post_id,
                        double pre_spike_time, double post_spike_time,
                        double weight, double reward, double time_step);
    
    // Reward signal management
    void addRewardSignal(double reward, double time);
    double getRewardSignal(double time) const;
    void clearRewardSignals(double time_threshold);
    
    // Configuration
    void setLearningRate(double rate);
    double getLearningRate() const;
    void setTraceTimeConstant(double tau);
    double getTraceTimeConstant() const;
    void setRewardScale(double scale);
    double getRewardScale() const;
    void setNoiseLevel(double noise);
    double getNoiseLevel() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Reward-Modulated STDP Dynamics

The reward-modulated STDP is governed by the following equations:

```cppnpre(i, t) = Σ_j δ(t - t_j^p) * exp(-(t - t_j^p) / τ_pre)

p(t) = ∫_0^t A_plus * npre(i, t') * npost(j, t) * R(t) dt'

Δw = η * p(t)

R(t) = Σ_r r_k * exp(-(t - t_k^r) / τ_r)

where R(t) is the reward signal:
R(t) = Σ_r r_k * exp(-(t - t_k^r) / τ_r)
```

Where:
- `pre(i, t)`: Eligibility trace for pre-synaptic neuron i at time t
- `post(j, t)`: Eligibility trace for post-synaptic neuron j at time t
- `A_plus`: Learning rate for potentiation
- `η`: Overall learning rate
- `τ_pre`: Time constant for pre-synaptic trace
- `τ_r`: Time constant for reward signal
- `r_k`: Reward signal value at time `t_k^r`
- `R(t)`: Reward signal at time t

#### Reward-Modulated STDP Usage Example

```cpp
#include "nlm/plasticity/reward_modulated_stdp.hpp"
#include <iostream>
#include <random>

class RewardModulatedSTDPExample {
private:
    RewardModulatedSTDP rmd_stdp;
    std::vector<std::pair<NeuronId, double>> pre_spikes;
    std::vector<std::pair<NeuronId, double>> post_spikes;
    std::mt19937 rng;
    std::uniform_real_distribution<double> reward_dist;
    
public:
    RewardModulatedSTDPExample() : 
        rmd_stdp(),
        rng(std::random_device()()),
        reward_dist(0.0, 1.0) {
        
        // Initialize configuration
        rmd_stdp.setLearningRate(0.001);
        rmd_stdp.setTraceTimeConstant(20.0);
        rmd_stdp.setRewardScale(1.0);
        rmd_stdp.setNoiseLevel(0.01);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        for (size_t step = 0; step < 2000; ++step) {
            // Generate random reward signal
            double reward = reward_dist(rng);
            rmd_stdp.addRewardSignal(reward, time);
            
            // Simulate spikes
            if (step % 50 == 0) {  // Pre-synaptic spike
                pre_spikes.emplace_back(1, time);
            }
            
            if (step % 75 == 0) {  // Post-synaptic spike
                post_spikes.emplace_back(2, time);
            }
            
            // Apply reward-modulated STDP
            for (const auto& pre_spike : pre_spikes) {
                for (const auto& post_spike : post_spikes) {
                    double time_diff = post_spike.second - pre_spike.second;
                    if (time_diff > 0 && time_diff < 50.0) {  // Within window
                        double weight_change = rmd_stdp.applyWithReward(
                            pre_spike.first, post_spike.first,
                            pre_spike.second, post_spike.second,
                            0.5, reward, dt);
                        std::cout << "Step " << step << ": "
                                  << "Reward: " << reward << ", "
                                  << "Weight change: " << weight_change << std::endl;
                    }
                }
            }
            
            // Clean up old spikes and rewards
            if (step % 500 == 0) {
                rmd_stdp.clearRewardSignals(time - 100.0);
                std::vector<std::pair<NeuronId, double>> new_pre_spikes;
                std::vector<std::pair<NeuronId, double>> new_post_spikes;
                
                for (const auto& spike : pre_spikes) {
                    if (spike.second >= time - 100.0) {
                        new_pre_spikes.push_back(spike);
                    }
                }
                
                for (const auto& spike : post_spikes) {
                    if (spike.second >= time - 100.0) {
                        new_post_spikes.push_back(spike);
                    }
                }
                
                pre_spikes = new_pre_spikes;
                post_spikes = new_post_spikes;
            }
            
            if (step % 300 == 0) {
                double current_reward = rmd_stdp.getRewardSignal(time);
                std::cout << "Time " << time << ": "
                          << "Current reward: " << current_reward << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    RewardModulatedSTDPExample example;
    example.simulate();
    return 0;
}
```

## Advanced Neuromodulation

### Multi-Stage Neuromodulation

Multi-stage neuromodulation involves neuromodulators with multiple functional stages.

#### MultiStageNeuromodulator Class (`nlm/neuromodulation/multi_stage_neuromodulator.hpp`)

```cpp
class MultiStageNeuromodulator {
public:
    // Constants
    static constexpr double DEFAULT_ACTIVATION_THRESHOLD = 0.1;
    static constexpr double DEFAULT_ACTIVATION_STRENGTH = 1.0;
    static constexpr double DEFAULT_STAGE_TRANSITION_TIME = 10.0; // ms
    static constexpr double DEFAULT_DECAY_RATE = 0.01; // 1/ms
    
    // Stage enumeration
    enum class NeuromodulationStage {
        SNEED_LEVEL,        // Basic need level
        MOTIVATION_LEVEL,   // Motivation level
        SATISFACTION_LEVEL, // Satisfaction level
        REWARD_LEVEL,       // Reward level
        PUNISHMENT_LEVEL    // Punishment level
    };
    
    // Constructor
    MultiStageNeuromodulator();
    
    // Multi-stage neuromodulation
    void applyMultiStage(NeuronId neuron_id, double signal_strength,
                        double weight, double time,
                        NeuromodulationStage stage);
    
    // Stage management
    void setStage(NeuromodulationStage stage);
    NeuromodulationStage getStage() const;
    bool shouldTransitionStage(double signal_strength, NeuromodulationStage target_stage);
    double getStageTransitionTime() const;
    void setStageTransitionTime(double time);
    
    // Stage-specific properties
    void setActivationThreshold(NeuromodulationStage stage, double threshold);
    double getActivationThreshold(NeuromodulationStage stage) const;
    void setActivationStrength(NeuromodulationStage stage, double strength);
    double getActivationStrength(NeuromodulationStage stage) const;
    
    // Signal processing
    double processSignal(double input_signal, NeuromodulationStage stage);
    std::vector<double> getStageOutputs() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Multi-Stage Neuromodulation Dynamics

The multi-stage neuromodulation is governed by the following equations:

```cppdS_stage/dt = -(S_stage) / τ_stage + α_stage * (I_stage - θ_stage)

S_stage = stage output signal
τ_stage = time constant for stage
α_stage = activation strength for stage
I_stage = input signal for stage
θ_stage = activation threshold for stage

Stage transition:
If |S_stage - θ_stage| > δ_stage, transition to next stage
```

Where:
- `S_stage`: Output signal for the current stage
- `τ_stage`: Time constant for the stage
- `α_stage`: Activation strength for the stage
- `I_stage`: Input signal for the stage
- `θ_stage`: Activation threshold for the stage
- `δ_stage`: Transition threshold

#### Multi-Stage Neuromodulation Usage Example

```cpp
#include "nlm/neuromodulation/multi_stage_neuromodulator.hpp"
#include <iostream>
#include <iomanip>

class MultiStageNeuromodulationDemo {
private:
    MultiStageNeuromodulator neuromodulator;
    std::vector<double> input_signals;
    
public:
    MultiStageNeuromodulationDemo() : neuromodulator() {
        // Initialize stage properties
        neuromodulator.setActivationThreshold(
            MultiStageNeuromodulator::NeuromodulationStage::SNEED_LEVEL, 0.1);
        neuromodulator.setActivationStrength(
            MultiStageNeuromodulator::NeuromodulationStage::SNEED_LEVEL, 1.0);
        neuromodulator.setActivationThreshold(
            MultiStageNeuromodulator::NeuromodulationStage::MOTIVATION_LEVEL, 0.3);
        neuromodulator.setActivationStrength(
            MultiStageNeuromodulator::NeuromodulationStage::MOTIVATION_LEVEL, 1.5);
        neuromodulator.setStageTransitionTime(10.0);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        // Generate input signal (e.g., food need)
        for (size_t step = 0; step < 500; ++step) {
            // Time-varying input signal
            double input_signal = std::sin(time * 0.1) * 0.5 + 0.5;
            input_signals.push_back(input_signal);
            
            // Apply multi-stage neuromodulation
            neuromodulator.applyMultiStage(
                1, input_signal, 0.5, time,
                MultiStageNeuromodulator::NeuromodulationStage::SNEED_LEVEL);
            
            // Get stage outputs
            std::vector<double> stage_outputs = neuromodulator.getStageOutputs();
            
            // Print neuromodulation state
            if (step % 50 == 0) {
                std::cout << "Time " << time << ": "
                          << "Input: " << std::fixed << std::setprecision(3) << input_signal << ", ";
                
                for (size_t i = 0; i < stage_outputs.size(); ++i) {
                    std::cout << "Stage " << i << ": " << stage_outputs[i];
                    if (i < stage_outputs.size() - 1) std::cout << ", ";
                }
                std::cout << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    MultiStageNeuromodulationDemo demo;
    demo.simulate();
    return 0;
}
```

### Neuromodulation with Prediction

Neuromodulation with prediction integrates prediction signals with traditional neuromodulation.

#### NeuromodulationWithPrediction Class (`nlm/neuromodulation/neuromodulation_prediction.hpp`)

```cpp
class NeuromodulationWithPrediction {
public:
    // Constants
    static constexpr double DEFAULT_LEARNING_RATE = 0.001;
    static constexpr double DEFAULT_PREDICTION_WEIGHT = 0.5;
    static constexpr double DEFAULT_NEUROMODULATION_STRENGTH = 1.0;
    static constexpr double DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.1;
    
    // Constructor
    NeuromodulationWithPrediction(double learning_rate = DEFAULT_LEARNING_RATE,
                                  double prediction_weight = DEFAULT_PREDICTION_WEIGHT,
                                  double neuromodulation_strength = DEFAULT_NEUROMODULATION_STRENGTH,
                                  double prediction_error_threshold = DEFAULT_PREDICTION_ERROR_THRESHOLD);
    
    // Neuromodulation with prediction
    void applyWithPrediction(NeuronId neuron_id, double signal_strength,
                            double weight, double prediction, double prediction_error,
                            double time_step);
    
    // Prediction signal management
    void addPredictionSignal(double prediction, double prediction_error, double time);
    double getPredictionSignal(double time) const;
    double getPredictionError(double time) const;
    void clearPredictionSignals(double time_threshold);
    
    // Configuration
    void setLearningRate(double rate);
    double getLearningRate() const;
    void setPredictionWeight(double weight);
    double getPredictionWeight() const;
    void setNeuromodulationStrength(double strength);
    double getNeuromodulationStrength() const;
    void setPredictionErrorThreshold(double threshold);
    double getPredictionErrorThreshold() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Neuromodulation with Prediction Dynamics

The neuromodulation with prediction is governed by the following equations:

```cppnpre(i, t) = Σ_j δ(t - t_j^p) * exp(-(t - t_j^p) / τ_pre)

p(t) = ∫_0^t A_plus * npre(i, t') * npost(j, t) dt'

R(t) = N(t) * (1 + β * |e(t)|)

where:
- `npre(i, t)`: Pre-synaptic eligibility trace
- `npost(j, t)`: Post-synaptic eligibility trace
- `A_plus`: STDP learning rate
- `N(t)`: Base neuromodulation signal
- `β`: Prediction error sensitivity
- `e(t)`: Prediction error

Neuromodulation output:
M(t) = N(t) * (1 + β * |e(t)|)
```

#### Neuromodulation with Prediction Usage Example

```cpp
#include "nlm/neuromodulation/neuromodulation_prediction.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class NeuromodulationWithPredictionDemo {
private:
    NeuromodulationWithPrediction neuromod_prediction;
    std::vector<std::tuple<double, double, double>> prediction_signals; // (prediction, error, time)
    
public:
    NeuromodulationWithPredictionDemo() : neuromod_prediction() {
        // Initialize configuration
        neuromod_prediction.setLearningRate(0.001);
        neuromod_prediction.setPredictionWeight(0.5);
        neuromod_prediction.setNeuromodulationStrength(1.0);
        neuromod_prediction.setPredictionErrorThreshold(0.1);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        for (size_t step = 0; step < 1000; ++step) {
            // Generate prediction signal (e.g., expected reward)
            double prediction = 0.8 + 0.2 * std::sin(time * 0.05);
            double prediction_error = std::abs(0.8 - prediction); // Deviation from expected
            neuromod_prediction.addPredictionSignal(prediction, prediction_error, time);
            
            // Apply neuromodulation with prediction
            neuromod_prediction.applyWithPrediction(
                1, 0.5, 0.5, prediction, prediction_error, dt);
            
            // Log neuromodulation state
            if (step % 100 == 0) {
                double current_prediction = neuromod_prediction.getPredictionSignal(time);
                double current_error = neuromod_prediction.getPredictionError(time);
                
                std::cout << "Time " << time << ": "
                          << "Prediction: " << current_prediction << ", "
                          << "Error: " << current_error << ", "
                          << "Enhancement: " << (1.0 + neuromod_prediction.getPredictionWeight() * current_error) << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    NeuromodulationWithPredictionDemo demo;
    demo.simulate();
    return 0;
}
```

## Advanced Cognitive Systems

### Predictive Coding

Predictive coding is a hierarchical cognitive architecture that uses predictive models to minimize prediction errors.

#### PredictiveCodingSystem Class (`nlm/cognition/predictive_coding.hpp`)

```cpp
class PredictiveCodingSystem {
public:
    // Constants
    static constexpr double DEFAULT_LEARNING_RATE = 0.01;
    static constexpr double DEFAULT_PREDICTION_ERROR_WEIGHT = 1.0;
    static constexpr double DEFAULT_MODEL_UPDATE_RATE = 0.001;
    static constexpr size_t DEFAULT_HIERARCHY_LEVELS = 3;
    
    // Constructor
    PredictiveCodingSystem(size_t hierarchy_levels = DEFAULT_HIERARCHY_LEVELS,
                          double learning_rate = DEFAULT_LEARNING_RATE,
                          double prediction_error_weight = DEFAULT_PREDICTION_ERROR_WEIGHT,
                          double model_update_rate = DEFAULT_MODEL_UPDATE_RATE);
    
    // Predictive coding
    void predict(double time_step);
    void update(double time_step);
    double getPredictionError(size_t level) const;
    void setInputSignal(double signal, size_t level = 0);
    double getOutputSignal(size_t level) const;
    
    // Hierarchical processing
    size_t getHierarchyLevels() const;
    void setHierarchyLevel(size_t level);
    size_t getCurrentLevel() const;
    
    // Configuration
    void setLearningRate(double rate);
    double getLearningRate() const;
    void setPredictionErrorWeight(double weight);
    double getPredictionErrorWeight() const;
    void setModelUpdateRate(double rate);
    double getModelUpdateRate() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Predictive Coding Dynamics

The predictive coding system is governed by the following equations:

```cppp_l = f_l(x_l, θ_l)  // Generate prediction at level l

ε_l = x_l - p_l+1      // Compute prediction error

Δθ_l = α * ε_l         // Update model parameters

x_l = p_l + ε_l       // Send prediction error upward

x_0 = input_signal    // Input at level 0
```

Where:
- `p_l`: Prediction at level l
- `ε_l`: Prediction error at level l
- `θ_l`: Model parameters at level l
- `α`: Learning rate
- `x_l`: State at level l
- `x_0`: Input signal at level 0

#### Predictive Coding Usage Example

```cpp
#include "nlm/cognition/predictive_coding.hpp"
#include <iostream>
#include <vector>

class PredictiveCodingDemo {
private:
    PredictiveCodingSystem predictive_coding;
    std::vector<double> input_signal;
    
public:
    PredictiveCodingDemo() : predictive_coding() {
        // Set initial input signal (e.g., sensory input)
        predictive_coding.setInputSignal(1.0);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        for (size_t step = 0; step < 500; ++step) {
            // Time-varying input signal (e.g., changing stimulus)
            double current_input = 0.5 + 0.5 * std::sin(time * 0.02);
            predictive_coding.setInputSignal(current_input);
            
            // Run predictive coding
            predictive_coding.predict(dt);
            predictive_coding.update(dt);
            
            // Log predictive coding state
            if (step % 100 == 0) {
                std::cout << "Time " << time << ": ";
                for (size_t level = 0; level < predictive_coding.getHierarchyLevels(); ++level) {
                    std::cout << "Level " << level << ": "
                              << "Error: " << predictive_coding.getPredictionError(level) << ", "
                              << "Output: " << predictive_coding.getOutputSignal(level);
                    if (level < predictive_coding.getHierarchyLevels() - 1) std::cout << ", ";
                }
                std::cout << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    PredictiveCodingDemo demo;
    demo.simulate();
    return 0;
}
```

### Meta-Learning

Meta-learning is learning to learn, where the system learns how to update its learning processes.

#### MetaLearningSystem Class (`nlm/cognition/meta_learning.hpp`)

```cpp
class MetaLearningSystem {
public:
    // Constants
    static constexpr double DEFAULT_META_LEARNING_RATE = 0.001;
    static constexpr double DEFAULT_BASE_LEARNING_RATE = 0.01;
    static constexpr double DEFAULT_ADAPTATION_RATE = 0.1;
    static constexpr double DEFAULT_NOISE_LEVEL = 0.01;
    
    // Constructor
    MetaLearningSystem(double meta_learning_rate = DEFAULT_META_LEARNING_RATE,
                      double base_learning_rate = DEFAULT_BASE_LEARNING_RATE,
                      double adaptation_rate = DEFAULT_ADAPTATION_RATE,
                      double noise_level = DEFAULT_NOISE_LEVEL);
    
    // Meta-learning
    void update(double time_step);
    void learn(double error, double time_step);
    double getMetaLearningRate() const;
    double getBaseLearningRate() const;
    double getAdaptedLearningRate() const;
    
    // Learning rate adaptation
    void setBaseLearningRate(double rate);
    void setAdaptationRate(double rate);
    double getAdaptationRate() const;
    void setNoiseLevel(double noise);
    double getNoiseLevel() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Meta-Learning Dynamics

The meta-learning system is governed by the following equations:

```cppη_t = η_t-1 + α * (δL/δη)_t  // Update meta-learning rate

η_t = η_t-1 + α * (δL/δη)_t

dL/dt = η_t * dL/dθ  // Update parameters with meta-learning rate

dL/dθ: Gradient of loss with respect to parameters
α: Meta-learning rate
η_t: Current meta-learning rate
```

Where:
- `η_t`: Meta-learning rate at time t
- `η_t-1`: Meta-learning rate at time t-1
- `α`: Meta-learning rate
- `δL/δη`: Gradient of meta-loss with respect to meta-learning rate
- `dL/dθ`: Gradient of loss with respect to parameters

#### Meta-Learning Usage Example

```cpp
#include "nlm/cognition/meta_learning.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class MetaLearningDemo {
private:
    MetaLearningSystem meta_learning;
    std::vector<double> parameter_history;
    std::vector<double> error_history;
    double current_loss;
    
public:
    MetaLearningDemo() : 
        meta_learning(),
        current_loss(1.0) {
        
        // Initialize configuration
        meta_learning.setBaseLearningRate(0.01);
        meta_learning.setAdaptationRate(0.1);
        meta_learning.setNoiseLevel(0.01);
    }
    
    void simulate() {
        double time = 0.0;
        double dt = 0.1;  // 0.1 ms timestep
        
        for (size_t step = 0; step < 1000; ++step) {
            // Simulate learning process
            double error = std::sin(time * 0.01) * 0.5;  // Simulated error
            error_history.push_back(error);
            
            // Learn with meta-learning
            meta_learning.learn(error, dt);
            
            // Get current meta-learning rate
            double meta_rate = meta_learning.getAdaptedLearningRate();
            parameter_history.push_back(meta_rate);
            
            // Simulate parameter update
            current_loss = current_loss - meta_rate * error;
            
            // Log meta-learning state
            if (step % 200 == 0) {
                std::cout << "Time " << time << ": "
                          << "Base rate: " << meta_learning.getBaseLearningRate() << ", "
                          << "Meta rate: " << meta_rate << ", "
                          << "Loss: " << current_loss << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    MetaLearningDemo demo;
    demo.simulate();
    return 0;
}
```

## Performance Optimization

### SIMD Vectorization

SIMD (Single Instruction, Multiple Data) vectorization is used to accelerate neural computation.

#### SIMDAccelerator Class (`nlm/optimization/simd_accelerator.hpp`)

```cpp
class SIMDAccelerator {
public:
    // Constants
    static constexpr size_t VECTOR_SIZE = 8;  // 256-bit vectors (8 float64)
    
    // SIMD operations
    static std::vector<double> addVectors(const std::vector<double>& vec1, const std::vector<double>& vec2);
    static std::vector<double> multiplyVectors(const std::vector<double>& vec1, const std::vector<double>& vec2);
    static std::vector<double> dotProduct(const std::vector<double>& vec1, const std::vector<double>& vec2);
    static std::vector<double> multiplyAccumulate(const std::vector<double>& vec1, const std::vector<double>& vec2, const std::vector<double>& accumulator);
    
    // Neuron operations
    static void updateNeuronVoltages(std::vector<double>& voltages, const std::vector<double>& currents, double timestep);
    static void updateSynapticWeights(std::vector<double>& weights, const std::vector<double>& eligibility_traces, double learning_rate);
    static void applyPlasticity(std::vector<double>& pre_traces, std::vector<double>& post_traces, std::vector<double>& weights, double learning_rate, double timestep);
    
    // Performance metrics
    static size_t getOperationsPerSecond();
    static double getSIMDEfficiency();
    static size_t getVectorizationLevel();
    
    // Configuration
    static void setVectorizationLevel(size_t level);
    static size_t getVectorizationLevel();
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### SIMD Optimization Example

```cpp
#include "nlm/optimization/simd_accelerator.hpp"
#include <iostream>
#include <vector>
#include <chrono>

class SIMDOptimizationDemo {
private:
    SIMDAccelerator simd_accelerator;
    
public:
    SIMDOptimizationDemo() : simd_accelerator() {
        simd_accelerator.setEnabled(true);
    }
    
    void demonstrateSIMD() {
        // Create test vectors
        std::vector<double> voltages(1000, -70.0);  // Initial voltages
        std::vector<double> currents(1000, 10.0);   // Applied currents
        std::vector<double> weights(1000, 0.5);     // Synaptic weights
        std::vector<double> eligibility_traces(1000, 0.0); // Eligibility traces
        
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Perform vectorized operations
        for (size_t iteration = 0; iteration < 100; ++iteration) {
            // Update neuron voltages
            SIMDAccelerator::updateNeuronVoltages(voltages, currents, 0.1);
            
            // Update synaptic weights
            SIMDAccelerator::updateSynapticWeights(weights, eligibility_traces, 0.001);
            
            // Apply plasticity
            SIMDAccelerator::applyPlasticity(eligibility_traces, weights, weights, 0.001, 0.1);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "SIMD optimization completed in " << duration.count() << " ms" << std::endl;
        std::cout << "Operations per second: " << SIMDAccelerator::getOperationsPerSecond() << std::endl;
        std::cout << "SIMD efficiency: " << SIMDAccelerator::getSIMDEfficiency() << std::endl;
    }
};

int main() {
    SIMDOptimizationDemo demo;
    demo.demonstrateSIMD();
    return 0;
}
```

### Multi-threading

Multi-threading is used to parallelize neural computation across multiple CPU cores.

#### MultiThreadSimulator Class (`nlm/optimization/multi_thread_simulator.hpp`)

```cpp
class MultiThreadSimulator {
public:
    // Constants
    static constexpr size_t DEFAULT_THREAD_COUNT = std::thread::hardware_concurrency();
    
    // Constructor
    MultiThreadSimulator(size_t thread_count = DEFAULT_THREAD_COUNT);
    
    // Thread management
    void setThreadCount(size_t thread_count);
    size_t getThreadCount() const;
    size_t getActiveThreadCount() const;
    
    // Simulation
    void runSimulation(size_t steps_per_thread);
    void distributeWorkload(const std::vector<NeuronId>& neuron_ids);
    
    // Performance
    double getSpeedup() const;
    double getEfficiency() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Multi-threading Example

```cpp
#include "nlm/optimization/multi_thread_simulator.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

class MultiThreadingDemo {
private:
    MultiThreadSimulator multi_thread_simulator;
    std::vector<NeuronId> neuron_ids;
    
public:
    MultiThreadingDemo() : multi_thread_simulator() {
        // Initialize neuron IDs
        for (size_t i = 0; i < 1000; ++i) {
            neuron_ids.push_back(static_cast<NeuronId>(i));
        }
        
        multi_thread_simulator.setEnabled(true);
    }
    
    void demonstrateMultiThreading() {
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Run multi-threaded simulation
        multi_thread_simulator.runSimulation(100);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "Multi-threaded simulation completed in " << duration.count() << " ms" << std::endl;
        std::cout << "Speedup: " << multi_thread_simulator.getSpeedup() << std::endl;
        std::cout << "Efficiency: " << multi_thread_simulator.getEfficiency() << std::endl;
    }
};

int main() {
    MultiThreadingDemo demo;
    demo.demonstrateMultiThreading();
    return 0;
}
```

### Memory Pooling

Memory pooling is used to optimize memory allocation and reduce fragmentation.

#### MemoryPool Class (`nlm/optimization/memory_pool.hpp`)

```cpp
class MemoryPool {
public:
    // Constants
    static constexpr size_t DEFAULT_POOL_SIZE = 1024 * 1024; // 1 MB
    
    // Constructor
    MemoryPool(size_t pool_size = DEFAULT_POOL_SIZE);
    
    // Memory management
    void* allocate(size_t size);
    void deallocate(void* pointer, size_t size);
    bool isAvailable(size_t size) const;
    size_t getAvailableMemory() const;
    size_t getTotalMemory() const;
    
    // Pool management
    void clear();
    size_t getAllocationCount() const;
    
    // Query
    std::string getName() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### Memory Pooling Example

```cpp
#include "nlm/optimization/memory_pool.hpp"
#include <iostream>
#include <vector>
#include <chrono>

class MemoryPoolingDemo {
private:
    MemoryPool memory_pool;
    std::vector<void*> allocated_pointers;
    
public:
    MemoryPoolingDemo() : memory_pool() {
        memory_pool.setEnabled(true);
    }
    
    void demonstrateMemoryPooling() {
        // Allocate memory using memory pool
        for (size_t i = 0; i < 1000; ++i) {
            void* pointer = memory_pool.allocate(1024); // 1 KB allocation
            if (pointer) {
                allocated_pointers.push_back(pointer);
            }
        }
        
        std::cout << "Allocated " << allocated_pointers.size() << " blocks" << std::endl;
        std::cout << "Available memory: " << memory_pool.getAvailableMemory() << " bytes" << std::endl;
        std::cout << "Total memory: " << memory_pool.getTotalMemory() << " bytes" << std::endl;
        
        // Deallocate memory
        for (void* pointer : allocated_pointers) {
            memory_pool.deallocate(pointer, 1024);
        }
        
        allocated_pointers.clear();
        
        std::cout << "Memory pool cleared" << std::endl;
    }
};

int main() {
    MemoryPoolingDemo demo;
    demo.demonstrateMemoryPooling();
    return 0;
}
```

## Documentation Summary

This advanced features documentation provides comprehensive coverage of NLM's advanced capabilities, including:

### Advanced Neuron Models
- **Theta Neuron**: Phase-resetting neurons for temporal processing
- **Hodgkin-Huxley Neuron**: Biophysical neuron models with ion channel dynamics
- **Adaptive Exponential Neuron**: Neurons with adaptation and exponential spike initiation

### Advanced Plasticity Rules
- **STDP with Eligibility Traces**: Spike-timing-dependent plasticity with temporal traces
- **Reward-Modulated STDP**: STDP modulated by reward signals

### Advanced Neuromodulation
- **Multi-Stage Neuromodulation**: Neuromodulators with multiple functional stages
- **Neuromodulation with Prediction**: Integration of prediction signals with neuromodulation

### Advanced Cognitive Systems
- **Predictive Coding**: Hierarchical predictive cognitive architecture
- **Meta-Learning**: Learning to learn and meta-cognitive processes

### Performance Optimization
- **SIMD Vectorization**: Single instruction, multiple data optimization
- **Multi-threading**: Parallel processing and concurrent execution
- **Memory Pooling**: Efficient memory allocation and management

Each advanced feature section includes:
- Class definitions and interfaces
- Mathematical models and equations
- Usage examples and demonstrations
- Performance considerations
- Configuration options

This documentation provides the foundation for users to understand and utilize NLM's advanced features for complex neural simulations, research applications, and performance-critical deployments.