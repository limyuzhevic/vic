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

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Optional**: `git` for checking out submodules

### Quick Start

```bash
# Create build directory and configure
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build everything (recommended for development)
make -j$(nproc)

# Build just the core library (fastest)
# make nlm_core

# Build just the main demo
# make nlm_phase6_demo
```

### Alternative Python Installation (for development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install the Python package (builds and installs)
pip install .

# Or install in development mode (links source)
pip install -e .
```

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
| `pynlm` | Python Module | Python bindings |

### Running Tests

```bash
# Run C++ unit tests
ctest --output-on-failure

# Or run specific test
./nlm_test
```

### Running Demos

```bash
# Phase 6 Integration Demo (main demo)
./nlm_phase6_demo

# Phase 3 Demo (neural dynamics only)
./nlm_phase3_demo

# Phase 4 Demo (cognitive capabilities)
./nlm_phase4_demo
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Usage

```python
import pynlm

# Create a simple virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Main build system
├── README.md                         # This file
├── docs/                             # Technical documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   └── PHASE6_FINAL_AUDIT.md        # Phase 6 integration audit
├── python/                           # Python bindings
│   ├── bindings.cpp                 # Core bindings
│   └── CMakeLists.txt              # Python build
├── src/                              # C++ source code
│   ├── core/                       # Core utilities
│   ├── brain/                      # Neural computation
│   ├── environment/                # Environment simulation
│   ├── agent/                      # Agent interface
│   └── ... (additional layers)
├── tests/                           # Unit tests
├── configs/                        # Configuration templates
└── build/                          # Build output (generated)
```

## Installation

### From Source (Recommended)

```bash
# Build the C++ libraries
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Install to system
make install

# Install Python package
pip install .
```

### Quick Development Setup

```bash
# Clone the repository
cd /path/to/your/projects
git clone https://github.com/nlm-project/nlm.git

# Set up build directory
cd nlm
mkdir -p build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Install Python bindings
pip install -e .
```

## Usage Examples

### Basic Neural Simulation (Python)

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    
print(f"Simulation complete. {brain.getTotalNeuronCount()} neurons simulated.")
```

### Complete Agent Simulation (Python)

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Complete agent simulation with brain and world interaction."""
    
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
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Configuration System

The NLM configuration system supports hierarchical JSON configuration:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "decay_rate": 0.05
        }
    }
}
```

Load configuration:

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Modify and save
config.set("brain.neuron_count", 2000)
config.saveToFile("modified_config.json")
```

### Brain Class Reference

#### Core Operations
- `createBrain(config)` - Create a new brain instance
- `brain.initialize()` - Initialize the brain with configuration
- `brain.step(step_number)` - Run one simulation step
- `brain.step(step_number, time)` - Run step with timestamp
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` / `brain.load(filepath)` - Persistent storage

#### Statistics and Monitoring
- `brain.getTotalNeuronCount()` - Total neurons
- `brain.getTotalSynapseCount()` - Total synapses  
- `brain.getFiringNeuronCount()` - Currently firing
- `brain.getAverageFiringRate()` - Average firing rate
- `brain.getExcitationInhibitionRatio()` - E/I balance
- `brain.getDevelopmentalStage()` - Current developmental stage

#### Input/Output
- `brain.receiveSensoryInput(sensory_input)` - Inject sensory data
- `brain.produceAction()` - Get motor action

#### Regions and Organization
- `brain.addRegion(name)` - Add neural region
- `brain.getRegion(id)` - Get region by ID
- `brain.getTotalNeuronCount()` - Neurons across all regions
- `brain.getTotalSynapseCount()` - Synapses across all regions

### AgentBrain Class Reference

#### Sensory-Motor Interface
- `createAgentBrain(brain)` - Create agent interface
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory data
- `agent.decodeMotorCommand()` - Get action from brain

#### Neuromodulation
- `agent.applyRewardModulation(reward, predicted_reward)` - Reward-based learning
- `agent.enableRewardModulation(bool)` - Enable/disable reward learning
- `agent.enableCuriosity(bool)` - Enable exploration
- `agent.enableDevelopment(bool)` - Enable developmental changes

#### State Access
- `agent.getNeuromodulationLevel()` - Current neuromodulation level
- `agent.getCuriosityLevel()` - Curiosity/exploration level
- `agent.getNoveltyLevel()` - Novelty detection level
- `agent.getPredictionError()` - Prediction error signals
- `agent.getDevelopmentalStage()` - Current developmental stage

### SimpleWorld Class Reference

#### World Management
- `createSimpleWorld()` - Create simulation world
- `world.configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `world.reset()` - Reset to initial state
- `world.update(timestep)` - Advance simulation time

#### Agent Interaction
- `world.setAgentStart(x, y)` - Set starting position
- `world.applyMotorCommand(cmd, current_time)` - Execute action
- `world.getSensoryPercept()` - Get agent observations
- `world.getAgentBody()` - Get agent physical state

#### World Objects
- `world.addObject(object)` - Add resource/hazard/walls
- `world.removeObject(x, y)` - Remove object
- `world.isValidPosition(x, y)` - Check if position is free

### Enumerations

#### Neuron Types
- `pynlm.NeuronType.Excitatory` - Excitatory neurons
- `pynlm.NeuronType.Inhibitory` - Inhibitory neurons
- `pynlm.NeuronType.Sensory` - Sensory neurons
- `pynlm.NeuronType.Motor` - Motor neurons
- `pynlm.NeuronType.Modulatory` - Neuromodulatory neurons
- `pynlm.NeuronType.Internal` - Internal state neurons

#### Developmental Stages
- `pynlm.DevelopmentalStage.Initial` - High plasticity (0-60s)
- `pynlm.DevelopmentalStage.CriticalPeriod` - Sensitive period (60-300s)
- `pynlm.DevelopmentalStage.Maturation` - Circuit refinement (300-900s)
- `pynlm.DevelopmentalStage.Adult` - Stable adult (900s+)

#### Action Types
- `pynlm.ActionType.MoveForward`, `MoveBackward`
- `pynlm.ActionType.TurnLeft`, `TurnRight`
- `pynlm.ActionType.LookLeft`, `LookRight`
- `pynlm.ActionType.Interact`, `Wait`
- `pynlm.ActionType.Eat`, `Drink`, `Rest`

### Troubleshooting

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

#### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Common Errors and Solutions

1. **"Brain not initialized"**: Always call `brain.initialize()` before first `brain.step()`
2. **"Invalid configuration"**: Check JSON syntax and required parameters
3. **"Out of memory"**: Reduce `brain.neuron_count` or increase system memory
4. **"No visual output"**: Check visualization setup in experiments

### Performance Tips

- Use `brain.step(step, time)` for time-aware simulations
- Enable `brain.produceAction()` for action selection
- Use `brain.save()` and `brain.load()` for persistent states
- Monitor `getAverageFiringRate()` for efficiency
- Use `getExcitationInhibitionRatio()` for balance checking

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Integration audit results

## License

MIT

## Authors

Research project — See docs for scientific background.

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
