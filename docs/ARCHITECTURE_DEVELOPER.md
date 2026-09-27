# NLM Architecture Developer Guide

## Overview

NLM (熙然) — An experimental artificial developmental brain with **Phase 6: Full Integration**

This document provides comprehensive technical guidance for developers wanting to extend and modify the NLM neural simulation framework. It focuses on the **Phase 6 integration challenge**: connecting existing disconnected components into a coherent artificial brain.

## 1. System Architecture Overview for Developers

### Current Architecture State

**NLM has 12+ sophisticated component systems, but most are disconnected:**

```
┌─────────────────────────────────────────────────────────────────┐
│                    COHERENT ARTIFICIAL BRAIN                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐│
│  │  SENSORY       │  │   COGNITIVE     │  │    MOTOR       ││
│  │   INPUT       │  │   SYSTEMS       │  │   OUTPUT       ││
│  └─────────────────┘  └─────────────────┘  └─────────────────┘│
│      │                      │                        │        │
│      ▼                      ▼                        ▼        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐│
│  │   AGENTS       │  │   MEMORY        │  │   DEVELOPMENT   ││
│  │   (AgentBrain) │  │   SYSTEMS       │  │   SYSTEMS       ││
│  └─────────────────┘  └─────────────────┘  └─────────────────┘│
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    COMPUTATIONAL CORE                       ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │NEURONS │  │PLASTICITY│  │DYAMICS │  │  BRAIN  │       ││
│  │  │   (LIF)│  │   (STDP)│  │(SPIKING)│  │ COORD   │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### Layered Architecture

#### Core Layer (foundation)
- **Types**: Strong identifiers (NeuronId, SynapseId, RegionId, etc.)
- **Config**: Hierarchical configuration system with JSON support
- **Logger**: Structured logging with levels and filtering
- **Random**: Deterministic thread-safe RNG for reproducibility
- **SimulationClock**: Monotonic time management

#### Neural Computation Layer
- **Brain**: Central coordinator with LIF neuron dynamics
- **Neuron**: Leaky Integrate-and-Fire neuron model
- **Synapse**: Synaptic connections with delays and plasticity
- **NeuralRegion/NeuralPopulation**: Hierarchical organization
- **SpikeSystem**: Event-driven spike propagation

#### Functionality Layers (currently disconnected)
- **Plasticity**: STDP, Hebbian, structural plasticity
- **Development**: Synaptogenesis, pruning, maturation stages
- **Neuromodulation**: Dopamine, novelty, curiosity, prediction error
- **Memory**: Working, episodic, associative memory
- **Prediction**: Temporal sequence learning and prediction error
- **Cognition**: Attention, planning, concept formation
- **Sensory**: Vision, touch, internal signals processing
- **Motor**: Action selection and motor command decoding
- **Environment**: SimpleWorld simulation interface

### Phase 6 Integration Goal

**Primary Challenge**: The NLM brain has the **anatomy of a cognitive system but lacks integration**. Most systems exist but don't interact cohesively.

**Current Integration Status**:
- ✅ **Neural Computation**: LIF dynamics, STDP, structural plasticity
- ✅ **Basic Agent-World Interface**: AgentBrain connects brain to world
- ✅ **Memory System Structures**: Memory classes exist but return nullptr
- ❌ **Working Memory**: Not connected to sensory processing
- ❌ **Episodic Memory**: Not storing experiences
- ❌ **Prediction System**: Not being used
- ❌ **Cognition**: Planning, attention, concept formation disconnected
- ❌ **Full Neuromodulation Integration**: Partial implementation
- ❌ **Checkpoint/Save-Load**: Not functional

## 2. Component Interaction Patterns and Data Flows

### Primary Data Flow Paths

#### Phase 2: Neural Computation Flow
```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Structural Plasticity (synaptogenesis/pruning)
```

#### Phase 3: Agent-World Interaction Flow
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis/pruning)
      ↓
Development System (stage progression)
```

