# NLM System Integration Guide

## Quick Start: Understanding How Everything Connects

This document provides a high-level overview of how the NLM systems integrate together, designed for developers new to the codebase. For detailed technical architecture, see `docs/ARCHITECTURE.md`.

---

## Core Integration Flow

The NLM system follows a consistent data flow pattern across all phases:

```
[EXTERNAL INPUT] → [WORLD] → [SENSORY PERCEPT] → [BRAIN] → [MOTOR ACTION] → [WORLD]
```

### Phase 1: Skeleton (Basic Framework)
- Neurons that can spike and connect
- Basic plasticity rules (STDP, Hebbian)
- Simple event-driven spike propagation

### Phase 2: Neural Computation
- LIF neuron dynamics with biological accuracy
- Spike propagation with delays
- Structural plasticity (growing/removing connections)
- Age-dependent developmental changes

### Phase 3: World Interaction (Agent System)
- **AgentBrain**: Bridge between brain and world
- **SensoryTransduction**: World percept → neural input
- **MotorDecoding**: Neural activity → actions
- **Neuromodulation**: Reward signals affecting learning
- **Development**: Brain maturation over time

### Phase 4: Emerging Cognition
- **Working Memory**: Temporary pattern storage
- **Episodic Memory**: Experience encoding
- **Prediction System**: Temporal sequence learning
- **Concept Formation**: Pattern discovery without labels
- **Attention**: Competitive processing
- **Planning**: Multi-step action selection
- **Self-Model**: Body awareness
- **Social Learning**: Observation and imitation

### Phase 5: Personality/Sandbox
- Brain-personality development through experience
- Checkpoint system for state persistence
- Preference learning and bias accumulation
- Sandbox for training/teaching

### Phase 6: Final Integration
- All cognitive systems working together
- Comprehensive world simulation
- Reward-based learning
- Reduced hyperactivity
- Selective learning

---

## Key Integration Points

### 1. Sensorimotor Loop (Phase 3/6)
```
World Update → World.getSensoryPercept() 
              ↓
AgentBrain.processSensoryInput()
              ↓
Brain.receiveSensoryInput() → Brain.step()
              ↓
AgentBrain.decodeMotorCommand()
              ↓
World.applyMotorCommand()
```

**Dependencies:**
- Core (Brain, Neuron, Synapse)
- Agent (AgentBrain, SensoryPercept)
- World (SimpleWorld, ActionResult)
- Neuromodulation (for reward learning)

### 2. Memory Integration (Phase 4/6)
```
Working Memory ←→ Brain (persistent activity)
Episodic Memory ←→ Brain (experience encoding)
Concept Formation ←→ Brain (pattern discovery)
Attention ←→ Brain (competitive selection)
Planning ←→ Brain (prediction-based)
```

**Memory System Components:**
- `NeuralWorkingMemory`: Maintains patterns across steps
- `NeuralEpisodicMemory`: Encodes experiences as neural patterns
- `NeuralAssociativeMemory`: Creates associations between patterns
- `ConceptFormation`: Discovers abstract patterns
- `SpatialRepresentation`: Learns spatial relationships
- `TemporalRelation`: Encodes sequence information

### 3. Prediction System Integration (Phase 4/6)
```
PredictionError ←→ Neuromodulation (dopamine-like)
NeuralPrediction ←→ Memory systems
ActionConsequencePredictor ←→ AgentBrain
```

### 4. Development Integration (Phase 2/6)
```
Development System ←→ Plasticity Rules
Maturation ←→ Neural Dynamics
Critical Periods ←→ Learning windows
```

---

## Configuration System Integration

The configuration system connects all components through key settings:

### Required Configuration Parameters

| Component | Key | Description | Default |
|-----------|------|-------------|---------|
| Brain | `neuron_count` | Number of neurons in simulation | 1000 |
| Brain | `region_count` | Number of brain regions | 2 |
| World | `environment_name` | World type | GridWorld |
| World | `environment_width` | World width | 10 |
| World | `environment_height` | World height | 10 |
| Plasticity | `plasticity_learning_rate` | STDP learning rate | 0.01 |
| Neuromodulation | `dopamine_baseline` | Baseline dopamine level | 0.1 |
| Development | `development_synaptogenesis_rate` | New connection rate | 0.001 |

