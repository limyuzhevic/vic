# NLM Synapse Module API

## Overview

The Synapse Module implements the fundamental connection mechanism in the NLM neural simulation framework. Synapses are the physical links between neurons that enable information transmission and plasticity. The module provides biologically realistic synaptic dynamics with support for multiple plasticity mechanisms, transmission delays, and activity-dependent modulation.

## Key Features

- **Biologically accurate dynamics**: Spike-driven synaptic transmission with realistic delays
- **Multiple plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Activity-dependent transmission**: Modulated by pre-synaptic and post-synaptic activity
- **Transmission delays**: Realistic timing for spike propagation
- **Weight management**: Configurable synaptic strength with bounds
- **State tracking**: Comprehensive monitoring of synaptic activity and properties
- **Memory efficiency**: Sparse connectivity for large-scale networks
- **Performance optimization**: Vectorized operations and memory pools

## Core Classes

### SynapseId Type

```cpp
struct SynapseId {
    uint64_t value;
    
    // Comparisons
    bool operator==(const SynapseId& other) const;
    bool operator!=(const SynapseId& other) const;
    bool operator<(const SynapseId& other) const;
    
    // Hash for unordered containers
    size_t hash() const;
};
```

### SynapseHandle Type

```cpp
struct SynapseHandle {
    SynapseId id;
    bool isValid() const;
    void invalidate();
};
```

### Synapse Class

```cpp
class Synapse {
public:
    // Construction and lifecycle
    Synapse(NeuronId preNeuron, NeuronId postNeuron);
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Core properties
    SynapseId getId() const;                           // Get synapse identifier
    NeuronId getPreNeuronId() const;                   // Get pre-synaptic neuron
    NeuronId getPostNeuronId() const;                  // Get post-synaptic neuron
    
    // Synaptic strength
    float getWeight() const;                           // Get current weight
    void setWeight(float weight);                      // Set weight
    void setWeight(float weight, float maxWeight);     // Set with bounds checking
    
    // Transmission timing
    Delay getDelay() const;                            // Get transmission delay (ms)
    void setDelay(Delay delay);                        // Set delay
    
    // Plasticity control
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    bool hasPlasticityEnabled() const;
    const PlasticityFlags& getPlasticityFlags() const;
    
    // State tracking
    bool hasSpikePending() const;                      // Check for pending spike
    Timestamp getLastPreSpikeTime() const;            // Get last pre-synaptic spike
    Timestamp getLastPostSpikeTime() const;           // Get last post-synaptic spike
    float getPrePostSpikeInterval() const;             // Time between spikes
    
    // Activity monitoring
    uint32_t getSpikeCount() const;                    // Total spikes transmitted
    float getTransmissionEfficiency() const;            // Success rate of transmission
    bool isFunctional() const;                         // Check if synapse is functional
    
    // Transmission
    void transmitSpike();                              // Send spike to post-synaptic neuron
    void transmitSpike(Timestamp spikeTime);            // Send spike with specific time
    
    // Plasticity application
    void applyPrePostPlasticity(Timestamp preSpikeTime, Timestamp postSpikeTime);
    void applyHebbianLearning();                       // Apply Hebbian strengthening
    void applyStructuralPlasticity();                  // Apply structural changes
    
    // State management
    void reset();                                      // Reset synaptic state
    void saveState();                                  // Save current state
    void restoreState();                               // Restore saved state
    
    // Debugging and analysis
    std::string debugInfo() const;                     // Get debug information
    void logState() const;                             // Log current state
    
    // Configuration
    void setMaxWeight(float maxWeight);                 // Set maximum allowed weight
    void setMinWeight(float minWeight);                 // Set minimum allowed weight
    void setPlasticityParameters(float hebbianRate,
                                 float stdpLTP,
                                 float stdpLTD,
                                 float stdpTimeConstant);
};
```

### SynapseState Structure

