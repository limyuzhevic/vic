# NLM - Neural Learning Machine Extension Guide

**Comprehensive Guide for Adding New Components and Advanced Features**

## Table of Contents

1. [Quick Start: Running Tests and Demos](#quick-start)
2. [Architecture Overview](#architecture-overview)
3. [Adding New Neuron Types](#adding-new-neuron-types)
4. [Adding New Plasticity Rules](#adding-new-plasticity-rules)
5. [Adding New Neuromodulators](#adding-new-neuromodulators)
6. [Adding New Memory Systems](#adding-new-memory-systems)
7. [Creating Custom Environments](#creating-custom-environments)
8. [Adding New Agent Behaviors](#adding-new-agent-behaviors)
9. [Advanced Configuration](#advanced-configuration)
10. [Performance Optimization](#performance-optimization)
11. [Testing and Validation](#testing-and-validation)
12. [Troubleshooting](#troubleshooting)

## Quick Start

### Running Tests

```bash
# Run Phase 2 unit tests (neural computation basics)
./build/nlm --help
# Run specific tests: see test_*.cpp in tests/ directory

# Run Phase 3 demo
./build/nlm_phase3_demo 5 200

# Run Phase 6 integration demo
./build/nlm_phase6_demo
```

### Building Examples

```bash
# Standard build
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Debug build with all checks
mkdir build_debug && cd build_debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && make -j4

# With development features enabled
cmake .. -DNLM_ENABLE_DEVELOPMENT=ON -DNLM_ENABLE_PROFILING=ON
```

## Architecture Overview

The NLM framework follows a modular, component-based architecture:

```
┌─────────────────────────────────────────────────────────────────┐
│                          World Interface                          │
├─────────────────────────────────────────────────────────────────┤
│                ┌─────────────────┐           ┌─────────────────┐ │
│                │   Environment    │           │   Visualization │ │
│                └─────────────────┘           └─────────────────┘ │
│                             │                        │           │
│                             └────────────────────────┼───────────┤
│                                             ┌────▼─────┐        │
│                ┌────────────────────────────┤  Agent  ├───▶      │
│                │                            └─────────┘         │
│                │           ┌─────────────────┐               │
│                │           │   Sensory       │               │
│                │           │   Processing    │               │
│                └───────────┤ AgentBrain      │◀──────────────┘
│                          └─────────────────┘
│                             │           ┌─────────────────┐
│                             │           │   Motor        │ │
│                             └───────────┤   System       │ │
│                                     ┌────▼─────┐           │
│                                     │ Actions  │           │
│                                     └─────────┘           │
├─────────────────────────────────────────────────────────────────┤
│                      ┌─────────────────┐                       │
│                      │    Neural       │                       │
│  ┌───────────────────┤     Brain       │───────────────────────┤
│  │                   └─────────────────┘                       │
│  │                             │                                 │
│  │       ┌─────────────────────┼─────────────────────┐           │
│  │       │                     │                     │           │
│  │ ┌────▼─────┐           ┌────▼─────┐           ┌────▼─────┐     │
│  │ │  Brain   │◀─────────┤   Core   │◀─────────┤  Brain   │     │
│  │ │ Components├─────────┤ Systems │─────────┤ Interface │     │
│  │ └─────┬─────┘         └─────┬─────┘         └─────┬─────┘     │
│  │       │                     │                     │           │
│  │ ┌─────▼─────┐         ┌──────▼──────┐         ┌─────▼─────┐     │
│  │ │   Memory  │◀───────┤  Plasticity  │◀───────┤  Dynamics │     │
│  │ │ Systems   │         │   Systems    │         │  Systems  │     │
│  │ └───────────┘         └──────────────┘         └───────────┘     │
│  │                                                             │
│  │ ┌─────────────────┐         ┌─────────────────┐             │
│  │ │  Neuromod       │◀────────┤  Prediction     │◀────────────┤
│  │ │ ulation        │         │   Systems      │             │
│  │ └─────────────────┘         └─────────────────┘             │
│  │                                                           │
│  │ ┌─────────────────┐         ┌─────────────────┐             │
│  │ │   Cognition     │◀────────┤  Development    │◀────────────┤
│  │ │  Systems       │         │   Systems       │             │
│  │ └─────────────────┘         └─────────────────┘             │
└─────────────────────────────────────────────────────────────────┘
```

## Component Categories

### Core Components
- **Brain**: Main neural simulation coordinator
- **Regions/Populations**: Neural architecture organization
- **Neurons**: Computational units (LIF, custom types)
- **Synapses**: Connection mechanisms with plasticity

### Plasticity Systems
- **STDP**: Spike-timing dependent plasticity
- **Hebbian**: Activity-dependent strengthening
- **Structural**: Synaptogenesis and pruning
- **Reward-modulated**: Value-based learning

### Integration Systems
- **Memory**: Working, episodic, semantic, procedural
- **Prediction**: Forward models and error signals
- **Cognition**: Planning, attention, concept formation
- **Development**: Growth and maturation processes

### Control Systems
- **Neuromodulation**: Dopamine, curiosity, novelty
- **Motor**: Action selection and execution
- **Sensory**: Input processing and feature extraction

## Adding New Neuron Types

### Step 1: Create Neuron Class

Create a new header file in `src/brain/`:

```cpp
// src/brain/CustomNeuron.hpp
#pragma once

#include "Neuron.hpp"
#include <vector>
#include <string>

namespace nlm {

class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id);
    ~CustomNeuron() override;
    
    // Override virtual methods
    void stepLIF(Timestamp currentTime, TimestepDuration dt) override;
    void initializeRandom(RandomGenerator& rng) override;
    void reset() override;
    
    // Custom methods
    void setAdaptationParameter(float param);
    float getAdaptationParameter() const;
    void addInputModulator(float modulator);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

### Step 2: Implement Custom Dynamics

```cpp
// src/brain/CustomNeuron.cpp
#include "CustomNeuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

struct CustomNeuron::Impl {
    float adaptationParameter;
    std::vector<float> inputModulators;
    float customThreshold;
    float customMembraneCapacitance;
    
    Impl() : adaptationParameter(1.0f), customThreshold(-55.0f), 
             customMembraneCapacitance(1.0f) {}
};

CustomNeuron::CustomNeuron(NeuronId id) : Neuron(id), pImpl(new Impl) {}

CustomNeuron::~CustomNeuron() = default;

void CustomNeuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Custom LIF-like dynamics
    float V = getMembranePotential();
    float V_rest = getRestingPotential();
    float threshold = pImpl->customThreshold;
    float tau = 20.0f;  // Custom time constant
    float C = pImpl->customMembraneCapacitance;
    
    // Custom integration with adaptation
    float adaptation = pImpl->adaptationParameter * getFiringRate();
    float synapticInput = getSynapticInput();
    
    // Modified LIF equation
    float dV = (V_rest - V) / tau + (synapticInput - adaptation) / C;
    V += dV * static_cast<float>(dt) * 1000.0f;
    
    // Spike detection and reset
    if (V >= threshold) {
        recordSpike(currentTime);
        V = getResetPotential();
        // Custom reset dynamics
        pImpl->adaptationParameter += 0.5f;
    }
    
    setMembranePotential(V);
}

void CustomNeuron::initializeRandom(RandomGenerator& rng) {
    Neuron::initializeRandom(rng);
    pImpl->adaptationParameter = rng.uniformReal(0.5f, 2.0f);
    pImpl->customThreshold = -50.0f + rng.uniformReal(-10.0f, 10.0f);
    pImpl->customMembraneCapacitance = rng.uniformReal(0.5f, 2.0f);
}

} // namespace nlm
```

### Step 3: Register New Neuron Type

Update the Brain initialization to support the new neuron type:

```cpp
// In Brain::initialize()
void Brain::registerCustomNeuronTypes() {
    // Add to region configuration
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            // Register custom neuron type with the region
            region->registerNeuronType(NeuronType::Custom, [](NeuronId id) {
                return std::make_unique<CustomNeuron>(id);
            });
        }
    }
}
```

### Step 4: Usage in Experiments

```cpp
// In your experiment code
void useCustomNeuron(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        // Create custom neurons
        for (size_t i = 0; i < 10; ++i) {
            NeuronId id = region->addNeuron(NeuronType::Custom);
            auto* neuron = region->getNeuron(id);
            if (neuron) {
                static_cast<CustomNeuron*>(neuron)->setAdaptationParameter(1.5f);
            }
        }
    }
}
```

## Adding New Plasticity Rules

### Step 1: Create Plasticity Rule Interface

```cpp
// src/plasticity/PlasticityRule.hpp
#pragma once

#include "brain/Synapse.hpp"
#include <string>
#include <vector>

namespace nlm {

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    virtual std::string getName() const = 0;
    virtual void configure(const Config& config) = 0;
    virtual void update(Synapse* synapse, 
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) = 0;
    
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

} // namespace nlm
```

### Step 2: Implement Custom STDP Variant

```cpp
// src/plasticity/CustomSTDP.hpp
#pragma once

#include "PlasticityRule.hpp"
#include <cmath>

namespace nlm {

class CustomSTDP : public PlasticityRule {
public:
    CustomSTDP();
    ~CustomSTDP() override = default;
    
    std::string getName() const override { return "CustomSTDP"; }
    void configure(const Config& config) override;
    void update(Synapse* synapse,
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt) override;
    
    // Custom methods
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setTimeConstant(float tau) { timeConstant_ = tau; }
    void enableTripletRules(bool enable) { enableTriplet_ = enable; }
    
private:
    float learningRate_;
    float timeConstant_;
    bool enableTriplet_;
    bool enabled_;
};

} // namespace nlm
```

### Step 3: Implement Custom Rules

```cpp
// src/plasticity/CustomSTDP.cpp
#include "CustomSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

CustomSTDP::CustomSTDP() 
    : learningRate_(0.01f), timeConstant_(20.0f), 
      enableTriplet_(false), enabled_(true) {}

void CustomSTDP::configure(const Config& config) {
    learningRate_ = config.getOr<float>("custom_stdp_rate", 0.01f);
    timeConstant_ = config.getOr<float>("custom_stdp_tau", 20.0f);
    enableTriplet_ = config.getOr<bool>("enable_triplet_rules", false);
}

void CustomSTDP::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || !enabled_) return;
    
    float totalDelta = 0.0f;
    
    if (enableTriplet_) {
        // Implement triplet STDP rules
        totalDelta = computeTripletSTDP(synapse, preSpikes, postSpikes);
    } else {
        // Standard pairwise STDP
        totalDelta = computePairwiseSTDP(synapse, preSpikes, postSpikes);
    }
    
    // Apply learning rate
    totalDelta *= learningRate_;
    
    if (std::abs(totalDelta) > 1e-6f) {
        synapse->addToWeight(totalDelta);
    }
}

float CustomSTDP::computePairwiseSTDP(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) {
    float totalDelta = 0.0f;
    float tau = timeConstant_;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Potentiation
                float delta = std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Depression
                float delta = -std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float CustomSTDP::computeTripletSTDP(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) {
    // Simplified triplet STDP implementation
    // In practice, this would be more complex
    float totalDelta = computePairwiseSTDP(synapse, preSpikes, postSpikes);
    
    // Add triplet contributions
    // ... (implementation details)
    
    return totalDelta;
}

} // namespace nlm
```

### Step 4: Register Custom Plasticity

Update the Brain to use custom plasticity rules:

```cpp
// In Brain::initialize()
void Brain::setupPlasticitySystem() {
    // Add custom STDP
    auto customSTDP = std::make_unique<CustomSTDP>();
    customSTDP->configure(*pImpl->config);
    pImpl->plasticitySystem.push_back(std::move(customSTDP));
    
    // Add standard STDP
    auto stdp = std::make_unique<STDP>();
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);
    pImpl->plasticitySystem.push_back(std::move(stdp));
}
```

## Adding New Neuromodulators

### Step 1: Create Neuromodulator Base Class

```cpp
// src/neuromodulation/Neuromodulator.hpp
#pragma once

#include <string>
#include <memory>

namespace nlm {

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    virtual std::string getName() const = 0;
    virtual float getModulationLevel() const = 0;
    virtual void update(float timeStep, const BrainState& state) = 0;
    virtual void reset() = 0;
    
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;
};

} // namespace nlm
```

### Step 2: Implement Custom Neuromodulator

```cpp
// src/neuromodulation/CustomNeuromodulator.hpp
#pragma once

#include "Neuromodulator.hpp"
#include <queue>

namespace nlm {

class CustomNeuromodulator : public Neuromodulator {
public:
    CustomNeuromodulator();
    ~CustomNeuromodulator() override = default;
    
    std::string getName() const override { return "CustomNeuromodulator"; }
    float getModulationLevel() const override { return currentLevel_; }
    void update(float timeStep, const BrainState& state) override;
    void reset() override;
    
    // Custom methods
    void addEvent(NeuromodulationEvent event);
    void setResponseCurve(const ResponseCurve& curve);
    void enableBurstMode(bool enable);
    
private:
    float currentLevel_;
    std::queue<NeuromodulationEvent> eventQueue_;
    ResponseCurve responseCurve_;
    bool burstMode_;
    bool active_;
};

} // namespace nlm
```

### Step 3: Register and Use Neuromodulator

```cpp
// In Brain::initialize()
void Brain::setupNeuromodulation() {
    // Add custom neuromodulator
    auto customMod = std::make_unique<CustomNeuromodulator>();
    customMod->setResponseCurve(loadResponseCurve("custom_mod.json"));
    customMod->enableBurstMode(true);
    pImpl->neuromodulators.push_back(std::move(customMod));
    
    // Add standard neuromodulators
    pImpl->neuromodulators.push_back(std::make_unique<Dopamine>());
    pImpl->neuromodulators.push_back(std::make_unique<Curiosity>());
}
```

## Adding New Memory Systems

### Step 1: Create Memory System Base

```cpp
// src/memory/MemorySystem.hpp
#pragma once

#include <string>
#include <memory>
#include <vector>

namespace nlm {

class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    virtual std::string getName() const = 0;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(float timeStep) = 0;
    virtual void reset() = 0;
    
    virtual size_t getMemoryLoad() const = 0;
    virtual float getConsolidationStrength() const = 0;
    
    virtual bool shouldReplay() const = 0;
    virtual std::vector<Episode> getReplaySequence() = 0;
};

} // namespace nlm
```

### Step 2: Implement Custom Memory System

```cpp
// src/memory/CustomMemorySystem.hpp
#pragma once

#include "MemorySystem.hpp"
#include <unordered_map>
#include <chrono>

namespace nlm {

class CustomMemorySystem : public MemorySystem {
public:
    CustomMemorySystem();
    ~CustomMemorySystem() override = default;
    
    std::string getName() const override { return "CustomMemorySystem"; }
    void initialize(Brain* brain) override;
    void update(float timeStep) override;
    void reset() override;
    
    // Custom methods
    void setRetentionPolicy(const RetentionPolicy& policy);
    void enableMetaLearning(bool enable);
    void set consolidationThreshold(float threshold);
    
private:
    Brain* brain_;
    RetentionPolicy policy_;
    bool metaLearningEnabled_;
    float consolidationThreshold_;
    std::unordered_map<NeuronId, Episode> memoryMap_;
    std::chrono::system_clock::time_point lastCleanup_;
};

} // namespace nlm
```

## Creating Custom Environments

### Step 1: Create Environment Base Class

```cpp
// src/environment/CustomEnvironment.hpp
#pragma once

#include "Environment.hpp"
#include <vector>

namespace nlm {

class CustomEnvironment : public Environment {
public:
    CustomEnvironment();
    ~CustomEnvironment() override = default;
    
    std::string getName() const override { return "CustomEnvironment"; }
    void initialize(int width, int height) override;
    void reset() override;
    SensoryPercept observe(AgentBrain* agent) override;
    float computeReward(AgentBrain* agent) override;
    void applyAction(AgentBrain* agent, MotorCommand action) override;
    
    // Custom environment dynamics
    virtual void updateEnvironment(float timeStep);
    virtual void setWorldParameters(const WorldParameters& params);
    
private:
    WorldParameters worldParams_;
    std::vector<std::vector<CellState>> grid_;
    std::vector<DynamicObject> objects_;
};

} // namespace nlm
```

## Adding New Agent Behaviors

### Step 1: Create Agent Behavior Base

```cpp
// src/agent/CustomBehavior.hpp
#pragma once

#include <string>
#include <memory>

namespace nlm {

class AgentBehavior {
public:
    virtual ~AgentBehavior() = default;
    
    virtual std::string getName() const = 0;
    virtual void initialize(AgentBrain* agent) = 0;
    virtual MotorCommand selectAction(const SensoryPercept& percept) = 0;
    virtual void update(float timeStep, const ActionResult& result) = 0;
    
    virtual void setParameter(std::string name, float value) = 0;
    virtual float getParameter(std::string name) const = 0;
};

} // namespace nlm
```

### Step 2: Implement Custom Behavior

```cpp
// src/agent/ExplorationBehavior.hpp
#pragma once

#include "CustomBehavior.hpp"
#include <random>

namespace nlm {

class ExplorationBehavior : public AgentBehavior {
public:
    ExplorationBehavior();
    ~ExplorationBehavior() override = default;
    
    std::string getName() const override { return "ExplorationBehavior"; }
    void initialize(AgentBrain* agent) override;
    MotorCommand selectAction(const SensoryPercept& percept) override;
    void update(float timeStep, const ActionResult& result) override;
    
    // Custom exploration strategies
    void setExplorationStrategy(ExplorationStrategy strategy);
    void setExplorationRate(float rate);
    
private:
    AgentBrain* agent_;
    ExplorationStrategy strategy_;
    float explorationRate_;
    std::mt19937 rng_;
    std::discrete_distribution<> actionDistribution_;
};

} // namespace nlm
```

## Advanced Configuration

### Configuration Schema

Advanced configuration uses JSON format for complex parameter sets:

```json
{
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.15,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "ltp_weight": 0.02,
                "ltd_weight": 0.015,
                "time_constant": 20.0
            },
            "hebbian": {
                "enabled": true,
                "learning_rate": 0.01
            },
            "structural": {
                "enabled": true,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 500,
            "decay_rate": 0.95
        },
        "episodic": {
            "max_episodes": 10000,
            "consolidation_interval": 1000
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.1,
            "phasic_response": true
        },
        "curiosity": {
            "baseline": 0.05,
            "learning_rate": 0.02
        }
    }
}
```

### Configuration Validation

```cpp
// src/core/Config/AdvancedConfigValidator.hpp
#pragma once

