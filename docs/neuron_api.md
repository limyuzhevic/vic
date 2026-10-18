# NLM Neuron Module API

## Overview

The Neuron Module implements the fundamental computational units of the NLM artificial brain. Neurons are modeled using the Leaky Integrate-and-Fire (LIF) framework, which provides a biologically plausible yet computationally efficient representation of neural dynamics. Each neuron processes incoming synaptic inputs, integrates them over time, and generates output spikes when threshold conditions are met.

## Key Features

- **Biologically inspired dynamics**: Leaky Integrate-and-Fire neuron model
- **Multiple neuron types**: Sensory, Motor, and Internal neurons with specialized properties
- **Event-driven computation**: Spike-driven processing for efficient simulation
- **Plasticity integration**: Activity-dependent synaptic modification
- **State management**: Comprehensive tracking of membrane potential and internal state
- **Performance optimization**: Memory-efficient representation and vectorized operations
- **Extensible architecture**: Plugin system for custom neuron models

## Neuron Model

### Leaky Integrate-and-Fire (LIF) Dynamics

The LIF model describes how neurons integrate incoming currents and generate spikes:

```
dV/dt = (I_leak + I_syn - I_adapt) / C_m

I_leak = g_leak * (V - E_leak)
I_adapt = w_adapt * (V - V_adapt)
```

Where:
- **V**: Membrane potential
- **I_leak**: Leak current
- **I_syn**: Synaptic current
- **I_adapt**: Adaptation current
- **C_m**: Membrane capacitance
- **g_leak**: Leak conductance
- **E_leak**: Leak reversal potential
- **w_adapt**: Adaptation weight
- **V_adapt**: Adaptation reversal potential

## Core Classes

### NeuronId Type

```cpp
struct NeuronId {
    uint64_t value;
    
    // Comparisons
    bool operator==(const NeuronId& other) const;
    bool operator!=(const NeuronId& other) const;
    bool operator<(const NeuronId& other) const;
    bool operator<=(const NeuronId& other) const;
    bool operator>(const NeuronId& other) const;
    bool operator>=(const NeuronId& other) const;
    
    // Hash for unordered containers
    size_t hash() const;
};
```

### NeuronType Enum

```cpp
enum class NeuronType {
    Internal,    // Internal processing neurons
    Sensory,     // Sensory input neurons
    Motor,       // Motor output neurons
    Error,       // Error or warning neurons
    Unknown      // Unknown or unclassified neurons
};
```

### NeuronState Structure

```cpp
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
    
    // Default constructor
    NeuronState();
    
    // Reset to initial state
    void reset();
    
    // Check if neuron is in refractory period
    bool isRefractory() const;
    
    // Update state with new inputs
    void update(double timestep, MembranePotential synapticInput, 
                MembranePotential adaptationInput);
};
```

### PlasticityFlags Struct

```cpp
struct PlasticityFlags {
    bool hebbian;      // Enable Hebbian learning
    bool stdp;         // Enable Spike-Timing-Dependent Plasticity
    bool structural;   // Enable structural plasticity
    bool homeostatic;  // Enable homeostatic regulation
    
    PlasticityFlags();
    bool isEnabled() const;
};
```

### Neuron Class

