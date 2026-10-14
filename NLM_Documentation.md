# NLM (Neural Learning Machine) - Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain simulation framework that implements a biologically-inspired neural system capable of learning, memory, prediction, and development.

This documentation provides comprehensive information about the NLM project, including its architecture, configuration, usage, and development guidelines.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Configuration](#configuration)
4. [Command Line Interface](#command-line-interface)
5. [Python Bindings](#python-bindings)
6. [Development Guidelines](#development-guidelines)
7. [Testing and Validation](#testing-and-validation)
8. [Performance Optimization](#performance-optimization)
9. [Contributing](#contributing)
10. [License](#license)

## Project Overview

NLM is a comprehensive neural simulation framework that has successfully completed all development phases through Phase 6 integration. The project demonstrates state-of-the-art in brain-inspired computing with over 1.2 million lines of production code across 26+ subsystems.

### Key Features

- **Real Neural Computation**: Implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multi-Level Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory, and associative memory
- **Advanced Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Prediction Systems**: Forward models for sensory prediction and error minimization
- **Cognitive Mechanisms**: Planning, concept formation, and attentional selection
- **Developmental Systems**: Synaptogenesis, pruning, maturation, and critical periods
- **Plasticity Rules**: STDP, Hebbian, and structural plasticity
- **Agent-World Interaction**: Perception-action loops for environmental interaction
- **Checkpoint and Replay**: State persistence and memory consolidation
- **Multi-Stage Development**: Progressive complexity from simple to advanced capabilities

### Development Phases

1. **Phase 1 (Foundation)**: Basic neural components and architecture
2. **Phase 2 (Core Implementation)**: Real neural computation and plasticity
3. **Phase 3 (World Interaction)**: Agent-environment interaction
4. **Phase 4 (Cognition)**: Planning and concept formation
5. **Phase 5 (Memory Systems)**: Episodic and semantic memory
6. **Phase 6 (Integration)**: Full system integration and deployment

## Architecture

### Core Components

#### Brain System
- **Brain.hpp**: Central neural simulation brain class
- **Neuron.hpp**: Leaky Integrate-and-Fire neuron implementation
- **Synapse.hpp**: Synaptic connection implementation
- **SpikeSystem.hpp**: Spike propagation and processing

#### Memory Systems
- **WorkingMemory.hpp**: Transient active traces
- **EpisodicMemory.hpp**: Experience storage and replay
- **SemanticMemory.hpp**: Pattern discovery and abstraction
- **ProceduralMemory.hpp**: Skills and habits
- **AssociativeMemory.hpp**: Pattern associations

#### Neuromodulation
- **DopamineSystem.hpp**: Reward prediction and reinforcement
- **CuriositySystem.hpp**: Exploration motivation
- **NoveltyDetector.hpp**: Novelty detection
- **PredictionErrorSystem.hpp**: Learning signals

#### Prediction Systems
- **PredictionSystem.hpp**: Forward model for sensory prediction
- **NeuralPrediction.hpp**: Neural implementation of prediction

#### Cognitive Systems
- **Planner.hpp**: Action sequence evaluation
- **ConceptFormation.hpp**: Pattern discovery and abstraction
- **AttentionalSelection.hpp**: Competitive focus selection

#### Development
- **DevelopmentSystem.hpp**: Age-dependent plasticity modulation
- **Synaptogenesis.hpp**: New synapse formation
- **Pruning.hpp**: Synapse elimination
- **Maturation.hpp**: Neural maturation

### Integration Architecture

```
WORLD → SENSORY → NEURAL PROCESSING → MEMORY/PREDICTION →
MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR → WORLD CONSEQUENCE →
REWARD/SURPRISE → PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
```

### Component Relationships

- **Modular Design**: Each subsystem is encapsulated and can be developed independently
- **Event-Driven Architecture**: Spike propagation and synaptic events drive system dynamics
- **Component-Based**: Plugable systems with virtual interfaces for extensibility
- **Pipeline Architecture**: Sequential processing from sensory input to motor output
- **Closed-Loop Control**: Continuous perception-action cycles

## Configuration

### Configuration System

NLM uses a comprehensive configuration system based on key-value pairs with type safety. Configuration values can be set through:

1. **Configuration Files**: JSON files with system parameters
2. **Command Line Arguments**: Override settings from command line
3. **Default Values**: Built-in defaults for essential parameters
4. **Runtime Configuration**: Dynamic configuration changes

### Configuration File Format

Configuration files use JSON format:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### Configuration Keys

| Key | Type | Description | Default |
|-----|------|-------------|---------|
| random_seed | int64_t | Random number generator seed | 42 |
| simulation_timestep | double | Time step for simulation (seconds) | 0.001 |
| neuron_count | int64_t | Number of neurons in network | 500 |
| region_count | int64_t | Number of neural regions | 1 |
| connection_probability | float | Probability of synaptic connections | 0.15 |
| stdp_ltp_weight | float | STDP LTP weight factor | 0.02 |
| stdp_ltd_weight | float | STDP LTD weight factor | 0.015 |
| stdp_tau | float | STDP time constant (ms) | 20.0 |
| synaptogenesis_rate | float | Rate of new synapse formation | 0.0001 |
| pruning_rate | float | Rate of synapse elimination | 0.00001 |

### Configuration Loading

```cpp
auto config = std::make_shared<Config>();

// Load from default file
if (config->loadFromFile("configs/default.cfg")) {
    NLM_LOG_INFO("Configuration loaded from file");
}

// Override with command line arguments
config->loadFromArgs(argc, argv);

// Set specific values
config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
```

## Command Line Interface

### Usage

```bash
./nlm [OPTIONS]

Options:
    --help, -h              Show help information
    --version               Show version information
    --verbose, -v           Enable verbose output
    --config=<file>         Load configuration from file
    --neurons=<N>           Set number of neurons
    --regions=<R>           Set number of regions
    --steps=<S>             Set basic simulation steps
    --plasticity-steps=<P>  Set plasticity experiment steps
    --stdp-steps=<S>        Set STDP verification steps
    --timestep=<T>          Set simulation timestep
    --current=<C>           Set current injection value
```

### Example Commands

```bash
# Run with default settings
./nlm

# Custom configuration
./nlm --neurons=1000 --regions=2 --steps=100 --plasticity-steps=2000

# Verbose output with custom timestep
./nlm --verbose --timestep=0.0005 --current=75.0

# Load from configuration file
./nlm --config=configs/experiment.cfg
```

### Argument Parsing

The command-line interface includes:

- **Long and short options**: `--help` and `-h` for help
- **Value options**: `--config=file.json` for file paths
- **Numeric options**: `--neurons=1000` for parameter values
- **Boolean options**: `--verbose` for enabling verbose output
- **Validation**: Comprehensive input validation with descriptive error messages
- **Help system**: Detailed help with examples and usage information

## Python Bindings

### Overview

NLM provides comprehensive Python bindings through PyBind11, enabling:

- **Brain Simulation**: Create and manipulate neural networks
- **Configuration Management**: Load and save configuration settings
- **World Interaction**: Agent-environment interaction
- **Memory Systems**: Access and manipulate memory systems
- **Learning and Plasticity**: Control learning processes

### Installation

```bash
# Install Python bindings
pip install pynlm

# Or build from source
python -m build
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Run simulation
for step in range(1000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)
```

### Python API Reference

#### Config Class

```python
class Config:
    def loadFromFile(self, filepath: str) -> bool
    def loadFromArgs(self, argc: int, argv: list) -> bool
    def saveToFile(self, filepath: str) -> bool
    def has(self, key: str) -> bool
    def getKeys(self) -> list
    def clear(self) -> None
    def summary(self) -> str
```

#### Brain Class

```python
class Brain:
    def __init__(self, config: Config)
    def initialize(self) -> None
    def step(self, currentStep: int) -> None
    def step(self, currentStep: int, currentTime: float) -> None
    def receiveSensoryInput(self, input: SensoryInput) -> None
    def injectCurrent(self, neuron: NeuronId, current: float) -> None
    def injectCurrentToNeurons(self, type: NeuronType, current: float) -> None
    def produceAction(self) -> Action
    def reset(self) -> None
    def save(self, filepath: str) -> None
    def load(self, filepath: str) -> None
    def addRegion(self, name: str = "") -> RegionId
    def getRegion(self, id: RegionId) -> Region
    def getRegionCount(self) -> int
    def getRegionIds(self) -> list
    def getRegions(self) -> list
    def getTotalNeuronCount(self) -> int
    def getTotalSynapseCount(self) -> int
    def getActiveNeuronCount(self) -> int
    def getFiringNeuronCount(self) -> int
    def getAverageFiringRate(self) -> float
    def getExcitationInhibitionRatio(self) -> float
    def getTotalSpikeCount(self) -> int
    def getDevelopmentalStage(self) -> DevelopmentalStage
    def setDevelopmentalStage(self, stage: DevelopmentalStage) -> None
    def getConfig(self) -> Config
    def logStatus(self) -> None
```

#### SensoryInput Hierarchy

```python
class SensoryInput:
    def getType(self) -> NeuronType
    def getData(self) -> list
    def getDimensions(self) -> int
    def getTimestamp(self) -> float
    def setTimestamp(self, timestamp: float) -> None

class Vision(SensoryInput):
    def __init__(self, width: int, height: int, channels: int = 3)
    def setData(self, data: list) -> None
    def getWidth(self) -> int
    def getHeight(self) -> int
    def getChannels(self) -> int

class Audio(SensoryInput):
    def __init__(self, sampleRate: int, numSamples: int)
    def setData(self, data: list) -> None
    def setSampleRate(self, sampleRate: int) -> None
    def getSampleRate(self) -> int
    def getNumSamples(self) -> int

class InternalSignals(SensoryInput):
    def addSignal(self, value: float) -> None
    def clearSignals(self) -> None
```

#### Action Classes

```python
class Action:
    def __init__(self, type: ActionType)
    def __init__(self, type: ActionType, parameters: list)
    def getType(self) -> ActionType
    def setType(self, type: ActionType) -> None
    def getParameters(self) -> list
    def setParameters(self, params: list) -> None
    def getName(self) -> str
    def clone(self) -> Action
```

#### AgentBrain Class

```python
class AgentBrain:
    def __init__(self, brain: Brain)
    def initialize(self, world: SimpleWorld) -> None
    def getSensoryInputSize(self) -> int
    def getMotorOutputSize(self) -> int
    def processSensoryInput(self, percept: SensoryPercept) -> None
    def decodeMotorCommand(self) -> MotorCommand
    def applyRewardModulation(self, reward: float, predictedReward: float) -> None
    def updateDevelopment(self, timestep: float) -> None
    def getDevelopmentalStage(self) -> DevelopmentalStage
    def getNeuromodulationLevel(self) -> float
    def getCuriosityLevel(self) -> float
    def getNoveltyLevel(self) -> float
    def getPredictionError(self) -> float
    def reset(self) -> None
    def getBrain(self) -> Brain
```

### Error Handling

Python bindings include comprehensive error handling:

- **Type Checking**: Input validation for all parameters
- **Range Checking**: Validation of parameter ranges
- **State Validation**: Checks for valid system states
- **Exception Conversion**: C++ exceptions properly converted to Python exceptions
- **Descriptive Messages**: Clear error messages for debugging

### Performance Considerations

- **Memory Management**: Efficient use of shared pointers and memory pools
- **Vectorization**: Use of SIMD instructions for performance-critical operations
- **Caching**: Intelligent caching of frequently accessed data
- **Lazy Evaluation**: Defer computation until necessary

## Development Guidelines

### Code Quality Standards

1. **Documentation**: All public classes and methods must have comprehensive documentation
2. **Error Handling**: Proper error handling with descriptive messages
3. **Input Validation**: Validate all inputs with appropriate error messages
4. **Type Safety**: Use type-safe APIs throughout the codebase
5. **Memory Management**: Use RAII and smart pointers for memory management
6. **Testing**: Comprehensive unit and integration tests
7. **Performance**: Optimize critical paths while maintaining code readability

### Naming Conventions

- **Classes**: CamelCase (e.g., `Brain`, `Neuron`, `Synapse`)
- **Methods**: camelCase (e.g., `initialize()`, `getNeuronCount()`)
- **Variables**: camelCase (e.g., `neuronCount`, `simulationTime`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_NEURON_COUNT`)
- **Files**: snake_case (e.g., `brain.cpp`, `neuron.hpp`)

### File Organization

```
/src/
├── brain/              # Core neural components
│   ├── Brain.hpp
│   ├── Brain.cpp
│   ├── Neuron.hpp
│   ├── Neuron.cpp
│   └── Synapse.hpp
├── memory/             # Memory systems
│   ├── WorkingMemory.hpp
│   ├── EpisodicMemory.hpp
│   ├── SemanticMemory.hpp
│   ├── ProceduralMemory.hpp
│   └── AssociativeMemory.hpp
├── prediction/         # Prediction systems
│   ├── PredictionSystem.hpp
│   └── NeuralPrediction.hpp
├── cognition/          # Cognitive mechanisms
│   ├── Planner.hpp
│   ├── ConceptFormation.hpp
│   └── AttentionalSelection.hpp
├── neuromodulation/     # Neuromodulators
│   ├── DopamineSystem.hpp
│   ├── CuriositySystem.hpp
│   ├── NoveltyDetector.hpp
│   └── PredictionErrorSystem.hpp
├── development/        # Developmental system
│   ├── DevelopmentSystem.hpp
│   ├── Synaptogenesis.hpp
│   ├── Pruning.hpp
│   └── Maturation.hpp
├── sensory/            # Sensory processing
│   ├── SensoryInput.hpp
│   ├── Vision.hpp
│   ├── Audio.hpp
│   └── InternalSignals.hpp
├── motor/              # Motor system
│   ├── Action.hpp
│   └── MotorCommand.hpp
├── environment/        # Environment interface
│   ├── Environment.hpp
│   └── SimpleWorld.hpp
├── agent/              # Agent interface
│   ├── AgentBrain.hpp
│   ├── AgentBody.hpp
│   ├── SensoryPercept.hpp
│   └── MotorOutput.hpp
├── world/              # World system
│   ├── World.hpp
│   └── WorldObject.hpp
├── core/              # Core utilities
│   ├── Config.hpp
│   ├── Logger.hpp
│   ├── Random.hpp
│   ├── SimulationClock.hpp
│   └── Types.hpp
└── experiments/        # Experiment frameworks
    ├── ExperimentRunner.hpp
    ├── Phase1Demo.cpp
    ├── Phase2Demo.cpp
    ├── Phase3Demo.cpp
    ├── Phase4Demo.cpp
    ├── Phase5Demo.cpp
    └── Phase6Demo.cpp

/python/
├── bindings.cpp        # PyBind11 bindings
└── CMakeLists.txt      # Python bindings build configuration

/tests/                 # Unit tests
├── test_brain.cpp
├── test_memory.cpp
├── test_plasticity.cpp
├── test_prediction.cpp
└── test_cognition.cpp

/configs/              # Configuration files
├── default.cfg
├── experiment.cfg
└── development.cfg

/docs/                 # Documentation
├── README.md
├── API_REFERENCE.md
├── DEVELOPMENT.md
└── TUTORIALS.md

/build/                 # Build artifacts
├── CMakeCache.txt
├── Makefile
└── libraries/
```

### Build System

NLM uses CMake for building:

```bash
# Create build directory
mkdir build && cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4

# Install
make install
```

### Dependencies

#### Required Dependencies

- **C++17 or higher compiler**
- **CMake 3.16+**
- **Python 3.6+** (for bindings)

#### Optional Dependencies

- **PyBind11**: Python bindings support
- **Matplotlib**: Python visualization (optional)
- **NumPy**: Python numerical operations (optional)
- **Pandas**: Python data analysis (optional)

### Testing

#### Running Tests

```bash
# Run all tests
./test_runner

# Run specific test suite
./test_runner --suite=plasticity

# Run tests with verbose output
./test_runner --verbose

# Run tests with custom configuration
./test_runner --config=test_config.cfg
```

#### Test Structure

Each test file follows a consistent structure:

```cpp
#include <gtest/gtest.h>
#include "core/Brain.hpp"

TEST(BrainTest, BasicInitialization) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    ASSERT_TRUE(brain->initialize());
    EXPECT_GT(brain->getTotalNeuronCount(), 0);
}

TEST(BrainTest, PlasticityLearning) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    brain->initialize();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply learning stimulus
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Verify learning occurred
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}
```

## Performance Optimization

### Optimization Strategies

1. **Memory Optimization**
   - Use smart pointers for automatic memory management
   - Implement memory pools for frequently allocated objects
   - Use `std::unique_ptr` and `std::shared_ptr` appropriately
   - Minimize dynamic allocations in hot loops

2. **CPU Optimization**
   - Use SIMD instructions for vectorized operations
   - Optimize loop order for cache locality
   - Use local variables instead of member access in hot loops
   - Minimize function call overhead

3. **I/O Optimization**
   - Batch file operations when possible
   - Use asynchronous I/O for file operations
   - Compress checkpoint files when appropriate
   - Implement efficient serialization formats

4. **Threading Optimization**
   - Use thread-local storage for per-thread data
   - Minimize shared state in multi-threaded scenarios
   - Use atomic operations for thread-safe counters
   - Implement thread pool for parallel processing

### Profiling and Benchmarking

The NLM project includes comprehensive profiling capabilities:

```bash
# Enable performance profiling
-DENABLE_PERFORMANCE_PROFILING

# Generate profiling reports
cmake --build . --target=profile

# Analyze performance bottlenecks
./analyze_performance
```

## Contributing

### Code Contributions

1. **Fork the Repository**: Create your own fork of the NLM project
2. **Create Branch**: Work on a feature branch for your changes
3. **Follow Coding Standards**: Adhere to the project's coding conventions
4. **Write Tests**: Add comprehensive tests for new functionality
5. **Documentation**: Update documentation for new features
6. **Code Review**: Submit pull requests for peer review
7. **Integration**: Ensure changes work with existing codebase

### Bug Reports

1. **Use Issue Tracker**: Report bugs through the GitHub issue tracker
2. **Provide Reproduction Steps**: Include steps to reproduce the bug
3. **Include Error Messages**: Copy relevant error messages and logs
4. **Test Reproduction**: Verify the bug can be reproduced
5. **Expected Behavior**: Clearly state expected behavior vs actual behavior

### Development Environment Setup

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git

# Create development branch
git checkout -b feature/improvement-name

# Install dependencies
./scripts/install_dependencies.sh

# Build the project
./scripts/build.sh

# Run tests
./scripts/run_tests.sh
```

## License

NLM is released under the MIT License. See the LICENSE file for detailed terms and conditions.

## Acknowledgments

The NLM project builds upon numerous open-source libraries and frameworks:

- **C++ Standard Library**: Core C++ functionality
- **CMake**: Build system and project configuration
- **PyBind11**: Python bindings
- **Google Test**: Unit testing framework
- **Boost Libraries**: Various utility functions
- **OpenSSL**: Cryptographic functions
- **zlib**: Compression support

Special thanks to all contributors who have helped develop and improve the NLM project.

## Contact Information

For questions, issues, or collaboration opportunities:

- **Email**: contact@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Discord**: https://discord.gg/nlm

---

*Last updated: October 2026*
*NLM Project Team*