#include "Config.hpp"
#include <string>
#include <vector>

namespace nlm {

class AdvancedConfigValidator {
public:
    static bool validateConfig(const Config& config, std::vector<std::string>& errors);
    static Config optimizeForPerformance(const Config& config, PerformanceGoal goal);
    static Config mergeConfigs(const Config& base, const Config& override);
    
private:
    static bool validatePlasticityConfig(const Config& config, std::vector<std::string>& errors);
    static bool validateMemoryConfig(const Config& config, std::vector<std::string>& errors);
    static bool validateNeuromodulationConfig(const Config& config, std::vector<std::string>& errors);
};

} // namespace nlm
```

## Performance Optimization

### Profiling Tools

```cpp
// src/core/PerformanceMonitor.hpp
#pragma once

#include <chrono>
#include <unordered_map>
#include <string>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        std::chrono::duration<double> totalTime;
        std::unordered_map<std::string, std::chrono::duration<double>> stageTimes;
        size_t spikeCount;
        size_t synapseCount;
    };
    
    static PerformanceMonitor& getInstance();
    
    void startMeasurement(const std::string& stage);
    void stopMeasurement(const std::string& stage);
    PerformanceMetrics getCurrentMetrics() const;
    void reset();
    
    void enableProfiling(bool enable);
    bool isProfilingEnabled() const;
    
    void saveReport(const std::string& filename) const;
    
private:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    bool profilingEnabled_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> stageStartTimes_;
    PerformanceMetrics currentMetrics_;
};

} // namespace nlm
```

### Memory Pool Optimization

```cpp
// src/core/MemoryPool.hpp
#pragma once

#include <memory>
#include <vector>
#include <mutex>

namespace nlm {

template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t initialCapacity = 1024, size_t growthFactor = 2);
    ~MemoryPool();
    
    T* allocate();
    void deallocate(T* ptr);
    void clear();
    
    size_t getAllocatedCount() const { return allocatedCount_; }
    size_t getCapacity() const { return pool_.size(); }
    
private:
    struct Block {
        T* data;
        size_t size;
        Block* next;
        Block(T* d, size_t s) : data(d), size(s), next(nullptr) {}
    };
    
    std::vector<T*> pool_;
    Block* freeList_;
    size_t allocatedCount_;
    size_t capacity_;
    std::mutex mutex_;
};

} // namespace nlm
```

## Testing and Validation

### Automated Testing Framework

```cpp
// tests/test_custom_components.cpp
#include "custom_components/CustomNeuron.hpp"
#include "custom_components/CustomSTDP.hpp"
#include <gtest/gtest.h>

TEST(CustomNeuron, BasicFunctionality) {
    CustomNeuron neuron(1);
    neuron.initializeRandom(randomGenerator);
    
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), neuron.getRestingPotential());
    EXPECT_TRUE(neuron.getFiringState() == FiringState::Resting);
}

