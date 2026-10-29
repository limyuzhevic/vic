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

**PHASE 2: REAL NEURAL COMPUTATION**

Phase 2 implements real spiking neural computation with:

- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **STDP and Hebbian plasticity**
- **Structural plasticity (synaptogenesis/pruning)**

### Phase 2 Achievements

#### Core Neural Computation
- Real LIF neuron dynamics with membrane potentials and spike generation
- Event-driven spike propagation through synaptic connections
- Spike timing-dependent plasticity (STDP) for learning
- Hebbian learning for co-activation strengthening
- Structural plasticity for synapse formation and elimination

#### Enhanced Configuration System
- Advanced configuration with type conversion and validation
- Comprehensive parameter support for all neural systems
- Command-line argument support with key=value format
- File-based configuration with JSON-like key=value pairs
- Default configuration templates for different use cases

#### Python Bindings
- Comprehensive Python API with advanced features
- Enhanced type conversion and error handling
- Pythonic design with convenient methods
- Performance monitoring and benchmarking tools
- Complete integration with C++ neural simulation framework

#### Command-Line Interface
- Extended command-line options for all configuration parameters
- Support for key=value format for easy configuration
- Help and version information
- Test mode options for different simulation scenarios
- Enhanced logging control

### Build Instructions

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

# Run the main simulation executable
./nlm

# Or run with specific options
./nlm --neuron_count 1000 --region_count 2 --test_mode basic
```

### Running the Main Simulation

The main executable (`nlm`) runs a comprehensive demonstration of Phase 2 capabilities:

```bash
# Run with default configuration
./nlm

# Run with custom configuration
./nlm --config myconfig.cfg

# Run with specific parameters
./nlm --neuron_count 2000 --region_count 3 --connection_prob 0.2

# Enable debug logging
./nlm --log_level DEBUG

# Run specific test mode
./nlm --test_mode basic
./nlm --test_mode plasticity
./nlm --test_mode stdp
./nlm --test_mode all
```

### Python Usage

The enhanced Python bindings provide comprehensive access to the NLM framework:

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")

# Example with advanced configuration
config = pynlm.createDefaultConfig()
config.setString("brain.neuron_count", "2000")
config.setFloat("brain.connection_probability", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world setup
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Configuration Options

The enhanced configuration system supports extensive parameters organized by category:

#### Brain Configuration
- `brain.neuron_count`: Total number of neurons
- `brain.region_count`: Number of brain regions
- `brain.connection_probability`: Connectivity density
- `brain.initial_weight_mean`: Mean initial synaptic weight
- `brain.initial_weight_std`: Standard deviation of initial weights
- `brain.v_thresh`: Neuron threshold potential
- `brain.v_rest`: Resting potential
- `brain.v_reset`: Reset potential after spike
- `brain.tau_mem`: Membrane time constant
- `brain.tau_ref`: Refractory period

#### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP LTP time constant
- `plasticity.stdp.tau_minus`: STDP LTD time constant
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity
- `plasticity.stdp.ltp_weight`: STDP LTP weight
- `plasticity.stdp.ltd_weight`: STDP LTD weight

#### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection

#### Development Configuration
- `development.synaptogenesis_rate`: Synaptogenesis rate
- `development.pruning_rate`: Pruning rate
- `development.maturation_rate`: Maturation rate

#### Environment Configuration
- `environment.width`: Environment width
- `environment.height`: Environment height
- `environment.vision_width`: Vision width
- `environment.vision_height`: Vision height

### Test Modes

The enhanced command-line interface supports multiple test modes:

#### `--test_mode basic`
- Tests basic neural connectivity
- Injects current into neurons and verifies spike propagation
- Checks neuron dynamics and membrane potential changes

#### `--test_mode plasticity`
- Tests plasticity learning experiment
- Demonstrates synaptic weight changes through Hebbian and STDP learning
- Shows measurable learning effects from experience

#### `--test_mode stdp`
- Tests STDP verification specifically
- Creates correlated pre-post activity to trigger long-term potentiation
- Verifies pre-before-post produces potentiation

#### `--test_mode all`
- Runs all tests sequentially
- Comprehensive integration test of all neural systems
- Demonstrates complete Phase 2 functionality

### Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   └── LIMITATIONS.md
├── src/                   # Source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
├── python/                # Python bindings
│   └── bindings.cpp       # Enhanced Python bindings
├── configs/               # Configuration files
│   └── default.cfg        # Default configuration
├── tests/                 # Test suite
│   ├── test_*.cpp         # C++ unit tests
└── build/                 # Build directory (generated)
```

### Scientific Background

NLM is an experimental artificial developmental brain project. The Phase 2 implementation focuses on:

1. **Real Neural Computation**: Implementing biologically inspired neuron models that can actually produce spikes and learn from experience
2. **Learning Mechanisms**: Demonstrating multiple learning rules (STDP, Hebbian, structural plasticity) that work together
3. **Integration**: Showing how different neural systems can work together as a cohesive brain
4. **Development**: Exploring how neural systems can change and adapt over time

### Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 2 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 2 DOES investigate:
- Whether neural dynamics can support learning
- Whether plasticity rules can produce meaningful changes
- Whether networks can show activity-dependent synaptic modification
- Whether simple learning can emerge from spiking neurons

### License

MIT

### Authors

Research project — See docs for scientific background.

