# NLM Extension Guide

## Overview

This guide provides comprehensive instructions for extending the NLM (Neural Learning Machine) system with custom components, plugins, and functionality. It covers how to add new neuron types, plasticity rules, memory systems, neuromodulators, and other cognitive components.

## Table of Contents

1. [Introduction](#introduction)
2. [Extension Architecture](#extension-architecture)
3. [Plugin System](#plugin-system)
4. [Extending Plasticity Rules](#extending-plasticity-rules)
5. [Adding New Neuron Types](#adding-new-neuron-types)
6. [Creating Memory Systems](#creating-memory-systems)
7. [Developing Neuromodulators](#developing-neuromodulators)
8. [Adding Cognitive Systems](#adding-cognitive-systems)
9. [Creating Custom Worlds](#creating-custom-worlds)
10. [Developing Extensions](#developing-extensions)
11. [Plugin Development Best Practices](#plugin-development-best-practices)
12. [Testing Extensions](#testing-extensions)
13. [Deployment and Installation](#deployment-and-installation)
14. [Plugin Registry](#plugin-registry)
15. [Extension API Reference](#extension-api-reference)
16. [Troubleshooting](#troubleshooting)
17. [Future Extension Directions](#future-extension-directions)
18. [References](#references)

## Introduction

NLM is designed with extensibility in mind. The system provides clear extension points and interfaces for adding new functionality. This guide covers how to create:

- **Custom neuron types** with different dynamics
- **New plasticity rules** beyond STDP and Hebbian learning
- **Memory systems** for different types of storage
- **Neuromodulators** for motivation and learning
- **Cognitive systems** for attention, planning, prediction
- **Custom worlds** with different environments
- **Plugins** that can be loaded dynamically

## Extension Architecture

### Core Extension Components

NLM extensions are organized into several components:

1. **Extension Base Classes**: Abstract base classes providing extension interfaces
2. **Factory System**: Factory pattern for creating extension instances
3. **Plugin System**: Dynamic loading of extensions
4. **Configuration System**: Extension-specific configuration
5. **Integration Points**: Hooks for connecting extensions to existing systems

### Extension Lifecycle

```cpp
// Extension lifecycle:
// 1. Extension class defined
// 2. Extension registered with factory
// 3. Extension loaded (if using plugin system)
// 4. Extension configured
// 5. Extension integrated with NLM systems
// 6. Extension unregistered (when unloaded)
```

### Extension Points

| Extension Point | Description | Example |
|----------------|-------------|---------|
| `PlasticityRule` | Abstract base for plasticity rules | STDP, Hebbian, Custom |
| `NeuralDynamics` | Abstract base for neuron dynamics | LIF, Custom types |
| `Memory` | Abstract base for memory systems | WorkingMemory, EpisodicMemory |
| `Neuromodulator` | Abstract base for neuromodulators | Dopamine, Curiosity, Novelty |
| `PredictionSystem` | Abstract base for prediction | TemporalPrediction, ActionConsequence |
| `CognitiveSystem` | Abstract base for cognition | Attention, Planning, SelfModel |
| `World` | Abstract base for environments | SimpleWorld, CustomWorld |
| `ExtensionPlugin` | Abstract base for plugins | DataLoader, Visualizer, Logger |

## Plugin System

### Plugin Architecture

NLM supports two plugin architectures:

#### Static Plugins

Compiled directly into the NLM executable. Benefits:
- **Performance**: No runtime overhead
- **Reliability**: Linked at compile time
- **Debuggability**: Standard debugging tools work

```cpp
// Static plugin example
class CustomPlasticityPlugin {
public:
    void initialize(const ExtensionConfig& config) {
        // Initialize plugin
    }
    
    void integrateWithBrain(Brain& brain) {
        // Integrate with brain systems
        brain.registerPlasticityRule("custom_rule", &CustomPlasticityPlugin::create);
    }
    
    static PlasticityRule* create() {
        return new CustomPlasticityRule();
    }
};

// Register plugin
NLM_REGISTER_EXTENSION(CustomPlasticityPlugin);
```

#### Dynamic Plugins

Loaded at runtime using shared libraries. Benefits:
- **Flexibility**: Load/unload plugins at runtime
- **Extensibility**: Users can create their own plugins
- **Safety**: Isolation between plugins

```cpp
// Dynamic plugin interface
class ExtensionPlugin {
public:
    virtual ~ExtensionPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual bool initialize(const ExtensionConfig& config) = 0;
    virtual void shutdown() = 0;
    
    virtual void integrateWithBrain(Brain& brain) = 0;
    virtual void integrateWithWorld(World& world) = 0;
    
    virtual std::vector<std::string> getRequiredExtensions() const = 0;
    virtual std::vector<std::string> getProvidedExtensions() const = 0;
};

// Plugin implementation
class CustomExtensionPlugin : public ExtensionPlugin {
public:
    std::string getName() const override { return "CustomExtension"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Example custom extension"; }
    
    bool initialize(const ExtensionConfig& config) override {
        // Initialize plugin
        return true;
    }
    
    void shutdown() override {
        // Cleanup resources
    }
    
    void integrateWithBrain(Brain& brain) override {
        // Integrate with brain
    }
    
    void integrateWithWorld(World& world) override {
        // Integrate with world
    }
    
    std::vector<std::string> getRequiredExtensions() const override {
        return {"core"};
    }
    
    std::vector<std::string> getProvidedExtensions() const override {
        return {"custom_plasticity", "custom_memory"};
    }
};
```

### Plugin Loading and Management

```cpp
// Plugin manager
class ExtensionManager {
public:
    template<typename T>
    bool loadPlugin(const std::string& libraryPath) {
        // Load shared library
        void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
            return false;
        }
        
        // Get plugin factory function
        typedef ExtensionPlugin* (*PluginFactory)();
        PluginFactory factory = reinterpret_cast<PluginFactory>(
            dlsym(handle, "createPlugin")
        );
        
        if (!factory) {
            std::cerr << "Plugin factory not found: " << dlerror() << std::endl;
            dlclose(handle);
            return false;
        }
        
        // Create plugin instance
        ExtensionPlugin* plugin = factory();
        plugins.push_back(std::make_unique<ExtensionPlugin>(plugin, handle));
        
        return true;
    }
    
    bool unloadPlugin(const std::string& name) {
        // Find and unload plugin
        auto it = std::find_if(plugins.begin(), plugins.end(),
                             [&name](const std::unique_ptr<ExtensionPlugin>& plugin) {
                                 return plugin->getName() == name;
                             });
        
        if (it == plugins.end()) {
            return false;
        }
        
        it->get()->shutdown();
        dlclose(it->get()->getHandle());
        plugins.erase(it);
        return true;
    }
    
    void listPlugins() const {
        for (const auto& plugin : plugins) {
            std::cout << plugin->getName() << " " << plugin->getVersion()
                     << ": " << plugin->getDescription() << std::endl;
        }
    }
    
private:
    struct PluginWithHandle {
        std::unique_ptr<ExtensionPlugin> plugin;
        void* handle;
        
        PluginWithHandle(std::unique_ptr<ExtensionPlugin> p, void* h)
            : plugin(std::move(p)), handle(h) {}
        
        ~PluginWithHandle() {
            if (plugin) {
                plugin->shutdown();
            }
            if (handle) {
                dlclose(handle);
            }
        }
    };
    
    std::vector<PluginWithHandle> plugins;
};
```

## Extending Plasticity Rules

### Creating Custom Plasticity Rules

#### Basic Plasticity Rule Extension

```cpp
#include "plasticity/PlasticityRule.hpp"

class CustomPlasticityRule : public PlasticityRule {
public:
    // Required methods
    void update(Synapse& synapse, Neuron* pre, Neuron* post,
                float timeSincePre, float timeSincePost) override {
        // Calculate weight change based on custom rule
        float weightChange = calculateWeightChange(pre, post, timeSincePre, timeSincePost);
        
        // Apply with bounds checking
        float newWeight = synapse.getWeight() + weightChange;
        newWeight = std::clamp(newWeight, getMinWeight(), getMaxWeight());
        synapse.setWeight(newWeight);
    }
    
    bool shouldApply(const Synapse& synapse, Neuron* pre, Neuron* post) override {
        // Determine if rule should apply
        return pre->isActive() && post->isActive() &&
               synapse.getWeight() > getEffectiveThreshold();
    }
    
    // Factory method
    static PlasticityRule* create() {
        return new CustomPlasticityRule();
    }
    
    // Configuration parameters
    static float getMinWeight() { return -1.0f; }
    static float getMaxWeight() { return 1.0f; }
    static float getEffectiveThreshold() { return 0.1f; }
    
private:
    float calculateWeightChange(Neuron* pre, Neuron* post,
                                float dtPre, float dtPost) {
        // Custom weight change calculation
        // Example: Asymmetric Hebbian with history dependence
        float preTrace = pre->getTrace();
        float postTrace = post->getTrace();
        
        // History-dependent Hebbian
        float hebbian = preTrace * postTrace * 0.001f;
        
        // Spike timing dependence
        float stdp = 0.0f;
        if (dtPre > 0 && dtPre < 50.0f) {
            // LTP
            stdp = 0.01f * std::exp(-dtPre / 20.0f);
        } else if (dtPost > 0 && dtPost < 50.0f) {
            // LTD
            stdp = -0.01f * std::exp(-dtPost / 20.0f);
        }
        
        return hebbian + stdp;
    }
};
```

#### Advanced Plasticity Rule with Learning

```cpp
class AdaptivePlasticityRule : public PlasticityRule {
public:
    struct LearningParameters {
        float learningRate = 0.001f;
        float traceDecay = 0.95f;
        float eligibilityTrace = 0.0f;
        float predictionError = 0.0f;
        float noveltySignal = 0.0f;
    };
    
    void initialize(const PlasticityConfig& config) override {
        parameters.learningRate = config.getFloat("learning_rate", 0.001f);
        parameters.traceDecay = config.getFloat("trace_decay", 0.95f);
        // ... other initialization
    }
    
    void update(Synapse& synapse, Neuron* pre, Neuron* post,
                float dtPre, float dtPost) override {
        // Update eligibility traces
        updateEligibilityTraces(pre, post, dtPre, dtPost);
        
        // Calculate weight change with neuromodulation
        float weightChange = calculateModulatedWeightChange(synapse, pre, post, dtPre, dtPost);
        
        // Apply with bounds checking
        float newWeight = synapse.getWeight() + weightChange * parameters.learningRate;
        newWeight = clampWeight(newWeight);
        synapse.setWeight(newWeight);
    }
    
    bool shouldApply(const Synapse& synapse, Neuron* pre, Neuron* post) override {
        // Apply if there's sufficient activity and eligibility
        return pre->isActive() && post->isActive() &&
               std::abs(parameters.eligibilityTrace) > 0.01f;
    }
    
    static PlasticityRule* create() {
        return new AdaptivePlasticityRule();
    }
    
private:
    void updateEligibilityTraces(Neuron* pre, Neuron* post, float dtPre, float dtPost) {
        // Update eligibility traces based on neural activity
        parameters.eligibilityTrace *= parameters.traceDecay;
        
        if (pre->isActive()) {
            parameters.eligibilityTrace += 1.0f;  // Pre-synaptic trace
        }
        
        if (post->isActive()) {
            parameters.eligibilityTrace -= 1.0f;  // Post-synaptic trace
        }
    }
    
    float calculateModulatedWeightChange(const Synapse& synapse, Neuron* pre, Neuron* post,
                                         float dtPre, float dtPost) {
        // Calculate weight change modulated by neuromodulators
        float baseChange = calculateBaseWeightChange(pre, post, dtPre, dtPost);
        
        // Apply neuromodulation factors
        float neuromodulationFactor = 1.0f;
        
        if (dopamineLevel > 0) {
            neuromodulationFactor *= std::pow(dopamineLevel, dopamineSensitivity);
        }
        
        if (noveltyLevel > 0) {
            neuromodulationFactor *= (1.0f + noveltySensitivity * noveltyLevel);
        }
        
        return baseChange * neuromodulationFactor;
    }
    
    float clampWeight(float weight) {
        return std::clamp(weight, getMinWeight(), getMaxWeight());
    }
    
    LearningParameters parameters;
    float dopamineLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float dopamineSensitivity = 0.5f;
    float noveltySensitivity = 0.3f;
};
```

### Registering Custom Plasticity Rules

```cpp
// Register plasticity rule with NLM factory
void registerCustomPlasticityRules() {
    // Register simple custom rule
    PlasticityFactory::registerRule("custom", &CustomPlasticityRule::create);
    
    // Register adaptive rule
    PlasticityFactory::registerRule("adaptive", &AdaptivePlasticityRule::create);
    
    // Register with configuration parameters
    PlasticityFactory::registerRuleWithConfig(
        "custom_configured",
        &CustomPlasticityRule::create,
        {
            {"min_weight", -0.5f},
            {"max_weight", 0.5f},
            {"threshold", 0.2f}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomPlasticityRules);
```

## Adding New Neuron Types

### Creating Custom Neuron Types

#### Basic Custom Neuron Type

```cpp
#include "brain/NeuronDynamics.hpp"

class CustomNeuronType : public NeuralDynamics {
public:
    struct Parameters {
        float vRest = -65.0f;           // Resting potential
        float vReset = -70.0f;          // Reset potential
        float threshold = -50.0f;       // Spike threshold
        float tauMem = 20.0f;           // Membrane time constant
        float tauRef = 2.0f;            // Refractory period
        float resetCurrent = 0.0f;     // Current to reset
        float spikeWidth = 1.0f;        // Spike width
        float adaptationStrength = 0.0f; // Spike frequency adaptation
    } params;
    
    void initialize(Neuron* neuron) override {
        // Set neuron properties
        neuron->setVRest(params.vRest);
        neuron->setVReset(params.vReset);
        neuron->setThreshold(params.threshold);
        neuron->setTauMem(params.tauMem);
        neuron->setTauRef(params.tauRef);
        
        // Initialize internal state
        neuron->setVMembrane(params.vRest);
        neuron->setRefractoryTimer(0.0f);
        neuron->setAdaptationCurrent(0.0f);
    }
    
    void step(Neuron* neuron, float dt) override {
        // Update membrane potential
        updateMembranePotential(neuron, dt);
        
        // Check for spike
        if (checkForSpike(neuron)) {
            onSpike(neuron, dt);
        }
        
        // Update adaptation
        updateAdaptation(neuron, dt);
    }
    
    bool shouldSpike(Neuron* neuron) override {
        return neuron->getVMembrane() >= neuron->getThreshold() &&
               neuron->getRefractoryTimer() <= 0.0f;
    }
    
    void onSpike(Neuron* neuron, float dt) override {
        // Handle spike event
        neuron->setVMembrane(neuron->getVReset());
        neuron->setRefractoryTimer(neuron->getTauRef());
        neuron->incrementSpikeCount();
        
        // Apply adaptation
        if (params.adaptationStrength > 0) {
            float adaptationCurrent = params.adaptationStrength * neuron->getSpikeCount();
            neuron->setAdaptationCurrent(adaptationCurrent);
        }
        
        // Notify connected neurons
        notifyConnectedNeurons(neuron, dt);
    }
    
    static NeuralDynamics* create() {
        return new CustomNeuronType();
    }
    
private:
    void updateMembranePotential(Neuron* neuron, float dt) {
        // LIF dynamics with adaptation
        float vMem = neuron->getVMembrane();
        float vRest = neuron->getVRest();
        float tauMem = neuron->getTauMem();
        float adaptation = neuron->getAdaptationCurrent();
        
        // Standard LIF equation
        float dv = (vRest - vMem + adaptation) / tauMem * dt;
        vMem += dv;
        
        // Update refractory timer
        if (neuron->getRefractoryTimer() > 0) {
            neuron->setRefractoryTimer(neuron->getRefractoryTimer() - dt);
        }
        
        neuron->setVMembrane(vMem);
    }
    
    void updateAdaptation(Neuron* neuron, float dt) {
        if (params.adaptationStrength > 0) {
            // Exponential decay of adaptation current
            float adaptation = neuron->getAdaptationCurrent();
            adaptation *= std::exp(-dt / params.tauMem);
            neuron->setAdaptationCurrent(adaptation);
        }
    }
    
    void notifyConnectedNeurons(Neuron* neuron, float dt) {
        // Notify connected neurons about spike
        // This would typically use the spike delivery system
    }
};
```

#### Advanced Custom Neuron with Input Processing

```cpp
class InputProcessingNeuron : public NeuralDynamics {
public:
    enum class InputType {
        EXCITATORY,
        INHIBITORY,
        MODULATORY,
        SELF_RECURRENT
    };
    
    struct SynapticInput {
        float weight;
        float delay;
        InputType type;
        float conductance;
    };
    
    void initialize(Neuron* neuron) override {
        NeuralDynamics::initialize(neuron);
        
        // Add synaptic inputs based on neuron type
        if (type == InputType::EXCITATORY) {
            addInput(1.0f, 2.0f, InputType::EXCITATORY);
            addInput(0.5f, 1.0f, InputType::EXCITATORY);
        } else if (type == InputType::INHIBITORY) {
            addInput(-1.5f, 0.0f, InputType::INHIBITORY);
        }
        
        // Configure ion channels
        configureIonChannels();
    }
    
    void step(Neuron* neuron, float dt) override {
        // Update synaptic inputs
        updateSynapticInputs(neuron, dt);
        
        // Update membrane potential with conductance-based dynamics
        updateConductanceBasedDynamics(neuron, dt);
        
        // Standard spike processing
        if (checkForSpike(neuron)) {
            onSpike(neuron, dt);
        }
    }
    
    void setNeuronType(InputType type) {
        this->type = type;
    }
    
    static NeuralDynamics* create() {
        return new InputProcessingNeuron();
    }
    
private:
    void addInput(float weight, float delay, InputType type) {
        SynapticInput input;
        input.weight = weight;
        input.delay = delay;
        input.type = type;
        input.conductance = 0.0f;
        
        inputs.push_back(input);
    }
    
    void updateSynapticInputs(Neuron* neuron, float dt) {
        for (auto& input : inputs) {
            // Update conductance for exponential synapses
            if (input.type == InputType::EXCITATORY) {
                input.conductance = std::max(0.0f, input.conductance - dt / 5.0f);
            }
            
            // Add to membrane potential
            if (input.delay <= 0.0f) {
                neuron->setVMembrane(neuron->getVMembrane() + input.weight * input.conductance);
            } else {
                // Store for delayed delivery
                delayedInputs.push_back({input, neuron, dt + input.delay});
            }
        }
        
        // Process delayed inputs
        processDelayedInputs(dt);
    }
    
    void updateConductanceBasedDynamics(Neuron* neuron, float dt) {
        // Conductance-based neuron dynamics
        float vMem = neuron->getVMembrane();
        float vRest = neuron->getVRest();
        float tauMem = neuron->getTauMem();
        
        // Calculate total synaptic conductance
        float totalConductance = 0.0f;
        for (const auto& input : inputs) {
            totalConductance += input.conductance;
        }
        
        // Update membrane potential with conductance-based dynamics
        float dv = (totalConductance * (vRest - vMem)) / tauMem * dt;
        vMem += dv;
        
        neuron->setVMembrane(vMem);
    }
    
    void configureIonChannels() {
        // Configure ion channel properties
        // This would typically involve setting up Hodgkin-Huxley channels
        // or simplified models for computational efficiency
    }
    
    void processDelayedInputs(float dt) {
        // Process inputs scheduled for future delivery
        auto it = delayedInputs.begin();
        while (it != delayedInputs.end()) {
            it->remainingTime -= dt;
            
            if (it->remainingTime <= 0.0f) {
                // Deliver input
                it->neuron->setVMembrane(it->neuron->getVMembrane() + it->input.weight);
                it = delayedInputs.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    InputType type;
    std::vector<SynapticInput> inputs;
    std::vector<std::pair<SynapticInput, Neuron*, float>> delayedInputs;
};
```

### Registering Custom Neuron Types

```cpp
// Register neuron type with NLM factory
void registerCustomNeuronTypes() {
    // Register simple custom neuron
    NeuralDynamicsFactory::registerType("custom_neuron", &CustomNeuronType::create);
    
    // Register input processing neuron
    NeuralDynamicsFactory::registerType("input_neuron", &InputProcessingNeuron::create);
    
    // Register with configuration parameters
    NeuralDynamicsFactory::registerTypeWithConfig(
        "custom_neuron_configured",
        &CustomNeuronType::create,
        {
            {"v_rest", -65.0f},
            {"v_reset", -70.0f},
            {"threshold", -50.0f},
            {"tau_mem", 20.0f},
            {"adaptation_strength", 0.01f}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomNeuronTypes);
```

## Creating Memory Systems

### Working Memory Extension

```cpp
#include "memory/Memory.hpp"

class CustomWorkingMemory : public WorkingMemory {
public:
    struct Parameters {
        float capacity = 0.8f;              // Capacity as fraction of maximum
        float decayRate = 0.95f;            // Decay per timestep
        float competitionStrength = 0.8f;  // Competition strength
        float consolidationStrength = 0.3f; // Consolidation strength
        float updateFrequency = 1.0f;      // Update frequency
        bool enableRecurrence = true;       // Enable recurrent connections
        bool enableAttention = true;        // Enable attention mechanism
    } params;
    
    struct MemoryItem {
        NeuralPattern pattern;
        float priority;
        float stability;
        float age;
        float activationCount;
    };
    
    void initialize(const MemoryConfig& config) override {
        params.capacity = config.getFloat("capacity", 0.8f);
        params.decayRate = config.getFloat("decay_rate", 0.95f);
        params.competitionStrength = config.getFloat("competition_strength", 0.8f);
        params.consolidationStrength = config.getFloat("consolidation_strength", 0.3f);
        params.updateFrequency = config.getFloat("update_frequency", 1.0f);
        params.enableRecurrence = config.getBool("enable_recurrence", true);
        params.enableAttention = config.getBool("enable_attention", true);
        
        // Calculate maximum capacity
        maxCapacity = static_cast<size_t>(params.capacity * MAX_POSSIBLE_CAPACITY);
    }
    
    void store(const NeuralPattern& pattern, float priority = 1.0f) override {
        // Check capacity
        if (items.size() >= maxCapacity) {
            // Remove lowest priority item
            removeLowestPriorityItem();
        }
        
        // Create memory item
        MemoryItem item;
        item.pattern = pattern;
        item.priority = priority;
        item.stability = 1.0f;
        item.age = 0.0f;
        item.activationCount = 0;
        
        // Add to memory
        items.push_back(item);
        
        // Apply recurrence if enabled
        if (params.enableRecurrence) {
            addRecurrenceConnections(item.pattern);
        }
        
        // Trigger attention if enabled
        if (params.enableAttention) {
            updateAttentionWeights();
        }
    }
    
    NeuralPattern retrieve(size_t index = 0) override {
        if (index >= items.size()) {
            return NeuralPattern(); // Return empty pattern
        }
        
        // Update item activation
        items[index].activationCount++;
        items[index].stability = std::min(1.0f, items[index].stability * 1.1f);
        
        return items[index].pattern;
    }
    
    size_t size() const override {
        return items.size();
    }
    
    void clear() override {
        items.clear();
        // Clear recurrence connections
        clearRecurrenceConnections();
    }
    
    void update(float timeStep) override {
        // Decay items based on age
        for (auto& item : items) {
            item.age += timeStep;
            item.stability *= params.decayRate;
            
            // Apply decay to pattern
            item.pattern *= params.decayRate;
        }
        
        // Remove unstable items
        removeUnstableItems();
        
        // Update attention if enabled
        if (params.enableAttention) {
            updateAttentionWeights();
        }
    }
    
    static Memory* create() {
        return new CustomWorkingMemory();
    }
    
private:
    void removeLowestPriorityItem() {
        if (items.empty()) {
            return;
        }
        
        // Find item with lowest priority
        auto minIt = std::min_element(items.begin(), items.end(),
                                      [](const MemoryItem& a, const MemoryItem& b) {
                                          return a.priority < b.priority;
                                      });
        
        // Remove item and its connections
        removeConnections(minIt->pattern);
        items.erase(minIt);
    }
    
    void addRecurrenceConnections(const NeuralPattern& pattern) {
        // Add recurrent connections to pattern
        // This would involve creating synapses from neurons in the pattern
        // back to the neurons themselves
    }
    
    void removeConnections(const NeuralPattern& pattern) {
        // Remove connections associated with pattern
    }
    
    void clearRecurrenceConnections() {
        // Clear all recurrence connections
    }
    
    void updateAttentionWeights() {
        // Update attention weights based on item stability and priority
        // This would involve competitive normalization of weights
    }
    
    void removeUnstableItems() {
        // Remove items with low stability
        items.erase(std::remove_if(items.begin(), items.end(),
                                   [](const MemoryItem& item) {
                                       return item.stability < STABILITY_THRESHOLD;
                                   }),
                    items.end());
    }
    
    static const float STABILITY_THRESHOLD = 0.1f;
    static const size_t MAX_POSSIBLE_CAPACITY = 10000;
    
    std::vector<MemoryItem> items;
    size_t maxCapacity;
};
```

### Episodic Memory Extension

```cpp
class CustomEpisodicMemory : public EpisodicMemory {
public:
    struct Parameters {
        size_t capacity = 1000;            // Maximum number of episodes
        float encodingStrength = 1.0f;     // Encoding strength
        float retrievalSuccessRate = 0.8f; // Retrieval success rate
        float replayFrequency = 0.1f;     // Replay frequency
        float replayStrength = 0.5f;      // Replay strength
        float consolidationRate = 0.01f;  // Consolidation rate
        float stabilityThreshold = 0.6f;  // Stability threshold
        bool enableLabilization = true;    // Enable memory labilization
        bool enableSystematicConsolidation = true; // Enable systematic consolidation
    } params;
    
    struct Episode {
        uint64_t id;                       // Unique episode ID
        TimeWindow timeWindow;             // Time window of episode
        NeuralPattern pattern;             // Pattern representing episode
        std::vector<float> rewardHistory;  // Reward history
        std::vector<float> predictionError; // Prediction errors
        float salience;                     // Episode salience
        float stability;                    // Episode stability
        bool consolidated;                 // Whether episode is consolidated
    };
    
    void initialize(const MemoryConfig& config) override {
        params.capacity = config.getInt("capacity", 1000);
        params.encodingStrength = config.getFloat("encoding_strength", 1.0f);
        params.retrievalSuccessRate = config.getFloat("retrieval_success_rate", 0.8f);
        params.replayFrequency = config.getFloat("replay_frequency", 0.1f);
        params.replayStrength = config.getFloat("replay_strength", 0.5f);
        params.consolidationRate = config.getFloat("consolidation_rate", 0.01f);
        params.stabilityThreshold = config.getFloat("stability_threshold", 0.6f);
        params.enableLabilization = config.getBool("enable_labilization", true);
        params.enableSystematicConsolidation = config.getBool("enable_systematic_consolidation", true);
    }
    
    void store(const Experience& experience) override {
        // Create episode from experience
        Episode episode;
        episode.id = nextEpisodeId++;
        episode.timeWindow.start = experience.time;
        episode.timeWindow.end = experience.time + experience.duration;
        
        // Encode experience into neural pattern
        episode.pattern = encodeExperience(experience);
        
        // Calculate salience
        episode.salience = calculateSalience(experience);
        
        // Add to memory
        episodes.push_back(episode);
        
        // Trim to capacity
        trimToCapacity();
        
        // Schedule replay
        scheduleReplay(episode.id);
    }
    
    Experience retrieve(const TimeWindow& window) override {
        // Find episodes in time window
        std::vector<Episode*> matchingEpisodes;
        for (auto& episode : episodes) {
            if (window.overlaps(episode.timeWindow)) {
                matchingEpisodes.push_back(&episode);
            }
        }
        
        if (matchingEpisodes.empty()) {
            return Experience(); // Return empty experience
        }
        
        // Combine patterns from matching episodes
        NeuralPattern combinedPattern;
        for (Episode* episode : matchingEpisodes) {
            combinedPattern += episode->pattern * episode->stability;
        }
        
        // Normalize combined pattern
        combinedPattern.normalize();
        
        // Create experience from combined pattern
        Experience result;
        result.pattern = combinedPattern;
        result.time = window.end;
        result.duration = window.duration();
        
        // Update episode retrieval statistics
        for (Episode* episode : matchingEpisodes) {
            episode->retrievalCount++;
        }
        
        return result;
    }
    
    void replay(float consolidationStrength) override {
        // Replay episodes for consolidation
        std::vector<size_t> episodesToReplay;
        
        for (size_t i = 0; i < episodes.size(); ++i) {
            if (!episodes[i].consolidated && 
                randomFloat() < params.replayFrequency) {
                episodesToReplay.push_back(i);
            }
        }
        
        // Replay selected episodes
        for (size_t episodeIndex : episodesToReplay) {
            replayEpisode(episodeIndex, consolidationStrength);
        }
    }
    
    void consolidate() override {
        // Consolidate episodes
        for (auto& episode : episodes) {
            if (!episode.consolidated) {
                // Check if episode meets stability threshold
                if (episode.stability >= params.stabilityThreshold) {
                    // Consolidate episode
                    consolidateEpisode(episode);
                } else if (params.enableLabilization) {
                    // Attempt labilization
                    stabilizeEpisode(episode);
                }
            }
        }
    }
    
    static Memory* create() {
        return new CustomEpisodicMemory();
    }
    
private:
    void trimToCapacity() {
        if (episodes.size() > params.capacity) {
            // Remove least stable episodes
            std::partial_sort(episodes.begin(), episodes.begin() + 
                            params.capacity, episodes.end(),
                            [](const Episode& a, const Episode& b) {
                                return a.stability > b.stability;
                            });
            episodes.resize(params.capacity);
        }
    }
    
    NeuralPattern encodeExperience(const Experience& experience) {
        // Encode experience into neural pattern
        NeuralPattern pattern;
        
        // Add sensory components
        if (experience.sensoryInput) {
            pattern += experience.sensoryInput->toNeuralPattern();
        }
        
        // Add action components
        if (experience.action) {
            pattern += experience.action->toNeuralPattern();
        }
        
        // Add reward components
        if (experience.reward > 0) {
            pattern += NeuralPattern(1.0f, 1.0f); // Simple reward pattern
        }
        
        // Apply encoding strength
        pattern *= params.encodingStrength;
        
        return pattern;
    }
    
    float calculateSalience(const Experience& experience) {
        // Calculate episode salience based on reward prediction error
        float rewardPredictionError = std::abs(experience.reward - experience.predictedReward);
        
        // Also consider novelty
        float novelty = calculateNovelty(experience);
        
        return rewardPredictionError + novelty;
    }
    
    void scheduleReplay(size_t episodeId) {
        // Schedule replay for episode
        // This would involve adding the episode to a replay queue
    }
    
    void replayEpisode(size_t episodeIndex, float strength) {
        // Replay specific episode
        Episode& episode = episodes[episodeIndex];
        
        // Strengthen connections
        strengthenConnections(episode.pattern, strength);
        
        // Update stability
        episode.stability = std::min(1.0f, episode.stability + params.consolidationRate * strength);
    }
    
    void consolidateEpisode(Episode& episode) {
        // Consolidate episode by stabilizing connections
        strengthenConnections(episode.pattern, params.consolidationRate);
        
        // Mark as consolidated
        episode.consolidated = true;
    }
    
    void stabilizeEpisode(Episode& episode) {
        // Attempt to stabilize episode
        // This might involve additional replay or external inputs
    }
    
    float calculateNovelty(const Experience& experience) {
        // Calculate novelty based on similarity to recent experiences
        // This would compare with recent episodes
        return 0.0f;
    }
    
    void strengthenConnections(const NeuralPattern& pattern, float strength) {
        // Strengthen connections associated with pattern
        // This would involve modifying synaptic weights
    }
    
    uint64_t nextEpisodeId = 1;
    std::vector<Episode> episodes;
};
```

### Registering Custom Memory Systems

```cpp
// Register memory systems with NLM factory
void registerCustomMemorySystems() {
    // Register custom working memory
    MemoryFactory::registerMemory("working_memory", &CustomWorkingMemory::create);
    
    // Register custom episodic memory
    MemoryFactory::registerMemory("episodic_memory", &CustomEpisodicMemory::create);
    
    // Register with configuration parameters
    MemoryFactory::registerMemoryWithConfig(
        "working_memory_configured",
        &CustomWorkingMemory::create,
        {
            {"capacity", 0.8f},
            {"decay_rate", 0.95f},
            {"competition_strength", 0.8f}
        }
    );
    
    MemoryFactory::registerMemoryWithConfig(
        "episodic_memory_configured",
        &CustomEpisodicMemory::create,
        {
            {"capacity", 1000},
            {"encoding_strength", 1.0f},
            {"retrieval_success_rate", 0.8f},
            {"replay_frequency", 0.1f}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomMemorySystems);
```

## Developing Neuromodulators

### Creating Custom Neuromodulators

```cpp
#include "neuromodulation/Neuromodulator.hpp"

class CustomNeuromodulator : public Neuromodulator {
public:
    struct Parameters {
        float scale = 1.0f;                  // Overall scale factor
        float decayRate = 0.95f;            // Decay rate
        float baseline = 0.1f;              // Baseline level
        float noise = 0.05f;                // Random noise level
        float adaptationRate = 0.01f;      // Adaptation rate
        float historySize = 100;            // Size of history for novelty detection
        float sensitivity = 1.0f;           // Sensitivity to inputs
        bool enablePredictionError = true;  // Enable prediction error integration
        bool enableRewardIntegration = true; // Enable reward integration
        bool enableCuriosityIntegration = true; // Enable curiosity integration
    } params;
    
    struct InternalState {
        std::vector<float> history;          // Signal history for novelty detection
        float currentSignal = 0.0f;          // Current signal value
        float predictionError = 0.0f;       // Prediction error signal
        float rewardPrediction = 0.0f;       // Reward prediction
        float curiosityLevel = 0.0f;        // Curiosity level
        float noveltyLevel = 0.0f;           // Novelty level
        float timeSinceUpdate = 0.0f;        // Time since last update
    } state;
    
    void initialize(const NeuromodulationConfig& config) override {
        params.scale = config.getFloat("scale", 1.0f);
        params.decayRate = config.getFloat("decay_rate", 0.95f);
        params.baseline = config.getFloat("baseline", 0.1f);
        params.noise = config.getFloat("noise", 0.05f);
        params.adaptationRate = config.getFloat("adaptation_rate", 0.01f);
        params.historySize = config.getInt("history_size", 100);
        params.sensitivity = config.getFloat("sensitivity", 1.0f);
        params.enablePredictionError = config.getBool("enable_prediction_error", true);
        params.enableRewardIntegration = config.getBool("enable_reward_integration", true);
        params.enableCuriosityIntegration = config.getBool("enable_curiosity_integration", true);
        
        // Initialize history
        state.history.resize(params.historySize, 0.0f);
    }
    
    float computeSignal(const BrainState& brainState) override {
        // Update internal state
        updateState(brainState);
        
        // Compute signal based on current state
        float signal = params.baseline;
        
        // Add prediction error contribution
        if (params.enablePredictionError) {
            signal += state.predictionError * params.sensitivity;
        }
        
        // Add reward contribution
        if (params.enableRewardIntegration) {
            signal += (brainState.reward - state.rewardPrediction) * params.sensitivity;
        }
        
        // Add curiosity contribution
        if (params.enableCuriosityIntegration) {
            signal += state.curiosityLevel * params.sensitivity;
        }
        
        // Add novelty contribution
        signal += state.noveltyLevel * params.sensitivity;
        
        // Apply scale
        signal *= params.scale;
        
        // Add noise
        signal += (randomFloat() - 0.5f) * params.noise * 2.0f;
        
        // Clamp to valid range
        signal = std::clamp(signal, 0.0f, 1.0f);
        
        // Update history
        updateHistory(signal);
        
        return signal;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Apply neuromodulator effect to brain systems
        applyToPlasticity(brain, signal);
        applyToMemory(brain, signal);
        applyToAttention(brain, signal);
        applyToDevelopment(brain, signal);
    }
    
    static Neuromodulator* create() {
        return new CustomNeuromodulator();
    }
    
private:
    void updateState(const BrainState& brainState) {
        // Update prediction error
        state.predictionError = brainState.predictionError;
        
        // Update reward prediction
        state.rewardPrediction = brainState.rewardPrediction;
        
        // Update curiosity level
        state.curiosityLevel = brainState.curiosityLevel;
        
        // Update novelty level
        state.noveltyLevel = brainState.noveltyLevel;
        
        // Update time since update
        state.timeSinceUpdate = brainState.time;
        
        // Apply decay
        state.predictionError *= params.decayRate;
        state.rewardPrediction *= params.decayRate;
        state.curiosityLevel *= params.decayRate;
        state.noveltyLevel *= params.decayRate;
        
        // Adapt based on error
        state.predictionError += params.adaptationRate * 
                                 (brainState.reward - state.rewardPrediction);
    }
    
    void updateHistory(float signal) {
        // Update signal history
        state.history.erase(state.history.begin());
        state.history.push_back(signal);
    }
    
    float calculateNoveltyLevel() {
        // Calculate novelty level based on signal history
        if (state.history.size() < 2) {
            return 0.0f;
        }
        
        // Calculate variance of recent signals
        float mean = std::accumulate(state.history.begin(), state.history.end(), 0.0f) / 
                    static_cast<float>(state.history.size());
        
        float variance = 0.0f;
        for (float value : state.history) {
            variance += (value - mean) * (value - mean);
        }
        variance /= static_cast<float>(state.history.size());
        
        // Convert variance to novelty level (0-1)
        return std::min(1.0f, std::sqrt(variance) * 2.0f);
    }
    
    void applyToPlasticity(Brain& brain, float signal) {
        // Apply signal to plasticity systems
        // This would involve modulating plasticity rates
        // or modifying plasticity rules based on signal strength
    }
    
    void applyToMemory(Brain& brain, float signal) {
        // Apply signal to memory systems
        // This would involve enhancing or suppressing memory operations
    }
    
    void applyToAttention(Brain& brain, float signal) {
        // Apply signal to attention systems
        // This would involve biasing attentional selection
    }
    
    void applyToDevelopment(Brain& brain, float signal) {
        // Apply signal to development systems
        // This would involve accelerating or decelerating development
    }
};
```

### Neuromodulator Categories

#### Dopamine-like Neuromodulators

```cpp
class DopamineNeuromodulator : public Neuromodulator {
public:
    float computeSignal(const BrainState& brainState) override {
        // Dopamine: Reward prediction error
        float rewardPredictionError = brainState.reward - brainState.rewardPrediction;
        return std::max(0.0f, rewardPredictionError) * scale;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Dopamine: Modulate plasticity rates
        modulatePlasticityRates(brain, signal);
        
        // Dopamine: Enhance working memory consolidation
        enhanceWorkingMemoryConsolidation(brain, signal);
    }
    
    static Neuromodulator* create() {
        return new DopamineNeuromodulator();
    }
};
```

#### Curiosity Neuromodulators

```cpp
class CuriosityNeuromodulator : public Neuromodulator {
public:
    float computeSignal(const BrainState& brainState) override {
        // Curiosity: Novelty + prediction error
        float novelty = calculateNovelty(brainState);
        float predictionError = brainState.predictionError;
        return (novelty * curiosityWeight + predictionError * errorWeight) * scale;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Curiosity: Drive exploration
        driveExploration(brain, signal);
        
        // Curiosity: Enhance learning from novel experiences
        enhanceNoveltyLearning(brain, signal);
    }
    
    static Neuromodulator* create() {
        return new CuriosityNeuromodulator();
    }
};
```

#### Novelty Neuromodulators

```cpp
class NoveltyNeuromodulator : public Neuromodulator {
public:
    float computeSignal(const BrainState& brainState) override {
        // Novelty: Deviation from expected patterns
        float patternDeviation = calculatePatternDeviation(brainState);
        float behavioralNovelty = calculateBehavioralNovelty(brainState);
        
        return (patternDeviation * patternWeight + behavioralNovelty * behavioralWeight) * scale;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Novelty: Capture attention
        captureAttention(brain, signal);
        
        // Novelty: Encode new memories
        encodeNovelExperience(brain, signal);
    }
    
    static Neuromodulator* create() {
        return new NoveltyNeuromodulator();
    }
};
```

### Registering Custom Neuromodulators

```cpp
// Register neuromodulators with NLM factory
void registerCustomNeuromodulators() {
    // Register custom neuromodulator
    NeuromodulatorFactory::registerNeuromodulator("custom", &CustomNeuromodulator::create);
    
    // Register specialized neuromodulators
    NeuromodulatorFactory::registerNeuromodulator("dopamine", &DopamineNeuromodulator::create);
    NeuromodulatorFactory::registerNeuromodulator("curiosity", &CuriosityNeuromodulator::create);
    NeuromodulatorFactory::registerNeuromodulator("novelty", &NoveltyNeuromodulator::create);
    
    // Register with configuration parameters
    NeuromodulatorFactory::registerNeuromodulatorWithConfig(
        "custom_configured",
        &CustomNeuromodulator::create,
        {
            {"scale", 1.0f},
            {"decay_rate", 0.95f},
            {"baseline", 0.1f},
            {"sensitivity", 1.0f}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomNeuromodulators);
```

## Adding Cognitive Systems

### Cognitive System Extension

```cpp
#include "cognition/CognitiveSystem.hpp"

class CustomCognitiveSystem : public CognitiveSystem {
public:
    struct Parameters {
        float attentionStrength = 1.0f;           // Attention strength
        float planningHorizon = 10.0f;           // Planning horizon (time steps)
        float predictionAccuracy = 0.8f;         // Prediction accuracy
        float selfModelConfidence = 0.5f;         // Self-model confidence
        float socialLearningWeight = 0.3f;       // Weight of social learning
        bool enableAttention = true;              // Enable attentional selection
        bool enablePlanning = true;               // Enable planning
        bool enableSelfModel = true;              // Enable self-model
        bool enableSocialLearning = true;         // Enable social learning
    } params;
    
    void initialize(const CognitiveConfig& config) override {
        params.attentionStrength = config.getFloat("attention_strength", 1.0f);
        params.planningHorizon = config.getFloat("planning_horizon", 10.0f);
        params.predictionAccuracy = config.getFloat("prediction_accuracy", 0.8f);
        params.selfModelConfidence = config.getFloat("self_model_confidence", 0.5f);
        params.socialLearningWeight = config.getFloat("social_learning_weight", 0.3f);
        params.enableAttention = config.getBool("enable_attention", true);
        params.enablePlanning = config.getBool("enable_planning", true);
        params.enableSelfModel = config.getBool("enable_self_model", true);
        params.enableSocialLearning = config.getBool("enable_social_learning", true);
    }
    
    void process(const BrainState& input, BrainState& output) override {
        // Apply attentional selection
        if (params.enableAttention) {
            applyAttention(input, output);
        }
        
        // Apply planning
        if (params.enablePlanning) {
            applyPlanning(input, output);
        }
        
        // Apply self-model
        if (params.enableSelfModel) {
            applySelfModel(input, output);
        }
        
        // Apply social learning
        if (params.enableSocialLearning) {
            applySocialLearning(input, output);
        }
        
        // Apply prediction
        applyPrediction(input, output);
        
        // Modify neural activity based on cognitive output
        modifyNeuralActivity(output);
    }
    
    static CognitiveSystem* create() {
        return new CustomCognitiveSystem();
    }
    
private:
    void applyAttention(const BrainState& input, BrainState& output) {
        // Implement attentional selection
        // This would involve competitive selection of neural populations
        // based on input strength and current state
    }
    
    void applyPlanning(const BrainState& input, BrainState& output) {
        // Implement multi-step planning
        // This would involve evaluating potential actions and selecting best
    }
    
    void applySelfModel(const BrainState& input, BrainState& output) {
        // Apply self-model for sensorimotor prediction
        // This would involve predicting consequences of actions
    }
    
    void applySocialLearning(const BrainState& input, BrainState& output) {
        // Implement social learning from observed behaviors
        // This would involve imitation and observation
    }
    
    void applyPrediction(const BrainState& input, BrainState& output) {
        // Apply predictive processing
        // This would involve forward models and prediction errors
    }
    
    void modifyNeuralActivity(const BrainState& cognitiveOutput) {
        // Modify neural activity based on cognitive processes
        // This would involve biasing or enhancing neural populations
    }
};
```

### Cognitive System Categories

#### Attention System

```cpp
class AttentionSystem : public CognitiveSystem {
public:
    float computeSignal(const BrainState& brainState) override {
        // Attention: Competitive selection of neural populations
        // Based on bottom-up salience and top-down bias
        float bottomUpSalience = calculateBottomUpSalience(brainState);
        float topDownBias = calculateTopDownBias(brainState);
        
        return normalize(bottomUpSalience * topDownBias);
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Attention: Modulate neural activity
        // Enhance activity in attended neural populations
        // Suppress activity in unattended populations
        modulateAttentionalSelection(brain, signal);
    }
    
    static CognitiveSystem* create() {
        return new AttentionSystem();
    }
};
```

#### Planning System

```cpp
class PlanningSystem : public CognitiveSystem {
public:
    float computeSignal(const BrainState& brainState) override {
        // Planning: Evaluate potential action sequences
        // Use learned predictions to select optimal actions
        return evaluateActionSequences(brainState) * planningStrength;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Planning: Select actions based on predictions
        selectActionsBasedOnPredictions(brain, signal);
    }
    
    static CognitiveSystem* create() {
        return new PlanningSystem();
    }
};
```

#### Self-Model System

```cpp
class SelfModelSystem : public CognitiveSystem {
public:
    float computeSignal(const BrainState& brainState) override {
        // Self-model: Predict sensory consequences of actions
        return predictSensoryConsequences(brainState) * selfModelConfidence;
    }
    
    void applyEffect(Brain& brain, float signal) override {
        // Self-model: Update internal representation
        updateSelfModel(brain, signal);
    }
    
    static CognitiveSystem* create() {
        return new SelfModelSystem();
    }
};
```

### Registering Cognitive Systems

```cpp
// Register cognitive systems with NLM factory
void registerCustomCognitiveSystems() {
    // Register custom cognitive system
    CognitiveSystemFactory::registerSystem("custom", &CustomCognitiveSystem::create);
    
    // Register specialized cognitive systems
    CognitiveSystemFactory::registerSystem("attention", &AttentionSystem::create);
    CognitiveSystemFactory::registerSystem("planning", &PlanningSystem::create);
    CognitiveSystemFactory::registerSystem("self_model", &SelfModelSystem::create);
    
    // Register with configuration parameters
    CognitiveSystemFactory::registerSystemWithConfig(
        "attention_configured",
        &AttentionSystem::create,
        {
            {"strength", 1.0f},
            {"bias", 1.0f},
            {"selection_method", "competitive"}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomCognitiveSystems);
```

## Creating Custom Worlds

### Custom World Extension

```cpp
#include "world/World.hpp"

class CustomWorld : public World {
public:
    struct WorldParameters {
        int width = 50;                          // World width
        int height = 50;                         // World height
        float timestep = 0.1f;                  // Simulation timestep
        float maxEnergy = 100.0f;               // Maximum agent energy
        int visionWidth = 8;                    // Vision width
        int visionHeight = 8;                   // Vision height
        bool enablePhysics = true;              // Enable physics simulation
        bool enableResources = true;            // Enable resources
        bool enableHazards = true;              // Enable hazards
        bool enableSocial = false;              // Enable social interactions
        float gravity = 0.0f;                    // Gravity constant
        float friction = 0.1f;                   // Friction coefficient
        std::string terrainType = "flat";     // Terrain type
        std::string weatherPattern = "none";   // Weather pattern
    } params;
    
    void initialize(const WorldConfig& config) override {
        params.width = config.getInt("width", 50);
        params.height = config.getInt("height", 50);
        params.timestep = config.getFloat("timestep", 0.1f);
        params.maxEnergy = config.getFloat("max_energy", 100.0f);
        params.visionWidth = config.getInt("vision_width", 8);
        params.visionHeight = config.getInt("vision_height", 8);
        params.enablePhysics = config.getBool("enable_physics", true);
        params.enableResources = config.getBool("enable_resources", true);
        params.enableHazards = config.getBool("enable_hazards", true);
        params.enableSocial = config.getBool("enable_social", false);
        params.gravity = config.getFloat("gravity", 0.0f);
        params.friction = config.getFloat("friction", 0.1f);
        params.terrainType = config.getString("terrain_type", "flat");
        params.weatherPattern = config.getString("weather_pattern", "none");
        
        // Initialize world state
        initializeWorldState();
        
        // Generate world features
        generateWorldFeatures();
    }
    
    void update(float timeStep) override {
        // Update world physics
        if (params.enablePhysics) {
            updatePhysics(timeStep);
        }
        
        // Update resources
        if (params.enableResources) {
            updateResources(timeStep);
        }
        
        // Update hazards
        if (params.enableHazards) {
            updateHazards(timeStep);
        }
        
        // Update social interactions
        if (params.enableSocial) {
            updateSocialInteractions(timeStep);
        }
        
        // Apply environmental effects
        applyEnvironmentalEffects(timeStep);
        
        // Update agent states
        updateAgentStates(timeStep);
    }
    
    void reset() override {
        // Reset world to initial state
        resetWorldState();
        
        // Regenerate world features
        generateWorldFeatures();
    }
    
    static World* create() {
        return new CustomWorld();
    }
    
private:
    void initializeWorldState() {
        // Initialize agent positions, velocities, etc.
    }
    
    void generateWorldFeatures() {
        // Generate world features based on configuration
        // This would include terrain, resources, hazards, etc.
    }
    
    void updatePhysics(float timeStep) {
        // Update physics simulation
        // This would include collision detection, gravity, friction
    }
    
    void updateResources(float timeStep) {
        // Update resource generation and consumption
        // This would include regeneration, depletion, etc.
    }
    
    void updateHazards(float timeStep) {
        // Update hazard dynamics
        // This would include hazard movement, damage, etc.
    }
    
    void updateSocialInteractions(float timeStep) {
        // Update social interactions between agents
        // This would include communication, cooperation, competition
    }
    
    void applyEnvironmentalEffects(float timeStep) {
        // Apply environmental effects based on configuration
        // This would include weather, temperature, etc.
    }
    
    void updateAgentStates(float timeStep) {
        // Update agent states based on inputs
        // This would include perception, action, learning
    }
    
    void resetWorldState() {
        // Reset world to initial state
    }
};
```

### World Types

#### Grid World

```cpp
class GridWorld : public World {
public:
    void initialize(const WorldConfig& config) override {
        // Initialize grid-based world
        World::initialize(config);
        
        // Set grid dimensions
        setGridDimensions(config.getInt("grid_width", 50),
                         config.getInt("grid_height", 50));
        
        // Initialize grid
        initializeGrid();
    }
    
    static World* create() {
        return new GridWorld();
    }
    
private:
    void initializeGrid() {
        // Initialize grid with terrain
        grid = std::vector<std::vector<TerrainType>>(
            params.height, std::vector<TerrainType>(params.width, TerrainType::PLAIN)
        );
        
        // Add features
        addTerrainFeatures();
        addResources();
        addHazards();
    }
    
    enum class TerrainType {
        PLAIN,
        MOUNTAIN,
        WATER,
        FOREST,
        DESERT
    };
};
```

#### Continuous World

```cpp
class ContinuousWorld : public World {
public:
    void initialize(const WorldConfig& config) override {
        // Initialize continuous world
        World::initialize(config);
        
        // Set world boundaries
        setWorldBoundaries(
            config.getFloat("x_min", -100.0f),
            config.getFloat("x_max", 100.0f),
            config.getFloat("y_min", -100.0f),
            config.getFloat("y_max", 100.0f)
        );
        
        // Initialize continuous state
        initializeContinuousState();
    }
    
    static World* create() {
        return new ContinuousWorld();
    }
    
private:
    void initializeContinuousState() {
        // Initialize continuous agent states
        // This would include positions, velocities, orientations
    }
    
    void setWorldBoundaries(float xMin, float xMax, float yMin, float yMax) {
        // Set world boundary coordinates
    }
};
```

### Registering Custom Worlds

```cpp
// Register world types with NLM factory
void registerCustomWorlds() {
    // Register custom world
    WorldFactory::registerWorld("custom", &CustomWorld::create);
    
    // Register specialized world types
    WorldFactory::registerWorld("grid", &GridWorld::create);
    WorldFactory::registerWorld("continuous", &ContinuousWorld::create);
    
    // Register with configuration parameters
    WorldFactory::registerWorldWithConfig(
        "grid_configured",
        &GridWorld::create,
        {
            {"grid_width", 50},
            {"grid_height", 50},
            {"terrain_type", "flat"}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerCustomWorlds);
```

## Developing Extensions

### Extension Development Workflow

#### 1. Define Extension Interface

```cpp
// Define extension interface
class MyExtensionInterface {
public:
    virtual ~MyExtensionInterface() = default;
    
    // Core extension methods
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual bool initialize(const ExtensionConfig& config) = 0;
    virtual void shutdown() = 0;
    
    // Integration methods
    virtual void integrateWithBrain(Brain& brain) = 0;
    virtual void integrateWithWorld(World& world) = 0;
    virtual void integrateWithAgent(Agent& agent) = 0;
    
    // Query methods
    virtual std::vector<std::string> getRequiredExtensions() const = 0;
    virtual std::vector<std::string> getProvidedExtensions() const = 0;
    
    // Configuration methods
    virtual ExtensionConfig getDefaultConfiguration() const = 0;
    virtual bool validateConfiguration(const ExtensionConfig& config) const = 0;
};
```

#### 2. Implement Extension Class

```cpp
// Implement extension class
class MyExtension : public MyExtensionInterface {
public:
    // Required interface methods
    std::string getName() const override { return "MyExtension"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Example extension implementation"; }
    
    bool initialize(const ExtensionConfig& config) override {
        // Initialize extension with configuration
        // Return true if successful, false otherwise
        return true;
    }
    
    void shutdown() override {
        // Cleanup extension resources
    }
    
    void integrateWithBrain(Brain& brain) override {
        // Integrate with brain systems
        // Register extension components with brain
    }
    
    void integrateWithWorld(World& world) override {
        // Integrate with world systems
        // Add world features or modify world behavior
    }
    
    void integrateWithAgent(Agent& agent) override {
        // Integrate with agent systems
        // Modify agent behavior or capabilities
    }
    
    // Query methods
    std::vector<std::string> getRequiredExtensions() const override {
        return {"core", "plasticity"};
    }
    
    std::vector<std::string> getProvidedExtensions() const override {
        return {"custom_plasticity", "custom_memory"};
    }
    
    // Configuration methods
    ExtensionConfig getDefaultConfiguration() const override {
        ExtensionConfig config;
        config.set("parameter1", 1.0f);
        config.set("parameter2", "default");
        return config;
    }
    
    bool validateConfiguration(const ExtensionConfig& config) const override {
        // Validate configuration
        bool valid = true;
        
        if (config.has("parameter1")) {
            float value = config.getFloat("parameter1");
            if (value < 0.0f || value > 10.0f) {
                valid = false;
            }
        }
        
        return valid;
    }
    
private:
    // Extension state
    bool initialized = false;
    ExtensionConfig currentConfig;
};
```

#### 3. Register Extension

```cpp
// Register extension with NLM
void registerMyExtension() {
    // Register extension class
    ExtensionFactory::registerExtension<MyExtension>("my_extension");
    
    // Register with configuration parameters
    ExtensionFactory::registerExtensionWithConfig<MyExtension>(
        "my_extension_configured",
        {
            {"parameter1", 1.0f},
            {"parameter2", "default"},
            {"parameter3", true}
        }
    );
}

// Call in extension initialization
NLM_EXPORT_EXTENSION(registerMyExtension);
```

#### 4. Build Extension

```bash
# Build extension using CMake
cmake_minimum_required(VERSION 3.16)
project(my_extension VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create extension library
add_library(my_extension SHARED extension.cpp)

# Link with NLM libraries
target_link_libraries(my_extension PRIVATE nlm_core nlm_plasticity)

# Set output name
set_target_properties(my_extension PROPERTIES OUTPUT_NAME "my_extension")

# Install extension
install(TARGETS my_extension
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
```

#### 5. Test Extension

```cpp
// Test extension
#include "extensions/MyExtension.hpp"

int main() {
    // Create NLM brain
    Config config;
    config.set("brain.neuron_count", 100);
    
    auto brain = std::make_unique<Brain>(config);
    brain->initialize();
    
    // Load and initialize extension
    ExtensionManager extensionManager;
    
    if (!extensionManager.loadPlugin("./libmy_extension.so")) {
        std::cerr << "Failed to load extension" << std::endl;
        return 1;
    }
    
    // Get extension instance
    auto extension = extensionManager.getExtension("my_extension");
    
    // Initialize extension
    ExtensionConfig extConfig;
    extConfig.set("parameter1", 2.0f);
    extConfig.set("parameter2", "test");
    
    if (!extension->initialize(extConfig)) {
        std::cerr << "Failed to initialize extension" << std::endl;
        return 1;
    }
    
    // Integrate extension with brain
    extension->integrateWithBrain(*brain);
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    // Shutdown extension
    extension->shutdown();
    
    // Unload extension
    extensionManager.unloadPlugin("my_extension");
    
    return 0;
}
```

## Plugin Development Best Practices

### 1. Extension Design Principles

- **Single Responsibility**: Each extension should have one clear purpose
- **Loose Coupling**: Extensions should have minimal dependencies
- **High Cohesion**: Related functionality should be grouped together
- **Extensibility**: Design for future extensions
- **Testability**: Write unit tests for each extension

### 2. Error Handling

```cpp
// Robust error handling
bool initialize(const ExtensionConfig& config) override {
    try {
        // Initialize extension
        // Validate configuration
        if (!validateConfiguration(config)) {
            throw std::invalid_argument("Invalid configuration");
        }
        
        // Load required resources
        if (!loadResources()) {
            throw std::runtime_error("Failed to load resources");
        }
        
        // Setup logging
        setupLogging(config);
        
        initialized = true;
        currentConfig = config;
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        NLM_LOG_ERROR(std::string("Extension initialization failed: ") + e.what());
        
        // Cleanup
        shutdown();
        
        return false;
    }
}
```

### 3. Configuration Validation

```cpp
bool validateConfiguration(const ExtensionConfig& config) const override {
    bool valid = true;
    
    // Check required parameters
    if (!config.has("required_parameter1")) {
        NLM_LOG_WARN("Missing required parameter: required_parameter1");
        valid = false;
    }
    
    // Check parameter ranges
    if (config.has("parameter1")) {
        float value = config.getFloat("parameter1");
        if (value < MIN_VALUE || value > MAX_VALUE) {
            NLM_LOG_ERROR("Parameter parameter1 out of range: " + std::to_string(value));
            valid = false;
        }
    }
    
    // Check parameter combinations
    if (config.has("parameterA") && config.has("parameterB")) {
        float a = config.getFloat("parameterA");
        float b = config.getFloat("parameterB");
        
        if (a > 0 && b < 0) {
            NLM_LOG_WARN("Incompatible parameter combination: A>0 and B<0");
        }
    }
    
    return valid;
}
```

### 4. Resource Management

```cpp
void shutdown() override {
    try {
        // Shutdown integration with NLM systems
        shutdownIntegration();
        
        // Release resources
        releaseResources();
        
        // Cleanup temporary files
        cleanupTemporaryFiles();
        
        // Reset state
        initialized = false;
        currentConfig.clear();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Extension shutdown failed: ") + e.what());
        // Continue cleanup even if errors occur
    }
}
```

### 5. Thread Safety

```cpp
void integrateWithBrain(Brain& brain) override {
    // Thread-safe integration
    std::lock_guard<std::mutex> lock(integrationMutex);
    
    if (!initialized) {
        throw std::runtime_error("Extension not initialized");
    }
    
    // Integrate with brain
    brain.registerExtension(this);
}
```

### 6. Performance Considerations

```cpp
void initialize(const ExtensionConfig& config) override {
    // Profile extension startup time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize extension
    // ...
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (duration.count() > 1000) {
        NLM_LOG_WARN("Extension initialization took " + std::to_string(duration.count()) + " ms");
    }
}
```

## Testing Extensions

### Unit Testing Framework

```cpp
// Unit tests for extensions
#include "extensions/MyExtension.hpp"
#include <gtest/gtest.h>

class MyExtensionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test configuration
        config.set("parameter1", 1.0f);
        config.set("parameter2", "test");
        
        // Create extension
        extension = std::make_unique<MyExtension>();
    }
    
    void TearDown() override {
        extension->shutdown();
        extension.reset();
    }
    
    ExtensionConfig config;
    std::unique_ptr<MyExtension> extension;
};

TEST_F(MyExtensionTest, Initialization) {
    EXPECT_TRUE(extension->initialize(config));
    EXPECT_TRUE(extension->isInitialized());
}

TEST_F(MyExtensionTest, ConfigurationValidation) {
    ExtensionConfig invalidConfig;
    invalidConfig.set("parameter1", 100.0f); // Invalid value
    
    EXPECT_FALSE(extension->validateConfiguration(invalidConfig));
}

TEST_F(MyExtensionTest, Integration) {
    EXPECT_TRUE(extension->initialize(config));
    
    // Create test brain
    Config brainConfig;
    brainConfig.set("brain.neuron_count", 10);
    
    auto brain = std::make_unique<Brain>(brainConfig);
    brain->initialize();
    
    // Integrate extension
    extension->integrateWithBrain(*brain);
    
    // Verify integration
    EXPECT_TRUE(brain->hasExtension("my_extension"));
}
```

### Integration Testing

```cpp
// Integration tests for extension with NLM
#include "extensions/MyExtension.hpp"
#include <gtest/gtest.h>

class ExtensionIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create NLM brain
        Config brainConfig;
        brainConfig.set("brain.neuron_count", 100);
        brainConfig.set("brain.synapse_density", 0.1);
        
        brain = std::make_unique<Brain>(brainConfig);
        brain->initialize();
        
        // Load extension
        ExtensionManager extensionManager;
        
        if (!extensionManager.loadPlugin("./libmy_extension.so")) {
            FAIL() << "Failed to load extension";
        }
        
        extension = extensionManager.getExtension("my_extension");
        
        // Initialize extension
        ExtensionConfig extConfig;
        extConfig.set("parameter1", 2.0f);
        
        if (!extension->initialize(extConfig)) {
            FAIL() << "Failed to initialize extension";
        }
        
        // Integrate extension
        extension->integrateWithBrain(*brain);
    }
    
    void TearDown() override {
        if (extension) {
            extension->shutdown();
        }
        
        if (brain) {
            brain.reset();
        }
    }
    
    std::unique_ptr<Brain> brain;
    std::unique_ptr<ExtensionPlugin> extension;
};

TEST_F(ExtensionIntegrationTest, SimulationWithExtension) {
    // Run simulation with extension active
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Verify extension effects
        EXPECT_TRUE(extension->isActive());
    }
}

TEST_F(ExtensionIntegrationTest, ExtensionPerformance) {
    // Measure extension performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Check performance
    double stepsPerSecond = 1000.0 / (duration.count() / 1000.0);
    EXPECT_GT(stepsPerSecond, 1000.0); // Should process at least 1000 steps per second
}
```

## Deployment and Installation

### Installation Script

```bash
#!/bin/bash
# install_extension.sh

set -e

EXTENSION_NAME="my_extension"
EXTENSION_VERSION="1.0.0"
BUILD_DIRECTORY="build"
INSTALL_PREFIX="/usr/local"

# Check if extension is already installed
if [ -d "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}" ]; then
    echo "Extension ${EXTENSION_NAME} is already installed"
    exit 0
fi

# Create installation directory
mkdir -p "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}"

# Copy extension library
cp "${BUILD_DIRECTORY}/lib${EXTENSION_NAME}.so" "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}/"

# Copy extension header
mkdir -p "${INSTALL_PREFIX}/include/${EXTENSION_NAME}"
cp "include/${EXTENSION_NAME}.h" "${INSTALL_PREFIX}/include/${EXTENSION_NAME}/"

# Create configuration directory
mkdir -p "${INSTALL_PREFIX}/etc/nlm/extensions"

# Create configuration file
cat > "${INSTALL_PREFIX}/etc/nlm/extensions/${EXTENSION_NAME}.conf" << EOF
# Configuration for ${EXTENSION_NAME} extension
# Version: ${EXTENSION_VERSION}

[extension]
name = ${EXTENSION_NAME}
version = ${EXTENSION_VERSION}
enabled = true

[parameters]
parameter1 = 1.0
parameter2 = default

EOF

# Update shared library cache
ldconfig

# Verify installation
echo "Extension ${EXTENSION_NAME} ${EXTENSION_VERSION} installed successfully"
echo "Installation directory: ${INSTALL_PREFIX}/lib/${EXTENSION_NAME}"
```

### Uninstallation Script

```bash
#!/bin/bash
# uninstall_extension.sh

set -e

EXTENSION_NAME="my_extension"
INSTALL_PREFIX="/usr/local"

# Check if extension is installed
if [ ! -d "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}" ]; then
    echo "Extension ${EXTENSION_NAME} is not installed"
    exit 0
fi

# Remove extension library
rm -rf "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}"

# Remove extension headers
rm -rf "${INSTALL_PREFIX}/include/${EXTENSION_NAME}"

# Remove configuration file
rm -f "${INSTALL_PREFIX}/etc/nlm/extensions/${EXTENSION_NAME}.conf"

# Update shared library cache
ldconfig

# Verify uninstallation
if [ ! -d "${INSTALL_PREFIX}/lib/${EXTENSION_NAME}" ]; then
    echo "Extension ${EXTENSION_NAME} uninstalled successfully"
else
    echo "Failed to uninstall extension ${EXTENSION_NAME}"
    exit 1
fi
```

## Plugin Registry

### Registry Structure

```cpp
// Plugin registry structure
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string email;
    std::string license;
    std::string homepage;
    
    std::vector<std::string> dependencies;
    std::vector<std::string> providedExtensions;
    
    std::string filePath;
    uint64_t fileSize;
    std::string checksum;
    
    std::chrono::system_clock::time_point lastModified;
    std::chrono::system_clock::time_point installedDate;
    
    bool enabled;
    bool required;
};
```

### Plugin Registry Implementation

```cpp
class PluginRegistry {
public:
    // Plugin registration
    void registerPlugin(const PluginInfo& plugin) {
        plugins[plugin.name] = plugin;
    }
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins(const std::string& directory) {
        std::vector<PluginInfo> discoveredPlugins;
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                PluginInfo plugin = parsePluginFile(entry.path());
                if (plugin.isValid()) {
                    discoveredPlugins.push_back(plugin);
                }
            }
        }
        
        return discoveredPlugins;
    }
    
    // Plugin management
    bool enablePlugin(const std::string& name) {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        it->second.enabled = true;
        return true;
    }
    
    bool disablePlugin(const std::string& name) {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        it->second.enabled = false;
        return true;
    }
    
    // Plugin loading
    std::unique_ptr<ExtensionPlugin> loadPlugin(const std::string& name) {
        auto it = plugins.find(name);
        if (it == plugins.end() || !it->second.enabled) {
            return nullptr;
        }
        
        return loadPluginFromFile(it->second.filePath);
    }
    
    // Plugin listing
    void listPlugins() const {
        std::cout << "Registered plugins:" << std::endl;
        
        for (const auto& pair : plugins) {
            const PluginInfo& plugin = pair.second;
            
            std::cout << "  " << plugin.name << " " << plugin.version << std::endl;
            std::cout << "    " << plugin.description << std::endl;
            std::cout << "    Author: " << plugin.author << std::endl;
            std::cout << "    Status: " << (plugin.enabled ? "enabled" : "disabled") << std::endl;
            
            if (!plugin.dependencies.empty()) {
                std::cout << "    Dependencies: ";
                for (const auto& dep : plugin.dependencies) {
                    std::cout << dep << " ";
                }
                std::cout << std::endl;
            }
            
            if (!plugin.providedExtensions.empty()) {
                std::cout << "    Provides: ";
                for (const auto& ext : plugin.providedExtensions) {
                    std::cout << ext << " ";
                }
                std::cout << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
private:
    PluginInfo parsePluginFile(const std::filesystem::path& filePath) {
        PluginInfo plugin;
        
        // Parse plugin metadata from file
        // This would typically involve reading a manifest file or
        // extracting metadata from the plugin library
        
        return plugin;
    }
    
    std::unique_ptr<ExtensionPlugin> loadPluginFromFile(const std::string& filePath) {
        // Load plugin from file
        // This would involve dynamic loading using dlopen
    }
    
    std::unordered_map<std::string, PluginInfo> plugins;
};
```

### Plugin Command Line Interface

```cpp
// Plugin management command line interface
class PluginCLI {
public:
    void run(int argc, char* argv[]) {
        if (argc < 2) {
            showHelp();
            return;
        }
        
        std::string command = argv[1];
        
        if (command == "list") {
            listPlugins();
        } else if (command == "enable") {
            if (argc < 3) {
                std::cerr << "Error: Plugin name required" << std::endl;
                return;
            }
            enablePlugin(argv[2]);
        } else if (command == "disable") {
            if (argc < 3) {
                std::cerr << "Error: Plugin name required" << std::endl;
                return;
            }
            disablePlugin(argv[2]);
        } else if (command == "load") {
            if (argc < 3) {
                std::cerr << "Error: Plugin file required" << std::endl;
                return;
            }
            loadPlugin(argv[2]);
        } else if (command == "info") {
            if (argc < 3) {
                std::cerr << "Error: Plugin name required" << std::endl;
                return;
            }
            showPluginInfo(argv[2]);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            showHelp();
        }
    }
    
private:
    void showHelp() {
        std::cout << "Plugin Management Commands:" << std::endl;
        std::cout << "  list          - List all registered plugins" << std::endl;
        std::cout << "  enable <name> - Enable a plugin" << std::endl;
        std::cout << "  disable <name> - Disable a plugin" << std::endl;
        std::cout << "  load <file>   - Load a plugin from file" << std::endl;
        std::cout << "  info <name>   - Show plugin information" << std::endl;
    }
    
    void listPlugins() {
        extensionRegistry.listPlugins();
    }
    
    void enablePlugin(const std::string& name) {
        if (extensionRegistry.enablePlugin(name)) {
            std::cout << "Plugin " << name << " enabled" << std::endl;
        } else {
            std::cerr << "Failed to enable plugin: " << name << std::endl;
        }
    }
    
    void disablePlugin(const std::string& name) {
        if (extensionRegistry.disablePlugin(name)) {
            std::cout << "Plugin " << name << " disabled" << std::endl;
        } else {
            std::cerr << "Failed to disable plugin: " << name << std::endl;
        }
    }
    
    void loadPlugin(const std::string& filePath) {
        auto plugin = extensionRegistry.loadPluginFromFile(filePath);
        if (plugin) {
            std::cout << "Plugin loaded: " << plugin->getName() << std::endl;
        } else {
            std::cerr << "Failed to load plugin: " << filePath << std::endl;
        }
    }
    
    void showPluginInfo(const std::string& name) {
        auto it = extensionRegistry.getPlugin(name);
        if (it != extensionRegistry.end()) {
            showPluginInfo(*it);
        } else {
            std::cerr << "Plugin not found: " << name << std::endl;
        }
    }
    
    void showPluginInfo(const PluginInfo& plugin) {
        std::cout << "Plugin Information:" << std::endl;
        std::cout << "  Name: " << plugin.name << std::endl;
        std::cout << "  Version: " << plugin.version << std::endl;
        std::cout << "  Description: " << plugin.description << std::endl;
        std::cout << "  Author: " << plugin.author << std::endl;
        std::cout << "  Email: " << plugin.email << std::endl;
        std::cout << "  License: " << plugin.license << std::endl;
        std::cout << "  Homepage: " << plugin.homepage << std::endl;
        std::cout << "  Enabled: " << (plugin.enabled ? "Yes" : "No") << std::endl;
        
        if (!plugin.dependencies.empty()) {
            std::cout << "  Dependencies:" << std::endl;
            for (const auto& dep : plugin.dependencies) {
                std::cout << "    - " << dep << std::endl;
            }
        }
        
        if (!plugin.providedExtensions.empty()) {
            std::cout << "  Provided Extensions:" << std::endl;
            for (const auto& ext : plugin.providedExtensions) {
                std::cout << "    - " << ext << std::endl;
            }
        }
    }
    
    PluginRegistry extensionRegistry;
};
```

## Extension API Reference

### Factory Interfaces

#### ExtensionFactory

```cpp
class ExtensionFactory {
public:
    template<typename T>
    static void registerExtension(const std::string& name) {
        // Register extension class
        extensionRegistry.registerExtension<T>(name);
    }
    
    template<typename T>
    static void registerExtensionWithConfig(
        const std::string& name,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        // Register extension class with configuration template
        extensionRegistry.registerExtensionWithConfig<T>(name, configTemplate);
    }
    
    template<typename T>
    static std::unique_ptr<T> createExtension(const std::string& name) {
        // Create extension instance
        return extensionRegistry.createExtension<T>(name);
    }
    
    static void listExtensions() {
        extensionRegistry.listExtensions();
    }
    
    static bool hasExtension(const std::string& name) {
        return extensionRegistry.hasExtension(name);
    }
    
private:
    ExtensionRegistry extensionRegistry;
};
```

#### PlasticityFactory

```cpp
class PlasticityFactory {
public:
    using PlasticityCreator = std::function<PlasticityRule*()>;
    
    static void registerRule(const std::string& name, PlasticityCreator creator) {
        plasticityRegistry.registerRule(name, creator);
    }
    
    static void registerRuleWithConfig(
        const std::string& name,
        PlasticityCreator creator,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        plasticityRegistry.registerRuleWithConfig(name, creator, configTemplate);
    }
    
    static PlasticityRule* createRule(const std::string& name) {
        return plasticityRegistry.createRule(name);
    }
    
    static bool hasRule(const std::string& name) {
        return plasticityRegistry.hasRule(name);
    }
    
    static void listRules() {
        plasticityRegistry.listRules();
    }
    
private:
    PlasticityRegistry plasticityRegistry;
};
```

#### MemoryFactory

```cpp
class MemoryFactory {
public:
    using MemoryCreator = std::function<Memory*()>;
    
    static void registerMemory(const std::string& name, MemoryCreator creator) {
        memoryRegistry.registerMemory(name, creator);
    }
    
    static void registerMemoryWithConfig(
        const std::string& name,
        MemoryCreator creator,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        memoryRegistry.registerMemoryWithConfig(name, creator, configTemplate);
    }
    
    static Memory* createMemory(const std::string& name) {
        return memoryRegistry.createMemory(name);
    }
    
    static bool hasMemory(const std::string& name) {
        return memoryRegistry.hasMemory(name);
    }
    
    static void listMemories() {
        memoryRegistry.listMemories();
    }
    
private:
    MemoryRegistry memoryRegistry;
};
```

#### NeuromodulatorFactory

```cpp
class NeuromodulatorFactory {
public:
    using NeuromodulatorCreator = std::function<Neuromodulator*()>;
    
    static void registerNeuromodulator(const std::string& name, NeuromodulatorCreator creator) {
        neuromodulatorRegistry.registerNeuromodulator(name, creator);
    }
    
    static void registerNeuromodulatorWithConfig(
        const std::string& name,
        NeuromodulatorCreator creator,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        neuromodulatorRegistry.registerNeuromodulatorWithConfig(name, creator, configTemplate);
    }
    
    static Neuromodulator* createNeuromodulator(const std::string& name) {
        return neuromodulatorRegistry.createNeuromodulator(name);
    }
    
    static bool hasNeuromodulator(const std::string& name) {
        return neuromodulatorRegistry.hasNeuromodulator(name);
    }
    
    static void listNeuromodulators() {
        neuromodulatorRegistry.listNeuromodulators();
    }
    
private:
    NeuromodulatorRegistry neuromodulatorRegistry;
};
```

#### CognitiveSystemFactory

```cpp
class CognitiveSystemFactory {
public:
    using CognitiveSystemCreator = std::function<CognitiveSystem*()>;
    
    static void registerSystem(const std::string& name, CognitiveSystemCreator creator) {
        cognitiveSystemRegistry.registerSystem(name, creator);
    }
    
    static void registerSystemWithConfig(
        const std::string& name,
        CognitiveSystemCreator creator,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        cognitiveSystemRegistry.registerSystemWithConfig(name, creator, configTemplate);
    }
    
    static CognitiveSystem* createSystem(const std::string& name) {
        return cognitiveSystemRegistry.createSystem(name);
    }
    
    static bool hasSystem(const std::string& name) {
        return cognitiveSystemRegistry.hasSystem(name);
    }
    
    static void listSystems() {
        cognitiveSystemRegistry.listSystems();
    }
    
private:
    CognitiveSystemRegistry cognitiveSystemRegistry;
};
```

#### WorldFactory

```cpp
class WorldFactory {
public:
    using WorldCreator = std::function<World*()>;
    
    static void registerWorld(const std::string& name, WorldCreator creator) {
        worldRegistry.registerWorld(name, creator);
    }
    
    static void registerWorldWithConfig(
        const std::string& name,
        WorldCreator creator,
        const std::unordered_map<std::string, std::string>& configTemplate) {
        worldRegistry.registerWorldWithConfig(name, creator, configTemplate);
    }
    
    static World* createWorld(const std::string& name) {
        return worldRegistry.createWorld(name);
    }
    
    static bool hasWorld(const std::string& name) {
        return worldRegistry.hasWorld(name);
    }
    
    static void listWorlds() {
        worldRegistry.listWorlds();
    }
    
private:
    WorldRegistry worldRegistry;
};
```

### Extension Manager

```cpp
class ExtensionManager {
public:
    // Extension loading
    bool loadExtension(const std::string& name) {
        auto extension = extensionRegistry.createExtension<ExtensionPlugin>(name);
        if (!extension) {
            return false;
        }
        
        // Initialize extension
        ExtensionConfig config = getDefaultExtensionConfig(name);
        if (!extension->initialize(config)) {
            return false;
        }
        
        extensions[name] = std::move(extension);
        return true;
    }
    
    bool unloadExtension(const std::string& name) {
        auto it = extensions.find(name);
        if (it == extensions.end()) {
            return false;
        }
        
        it->second->shutdown();
        extensions.erase(it);
        return true;
    }
    
    // Extension integration
    void integrateWithBrain(Brain& brain) {
        for (auto& pair : extensions) {
            extension->integrateWithBrain(brain);
        }
    }
    
    void integrateWithWorld(World& world) {
        for (auto& pair : extensions) {
            extension->integrateWithWorld(world);
        }
    }
    
    // Extension query
    ExtensionPlugin* getExtension(const std::string& name) {
        auto it = extensions.find(name);
        if (it == extensions.end()) {
            return nullptr;
        }
        
        return it->second.get();
    }
    
    bool hasExtension(const std::string& name) const {
        return extensions.find(name) != extensions.end();
    }
    
    // Extension listing
    void listExtensions() const {
        for (const auto& pair : extensions) {
            std::cout << pair.first << std::endl;
        }
    }
    
    // Extension configuration
    bool updateExtensionConfig(const std::string& name, const ExtensionConfig& config) {
        auto it = extensions.find(name);
        if (it == extensions.end()) {
            return false;
        }
        
        return it->second->initialize(config);
    }
    
private:
    ExtensionConfig getDefaultExtensionConfig(const std::string& name) {
        ExtensionConfig config;
        
        // Get default configuration for extension
        // This would involve looking up the extension in the registry
        
        return config;
    }
    
    std::unordered_map<std::string, std::unique_ptr<ExtensionPlugin>> extensions;
    ExtensionRegistry extensionRegistry;
};
```

## Troubleshooting

### Common Extension Issues

#### Issue 1: Extension Not Found

**Problem:** Extension cannot be found or loaded.

**Solution:**

```bash
# Check if extension is registered
./nlm_extension_manager list

# Check if extension file exists
ls -la ./libmy_extension.so

# Check for missing dependencies
ldd ./libmy_extension.so
```

#### Issue 2: Extension Initialization Failed

**Problem:** Extension fails to initialize.

**Solution:**

```cpp
// Add debug output to extension initialization
bool MyExtension::initialize(const ExtensionConfig& config) override {
    try {
        NLM_LOG_INFO("Initializing MyExtension");
        NLM_LOG_INFO("Configuration: " + config.toString());
        
        // Initialize extension
        // ...
        
        if (!validateConfiguration(config)) {
            NLM_LOG_ERROR("Configuration validation failed");
            return false;
        }
        
        NLM_LOG_INFO("MyExtension initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Extension initialization failed: ") + e.what());
        return false;
    }
}
```

#### Issue 3: Extension Integration Failed

**Problem:** Extension fails to integrate with NLM systems.

**Solution:**

```cpp
// Add debug output to extension integration
void MyExtension::integrateWithBrain(Brain& brain) override {
    try {
        NLM_LOG_INFO("Integrating MyExtension with Brain");
        
        // Check if extension is initialized
        if (!isInitialized()) {
            NLM_LOG_ERROR("Extension not initialized");
            return;
        }
        
        // Integrate with brain
        brain.registerExtension(this);
        
        // Verify integration
        if (!brain.hasExtension("my_extension")) {
            NLM_LOG_ERROR("Failed to verify extension integration");
            return;
        }
        
        NLM_LOG_INFO("MyExtension integrated with Brain successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Extension integration failed: ") + e.what());
    }
}
```

#### Issue 4: Performance Issues

**Problem:** Extension causes performance degradation.

**Solution:**

```cpp
// Add performance profiling
void MyExtension::initialize(const ExtensionConfig& config) override {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize extension
    // ...
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (duration.count() > 1000) {
        NLM_LOG_WARN(std::string("Extension initialization took ") + 
                    std::to_string(duration.count()) + " ms");
    }
}

void MyExtension::step(const BrainState& state) override {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process state
    // ...
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Log performance
    if (duration.count() > 10000) {
        NLM_LOG_WARN(std::string("Extension step took ") + 
                    std::to_string(duration.count() / 1000.0) + " ms");
    }
}
```

### Debugging Tools

#### Extension Debug Console

```cpp
// Extension debug console
class ExtensionDebugConsole {
public:
    void start() {
        std::cout << "=== Extension Debug Console ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help          - Show help information" << std::endl;
        std::cout << "  list          - List all extensions" << std::endl;
        std::cout << "  info <name>   - Show extension information" << std::endl;
        std::cout << "  enable <name> - Enable extension" << std::endl;
        std::cout << "  disable <name> - Disable extension" << std::endl;
        std::cout << "  config <name> - Show extension configuration" << std::endl;
        std::cout << "  exit          - Exit debug console" << std::endl;
        
        while (running) {
            std::string command = readInput(">>> ");
            processCommand(command);
        }
    }
    
private:
    void processCommand(const std::string& command) {
        if (command == "exit") {
            running = false;
        } else if (command == "list") {
            listExtensions();
        } else if (command.find("info ") == 0) {
            showExtensionInfo(command.substr(5));
        } else if (command.find("enable ") == 0) {
            enableExtension(command.substr(7));
        } else if (command.find("disable ") == 0) {
            disableExtension(command.substr(8));
        } else if (command.find("config ") == 0) {
            showExtensionConfig(command.substr(7));
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            showHelp();
        }
    }
    
    std::string readInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    bool running = true;
};
```

## Future Extension Directions

### Emerging Extension Types

#### AI/ML Extensions

- **Machine Learning Integration**: Integrate with popular ML frameworks (TensorFlow, PyTorch)
- **Model Loading**: Load pre-trained models for different cognitive functions
- **Transfer Learning**: Apply knowledge from one domain to another
- **Meta-Learning**: Learn how to learn new tasks quickly

#### Neuroscientific Extensions

- **Biologically Realistic Models**: Implement Hodgkin-Huxley neuron models
- **Neural Circuits**: Model real neural circuits (e.g., hippocampus, cortex)
- **Neurotransmission**: Simulate neurotransmitter dynamics
- **Gene Regulation**: Model gene expression effects on brain development

#### Cognitive Science Extensions

- **Theory of Mind**: Model mental state reasoning
- **Moral Cognition**: Model moral decision-making
- **Social Cognition**: Model social perception and behavior
- **Language Processing**: Model language comprehension and production

#### Computational Neuroscience Extensions

- **Network Neuroscience**: Model brain networks and connectivity
- **Systems Neuroscience**: Model brain systems and functions
- **Computational Modeling**: Develop mathematical models of cognitive processes
- **Cognitive Architectures**: Implement cognitive architectures (ACT-R, SOAR)

### Future Research Directions

#### Theoretical Extensions

- **Unified Theory of Cognition**: Develop comprehensive theory of cognitive processes
- ** Predictive Processing**: Implement predictive coding framework
- **Active Inference**: Model agent-environment interaction
- **Bayesian Brain**: Apply Bayesian statistics to cognitive modeling

#### Methodological Extensions

- **Multi-scale Modeling**: Model brain activity at multiple scales
- **Interdisciplinary Integration**: Integrate knowledge from multiple disciplines
- **Open Science**: Promote open source and open data practices
- **Reproducibility**: Improve reproducibility of cognitive models

#### Application Extensions

- **Brain-Computer Interfaces**: Interface directly with brain activity
- **Neurotechnology**: Develop new neurotechnologies
- **Cognitive Enhancement**: Enhance cognitive abilities
- **Mental Health**: Model and treat mental health conditions

## References

### Bibliography

1. **NLM Documentation**: NLM Phase 6 Documentation
2. **Extension Development**: Guide to developing NLM extensions
3. **Plugin System**: NLM plugin system architecture
4. **Configuration Management**: NLM configuration system
5. **Factory Pattern**: Factory pattern for extension creation
6. **Integration Points**: NLM integration points for extensions
7. **Testing Framework**: NLM extension testing framework
8. **Deployment Guide**: NLM extension deployment and installation

### Online Resources

- **NLM GitHub Repository**: https://github.com/your-org/nlm
- **NLM Documentation**: https://docs.your-org.com/nlm
- **NLM Extensions**: https://extensions.your-org.com/nlm
- **Extension Development Forum**: https://forum.your-org.com/extensions
- **Extension Discord**: https://discord.gg/your-org/extensions

### Technical Documentation

- **Extension API Reference**: `docs/api/extension/`
- **Plugin Development Guide**: `docs/extension/plugin/`
- **Extension Testing Guide**: `docs/extension/testing/`
- **Extension Deployment Guide**: `docs/extension/deployment/`
- **Extension Best Practices**: `docs/extension/best_practices/`

---

*NLM Extension Guide Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*