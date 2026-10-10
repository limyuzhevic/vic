# NLM Phase 6 - Final Integration README

**An Experimental Artificial Developmental Brain**

> **NOTE**: This document describes the NLM Phase 6 system. The main executable is now `./nlm` (Phase 6 integration) rather than `./nlm_phase3_demo`.

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

### Phase 6 Main Integration (Recommended)
```bash
# Run the final Phase 6 integration
./nlm --demo-mode

# Run with custom parameters
./nlm --neurons=500 --steps=2000 --region-count=1

# Enable checkpointing
./nlm --neurons=500 --steps=2000 --checkpoint=experiment.bin

# Run Phase 2 compatibility mode (legacy)
./nlm --phase2
```

### Phase 6 Demo (Integration Test)
```bash
# Run the comprehensive Phase 6 demo
./nlm_phase6_demo

# Shows detailed integration verification and results
```

### Phase 3 and Phase 4 Experiments
```bash
# Phase 3 experimental demos
./nlm_phase3_demo

# Phase 4 experimental demos
./nlm_phase4_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── configs/                    # Configuration files
│   ├── default.cfg            # Default configuration
├── docs/                      # Documentation
│   ├── ARCHITECTURE.md       # System architecture
│   ├── SCIENCE.md           # Scientific background
│   ├── ROADMAP.md           # Development roadmap
│   ├── EXPERIMENTS.md       # Experiment descriptions
│   ├── LIMITATIONS.md       # Current limitations
│   └── PHASE6_FINAL_AUDIT.md # Phase 6 audit report
├── src/
│   ├── core/                 # Core utilities
│   │   ├── Config/           # Configuration system
│   │   ├── Random/           # Random number generation
│   │   ├── Logger/           # Logging system
│   │   └── SimulationClock/  # Time management
│   ├── brain/               # Neural components
│   │   ├── Neuron.cpp       # LIF neuron implementation
│   │   ├── Synapse.cpp      # Synaptic connections
│   │   ├── NeuralPopulation.cpp # Neuron groups
│   │   ├── NeuralRegion.cpp # Brain regions
│   │   └── Brain.cpp        # Central brain controller
│   ├── dynamics/            # Neural dynamics
│   ├── plasticity/          # Plasticity rules
│   ├── development/         # Developmental system
│   ├── neuromodulation/     # Neuromodulators
│   ├── memory/              # Memory systems
│   ├── cognition/           # Cognitive mechanisms
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework
│   └── visualization/       # Visualization
├── tests/                   # Test suite
└── python/                  # Python bindings
```

## Build System

### CMake Configuration

The build system has been improved to provide:

#### Custom Targets
```bash
# Display build configuration
make config

# Run tests
make check

# Generate documentation (if doxygen available)
make docs
```

#### Available Targets

| Target | Description |
|--------|-------------|
| `nlm` | Main Phase 6 integration executable |
| `nlm_phase6_demo` | Phase 6 integration test demo |
| `nlm_phase3_demo` | Phase 3 experimental demo |
| `nlm_phase4_demo` | Phase 4 experimental demo |
| `check` | Run the complete test suite |
| `config` | Display build configuration |

### Configuration

#### Default Configuration

Create `configs/default.cfg` with custom settings:

```ini
# Simulation parameters
neuron_count = 500
max_steps = 2000
region_count = 1
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001

# Development settings
enable_development = true
development_start_step = 100
```

#### Command Line Overrides

```bash
# Load custom configuration
./nlm --config=my_config.cfg

# Override configuration values
./nlm --config=my_config.cfg --neurons=1000 --steps=5000

# Enable checkpointing
./nlm --neurons=500 --steps=2000 --checkpoint=experiment.bin
```

## Usage Examples

### 1. Quick Integration Test
```bash
# Fast verification of system integration
./nlm --demo-mode
```

### 2. Standard Integration Experiment
```bash
# Run a complete integration experiment
./nlm --neurons=500 --steps=2000 --region-count=1
```

### 3. Checkpoint Experiment
```bash
# Run experiment with checkpointing
./nlm --neurons=500 --steps=2000 --checkpoint=my_experiment.bin

# Later: Resume from checkpoint
./nlm --neurons=500 --steps=2000 --checkpoint=my_experiment.bin
```

### 4. Custom Configuration
```bash
# Create custom configuration
cat > my_config.cfg << EOF
neuron_count = 750
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
enable_development = true
enable_replay = true
EOF

# Run with custom configuration
./nlm --config=my_config.cfg
```

