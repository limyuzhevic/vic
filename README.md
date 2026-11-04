# NLM (熙然) - Neural Learning Machine

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Project Summary

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

## Current Status

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

## Building

### Prerequisites
- CMake 3.16 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, or MSVC 19+)
- Make or Ninja

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4
```

### Build Options

- `NLM_BUILD_PYTHON_BINDINGS`: Build Python bindings (OFF by default)
- `CMAKE_BUILD_TYPE`: Build type (Debug, Release, RelWithDebInfo)
- `NLM_ENABLE_TESTS`: Enable tests (ON by default)

## Running

### Build All Targets

Build all targets first:

```bash
cd build
make
```

### Running Demos

The entry points are separate demos per phase:

```bash
# Phase 2 demo - Real neural computation
./nlm

# Phase 3 demo - World interaction
./nlm_phase3_demo

# Phase 4 demo - Cognitive functions
./nlm_phase4_demo

# Phase 5 demo - Performance optimizations
./nlm_phase5_demo

# Phase 6 demo - Integration test
./nlm_phase6_demo

# Phase 6 text demo - Text processing
./nlm_phase6_text_demo

# Benchmark
./nlm_benchmark
```

### Configuration

Most demos can be configured with command-line arguments:

```bash
./nlm --neuron_count=1000 --region_count=2 --random_seed=42
./nlm_phase3_demo --environment_width=20 --environment_height=20
./nlm_phase6_demo --max_steps=1000 --enable_checkpointing=true
```

### Configuration Files

Configuration files are stored in the `configs/` directory:

```bash
# Show available configs
ls -la configs/

# Use a specific config
./nlm --config configs/custom.cfg
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
│   │   ├── Config/      # Configuration system
│   │   ├── Logger/      # Logging system
│   │   ├── Random/      # Random number generation
│   │   ├── SimulationClock/  # Simulation timing
│   │   └── Types/       # Type definitions
│   ├── agent/          # Agent-brain interface
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
│   ├── world/          # Simple world
│   ├── performance/    # Performance optimization
│   └── tests/          # Test framework
├── tests/              # Test executables
├── python/             # Python bindings (Phase 5)
├── configs/            # Configuration files
└── docs/               # Documentation
```

## Phase Summary

### Phase 1: Foundation
- Established project structure and build system
- Implemented core type system with strongly-typed IDs
- Created configuration and logging infrastructure
- Set up neural interfaces and basic communication

### Phase 2: Core Neural Computation
- Implemented Leaky Integrate-and-Fire (LIF) neuron dynamics
- Created event-driven spike propagation with synaptic delays
- Implemented STDP and Hebbian plasticity rules
- Added structural plasticity (synaptogenesis and pruning)

### Phase 3: World Interaction
- Integrated brain with simple world environment
- Implemented sensory input and motor output systems
- Added reward prediction error mechanism
- Implemented developmental stages and aging
- Added novelty detection and curiosity systems

### Phase 4: Cognitive Functions
- Implemented neural prediction systems
- Created working memory and episodic memory
- Added concept formation and neural attention
- Implemented predictive planning and self-model
- Added social learning and continual learning capabilities

### Phase 5: Performance Optimization
- Implemented memory pools and event queues
- Added SIMD vectorization for performance
- Created parallel processing capabilities
- Implemented checkpoint system for persistence

### Phase 6: Final Integration
- Integrated all previous phases into coherent brain loop
- Connected memory systems to neural processing
- Integrated neuromodulation effects
- Added prediction with learning
- Implemented developmental plasticity modulation
- Created checkpoint save/load functionality
- Implemented replay and consolidation
- Added comprehensive integration test

## Scientific Background

### Biological Inspiration

NLM is inspired by several aspects of biological brains:

1. **Neural Coding**: Uses spike-based coding similar to biological neurons
2. **Plasticity**: Implements synaptic plasticity rules found in biological systems
3. **Development**: Includes developmental stages similar to brain maturation
4. **Learning**: Uses reward-based and unsupervised learning mechanisms
5. **Memory**: Implements multiple memory systems similar to biological memory

### Key Features

1. **Event-Driven Processing**: Uses spikes for efficient computation
2. **Plastic Synapses**: Synapses change strength based on activity
3. **Structural Plasticity**: New connections form and old ones prune
4. **Neuromodulation**: Chemicals like dopamine affect learning and behavior
5. **Development**: Brain changes over time, affecting plasticity and behavior
6. **Memory**: Stores experiences and replays them for learning

### Scientific Limitations

NLM makes no claims about accurately reproducing biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

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
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Authors

Research project — See docs for scientific background.

## Getting Help

### Documentation

For detailed documentation, see the `docs/` directory:

- `ARCHITECTURE.md`: Technical architecture and design
- `SCIENCE.md`: Scientific background and principles
- `ROADMAP.md`: Project roadmap and future plans
- `EXPERIMENTS.md`: Experiment descriptions and results
- `LIMITATIONS.md`: Current limitations and future challenges
- `PHASE6_FINAL_AUDIT.md`: Phase 6 integration audit

### Support

For technical support, please refer to the project's issue tracker or contact the development team through the appropriate channels.

### Contributing

The NLM project welcomes contributions from the community. Please refer to the project's contribution guidelines for information on how to get involved.

## Quick Start Guide

### 1. Clone the Repository

```bash
git clone <repository-url>
cd NLM
```

### 2. Build the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 3. Run a Demo

```bash
# Run Phase 2 demo (basic neural computation)
./nlm