```cpp
struct SynapseState {
    float weight;                                      // Current synaptic weight
    Delay delay;                                       // Transmission delay (ms)
    bool enabled;                                      // Whether synapse is enabled
    uint32_t spikeCount;                               // Number of spikes transmitted
    Timestamp lastPreSpike;                            // Time of last pre-synaptic spike
    Timestamp lastPostSpike;                           // Time of last post-synaptic spike
    float totalTransmission;                           // Total transmission success
    float totalFailures;                               // Total transmission failures
    
    SynapseState();
    void reset();
    float getEfficiency() const;
    bool isHealthy() const;
};
```

## Synaptic Transmission

### Basic Transmission Algorithm

```cpp
void Synapse::transmitSpike() {
    if (!isEnabled()) {
        return;
    }
    
    // Schedule post-synaptic spike delivery
    Timestamp deliveryTime = getCurrentTime() + getDelay();
    schedulePostSynapticSpike(deliveryTime);
    
    // Update statistics
    ++pImpl->state.spikeCount;
    ++pImpl->state.totalTransmission;
}

void Synapse::transmitSpike(Timestamp spikeTime) {
    // Same as above but with specific spike time
    Timestamp deliveryTime = spikeTime + getDelay();
    schedulePostSynapticSpike(deliveryTime);
    
    // Update pre-synaptic spike time
    pImpl->state.lastPreSpike = spikeTime;
}
```

### Delay-Based Transmission

```cpp
class DelayManager {
public:
    // Schedule delayed spike delivery
    void scheduleDelayedSpike(SynapseId synapseId, Timestamp spikeTime, Delay delay);
    
    // Cancel scheduled spike delivery
    void cancelDelayedSpike(SynapseId synapseId);
    
    // Process pending delayed spikes
    void processDelayedSpikes(Timestamp currentTime);
    
    // Query pending spikes
    std::vector<SynapseId> getPendingSpikes(Timestamp currentTime) const;
};
```

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticity (STDP)

```cpp
void Synapse::applyPrePostPlasticity(Timestamp preSpikeTime, Timestamp postSpikeTime) {
    if (!hasPlasticityEnabled().stdp) {
        return;
    }
    
    float timeDifference = postSpikeTime - preSpikeTime;
    
    if (timeDifference > 0) {
        // Pre before post (LTP - Long-Term Potentiation)
        applyLTP(timeDifference);
    } else {
        // Post before pre (LTD - Long-Term Depression)
        applyLTD(-timeDifference);
    }
}

void Synapse::applyLTP(float timeDifference) {
    float potentiation = calculateLTP(timeDifference);
    float newWeight = std::min(pImpl->state.weight + potentiation, 
                              pImpl->maxWeight);
    
    setWeight(newWeight);
}

void Synapse::applyLTD(float timeDifference) {
    float depression = calculateLTD(timeDifference);
    float newWeight = std::max(pImpl->state.weight - depression, 
                              pImpl->minWeight);
    
    setWeight(newWeight);
}
```

### Hebbian Learning

```cpp
void Synapse::applyHebbianLearning() {
    if (!hasPlasticityEnabled().hebbian) {
        return;
    }
    
    // Co-activation strengthening
    float hebbianIncrement = calculateHebbianIncrement();
    float newWeight = std::min(pImpl->state.weight + hebbianIncrement, 
                              pImpl->maxWeight);
    
    setWeight(newWeight);
}

float Synapse::calculateHebbianIncrement() const {
    // Implementation depends on co-activation history
    // Typically based on product of pre and post-synaptic activities
    return pImpl->preActivation * pImpl->postActivation * HEBBIAN_RATE;
}
```

### Structural Plasticity

```cpp
void Synapse::applyStructuralPlasticity() {
    if (!hasPlasticityEnabled().structural) {
        return;
    }
    
    // Activity-dependent formation/elimination
    float activity = getAverageActivity();
    
    if (activity < LOW_ACTIVITY_THRESHOLD) {
        // Eliminate synapse
        eliminate();
    } else if (activity > HIGH_ACTIVITY_THRESHOLD) {
        // Form new connections
        formNewConnections();
    }
}
```