### Configuration Loading
```cpp
// Each component can access its configuration through the shared Config object
Brain brain(config);
AgentBrain agent(brain);
World world;
agent.initialize(world);
```

### Configuration Validation
- Config object validates all keys before use
- Default values ensure simulation stability
- Components can override defaults

---

## Error Handling Integration

### Exception Types
```cpp
// Components use consistent error handling:
enum ErrorType {
    CONFIG_ERROR,       // Configuration loading issues
    MEMORY_ERROR,       // Memory allocation failures
    PLASTICITY_ERROR,   // Learning rule failures
    NEUROMODULATION_ERROR, // Signal processing errors
    SIMULATION_ERROR    // Runtime execution problems
};
```

### Error Recovery
- Components use try-catch blocks for resource management
- Graceful degradation on resource constraints
- Checkpoint system for state recovery

---

## Performance and Scalability

### Memory Pool Integration
```cpp
// All components use memory pools for efficiency
class MemoryPool {
    NeuronPool, SynapsePool, RegionPool
};

// Components cooperate through shared pools
Brain brain(memoryPool);
AgentBrain agent(brain);
World world(memoryPool);
```

### Parallel Processing
- Spike propagation uses event queues
- Components can process independently
- Thread-safe shared state

### Profiling Integration
```cpp
// All major components support profiling:
class Profiler {
    step timing, memory usage, plasticity efficiency
};

// Components integrate via profiler interface
Profiler profiler;
Brain brain(profiler);
AgentBrain agent(brain, profiler);
```

---

## Testing and Validation

### Unit Testing
Each major component has comprehensive tests:
- Core: Neuron, Synapse, Brain
- World: SimpleWorld, ActionResult
- Memory: WorkingMemory, EpisodicMemory
- Plasticity: STDP, Hebbian, Structural

### Integration Testing (Phase 6)
```cpp
// Integration demos verify system cohesion:
// - Pattern discrimination
// - World interaction
// - Memory functionality
// - Prediction accuracy
// - Reward-based learning
```

### Phase 6 Demo Results
The Phase 6 demo shows all systems working together:
- World interaction with meaningful rewards
- Memory formation and retrieval
- Prediction and planning
- Reduced noise/hyperactivity
- Selective learning patterns

---

## Extension Points

### Adding New Components
1. **Create new subdirectory** in src/ (e.g., src/my_new_feature/)
2. **Implement component class** with clear interface
3. **Add to appropriate CMakeLists.txt**
4. **Update documentation**

### Modifying Existing Components
1. **Follow existing patterns** (naming, error handling)
2. **Maintain backward compatibility** where possible
3. **Update unit tests**
4. **Document changes**

### Configuration Extension
1. **Add new configuration keys** to config system
2. **Update default.cfg** with sensible defaults
3. **Add validation** for new parameters
4. **Document in HOW_TO_USE.md**

---

## Debugging and Development

### Common Issues and Solutions

#### "Brain isn't learning"
```cpp
// Check these integration points:
1. Neuromodulation enabled: agent.enableRewardModulation(true)
2. Plasticity enabled: agent.enableStructuralPlasticity(true)
3. Configuration: Check dopamine_baseline and learning rates
4. Memory: Verify working/episodic memory functionality
```

#### "Simulation too slow"
```cpp
// Profile these components:
1. Spike propagation (event queue efficiency)
2. Memory access patterns (pool usage)
3. Plasticity computations (sparse updates)
4. World updates (limit per-step work)
```

#### "Integration errors between components"
```cpp
// Check these interfaces:
1. AgentBrain.connect() – Ensure brain/world compatibility
2. Config keys – Verify all components can access needed parameters
3. Initialization order – Follow agent.initialize(world) sequence
4. Thread safety – Check shared state access patterns
```

---

## Summary

The NLM system integrates through:

1. **Central Configuration**: Shared Config object for all parameters
2. **Agent Interface**: AgentBrain provides unified access point
3. **Memory System**: Neural-based memory storage/retrieval
4. **Prediction System**: Temporal sequence learning
5. **Development System**: Age-dependent changes
6. **Consistent Error Handling**: Standardized exception management
7. **Performance Optimizations**: Memory pools and parallel processing

Each component maintains its specialization while providing clear interfaces for integration with the whole system. The Phase 6 demo shows all systems working together in a coherent, biologically plausible simulation.