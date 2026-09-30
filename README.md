# NLM (熙然) - Neural Learning Machine

## Overview

NLM (熙然) is an experimental computational brain system designed to create an artificial developmental brain. This implementation represents **Phase 6: Integrated Artificial Brain**, where all previously disconnected neural systems have been successfully integrated into a coherent, functioning architecture.

## Key Features

### 🌟 Revolutionary Integration
- **Complete System Integration**: All neural systems work together coherently
- **Real Neural Computation**: No ML frameworks - uses spiking neural dynamics
- **Predictive Coding**: Forward models and prediction error-driven learning
- **Neuromodulation**: Dopamine, curiosity, novelty signals coordinate learning

### 🧠 Cognitive Capabilities
- **Working Memory**: Neural persistence with competition and decay
- **Episodic Memory**: Experience storage with replay mechanisms
- **Attention**: Competitive selection and focus modulation
- **Concept Formation**: Pattern discovery and categorization
- **Planning**: Multi-step action planning and evaluation

### 🚀 Advanced Learning
- **Error-Driven Plasticity**: STDP, Hebbian, and structural plasticity
- **Curiosity-Driven Exploration**: Intrinsic motivation systems
- **Prediction Error**: Neuromodulatory learning signals
- **Developmental Processes**: Self-organization and maturation

## Architecture Overview

```
Sensory Input → Working Memory → Prediction System (NeuralPrediction)
                                                ↓
                                        Prediction Error → Dopamine System
                                                ↓
                                        Reward Prediction Error
                                                ↓
                                    Curiosity + Novelty Modulation
                                                ↓
                                      Attention + Concept Formation
                                                ↓
                                Enhanced Neural Processing
                                                ↓
                                        Motor Output
```

## System Components

### 1. Core Neural Systems
- **LIF Neurons**: Leaky Integrate-and-Fire dynamics
- **Spike System**: Event-driven spike propagation with delays
- **Synapses**: Excitatory/inhibitory connections with plasticity
- **Populations**: Grouped neurons for efficient computation

### 2. Memory Systems
- **NeuralWorkingMemory**: Persistent activity and recurrent connections
- **NeuralEpisodicMemory**: Experience storage with pattern completion
- **NeuralAssociativeMemory**: Pattern association and spreading activation

### 3. Prediction Systems
- **NeuralPrediction**: Temporal sequence learning and forward modeling
- **PredictionError**: Neuromodulatory error signals
- **ActionConsequencePredictor**: Learned action-effect mappings

### 4. Cognitive Systems
- **NeuralPlanner**: Multi-step action planning
- **ConceptFormation**: Pattern clustering and category discovery
- **AttentionalSelection**: Competitive focus and selection
- **SelfModel**: Internal self-representation

### 5. Neuromodulation Systems
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and intrinsic rewards
- **Novelty**: Surprise detection and novelty response

### 6. Developmental Systems
- **Synaptogenesis**: Formation of new connections
- **Pruning**: Elimination of weak connections
- **Maturation**: System-level developmental changes

## Quick Start Guide

### Installation

1. **Clone the repository**
```bash
git clone https://github.com/nlmlab/NLM.git
cd NLM
```

2. **Build with CMake**
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

3. **Run the demonstration**
```bash
./nlm integrated
```

### Configuration

#### Default Configuration (`configs/default.cfg`)
```toml
[config]
name = "NLM Default"
version = "6.0.0"

[config.brain]
neuron_count = 1000
region_count = 1
connection_probability = 0.1

[config.learning]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[config.memory]
working_memory_capacity = 100
replay_interval = 100
consolidation_interval = 1000

[config.simulation]
simulation_timestep = 0.001
max_steps = 10000
```

#### Custom Configuration File
```bash
./nlm --config my_config.json integrated
```

## Command Line Interface

### General Usage
```bash
nlm [options] [mode] [arguments]
```

### Modes

