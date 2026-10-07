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

↓↓
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

## Enhanced Python Bindings

NLM provides enhanced Python bindings that offer a Pythonic interface to the C++ neural simulation framework. The bindings feature:

### Key Features

1. **Better Error Handling**: Custom Python exceptions for robust error handling
2. **Convenience Methods**: Enhanced methods for common operations
3. **Utility Functions**: Helper functions for creating and managing NLM components
4. **Type Checking**: Input validation and type checking
5. **Advanced Features**: Factory methods and utility functions for experienced users
6. **Comprehensive Testing**: Extensive test suite for reliable usage

### Quick Start with Enhanced Bindings

```bash
# Build NLM with Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Test the enhanced Python bindings
python3 python/test_pynlm.py

# Run quick test
python3 python/test_quick.py
```

### Basic Python Usage

```python
import pynlm

# Create a default configuration with typical settings
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Create a simple world with specified dimensions
world = pynlm.createSimpleWorldConfigured(100, 100, 16, 16)

# Create and initialize an agent brain
agent = pynlm.createAgentBrainWithWorld(brain, world)

# Enhanced vision processing
vision = pynlm.createVision(10, 10, 3)
vision.setDataNormalized([0.5, 0.7, 0.3, 0.9], 0.0, 1.0)

# Enhanced actions
action = pynlm.createAction(pynlm.ActionType.MoveForward)
```

### Enhanced Configuration

```python
# Create configuration with specific capacity
config = pynlm.Brain.createDefaultConfigWithCapacity(1000)

# Configure with typical settings
config.set("simulation.steps", 100)
config.set("simulation.dt", 0.01)
config.set("brain.memory.enabled", True)
config.set("brain.plasticity.enabled", True)
config.set("brain.development.enabled", True)

# Validate configuration
if config.validate():
    print("Configuration is valid")
```

### Advanced Simulation

```python
# Run simulation episode
reward = agent.runEpisode(world, sensory_inputs)

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get brain statistics
total_neurons = brain.getTotalNeuronCount()
excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()

# Save and load brain state
brain.save("brain_state.json")
brain2 = pynlm.Brain(config)
brain2.load("brain_state.json")
```

## Python Testing

The enhanced Python bindings include comprehensive testing:

```bash
# Run comprehensive Python test suite
python3 python/test_pynlm.py

# Run quick test
python3 python/test_quick.py
```

### Python Test Categories:

1. **Basic Imports** - Tests all imports and basic functionality
2. **Config Class** - Tests enhanced Config methods
3. **Brain Class** - Tests brain creation and configuration
4. **Vision Class** - Tests enhanced Vision methods
5. **SensoryPercept Class** - Tests sensory perception
6. **Action Class** - Tests action creation and management
7. **SimpleWorld Class** - Tests world simulation
8. **AgentBrain Class** - Tests agent brain functionality
9. **Utility Functions** - Tests utility functions
10. **Error Handling** - Tests error handling
11. **Advanced Features** - Tests advanced simulation features

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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/
│   ├── CMakeLists.txt
│   ├── test_*.cpp     # C++ tests
│   └── test_main.cpp
└── python/            # Python bindings and utilities
    ├── bindings.cpp    # Enhanced C++ bindings
    ├── CMakeLists.txt  # Python bindings build
    ├── test_pynlm.py   # Comprehensive test suite
    ├── test_quick.py  # Quick test script
    └── ENHANCED_BINDINGS.md  # Detailed documentation
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

## C++ Testing

The C++ tests verify the core neural computation functionality:

```bash
# Build and run C++ tests
mkdir build
cd build
cmake ..
make nlm_test
./nlm_test
```

## Python Bindings Testing

The enhanced Python bindings include comprehensive testing:

```bash
# Run comprehensive Python test suite
python3 python/test_pynlm.py

# Run quick test
python3 python/test_quick.py
```

## License

MIT

## Authors

Research project — See docs for scientific background.
