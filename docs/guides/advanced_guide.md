# NLM Advanced Guide

## Overview

This guide provides advanced information for experienced NLM users and developers. It covers extension points, performance optimization, configuration management, and advanced usage patterns.

## Table of Contents

1. [Introduction](#introduction)
2. [Extension Points](#extension-points)
3. [Performance Optimization](#performance-optimization)
4. [Configuration Management](#configuration-management)
5. [Advanced Brain Operations](#advanced-brain-operations)
6. [Memory System Configuration](#memory-system-configuration)
7. [Neuromodulation Configuration](#neuromodulation-configuration)
8. [Development Configuration](#development-configuration)
9. [Multi-Agent Systems](#multi-agent-systems)
10. [Custom Worlds and Environments](#custom-worlds-and-environments)
11. [Debugging and Profiling](#debugging-and-profiling)
12. [Migration Guide](#migration-guide)
13. [Future Development](#future-development)
14. [References](#references)

## Introduction

This guide is intended for users who are familiar with basic NLM operations and want to:

- Extend NLM with custom components
- Optimize performance for large-scale simulations
- Configure advanced features
- Debug and profile applications
- Migrate from older versions

## Extension Points

### Extending Plasticity Rules

NLM provides a framework for adding custom plasticity rules. Here's how to extend the system.

#### Creating Custom Plasticity Rule

```cpp
#include "plasticity/PlasticityRule.hpp"

class CustomPlasticityRule : public PlasticityRule {
public:
    void update(Synapse& synapse, Neuron* pre, Neuron* post,
                float timeSincePre, float timeSincePost) override {
        // Custom plasticity update logic
        float weightChange = calculateWeightChange(pre, post, timeSincePre, timeSincePost);
        
        // Apply weight change with bounds checking
        synapse.setWeight(std::clamp(synapse.getWeight() + weightChange,
                                   getMinWeight(), getMaxWeight()));
    }
    
    static PlasticityRule* create() {
        return new CustomPlasticityRule();
    }
    
    static float getMinWeight() { return -1.0f; }
    static float getMaxWeight() { return 1.0f; }
};
```

#### Registering Custom Plasticity Rule

```cpp
// In your extension source file
void registerCustomPlasticityRule() {
    PlasticityFactory::registerRule("custom_rule", &CustomPlasticityRule::create);
}

// Call in your extension initialization
NLM_EXPORT_PLUGIN(registerCustomPlasticityRule);
```

### Adding New Neuron Types

```cpp
#include "brain/NeuronDynamics.hpp"

class CustomNeuronType : public NeuralDynamics {
public:
    void initialize(Neuron* neuron) override {
        // Custom neuron initialization
        neuron->setVRest(-65.0f);
        neuron->setVReset(-70.0f);
        neuron->setThreshold(-50.0f);
        // ... other initialization
    }
    
    void step(Neuron* neuron, float dt) override {
        // Custom dynamics update
        updateCustomDynamics(neuron, dt);
        
        // Check for spike
        if (neuron->getVMembrane() >= neuron->getThreshold()) {
            neuron->setVMembrane(neuron->getVReset());
            neuron->incrementSpikeCount();
            onSpike(neuron);
        }
    }
    
    bool shouldSpike(Neuron* neuron) override {
        return neuron->getVMembrane() >= neuron->getThreshold();
    }
    
    void onSpike(Neuron* neuron) override {
        // Custom spike handling
        neuron->setRefractoryTimer(neuron->getRefractoryPeriod());
        // ... custom spike logic
    }
    
    static NeuralDynamics* create() {
        return new CustomNeuronType();
    }
};
```

### Adding New Memory Systems

```cpp
#include "memory/Memory.hpp"

class CustomMemorySystem : public Memory {
public:
    void store(const Experience& experience) override {
        // Custom storage logic
        experiences.push_back(experience);
        
        // Apply storage consolidation
        consolidate(experience);
    }
    
    Experience retrieve(const TimeWindow& window) override {
        // Custom retrieval logic
        return findMatchingExperience(window);
    }
    
    void consolidate() override {
        // Custom consolidation logic
        for (auto& experience : experiences) {
            strengthenConnections(experience);
        }
    }
    
    void update(float timeStep) override {
        // Custom update logic
        updateInternalState(timeStep);
    }
    
    static Memory* create() {
        return new CustomMemorySystem();
    }
    
private:
    std::vector<Experience> experiences;
    float consolidationStrength = 1.0f;
};
```

### Adding New Neuromodulators

```cpp
#include "neuromodulation/Neuromodulator.hpp"

class CustomNeuromodulator : public Neuromodulator {
public:
    void initialize(const NeuromodulationConfig& config) override {
        strength = config.getFloat("strength", 1.0f);
        decayRate = config.getFloat("decay_rate", 0.95f);
        // ... other initialization
    }
    
    float computeSignal(const BrainState& state) override {
        // Custom neuromodulator signal computation
        return computeCustomSignal(state) * strength;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Custom neuromodulator effect application
        applyCustomEffect(brain, signal);
    }
    
    static Neuromodulator* create() {
        return new CustomNeuromodulator();
    }
    
private:
    float strength;
    float decayRate;
};
```

## Performance Optimization

### Configuration Optimization

#### Optimize for Large Brains

```cpp
// Configuration for large-scale simulations
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.05)  // Sparse connectivity
config.set("performance.threads", 8)       // Use multiple threads
config.set("performance.sparse_connectivity", true)
config.set("performance.simd_optimization", true)
config.set("performance.memory_pool_size", 16384)
```

#### Optimize for Memory Usage

```cpp
// Configuration for memory-constrained systems
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.01)  // Very sparse
config.set("performance.memory_pool_size", 512)
config.set("performance.event_queue_size", 1024)
config.set("performance.parallel_plasticity", false)
```

### Code Optimization Techniques

#### Use SIMD for Neural Computations

```cpp
// SIMD-optimized neural computation
void processNeuronsSIMD(Neuron* neurons, size_t count, float dt) {
    // Use SIMD intrinsics for parallel computation
    __m128 v_rest = _mm_set1_ps(-70.0f);
    __m128 v_thresh = _mm_set1_ps(-50.0f);
    
    for (size_t i = 0; i < count; i += 4) {
        // Process 4 neurons at once
        __m128 v_mem = _mm_loadu_ps(&neurons[i].getVMembrane());
        __m128 v_leak = _mm_set1_ps(1.0f / neurons[i].getTauMem());
        
        // Apply LIF dynamics
        v_mem = v_mem + (v_rest - v_mem) * v_leak * dt;
        
        // Check for spikes
        __m128 spike_mask = _mm_cmpge_ps(v_mem, v_thresh);
        v_mem = _mm_and_ps(v_mem, _mm_cmpeq_ps(spike_mask, spike_mask)); // Reset spikes
        
        _mm_storeu_ps(&neurons[i].getVMembrane(), v_mem);
    }
}
```

#### Memory Pool Optimization

```cpp
// Custom memory pool for neuron objects
class NeuronMemoryPool {
public:
    NeuronMemoryPool(size_t capacity) {
        pool = new Neuron[capacity];
        freeList = new size_t[capacity];
        poolSize = capacity;
        nextFree = 0;
        
        // Initialize free list
        for (size_t i = 0; i < capacity; ++i) {
            freeList[i] = i;
        }
    }
    
    Neuron* allocate() {
        if (nextFree >= poolSize) {
            return nullptr; // Pool exhausted
        }
        return &pool[freeList[nextFree++]];
    }
    
    void deallocate(Neuron* neuron) {
        // Find neuron in pool and return to free list
        size_t index = neuron - pool;
        if (index < poolSize) {
            freeList[--nextFree] = index;
        }
    }
    
    ~NeuronMemoryPool() {
        delete[] pool;
        delete[] freeList;
    }
    
private:
    Neuron* pool;
    size_t* freeList;
    size_t poolSize;
    size_t nextFree;
};
```

#### Event-Driven Optimization

```cpp
// Event-driven spike processing
class SpikeEventProcessor {
public:
    void scheduleSpike(Neuron* neuron, float time) {
        SpikeEvent event;
        event.neuron = neuron;
        event.time = time;
        
        // Insert into priority queue
        spikeQueue.push(event);
    }
    
    void processEvents(float currentTime) {
        while (!spikeQueue.empty() && spikeQueue.top().time <= currentTime) {
            const SpikeEvent& event = spikeQueue.top();
            
            // Process spike
            processSpike(event.neuron, event.time);
            
            spikeQueue.pop();
        }
    }
    
private:
    std::priority_queue<SpikeEvent> spikeQueue;
};
```

### Build Optimization

#### Custom CMake Configuration

```bash
# Optimized build configuration
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto" \
         -DENABLE_PYTHON_BINDINGS=ON \
         -DENABLE_VISUALIZATION=OFF \
         -DENABLE_TESTS=ON \
         -DPYTHON_EXECUTABLE=$(which python3) \
         -DPYTHON_MODULE_NAME=pynlm \
         -DBUILD_SHARED_LIBS=ON
```

#### Build Scripts

```bash
#!/bin/bash
# build_nlm.sh

set -e

# Configuration
BUILD_TYPE="Release"
PYTHON_BINDING="ON"
VISUALIZATION="OFF"
NUM_JOBS=$(nproc)

# Create build directory
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
         -DENABLE_PYTHON_BINDINGS=$PYTHON_BINDING \
         -DENABLE_VISUALIZATION=$VISUALIZATION \
         -DCMAKE_CXX_FLAGS="-O3 -march=native"

# Build
make -j$NUM_JOBS

# Run tests
if [ "$PYTHON_BINDING" = "ON" ]; then
    ctest --output-on-failure
fi

# Install Python bindings
if [ -f "../python/setup.py" ]; then
    cd ../python
    pip install -e .
fi
```

## Configuration Management

### Configuration Templates

```python
# Configuration templates for different use cases

# Template: High-performance simulation
HIGH_PERFORMANCE_CONFIG = {
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.05,
        "excitatory_ratio": 0.85
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0001
        }
    },
    "performance": {
        "threads": 8,
        "memory_pool_size": 8192,
        "event_queue_size": 16384,
        "sparse_connectivity": True,
        "simd_optimization": True
    }
}

# Template: Learning-focused simulation
LEARNING_CONFIG = {
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.1,
        "excitatory_ratio": 0.8
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5,
            "enable": True
        },
        "curiosity": {
            "enable": True,
            "scale": 1.2
        },
        "novelty": {
            "enable": True,
            "sensitivity": 0.8
        }
    },
    "memory": {
        "working": {
            "enable": True,
            "capacity": 0.7
        },
        "episodic": {
            "enable": True,
            "capacity": 100
        }
    }
}

# Template: Real-time interaction
REAL_TIME_CONFIG = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.2,
        "v_thresh": -45.0,  // Lower threshold for faster response
        "tau_mem": 10.0     // Faster dynamics
    },
    "performance": {
        "threads": 4,
        "timestep": 0.01,  // Smaller timestep
        "memory_pool_size": 2048
    },
    "world": {
        "timestep": 0.01,
        "vision_width": 16,
        "vision_height": 16
    }
}
```

### Configuration Validation

```python
class ConfigurationValidator {
public:
    static bool validate(const Config& config) {
        bool valid = true;
        
        // Check brain configuration
        if (!validateBrainConfig(config)) {
            valid = false;
        }
        
        // Check plasticity configuration
        if (!validatePlasticityConfig(config)) {
            valid = false;
        }
        
        // Check memory configuration
        if (!validateMemoryConfig(config)) {
            valid = false;
        }
        
        // Check performance configuration
        if (!validatePerformanceConfig(config)) {
            valid = false;
        }
        
        return valid;
    }
    
private:
    static bool validateBrainConfig(const Config& config) {
        int neuronCount = config.getInt("brain.neuron_count", 0);
        if (neuronCount < 100 || neuronCount > 10000) {
            std::cout << "Error: Invalid neuron count. Must be between 100 and 10000." << std::endl;
            return false;
        }
        
        float synapseDensity = config.getFloat("brain.synapse_density", 0.0f);
        if (synapseDensity < 0.01 || synapseDensity > 0.5) {
            std::cout << "Error: Invalid synapse density. Must be between 0.01 and 0.5." << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Similar validation methods for other configurations
};
```

### Configuration Diffing

```python
class ConfigurationDiff {
public:
    static std::vector<std::string> diff(const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        
        // Get all keys from both configurations
        std::vector<std::string> allKeys;
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        allKeys.insert(allKeys.end(), keys1.begin(), keys1.end());
        allKeys.insert(allKeys.end(), keys2.begin(), keys2.end());
        
        std::sort(allKeys.begin(), allKeys.end());
        allKeys.erase(std::unique(allKeys.begin(), allKeys.end()), allKeys.end());
        
        // Compare each key
        for (const std::string& key : allKeys) {
            if (config1.has(key) && config2.has(key)) {
                // Both have the key, check if values differ
                auto value1 = config1.get(key);
                auto value2 = config2.get(key);
                
                if (value1 != value2) {
                    differences.push_back(f"Key '{key}': {value1} -> {value2}");
                }
            } else if (config1.has(key)) {
                // Only in config1
                auto value = config1.get(key);
                differences.push_back(f"Key '{key}': Only in config1, value = {value}");
            } else {
                // Only in config2
                auto value = config2.get(key);
                differences.push_back(f"Key '{key}': Only in config2, value = {value}");
            }
        }
        
        return differences;
    }
};
```

## Advanced Brain Operations

### Custom Brain Regions

```cpp
// Add custom brain regions with specialized properties
class CustomRegion {
public:
    CustomRegion(const std::string& name, size_t neuronCount, float specialization) {
        this->name = name;
        this->neuronCount = neuronCount;
        this->specialization = specialization;
        
        // Create neurons with specialized properties
        createSpecializedNeurons();
    }
    
    void createSpecializedNeurons() {
        for (size_t i = 0; i < neuronCount; ++i) {
            Neuron* neuron = new Neuron();
            
            // Apply specialization based on region type
            if (specialization > 0.7) {
                // High specialization: specific neuron type
                neuron->setNeuronType(NeuronType::Sensory);
                neuron->setThreshold(-40.0f);  // Lower threshold for sensory neurons
            } else if (specialization > 0.4) {
                // Medium specialization: balanced properties
                neuron->setNeuronType(NeuronType::Excitatory);
                neuron->setThreshold(-50.0f);
            } else {
                // Low specialization: generic neurons
                neuron->setNeuronType(NeuronType::Modulatory);
                neuron->setThreshold(-60.0f);
            }
            
            neurons.push_back(neuron);
        }
    }
    
    void connectToRegion(CustomRegion* otherRegion, float connectionProbability) {
        for (Neuron* pre : neurons) {
            for (Neuron* post : otherRegion->neurons) {
                if (randomFloat() < connectionProbability) {
                    createSynapse(pre, post);
                }
            }
        }
    }
    
private:
    std::string name;
    size_t neuronCount;
    float specialization;
    std::vector<Neuron*> neurons;
};
```

### Advanced Plasticity Scheduling

```cpp
class AdvancedPlasticityScheduler {
public:
    void schedulePlasticityUpdate(Brain& brain, float timeStep) {
        // Priority-based plasticity scheduling
        std::vector<PlasticityRule*> rules = getHighPriorityRules(brain);
        
        for (PlasticityRule* rule : rules) {
            applyPlasticityRule(rule, brain, timeStep);
        }
        
        // Update plasticity rule priorities based on learning progress
        updateRulePriorities(brain);
    }
    
private:
    std::vector<PlasticityRule*> getHighPriorityRules(Brain& brain) {
        std::vector<PlasticityRule*> highPriorityRules;
        
        for (auto& rule : plasticityRules) {
            if (rule->shouldApply(brain)) {
                highPriorityRules.push_back(rule.get());
            }
        }
        
        // Sort by priority
        std::sort(highPriorityRules.begin(), highPriorityRules.end(),
                 [this](PlasticityRule* a, PlasticityRule* b) {
                     return getRulePriority(a) > getRulePriority(b);
                 });
        
        return highPriorityRules;
    }
    
    float getRulePriority(PlasticityRule* rule) {
        // Calculate priority based on various factors
        float priority = 1.0f;
        
        // Factor 1: Recent learning activity
        priority *= getRecentLearningActivity(rule);
        
        // Factor 2: Error signal
        priority *= getErrorSignal();
        
        // Factor 3: Neuromodulation level
        priority *= getNeuromodulationLevel();
        
        return priority;
    }
};
```

## Memory System Configuration

### Working Memory Configuration

```python
# Advanced working memory configuration
WORKING_MEMORY_CONFIG = {
    "enable": True,
    "capacity": 0.8,  // 80% of maximum capacity
    "decay_rate": 0.95,  // 95% decay per timestep
    "recurrent_strength": 0.9,  // Strong recurrent connections
    "competition_strength": 0.8,  // Strong competition for attention
    "update_frequency": 1,  // Update every timestep
    "consolidation": {
        "enable": True,
        "strength": 0.3,  // Consolidation strength
        "delay": 10,  // Delay before consolidation
        "trigger_threshold": 0.5  // Threshold for consolidation
    }
}
```

### Episodic Memory Configuration

```python
# Advanced episodic memory configuration
EPISODIC_MEMORY_CONFIG = {
    "enable": True,
    "capacity": 1000,  // Store up to 1000 episodes
    "encoding_strength": 1.0,
    "retrieval_success_rate": 0.8,
    "replay_frequency": 0.1,  // Replay 10% of the time
    "replay_strength": 0.5,
    "consolidation": {
        "enable": True,
        "rate": 0.01,  // Consolidation rate
        "stability_threshold": 0.6,
        "labilization": True
    },
    "pattern_completion": {
        "enable": True,
        "similarity_threshold": 0.7,
        "max_patterns": 50
    }
}
```

### Semantic Memory Configuration

```python
# Advanced semantic memory configuration
SEMANTIC_MEMORY_CONFIG = {
    "enable": True,
    "capacity": 200,  // Number of concepts
    "concept_update_rate": 0.001,
    "concept_stability": 0.9,
    "category_depth": 3,  // Depth of category hierarchy
    "association_strength": 0.7,
    "inferrence_enabled": True,
    "generalization_enabled": True,
    "transfer_enabled": True
}
```

## Neuromodulation Configuration

### Dopamine System Configuration

```python
# Advanced dopamine configuration
DOPAMINE_CONFIG = {
    "scale": 2.0,  // Stronger dopamine effects
    "enable": True,
    "prediction_weight": 0.7,  // Heavier weight on prediction error
    "reward_weight": 0.3,  // Lighter weight on reward
    "noise_level": 0.05,  // Add some noise for exploration
    "adaptation_rate": 0.02,  // Rate of adaptation
    "effect_duration": 100,  // Duration of dopamine effects
    "target_plasticity_rules": ["stdp", "hebbian"],  // Which rules dopamine affects
    "target_memory_systems": ["working", "episodic"],  // Which memory systems it affects
    "baseline": 0.1  // Baseline dopamine level
}
```

### Curiosity Configuration

```python
# Advanced curiosity configuration
CURIOSITY_CONFIG = {
    "enable": True,
    "scale": 1.5,  // Stronger curiosity signals
    "noise": 0.02,  // Less noise for more stable curiosity
    "baseline": 0.2,  // Higher baseline curiosity
    "prediction_error_weight": 0.6,
    "novelty_weight": 0.4,
    "adaptation_rate": 0.01,
    "exploration_threshold": 0.3,
    "exploitation_threshold": 0.7,
    "min_curiosity": 0.01,
    "max_curiosity": 1.0,
    "decay_rate": 0.98
}
```\n
### Novelty Configuration

```python
# Advanced novelty configuration
NOVELTY_CONFIG = {
    "enable": True,
    "sensitivity": 0.8,  // Higher sensitivity
    "decay": 0.9,  // Slower decay
    "threshold": 0.3,  // Lower threshold for novelty detection
    "integration_strength": 0.7,  // Strong integration with other systems
    "history_size": 1000,  // Larger history for comparison
    "similarity_metric": "euclidean",  // Similarity metric
    "weight_by_importance": True,
    "update_frequency": 1  // Update every timestep
}
```

## Development Configuration

### Development Stage Configuration

```python
# Advanced development configuration
DEVELOPMENT_CONFIG = {
    "enable": True,
    "stage_duration": 200.0,  // Longer development stages
    "initial_plasticity": 2.0,  // Higher initial plasticity
    "final_plasticity": 0.05,  // Lower final plasticity
    "adaptation_rate": 0.02,  // Slower adaptation
    "critical_period_enabled": True,
    "critical_period_duration": 50.0,
    "critical_period_plasticity": 5.0,  // Very high plasticity during critical period
    "maturation_enabled": True,
    "maturation_rate": 0.01,
    "aging_enabled": False,
    "aging_rate": 0.001
}
```

### Plasticity Development Configuration

```python
# Advanced plasticity development configuration
PLASTICITY_DEVELOPMENT_CONFIG = {
    "enable": True,
    "rate_by_stage": [2.0, 1.5, 1.0, 0.5, 0.1],  // Stage-specific plasticity rates
    "initial_LTP": 0.02,
    "initial_LTD": 0.02,
    "LTP_maturation": 0.01,
    "LTD_maturation": 0.01,
    "structural_maturation": 0.005,
    "rew modulation_sensitivity": 1.0,
    "novelty_sensitivity": 0.8
}
```

## Multi-Agent Systems

### Multi-Agent Configuration

```python
# Multi-agent system configuration
MULTI_AGENT_CONFIG = {
    "num_agents": 5,
    "social_learning_enabled": True,
    "communication_enabled": True,
    "competition_enabled": True,
    "cooperation_enabled": True,
    "social_influence_strength": 0.3,
    "observation_noise": 0.1,
    "imitation_accuracy": 0.8,
    "shared_environment": True,
    "independent_worlds": False,
    "resource_sharing": True,
    "conflict_resolution": "negotiation"
}
```

### Multi-Agent Simulation

```python
class MultiAgentSimulation {
public:
    void run(const MultiAgentConfig& config) {
        // Create agents
        std::vector<Agent> agents;
        for (int i = 0; i < config.num_agents; ++i) {
            Agent agent = createAgent(i, config);
            agents.push_back(agent);
        }
        
        // Initialize agents
        for (Agent& agent : agents) {
            agent.initialize(sharedWorld);
        }
        
        // Run simulation
        for (int step = 0; step < maxSteps; ++step) {
            // Update world
            world.update(timestep);
            
            // Process each agent
            for (Agent& agent : agents) {
                // Get observation
                Observation observation = world.observe(agent);
                
                // Add observation noise if enabled
                if (config.observation_noise > 0) {
                    observation = addNoise(observation, config.observation_noise);
                }
                
                // Process sensory input
                agent.processSensoryInput(observation);
                
                // Get action
                Action action = agent.decodeMotorCommand();
                
                // Apply action
                world.applyAction(agent, action);
                
                // Social learning
                if (config.social_learning_enabled) {
                    learnFromOthers(agent, agents);
                }
            }
            
            // Record metrics
            recordStepMetrics(step, agents);
        }
        
        // Analyze results
        analyzeResults(agents);
    }
    
private:
    Agent createAgent(int id, const MultiAgentConfig& config) {
        // Create agent with specific characteristics
        Agent agent;
        agent.setId(id);
        
        // Set agent properties based on ID for diversity
        agent.setCuriosityBase(0.1 + (id * 0.1));  // Different curiosity levels
        agent.setLearningRate(0.01 + (id * 0.005));  // Different learning rates
        agent.setRiskTolerance(id * 0.1);  // Different risk tolerances
        
        // Create brain
        Config agentConfig = createAgentConfig(id, config);
        agent.setBrain(std::make_unique<Brain>(agentConfig));
        agent.getBrain()->initialize();
        
        return agent;
    }
    
    void learnFromOthers(Agent& agent, const std::vector<Agent>& others) {
        // Social learning implementation
        for (const Agent& other : others) {
            if (agent.canObserve(other)) {
                // Observe other's behavior
                Action observedAction = other.getLastAction();
                
                // Imitate with probability based on similarity
                if (shouldImitate(agent, other, observedAction)) {
                    agent.imitateAction(observedAction);
                }
            }
        }
    }
    
    bool shouldImitate(const Agent& agent, const Agent& other, const Action& action) {
        // Imitation probability based on similarity
        float similarity = calculateSimilarity(agent, other);
        float imitationProbability = config.social_influence_strength * similarity;
        
        return randomFloat() < imitationProbability;
    }
};
```

## Custom Worlds and Environments

### Creating Custom Worlds

```cpp
class CustomWorld : public SimpleWorld {
public:
    CustomWorld() : SimpleWorld() {
        // Custom world initialization
        setWidth(50);
        setHeight(50);
        setVisionWidth(12);
        setVisionHeight(12);
    }
    
    void update(float timestep) override {
        // Custom world update logic
        SimpleWorld::update(timestep);
        
        // Add custom dynamics
        updateCustomDynamics(timestep);
        
        // Apply custom rules
        applyCustomRules();
    }
    
    SensoryPercept observe(const Agent& agent) override {
        // Custom observation logic
        SensoryPercept percept = SimpleWorld::observe(agent);
        
        // Add custom sensory information
        percept.customSignal = computeCustomSignal(agent);
        
        return percept;
    }
    
    void applyAction(const Agent& agent, const Action& action) override {
        // Custom action application
        SimpleWorld::applyAction(agent, action);
        
        // Apply custom action effects
        applyCustomActionEffects(agent, action);
    }
    
private:
    void updateCustomDynamics(float timestep) {
        // Custom world dynamics
        // Implement custom physics, biology, or other dynamics
    }
    
    float computeCustomSignal(const Agent& agent) {
        // Compute custom signal for observation
        return 0.0f;
    }
    
    void applyCustomActionEffects(const Agent& agent, const Action& action) {
        // Apply custom effects of actions
    }
};
```

### Environment Configuration

```python
# Custom environment configuration
CUSTOM_ENVIRONMENT_CONFIG = {
    "world_type": "grid",  // "grid", "continuous", "graph"
    "physics_enabled": True,
    "biology_enabled": False,
    "resource_dynamics": True,
    "hazard_dynamics": True,
    "social_dynamics": False,
    "temperature_enabled": False,
    "humidity_enabled": False,
    "time_of_day_enabled": False,
    "weather_enabled": False,
    "seasonal_changes": False,
    "random_events_enabled": False,
    "random_event_probability": 0.01
}
```

## Debugging and Profiling

### Debugging Tools

#### NLM Debug Console

```python
# Interactive debugging console
class NLMDebugConsole {
public:
    void start() {
        print("=== NLM Debug Console ===");
        print("Available commands:");
        print("  help - Show help information");
        print("  status - Show system status");
        print("  brain - Brain operations");
        print("  world - World operations");
        print("  agent - Agent operations");
        print("  config - Configuration operations");
        print("  exit - Exit debug console");
        
        while (running) {
            string command = readInput(">>> ");
            processCommand(command);
        }
    }
    
private:
    void processCommand(const string& command) {
        if (command == "exit") {
            running = false;
        } else if (command == "status") {
            showSystemStatus();
        } else if (command.find("brain ") == 0) {
            processBrainCommand(command.substr(6));
        } else if (command.find("world ") == 0) {
            processWorldCommand(command.substr(6));
        } else if (command.find("agent ") == 0) {
            processAgentCommand(command.substr(6));
        } else {
            print("Unknown command. Type 'help' for available commands.");
        }
    }
    
    void showSystemStatus() {
        // Show system status
        print("System Status:");
        print(f"  Brain neurons: {brain.total_neuron_count}");
        print(f"  Brain synapses: {brain.total_synapse_count}");
        print(f"  World size: {world.width}x{world.height}");
        print(f"  Agent energy: {agent.getEnergy()}");
        print(f"  Simulation time: {world.simulation_time}");
    }
    
    // More command processing methods...
    
    bool running = true;
};
```

#### Logging and Tracing

```cpp
// Comprehensive logging system
class NLMLogger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        std::chrono::steady_clock::time_point timestamp;
        LogLevel level;
        std::string message;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    void log(LogLevel level, const std::string& message) {
        LogEntry entry;
        entry.timestamp = std::chrono::steady_clock::now();
        entry.level = level;
        entry.message = message;
        
        // Write to log file
        writeToFile(entry);
        
        // Write to console if level is high enough
        if (level >= consoleLevel) {
            writeToConsole(entry);
        }
        
        // Send to remote log server if configured
        if (logServerEnabled) {
            sendToServer(entry);
        }
    }
    
    void setLevel(LogLevel level) {
        consoleLevel = level;
    }
    
    void enableTracing(const std::string& traceFile) {
        // Enable detailed tracing
        traceEnabled = true;
        traceFileName = traceFile;
    }
    
    void startTrace(const std::string& event) {
        if (traceEnabled) {
            TraceEvent traceEvent;
            traceEvent.event = event;
            traceEvent.startTime = std::chrono::steady_clock::now();
            currentTrace = traceEvent;
        }
    }
    
    void endTrace() {
        if (traceEnabled && currentTrace.event.empty()) {
            TraceEvent traceEvent = currentTrace;
            traceEvent.endTime = std::chrono::steady_clock::now();
            
            // Write trace to file
            writeTraceToFile(traceEvent);
            
            currentTrace = TraceEvent();
        }
    }
    
private:
    LogLevel consoleLevel = LogLevel::INFO;
    bool traceEnabled = false;
    std::string traceFileName;
    bool logServerEnabled = false;
    std::string logServerAddress;
    int logServerPort = 0;
    std::vector<LogEntry> logBuffer;
    TraceEvent currentTrace;
};
```

### Performance Profiling

#### Profiling with Perf

```bash
# Profile NLM with Linux perf
perf record -g ./nlm --neurons 1000 --steps 1000
perf report

# Profile specific functions
perf record -e cpu-clock,task-clock,instructions,cycles,cache-references,cache-misses ./nlm
perf report -p $(pgrep nlm)

# Generate profiling data
perf script > perf.data
fasttrace -i perf.data > fftrace.txt
```

#### Profiling with Valgrind

```bash
# Memory profiling
valgrind --tool=massif --stacktrace=yes ./nlm
ms_print massif.out.12345 > massif_report.html

# Memory leak detection
valgrind --tool=memcheck --leak-check=full ./nlm
valgrind --tool=memcheck --track-origins=yes ./nlm

# Thread debugging
valgrind --tool=helgrind ./nlm
valgrind --tool=drd ./nlm
```

#### Python Profiling

```python
import cProfile
import pstats
import pynlm

def profileBrainSimulation() {
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000) {
        brain.step(step)
    }
}

# Profile with cProfile
profiler = cProfile.Profile()
profiler.enable()
profileBrainSimulation()
profiler.disable()

# Write profile to file
profiler.dump_stats("brain_simulation.prof")

# Print profile summary
stats = pstats.Stats("brain_simulation.prof")
stats.sort_stats("cumulative")
stats.print_stats(20)
```

## Migration Guide

### From NLM Phase 5 to Phase 6

#### Key Changes in Phase 6

1. **Memory Systems**: Now fully integrated with brain loop
2. **Prediction System**: Connected to sensory processing
3. **Neuromodulation**: All neuromodulators fully implemented
4. **Development**: Development affects plasticity, attention, and memory
5. **Persistence**: CheckpointSystem connected to Brain save/load

#### Migration Steps

```python
// Phase 5 code (memory systems disconnected)
WorkingMemory* wm = brain->getWorkingMemory();  // Returns nullptr
if (wm) {
    wm->store(pattern);
}

// Phase 6 code (memory systems integrated)
if (auto* wm = brain->getWorkingMemory()) {
    wm->store(pattern, priority=1.0);
}
```

#### Configuration Changes

```python
// Phase 5 configuration
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

// Phase 6 configuration (additional options)
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);
config.set("memory.working.enable", true);
config.set("memory.episodic.enable", true);
config.set("neuromod.dopamine.scale", 1.0);
config.set("neuromod.curiosity.enable", true);
config.set("neuromod.novelty.enable", true);
```

### From NLM 1.0 to 2.0

#### API Changes

```cpp
// NLM 1.0 API
brain->step(step, time);  // Old API

// NLM 2.0 API
brain->step(step, time);  // Same API

// NLM 1.0 configuration
brain->setConfig(config);  // Old API

// NLM 2.0 configuration
brain->setConfig(config);  // Same API
```

#### Configuration Schema Changes

```python
// NLM 1.0 configuration schema
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    }
}

// NLM 2.0 configuration schema (additional sections)
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        }
    },
    "memory": {
        "working": {
            "enable": true
        }
    }
}
```

### Backward Compatibility

NLM 2.0 maintains backward compatibility:

- **Core APIs**: All existing APIs remain unchanged
- **Configuration**: New configuration options are optional
- **Memory Systems**: Existing code will continue to work
- **Neuromodulation**: Enhanced but not required
- **Development**: New features are opt-in

## Future Development

### Planned Features for NLM 3.0

1. **Enhanced Memory Systems**
   - Hierarchical memory architecture
   - Cross-modal memory integration
   - Memory compression and compression

2. **Advanced Neuromodulation**
   - Serotonin and norepinephrine systems
   - Acetylcholine for attention
   - Comprehensive neuromodulation architecture

3. **Learning Enhancements**
   - Meta-learning and learning-to-learn
   - Transfer learning capabilities
   - Continual learning without catastrophic forgetting

4. **Development Extensions**
   - Multi-stage development
   - Experience-dependent development
   - Epigenetic influences

5. **Social Cognition**
   - Theory of mind
   - Empathy and perspective taking
   - Social learning and cultural transmission

### Research Directions

#### Biological Inspiration

- **Hippocampal-cortical interactions**: Memory consolidation
- **Basal ganglia-thalamocortical loops**: Action selection
- **Prefrontal cortex working memory**: Cognitive control
- **Amygdala fear conditioning**: Emotional learning

#### Computational Approaches

- **Predictive coding**: Hierarchical prediction models
- **Active inference**: Bayesian brain hypothesis
- **Free energy principle**: Minimizing prediction errors
- **Internal models**: Forward models of the world

#### Experimental Paradigms

- **Adaptive task switching**: Demonstrate cognitive flexibility
- **Multi-modal integration**: Combine sensory inputs
- **Social learning**: Learn from others
- **Development over lifetime**: Age-dependent changes

## References

### Bibliography

1. **Basic NLM Documentation**: NLM Phase 6 Documentation
2. **Neural Dynamics**: LIF neuron models and spike propagation
3. **Plasticity Mechanisms**: STDP, Hebbian learning, and structural plasticity
4. **Memory Systems**: Working memory, episodic memory, and semantic memory
5. **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
6. **Development**: Age-dependent plasticity and maturation
7. **Performance**: Memory pools, event queues, and SIMD optimizations
8. **Integration**: Agent-brain-world loop and embodied cognition

### Online Resources

- **NLM GitHub Repository**: https://github.com/your-org/nlm
- **NLM Documentation**: https://docs.your-org.com/nlm
- **PyNLM Package**: https://pypi.org/project/pynlm
- **NLM Forum**: https://forum.your-org.com/nlm
- **NLM Discord**: https://discord.gg/your-org/nlm

### Technical Documentation

- **API References**: `docs/api/`
- **Configuration Guide**: `docs/configuration/`
- **Architecture Documentation**: `docs/architecture/`
- **Examples and Tutorials**: `examples/`
- **Development Guides**: `docs/guides/`

## Index

- Extension Points: 7-101
  - Custom Plasticity Rules: 9-35
  - Custom Neuron Types: 37-58
  - Custom Memory Systems: 60-78
  - Custom Neuromodulators: 80-101
- Performance Optimization: 103-212
  - Configuration Optimization: 105-124
  - Code Optimization Techniques: 126-177
  - Build Optimization: 179-190
  - Profiling and Debugging: 192-212
- Configuration Management: 214-260
  - Configuration Templates: 216-236
  - Configuration Validation: 238-248
  - Configuration Diffing: 250-260
- Advanced Brain Operations: 262-298
  - Custom Brain Regions: 264-286
  - Advanced Plasticity Scheduling: 288-298
- Memory System Configuration: 300-350
  - Working Memory Configuration: 302-314
  - Episodic Memory Configuration: 316-330
  - Semantic Memory Configuration: 332-340
- Neuromodulation Configuration: 342-382
  - Dopamine Configuration: 344-356
  - Curiosity Configuration: 358-370
  - Novelty Configuration: 372-382
- Development Configuration: 384-398
  - Development Stage Configuration: 386-392
  - Plasticity Development Configuration: 394-398
- Multi-Agent Systems: 400-464
  - Multi-Agent Configuration: 402-410
  - Multi-Agent Simulation: 412-464
- Custom Worlds and Environments: 466-528
  - Creating Custom Worlds: 468-492
  - Environment Configuration: 494-498
- Debugging and Profiling: 500-564
  - Debugging Tools: 502-528
  - Performance Profiling: 530-564
- Migration Guide: 566-620
  - Phase 5 to Phase 6 Migration: 568-590
  - Version 1.0 to 2.0 Migration: 592-604
- Future Development: 622-640
  - Planned Features: 624-636
  - Research Directions: 638-640
- References: 642-660
  - Bibliography: 644-654
  - Online Resources: 656-658
  - Technical Documentation: 660

---

*NLM Advanced Guide Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*