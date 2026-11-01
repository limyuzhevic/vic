# NLM (熙然) - Improved Developer Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain simulator. This document provides comprehensive guidance for developers working on the NLM project.

## Project Structure

```
NLM/
├── src/
│   ├── core/              # Core utilities and base systems
│   ├── brain/             # Neural computation and brain architecture
│   ├── agent/             # Agent-brain interface
│   ├── world/             # Environment and interaction
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor control
│   ├── neuromodulation/    # Neuromodulation systems
│   ├── memory/            # Memory systems
│   ├── prediction/         # Prediction mechanisms
│   ├── development/        # Developmental systems
│   ├── plasticity/         # Plasticity rules
│   ├── dynamics/           # Neural dynamics
│   └── experiments/        # Experiment framework
├── tests/                 # Unit tests
├── configs/               # Configuration files
└── docs/                  # Documentation (this directory)
```

## Key Design Patterns

### 1. Event-Driven Architecture
NLM uses an event-driven approach for spike propagation and synaptic transmission, allowing for efficient simulation of large neural networks.

### 2. Observer Pattern
Spike events are managed through an event system that notifies interested components (plasticity rules, neuromodulation, etc.) of neural activity.

### 3. Command Pattern
Motor commands are implemented as action objects, allowing for flexible action selection and execution.

### 4. Strategy Pattern
Multiple plasticity mechanisms (STDP, Hebbian, structural) are implemented as interchangeable strategies.

## Development Philosophy

### Phase-Based Development
The project evolved through 6 phases:
- **Phase 1**: Core skeleton and types
- **Phase 2**: Real LIF neuron dynamics and basic plasticity
- **Phase 3**: World interaction and sensory-motor loop
- **Phase 4**: Advanced cognition and prediction
- **Phase 5**: Performance optimizations and checkpointing
- **Phase 6**: **Final integration** - all systems working coherently

### Integration Goals
Phase 6 achieved:
- ✅ Working memory, episodic memory, and associative memory systems
- ✅ Neuromodulation (dopamine, curiosity, novelty) affecting plasticity
- ✅ Prediction system integrated into brain loop
- ✅ Neural planner, concept formation, attention systems
- ✅ Developmental stages affecting plasticity rates
- ✅ Checkpoint save/load and brain state serialization
- ✅ Episodic memory replay and consolidation

## Configuration System

### Configuration File Format
NLM supports multiple configuration formats:

#### Simple Key=Value Format (Default)
```
# Simple key=value format
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.enable = true
neuromod.dopamine.scale = 1.0
```

#### JSON Format (Planned)
```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    }
  }
}
```

### Configuration Sources
1. **Default**: Hard-coded in source code
2. **File**: From `.nlm` or `.json` files
3. **Command Line**: `--key=value` or `-key value`
4. **Runtime**: Programmatically via `Config::set()`

### Configuration Access
```cpp
// Get a value (returns std::optional)
auto neuronCount = config->get<int64_t>("brain.neuron_count");
if (neuronCount) {
    std::cout << "Neuron count: " << *neuronCount << std::endl;
}

// Get with default value
int synapses = config->getOr<int64_t>("brain.synapse_density", 100);
std::cout << "Synapses: " << synapses << std::endl;
```

## Build System

### Quick Build Instructions

#### Linux/macOS
```bash
# Basic build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Install
make install
```

#### Python Bindings
```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install Python module (rebuilds C++ bindings)
pip install .

# Or build separately
python -m build
pip install dist/*.whl
```

### Build Outputs
| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_benchmark` | Executable | Performance benchmark |
| `pynlm` | Python Module | Python bindings |

### Build Options
```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build only Python bindings
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

## Testing

### Running Tests
```bash
# Run all tests
mkdir build
cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure
```

### Test Structure
- **Unit Tests**: C++ tests using the standard test framework
- **Integration Tests**: Phase-specific demos (phase3_demo, phase4_demo, etc.)
- **Python Tests**: Tests for Python bindings

### Test Coverage
Phase 6 tests verify:
- ✅ Memory systems integration
- ✅ Neuromodulation effects
- ✅ Prediction system functionality
- ✅ Cognitive mechanisms
- ✅ Developmental processes
- ✅ Checkpoint save/load
- ✅ Memory replay and consolidation

