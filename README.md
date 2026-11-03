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
## Getting Started

### Quick Start for Beginners

If you want to start using NLM quickly, follow these steps:

1. **Install the Python bindings** (recommended for beginners):
```bash
pip install pynlm
```

2. **Create your first brain**:
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
brain.step(0)

print(f"Brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

3. **Run a complete simulation**:
```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### Advanced Usage

For more advanced usage, refer to the `HOW_TO_USE.md` and `easy_usage.md` files.

## Project Purpose

NLM (熙然) is an experimental artificial developmental brain project designed to create neural systems that:

- **Begin in a primitive state** and **develop** through experience
- **Learn from interaction** with their environment
- **Exhibit emergent cognition** from neural dynamics
- **Integrate multiple systems** (memory, prediction, attention, neuromodulation)

### Key Design Principles

1. **Brain-inspired**: Uses spiking neurons and synapses, not deep learning
2. **Developmental**: Starts simple and matures over time
3. **Experience-based**: No pre-trained knowledge, learns from scratch
4. **Modular**: Clear separation between different brain systems
5. **Deterministic**: Reproducible experiments via explicit random seeds

### What NLM Is NOT

- A transformer or large language model
- A chatbot or conversational AI
- A traditional deep learning system
- A pretrained or pre-trained system

## Current Status (Phase 6: Final Integration)

NLM is currently in **Phase 6 - Final Integration**. This phase integrates all previous systems into a coherent artificial brain:

### ✅ Complete Systems

- **Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Prediction Systems**: Temporal sequence learning and action prediction
- **Cognitive Systems**: Attention, planning, self-model, social learning
- **Development Systems**: Stage-based development and maturation
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error

### 🔄 Active Development

Phase 6 focuses on:
- Integrating memory with neural dynamics
- Coordinating neuromodulation effects
- Making prediction a central organizing principle
- Demonstrating emergent cognition

## Building and Running

### Building from Source

```bash
# Create build directory and compile
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Demos

```bash
# Run different phase demonstrations
cd build
./nlm                 # Phase 2: Real neural computation
./nlm_phase3_demo     # Phase 3: World interaction
./nlm_phase4_demo     # Phase 4: Emerging cognition
./nlm_phase6_demo     # Phase 6: Final integration test
./nlm_benchmark       # Performance benchmarks
```

### Installing Python Bindings

```bash
# Install from source (recommended for development)
pip install .

# Or install in development mode
pip install -e .

# After installation, you can use NLM in Python:
```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

## Project Structure

```
NLM/
├── src/                    # C++ source code
│   ├── core/              # Core utilities (Config, Logger, Random, etc.)
│   ├── brain/             # Neural components (Neurons, Synapses, Brain)
│   ├── dynamics/          # Neural dynamics (LIF, SpikeSystem)
│   ├── plasticity/        # Plasticity rules (STDP, Hebbian, etc.)
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators (Dopamine, Curiosity, etc.)
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   └── experiments/       # Experiment framework
├── python/                 # Python bindings
│   ├── bindings.cpp       # Pybind11 bindings
│   └── CMakeLists.txt     # Python bindings build
├── docs/                   # Documentation
│   ├── README.md          # This file
│   ├── HOW_TO_USE.md      # Technical usage guide
│   ├── easy_usage.md      # Beginner-friendly guide
│   ├── ARCHITECTURE.md    # System architecture
│   └── ...               # More documentation
├── tests/                  # Unit tests
│   ├── test_brain.cpp     # Brain tests
│   ├── test_config.cpp    # Config tests
│   ├── test_neuron.cpp    # Neuron tests
│   └── ...               # More tests
└── configs/               # Configuration files
```

## Scientific Goals

NLM investigates whether artificial neural systems can:

- **Integrate multiple memory systems** with neural dynamics
- **Use coordinated neuromodulation** for adaptive behavior
- **Employ prediction** as an organizing principle for cognition
- **Show developmental effects** on learning and plasticity
- **Exhibit replay and consolidation** mechanisms
- **Demonstrate emergent cognitive abilities** from simple neural architectures

### Research Questions

1. Can memory systems effectively work with spiking neural dynamics?
2. How can neuromodulation coordinate different plasticity mechanisms?
3. Can prediction become a central principle for organizing cognitive function?
4. Does developmental progression affect learning capabilities?
5. Can replay mechanisms strengthen important memories?
6. Do integrated brain systems produce emergent cognitive abilities?

## Learning Capabilities

### 1. Sensory Processing
- **Vision**: Visual input processing
- **Audio**: Sound input processing  
- **Proprioception**: Body position awareness
- **Internal Signals**: Internal state monitoring

### 2. Memory Systems
- **Working Memory**: Transient active information maintenance
- **Episodic Memory**: Experience encoding and storage
- **Associative Memory**: Pattern-based associations
- **Concept Formation**: Pattern discovery without labels

### 3. Cognitive Functions
- **Attention**: Competitive selection of information
- **Planning**: Multi-step action prediction
- **Self-Model**: Sensorimotor self-awareness
- **Social Learning**: Observation and imitation

### 4. Neuromodulation
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signal for prediction updates

## Configuration

NLM uses a flexible configuration system for:

### Brain Configuration
- **neuron_count**: Number of neurons (default: 1000)
- **region_count**: Number of brain regions (default: 1)
- **connection_probability**: Synaptic connection probability (default: 0.1)

### Plasticity Configuration
- **stdp_learning_rate**: STDP learning rate (default: 0.01)
- **stdp_tau**: STDP time constant (default: 20.0)
- **synaptogenesis_rate**: New synapse formation rate
- **pruning_rate**: Weak synapse removal rate

### Neuromodulation Configuration
- **dopamine_scale**: Dopamine modulation strength (default: 1.0)
- **curiosity_enabled**: Enable curiosity-driven exploration (default: true)
- **novelty_enabled**: Enable novelty detection (default: true)

### Simulation Configuration
- **simulation_timestep**: Simulation time step (default: 0.001)
- **replay_interval**: Memory replay interval (default: 100)
- **consolidation_interval**: Memory consolidation interval (default: 1000)

Configuration can be:
- **Loaded from files** (JSON/YAML format)
- **Set via command line** (--key=value)
- **Modified at runtime** (programmatic)

## Performance

NLM is designed for:

### Scale
- **Large networks**: 1000+ neurons
- **Complex connectivity**: Event-driven spike propagation
- **Efficient simulation**: C++20 with SIMD support

### Architecture
- **Data-oriented design**: Cache-friendly memory access
- **Event-driven processing**: Efficient spike handling
- **Modular components**: Clear separation of concerns

### Optimization
- **Parallel processing**: Multi-core support
- **Memory pools**: Efficient memory allocation
- **SIMD vectorization**: Parallel neuron computation
- **Checkpointing**: State persistence and recovery

## Testing

The project includes comprehensive unit tests:

```bash
# Build tests
cd build
make nlm_test

# Run all tests
./nlm_test
```

### Test Coverage

- **test_brain.cpp**: Brain initialization, stepping, memory systems
- **test_config.cpp**: Configuration management and validation
- **test_neuron.cpp**: Neuron dynamics and plasticity
- **test_stdp.cpp**: Spike-timing-dependent plasticity
- **test_synapse.cpp**: Synaptic transmission and dynamics

## Contributing

### Development Guidelines

1. **Follow the coding style**: C++20 with clear separation of concerns
2. **Write unit tests**: All new code should have comprehensive tests
3. **Document clearly**: Comments and documentation for complex logic
4. **Maintain modularity**: Keep components focused and testable
5. **Ensure determinism**: Use explicit random seeds for reproducible experiments

### Building for Development

```bash
# Debug build with assertions
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run tests
./nlm_test
```

## Contact and Support

### Documentation
- **Main documentation**: README.md, HOW_TO_USE.md, easy_usage.md
- **Architecture details**: docs/ARCHITECTURE.md
- **Scientific background**: docs/SCIENCE.md

### Community
- **Issues and feature requests**: GitHub repository
- **Discussions**: Project forum or GitHub discussions
- **Collaborations**: Research collaborations and academic partnerships

### Technical Support
- **Build issues**: CMake configuration and compilation
- **Runtime issues**: Simulation and learning problems
- **API questions**: Python bindings and C++ interfaces

## License

NLM is released under the **MIT License**. This permits:

- **Commercial use**: Use in commercial applications
- **Modification**: Modify and extend the code
- **Distribution**: Share with others
- **Private use**: Use without disclosure

See the LICENSE file for complete details.

## Future Directions

### Phase 7: Enhanced Capabilities

Future development may include:

- **More complex memory systems**: Hierarchical memory architecture
- **Advanced neuromodulation**: Additional neuromodulators and mechanisms
- **Sophisticated cognition**: Language processing and reasoning
- **Social complexity**: Multi-agent interaction and communication
- **Developmental complexity**: Multiple developmental stages

### Research Applications

NLM can be used for:

- **Brain-inspired AI**: Alternative to deep learning
- **Cognitive modeling**: Testing cognitive theories
- **Neuroscientific research**: Studying neural mechanisms
- **Educational tools**: Neural network simulation and visualization

---

**Ready to start?** Install the Python bindings and create your first brain!

**Need help?** Check the documentation files or project repository for more information.

**Want to contribute?** Fork the repository and add your improvements!

---

*Last updated: 2026-11-03*
*Version: Phase 6 (Final Integration)*
*Status: Active development*


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