#### Phase 6: Full Integration Flow
```
ENVIRONMENT (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (working memory competition)
      ↓
PREDICTION (temporal sequence learning)
      ↓
WORKING MEMORY (persistent activity storage)
      ↓
EPISODIC MEMORY (experience encoding)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step action selection)
      ↓
SELF-MODEL (sensorimotor self-awareness)
      ↓
MOTOR ACTION
      ↓
ENVIRONMENT.applyAction()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION (dopamine, novelty, curiosity)
      ↓
PLASTICITY (all forms)
      ↓
SOCIAL LEARNING (observation/imitation)
```

### Integration Points

#### Direct Coupling
- **Neurons ↔ Synapses ↔ Plasticity**: Tight coupling, essential for function
- **Dopamine ↔ Eligibility Traces ↔ Synaptic Weights**: Core learning mechanism
- **Novelty ↔ Curiosity ↔ Exploration**: Exploration motivation chain
- **Development ↔ Plasticity Rates ↔ Structural Plasticity**: Age-dependent learning

#### Loose Coupling (potential independent operation)
- **Working Memory**: Stores current neural activity, influences attention
- **Episodic Memory**: Stores experiences, enables replay and consolidation
- **Prediction System**: Computes prediction errors for curiosity
- **Concept Formation**: Organizes patterns without direct coupling

## 3. Extension Guidelines and API Design

### Extension Patterns

#### Adding New Plasticity Rules
```cpp
// Pattern in src/plasticity/PlasticityRule.hpp
class NewPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
};

// Integration in Brain.hpp/cpp
void Brain::step(...) {
    // Call new rule after existing plasticity
    pImpl->plasticitySystem->applyNewRule(synapse, preSpikes, postSpikes, dt);
}
```

#### Adding New Neuromodulators
```cpp
// Pattern in src/neuromodulation/Neuromodulator.hpp
class NewNeuromodulator : public Neuromodulator {
public:
    void signalEvent(float signal);
    float getEffectStrength() const;
    void update(TimestepDuration dt) override;
};

// Integration in Brain::step()
void Brain::step(...) {
    // Apply modulation effects
    pImpl->dopamine->update(dt);
    pImpl->novelty->update(dt);
    pImpl->curiosity->update(dt);
    // Add new neuromodulator
    pImpl->newModulator->update(dt);
}
```

#### Extending Cognitive Systems
```cpp
// Pattern in src/cognition/NeuralPlanner.hpp
class AdvancedPlanner : public NeuralPlanner {
public:
    std::vector<Action> planActions(
        const WorkingMemory& memory,
        const PredictionSystem& predictions,
        const SelfModel& selfModel) override;
};

// Integration in Brain constructor
Brain::Brain(std::shared_ptr<Config> config) {
    // Set advanced planner
    pImpl->planner = std::make_unique<AdvancedPlanner>(config);
}
```

### Extension Points and APIs

#### Component Access Methods
```cpp
// In src/brain/Brain.hpp
// High-level accessors
Neuron* getNeuron(NeuronId id);
Synapse* getSynapse(SynapseId id);
NeuralRegion* getRegion(RegionId id);

// Memory system accessors (Phase 6 focus)
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

// Prediction system access
PredictionSystem* getPredictionSystem();

// Cognitive system access
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
AttentionalSelection* getAttention();

// Neuromodulation access
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();
```

#### Integration Hooks
```cpp
// Key integration entry points

// 1. Sensory input -> working memory integration
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    brain_->receiveSensoryInput(percept);
    brain_->getWorkingMemory()->store(percept);  // Integration point
}

// 2. Motor output -> planning integration  
std::unique_ptr<Action> Brain::produceAction() {
    auto action = decodeMotorActivity();
    // Integrate with planner
    if (auto* planner = getPlanner()) {
        action = planner->selectAction(action);
    }
    return action;
}

// 3. Learning signals -> memory integration
void AgentBrain::applyRewardModulation(float reward, float predicted) {
    // Apply to plasticity
    // Also apply to memory consolidation
    if (auto* episodicMem = brain_->getEpisodicMemory()) {
        episodicMem->consolidate(reward, predicted);
    }
}
```

## 4. Integration Patterns for Connecting Components

### Integration Patterns

#### Pattern 1: Memory Integration

**Problem**: Memory systems exist but return nullptr, disconnected from neural activity.

**Solution**: Connect neural activity to memory systems.

