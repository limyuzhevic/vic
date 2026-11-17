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

## Current Status (Phase 6: Final Integration) ✅

**Phase 6: Final Integration Complete!**

Phase 6 successfully integrated all existing systems into a coherent artificial brain. Key achievements:

### 🔬 Core Neural Processing
- **Real LIF neuron dynamics** with event-driven spike propagation
- **~37,000 synapses** for a 500-neuron brain with random connectivity
- **Spiking activity**: 8k-199k spikes per 1000 simulation steps
- **STDP plasticity** successfully potentiates synapses (0.35 → 0.99)
- **Inhibitory neurons** (20% of internal population) with stable plasticity

### 🧠 Integrated Memory Systems
- **Working memory** with persistent activity and competitive dynamics
- **Episodic memory** with experience encoding and replay capabilities
- **Associative memory** for pattern associations and generalization
- **All memory systems** fully connected to neural processing

### 🎯 Neuromodulation Integration
- **Dopamine** affects neural excitability and plasticity
- **Curiosity** drives exploration behavior
- **Novelty detection** integrated with sensory processing
- **All neuromodulators** connected to plasticity rules

### 🔮 Prediction System
- **Prediction system** integrated into the main brain loop
- **Prediction error** signals drive learning and adaptation
- **Confidence tracking** implemented for uncertainty estimation

### 🧭 Cognition Systems
- **Neural planner** with action sequence evaluation
- **Concept formation** from experience patterns
- **Attention** with competitive selection dynamics
- **All cognition systems** connected to perception and action

### 🏃‍♂️ Development Integration
- **Developmental stages** affect plasticity rates
- **Structural plasticity** modulated by age and development
- **Neural excitability** changes with developmental stage

### 💾 Persistence
- **Checkpoint save/load** implemented and working
- **Brain state serialization** for persistent learning
- **Resume capability** from saved checkpoints

### 🎯 Replay and Consolidation
- **Episodic memory replay** during simulation periods
- **Memory consolidation** for important episodes
- **Integration with** sleep/rest cycles for learning optimization

## 🚀 Quick Start (Python API)

The easiest way to start is using the Python bindings:

```bash
# Install with scikit-build-core (modern Python packaging)
pip install scikit-build-core pybind11 pytest numpy
pip install .
```

### Minimal Working Example

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## 📋 Building from Source

### Prerequisites
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Commands

#### Standard C++ Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

#### Build with Python Bindings (Recommended)
```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

## 🏃‍♂️ Running Demos

### Phase 2: Basic Neural Computation
```bash
./nlm
```

### Phase 3: World Interaction
```bash
./nlm_phase3_demo
```

### Phase 4: Full Cognitive Abilities
```bash
./nlm_phase4_demo
```

### Phase 6: Final Integration Test
```bash
./nlm_phase6_demo
```

## 📖 Documentation

- **`easy_usage.md`** - Quick start guide for beginners
- **`HOW_TO_USE.md`** - Comprehensive usage instructions
- **`docs/`** - Technical documentation

## 🧪 Testing

```bash
# Run Python tests
pytest
```

## 📝 License

MIT License. See LICENSE file for details.
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
Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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