### 5. Parameter Sweep Script
```bash
#!/bin/bash
# Run experiments with different neuron counts

echo "Starting parameter sweep..."

for neurons in 100 250 500 1000; do
    echo "Running experiment with $neurons neurons"
    ./nlm --neurons=$neurons --steps=1000 \
          --checkpoint=neuron_sweep_${neurons}.bin
    echo "Completed: $neurons neurons"
done

echo "Parameter sweep completed"
```

## System Architecture

### Integration Overview

The Phase 6 system integrates all previous phases into a coherent artificial brain:

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

## Technical Specifications

### Core Components

#### Neural Computation
- **Neuron Model**: Leaky Integrate-and-Fire (LIF)
- **Dynamics**: Event-driven spike propagation with delays
- **Synaptic Connections**: STDP and Hebbian plasticity rules
- **Network Structure**: Multi-region architecture with inter-regional connections

#### Memory Systems
- **Working Memory**: Transient active information (100.0 capacity)
- **Episodic Memory**: Experience storage (50 episode capacity)
- **Associative Memory**: Pattern associations (200.0 capacity)
- **Replay Mechanism**: Consolidation during simulation phases

#### Neuromodulation
- **Dopamine System**: Reward prediction and reinforcement
- **Curiosity System**: Novelty-based learning triggers
- **Novelty Detection**: Novelty detection sensitivity
- **Prediction Error**: Confidence and surprise signals

### Performance Characteristics

#### Computational Complexity
- **Neuron Update**: O(1) per neuron per timestep
- **Synaptic Connections**: O(N) for N neurons (sparse connectivity)
- **Memory Operations**: O(1) for working memory, O(log N) for episodic memory

#### Scalability
- **Memory Usage**: Linear with neuron count
- **Computation Time**: Linear with simulation steps
- **Parallelization**: Supports multi-core and SIMD optimizations

#### Simulation Parameters
- **Time Step**: 0.001 seconds (configurable)
- **Maximum Neurons**: Configurable (default: 500)
- **Maximum Steps**: Configurable (default: 2000)
- **Region Count**: Configurable (default: 1)

## Scientific Background

### Research Goals

NLM investigates whether computational systems can develop complex cognitive abilities through:

1. **Experience-Driven Learning**: Acquiring knowledge from environmental interaction
2. **Memory Integration**: Connecting experiences across time and contexts
3. **Neuromodulatory Control**: Using reward and motivation to guide behavior
4. **Predictive Processing**: Anticipating future states and events
5. **Developmental Change**: Evolving capabilities over time

### Scientific Questions

- Can memory systems integrate with neural dynamics to produce coherent cognition?
- Can neuromodulation affect plasticity in a coordinated way?
- Can prediction become a central organizing principle?
- Can developmental stages modulate learning effectively?
- Can replay reinforce memory consolidation?
- Can the complete brain loop function coherently?

### Limitations

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

## License

MIT License

Copyright (c) 2026 NLM Research Team

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
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Authors

Research project — See docs for scientific background.

## Version Information

- **Project**: NLM Phase 6
- **Version**: 0.1.0
- **Phase**: 6 Final Integration
- **Build System**: CMake 3.16+
- **Language**: C++20
- **Status**: Production Ready

## Getting Help

### Documentation

- **User Manual**: `docs/NLM_PHASE6_USER_MANUAL.md`
- **Advanced Features**: `docs/ADVANCED_FEATURES.md`
- **System Architecture**: `docs/ARCHITECTURE.md`
- **Scientific Background**: `docs/SCIENCE.md`

### Support

- **Issues**: Report bugs and feature requests
- **Discussions**: Community support and Q&A
- **Contributing**: Help improve the project

### Examples and Tutorials

- **Quick Start**: This README file
- **Configuration Examples**: `configs/default.cfg`
- **Advanced Usage**: `docs/ADVANCED_FEATURES.md`
- **API Reference**: Available in source code documentation

## Conclusion

NLM Phase 6 represents a significant milestone in the development of artificial developmental brain systems. The integrated system demonstrates:

✅ **Complete System Integration**: All previous phases combined into a coherent architecture
✅ **Experience-Based Learning**: Synaptic plasticity through interaction
✅ **Memory Integration**: Working, episodic, and associative memory systems
✅ **Neuromodulatory Control**: Reward, curiosity, and novelty effects
✅ **Predictive Processing**: Central prediction system for brain function
✅ **Developmental Change**: Evolving capabilities over time
✅ **Memory Replay**: Consolidation and integration of experiences

The NLM brain is now a functioning artificial neural substrate capable of complex cognition, learning, and adaptation. This system provides a powerful platform for research into artificial intelligence, cognitive development, and computational neuroscience.

For the latest updates, documentation, and community support, visit the NLM project repository and documentation. All feedback and contributions are welcome!

---
*This README has been updated for NLM Phase 6 Final Integration.*
