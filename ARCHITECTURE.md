# NLM - Improved Architecture and Documentation

# Architecture Overview

The NLM (Neural Learning Machine) has been significantly improved with the following enhancements:

## Core Improvements

### 1. Configuration System
- **Fixed configuration issues**: Corrected duplicate entries and improved formatting
- **Advanced configuration options**: Added new settings for development, neuromodulation, and memory systems
- **Better configuration validation**: Improved error handling and configuration management

### 2. Python Bindings
- **Enhanced Python interface**: Created `python/nlm_interface.py` with user-friendly API
- **Improved documentation**: Added comprehensive docstrings and examples
- **Better integration**: Direct access to all NLM functionality from Python

### 3. Command-Line Interface
- **Comprehensive CLI**: Created `nlm_cli.py` with extensive command options
- **User-friendly commands**: Support for running experiments, managing configurations, and benchmarking
- **Help system**: Integrated help system with examples and usage instructions

### 4. Logging and Debugging
- **Enhanced logging**: Improved log formatting with JSON support and log rotation
- **Performance monitoring**: Built-in performance metrics and timing
- **Better error handling**: More informative error messages and debugging information

### 5. Examples and Tutorials
- **Comprehensive examples**: Created `examples_and_tutorials.py` with 8 practical examples
- **Educational content**: Examples for beginners, intermediate users, and advanced users
- **Real-world applications**: Examples showing integration with Python ecosystem

## Code Organization

### Directory Structure
```
/workspace/app
├── CMakeLists.txt              # Build configuration
├── pyproject.toml             # Python package configuration
├── README.md                  # Main documentation
├── configs/                   # Configuration files
│   └── default.cfg            # Default configuration
├── docs/                      # Technical documentation
├── python/                    # Python bindings and interface
│   ├── nlm_interface.py      # High-level Python API
│   └── bindings.cpp          # C++ bindings (for pybind11)
├── src/                       # C++ source code
│   ├── core/                 # Core systems
│   │   ├── Config/           # Configuration system
│   │   ├── Logger/           # Logging system (enhanced)
│   │   ├── Random/           # Random number generation
│   │   └── SimulationClock/  # Time management
│   ├── brain/               # Core neural systems
│   ├── agent/              # Agent interface
│   ├── world/              # World simulation
│   └── experiments/         # Pre-built experiments
├── tests/                    # Unit tests
├── nlm_cli.py                # Command line interface
├── examples_and_tutorials.py  # Examples and tutorials
└── README.md                 # Project documentation
```

### Key Components

#### Core Systems
- **Config**: Configuration management with support for multiple formats
- **Logger**: Enhanced logging with performance monitoring and JSON support
- **Random**: Reproducible random number generation
- **SimulationClock**: Precise time management

#### Neural Systems
- **Brain**: Main neural simulation coordinator
- **Neuron**: LIF neuron dynamics implementation
- **Synapse**: Synaptic connections and plasticity
- **SpikeSystem**: Event-driven spike propagation

#### Plasticity Systems
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian**: Hebbian learning rules
- **StructuralPlasticity**: Synaptogenesis and pruning

#### Memory Systems
- **WorkingMemory**: Transient information storage
- **EpisodicMemory**: Experience recording and replay
- **AssociativeMemory**: Pattern associations

#### Neuromodulation
- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Learning signal computation

#### Cognitive Systems
- **PredictionSystem**: Forward models and prediction
- **NeuralPlanner**: Action planning
- **ConceptFormation**: Pattern discovery
- **AttentionalSelection**: Competitive attention

#### World and Environment
- **SimpleWorld**: 2D world simulation
- **SensoryInput**: Sensory transduction
- **MotorSystem**: Action selection

## Usage Examples

### 1. Basic Usage
```python
import nlm

# Create default configuration
config = nlm.create_default_config()
brain = nlm.create_brain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### 2. Agent-Based Simulation
```python
# Create complete agent setup
brain, agent_brain, world = nlm.create_agent()

# Run simulation
for step in range(500):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process through agent
    agent_brain.processSensoryInput(percept)
    brain.receiveSensoryInput(percept)
    
    # Simulate step
    brain.step(step)
    
    # Get action
    action = brain.produceAction()
    result = world.applyMotorCommand(action)
    
    # Apply neuromodulation
    agent_brain.applyRewardModulation(result.reward, 0.0)
```

### 3. Command Line Interface
```bash
# Run Phase 6 integration demo
nlm run --phase6

# Manage configuration
nlm config show
nlm config create custom.cfg

# Run experiment
nlm experiment phase6

# Benchmark performance
nlm benchmark simulation
```

### 4. Running Examples
```bash
# Run all examples
python examples_and_tutorials.py

# Run specific example
python examples_and_tutorials.py basic
python examples_and_tutorials.py simple
python examples_and_tutorials.py phase6
```

## Build Instructions

### Build the C++ Core
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Python Bindings
```bash
cd build
make pynlm
```

### Install
```bash
make install
```

## Testing

### Unit Tests
```bash
cd tests
./test_runner
```

### Integration Tests
```bash
./nlm_phase6_demo
./nlm_phase4_demo
```

## Advanced Features

### 1. Custom Configuration
```python
config = nlm.Config()
config.random_seed = 12345
config.neuron_count = 2000
config.stdp_ltp_weight = 0.015
config.development_enabled = True
config.curiosity_enabled = True
```

### 2. Performance Benchmarking
```python
# Benchmark brain creation
nlm.benchmark_brain_creation()

# Benchmark simulation
nlm.benchmark_simulation(steps=1000)
```

### 3. Checkpointing and Persistence
```python
# Save brain state
brain.save("my_brain.checkpoint")

# Load brain state
brain.load("my_brain.checkpoint")
```

### 4. Session Management
```python
# Use NLM session for complex experiments
session = nlm.NLMSession(config)
session.setup()

# Run simulation
result = session.run(steps=1000)

# Save checkpoint
session.save_checkpoint("session_checkpoint")
```

## Research and Development

### 1. Experiment Tracking
```python
# Create reproducible experiment sessions
experiment_dir = "experiment_2024_01_17"
config = nlm.create_experiment_config(seed=42, neurons=1000)

# Run and save results
result = run_experiment(config, experiment_dir)
```

### 2. Data Analysis
```python
import pandas as pd

# Load experiment results
df = pd.read_csv("experiment_results.csv")

# Analyze neural activity
activity_stats = df.groupby('step').agg({
    'firing_rate': ['mean', 'std'],
    'reward': 'sum'
})
```

### 3. Visualization
```python
import matplotlib.pyplot as plt

# Plot neural activity
plt.plot(activation_history)
plt.xlabel('Time (steps)')
plt.ylabel('Normalized Activity')
plt.title('Neural Activity from NLM Simulation')
plt.show()
```

## Future Improvements

### 1. Additional Python Libraries
- **scikit-learn**: For comparing NLM with traditional ML models
- **TensorFlow/PyTorch**: For hybrid neural systems
- **MLflow**: For experiment tracking and management

### 2. Enhanced Features
- **Dynamic architectures**: Runtime modification of neural structures
- **Transfer learning**: Knowledge transfer between tasks
- **Meta-learning**: Learning how to learn

### 3. Application Domains
- **Neuroscience**: Modeling biological neural systems
- **Robotics**: Developing autonomous agents
- **AI**: Creating general intelligence systems

## License

MIT License

## Authors

NLM Research Team

## Version

0.1.0 (Enhanced)