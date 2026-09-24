# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Advanced Usage
NLM now includes advanced features for power users and researchers:

#### Command Line Options
```bash
./nlm --help
```

Basic options:
```bash
  --config FILE          Load configuration from file
  --neurons N            Number of neurons (default: 1000)
  --regions R            Number of brain regions (default: 1)
  --steps S              Number of simulation steps (default: 1000)
  --timestep T           Simulation timestep in ms (default: 0.001)
```

Advanced options:
```bash
  --profile              Enable performance profiling
  --debug-level LEVEL    Debug level: none, basic, detailed, verbose, profiling
  --checkpoint NAME      Save checkpoint with given name
  --performance-mode MODE Performance mode: balanced, fast, quality
  --max-steps N          Maximum simulation steps (0 = unlimited)
  --memory-limit BYTES   Memory limit in bytes (0 = unlimited)
  --enable-features F1,F2 Comma-separated features to enable
  --validate-config      Validate configuration before starting
  --enable-simd          Enable SIMD optimizations
  --thread-count N       Number of threads for parallel processing
  --enable-memory-pools  Use memory pools for faster allocation
  --memory-compression L Memory compression level (0-9)
```

#### Example Advanced Commands
```bash
# Run with performance profiling
./nlm --profile --debug-level=verbose --max-steps=5000

# Save checkpoint at specific step
./nlm --checkpoint=experiment1 --steps=1000

# Use memory pools and SIMD for performance
./nlm --enable-memory-pools --enable-simd --thread-count=4

# Validate configuration before starting
./nlm --validate-config --config=my_config.cfg
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## API Features

NLM provides a comprehensive C++ API for neuroscience research:

### Core Brain Interface
```cpp
class Brain {
public:
    // Brain management
    bool initialize();          // Initialize brain with current configuration
    void reset();               // Reset brain state
    void step(SimulationStep step);     // Run simulation step
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    bool validateConfiguration() const;
    
    // Statistics and monitoring
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Advanced features
    void pause();
    void resume();
    bool isRunning() const;
    Timestamp getCurrentTime() const;
    
    // Event handlers for custom behavior
    void registerSpikeHandler(SpikeHandler handler);
    void registerMemoryUpdatedHandler(MemoryUpdatedHandler handler);
    void registerNeuromodulatorUpdatedHandler(NeuromodulatorUpdatedHandler handler);
    
    // Performance and profiling
    void enablePerformanceProfiling();
    std::string getPerformanceStats() const;
    void enableDebugMode();
    void setDebugLevel(DebugLevel level);
};
```

### Memory Systems
```cpp
// Working memory - transient active information
NeuralWorkingMemory* brain->getWorkingMemory();

// Episodic memory - experience storage
NeuralEpisodicMemory* brain->getEpisodicMemory();

// Associative memory - pattern associations
NeuralAssociativeMemory* brain->getAssociativeMemory();
```

### Neuromodulation
```cpp
// Dopamine - reward and reinforcement
Dopamine* brain->getDopamine();

// Curiosity - exploration motivation
Curiosity* brain->getCuriosity();

// Novelty - novelty detection
Novelty* brain->getNovelty();

// Prediction error
PredictionError* brain->getPredictionErrorSignal();
```

### Cognition Systems
```cpp
// Neural planner for action planning
NeuralPlanner* brain->getPlanner();

// Concept formation for pattern discovery
ConceptFormation* brain->getConceptFormation();

// Attentional selection for focus
AttentionalSelection* brain->getAttention();
```

### Development
```cpp
// Developmental system
DevelopmentSystem* brain->getDevelopmentSystem();

// Access developmental stage
DevelopmentalStage brain->getDevelopmentalStage() const;
```

## Python Bindings

For users who prefer Python, NLM provides Python bindings through the `pynlm` package:

```bash
pip install pynlm
```

### Basic Python Usage
```python
import pynlm

# Create a brain with custom configuration
config = {
    'neuron_count': 1000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'connection_probability': 0.1
}

brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
# Save brain state
brain.save('my_brain.chk')
```

## Advanced Features

### Performance Optimization
NLM includes several performance optimization options:

1. **Memory Pools**: Reduce allocation overhead with custom memory pools
2. **SIMD Support**: Utilize SIMD instructions for vectorized operations
3. **Parallel Processing**: Multi-threaded simulation for better performance
4. **Compression**: Memory compression for large simulations

### Custom Extensions
NLM is designed to be extensible:

1. **Plugin System**: Add new brain regions and neural populations
2. **Custom Plasticity Rules**: Implement novel learning rules
3. **Event Callbacks**: Integrate with external systems through event handlers
4. **Visualization Plugins**: Extend with custom visualization tools

### Research Tools
NLM provides tools for neuroscience research:

1. **Checkpoint System**: Save and resume brain states
2. **Performance Profiling**: Monitor and optimize simulation performance
3. **Memory Analysis**: Track memory usage and patterns
4. **Debug Mode**: Detailed logging and debugging capabilities

## Development and Contributing

NLM is an experimental project. Contributions are welcome for:

1. **New Brain Regions**: Implement novel neural architectures
2. **Plasticity Rules**: Explore new learning mechanisms
3. **Integration**: Combine NLM with other neuroscience tools
4. **Documentation**: Improve examples and user guides
5. **Performance**: Optimize existing algorithms and data structures

## License

MIT

## Authors

Research project — See docs for scientific background.

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

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

## License

MIT

## Authors

Research project — See docs for scientific background.
