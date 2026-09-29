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

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Python API Enhancements

**Phase 7: Enhanced User Experience**

NLM now includes significant improvements to the Python bindings, providing advanced tools for both beginners and researchers:

### New Advanced Functions

| Function | Description | Use Case |
|----------|-------------|----------|
| `createResearchBrain()` | Create a research-grade brain with all systems enabled | Research experiments |
| `createQuickTestBrain()` | Create a small brain for rapid testing | Development and debugging |
| `createConfigWithDefaults()` | Create configuration with predefined settings | Different use cases |
| `validateBrainConfig()` | Validate brain configuration | Safety-critical applications |
| `createBrainSafe()` | Create and initialize brain with validation | Production systems |
| `runBrainEpisode()` | Run complete brain episodes with learning | Simulation studies |
| `runSimulationWithStats()` | Run simulation with detailed statistics | Performance analysis |
| `createCompleteSystem()` | Create complete brain-world-agent system | Complex experiments |
| `optimizeBrainForPerformance()` | Optimize brain for specific characteristics | Performance tuning |

### Configuration Presets

The enhanced API provides several predefined configuration presets:

- **`fast_simulation`**: Optimized for speed and throughput
- **`learning_focus`**: Optimized for plasticity and learning
- **`memory_focus`**: Optimized for memory systems
- **`curiosity_driven`**: Optimized for exploration and discovery

### Error Handling Improvements

Advanced error handling and validation:

```python
# Validate configuration before use
pynlm.validateBrainConfig(config)

# Create brain safely with validation
brain = pynlm.createBrainSafe(config)

# Handle errors gracefully
except Exception as e:
    print(f"Error: {e}")
    # Recovery and retry
```

### Enhanced Examples

The documentation now includes:

1. **Basic examples** for getting started
2. **Advanced research examples** for complex experiments
3. **Performance testing examples** for optimization
4. **Error handling examples** for robust applications
5. **Configuration management examples** for different use cases
6. **Research system setup examples** for comprehensive experiments

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

### Python Usage

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)
    
print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/                    # Python bindings
│   └── bindings.cpp          # Enhanced Python API
├── src/                      # C++ source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components
│   ├── memory/               # Memory systems
│   ├── neuromodulation/      # Neuromodulators
│   ├── prediction/           # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization
├── tests/                    # Unit tests
├── docs/                     # Documentation
└── configs/                  # Configuration files
```

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

## License

MIT

## Authors

Research project — See docs for scientific background.