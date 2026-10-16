# NLM Architecture Documentation

## Overview

The NLM (熙然) project implements an experimental artificial developmental brain system that simulates brain-like computation through spiking neural networks, developmental processes, and integrated cognitive systems. Phase 6 represents the complete integration of all brain systems into a unified computational architecture.

## System Architecture

### Core Components

The NLM brain operates as a closed-loop system with the following key components:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Brain Architecture

The brain consists of several interconnected systems:

#### 1. Neural Core
- **Neuron**: Basic spiking neuron model with LIF dynamics (`src/brain/Neuron.hpp`)
- **Brain**: Central coordinator with spiking neural computation (`src/brain/Brain.hpp`)
- **NeuralPopulation**: Groups of neurons with shared properties (`src/brain/NeuralPopulation.hpp`)
- **NeuralRegion**: Functional brain regions with connectivity (`src/brain/NeuralRegion.hpp`)
- **Synapse**: Event-driven spike transmission (`src/brain/Synapse.hpp`)

#### 2. Dynamics and Plasticity
- **NeuralDynamics**: Spike propagation and neuron dynamics (`src/dynamics/NeuralDynamics.hpp`)
- **SpikeSystem**: Event-driven spike delivery (`src/dynamics/SpikeSystem.hpp`)
- **STDP**: Spike-timing-dependent plasticity (`src/plasticity/STDP.hpp`)
- **Hebbian**: Hebbian synaptic strengthening (`src/plasticity/Hebbian.hpp`)
- **StructuralPlasticity**: Structural synaptic rewiring (`src/plasticity/StructuralPlasticity.hpp`)

#### 3. Cognitive Systems
- **WorkingMemory**: Transient active information storage (`src/memory/WorkingMemory.hpp`)
- **EpisodicMemory**: Experience encoding and replay (`src/memory/EpisodicMemory.hpp`)
- **AssociativeMemory**: Pattern association through Hebbian learning (`src/memory/AssociativeMemory.hpp`)
- **PredictionSystem**: Sensory prediction and error computation (`src/prediction/PredictionSystem.hpp`)
- **NeuralPlanner**: Action sequence evaluation and planning (`src/cognition/NeuralPlanner.hpp`)
- **ConceptFormation**: Pattern discovery and categorization (`src/cognition/ConceptFormation.hpp`)
- **AttentionalSelection**: Competitive focus selection (`src/cognition/AttentionalSelection.hpp`)

#### 4. Neuromodulation Systems
- **Dopamine**: Reward prediction and reinforcement signaling (`src/neuromodulation/Dopamine.hpp`)
- **Curiosity**: Exploration motivation and novelty detection (`src/neuromodulation/Curiosity.hpp`)
- **Novelty**: Sensory novelty detection for exploration (`src/neuromodulation/Novelty.hpp`)
- **PredictionError**: Prediction error signaling for learning (`src/neuromodulation/PredictionError.hpp`)

#### 5. Developmental System
- **DevelopmentSystem**: Age-dependent plasticity modulation (`src/development/DevelopmentSystem.hpp`)
- **Maturation**: Neuronal maturation processes (`src/development/Maturation.hpp`)
- **Pruning**: Synaptic pruning and refinement (`src/development/Pruning.hpp`)
- **Synaptogenesis**: New synapse formation (`src/development/Synaptogenesis.hpp`)

#### 6. Agent Interface
- **AgentBrain**: Sensory transduction and motor decoding (`src/agent/AgentBrain.hpp`)
- **AgentBody**: Interface to the world simulation (`src/agent/AgentBody.hpp`)
- **SensoryPercept**: Processed sensory input representation (`src/agent/SensoryPercept.hpp`)

#### 7. Supporting Systems
- **Config**: Configuration management (`src/core/Config/Config.hpp`)
- **Logger**: Logging and debugging (`src/core/Logger/Logger.hpp`)
- **RandomGenerator**: Random number generation (`src/core/Random/Random.hpp`)
- **SimulationClock**: Time management (`src/core/SimulationClock/SimulationClock.hpp`)
- **Environment**: World interface (`src/environment/Environment.hpp`)
- **Observation**: Perceptual state representation (`src/environment/Observation.hpp`)

## Design Principles

### Biological Inspiration
1. **Spiking Neurons**: Leaky Integrate-and-Fire (LIF) neuron model
2. **Event-Driven Computation**: Spike-based communication
3. **Developmental Processes**: Age-dependent plasticity and maturation
4. **Neuromodulation**: Multiple neuromodulators coordinating behavior
5. **Memory Integration**: Multiple memory systems integrated with processing

### Computational Design
1. **Closed-Loop Architecture**: Continuous interaction with environment
2. **Hierarchical Organization**: Multiple levels of abstraction
3. **Emergent Behavior**: Complex behavior from simple components
4. **Plastic Adaptation**: Continuous structural and functional change
5. **Predictive Processing**: Prediction-based learning and inference

## Key Features

### Developmental Stages
- **Juvenile**: High plasticity, rapid learning (plasticity multiplier = 1.0)
- **Adult**: Optimized performance, stable behavior (plasticity multiplier = 0.5)
- **Aging**: Declining plasticity, behavioral changes (plasticity multiplier = 0.2)