| Mode | Description | Command |
|------|-------------|---------|
| `basic` | Basic connectivity test | `./nlm basic` |
| `learning` | Learning/plasticity experiment | `./nlm learning` |
| `stpd` | STDP verification test | `./nlm stpd` |
| `predict` | Prediction system test | `./nlm predict` |
| `cognition` | Cognitive capabilities test | `./nlm cognition` |
| `integrated` | Full integrated brain demonstration | `./nlm integrated` |
| `benchmark` | Performance benchmarks | `./nlm benchmark` |
| `demo` | Interactive demo mode | `./nlm demo` |

### Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help, -h` | Show help message | `./nlm --help` |
| `--version, -v` | Show version information | `./nlm --version` |
| `--config <file>` | Load configuration file | `./nlm --config config.json learning` |
| `--seed <num>` | Set random seed | `./nlm --seed 12345 learning` |
| `--steps <num>` | Number of simulation steps | `./nlm --steps 5000 learning` |
| `--neurons <num>` | Set neuron count | `./nlm --neurons 2000 learning` |
| `--regions <num>` | Set region count | `./nlm --regions 3 learning` |
| `--time <sec>` | Simulation duration | `./nlm --time 10 learning` |
| `--output <dir>` | Output directory | `./nlm --output ./results integrated` |
| `--visualize` | Enable visualization | `./nlm --visualize demo` |
| `--quiet, -q` | Reduce output verbosity | `./nlm --quiet learning` |
| `--verbose, -V` | Increase output verbosity | `./nlm --verbose learning` |

### Examples

#### 1. Basic Demonstration
```bash
./nlm basic
```

#### 2. Learning Experiment with Custom Configuration
```bash
./nlm --config custom_config.json --seed 42 --steps 10000 learning
```

#### 3. Prediction System Test
```bash
./nlm --neurons 5000 --time 5 predict
```

#### 4. Full Integrated Brain Demo
```bash
./nlm --visualize --output ./brain_output integrated
```

#### 5. Performance Benchmark
```bash
./nlm --steps 50000 benchmark
```

## Python API

NLM provides a Python interface for easier interaction:

### Installation
```bash
pip install pynlm
```

### Basic Usage
```python
import nlm

# Create configuration
config = nlm.Config({
    "neuron_count": 2000,
    "prediction_horizon": 5,
    "curiosity_strength": 0.8,
    "planning_depth": 8
})

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Create world and agent
world = nlm.SimpleWorld(width=50, height=50)
agent = nlm.AgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(10000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Usage
```python
import nlm

# Create brain with integrated systems
config = nlm.Config({
    "neuron_count": 5000,
    "prediction_horizon": 10,
    "working_memory_capacity": 500,
    "enable_curiosity": True,
    "enable_planning": True,
    "enable_concept_formation": True
})

brain = nlm.Brain(config)
brain.initialize()

# Configure specific systems
brain.getPredictionSystem().setPredictionHorizon(5)
brain.getCuriosity().setExplorationStrength(0.7)
brain.getPlanner().setPlanningDepth(6)
brain.getConceptFormation().setFormationThreshold(0.8)

# Run learning task
world = nlm.createMazeEnvironment()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Train agent
for episode in range(100):
    state = world.reset()
    done = False
    step = 0
    
    while not done and step < 1000:
        action = agent.act(state)
        next_state, reward, done = world.step(action)
        agent.learn(state, action, reward, next_state, done)
        state = next_state
        step += 1