```cpp
// Integration in Brain::step()
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing neural dynamics ...
    
    // NEW: Connect to working memory
    if (auto* workingMem = pImpl->workingMemory.get()) {
        workingMem->update(pImpl->timestep);
        // Store active neural populations
        storeNeuralActivityInWorkingMemory();
    }
    
    // NEW: Connect to episodic memory (every 10 steps)
    if (currentStep % 10 == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->encodeExperience(currentStep, currentTime);
    }
    
    // NEW: Connect to prediction system
    if (auto* predSys = pImpl->predictionSystem.get()) {
        predSys->trainFromWorkingMemory(*pImpl->workingMemory);
    }
}
```

#### Pattern 2: Neuromodulation Integration

**Problem**: Neuromodulators affect plasticity but don't integrate with higher functions.

**Solution**: Connect neuromodulation to memory and cognition.

```cpp
// Integration in AgentBrain.cpp
void AgentBrain::applyRewardModulation(float reward, float predicted) {
    // Existing plasticity effects
    // ...
    
    // NEW: Memory consolidation effects
    if (auto* episodicMem = brain_->getEpisodicMemory()) {
        float predictionError = reward - predicted;
        episodicMem->modulateConsolidation(predictionError);
    }
    
    // NEW: Attention effects through acetylcholine placeholder
    // (when implemented)
    // attentionSystem_->modulateReverie(dopamineLevel_);
}
```

#### Pattern 3: Cognitive Integration

**Problem**: Cognitive systems (planning, attention, concept formation) are never called.

**Solution**: Integrate cognitive systems into brain loop.

```cpp
// Integration in Brain.cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing systems ...
    
    // NEW: Attention integration
    if (auto* attention = pImpl->attentionSystem.get()) {
        attention->processWorkingMemory(*pImpl->workingMemory);
    }
    
    // NEW: Planning integration
    if (currentStep % 100 == 0 && auto* planner = pImpl->planner.get()) {
        planner->evaluateFutureScenarios(*pImpl->workingMemory);
    }
    
    // NEW: Concept formation integration
    if (auto* conceptForm = pImpl->conceptFormation.get()) {
        conceptForm->analyzeWorkingMemoryPatterns(*pImpl->workingMemory);
    }
}
```

### Integration Implementation Templates

#### Template: Connecting Memory to Perception

```cpp
// File: src/agent/AgentBrain.cpp
// Integration point in processSensoryInput()

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing sensory processing ...
    
    // NEW: Store in working memory
    if (auto* workingMem = brain_->getWorkingMemory()) {
        workingMem->storeVision(0, 0, 16, 16, // vision grid size
                               percept.getVision());
        workingMem->storeTouch(0, 0, 8,
                              percept.getTouch());
        workingMem->storeInternal(0, 0, 4,
                                 percept.getInternal());
        workingMem->storeProprioception(0, 0, 6,
                                       percept.getProprioception());
    }
    
    // NEW: Trigger episodic encoding (agent-level memory)
    // Could be moved to brain loop
    if (auto* episodicMem = brain_->getEpisodicMemory()) {
        episodicMem->recordExperience(percept, currentTime);
    }
}
```

#### Template: Connecting Learning to Memory Consolidation

```cpp
// File: src/agent/AgentBrain.cpp
// Integration point in applyRewardModulation()

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // ... existing plasticity effects ...
    
    // NEW: Memory consolidation on strong prediction errors
    if (auto* episodicMem = brain_->getEpisodicMemory()) {
        float predictionError = std::abs(reward - predictedReward);
        
        if (predictionError > 0.5f) {
            // Strong prediction error = important learning moment
            episodicMem->consolidateMemory(predictionError * 2.0f);
        }
    }
}
```

#### Template: Connecting Cognition to Action Selection

```cpp
// File: src/agent/AgentBrain.cpp
// Integration point in decodeMotorCommand()

MotorCommand AgentBrain::decodeMotorCommand() {
    // ... existing neural decoding ...
    
    // NEW: Integrate with planner if available
    if (auto* planner = brain_->getPlanner()) {
        // Get current context from working memory
        auto context = planner->getCurrentContext(*brain_->getWorkingMemory());
        
        // Get planned actions
        auto plannedActions = planner->evaluatePlans(context);
        
        // Blend planned and reactive actions
        return blendActions(decodedCommand, plannedActions);
    }
    
    return decodedCommand;
}
```

