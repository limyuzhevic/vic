# NLM Examples and Tutorials

## Overview

This document provides examples and tutorials for using NLM (Neural Learning Machine) across different use cases. It covers:

1. Basic neural simulation examples
2. Learning and plasticity experiments  
3. Agent-environment interaction
4. Configuration examples
5. Testing examples

## Basic Examples

### Example 1: Simple Neural Activity

```cpp
// Create brain with configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 100);
config->set("simulation_timestep", 0.001);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Run simulation
for (int step = 0; step < 100; ++step) {
    brain->step(step);
    if (step % 25 == 0) {
        NLM_LOG_INFO("Step " + std::to_string(step) + 
                    ": Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    }
}
```

### Example 2: Neuron Dynamics

```cpp
// Create and test a neuron
Neuron neuron;
neuron.setMembranePotential(-70.0);
neuron.injectCurrent(50.0f);
neuron.update(0.001);

EXPECT_TRUE(neuron.getMembranePotential() > -70.0);
```

## Learning Examples

### Example 3: Hebbian Learning

```cpp
void runHebbianExperiment(std::shared_ptr<Brain> brain) {
    // Enable Hebbian plasticity
    auto region = brain->getRegion(RegionId(0));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, false, false);
        }
    }
    
    // Run correlated activity
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
    }
}
```

### Example 4: STDP (Spike-Timing-Dependent Plasticity)

```cpp
void runSTDPExperiment(std::shared_ptr<Brain> brain) {
    // Enable STDP
    auto region = brain->getRegion(RegionId(0));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(false, true, false);
        }
    }
    
    // Create pre-before-post spike timing
    // ... (implementation details)
}
```

## Agent-Environment Examples

### Example 5: Complete Agent

```cpp
void runAgentExample() {
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("plasticity.enable", true);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run simulation
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        ": Curiosity: " + std::to_string(agent->getCuriosityLevel()));
        }
    }
}
```

## Configuration Examples

### Example 6: Research Configuration

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.0001,
            "neuron_count": 10000,
            "region_count": 10
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.0001,
                "tau_plus": 30.0,
                "tau_minus": 30.0
            }
        }
    }
}
```

### Example 7: Testing Configuration

```json
{
    "nlm": {
        "simulation": {
            "timestep": 0.01,
            "neuron_count": 100
        },
        "plasticity": {
            "stdp": {
                "enable": false
            }
        }
    }
}
```

## Python Examples

### Example 8: Python Basic Usage

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 25 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 9: Python Learning

```python
import pynlm

# Create brain with learning
config = pynlm.createDefaultConfig()
config.set("plasticity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity in region
for region in brain.getRegions():
    for synapse in region.getSynapses():
        synapse.enablePlasticity(True, True, True)

# Run learning simulation
for step in range(1000):
    brain.step(step)
```

## Testing Examples

### Example 10: Unit Test Structure

```cpp
class TestNeuron : public TestCase {
public:
    std::string getName() const override {
        return "neuron_dynamics_test";
    }
    
    std::string getDescription() const override {
        return "Test Leaky Integrate-and-Fire neuron dynamics";
    }
    
    bool run() override {
        Neuron neuron;
        neuron.setMembranePotential(-70.0);
        neuron.injectCurrent(100.0f);
        neuron.update(0.001);
        
        EXPECT_TRUE(neuron.getMembranePotential() > -70.0);
        return true;
    }
};
```

### Example 11: Integration Test

```cpp
class TestIntegration : public TestCase {
public:
    std::string getName() const override {
        return "system_integration_test";
    }
    
    std::string getDescription() const override {
        return "Test complete brain system integration";
    }
    
    bool run() override {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test basic functionality
        if (brain->getTotalNeuronCount() == 0) {
            return false;
        }
        
        return true;
    }
};
```

## Summary

This document provides examples for:

1. **Basic neural simulation**: Simple brain activity without world interaction
2. **Learning and plasticity**: Hebbian learning, STDP, and synaptic changes
3. **Agent-environment interaction**: Complete agent with world simulation
4. **Configuration**: Research and testing configurations
5. **Python API**: High-level Python usage examples
6. **Testing**: Unit tests and integration tests

Each example demonstrates practical usage of NLM features and can be used as a starting point for your own NLM applications.

## Further Resources

- [Getting Started Guide](getting-started.md) - Comprehensive introduction
- [Python API Documentation](python-api.md) - Complete API reference
- [C++ Developer Guide](cpp-developer.md) - Advanced C++ development
- [Configuration Guide](configuration.md) - Configuration management
- [Testing Guide](testing.md) - Testing framework and best practices
- [Architecture Guide](ARCHITECTURE.md) - System architecture
- [Science.md](SCIENCE.md) - Scientific background

These examples and documentation should provide a comprehensive foundation for working with NLM across different use cases and skill levels.
