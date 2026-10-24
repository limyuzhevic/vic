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

## Enhanced Phase 6 Features

### Advanced Integration Capabilities

#### Unified Brain Loop Architecture
The Phase 6 system implements a closed-loop architecture that seamlessly integrates all brain components:

- **Memory Integration**: Working, episodic, semantic, and procedural memory systems are fully connected to neural processing
- **Neuromodulation Hub**: Dopamine, curiosity, novelty, and prediction error signals coordinate plasticity and learning
- **Predictive Processing**: Forward models predict sensory input and generate prediction errors
- **Cognitive Architecture**: Neural planner, concept formation, and attentional selection work in concert
- **Development Engine**: Age-dependent plasticity, structural maturation, and learning critical periods

#### Real-Time Learning Mechanisms

The system implements sophisticated learning through:

- **Spatiotemporal Learning**: STDP with precise spike timing and neuromodulation
- **Experience-Dependent Plasticity**: Hebbian learning for co-activation patterns
- **Reward-Modulated Learning**: Dopamine signals gate plasticity based on prediction errors
- **Structural Adaptation**: Synaptogenesis, pruning, and rewiring based on experience
- **Memory Replay**: Offline consolidation during rest periods

#### Advanced User Interface

The Python API (pynlm) provides:

- **High-Level Brain Creation**: `pynlm.createBrain()` with configurable parameters
- **Simplified World Interaction**: `pynlm.createSimpleWorld()` with 2D environments
- **Agent Integration**: `pynlm.createAgentBrain()` for brain-world interaction
- **Learning Control**: Enable/disable neuromodulation, plasticity, curiosity
- **Real-Time Monitoring**: Get firing rates, spike counts, developmental stages
- **Checkpoint Management**: Save and load brain states

### System Capabilities

#### Neural Processing
- **500+ LIF neurons** with spike-timing dependent plasticity
- **Event-driven simulation** with synaptic delays
- **Multi-region connectivity** with different neuron types
- **Real-time plasticity** with STDP and Hebbian rules

#### Memory Systems
- **Working Memory**: Active maintenance and competitive selection
- **Episodic Memory**: Experience recording and replay
- **Semantic Memory**: Pattern association and concept formation
- **Procedural Memory**: Skills and habits from repeated experience

#### Cognitive Functions
- **Prediction**: Forward models for sensory expectations
- **Planning**: Neural computation of action sequences
- **Attention**: Competitive selection of neural representations
- **Concept Formation**: Pattern discovery from neural activity

#### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Novelty detection and exploration motivation
- **Novelty**: Surprise-based learning and attention
- **Prediction Error**: Mismatch between expected and actual outcomes

#### Development
- **Critical Periods**: Age-dependent plasticity windows
- **Maturation**: Structural and functional development
- **Aging**: Gradual decline in plasticity and performance
- **Experience--dependent**: Environmental enrichment effects

## Building and Running

### Basic Build (C++)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Integration Tests
```bash
# Phase 6 demo - comprehensive integration test
./nlm_phase6_demo

# Phase 4 demo - cognitive capabilities demo
./nlm_phase4_demo

# Phase 3 demo - basic world interaction
./nlm_phase3_demo

# Main simulation
./nlm
```

### Python Bindings
```bash
# Install Python bindings
pip install scikit-build-core pybind11 pytest numpy
pip install -e .
```

### Python Usage Example
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

### Command Line Options
```bash
# Run with custom configuration
./nlm --config=my_config.cfg

# Run with specific parameters
./nlm --neuron_count=2000 --connection_probability=0.05

# Debug mode
./nlm --debug

# Save checkpoint
ctrl+c during simulation or use --checkpoint=<filename>
```

## Performance

### Optimized Features

#### Memory Efficiency
- **Memory pools**: Pre-allocated neuron and synapse pools
- **Event queues**: Efficient spike event management
- **Sparse connectivity**: Only stores existing connections
- **Compact data structures**: Minimized memory overhead

#### Computational Speed
- **SIMD vectorization**: Parallel processing of neuron groups
- **Event-driven simulation**: Skip idle neurons
- **Multi-threading**: Parallel processing in different regions
- **Hardware acceleration**: Optimized for modern CPUs

#### Scaling
- **Capable of 10,000+ neurons** on standard hardware
- **Linear scaling** with increased neuron count
- **Memory efficient** for large-scale simulations

## Scientific Model

### Brain-Inspired Architecture

The NLM implements biologically-inspired principles:

#### Neuronal Dynamics
- **Leaky Integrate-and-Fire**: Biologically realistic membrane potential dynamics
- **Spike-frequency adaptation**: Address learning over long time scales
- **Refractory periods**: Realistic firing constraints
- **Ion channel dynamics**: Simplified but accurate modeling

#### Synaptic Properties
- **Short-term plasticity**: Depression and facilitation dynamics
- **Delay propagation**: Realistic synaptic transmission delays
- **Hebbian learning**: Co-activation strengthens connections
- **STDP**: Spike timing-dependent plasticity for learning

#### Neural Systems
- **Working memory**: Persistent activity maintained by recurrent connections
- **Episodic memory**: Pattern completion and sequence replay
- **Prediction error**: Forward models for expectation violations
- **Attention**: Competitive selection of neural representations

## Files and Directories

```
NLM/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This documentation
├── docs/                       # Scientific documentation
│   ├── ARCHITECTURE.md        # System architecture
│   ├── SCIENCE.md            # Scientific background
│   ├── ROADMAP.md            # Development roadmap
│   ├── EXPERIMENTS.md        # Experiment descriptions
│   └── PHASE6_FINAL_AUDIT.md # Phase 6 audit report
├── src/                        # Source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Neural components (integrated)
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity rules
│   ├── development/           # Developmental system
│   ├── neuromodulation/       # Neuromodulators (integrated)
│   ├── memory/               # Memory systems (integrated)
│   ├── prediction/            # Prediction systems (integrated)
│   ├── cognition/            # Cognitive mechanisms (integrated)
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework (Phase 6)
│   └── visualization/         # Visualization
├── tests/                      # Test suite
├── configs/                    # Default configuration files
└── .gitignore                  # Version control
```

## Advanced Usage

### Custom Configurations
Create your own configuration files in `configs/` directory:

```ini
# configs/custom.cfg
brain.neuron_count = 3000
brain.synapse_density = 0.15
plasticity.stdp.enable = true
neuromod.dopamine.scale = 1.5
neuromod.curiosity.enable = true
```

### Batch Processing
```python
import pynlm
import numpy as np

# Run multiple simulations with different parameters
results = []
for seed in range(10):
    config = pynlm.createDefaultConfig()
    config.set("random_seed", seed)
    config.set("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(500):
        brain.step(step)
    
    results.append({
        'seed': seed,
        'final_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    })
    
print(f"Average final spikes: {np.mean([r['final_spikes'] for r in results])}")
```

### Integration with External Systems

The NLM can be integrated with:

- **Robotics**: Control robotic systems with learned policies
- **Games**: Create adaptive AI opponents
- **Simulation**: Study neural development in virtual environments
- **Research**: Investigate learning mechanisms and brain function

## License

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

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
