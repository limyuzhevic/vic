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

#### Performance Optimizations (Phase 5)
- **OptimizedBrain**: High-performance neural simulation brain with event-driven processing
- **Structure-of-Arrays (SoA)**: Efficient memory layout for neural computations
- **Sparse Connectivity**: Memory-efficient neural connectivity representation
- **Multithreading**: Parallel processing for large-scale neural populations
- **SIMD Vectorization**: Hardware-accelerated neural computations
- **Event Queues**: Lock-free ring buffers with time-bucketed delayed spikes
- **Memory Pools**: Pre-allocated neuron/synapse pools for reduced fragmentation

#### Advanced Features (Phase 5 Extended)
- **AdvancedOptimizedBrain**: Enhanced brain with profiling, diagnostics, and power-user controls
- **Performance Monitoring**: Real-time performance statistics and profiling
- **Adaptive Algorithms**: Dynamic timestep adjustment, homeostatic plasticity, neural pruning
- **Configuration Export/Import**: Save/load brain configurations
- **Debug Mode**: Detailed logging for debugging and analysis
- **Advanced Analytics**: Neural dynamics analysis, plasticity effects analysis

### Phase 5 Performance Infrastructure

#### Event-Driven Processing
- Only active neurons consume computation
- Efficient spike event queues
- Time-bucketed delayed spikes
- Significant speedups for sparse networks

#### Structure-of-Arrays (SoA)
- Contiguous neuron state arrays
- Better cache utilization
- Improved SIMD vectorization
- Reduced memory for large networks

#### Sparse Connectivity
- Adjacency lists instead of dense matrices
- Efficient synapse lookup
- Reduced memory for large networks
- Cache-friendly iteration patterns

#### Multithreading
- Parallel population updates
- Work-stealing load balancing
- Thread-local state accumulation
- Synchronized spike aggregation

#### SIMD Vectorization
- SSE (4 floats), AVX2 (8 floats), AVX-512 (16 floats)
- Vectorized LIF updates
- Vectorized synaptic integration
- Vectorized plasticity calculations

#### Advanced Features (Phase 5 Extended)
- **AdvancedOptimizedBrain**: Enhanced brain with profiling, diagnostics, and power-user controls
- **Performance Monitoring**: Real-time performance statistics and profiling
- **Adaptive Algorithms**: Dynamic timestep adjustment, homeostatic plasticity, neural pruning
- **Configuration Export/Import**: Save/load brain configurations
- **Debug Mode**: Detailed logging for debugging and analysis
- **Advanced Analytics**: Neural dynamics analysis, plasticity effects analysis

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON
make -j4
```

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

### Phase Summary

#### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

#### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

#### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

#### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

#### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system
- **OptimizedBrain**: High-performance neural simulation brain
- **AdvancedOptimizedBrain**: Enhanced features with profiling and diagnostics

#### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## AdvancedOptimizedBrain Features

### Configuration Options
The AdvancedOptimizedBrain provides extensive configuration options for power users:

#### Performance Configuration
- `advanced_max_threads`: Number of threads for parallel processing
- `advanced_enable_simd`: Enable/disable SIMD vectorization
- `advanced_enable_multithreading`: Enable/disable multithreading
- `advanced_enable_event_driven`: Enable/disable event-driven processing
- `advanced_enable_sparse_connectivity`: Enable/disable sparse connectivity

#### Neural Model Configuration
- `advanced_neuron_temperature`: Temperature parameter for stochastic neuron behavior
- `advanced_weight_quantization`: Weight quantization precision
- `advanced_input_scaling`: Synaptic input scaling factor

#### Plasticity Configuration
- `advanced_stdp_ltp_weight`: STDP LTP weight
- `advanced_stdp_ltd_weight`: STDP LTD weight
- `advanced_stdp_tau`: STDP time constant
- `advanced_hebbian_rate`: Hebbian learning rate

#### Memory Configuration
- `advanced_neuron_cache_size`: Cache size for neuron access
- `advanced_synapse_cache_size`: Cache size for synapse access
- `advanced_enable_numa_optimization`: Enable NUMA optimization

#### Monitoring Configuration
- `advanced_enable_profiling`: Enable performance profiling
- `advanced_profiling_sample_rate`: Profiling sample rate
- `advanced_output_directory`: Output directory for analysis files

### Power-User Commands

The AdvancedOptimizedBrain provides several power-user commands for advanced usage:

#### Debugging and Analysis
- `enableDebugMode(bool enable)`: Enable/disable detailed debug logging
- `setProfilingLevel(size_t level)`: Set profiling verbosity level

#### Configuration Management
- `exportConfiguration(const std::string& filepath)`: Export current configuration to file
- `importConfiguration(const std::string& filepath)`: Import configuration from file

#### Advanced Simulation Controls
- `adaptiveTimestepControl(double targetDt)`: Enable adaptive timestep control
- `enableHomeostaticPlasticity(bool enable, float targetRate)`: Enable/disable homeostatic plasticity
- `enableNeuralPruning(bool enable, float threshold)`: Enable/disable neural pruning

### Performance Monitoring

The AdvancedOptimizedBrain includes comprehensive performance monitoring:

#### Real-Time Statistics
- Neurons per second
- Spikes per second
- Synaptic events per second
- Real-time factor
- Active and firing neuron counts
- Pending spike and delayed spike counts
- Average firing rate
- Memory usage in MB
- Step execution time

#### Performance Reports
The brain generates detailed performance reports including:
- Time performance statistics
- Spike frequency analysis
- Memory usage statistics
- Activity profiles
- Neural dynamics analysis
- Plasticity effects analysis

### Biological Fidelity Improvements

Phase 5 includes several biological fidelity improvements:

#### Adaptive Neuron Models
- Temperature-based stochastic behavior
- Weight quantization for realistic neural dynamics
- Configurable refractory periods

#### Short-Term Plasticity
- Facilitation and depression mechanisms
- Time-constant-based dynamics
- Synaptic efficacy modulation

#### Configurable Inhibitory Circuits
- Balance excitation and inhibition
- Tunable inhibitory connection strength
- Network stability control

#### Homeostatic Plasticity
- Activity-dependent regulation
- Target firing rate maintenance
- Network stability preservation

#### Neural Pruning
- Eliminate weak connections
- Maintain network efficiency
- Adapt to changing environments

## Running Phase 6 Integration Test

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Running Phase 5 Advanced Features

```bash
# Create build directory with advanced features enabled
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON
make -j4

# Run advanced brain demo
./nlm_phase5_advanced_demo
```

The Phase 5 advanced demo demonstrates all performance optimizations and advanced features.

## Configuration Files

See `configs/` directory for example configuration files:

- `configs/default_advanced.cfg`: Advanced configuration with all optimizations enabled
- `configs/performance_tuning.cfg`: Configuration for performance tuning
- `configs/debug.cfg`: Configuration for debugging and analysis

## License

MIT

## Authors

Research project — See docs for scientific background.

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