## 5. Performance Considerations and Optimization Strategies

### Performance Analysis

#### Bottleneck Identification

Based on current code analysis, **performance bottlenecks** include:

1. **Memory Access Patterns**: Frequent random access to neurons/synapses
2. **Spike Processing**: Event-driven but potentially O(N²) in dense networks
3. **Plasticity Updates**: STDP and Hebbian require spike history tracking
4. **Integration Overhead**: Additional systems increase computational complexity

#### Current Performance Profiles

```cpp
// Performance characteristics from benchmark.cpp
// Based on 500 neurons, 1ms timestep:
// - Neural dynamics: ~50-100μs per timestep
// - Spike processing: ~10-50μs per event  
// - Plasticity updates: ~20-100μs per synapse
// - Full brain step: ~1-10ms (depending on activity)
```

### Optimization Strategies

#### Strategy 1: Data-Oriented Design (DOD)

**Principle**: Improve cache locality and reduce pointer chasing.

```cpp
// Bad: Object-oriented design with pointers
class Neuron {
    float membranePotential;
    SpikeSystem* spikeSystem;  // Pointer chase
    std::vector<Synapse*> outgoing;
};

// Good: Data-oriented arrays
struct NeuronData {
    float membranePotential;
    float inputCurrent;
    uint32_t spikeCount;
    uint32_t refractoryUntil;
    // Contiguous arrays for spatial locality
};

// Layout: All neurons in contiguous memory
std::vector<NeuronData> neurons;
std::vector<float> synapticWeights;
```

#### Strategy 2: Event-Driven Optimization

**Principle**: Minimize unnecessary computations through intelligent event scheduling.

```cpp
// Optimized spike processing with priority queues
class OptimizedSpikeSystem {
    // Use std::priority_queue for O(log N) delivery
    std::priority_queue<SpikeEvent> spikeQueue;
    
    void processDelayedSpikes(SimulationStep currentStep) {
        while (!spikeQueue.empty() && 
               spikeQueue.top().deliveryTime <= currentStep) {
            auto event = spikeQueue.top(); spikeQueue.pop();
            // Process spike with batch updates
            broadcastSpike(event.synapse, event.targetNeurons);
        }
    }
};
```

#### Strategy 3: Plasticity Optimization

**Principle**: Reduce plasticity computation overhead while maintaining learning quality.

```cpp
// Sparse STDP updates (only on spikes)
class SparseSTDP {
    void updateOnSpike(Neuron* pre, Neuron* post, Timestamp time) {
        // Only update when spikes occur, not every timestep
        auto* synapse = findSynapse(pre, post);
        if (synapse) {
            applySTDP(synapse, pre->lastSpikeTime, post->lastSpikeTime);
        }
    }
};

// Weight clipping to prevent runaway dynamics
void clipWeights(float& weight) {
    weight = std::clamp(weight, -10.0f, 10.0f);
}
```

#### Strategy 4: Memory System Optimization

**Problem**: Memory systems add significant overhead.

**Solution**: Lazy updates and batched operations.

```cpp
// Lazy episodic memory encoding
class LazyEpisodicMemory : public NeuralEpisodicMemory {
    void update(SimulationStep step) {
        // Only encode every N steps, not every step
        if (step % ENCODING_INTERVAL == 0) {
            encodeCurrentState();
        }
        
        // Batch replay operations
        if (shouldReplay(step)) {
            replayImportantEpisodes();  // Batched replay
        }
    }
};
```

### Profiling and Measurement

#### Key Performance Metrics

```cpp
// Metrics to track for optimization
struct PerformanceMetrics {
    double simulationTime;     // Time per brain step
    double memoryUsage;        // Peak memory usage  
    double cacheEfficiency;    // L1/L2 cache hit rates
    double spikeProcessingTime; // Time spent in spike propagation
    double plasticityTime;      // Time spent in weight updates
    double integrationOverhead; // Time added by extra systems
};
```

#### Optimization Workflow

1. **Profile**: Use `perf`, `valgrind`, or custom timers
2. **Identify**: Find hotspots in the code
3. **Optimize**: Apply appropriate optimization strategy
4. **Validate**: Ensure correctness is maintained
5. **Test**: Verify performance improvements