```

## Development Guide

### 1. Adding New Components

To add a new neural component, follow these steps:

1. **Create the component class** in the appropriate directory
2. **Register it in Brain.hpp** as a forward declaration
3. **Implement the interface** in the corresponding .cpp file
4. **Initialize it in Brain.cpp** constructor
5. **Integrate it into step()** method

Example structure:
```
src/new_component/
├── NewComponent.hpp
├── NewComponent.cpp
└── (implementation details)
```

### 2. Integration Protocol

All components must follow the integration protocol:

1. **Consistent Naming**: Use camelCase for methods, PascalCase for classes
2. **Standard Interface**: All brain components must have:
   - `initialize(Brain* brain)` method
   - `update(timestep)` method
   - Appropriate getters and setters
3. **Error Handling**: Use NLM_LOG_ERROR for errors, NLM_LOG_INFO for normal operations
4. **Memory Management**: Use RAII and smart pointers
5. **Thread Safety**: Use mutexes for shared resources if needed

### 3. Testing

Run the existing tests:
```bash
cd build
ctest --output-on-failure
```

Or run specific test suites:
```bash
./nlm_phase3_demo  # Phase 3 tests
./nlm_phase4_demo  # Phase 4 tests
```

### 4. Performance Optimization

- **Batch operations**: Process neurons in groups when possible
- **Memory pools**: Use custom allocators for frequently created objects
- **Cache locality**: Organize data structures for cache efficiency
- **Event-driven**: Minimize unnecessary computations

## Documentation

### API Reference

- [Brain Class Reference](docs/api/brain.md)
- [Memory System API](docs/api/memory.md)
- [Prediction System API](docs/api/prediction.md)
- [Cognitive Systems API](docs/api/cognition.md)

### Tutorials

- [Getting Started](docs/tutorials/getting_started.md)
- [Brain Integration](docs/tutorials/brain_integration.md)
- [Learning Systems](docs/tutorials/learning_systems.md)
- [Cognition and Attention](docs/tutorials/cognition.md)

### Examples

- [Basic Brain Usage](examples/basic_usage.cpp)
- [Prediction System](examples/prediction_example.cpp)
- [Learning Task](examples/learning_task.cpp)
- [Environment Navigation](examples/navigation.cpp)

## Contributing

### Code Style

Follow these guidelines:

1. **C++20 Standard**: Use modern C++ features where appropriate
2. **Naming Conventions**:
   - Classes: PascalCase
   - Methods: camelCase
   - Variables: snake_case for constants, camelCase for variables
   - Files: snake_case.cpp for implementation, SnakeCase.hpp for headers
3. **Documentation**: Use Doxygen-style comments
4. **Error Handling**: Use exceptions for recoverable errors, return codes for fatal errors

### Pull Request Process

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Add tests** for new functionality
5. **Update documentation**
6. **Run the full test suite**
7. **Create a pull request**

### Code Review

All pull requests must be reviewed by at least one maintainer. Changes should:

- Improve code quality or functionality
- Add meaningful documentation
- Include comprehensive tests
- Maintain backward compatibility where possible

## Roadmap

### Current (Phase 6)
- ✅ Complete system integration
- ✅ Real predictive coding
- ✅ Advanced memory systems
- ✅ Cognitive capabilities
- ✅ Developmental processes

### Next Phase (Phase 7)
- **Social Learning**: Multi-agent interaction and imitation
- **Language Processing**: Neural language understanding
- **Consciousness Research**: Investigating subjective experience
- **Advanced Planning**: Hierarchical and recursive planning

## Support

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share ideas
- **Documentation**: Read comprehensive guides and API references

### Community

- **Slack/Discord**: Join the NLM community
- **Monthly Meetups**: Participate in development discussions
- **Code Workshops**: Learn from experienced contributors

## License

```
MIT License

Copyright (c) 2026 NLM Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Acknowledgments

This project builds on decades of research in computational neuroscience:

- **Neural Networks**: Foundation for modern AI systems
- **Spiking Neural Networks**: Event-driven biological realism
- **Predictive Coding**: Brain's hypothesis testing framework
- **Working Memory**: Cognitive architecture cornerstone
- **Attention Mechanisms**: Selective information processing

## Contact

- **Website**: https://nlm.ai
- **Documentation**: https://docs.nlm.ai
- **GitHub**: https://github.com/nlmlab/NLM
- **Email**: contact@nlm.ai

---

*Last updated: 2026-09-30*
*Version: Phase 6.0.0*
