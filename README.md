# NLM - Neural Learning Machine

## Overview

NLM (熙然) is an experimental artificial developmental brain project written in C++ with Python bindings. The goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Features

- **Real spiking neural computation** with LIF neurons and event-driven dynamics
- **Integrated brain systems** including memory, prediction, cognition, and neuromodulation
- **Developmental stages** that affect plasticity and learning
- **Python bindings** for easy access and research
- **Comprehensive command-line interface** for running experiments
- **Advanced configuration** and checkpointing for reproducible research

## System Architecture

### Phase-Based Development

The NLM follows a phased development approach:

1. **Phase 1**: Project skeleton and core utilities
2. **Phase 2**: Real neural computation (LIF neurons, STDP, structural plasticity)
3. **Phase 3**: World interaction and sensory-motor loops
4. **Phase 4**: Integrated cognitive systems (prediction, memory, planning)
5. **Phase 5**: Performance optimizations and checkpointing
6. **Phase 6**: Final integration (all systems working together)

### Core Systems

#### Neural Processing
- **Brain**: Central coordinator of all neural systems
- **Neuron**: Leaky Integrate-and-Fire dynamics
- **Synapse**: Plastic connections with STDP and Hebbian learning
- **SpikeSystem**: Event-driven spike propagation

#### Memory Systems
- **WorkingMemory**: Transient information storage
- **EpisodicMemory**: Experience recording and replay
- **AssociativeMemory**: Pattern associations

#### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Learning signal computation

#### Cognitive Systems
- **PredictionSystem**: Forward models and prediction
- **NeuralPlanner**: Action planning and evaluation
- **ConceptFormation**: Pattern discovery
- **AttentionalSelection**: Competitive attention

#### Development and Plasticity
- **DevelopmentSystem**: Developmental stages and maturation
- **Synaptogenesis**: Synapse formation
- **Pruning**: Synapse elimination
- **Maturation**: Maturation of neural properties

#### Environment and Behavior
- **SimpleWorld**: 2D world simulation
- **SensoryInput**: Sensory transduction
- **MotorSystem**: Action selection and execution

## Getting Started

### Quick Installation

```bash
# Clone the repository
cd /path/to/nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Basic Usage

#### Python Interface
```python
import nlm

# Create a default configuration
config = nlm.create_default_config()

# Create and initialize brain
brain = nlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get status
brain.logStatus()
```

#### Command Line Interface
```bash
# Show help
nlm --help

# Run Phase 6 integration demo
nlm run --phase6

# Create custom configuration
nlm config create my_config.cfg

# Run specific experiment
nlm experiment phase6
```

### Running Experiments

The NLM includes several pre-built experiments:

#### Phase 2: Real Neural Computation
```bash
./build/nlm  # Phase 2 demo (basic neural computation)
```

#### Phase 4: Integrated Cognitive Systems
```bash
./build/nlm_phase4_demo  # Cognitive systems demo
```

#### Phase 6: Full Integration
```bash
./build/nlm_phase6_demo  # Complete brain integration demo
```

## Configuration

### Default Configuration
The default configuration is stored in `configs/default.cfg`:

```cfg
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
dopamine_baseline = 0.1
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
max_simulation_steps = 10000
```

### Custom Configuration
```python
import nlm

# Create custom configuration
config = nlm.Config()
config.random_seed = 12345
config.neuron_count = 2000
config.stdp_ltp_weight = 0.015
config.development_enabled = True
config.curiosity_enabled = True

# Create brain with custom config
brain = nlm.create_brain(config)
brain.initialize()
```

## Python Interface

### High-Level API
```python
import nlm
from nlm import nlm_interface

# Create agent with world
brain, agent_brain, world = nlm.create_agent()

# Run simulation
for step in range(1000):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    brain.receiveSensoryInput(percept)
    
    # Simulate one step
    brain.step(step)
    
    # Get action
    action = brain.produceAction()
    
    # Apply to world
    result = world.applyMotorCommand(action)
    
    # Apply neuromodulation
    agent_brain.applyRewardModulation(result.reward, 0.0)
```

### Advanced Usage
```python
import nlm
import nlm.nlm_interface as nlm_interface

# Create custom session with advanced features
session = nlm_interface.NLMSession(config)

# Set up session
session.setup()

# Run simulation with checkpointing
result = session.run(steps=1000)

# Save checkpoint
session.save_checkpoint("my_session.checkpoint")

# Load checkpoint
session.load_checkpoint("my_session.checkpoint")
```

### Examples
```python
# Run all examples
python examples_and_tutorials.py