## Plasticity Parameters

### STDP Parameters

```cpp
struct STDPParameters {
    float ltpWeight;        // LTP weight factor
    float ltdWeight;       // LTD weight factor
    float ltpTimeConstant;  // LTP time constant (ms)
    float ltdTimeConstant; // LTD time constant (ms)
    float ltpThreshold;     // LTP threshold (ms)
    float ltdThreshold;    // LTD threshold (ms)
    
    STDPParameters() {
        ltpWeight = 0.01f;
        ltdWeight = 0.012f;
        ltpTimeConstant = 20.0f;
        ltdTimeConstant = 20.0f;
        ltpThreshold = 10.0f;
        ltdThreshold = 10.0f;
    }
};
```

### Hebbian Parameters

```cpp
struct HebbianParameters {
    float learningRate;     // Hebbian learning rate
    float saturationLevel; // Maximum weight saturation
    float inhibitionFactor; // Competitive inhibition factor
    
    HebbianParameters() {
        learningRate = 0.01f;
        saturationLevel = 1.0f;
        inhibitionFactor = 0.1f;
    }
};
```

## Activity Tracking

### Spike Timing Information

```cpp
class SpikeTimingTracker {
public:
    // Record spike times
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    
    // Calculate intervals
    float getPrePostInterval() const;
    float getPostPreInterval() const;
    
    // Get recent spike history
    std::vector<Timestamp> getRecentPreSpikes(size_t count) const;
    std::vector<Timestamp> getRecentPostSpikes(size_t count) const;
    
    // Calculate average firing rates
    float getPreFiringRate(double timeWindow) const;
    float getPostFiringRate(double timeWindow) const;
};
```

### Transmission Efficiency

```cpp
float Synapse::getTransmissionEfficiency() const {
    if (pImpl->state.spikeCount == 0) {
        return 0.0f;
    }
    return pImpl->state.totalTransmission / 
           (pImpl->state.totalTransmission + pImpl->state.totalFailures);
}
```

## Memory Management

### Pimpl Idiom Implementation

```cpp
class Synapse {
private:
    struct Impl {
        SynapseId id;
        NeuronId preNeuronId;
        NeuronId postNeuronId;
        SynapseState state;
        STDPParameters stdpParams;
        HebbianParameters hebbianParams;
        SpikeTimingTracker timingTracker;
        
        // Plasticity flags
        PlasticityFlags plasticityFlags;
        
        // Weight limits
        float maxWeight;
        float minWeight;
        
        Impl(SynapseId id, NeuronId pre, NeuronId post)
            : id(id), preNeuronId(pre), postNeuronId(post), 
              maxWeight(1.0f), minWeight(0.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};
```

### Memory Pools

```cpp
class SynapseMemoryPool {
public:
    static Synapse* allocate(SynapseId id, NeuronId pre, NeuronId post);
    static void deallocate(Synapse* synapse);
    static void clear();
    static size_t getUsedCount();
};
```

## Performance Optimization

### Vectorized Operations

```cpp
// Batch processing of multiple synapses
class SynapseBatch {
public:
    // Vectorized weight updates
    static void updateWeights(Synapse* synapses, const std::vector<float>& newWeights,
                             size_t count);
    
    // Vectorized delay updates
    static void updateDelays(Synapse* synapses, const std::vector<Delay>& delays,
                            size_t count);
    
    // Vectorized transmission
    static void transmitSpikes(Synapse* synapses, Timestamp currentTime, size_t count);
};
```

### Parallel Processing

```cpp
// Thread-safe parallel processing
class ParallelSynapseProcessor {
public:
    static void processSynapses(std::function<void(Synapse&)> processor);
    static void updateWeights(std::function<float(Synapse&)> weightCalculator);
    
    static size_t getAvailableThreads();
    static void setThreadCount(size_t count);
};
```

## Thread Safety

### Read-Only Operations

```cpp
// Thread-safe read operations
SynapseId Synapse::getId() const { return pImpl->id; }
NeuronId Synapse::getPreNeuronId() const { return pImpl->preNeuronId; }
NeuronId Synapse::getPostNeuronId() const { return pImpl->postNeuronId; }
float Synapse::getWeight() const { return pImpl->state.weight; }
```

### Write Operations

```cpp
// Write operations require external synchronization
void Synapse::setWeight(float weight) {
    // Thread-safe if accessed from single thread
    pImpl->state.weight = std::clamp(weight, pImpl->minWeight, pImpl->maxWeight);
}
```

## Usage Examples

### Basic Synapse Creation and Configuration

```cpp
#include <nlm/brain/Synapse.hpp>

int main() {
    // Create synapse
    auto synapse = std::make_shared<nlm::Synapse>(nlm::NeuronId{1}, nlm::NeuronId{2});
    
    // Configure synaptic properties
    synapse->setWeight(0.5f);
    synapse->setDelay(2.0f); // 2ms delay
    
    // Enable plasticity mechanisms
    synapse->enablePlasticity(true, true, false);
    
    // Set STDP parameters
    synapse->setPlasticityParameters(0.01f, 0.02f, 0.015f, 20.0f);
    
    return 0;
}
```

### Synaptic Transmission

```cpp
#include <nlm/brain/Synapse.hpp>
#include <nlm/brain/Neuron.hpp>

int main() {
    // Create pre and post-synaptic neurons
    auto preNeuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{1});
    auto postNeuron = std::make_shared<nlm::Neuron>(nlm::NeuronId{2});
    
    // Create synapse
    auto synapse = std::make_shared<nlm::Synapse>(preNeuron->getId(), postNeuron->getId());
    synapse->setWeight(0.8f);
    
    // Simulate pre-synaptic spike
    preNeuron->injectCurrent(20.0f);
    preNeuron->update(0.001);
    preNeuron->processSpikes();
    
    // Transmit spike through synapse
    synapse->transmitSpike();
    
    // Simulate post-synaptic neuron receiving spike
    postNeuron->update(0.002);
    postNeuron->processSpikes();
    
    return 0;
}
```

### Plasticity Demonstration

```cpp
#include <nlm/brain/Synapse.hpp>

int main() {
    // Create synapse
    auto synapse = std::make_shared<nlm::Synapse>(nlm::NeuronId{1}, nlm::NeuronId{2});
    synapse->enablePlasticity(true, true, true);
    
    // Simulate STDP
    double currentTime = 0.0;
    double preSpikeTime = 10.0;
    double postSpikeTime = 10.5;
    
    // Pre before post (LTP)
    if (postSpikeTime > preSpikeTime) {
        synapse->applyPrePostPlasticity(preSpikeTime, postSpikeTime);
        std::cout << "LTP applied: weight = " << synapse->getWeight() << std::endl;
    }
    
    // Post before pre (LTD)
    double postTime = 0.0;
    double preTime = 0.5;
    if (preTime > postTime) {
        synapse->applyPrePostPlasticity(preTime, postTime);
        std::cout << "LTD applied: weight = " << synapse->getWeight() << std::endl;
    }
    
    return 0;
}
```

### Activity Monitoring

```cpp
#include <nlm/brain/Synapse.hpp>

void monitorSynapseActivity(const nlm::Synapse& synapse) {
    // Get statistics
    float efficiency = synapse.getTransmissionEfficiency();
    uint32_t spikeCount = synapse.getSpikeCount();
    float weight = synapse.getWeight();
    
    // Check health
    bool isHealthy = synapse.isFunctional();
    
    // Log activity
    std::cout << "Synapse " << synapse.getId() 
              << ": weight = " << weight
              << ", efficiency = " << efficiency
              << ", spikes = " << spikeCount
              << ", healthy = " << isHealthy << std::endl;
}
```

## Error Handling

### Synapse State Validation

```cpp
bool Synapse::isFunctional() const {
    if (pImpl->state.weight < 0.0f || pImpl->state.weight > 1.0f) {
        return false; // Invalid weight
    }
    
    if (pImpl->state.delay < 0.0f) {
        return false; // Invalid delay
    }
    
    return pImpl->state.enabled;
}
```

### Memory Safety

```cpp
void Synapse::setWeight(float weight) {
    // Ensure weight is within valid range
    float clampedWeight = std::clamp(weight, pImpl->minWeight, pImpl->maxWeight);
    
    // Validate before assignment
    if (clampedWeight < pImpl->minWeight || clampedWeight > pImpl->maxWeight) {
        throw std::invalid_argument("Invalid weight value");
    }
    
    pImpl->state.weight = clampedWeight;
}
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/brain/Synapse.hpp>
#include <gtest/gtest.h>

TEST(SynapseTest, WeightClamping) {
    auto synapse = std::make_shared<nlm::Synapse>(nlm::NeuronId{1}, nlm::NeuronId{2});
    
    // Test weight clamping
    synapse->setWeight(2.0f); // Should be clamped to max weight
    EXPECT_LE(synapse->getWeight(), 1.0f);
    
    synapse->setWeight(-0.5f); // Should be clamped to min weight
    EXPECT_GE(synapse->getWeight(), 0.0f);
}

TEST(SynapseTest, TransmissionEfficiency) {
    auto synapse = std::make_shared<nlm::Synapse>(nlm::NeuronId{1}, nlm::NeuronId{2});
    
    // Initially, efficiency should be 0 (no spikes transmitted)
    EXPECT_EQ(synapse->getTransmissionEfficiency(), 0.0f);
    
    // After successful transmission, efficiency should be > 0
    synapse->transmitSpike();
    EXPECT_GT(synapse->getTransmissionEfficiency(), 0.0f);
}
```

### Performance Testing

```bash
// Run synapse performance tests
./benchmark --synapse --test transmission

// Profile synapse update performance
./profiler --target synapse --operation plasticity
```

## Migration Guide

### From Earlier Versions

Synapse API changes:

- **Version 0.1.0**: Basic synaptic transmission with fixed delays
- **Version 0.2.0**: Added plasticity mechanisms (STDP, Hebbian)
- **Version 0.3.0**: Enhanced weight management and activity tracking
- **Version 0.4.0**: Added structural plasticity and error handling
- **Version 0.5.0**: Improved memory management with Pimpl idiom
- **Version 0.6.0**: Enhanced type safety, thread safety, and performance optimization

### API Changes

- **Transmission algorithm**: Changed from simple to event-driven with delays
- **Plasticity system**: Expanded from single mechanism to multiple integrated systems
- **Weight management**: Added bounds checking and validation
- **Activity tracking**: Added comprehensive statistics and monitoring
- **Memory management**: Transition to Pimpl idiom for encapsulation
- **Thread safety**: Added explicit thread safety documentation
- **Performance optimization**: Added vectorized operations and memory pools
- **Error handling**: Enhanced error checking and validation

## References

1. [Synaptic Plasticity](https://en.wikipedia.org/wiki/Synaptic_plasticity)
2. [Spike-Timing-Dependent Plasticity](https://en.wikipedia.org/wiki/Spike-timing-dependent_plasticity)
3. [Hebbian Learning](https://en.wikipedia.org/wiki/Hebbian_theory)
4. [Neural Transmission](https://en.wikipedia.org/wiki/Neural_transmission)

## See Also

- [Brain API](brain_api.md) - Brain integration with synapses
- [Neuron API](neuron_api.md) - Neuron management
- [Plasticity API](plasticity_api.md) - Plasticity mechanism implementations
- [Types API](types_api.md) - Type definitions and enumerations

**Last updated:** October 18, 2026
**Version:** 0.6.0