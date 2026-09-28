# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system (ENHANCED)
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator (ENHANCED)

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer (ENHANCED)
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer (ENHANCED)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement (ENHANCED)
- **Novelty**: Novelty detection (ENHANCED)
- **Curiosity**: Exploration motivation (ENHANCED)
- **PredictionError**: Error signals (ENHANCED)

### 7. Memory Layer (Phase 4) (ENHANCED)
- **NeuralWorkingMemory**: Persistent activity for temporary storage (ENHANCED)
- **NeuralEpisodicMemory**: Experience encoding in neural patterns (ENHANCED)
- **NeuralAssociativeMemory**: Experience-based associations (ENHANCED)
- **ConceptFormation**: Pattern discovery without labels (ENHANCED)
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4) (ENHANCED)
- **NeuralPrediction**: Temporal sequence learning (ENHANCED)
- **ActionConsequencePredictor**: Action effect prediction (ENHANCED)
- **PredictionErrorSignal**: Error computation for learning (ENHANCED)

### 9. Cognitive Layer (Phase 4) (ENHANCED)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV) (ENHANCED)
- **NeuralPlanner**: Predictive action selection (ENHANCED)
- **SelfModel**: Sensorimotor self-awareness (ENHANCED)
- **SocialLearning**: Observation and imitation (ENHANCED)

### 10. Sensory Layer (ENHANCED)
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals (ENHANCED)

### 11. Motor Layer (ENHANCED)
- **Action**: Action representation (ENHANCED)
- **MotorSystem**: Neural-to-action conversion (ENHANCED)

### 12. Environment Layer (ENHANCED)
- **Environment**: Abstract environment interface (ENHANCED)
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer (ENHANCED)
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments
- **Phase6Experiment**: Phase 6 integration experiments (ENHANCED)

## Advanced Configuration System (NEW SECTION)

The configuration system has been significantly enhanced with:

```cpp
// Advanced configuration options
config->set("brain.neuron_count", 100000);           // 100k neurons
config->set("brain.synapse_density", 0.15f);       // Higher density
config->set("plasticity.stdp.learning_rate", 0.0005f); // Fine-tuned STDP
config->set("neuromod.curiosity.enable", true);    // Enable curiosity
config->set("neuromod.novelty.enable", true);      // Enable novelty detection
config->set("development.initial_plasticity", 1.5f); // High initial plasticity
config->set("simulation.dt", 0.001f);              // 1ms time step
config->set("simulation.max_steps", 50000);        // 50k steps
```

### Configuration Templates

```cpp
// Apply predefined templates
config.applyTemplate("default");       // Standard configuration
config.applyTemplate("high_performance"); // Optimized for performance
config.applyTemplate("debug");         // Optimized for development
```

### Configuration Validation

```cpp
// Validate configuration with comprehensive checks
if (config.validate()) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    auto errors = config.getValidationErrors();
    auto warnings = config.getValidationWarnings();
    // Handle validation errors
}
```

### Configuration Export/Import

```cpp
// Export to YAML
std::string yaml = config.toYAML();
std::ofstream file("config.yaml");
file << yaml;

// Import from YAML
config.loadFromYAML(yamlContent);
```

### Configuration Management

```cpp
// Merge with another config
config.merge(otherConfig);

// Check if configurations are identical
if (config.equals(otherConfig)) {
    std::cout << "Configurations are identical" << std::endl;
}

// Get configuration differences
auto differences = config.diff(otherConfig);
for (const auto& diff : differences) {
    std::cout << "Key: " << diff.first << std::endl;
    std::cout << "  Old: " << diff.second.first << std::endl;
    std::cout << "  New: " << diff.second.second << std::endl;
}
```

## Advanced Brain API (NEW SECTION)

The Brain class now provides advanced operations:

```cpp
// Advanced brain operations
brain->step(step, time);                    // With timestamp
brain->receiveSensoryInput(input);          // Inject sensory data
brain->injectCurrent(neuron, current);      // Targeted current injection
brain->produceAction();                     // Get motor output
brain->applyNeuromodulation(signal);        // Apply neuromodulators
brain->develop();                           // Update development
brain->updatePlasticity();                   // Update plastic synapses

// Advanced statistics
brain->getTotalSpikeCount();                 // Total spikes
brain->getAverageFiringRate();               // Average firing rate
brain->getExcitationInhibitionRatio();       // E/I balance
brain->getWorkingMemory();                    // Access working memory
brain->getEpisodicMemory();                   // Access episodic memory
brain->getDopamine();                        // Access dopamine system
brain->getCuriosity();                       // Access curiosity system

// Brain state management
brain->save("/path/to/checkpoint.bin");        // Save brain state
brain->load("/path/to/checkpoint.bin");        // Load brain state
brain->reset();                              // Reset brain
```