TEST(CustomSTDP, LearningRule) {
    CustomSTDP customSTDP;
    customSTDP.configure(config);
    
    // Create test synapse
    Synapse synapse(1, 2, 3);
    
    // Apply learning
    std::vector<Timestamp> preSpikes = {100, 200, 300};
    std::vector<Timestamp> postSpikes = {150, 250};
    customSTDP.update(&synapse, preSpikes, postSpikes, 0.001);
    
    EXPECT_TRUE(synapse.getWeight() != 0.0f);
}
```

### Performance Benchmarks

```cpp
// benchmarks/brain_performance.cpp
#include "brain/Brain.hpp"
#include <benchmark/benchmark.h>
#include <thread>

static void BM_BrainStep(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        brain->step(state.iterations);
    }
}
BENCHMARK(BM_BrainStep);

static void BM_PlasticityUpdate(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        // Trigger plasticity updates
        brain->triggerPlasticity();
    }
}
BENCHMARK(BM_PlasticityUpdate);
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Memory Leaks

**Symptoms**: Memory usage grows over time, application crashes

**Solutions**:
- Use memory pools for frequent allocations
- Implement proper cleanup in destructors
- Check for circular references

```cpp
// Use memory pool instead of new/delete
MemoryPool<Neuron> neuronPool;
Neuron* neuron = neuronPool.allocate();
neuronPool.deallocate(neuron);
```