## 6. Scalability Documentation

### Scalability Analysis

#### Current Scalability Limits

Based on implementation patterns and benchmarks:

- **Small Scale** (100-500 neurons): Excellent performance (<100ms per second)
- **Medium Scale** (1K-10K neurons): Acceptable (100ms-1s per second)  
- **Large Scale** (10K-100K neurons): Poor (>1s per second)
- **Very Large** (100K+ neurons): Infeasible with current architecture

#### Bottleneck Analysis for Scaling

1. **O(N²) Connectivity**: Current connection probability (0.15f) leads to dense connections
2. **Spike Processing**: Broadcast pattern becomes expensive with high neuron counts
3. **Memory Usage**: NeuralWorkingMemory stores spike history for all neurons
4. **Plasticity Overhead**: Each spike triggers potential plasticity updates

### Scalability Strategies

#### Strategy 1: Sparse Connectivity

**Principle**: Maintain biological realism while reducing computational cost.

```cpp
// Replace dense with sparse connectivity
void createSparseConnections() {
    // Instead of O(N²), use O(N log N) with spatial locality
    // For each neuron: connect only to nearby neurons
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (auto& region : regions) {
        auto& neurons = region->getNeurons();
        
        // For each neuron, connect only to nearby neurons
        for (size_t i = 0; i < neurons.size(); ++i) {
            auto& neuron = neurons[i];
            
            // Determine neighbor range based on spatial position
            size_t start = getNeighborStart(i);
            size_t end = getNeighborEnd(i);
            
            for (size_t j = start; j < end && j < neurons.size(); ++j) {
                if (dist(rng) < connectionProbability) {
                    createBidirectionalConnection(neurons[i], neurons[j]);
                }
            }
        }
    }
}
```

#### Strategy 2: Regional Partitioning

**Principle**: Divide brain into independent regions with limited inter-region communication.

```cpp
// Regional processing for parallel execution
class RegionalBrain {
    // Multiple regions with limited cross-talk
    std::vector<NeuralRegion> regions;
    
    // Each region processes independently
    void stepRegion(NeuralRegion& region, SimulationStep step) {
        // Process local neurons and connections
        // Only occasional inter-region spikes
    }
    
    // Inter-region connections: sparse and delayed
    void processInterRegionConnections() {
        // Limited to 1-2 connections per neuron on average
        // Delays scale with distance between regions
    }
};
```

#### Strategy 3: Approximate Computation

**Principle**: Accept bounded errors for significant speedup.

```cpp
// Approximate neural dynamics
class ApproximateLIFNeuron {
public:
    void stepApproximate(TimestepDuration dt) {
        // Use Euler integration instead of exact solution
        // Approximate membrane potential update
        float dv = (I - V * gL) / Cm * dt;
        V += dv;
        
        // Approximate spike threshold crossing
        if (V >= threshold) {
            V = resetVoltage;  // Approximate reset
            hasSpiked = true;
        }
    }
};

// Batch spike processing
void batchProcessSpikes(const std::vector<Neuron*>& neurons) {
    // Group spikes by target neuron to reduce updates
    std::unordered_map<Neuron*, std::vector<SpikeEvent>> spikeMap;
    
    for (Neuron* neuron : neurons) {
        if (neuron->hasSpiked) {
            spikeMap[neuron->getTarget()].push_back(createEvent(neuron));
        }
    }
    
    // Process each target once
    for (auto& [target, events] : spikeMap) {
        target->processSpikes(events);
    }
}
```

#### Strategy 4: Memory Management Optimization

**Principle**: Use memory pools and arena allocation.

```cpp
// Memory pool for neural components
class NeuronPool {
    struct alignas(64) NeuronData {
        float membranePotential;
        float inputCurrent;
        // Cache-friendly layout
    };
    
    std::vector<NeuronData> neurons;
    std::vector<uint32_t> freeIndices;
    
    Neuron* allocate() {
        if (freeIndices.empty()) {
            neurons.emplace_back();
            return &neurons.back();
        }
        return &neurons[freeIndices.back()];
    }
};

// Arena allocation for temporary data
class MemoryArena {
    char* buffer;
    size_t capacity;
    size_t position;
    
    template<typename T>
    T* allocate(size_t count = 1) {
        size_t size = sizeof(T) * count;
        if (position + size > capacity) {
            // Resize arena if needed
            grow(size);
        }
        T* result = reinterpret_cast<T*>(buffer + position);
        position += size;
        return result;
    }
};
```