## Advanced Neuromodulation System (NEW SECTION)

The neuromodulation system has been enhanced with:

```cpp
// Advanced neuromodulation
brain->getDopamine()->setScale(1.0f);        // Adjust dopamine effect
brain->getCuriosity()->setThreshold(0.5f);   // Set curiosity threshold
brain->getNovelty()->setDecayRate(0.1f);      // Set novelty decay
brain->getPredictionError()->setLearningRate(0.01f); // Set prediction error learning
```

## Advanced Memory Systems (NEW SECTION)

```cpp
// Advanced memory operations
auto* workingMem = brain->getWorkingMemory();
if (workingMem) {
    workingMem->addTrace(neuralPattern, strength);  // Add new trace
    auto traces = workingMem->getActiveTraces();     // Get active traces
    workingMem->decayTraces(0.01f);                  // Decay traces
}

auto* episodicMem = brain->getEpisodicMemory();
if (episodicMem) {
    episodicMem->storeExperience(event, timestamp);   // Store experience
    auto episodes = episodicMem->getRecentEpisodes(10); // Get recent episodes
    episodicMem->replayEpisodes(5);                  // Replay episodes
}
```

## Advanced Prediction System (NEW SECTION)

```cpp
// Advanced prediction operations
auto* predictionSystem = brain->getPredictionSystem();
if (predictionSystem) {
    predictionSystem->updatePrediction(input);         // Update prediction
    float error = predictionSystem->getPredictionError(); // Get prediction error
    predictionSystem->setLearningRate(0.001f);         // Adjust learning rate
}
```

## Advanced Development System (NEW SECTION)

```cpp
// Advanced development operations
auto* developmentSystem = brain->getDevelopmentSystem();
if (developmentSystem) {
    developmentSystem->updateStage();                  // Update developmental stage
    developmentSystem->setPlasticityRate(1.0f);         // Set plasticity rate
    developmentSystem->setCriticalPeriodEnd(1000);     // Set critical period end
}
```

## Enhanced AgentBrain API (NEW SECTION)

```cpp
// Enhanced AgentBrain operations
agent->processSensoryInput(percept);          // Process world input
agent->decodeMotorCommand();                  // Get motor command
agent->applyRewardModulation(reward, pred);   // Apply learning signals
agent->updateDevelopment(timestep);           // Update development
agent->enableLearning(true);                  // Enable learning subsystems
agent->enableDevelopment(true);               // Enable development
agent->enableCuriosity(true);                 // Enable curiosity

// Enhanced AgentBrain statistics
agent->getNeuromodulationLevel();             // Current neuromodulation
agent->getCuriosityLevel();                   // Curiosity level
agent->getNoveltyLevel();                     // Novelty detection
agent->getPredictionError();                  // Prediction error
agent->getDevelopmentalStage();              // Current developmental stage
```

## Performance Optimization Features (NEW SECTION)

### 1. Memory Pooling
```cpp
// Memory-efficient brain initialization
MemoryPool pool;
brain->initialize(&pool);
```

### 2. Event Queue Optimization
```cpp
// Efficient spike processing
EventQueue queue;
brain->setSpikeProcessor(&queue);
```

### 3. SIMD Vectorization
```cpp
// SIMD-optimized computations
brain->enableSIMD(true);
```

### 4. Parallel Processing
```cpp
// Parallel brain regions
brain->enableParallelProcessing(true);
brain->setParallelRegions({"cortex", "hippocampus", "amygdala"});
```

## Key Design Principles (ENHANCED)

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience
9. **Advanced Configuration**: Comprehensive configuration options for power users
10. **Error Handling**: Robust error handling and recovery mechanisms
11. **Production Features**: Monitoring, logging, and checkpointing for production use
12. **Performance Optimization**: Memory pooling, SIMD vectorization, and parallel processing

## Phase 4 Key Innovations (ENHANCED)

### Prediction System (ENHANCED)
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability
- Error signals modulate learning with enhanced neuromodulation

### Working Memory (ENHANCED)
- Persistent neural activity with enhanced maintenance
- Competition for selective retention with neural dynamics
- Decay mechanisms for memory aging
- Neural-grounded (not a vector store)

