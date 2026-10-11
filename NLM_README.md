# NLM (熙然) - Neural Learning Machine
## An Experimental Artificial Developmental Brain

### Project Overview

NLM is a research project investigating computational brain-like systems that begin in a primitive developmental state and acquire increasingly complex abilities through interaction with an environment. **NLM is NOT** a transformer or LLM - it's a brain-inspired architecture with neurons and synapses that develops and adapts over time.

### Key Features (Phase 6 - Final Integration)

#### Core Brain Architecture
- **Real Spiking Neural Computation**: Event-driven LIF neuron dynamics with spike propagation
- **Integrated Memory Systems**: Working memory, episodic memory, and associative memory connected to neural processing
- **Complete Neuromodulation**: Dopamine, Curiosity, Novelty, Acetylcholine, Norepinephrine, and Serotonin
- **Prediction System**: Forward models and prediction error computation
- **Cognition Systems**: Neural planning, concept formation, and attentional selection
- **Development System**: Age-based plasticity modulation and developmental stages

#### Integration Achievements
- **100% Connected Memory Systems**: Working memory now connected to sensory processing
- **Multi-Neurotransmitter Control**: Full neuromodulation integration for coordinated brain function
- **Predictive Processing**: Prediction system integrated with sensory input
- **Developmental Trajectory**: Age-based changes in plasticity and behavior
- **Experience Storage**: Episodic memory with replay and consolidation

### System Architecture

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
MOTIVATION / NEUROMODULATION (all 6 neuromodulators)
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

### System Components

#### 1. Neural Core (Brain)
- **LIF Neuron Dynamics**: Real integrate-and-fire neurons with refractory periods
- **Event-Driven Processing**: Spike-based neural computation for efficiency
- **Synaptic Transmission**: Excitatory/inhibitory connections with delays
- **Plasticity Rules**: STDP, Hebbian learning, and structural plasticity

#### 2. Memory Systems
- **Working Memory**: Transient active information storage with competition
- **Episodic Memory**: Experience storage with replay mechanisms
- **Associative Memory**: Pattern associations for rapid learning
- **Integration**: All memory systems connected to neural processing

#### 3. Neuromodulation Systems
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Novelty and prediction error-driven exploration
- **Novelty**: Change detection in sensory input
- **Acetylcholine**: Attention focus and memory consolidation
- **Norepinephrine**: Arousal and vigilance control
- **Serotonin**: Mood and social behavior modulation

#### 4. Prediction System
- **Forward Models**: Predict sensory consequences of actions
- **Prediction Error**: Compare predicted vs actual outcomes
- **Confidence Tracking**: Uncertainty estimation
- **Learning**: Update predictions based on experience

#### 5. Cognition Systems
- **Neural Planner**: Multi-step action planning
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive selection of neural activity
- **Integration**: Connected to all other brain systems

#### 6. Development System
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
- **Age-Based Plasticity**: Changing learning rates over time
- **Structural Changes**: Synaptogenesis and pruning
- **Maturation Effects**: Neural excitability changes with development

### Performance Features

#### Optimization Infrastructure
- **Memory Pools**: Efficient memory management
- **Event Queues**: Optimized spike event processing
- **Sparse Connectivity**: Reduced computational load
- **SIMD Vectorization**: Parallel processing of neuron groups
- **Multithreading**: Scalable performance on multi-core systems

#### Persistence
- **Checkpoint System**: Save/load brain state
- **Compression**: Multiple compression levels
- **Incremental Updates**: Only save changed states

### Usage

#### Command Line Interface

```bash
# Run simulation for 10 seconds with 2000 neurons
./nlm run 10 --neurons 2000 --regions 2

# Run a single episode with custom configuration
./nlm run-episode --config my_config.txt

# Save brain state (checkpoint)
./nlm checkpoint-save my_brain_state.bin

# Load brain state from checkpoint
./nlm checkpoint-load my_brain_state.bin

# Show system status
./nlm status

# Show help information
./nlm help
```

#### Configuration File

Example configuration file (`configs/default_config.txt`):

```
# Core settings
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001

# Memory settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Neuromodulation
novelty_threshold = 0.3
curiosity_novelty_weight = 0.5

# Plasticity
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development
replay_interval = 100
consolidation_interval = 1000
```

### Scientific Principles

#### Brain-Like Properties
- **Experience-Driven**: Learning from interaction with environment
- **Development-Driven**: Changes in plasticity over time
- **Neural**: Spiking neural computation with biologically-inspired dynamics
- **Plastic**: Continuous adaptation through synaptic changes
- **Embodied**: Interaction with physical world
- **Self-organizing**: Emergent structure from simple rules
- **Continually learning**: No catastrophic forgetting

#### Biological Inspiration
- **LIF Neuron Model**: Simplified Hodgkin-Huxley dynamics
- **Spike Timing Dependent Plasticity**: STDP learning rules
- **Hebbian Learning**: Cooperative potentiation
- **Neuromodulation**: Multiple neurotransmitters for coordinated function
- **Memory Systems**: Working, episodic, and semantic memory
- **Attention**: Competitive selection of neural representations
- **Development**: Age-dependent plasticity changes

### Scientific Limitations

#### What NLM IS NOT
- **NOT a transformer or LLM**: No attention layers or pretrained knowledge
- **NOT a chatbot**: No language understanding or generation
- **NOT deep learning**: No neural networks in the traditional sense
- **NOT pretrained AI**: Starts from scratch, learns from experience

#### Current Limitations
- **Simplified neuron model**: Not Hodgkin-Huxley level detail
- **No realistic ion channel dynamics**
- **No detailed dendritic morphology**
- **No detailed cortical architecture**
- **No consciousness or sentience**
- **Limited by computing resources**

### Research Questions

NLM investigates whether:

1. **Memory systems can integrate with neural dynamics**
2. **Neuromodulation can affect plasticity in a coordinated way**
3. **Prediction can become a central organizing principle**
4. **Developmental stages can modulate learning**
5. **Replay can reinforce memory consolidation**
6. **The complete brain loop functions coherently**

### Building from Source

#### Prerequisites
- C++17 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher (for build system)

#### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallelization
make -j4
```

#### Running Tests

```bash
# Run built-in tests
cd build
ctest --output-on-failure
```

### Performance Metrics

#### Neural Core
- **Neurons**: ~1000-10000 typical
- **Firing Rate**: ~1-100 Hz
- **Connectivity**: Sparse (10-30% probability)
- **Event-Driven**: Spike-based computation

#### Memory Systems
- **Working Memory**: 100-1000 traces
- **Episodic Memory**: 1000-100000 episodes
- **Consolidation**: Every 1000-10000 steps
- **Replay**: Every 100-1000 steps

#### Cognitive Processing
- **Planning Depth**: 3-10 steps ahead
- **Concept Formation**: Pattern abstraction
- **Attention Selection**: Competitive selection
- **Confidence Estimation**: Uncertainty quantification

### License

MIT License

### Authors

Research project - See documentation for scientific background and references.

### Citation

If you use NLM for research, please cite:

```
@article{nlm2026,
    title={NLM/熙然: An Experimental Artificial Developmental Brain},
    author={NLM Authors},
    year={2026},
    journal={Research Project},
    note={Phase 6: Final Integration}
}
```

### Support

For technical support, bug reports, or questions:
- Check the project documentation
- Review the source code for examples
- Join the NLM research community (when available)