```cpp
class Neuron {
public:
    // Construction and lifecycle
    explicit Neuron(NeuronId id);
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Core properties
    NeuronId getId() const;                           // Get neuron identifier
    NeuronType getType() const;                       // Get neuron type
    const NeuronState& getState() const;              // Get current state
    NeuronState& getState();                          // Get current state (non-const)
    
    // Neuron type setting
    void setType(NeuronType type);
    
    // Membrane potential access
    MembranePotential getMembranePotential() const;   // Current membrane potential
    MembranePotential getThreshold() const;           // Firing threshold
    MembranePotential getRestingPotential() const;    // Resting potential
    MembranePotential getResetPotential() const;      // Reset potential
    
    // Membrane properties
    void setMembraneProperties(MembranePotential resting,
                                MembranePotential threshold,
                                MembranePotential reset,
                                MembranePotential leakConductance);
    
    // Synaptic integration
    void injectCurrent(MembranePotential current);     // Inject synaptic current
    void update(double timestep);                     // Update membrane potential
    
    // Spike generation and detection
    bool hasSpiked() const;                           // Check if neuron spiked this step
    Timestamp getLastSpikeTime() const;              // Get last spike time
    void reset();                                     // Reset neuron state
    void processSpikes();                             // Process spike events
    
    // Plasticity control
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    bool hasPlasticityEnabled() const;
    const PlasticityFlags& getPlasticityFlags() const;
    
    // Activity monitoring
    float getFiringRate() const;                      // Current firing rate (Hz)
    bool isActive() const;                            // Check if neuron is currently active
    bool shouldSpike(double currentTime) const;       // Check if neuron should spike
    
    // Connectivity management
    void addIncomingSynapse(SynapseHandle synapse);    // Add incoming synapse
    void addOutgoingSynapse(SynapseHandle synapse);   // Add outgoing synapse
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // State management
    void saveState();                                 // Save current state
    void restoreState();                              // Restore saved state
    void resetToDefaults();                          // Reset to initial state
    
    // Statistics and monitoring
    uint32_t getSpikeCount() const;                   // Total spikes fired
    double getLastInterSpikeInterval() const;          // Time since last spike
    float getAverageFiringRate(size_t window) const;   // Average firing rate
    
    // Configuration
    void setRefractoryPeriod(uint32_t periods);        // Set refractory period
    void setAdaptation(float weight, float reversal);  // Set adaptation parameters
    
    // Debugging and analysis
    std::string debugInfo() const;                    // Get debug information
    void logState() const;                            // Log current state
};
```

## Neuron Types

### Internal Neurons

```cpp
void Neuron::setType(NeuronType::Internal) {
    // Characteristics:
    // - Higher threshold for stability
    // - Stronger adaptation
    // - Complex connectivity patterns
    // - Integration of multiple inputs
}
```

### Sensory Neurons

```cpp
void Neuron::setType(NeuronType::Sensory) {
    // Characteristics:
    // - Lower threshold for responsiveness
    // - Weak adaptation
    // - Specialized input channels
    // - Fast response dynamics
}
```

### Motor Neurons

```cpp
void Neuron::setType(NeuronType::Motor) {
    // Characteristics:
    // - Higher threshold for controlled output
    // - Moderate adaptation
    // - Output-specific connectivity
    // - Saccadic response patterns
}
```

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticity (STDP)

```cpp
void Neuron::update(double timestep) {
    // Apply STDP rules
    if (hasPlasticityEnabled().stdp) {
        applySTDP(timestep);
    }
}

void Neuron::applySTDP(double currentTime) {
    // LTP (Long-Term Potentiation)
    if (preSpikeTime_ > 0 && currentTime - preSpikeTime_ < LTP_WINDOW) {
        strengthenSynapticConnections();
    }
    
    // LTD (Long-Term Depression)
    if (postSpikeTime_ > 0 && currentTime - postSpikeTime_ < LTD_WINDOW) {
        weakenSynapticConnections();
    }
}
```

### Hebbian Learning

```cpp
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool structural) {
    if (hebbian) {
        coActivateWithPreSynapticNeurons();
    }
}
```

## Spike Generation Algorithm

### Basic LIF Spike Generation

```cpp
bool Neuron::shouldSpike(double currentTime) const {
    const NeuronState& state = getState();
    
    // Check if membrane potential exceeds threshold
    if (state.membranePotential >= state.threshold) {
        // Check if neuron is not in refractory period
        if (!state.isRefractory()) {
            return true;
        }
    }
    
    return false;
}

void Neuron::processSpikes() {
    NeuronState& state = getState();
    double currentTime = getTimestamp();
    
    if (shouldSpike(currentTime)) {
        // Record spike
        state.lastSpikeTime = currentTime;
        state.firingState = FiringState::Spiking;
        state.firingRate = 1.0 / (currentTime - state.lastSpikeTime);
        
        // Trigger post-synaptic neurons
        triggerPostSynapticSpikes();
        
        // Apply synaptic plasticity
        if (hasPlasticityEnabled().stdp) {
            applyPostSynapticPlasticity(currentTime);
        }
        
        // Reset membrane potential
        state.membranePotential = state.resetPotential;
    }
}
```