### Scaling Guidelines

#### When to Use Each Strategy

| Scale | Recommended Strategy | Rationale |
|-------|---------------------|-----------|
| **<1K neurons** | Current architecture | Low overhead, easy debugging |
| **1K-10K neurons** | Sparse + Regional | Balance performance and biology |
| **10K-100K neurons** | Regional + Approximate | Need for parallel execution |
| **100K+ neurons** | Distributed computing | Architecture cannot handle single core |

#### Implementation Roadmap

1. **Phase 1**: Profile current implementation with 1K neurons
2. **Phase 2**: Implement sparse connectivity (target 5K neurons)
3. **Phase 3**: Add regional partitioning (target 50K neurons)
4. **Phase 4**: Implement approximate computation (target 500K neurons)
5. **Phase 5**: Distributed architecture for >1M neurons

### Scaling Measurements

```cpp
// Performance measurements at different scales
target_neuron_counts = {100, 1000, 10000, 100000};

for (auto neurons : target_neuron_counts) {
    auto config = createDefaultConfig();
    config->set("neuron_count", neurons);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    double timePerSecond = (end - start) / 1000.0;  // 1000 simulation steps
    std::cout << "Scale: " << neurons << " neurons, "
              << "Time per simulation second: " << timePerSecond.count() << "s\n";
}
```

## Integration Priority for Developers

### Immediate Actions (Week 1-2)

1. **Fix Memory Integration**: Make `getWorkingMemory()` and `getEpisodicMemory()` return actual instances
2. **Connect Perception to Memory**: Store sensory input in working memory during `processSensoryInput()`
3. **Enable Prediction System**: Use prediction errors to update curiosity and exploration

### Short-term Actions (Week 3-4)

1. **Connect Cognition**: Integrate planner, concept formation, and attention systems
2. **Full Neuromodulation**: Connect dopamine to memory consolidation and curiosity to exploration
3. **Performance Optimization**: Profile and optimize memory access patterns

### Long-term Actions (Month 1+)

1. **Regional Architecture**: Implement cortical column organization
2. **Distributed Computing**: Scale beyond 100K neurons
3. **Biological Fidelity**: Validate against empirical data

### Integration Testing Checklist

- [ ] Memory systems are accessible via Brain::getWorkingMemory()
- [ ] Sensory input stored in working memory during processing
- [ ] Episodic memory records experiences for later replay
- [ ] Prediction system updates from working memory patterns
- [ ] Planner provides context for action selection
- [ ] Attention system modulates working memory competition
- [ ] Neuromodulators affect memory consolidation
- [ ] Full brain loop tested from perception to action

## Key Integration Files for Developers

### Primary Integration Points

1. **`src/brain/Brain.cpp`**: Main integration hub
   - Lines 634-636: Return nullptr for memory systems
   - Lines 155-160: Neuromodulation integration needed

2. **`src/agent/AgentBrain.cpp`**: Agent-world interface
   - Lines 87-150: `processSensoryInput()` - perfect place for memory integration
   - Lines 238-280: `applyRewardModulation()` - neuromodulation integration
   - Lines 282-313: `updateDevelopment()` - developmental integration

3. **`src/memory/`**: Memory system implementations
   - Implement actual working and episodic memory

4. **`src/prediction/`**: Prediction system
   - Implement usage of predictions for attention and planning

5. **`src/cognition/`**: Cognitive systems
   - Implement actual usage in brain loop and action selection

### Files to Examine First

1. `src/brain/Brain.hpp` - Integration interface definitions
2. `src/agent/AgentBrain.cpp` - Working integration examples  
3. `src/experiments/ExperimentRunner.hpp` - Experiment framework
4. `src/core/Config/Config.hpp` - Configuration system

This documentation serves as a **living technical guide** for developers extending the NLM architecture. Focus on integration of existing components before adding new systems, as this is the core challenge of Phase 6.