# Run specific example
python examples_and_tutorials.py phase6
python examples_and_tutorials.py advanced
```

## Command Line Interface

### Available Commands
```bash
nlm run                    # Run simulation
nlm config                 # Manage configuration files
nlm experiment             # Run pre-built experiments
nlm benchmark              # Performance benchmarking
nlm help                   # Show help information
```

### Run Command
```bash
nlm run [OPTIONS]

Options:
  --config FILE      Load configuration from FILE
  --steps N          Number of simulation steps (default: 1000)
  --seed N           Random seed
  --quick-demo       Run quick demo
  --phase2           Run Phase 2 experiment
  --phase4           Run Phase 4 experiment
  --phase6           Run Phase 6 experiment
```

### Config Command
```bash
nlm config <ACTION> [FILE]

Actions:
  show [FILE]        Show configuration from FILE
  create [FILE]      Create default configuration at FILE
  edit [FILE]        Edit configuration
```

### Experiment Command
```bash
nlm experiment <NAME> [OPTIONS]

Available experiments:
  phase2    Phase 2: Real Neural Computation
  phase4    Phase 4: Integrated Cognitive Systems
  phase6    Phase 6: Final Integration
  quick     Quick Integration Verification
  demo      Demo Experiment
```

### Benchmark Command
```bash
nlm benchmark [TYPE]

Types:
  brain       Benchmark brain creation
  simulation  Benchmark simulation performance
  memory      Benchmark memory systems
```

## Development

### Building
```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
make pynlm
```

### Running Tests
```bash
# Unit tests
cd tests
./test_runner

# Integration tests
./build/nlm_phase6_demo
```

### Adding New Features
1. Add new components in `src/` directory
2. Update `CMakeLists.txt` for new targets
3. Add Python bindings in `python/bindings.cpp`
4. Update documentation in `docs/`
5. Add tests in `tests/`

## Research Applications

### 1. Neuroscience
- Modeling biological neural systems
- Studying learning and memory
- Investigating developmental processes

### 2. Robotics
- Developing autonomous agents
- Creating adaptive controllers
- Implementing cognitive architectures

### 3. Artificial Intelligence
- Developing general intelligence systems
- Creating neuromorphic computing platforms
- Building cognitive architectures

### 4. Machine Learning
- Comparing with traditional ML models
- Hybrid neural-symbolic systems
- Meta-learning and transfer learning

## Performance

### Benchmark Results
```
Brain Creation Benchmark:
  5 brain creations: 12.34s total
  Average: 2.47s per brain

Simulation Benchmark:
  1000 simulation steps: 5.67s total
  Average step time: 5.67ms
  Steps per second: 176
```

### Optimization Features
- **Memory pools**: Efficient memory management
- **Event queues**: Optimized spike event handling
- **SIMD vectorization**: Parallel processing
- **Parallel processing**: Multi-core support
- **Checkpointing**: State persistence
- **Replay**: Experience consolidation

## Troubleshooting

### Common Issues

#### 1. Build Errors
```bash
# Check for build dependencies
apt-get update
apt-get install build-essential cmake python3-dev

# Re-run build
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### 2. Python Import Errors
```bash
# Install Python dependencies
pip install pybind11 numpy matplotlib

# Build Python bindings
python setup.py build_ext --inplace
```

#### 3. Runtime Errors
```python
import traceback

try:
    brain.step(step)
except Exception as e:
    print(f"Error: {e}")
    traceback.print_exc()
```

### Debugging
```python
import nlm.nlm_interface as nlm_interface

# Enable verbose logging
nlm.Logger.getGlobal().setLevel(nlm.LogLevel.Debug)

# Create brain with debug config
config = nlm.create_default_config()
config.log_level = "DEBUG"

brain = nlm.create_brain(config)
brain.initialize()
```

## Future Enhancements

### 1. Enhanced Python API
- **scikit-learn integration**: Machine learning utilities
- **TensorFlow/PyTorch**: Hybrid neural systems
- **MLflow**: Experiment tracking

### 2. Advanced Features
- **Dynamic architectures**: Runtime modification
- **Transfer learning**: Knowledge transfer
- **Meta-learning**: Learning to learn
- **Multi-agent systems**: Social learning

### 3. Performance Improvements
- **GPU acceleration**: CUDA support
- **Distributed computing**: Multi-machine training
- **Model compression**: Efficient models

### 4. Application Domains
- **Neuroscience**: Brain modeling
- **Healthcare**: Diagnostic systems
- **Gaming**: Intelligent agents
- **Education**: Adaptive learning

## License

MIT License

## Authors

NLM Research Team

## Version

0.1.0 (Enhanced)

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues