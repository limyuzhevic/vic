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

### C++ Version

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Version

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install the Python library
pip install .

# Or install in development mode
pip install -e .
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Examples

The NLM Python library provides a simple and intuitive API for working with neural networks. Here are some examples:

#### Basic Brain Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

#### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
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

#### Working Memory Example

```python
import pynlm

# Create brain with memory support
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active traces: {working_memory.getActiveTraces()}")

# Run simulation to fill working memory
for step in range(500):
    brain.step(step)

# Check memory statistics
if working_memory:
    print(f"Working memory after simulation: {working_memory.getActiveTraces()} active traces")
```

#### Neuromodulation Example

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with learning
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check neuromodulation levels
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    if step % 50 == 0:
        print(f"Step {step}: Dopamine={dopamine:.3f}, Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")

print("Neuromodulation simulation complete!")
```

#### Configuration Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query existing configuration options
print("Configuration options:")
for key in config.getKeys():
    print(f"  {key}")

# Modify configuration parameters
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)

# Create brain with modified configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Configured brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
```

#### Episode and Memory Management

```python
import pynlm

# Create brain with episodic memory support
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to create episodic memories
for step in range(1000):
    brain.step(step)

# Access episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episode_count = episodic_memory.getEpisodeCount()
    print(f"Created {episode_count} episodic memories")
    
    # Get episodes for replay
    episodes_to_replay = episodic_memory.getEpisodesForReplay(5)
    print(f"Available for replay: {len(episodes_to_replay)} episodes")

# Save brain state for persistence
brain.save("nlm_checkpoint.bin")
print("Brain state saved to checkpoint")

# Load brain state (for example, after restart)
# brain.load("nlm_checkpoint.bin")
```

#### Neural Development Example

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Run long-term simulation to observe development
for step in range(5000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.updateDevelopment(0.1)
    
    # Monitor developmental stage
    if step % 1000 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Developmental stage = {stage}")
        print(f"  Plasticity modifier: {agent.getBrain().getConfig().getOr<float>('development.plasticity_modifier', 1.0)}")

print("Developmental simulation complete!")
```

## Python API Reference

### Core Classes

#### `pynlm.createDefaultConfig()`
Create a default configuration object.

#### `pynlm.createBrain(config)`
Create a neural brain with the given configuration.

#### `pynlm.createAgentBrain(brain)`
Create an agent interface for the brain.

#### `pynlm.createSimpleWorld()`
Create a simple 2D world for simulation.

### Brain Methods

- `brain.initialize()` - Initialize the brain
- `brain.step(step_number)` - Run one simulation step
- `brain.reset()` - Reset brain state
- `brain.receiveSensoryInput(input)` - Inject sensory data
- `brain.produceAction()` - Get motor action
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getFiringNeuronCount()` - Get firing neuron count
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getDevelopmentalStage()` - Get current developmental stage

### AgentBrain Methods

- `agent.initialize(world)` - Initialize agent with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command from brain activity
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error
- `agent.getNeuromodulationLevel()` - Get neuromodulation level
- `agent.reset()` - Reset agent for new episode

### World Methods

- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset world to initial state
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.update(timestep)` - Update world by given timestep
- `world.applyMotorCommand(cmd, time)` - Apply motor command to world
- `world.getSensoryPercept()` - Get sensory percept from world
- `world.getAgentBody()` - Get agent body state
- `world.addObject(obj)` - Add object to world
- `world.removeObject(x, y)` - Remove object at position
- `world.isValidPosition(x, y)` - Check if position is valid

### Configuration Methods

- `config.set(key, value)` - Set configuration value
- `config.has(key)` - Check if configuration key exists
- `config.getKeys()` - Get all configuration keys
- `config.loadFromFile(filepath)` - Load configuration from file
- `config.saveToFile(filepath)` - Save configuration to file
- `config.summary()` - Get configuration summary

## Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
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
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running Tests

### Unit Tests

```bash
# After building with CMake
ctest --output-on-failure

# Or using Python
pytest tests/
```

### Phase Demos

```bash
# Phase 3 demo
./nlm_phase3_demo

# Phase 4 demo
./nlm_phase4_demo

# Phase 6 demo (integration test)
./nlm_phase6_demo
```

## Python Package Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Performance Benchmarking

The NLM project includes performance benchmarks:

```bash
# Build and run benchmark
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./nlm_benchmark
```

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── pyproject.toml
├── docs/                  # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                  # Source code
│   ├── core/            # Core utilities
│   ├── brain/           # Neural components (integrated)
│   ├── dynamics/        # Neural dynamics
│   ├── plasticity/      # Plasticity rules
│   ├── development/     # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/         # Memory systems (integrated)
│   ├── prediction/      # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/         # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── python/              # Python bindings
│   ├── bindings.cpp
│   └── CMakeLists.txt
├── tests/              # Test suite
├── configs/            # Configuration files
└── docs/               # Documentation
```

## Further Reading

- [easy_usage.md](easy_usage.md) - Beginner-friendly usage guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive usage documentation
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [CHANGELOG.md](CHANGELOG.md) - Version history and changes

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