## Memory Management

### Pimpl Idiom Implementation

```cpp
class Neuron {
private:
    struct Impl {
        NeuronId id;
        NeuronType type;
        NeuronState state;
        std::vector<SynapseHandle> incomingSynapses;
        std::vector<SynapseHandle> outgoingSynapses;
        PlasticityFlags plasticityFlags;
        
        // LIF parameters
        static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
        static constexpr float TIME_CONSTANT = 20.0f;
        static constexpr size_t MAX_SPIKE_HISTORY = 100;
        
        Impl() : id(), type(NeuronType::Internal), state() {}
    };
    
    std::unique_ptr<Impl> pImpl;
};
```

## Performance Optimization

### Vectorized Operations

```cpp
// Batch processing of multiple neurons
class NeuronBatch {
public:
    // Vectorized membrane potential update
    static void updateMembranePotentials(NeuronState* states, 
                                         const std::vector<MembranePotential>& inputs,
                                         double timestep, size_t count);
    
    // Vectorized spike detection
    static void detectSpikes(const NeuronState* states,
                             std::vector<Timestamp>& spikeTimes,
                             double currentTime, size_t count);
};
```

### Memory Pools

```cpp
class NeuronMemoryPool {
public:
    static Neuron* allocate(NeuronId id);
    static void deallocate(Neuron* neuron);
    static void clear();
    static size_t getUsedCount();
};
```

## Thread Safety

### Read-Only Operations

```cpp
// Thread-safe read operations
NeuronId Neuron::getId() const { return pImpl->id; }
NeuronType Neuron::getType() const { return pImpl->type; }
const NeuronState& Neuron::getState() const { return pImpl->state; }
```

### Write Operations

```cpp
// Write operations require external synchronization
void Neuron::injectCurrent(MembranePotential current) {
    // Thread-safe if accessed from single thread
    pImpl->state.membranePotential += current;
}
```

## Usage Examples

### Basic Neuron Creation and Configuration

```cpp
#include <nlm/brain/Neuron.hpp>

int main() {
    // Create sensory neuron
    auto sensoryNeuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{1});
    sensoryNeuron->setType(nlm::NeuronType::Sensory);
    
    // Configure membrane properties
    sensoryNeuron->setMembraneProperties(
        -70.0f,  // resting potential (mV)
        -55.0f,  // threshold (mV)
        -70.0f,  // reset potential (mV)
        10.0f    // leak conductance (nS)
    );
    
    // Enable plasticity
    sensoryNeuron->enablePlasticity(true, true, false);
    
    // Set refractory period
    sensoryNeuron->setRefractoryPeriod(5);
    
    return 0;
}
```

### Neuron Simulation Loop

```cpp
#include <nlm/brain/Neuron.hpp>
#include <nlm/brain/Synapse.hpp>

int main() {
    // Create neurons
    auto preNeuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{1});
    auto postNeuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{2});
    
    // Configure neurons
    preNeuron->setType(nlm::NeuronType::Internal);
    postNeuron->setType(nlm::NeuronType::Internal);
    
    // Create synapse
    auto synapse = std::make_shared<nlm::Synapse>(preNeuron->getId(), postNeuron->getId());
    synapse->setWeight(0.5f);
    
    // Connect neurons
    preNeuron->addOutgoingSynapse(std::make_shared<nlm::SynapseHandle>(synapse->getId()));
    postNeuron->addIncomingSynapse(std::make_shared<nlm::SynapseHandle>(synapse->getId()));
    
    // Simulation loop
    double timestep = 0.001;
    double currentTime = 0.0;
    
    for (int step = 0; step < 1000; ++step) {
        // Update pre-synaptic neuron
        preNeuron->update(timestep);
        preNeuron->processSpikes();
        
        // Propagate spikes through synapses
        preNeuron->transmitSpikes(currentTime);
        
        // Update post-synaptic neuron
        postNeuron->update(timestep);
        postNeuron->processSpikes();
        
        // Advance time
        currentTime += timestep;
    }
    
    return 0;
}
```

### Activity Monitoring and Analysis

