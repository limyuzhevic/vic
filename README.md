# NLM (熙然) — Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements a complete artificial developmental brain with multiple integrated systems including:

- **Phase 2**: Real neural computation with LIF neurons, STDP, Hebbian plasticity, structural plasticity
- **Phase 3**: World interaction loop with sensory input and motor output
- **Phase 4**: Prediction, working memory, episodic memory, concept formation, attention
- **Phase 5**: Performance optimizations (memory pools, SIMD, parallel processing, checkpointing)
- **Phase 6**: Final integration - all systems working together as a unified brain

NLM explores whether memory systems can integrate with neural dynamics, whether neuromodulation can affect plasticity in a coordinated way, and whether prediction can become a central organizing principle for intelligent behavior.

## Quick Start

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running Integration Demo

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo
```

### Python Bindings

```bash
# Install Python library
pip install .

# Use in Python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(1000):
    brain.step(i)
```

## Project Structure

```
NLM/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This documentation
├── easy_usage.md               # Quick start guide for beginners
├── HOW_TO_USE.md               # Complete usage documentation
├── pyproject.toml             # Python bindings configuration
├── configs/                    # Configuration files
│   └── default.cfg            # Default configuration
├── src/                        # Source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Neural components
│   ├── dynamics/              # Neural dynamics (LIF neurons)
│   ├── plasticity/            # Plasticity rules (STDP, Hebbian, structural)
│   ├── development/           # Developmental system
│   ├── neuromodulation/       # Neuromodulators (dopamine, curiosity, novelty)
│   ├── memory/               # Memory systems
│   ├── prediction/            # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization (Phase 5)
│   └── agent/                # Agent systems (bridge to world)
│   └── world/                # World simulation
├── tests/                     # Test suite
│   ├── test_*.cpp             # Unit tests for each component
│   └── scripts/              # Test runner scripts
└── docs/                      # Scientific documentation
    ├── ARCHITECTURE.md       # System architecture
    ├── SCIENCE.md            # Scientific background
    ├── ROADMAP.md            # Development roadmap
    └── ...                   # Other documentation
```

## Current Phase Status

### Phase 1: Skeleton (Complete)
- Core types and configuration
- Neural interfaces

### Phase 2: Real Neural Computation (Complete)
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: World Interaction (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4: Cognitive Integration (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance Optimizations (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration (Complete)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Key Features

### Neural Architecture
- **LIF Neurons**: Biologically-inspired Leaky Integrate-and-Fire neurons
- **Event-Driven Processing**: Efficient spike propagation with synaptic delays
- **Multiple Plasticity Rules**: STDP, Hebbian learning, structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty signals

### Memory Systems
- **Working Memory**: Active maintenance with competitive selection
- **Episodic Memory**: Experience storage with replay capability
- **Associative Memory**: Pattern association and memory recall
- **Memory Consolidation**: Integration of important experiences

### Cognitive Capabilities
- **Prediction System**: Anticipation of future states
- **Neural Planning**: Action sequence evaluation
- **Concept Formation**: Abstraction from experience patterns
- **Attention**: Competitive selection of neural representations

### Developmental Process
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult
- **Structural Plasticity**: Synaptogenesis and pruning modulated by age
- **Plasticity Modulation**: Neuromodulation affects learning rates

### Integration Architecture
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

## Build Instructions

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Options

```bash
# Standard release build (recommended)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_test` | Executable | Unit test suite |

## Running NLM

### Main Executable

```bash
# Run main simulation
./nlm

# With custom configuration
./nlm --config myconfig.cfg
```

### Phase Demos

```bash
# Phase 3 demo (world interaction)
./nlm_phase3_demo

# Phase 4 demo (cognitive integration)
./nlm_phase4_demo

# Phase 6 demo (full integration - recommended)
./nlm_phase6_demo
```

### Tests

```bash
# Run unit tests
./nlm_test

# Or with CMake testing
ctest --output-on-failure
```

## Python Bindings (pynlm)

### Installation

```bash
# Install from source
pip install .

# Or development mode
pip install -e .
```

### Quick Usage Examples

#### Basic Brain

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

#### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Complete API Reference

#### Brain Class

**Creation:**
```python
brain = pynlm.createBrain(config)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

**Initialization:**
```python
brain.initialize()
```

**Simulation:**
```python
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)      # Run step with timestamp
brain.reset()                      # Reset brain state
```

**Input/Output:**
```python
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()           # Get motor action
```

**Statistics:**
```python
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
```

**State Management:**
```python
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state
```

**Regions:**
```python
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

**Creation:**
```python
config = pynlm.createDefaultConfig()
```

**Query/Modify:**
```python
if config.has("brain.neuron_count"):
    keys = config.getKeys()

config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
```

**File I/O:**
```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

**Creation:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Initialization:**
```python
agent.initialize(world)
```

**Sensory Processing:**
```python
agent.processSensoryInput(percept)
```

**Motor Decoding:**
```python
motor_cmd = agent.decodeMotorCommand()
```

**Neuromodulation:**
```python
agent.applyRewardModulation(reward, predicted_reward)
```

**Development:**
```python
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

**Subsystem Enable/Disable:**
```python
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
```

**Statistics:**
```python
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

**Creation:**
```python
world = pynlm.createSimpleWorld()
```

**Configuration:**
```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

**Control:**
```python
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

**Agent Interaction:**
```python
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)
```

**State Access:**
```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

**World Objects:**
```python
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

**Properties:**
```python
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Python Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

#### World Object Types
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete usage documentation
- [easy_usage.md](easy_usage.md) - Quick start guide for beginners
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

## License

MIT

## Authors

Research project — See docs for scientific background.