#### 2. Performance Degradation

**Symptoms**: Slow simulation, long experiment times

**Solutions**:
- Profile with PerformanceMonitor
- Optimize plasticity algorithms
- Use parallel processing for independent regions

```cpp
// Enable parallel processing
PerformanceMonitor::getInstance().enableProfiling(true);
// In Brain::step()
for (auto& region : pImpl->regions) {
    std::thread(&NeuralRegion::step, region.get());
}
```

#### 3. Configuration Errors

**Symptoms**: Application fails to start, unexpected behavior

**Solutions**:
- Use AdvancedConfigValidator for validation
- Log all configuration changes
- Provide default values for missing configs

```cpp
std::vector<std::string> errors;
if (!AdvancedConfigValidator::validateConfig(config, errors)) {
    for (const auto& error : errors) {
        NLM_LOG_ERROR(error);
    }
    // Set safe defaults
    config->set("neuron_count", 1000);
}
```

#### 4. Custom Component Integration Issues

**Symptoms**: Segmentation faults, unexpected behavior

**Solutions**:
- Test components independently
- Use proper memory management
- Validate inputs in all public methods

```cpp
// In CustomNeuron::stepLIF()
if (!std::isfinite(V) || V > 100.0f) {
    NLM_LOG_WARNING("Unstable membrane potential: " + std::to_string(V));
    V = getRestingPotential();  // Reset to stable state
}
```

