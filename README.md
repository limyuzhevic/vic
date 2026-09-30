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

### Tutorial System

This repository includes a comprehensive tutorial system covering all aspects of NLM. The tutorials are organized into 10 major sections and provided in the `NLM_TUTORIALS.md` file for easy reference and study.

**To access the complete tutorial system:**
```bash
cat NLM_TUTORIALS.md
```

**Table of Contents Overview:**
1. Getting Started Guide - Neuroscience fundamentals and first simulation
2. Basic Usage Examples - Neuron simulation, synaptic transmission, STDP
3. Advanced Examples - Neuromodulation, curiosity, development, predictive coding
4. API Documentation - Python bindings, C++ API, configuration files, visualization
5. Use Case Scenarios - Research, development, cognitive, neuromodulation, sensory applications
6. Tutorial Series - 5 progressive tutorials from "Hello Neural World" to "Complete Integration"
7. Best Practices - Performance optimization, memory management, configuration patterns
8. Troubleshooting Guide - Common issues and debugging strategies
9. Performance Optimization - Parameter tuning, memory optimization, parallelization
10. Research Applications - Research methodology, experimental design, data analysis

## Quick Start Tutorial

If you're new to NLM, start with this simple example:

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run neural computation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Done! Your brain fired {brain.getTotalSpikeCount()} spikes")
print(f"Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
```

## Complete Tutorial System

The NLM tutorial system provides comprehensive documentation for users at all skill levels:

### For Complete Beginners
- **Getting Started Guide**: Neuroscience fundamentals and first working simulation
- **Basic Usage Examples**: Simple neuron simulation, synaptic transmission, STDP learning
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Simple Brain Integration**: Creating and managing brain regions
- **Phase 6 Integration Demo**: Complete integrated brain simulation

### For Intermediate Users
- **Advanced Examples**: Neuromodulation, curiosity, developmental biology, predictive coding
- **Use Case Scenarios**: Real-world applications across different domains
- **Tutorial Series**: 5 progressive tutorials building from basics to complexity
- **Best Practices**: Performance optimization and debugging strategies
- **Troubleshooting**: Common issues and solution approaches

### For Advanced Users
- **Performance Optimization**: Parameter tuning and computational efficiency
- **Research Applications**: Experimental design and data analysis methodologies
- **API Documentation**: Python bindings and C++ API with detailed examples
- **System Architecture**: Deep dive into Phase 6 integration details
- **Extension Points**: Adding new components and mechanisms

## Tutorial Learning Path

### Step 1: Foundation (30-45 minutes)
1. Read the Getting Started Guide section
2. Run the "Hello Neural World" Python example
3. Study the neuron simulation and synaptic transmission examples
4. Understand basic STDP learning mechanisms

### Step 2: Core Concepts (1-2 hours)
1. Complete the brain region creation and management examples
2. Work through the memory system integration demonstrations
3. Explore neuromodulation and reward learning examples
4. Study the Phase 6 integration demo

### Step 3: Advanced Applications (2-4 hours)
1. Implement the curiosity-driven learning examples
2. Create developmental biology simulations
3. Design predictive coding and error-based learning
4. Build use case scenario implementations

### Step 4: Research and Development (4-8 hours)
1. Study the best practices and performance optimization guides
2. Work through the troubleshooting and debugging examples
3. Implement research application methodologies
4. Create experimental designs and data analysis approaches

## System Architecture Overview

NLM implements a complete artificial developmental brain through Phase 6: FINAL INTEGRATION:

```
Phase 6 Integration Achievements:
├── Memory Systems: Working + Episodic + Associative (✓ Connected)
├── Neuromodulation: Dopamine + Curiosity + Novelty (✓ Connected)
├── Prediction System: Temporal sequence learning (✓ Integrated)
├── Cognition Systems: Planner + Concept + Attention + Self-model (✓ Connected)
├── Development: Stage-based maturation (✓ Functional)
├── Persistence: Checkpoint save/load (✓ Working)
└── Replay and Consolidation: Memory replay during rest (✓ Active)
```

### Core Component Categories

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Temporal sequence learning and action prediction
- **Cognition Systems**: Planning, concept formation, attention, self-model
- **Development**: Brain maturation from primitive to adult states
- **Plasticity**: STDP, Hebbian, and structural learning

## Technical Specifications

### Simulation Characteristics
- **Neuron Model**: Leaky Integrate-and-Fire (LIF)
- **Time Scale**: Continuous simulation with variable time steps
- **Memory Systems**: Neural implementations of working and episodic memory
- **Learning Mechanisms**: STDP, Hebbian, reward-modulated plasticity
- **Development**: Multi-stage progression from primitive to adult capabilities

### Performance Features
- **Event-Driven**: Efficient spike processing
- **Parallel Processing**: SIMD vectorization for performance
- **Memory Pools**: Optimized memory management
- **Checkpoint System**: Save/load brain state
- **Replay System**: Memory consolidation during rest

## Getting Help and Support

### Documentation Resources
- **Phase 6 Demo**: `src/experiments/Phase6Demo.cpp` - Complete integration test
- **Architecture Documentation**: `docs/ARCHITECTURE.md` - Detailed component descriptions
- **Science Background**: `docs/SCIENCE.md` - Biological inspiration and limitations
- **Phase 6 Architecture**: `docs/PHASE6_ARCHITECTURE.md` - Final integration details
- **Enhanced Python API**: `python/README.md` - Modern Python interface
- **Easy Usage**: `easy_usage.md` - Quick copy-paste examples
- **Demo CLI**: `demo_enhanced_cli.py` - Enhanced command-line interface

### Tutorial System Files
The complete tutorial system is organized into:
- **NLM_TUTORIALS.md**: Master table of contents and overview
- **Comprehensive code examples** for all 10 tutorial sections
- **Configuration file examples** for various scenarios
- **Visualization usage** and performance optimization guides
- **Research application** methodologies and experimental design

## License

MIT

## Authors

Research project — See docs for scientific background.

## Additional Resources

For the most current and comprehensive tutorial materials, please refer to the `NLM_TUTORIALS.md` file which contains:

- Complete step-by-step working examples
- Detailed explanations of all NLM concepts
- Progressive tutorial series from beginner to advanced
- Real-world use case scenarios
- Best practices and troubleshooting guidance
- Research application methodologies
- Performance optimization techniques

This tutorial system provides everything needed to master NLM from first principles through advanced research applications.

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
