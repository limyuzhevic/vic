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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

### Standard C++ Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings Build (pynlm)
```bash
# From the project root, install the Python module
pip install scikit-build-core pybind11 pytest numpy
pip install .  # This builds and installs pynlm

# Or in development mode
pip install -e .
```

## Running

Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

### Core Demos
```bash
./nlm                 # Phase 2: Real Neural Computation demo (from src/main.cpp)
./nlm_phase3_demo     # Phase 3: Basic environment interaction
./nlm_phase4_demo     # Phase 4: Advanced cognitive capabilities
./nlm_phase5_demo     # Phase 5: Pattern probe, sandbox, and checkpoint demo
./nlm_phase6_demo     # Phase 6: Complete integration test
./nlm_phase6_text_demo # Phase 6: Text-based interaction demo
./nlm_benchmark       # Performance benchmark suite
```

### Testing
```bash
# Run unit tests (requires build with tests enabled)
cd build
ctest --output-on-failure

# Or run specific test suites
./nlm_test           # Core unit tests
```

### Python API
```bash
# After installing pynlm with pip install .
python -c "import pynlm; print(pynlm.__version__)"
python examples/simple.py   # Example from examples/ directory
python examples/full_agent.py # Full agent example
```

## Python API Usage

The `pynlm` Python module provides a simplified interface to the NLM brain:

### Basic Usage
```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Agent in World Simulation
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify configuration options
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")
```

## Configuration Options

NLM supports extensive configuration through key-value pairs. Common configuration options include:

### Brain Configuration
- `brain.neuron_count`: Number of neurons (default: 1000)
- `brain.region_count`: Number of brain regions (default: 1)
- `brain.connection_probability`: Probability of synaptic connections (default: 0.1)
- `brain.initial_weight_mean`: Mean initial synaptic weight (default: 0.5)
- `brain.v_thresh`: Neuron threshold potential (mV, default: -50.0)

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP (default: true)
- `plasticity.stdp.learning_rate`: STDP learning rate (default: 0.001)
- `plasticity.hebbian.enable`: Enable Hebbian learning (default: true)

### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale (default: 1.0)
- `neuromod.curiosity.enable`: Enable curiosity (default: true)
- `neuromod.novelty.enable`: Enable novelty detection (default: true)

### Developmental Configuration
- `development.initial_plasticity`: Initial plasticity rate (default: 1.0)
- `development.maturation_rate`: Rate of developmental maturation (default: 0.01)

## Example Scripts

See the `examples/` directory for complete working examples:

### examples/simple.py
Basic brain initialization and simulation

### examples/full_agent.py
Complete agent-in-world simulation with learning

### examples/visualize.py
Visualize brain activity and connections

## Development Setup

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Build Options
```bash
# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Build with Python bindings
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
```

### Running Tests
```bash
# Build with tests
mkdir build
cd build
cmake .. -DBUILD_TESTING=ON
make -j4

# Run all tests
ctest --output-on-failure

# Run specific test category
./nlm_test                    # Core brain tests
./nlm_benchmark              # Performance tests
```

## Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"CMake cannot find Python"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"Segmentation fault"**
- Ensure `initialize()` is called before `step()`
- Check that `reset()` is called before re-running simulations
- Verify configuration values are within expected ranges

### Getting Help

For technical questions:
- Check the documentation in `docs/` directory
- Review the Phase-specific demo outputs for expected behavior
- Consult the API reference in the Python module documentation

### Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

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