# Run Phase 6 demo (integration test)
./nlm_phase6_demo
```

### 4. Explore the Code

```bash
# Browse source code
ls src/
ls src/core/
ls src/brain/
ls src/experiments/
```

### 5. Check Configuration

```bash
# View available configurations
cat configs/default.cfg

# Run with custom configuration
./nlm --config configs/default.cfg
```

### 6. Learn More

```bash
# View documentation
less docs/README.md
less docs/ARCHITECTURE.md
```

## Troubleshooting

### Common Issues

1. **Build Errors**
   - Ensure you have CMake 3.16+ and C++20 compatible compiler
   - Check for missing dependencies
   - Try building with less parallel jobs: `make -j2`

2. **Runtime Errors**
   - Check configuration file paths
   - Verify command-line arguments
   - Ensure required dependencies are installed

3. **Performance Issues**
   - Reduce neuron count for testing: `--neuron_count=100`
   - Disable visualization: `--visualization_enabled=false`
   - Reduce simulation steps: `--max_steps=100`

4. **Memory Issues**
   - Increase system memory
   - Reduce neuron count
   - Use checkpointing to save memory

### Getting Help

If you encounter issues, please:

1. Check the documentation in `docs/`
2. Review the example code in `examples/`
3. Check the project issue tracker
4. Look at the demo scripts for usage examples

## Examples

### Basic Usage

See `examples/` directory for practical examples of using NLM:

- `example_basic_usage.cpp`: Basic brain usage
- `example_configuration.cpp`: Configuration examples
- `example_plasticity.cpp`: Plasticity demonstrations
- `example_memory.cpp`: Memory system examples

### Configuration Examples

See `configs/` directory for configuration file examples:

- `default.cfg`: Default configuration
- `custom.cfg`: Custom configuration
- `benchmark.cfg`: Benchmark configuration

### Command Line Examples

```bash
# Basic neural computation
./nlm --neuron_count=500 --region_count=2 --random_seed=42

# World interaction
./nlm_phase3_demo --environment_width=20 --environment_height=20 --max_steps=1000

# Integration test with checkpointing
./nlm_phase6_demo --max_steps=500 --enable_checkpointing=true --enable_replay=true

# Performance benchmark
./nlm_benchmark --neuron_count=2000 --max_steps=10000
```

## Future Directions

### Research Goals

1. **Enhanced Learning**: Implement more sophisticated learning algorithms
2. **Social Interaction**: Add multi-agent interaction capabilities
3. **Cognition**: Develop more advanced cognitive functions
4. **Development**: Implement longer developmental stages
5. **Optimization**: Improve performance and scalability

### Technical Challenges

1. **Scalability**: Handle larger neural networks efficiently
2. **Real-time Performance**: Support real-time interaction
3. **Resource Management**: Optimize memory and CPU usage
4. **Integration**: Better integrate different system components
5. **Validation**: Develop more comprehensive testing

### Community Engagement

1. **Open Source**: Encourage community contributions
2. **Documentation**: Maintain comprehensive documentation
3. **Education**: Provide educational materials and examples
4. **Collaboration**: Foster collaboration with other research groups
5. **Application**: Develop practical applications and use cases

## Conclusion

NLM represents a significant step forward in creating artificial neural systems that can develop, learn, and adapt through interaction with their environment. While the project is still in its early stages, it has already demonstrated several key capabilities:

1. **Neural Processing**: Real spiking neural computation
2. **Learning**: Activity-dependent synaptic modification
3. **Development**: Age-dependent plasticity and changes
4. **Memory**: Experience storage and replay
5. **Integration**: Coherent brain system with multiple interacting components

The project continues to evolve, with Phase 6 representing a major milestone in creating a unified, functional artificial brain system. Future work will focus on enhancing capabilities, improving performance, and expanding the range of applications.

---

*This project is part of ongoing research in artificial neural systems and computational neuroscience.*
