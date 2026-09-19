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

## Configuration System

NLM includes a powerful and flexible configuration system that supports:

### Key Features

- **JSON/YAML Configuration Files**: Load and save configurations in standard formats with full nested structure support
- **Configuration Categories**: Organize parameters into logical groups (Brain, Dynamics, Plasticity, Development, Neuromodulation, Memory, Cognition, Sensory, Motor, Environment)
- **Configuration Validation**: Validate configuration values with custom validation functions
- **Default Values**: Set and use default configuration values with validation
- **Metadata Support**: Store descriptions and documentation for each configuration parameter

### Configuration Examples

**JSON Configuration**:
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.05,
    "v_thresh": -50.0,
    "v_rest": -70.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.0005
    }
  },
  "development": {
    "critical_period": 100,
    "structural_plasticity_rate": 0.01
  }
}
```

**YAML Configuration**:
```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.05
  v_thresh: -50.0
  v_rest: -70.0
plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.0005
development:
  critical_period: 100
  structural_plasticity_rate: 0.01
```

### Python Configuration Examples

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("config.json")

# Set individual values with validation
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)

# Save configuration
config.saveToFile("my_config.json", "json")

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

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

### Python Examples

**Basic Usage**:
```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

**Complete Agent Example**:
```python
import pynlm

# Setup complete agent with world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
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
├── python/                        # Python bindings
│   ├── bindings.cpp
│   └── CMakeLists.txt
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
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/                     # Test suite
├── configs/                   # Configuration files
└── docs/                      # Documentation
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

## Advanced Configuration Options

The configuration system supports advanced features:

### Configuration Categories
- **Brain**: Neuron properties, network topology
- **Dynamics**: Neural simulation parameters
- **Plasticity**: Learning rules and mechanisms
- **Development**: Growth and maturation processes
- **Neuromodulation**: Chemical regulation systems
- **Memory**: Storage and retrieval mechanisms
- **Cognition**: Thought and reasoning processes
- **Sensory**: Perception and input processing
- **Motor**: Action and output systems
- **Environment**: Simulation and interaction parameters

### Validation Functions
Custom validation for configuration values:

```cpp
// Range validation
auto validator = config_utils::validateIntRange(1, 10000);
auto doubleValidator = config_utils::validateDoubleRange(0.0, 1.0);

// Length validation
auto stringValidator = config_utils::validateStringLength(1, 100);

// Positive validation
auto positiveValidator = config_utils::validatePositive();
```

### Configuration Options
Each configuration entry can have:
- Description: Human-readable explanation
- Validator: Function to validate the value
- Default: Fallback value if not specified
- Category: Logical grouping
- Required: Whether the parameter must be set

## License

MIT

## Authors

Research project — See docs for scientific background.