## Python API

### Basic Usage
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters (optional)
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Complete Simulation Loop
```python
def run_simulation(config, num_steps=1000):
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
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_simulation(pynlm.createDefaultConfig())
```

### Available Classes

#### Brain Class
- `createBrain(config)`: Creates a new brain instance
- `brain.initialize()`: Initializes the brain
- `brain.step(step_number)`: Runs one simulation step
- `brain.reset()`: Resets brain state
- `brain.save(filename)`: Saves brain state to checkpoint
- `brain.load(filename)`: Loads brain state from checkpoint

#### Config Class
- `createDefaultConfig()`: Creates configuration with default values
- `config.set(key, value)`: Sets configuration value
- `config.get(key)`: Gets configuration value
- `config.has(key)`: Checks if key exists

#### AgentBrain Class
- `createAgentBrain(brain)`: Creates agent interface
- `agent.initialize(world)`: Initializes agent with world
- `agent.processSensoryInput(percept)`: Processes sensory input
- `agent.decodeMotorCommand()`: Decodes motor command
- `agent.applyRewardModulation(reward, predicted)`: Applies reward learning

#### SimpleWorld Class
- `createSimpleWorld()`: Creates world instance
- `world.configure(width, height, visionWidth, visionHeight)`: Configures world
- `world.update(timestep)`: Updates world state
- `world.getSensoryPercept()`: Gets sensory perception

### Enumerations

#### NeuronType
- `Excitatory`: Standard excitatory neurons
- `Inhibitory`: Inhibitory neurons
- `Sensory`: Sensory input neurons
- `Motor`: Motor output neurons
- `Modulatory`: Neuromodulatory neurons
- `Internal`: Internal processing neurons

#### DevelopmentalStage
- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation phase
- `Adult`: Adult stage
- `Aging`: Aging phase

#### ActionType
- `MoveForward`: Move forward one unit
- `TurnLeft`: Rotate left
- `TurnRight`: Rotate right
- `Interact`: Interact with nearby object
- `Eat`: Consume food item
- `Wait`: Do nothing

## Command Line Interface

### Basic Commands
```bash
# Run Phase 2 demo (basic neural simulation)
./nlm

# Run Phase 3 demo (sensory-motor integration)
./nlm_phase3_demo

# Run Phase 4 demo (cognition and prediction)
./nlm_phase4_demo

# Run Phase 5 demo (performance and checkpointing)
./nlm_phase5_demo

# Run Phase 6 demo (complete integration test)
./nlm_phase6_demo

# Run performance benchmark
./nlm_benchmark
```

### Command Line Options
```bash
# Basic usage
./nlm [options]

# Options:
--help                    Show help message
--config file             Load configuration from file
--steps N                 Run for N steps (default: 1000)
--neuron-count N          Set number of neurons (default: 1000)
--synapse-density F       Set synapse density (default: 0.1)
--help                    Show this help message
```

### Configuration File Format (Command Line)
```bash
# Set multiple options
./nlm --config demo.config --steps 5000 --neuron-count 2000

# Individual settings
./nlm --synapse-density 0.05 --plasticity.stdp.enable=true
```

## Advanced Features

### Performance Optimization
Phase 5 includes several performance optimizations:
- **Memory pools**: Reduce memory allocation overhead
- **Event queues**: Efficient spike event management
- **SIMD vectorization**: Parallel processing of neural computations
- **Parallel processing**: Multi-core simulation support
- **Checkpoint system**: Save/load for long simulations

### Neuromodulation
Multiple neuromodulators integrate with plasticity:
- **Dopamine**: Reward prediction error signaling
- **Curiosity**: Novelty-driven exploration
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signal from prediction mismatches

### Memory Systems
Three complementary memory systems:
- **Working Memory**: Short-term active maintenance
- **Episodic Memory**: Experience recording and replay
- **Associative Memory**: Pattern associations and schema formation

