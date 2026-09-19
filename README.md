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
- **Python Support:** `agent.enableRewardModulation()`, `agent.enableCuriosity()`, etc.

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented
- **Python Support:** `agent.applyRewardModulation()`, `agent.getPredictionError()`

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action
- **Python Support:** Via `AgentBrain` class methods

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- **Python Support:** `agent.updateDevelopment()`, `agent.getDevelopmentalStage()`

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints
- **Python Support:** `brain.save()`, `brain.load()`

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

#### Python API Integration
- Complete Python bindings via pybind11
- Access to all C++ brain systems through Python API
- `pynlm.createBrain()`, `pynlm.createAgentBrain()` factory functions
- Full access to neuromodulation, development, and learning systems

## Python Quick Start

For Python users, the NLM package provides:

```python
import pynlm

# Create and initialize a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

## Python Usage

NLM provides complete Python bindings through the `pynlm` package, making it easy to use the full neural simulation framework from Python.

### Quick Installation

For development or quick setup:

```bash
# Install from source
cd /path/to/nlm
pip install .

# Or in development mode for faster iteration
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### Quick Start Examples

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Core Python APIs

| Function | Description |
|----------|-------------|
| `pynlm.createDefaultConfig()` | Creates a new configuration object |
| `pynlm.createBrain(config)` | Creates a neural brain instance |
| `pynlm.createSimpleWorld()` | Creates a simulated world environment |
| `pynlm.createAgentBrain(brain)` | Creates an agent brain interface |

### Agent Brain Features

The `AgentBrain` class provides access to advanced brain features:

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)      # Reward-based learning
agent.enableCuriosity(True)             # Exploration drive
agent.enableDevelopment(True)           # Brain maturation
agent.enableStructuralPlasticity(True)  # Connection growth

# Get current state
print(f"Curiosity: {agent.getCuriosityLevel():.2f}")
print(f"Novelty: {agent.getNoveltyLevel():.2f}")
print(f"Dev Stage: {agent.getDevelopmentalStage()}")
```

### Complete Agent Example

```python
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
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        world.update(timestep=0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

### Configuration Options

Configuration is flexible and can be modified before creating the brain:

```python
config = pynlm.createDefaultConfig()

# Set brain parameters
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Save/Load configuration
config.saveToFile("my_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
```

### Further Resources

- [`HOW_TO_USE.md`](HOW_TO_USE.md) - Comprehensive Python usage guide
- [`easy_usage.md`](easy_usage.md) - Beginner-friendly examples
- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) - System architecture
- [`docs/SCIENCE.md`](docs/SCIENCE.md) - Scientific background

### Building from Source

If you need to build the Python bindings or modify the C++ code:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

For more details on the C++ build system, see the [NLM C++ documentation](docs/ARCHITECTURE.md).

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
