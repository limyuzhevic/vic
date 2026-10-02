# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is **NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start

If you want to start exploring NLM quickly, follow these three steps:

### 1. Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure and build
 cmake .. -DCMAKE_BUILD_TYPE=Release
 make -j4
```

### 2. Install Python Bindings

```bash
cd ..

# Install the Python package
pip install -e .
```

### 3. Run Examples

```bash
# Run C++ examples
./examples/basic/01_silent_brain

# Run Python examples
python examples/basic/01_silent_brain.py
```

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### What Makes NLM Special?

| Feature | Description |
|---------|-------------|
| **Biological Inspiration** | No transformers or deep learning models |
| **Modular Architecture** | Clear separation of concerns |
| **Event-Driven Processing** | Real-time spike propagation |
| **No Predefined Knowledge** | All learning from experience |
| **Deterministic Simulation** | Reproducible via explicit random seeds |
| **Performance-Oriented** | Data-oriented design for large-scale simulation |

## Project Structure

```
NLM/
├── src/                    # C++ source code (95% of project)
│   ├── core/              # Core utilities (Config, Random, Logger)
│   ├── brain/             # Neural components (Neurons, Synapses, Brain)
│   ├── dynamics/          # Neural dynamics and spike processing
│   ├── plasticity/        # Plasticity rules (STDP, Hebbian, Structural)
│   ├── development/       # Developmental processes
│   ├── neuromodulation/   # Neuromodulator signals
│   ├── memory/           # Memory systems (Working, Episodic, etc.)
│   ├── cognition/        # Cognitive mechanisms (Attention, Planning)
│   ├── sensory/          # Sensory processing
│   ├── prediction/       # Prediction systems
│   ├── motor/            # Motor control
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework
│   └── visualization/    # Visualization interfaces
├── python/                 # Python bindings
├── examples/              # Ready-to-run example scripts
├── tests/                 # C++ unit tests
├── configs/               # Configuration files
├── docs/                  # Documentation (12 comprehensive files)
├── CMakeLists.txt
├── pyproject.toml
├── README.md
├── HOW_TO_USE.md
└── easy_usage.md
```

## Building

### From Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build configuration (Debug/Release) | Release |
| `BUILD_TESTS` | Build unit tests | ON |
| `WITH_PYTHON_BINDINGS` | Build Python bindings | ON |
| `BUILD_SHARED_LIBS` | Build shared libraries | OFF |

### Quick Build Commands

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with Python bindings
pip install -e .

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./build/bin/nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Examples

Python examples are organized by complexity:

#### Basic Examples
```bash
python examples/basic/01_silent_brain.py      # Simplest possible brain
python examples/basic/02_minimal_brain.py      # One-line example
python examples/basic/03_brain_stats.py        # Statistics collection
```

#### Intermediate Examples
```bash
python examples/intermediate/01_agent_world.py    # Complete agent system
python examples/intermediate/02_learning_agent.py # Learning through experience
python examples/intermediate/03_development_demo.py # Brain development
```

#### Advanced Examples
```bash
python examples/advanced/01_neural_developer.py  # Full feature demonstration
python examples/advanced/02_research_experiment.py # Research-grade experiment
```

## Python API Overview

NLM provides a comprehensive Python API for creating and working with neural brains:

### Key Classes

| Class | Purpose |
|-------|---------|
| `createBrain(config)` | Create a neural brain |
| `createAgentBrain(brain)` | Connect brain to world interface |
| `createSimpleWorld()` | Create interactive world environment |
| `createDefaultConfig()` | Get default configuration |

### Basic Workflow

```python
import pynlm

# 1. Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# 2. Initialize
brain.initialize()

# 3. Run simulation
for step in range(100):
    brain.step(step)

# 4. Get results
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Agent Workflow (Complete System)

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Make motor decision
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration

NLM uses a simple key=value configuration format. Example `configs/default.cfg`:

```ini
# Random seed for reproducible experiments
random_seed = 42

# Simulation parameters
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Learning parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation parameters
dopamine_baseline = 0.1
neuromodulation.curiosity.enable = true
neuromodulation.novelty.enable = true
```

### Configuration Management

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Neurons: {neurons}")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.001)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config.loadFromFile("my_config.cfg")
```

## Testing

### Unit Tests (C++)

```bash
# Build and run tests
cd build
ctest --output-on-failure
```

### Example Tests

Run all examples to verify functionality:

```bash
cd ..  # Back to project root
python run_examples.py
```

## Key Features

### 1. Biological Plausibility

- **LIF Neurons**: Leaky Integrate-and-Fire neuron dynamics
- **Spike Timing Dependence**: STDP plasticity rules
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Developmental Stages**: Primitive → Adult brain maturation

### 2. Cognitive Abilities

- **Working Memory**: Persistent activity and competition
- **Episodic Memory**: Experience encoding and replay
- **Prediction Systems**: Forward models and prediction errors
- **Attention**: Competitive selection dynamics
- **Planning**: Neural action sequence evaluation
- **Concept Formation**: Pattern recognition and categorization

### 3. Developmental Learning

- **Critical Periods**: Enhanced plasticity in early development
- **Synaptogenesis**: Growing new connections
- **Pruning**: Eliminating unused connections
- **Maturation**: Refining neural circuits over time
- **Replay**: Memory consolidation during rest

### 4. Real-World Interaction

- **Sensory Processing**: Vision, touch, internal signals
- **Motor Control**: Action selection and execution
- **Reward Learning**: Reinforcement through prediction errors
- **Curiosity-Driven Exploration**: Novelty seeking behavior

## Performance Characteristics

| Aspect | Details |
|--------|---------|
| **Neuron Model** | Leaky Integrate-and-Fire (simplified) |
| **Synaptic Delays** | Realistic spike timing delays |
| **Memory Systems** | Working, episodic, and associative |
| **Plasticity Rules** | STDP, Hebbian, structural |
| **Scalability** | Designed for 1K-1M+ neurons |
| **Deterministic** | Reproducible with fixed seeds |

## Scientific Background

NLM investigates whether neural systems can:

1. **Integrate** memory systems with neural dynamics
2. **Coordinate** neuromodulation with plasticity
3. **Organize** prediction as a central principle
4. **Modulate** learning through developmental stages
5. **Consolidate** memories through replay mechanisms
6. **Function** as a complete brain loop

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

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Support

For questions and support, please:

1. Read the documentation files
2. Run the examples to understand the system
3. Check the issue tracker for known issues
4. Submit bug reports with reproduction steps
5. Contribute improvements to help others

---

**NLM is a research project investigating computational brain-like systems. It makes NO claims that NLM accurately reproduces biological brains, but rather investigates whether artificial neural systems can exhibit brain-like properties and behaviors.**