### Development
Brain development affects plasticity rates:
- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive learning window
- **Maturation**: Slower, more stable learning
- **Adult**: Stable adult capabilities
- **Aging**: Gradual decline in plasticity

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`? Always call this before stepping!
- Try increasing the number of steps
- Check that sensory input is connected properly

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called
- Check motor neuron activity

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that plasticity rules are enabled

#### "Build fails with Python errors"
```bash
pip install scikit-build-core pybind11

cmake .. -DPython_EXECUTABLE=$(which python)
```

#### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for memory access violations in configuration

### Debugging Tips

#### Enable Logging
```cpp
#include "Logger.hpp"
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
```

#### Log important events
```cpp
NLM_LOG_INFO("Starting simulation with " + std::to_string(neuronCount) + " neurons");
NLM_LOG_DEBUG("Spike detected at neuron " + std::to_string(neuronId));
```

### Performance Monitoring
```cpp
// Check simulation statistics
std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
std::cout << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
```

## Development Guidelines

### Coding Standards
1. **Use descriptive variable names**: `neuronId` not `id`
2. **Follow RAII**: Use smart pointers for memory management
3. **Minimal comments**: Clear code over verbose comments
4. **Error checking**: Validate inputs and check return codes
5. **Consistent formatting**: 2-space indentation, no tabs

### Testing Best Practices
1. **Test edge cases**: Empty inputs, maximum values, boundary conditions
2. **Memory management**: Check for leaks with valgrind/address sanitizer
3. **Performance benchmarks**: Measure before and after optimizations
4. **Integration tests**: Verify Phase 6 integration works

### Extending NLM

#### Adding New Brain Regions
```cpp
// Create a new region type
class NewRegion : public NeuralRegion {
public:
    NewRegion(const std::string& name) : NeuralRegion(name) {}
    
    void initialize(int neuronCount, RandomGenerator& rng) override {
        // Create neurons specific to this region
        // Set up connectivity patterns
    }
    
    // Override other pure virtual methods as needed
};
```

#### Adding New Plasticity Rules
```cpp
class NewPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Implement new learning rule
    }
    
    float getLearningRate() const override {
        return 0.001f; // Custom learning rate
    }
};
```

#### Adding New Sensor Types
```cpp
def sensor_callback(percept_data):
    """Process new sensor data"""
    # Extract features from sensor data
    # Update brain state via brain.receiveSensoryInput()
    pass
```

## Research and Applications

### Scientific Questions NLM Addresses
NLM investigates whether:
- Memory systems can integrate with neural dynamics
- Neuromodulation can affect plasticity in a coordinated way
- Prediction can become a central organizing principle
- Developmental stages can modulate learning
- Replay can reinforce memory consolidation
- The complete brain loop functions coherently

### Potential Applications
1. **Neuroscientific Research**: Testing hypotheses about brain function
2. **AI Development**: Creating more brain-like artificial intelligence
3. **Education**: Understanding learning and development
4. **Robotics**: Creating adaptive, learning robots
5. **Therapy**: Studying neurological conditions and treatments

## Future Directions

### Phase 7: Advanced Neuroscience
- Hodgkin-Huxley neuron models
- Detailed dendritic computation
- Realistic cortical architecture
- GABAergic and other neurotransmitter systems

### Phase 8: Enhanced Learning
- Supervised learning integration
- Reinforcement learning algorithms
- Meta-learning and optimization
- Transfer learning capabilities

### Phase 9: Scalability
- GPU acceleration
- Distributed computing
- Neuromorphic hardware support
- Large-scale network simulation

## References

### Primary Papers
1. **Documented Biology**: All mathematical models are documented in the source code comments
2. **Peer Review**: This documentation follows the project's rigorous standards
3. **Scientific Background**: See `docs/SCIENCE.md` for biological foundations

### Related Work
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Science Background](docs/SCIENCE.md)
- [Experimental Results](docs/EXPERIMENTS.md)

## Acknowledgments

This project represents years of collaborative research and development. Special thanks to:
- All contributors who implemented specific brain regions and systems
- The scientific advisors who provided biological guidance
- The test suite maintainers who ensure quality
- The documentation team that keeps this information current

## License

MIT License - See `LICENSE` file for details.