```cpp
#include <nlm/brain/Neuron.hpp>

void analyzeNeuronActivity(const nlm::Neuron& neuron) {
    // Get firing statistics
    uint32_t spikeCount = neuron.getSpikeCount();
    double avgFiringRate = neuron.getAverageFiringRate(100); // Last 100 steps
    
    // Check for abnormal activity
    if (neuron.isActive()) {
        float firingRate = neuron.getFiringRate();
        if (firingRate > 100.0f) {  // High firing rate
            std::cout << "Warning: High firing rate detected" << std::endl;
        }
    }
    
    // Log detailed information
    neuron.logState();
    
    // Get debug information
    std::string debugInfo = neuron.debugInfo();
    std::cout << debugInfo << std::endl;
}
```

## Error Handling

### Neuron State Validation

```cpp
bool Neuron::shouldSpike(double currentTime) const {
    const NeuronState& state = getState();
    
    // Validate state consistency
    if (state.threshold < state.restingPotential) {
        // Error: Invalid threshold configuration
        return false;
    }
    
    // Check spike conditions
    return (state.membranePotential >= state.threshold && 
            !state.isRefractory());
}
```

### Memory Safety

```cpp
synapse->setWeight(2.5f); // This should be clamped to valid range
// Better implementation:
float clampedWeight = std::clamp(weight, 0.0f, 1.0f);
synapse->setWeight(clampedWeight);
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/brain/Neuron.hpp>
#include <gtest/gtest.h>

TEST(NeuronTest, MembranePotentialUpdate) {
    auto neuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{1});
    
    // Set initial conditions
    neuron->setMembraneProperties(-70.0f, -55.0f, -70.0f, 10.0f);
    
    // Apply current
    neuron->injectCurrent(5.0f);
    
    // Update
    neuron->update(0.001);
    
    // Check membrane potential increased
    EXPECT_GT(neuron->getMembranePotential(), -70.0f);
}

TEST(NeuronTest, SpikeGeneration) {
    auto neuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{1});
    
    // Set high threshold for testing
    neuron->setMembraneProperties(-70.0f, -50.0f, -70.0f, 10.0f);
    
    // Inject enough current to reach threshold
    neuron->injectCurrent(30.0f);
    neuron->update(0.001);
    
    // Check if neuron spiked
    EXPECT_TRUE(neuron->hasSpiked());
}
```

### Performance Testing

```bash
// Run neuron performance tests
./benchmark --neuron --test spike_generation

// Profile neuron update performance
./profiler --target neuron --operation update
```

## Migration Guide

### From Earlier Versions

Neuron API changes:

- **Version 0.1.0**: Basic neuron model with simple state management
- **Version 0.2.0**: Added plasticity controls and activity monitoring
- **Version 0.3.0**: Enhanced membrane dynamics and spike generation
- **Version 0.4.0**: Improved memory management with Pimpl idiom
- **Version 0.5.0**: Added vectorized operations and performance optimizations
- **Version 0.6.0**: Enhanced type safety and thread safety guarantees

### API Changes

- **Membrane potential management**: Changed from simple float to NeuronState struct
- **Plasticity control**: Expanded from simple boolean to PlasticityFlags struct
- **Memory management**: Transition to Pimpl idiom for encapsulation
- **Thread safety**: Added explicit thread safety documentation
- **Performance optimization**: Added vectorized operations support
- **Error handling**: Enhanced error checking and validation

## References

1. [Leaky Integrate-and-Fire Model](https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model)
2. [Spike-Timing-Dependent Plasticity](https://en.wikipedia.org/wiki/Spike-timing-dependent_plasticity)
3. [Neural Dynamics](https://en.wikipedia.org/wiki/Neural_dynamics)
4. [Neurons and Synapses](https://en.wikipedia.org/wiki/Neuron_and_synapse)

## See Also

- [Brain API](brain_api.md) - Brain integration with neurons
- [Synapse API](synapse_api.md) - Synaptic connection management
- [Types API](types_api.md) - Type definitions and enumerations
- [Plasticity API](plasticity_api.md) - Plasticity mechanism implementations

**Last updated:** October 18, 2026
**Version:** 0.6.0