### Debug Scripts

#### Memory Leak Detection

```bash
#!/bin/bash
# detect_memory_leaks.sh

# Compile with memory debugging
CFLAGS="-g -fsanitize=address -Wall"

cd build
make clean
make $CFLAGS

# Run with memory profiler
./nlm --neurons 1000 --steps 100 2>&1 | tee debug_output.txt

# Check for memory issues
if [ -f debug_output.txt ]; then
    if grep -q "ERROR: memory" debug_output.txt; then
        echo "Potential memory issues detected"
        echo "Review debug_output.txt for details"
    fi
fi
```

#### Performance Profiling

```bash
#!/bin/bash
# profile_performance.sh

# Compile with profiling
CMAKE_FLAGS="-DNLM_ENABLE_PROFILING=ON -DCMAKE_BUILD_TYPE=Release"
mkdir build_profile && cd build_profile
cmake $CMAKE_FLAGS ..
make -j4

# Run with performance monitoring
./nlm --neurons 2000 --steps 500 --log-level debug > performance.log 2>&1

# Generate performance report
python3 -c "
import sys
import json
from pathlib import Path

log_file = Path('performance.log')
if log_file.exists():
    # Parse performance log
    # Generate performance report
    print('Performance analysis complete')
"
```

## Further Reading

### Research Papers
- [Deep Learning in Neural Networks: Fifty Years of Progress](https://proceedings.neurips.cc/paper/2015/hash/06b0b15a817f91b4ce7d576403e0b426-Abstract.html)
- [Short-Term Synaptic Plasticity](https://www.nature.com/articles/nrn1761)
- [Predictive Coding in the Brain](https://www.sciencedirect.com/science/article/pii/S0959438412000355)

### Documentation
- [NLM Architecture Documentation](https://github.com/nlm-project/nlm/docs/ARCHITECTURE.md)
- [Phase 6 Integration Guide](https://github.com/nlm-project/nlm/docs/PHASE6_FINAL_AUDIT.md)
- [Building and Installation](https://github.com/nlm-project/nlm/docs/INSTALL.md)

### Tutorials
1. [Getting Started with NLM](https://github.com/nlm-project/nlm/tutorials/GettingStarted.md)
2. [Building Custom Neurons](https://github.com/nlm-project/nlm/tutorials/CustomNeurons.md)
3. [Advanced Plasticity Rules](https://github.com/nlm-project/nlm/tutorials/AdvancedPlasticity.md)

## Acknowledgments

This extension guide was developed based on:
- The original NLM framework architecture
- Community feedback and feature requests
- Best practices in neural network engineering
- Modern C++ development standards

For issues, feature requests, or contributions, please visit the NLM project repository.

---

**Version**: 0.1.0  
**Last Updated**: $(date)  
**Contact**: nlm-project@nlm.org  
**License**: MIT