### Episodic Memory (ENHANCED)
- Experience encoded as neural patterns with consolidation
- Pattern completion retrieval with enhanced mechanisms
- Temporal and spatial indexing with rich indexing
- Replay during consolidation with enhanced replay dynamics

### Concept Formation (ENHANCED)
- Discovers patterns without labels
- Prototype formation through averaging with stability measurement
- Automatic stability estimation
- Category hints from properties with enhanced clustering

### Attention (ENHANCED)
- Competitive dynamics with enhanced lateral inhibition
- Winner-take-more dynamics with recurrent amplification
- Top-down and bottom-up bias with flexible control
- NOT Transformer QKV attention (neural circuit mechanism)

### Planning (ENHANCED)
- Uses learned predictions with enhanced prediction systems
- Evaluates potential futures with Monte Carlo methods
- Selects actions leading to goals with action evaluation
- Adapts based on outcomes with continuous learning

### Self-Model (ENHANCED)
- Learns body schema with sensorimotor integration
- Predicts action consequences with forward models
- Self vs external distinction with agency detection
- Capability representation with dynamic updating

### Social Learning (ENHANCED)
- Observes other agents with enhanced observation mechanisms
- Learns action effects with outcome prediction
- Imitation capability with action mirroring
- Simple communication signals with symbolic representation

## Advanced Configuration Presets (NEW SECTION)

### Default Configuration
```yaml
brain:
  neuron_count: 1000
  connection_probability: 0.05
  synapse_density: 0.1
  v_thresh: -50.0
  v_rest: -70.0
plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
  structural:
    enable: true
neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true
development:
  initial_plasticity: 1.0
  critical_period: 1000
```

### High-Performance Configuration
```yaml
brain:
  neuron_count: 50000
  connection_probability: 0.06
  synapse_density: 0.12
plasticity:
  stdp:
    enable: true
    learning_rate: 0.0008
  structural:
    enable: true
neuromodulation:
  curiosity:
    enable: true
  novelty:
    enable: true
development:
  initial_plasticity: 1.5
  critical_period: 2000
memory:
  working_memory:
    capacity: 5000
  episodic_memory:
    capacity: 500
```

### Debug Configuration
```yaml
brain:
  neuron_count: 100
  connection_probability: 0.03
  synapse_density: 0.05
plasticity:
  stdp:
    enable: true
    learning_rate: 0.01
neuromodulation:
  curiosity:
    enable: true
simulation:
  dt: 0.001
  max_steps: 500
logging:
  level: debug
```

## What NLM is NOT

NLM is NOT:
- A claim that we can simulate a human brain
- A model that accurately reproduces neural physiology
- A replacement for neuroscience research
- An intelligent system
- A transformer or LLM
- A pretrained AI system

NLM IS:
- A computational exploration of brain-inspired architecture
- A testbed for developmental learning hypotheses
- A research platform for artificial neural systems
- An educational tool for understanding neural computation
- An investigation of whether cognition can emerge from spiking dynamics
- A platform for advanced AI research with comprehensive configuration and advanced features

## Acknowledging Uncertainty

We acknowledge that:
1. Biological brains are far more complex than any current simulation
2. Our understanding of the brain is incomplete
3. Many approximations are necessary for computational tractability
4. Claims of "brain-like" behavior should be critically examined
5. Phase 4 does not claim human-like intelligence, consciousness, or genuine understanding
6. Advanced features require careful configuration and validation
7. Performance optimization requires understanding of computational constraints

## Conclusion

The NLM architecture has been significantly enhanced with:

1. **Advanced Configuration System**: Comprehensive configuration options, validation, and templates
2. **Enhanced Brain API**: Rich set of operations for brain control and analysis
3. **Enhanced Neuromodulation**: Advanced neuromodulation systems with fine-grained control
4. **Enhanced Memory Systems**: Sophisticated memory systems with consolidation and replay
5. **Enhanced Prediction System**: Advanced prediction with temporal sequence learning
6. **Enhanced Development System**: Enhanced developmental processes with stage progression
7. **Enhanced Cognitive Systems**: Advanced cognition with attention, planning, and social learning
8. **Performance Optimization**: Memory pooling, SIMD vectorization, and parallel processing
9. **Error Handling**: Robust error handling and recovery mechanisms
10. **Production Features**: Monitoring, logging, and checkpointing for production use

These enhancements make NLM a powerful platform for advanced AI research, providing researchers with comprehensive tools for exploring brain-inspired architectures, developing novel learning algorithms, and investigating the emergence of cognition from spiking neural dynamics.