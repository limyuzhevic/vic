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

### Build Steps

```bash
# Navigate to the project directory
mkdir build
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
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase6` | Static Library | Phase 6 integration experiments |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |

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

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Other Demos

- **Phase 3 Demo**: `./nlm_phase3_demo` - Basic world interaction
- **Phase 4 Demo**: `./nlm_phase4_demo` - Emerging cognition
- **Main Executable**: `./nlm` - Basic neural computation demo

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Legacy documentation (see README for consolidated docs)
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
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
├── tests/              # Unit tests
└── configs/            # Configuration files
    ├── default.cfg
    └── (JSON/YAML configs for advanced usage)
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

## Configuration System

### File-Based Configuration

NLM supports multiple configuration file formats:

#### Legacy Format (configs/default.cfg)
```
# Key = value format with comments
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001
```

#### JSON Configuration (recommended)
```json
{
    "random_seed": 42,
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "plasticity_learning_rate": 0.01,
    "connection_probability": 0.1
}
```

### Configuration Commands

#### Load Configuration
```bash
./nlm --config my_config.json
./nlm --config my_config.cfg
```

#### Command Line Overrides
```bash
./nlm --neuron_count=5000 --simulation_timestep=0.0005
```

#### Save Configuration
```bash
// Programmatically
config.saveToFile("my_config.json")

// From command line (future feature)
./nlm --export_config my_config.json
```

### Configuration Validation

The configuration system includes validation for:
- Parameter bounds and ranges
- Consistency between related parameters
- File format validation

## Advanced Features

### Phase 6 Integration Features

1. **Multi-System Coordination**: All brain systems (memory, prediction, cognition, neuromodulation) work together
2. **Experience Replay**: Episodic memory replay for consolidation
3. **Developmental Trajectory**: Brain matures through distinct stages
4. **Checkpoint/Restart**: Save and resume complex simulations
5. **Real-Time Monitoring**: Track brain state and performance metrics

### Performance Features

1. **Event-Driven Architecture**: Efficient spike processing
2. **Memory Pools**: Reduce allocation overhead
3. **SIMD Vectorization**: Fast parallel computation
4. **Parallel Processing**: Multi-core support

### Extensibility

1. **Plugin Architecture**: Add new brain components
2. **Custom Plasticity Rules**: Implement novel learning algorithms
3. **Environment Extensions**: Create custom simulation environments
4. **New Data Types**: Support for advanced neural representations

## Python Bindings

NLM includes comprehensive Python bindings via pybind11:

```python
import pynlm

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with world interaction
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Usage Examples

### Basic Neural Simulation
```python
import pynlm

# Simplest possible brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
```

### Complete Agent Simulation
```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = world.computeReward(agent.getBrain().getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0)
```

### Configuration Examples
```python
# Example 1: Fast simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)      # Smaller brain
config.set("simulation_timestep", 0.01)    # Larger time step
config.set("max_simulation_steps", 1000)   # Fewer steps

# Example 2: Complex learning
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
```

## Testing

### Running Tests

```bash
# Build with tests
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Run specific test
./nlm_test --test_config
```

### Test Coverage

- **Unit Tests**: Core functionality (config, neurons, synapses, memory)
- **Integration Tests**: System coordination
- **Performance Tests**: Benchmarking
- **Phase Tests**: Each phase verification

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
- Verify configuration parameters are valid

## Performance Optimization

### Profiling

```bash
# Build with profiling
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_PROFILE=ON
make -j$(nproc)

# Run with profiling
./nlm_phase6_demo --profile
```

### Memory Usage

- Use smaller brain configurations for development
- Enable memory pool optimizations
- Monitor for memory leaks during long simulations

## Future Directions

### Phase 7: Advanced Cognition

1. **Abstract Reasoning**: Symbolic logic integration
2. **Language Processing**: Natural language understanding
3. **Transfer Learning**: Knowledge across domains
4. **Meta-Learning**: Learning how to learn

### Phase 8: Biological Realism

1. **Detailed Neuron Models**: Hodgkin-Huxley dynamics
2. **Regional Specialization**: Cortical column models
3. **Neurovascular Coupling**: Blood flow regulation
4. **Genetic Factors**: Epigenetic influences

### Applications

1. **Neurological Research**: Modeling brain disorders
2. **AI Safety**: Understanding emergent behavior
3. **Cognitive Architecture**: Brain-inspired AI systems
4. **Education**: Adaptive learning systems

## License

MIT

## Authors

Research project — See docs for scientific background.

## Documentation

### Core Documentation
- **README.md**: This comprehensive guide
- **HOW_TO_USE.md**: Detailed Python API usage
- **easy_usage.md**: Beginner-friendly guide

### Scientific Documentation
- **docs/ARCHITECTURE.md**: System architecture overview
- **docs/SCIENCE.md**: Scientific background and theory
- **docs/PHASE6_FINAL_AUDIT.md**: Phase 6 audit results

### Advanced Documentation
- **docs/EXPERIMENTS.md**: Experiment descriptions and results
- **docs/LIMITATIONS.md**: Current limitations and challenges
- **docs/PERFORMANCE.md**: Performance characteristics and optimizations

For complete documentation, refer to the individual files in the docs/ directory.

## Quick Start

For beginners, use the easy usage guide or start with:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./nlm_phase6_demo
```

For Python users:
```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

That's it! You're now ready to use NLM.