### Memory Systems
- **Working Memory**: Rapid, transient storage (capacity configurable via `memory.workingMemory.capacity`)
- **Episodic Memory**: Long-term experience storage (max episodes configurable via `memory.episodicMemory.maxEpisodes`)
- **Associative Memory**: Pattern completion and recall (similarity threshold configurable via `memory.associativeMemory.similarityThreshold`)

### Learning Mechanisms
- **STDP**: Spike-timing-dependent synaptic plasticity (time window configurable via `plasticity.stdp.timeWindow`)
- **Hebbian Learning**: Co-activation-based strengthening (learning rate configurable via `plasticity.hebbian.learningRate`)
- **Prediction Error Learning**: Error-driven optimization (integrated with neuromodulation)
- **Curiosity-Driven Exploration**: Novelty-seeking behavior (driven by novelty detection)

## Performance Characteristics

### Computational Complexity
- **Time Complexity**: O(N²) for synaptic connections (optimized with sparsity via `brain.connectionProbability`)
- **Memory Usage**: O(N) for neural populations with compression
- **Scalability**: Parallel processing for large networks (SMP and MPI support)

### Real-Time Performance
- **Time Step**: Configurable simulation granularity (1μs to 1s steps)
- **Event-Driven**: Efficient spike propagation via `SpikeSystem`
- **Checkpointing**: State persistence for long simulations (interval configurable via `simulation.checkpointInterval`)

### Memory Efficiency
- **Memory Pools**: High-frequency allocations via `MemoryPool` templates
- **Compression**: Episodic memory compression for long-term storage
- **Sparsity**: Connection sparsity reduces memory requirements

## Integration Points

### Memory Systems Integration
All memory systems are integrated with neural processing through:
- Working memory influencing attentional selection (via `getWorkingMemory()`)
- Episodic memory providing context for prediction (via `getEpisodicMemory()`)
- Associative memory enabling pattern completion (via `getAssociativeMemory()`)

### Neuromodulation Integration
Neuromodulators affect multiple systems:
- Dopamine influences plasticity and motivation (via `getDopamine()`)
- Curiosity drives exploration and novel behavior (via `getCuriosity()`)
- Novelty detection triggers learning mechanisms (via `getNovelty()`)

### Development Integration
Developmental processes modulate:
- Plasticity rates across systems (via `getDevelopmentSystem()->getPlasticityMultiplier()`)
- Neural excitability and connectivity (via developmental stage changes)
- Learning capacity and stability (age-dependent plasticity decay)

## Technical Specifications

### Neuron Model
- **Type**: Leaky Integrate-and-Fire (LIF)
- **Membrane Potential**: Continuous variable with threshold (typically -55mV to +30mV)
- **Refractory Period**: Fixed duration after spike (typically 2-5ms)
- **Synaptic Input**: Event-driven current injection (spike-triggered)

### Spike Propagation
- **Mechanism**: Event-driven synaptic transmission via `SpikeSystem`
- **Delays**: Configurable transmission delays (typically 1-10ms)
- **Strength**: Synaptic weight modulation (typically 0.1-1.0)
- **Plasticity**: Activity-dependent modification (STDP, Hebbian)

### Developmental Dynamics
- **Maturation Rate**: Age-dependent decrease in plasticity (plasticity decay rate configurable)
- **Structural Plasticity**: Synaptic rewiring based on activity (formation and elimination rates)
- **Excitability**: Developmental changes in neural responsiveness (age-dependent threshold changes)

## Configuration Integration

### Core Brain Configuration
```json
{
    "brain": {
        "maxNeurons": 1000,
        "learningRate": 0.01,
        "plasticityEnabled": true,
        "memory": {
            "workingMemory": {
                "enabled": true,
                "capacity": 500,
                "decayRate": 0.01
            },
            "episodicMemory": {
                "enabled": true,
                "maxEpisodes": 1000,
                "replayWindow": 1000
            },
            "associativeMemory": {
                "enabled": true,
                "similarityThreshold": 0.8
            }
        },
        "neuromodulation": {
            "dopamine": {
                "enabled": true,
                "baselineLevel": 0.1
            },
            "curiosity": {
                "enabled": true,
                "baselineLevel": 0.1
            }
        },
        "development": {
            "enabled": true,
            "plasticityDecayRate": 0.02
        }
    }
}
```

## Performance Optimization

### Memory Optimization
- **Memory Pools**: Reduce allocation overhead for high-frequency operations
- **Connection Sparsity**: Limit connections to reduce memory usage
- **Episodic Compression**: Compress stored experiences for long-term storage

### Computational Optimization
- **Event-Driven**: Only process spikes when they occur
- **Vectorization**: Use SIMD for parallel processing of neurons
- **Cache Locality**: Organize data structures for cache efficiency

## References

For detailed scientific background, see:
- **SCIENCE.md**: Scientific methodology and principles
- **BIOLOGICAL_FIDELITY.md**: Biological realism assessment
- **PHASE6_ARCHITECTURE.md**: Phase 6 specific architectural details
- **EXPERIMENTS.md**: Experimental validation and results

## Conclusion

The NLM architecture represents a computationally efficient, biologically-inspired system for studying developmental learning and brain-like computation. The integration of multiple cognitive systems, neuromodulation, and developmental processes enables the emergence of complex adaptive behavior from simple neural components. Phase 6 achieves complete system integration, allowing all components to work together as a unified artificial brain capable of learning, memory, and adaptive behavior.