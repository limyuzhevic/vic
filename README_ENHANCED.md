# NLM (熙然) - Neural Learning Machine
# Phase 6: FINAL INTEGRATION - Enhanced Version
# An Experimental Artificial Developmental Brain

**This is an enhanced, production-ready version of the NLM brain simulation framework.**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**IMPORTANT**: This version (Phase 6) represents the final integration of all systems, not Phase 2, 3, or 4.

## Key Improvements

### 🚀 **Enhanced User Experience**

1. **Better Command-Line Interface**:
   - Subcommands for different modes: `phase6`, `phase2`, `phase3`, `phase4`, `info`, `config`, `benchmark`
   - Comprehensive help system with examples
   - Support for configuration files, JSON/YAML export, and environment variables

2. **Robust Configuration System**:
   - JSON and YAML support (previously simple key=value)
   - Configuration validation and schema checking
   - Environment variable support
   - Time-based configuration updates
   - Configuration groups for organization

3. **Comprehensive Python Bindings**:
   - Full API for configuration, simulation, and experimentation
   - Memory management utilities
   - Performance monitoring
   - Checkpoint management
   - Experiment running

4. **Production Features**:
   - Proper error handling and validation
   - Performance monitoring and benchmarking
   - Advanced configuration management
   - Comprehensive documentation

### 🔧 **Technical Improvements**

1. **Enhanced Core Libraries**:
   - Improved Config class with validation
   - Better error handling and reporting
   - Memory management improvements
   - Performance optimizations

2. **Integration Testing**:
   - Phase 6 integration verification
   - Individual system testing
   - Checkpoint/load testing
   - Replay system testing

3. **Development Tools**:
   - Built-in examples and tutorials
   - Performance benchmarking
   - Configuration validation
   - Debug and monitoring tools

## 🚀 Quick Start

### Command Line

```bash
# Run Phase 6 integration demo (default)
nlm

# Run with custom configuration
nlm phase6 --neuron-count 500 --max-steps 5000

# Run specific phase demo
nlm phase2                    # Basic neural computation
nlm phase3                    # Sensory-motor loop
nlm phase4                    # Advanced cognition

# Show system information
nlm info

# Test configuration file
nlm config configs/default.cfg

# Run performance benchmark
nlm benchmark

# Get help
nlm --help
```

### Python

```python
import nlm

# Create brain with default settings
brain = nlm.Brain()

# Run simulation
result = brain.run_steps(1000)
print(f"Average reward: {result['total_reward']:.4f}")
print(f"Average firing rate: {result['average_firing_rate']:.2f} Hz")

# Save checkpoint
brain.save_checkpoint("my_brain.bin")

# Load checkpoint
new_brain = nlm.Brain()
new_brain.load_checkpoint("my_brain.bin")
```

### Configuration Management

```python
import nlm

# Create configuration
config = nlm.Config()
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.002)
config.set("random_seed", 12345)

# Create brain with custom config
brain = nlm.Brain(config)
```

## 📚 Examples

Run the examples to learn how to use NLM:

```bash
cd /workspace/app
python3 examples/basic_examples.py
```

The examples demonstrate:
- Basic usage and simulation
- Configuration management
- Memory systems
- Neuromodulation
- Checkpointing
- Experiment integration
- Performance monitoring

## 🏗️ Architecture

NLM operates as a closed-loop system:

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

## 🧪 Available Systems

### **Memory Systems**
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations

### **Neuromodulation**
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signal

### **Prediction System**
- **Neural Prediction**: Forward model for sensory prediction
- **Prediction Error**: Discrepancy detection

### **Cognition**
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern discovery
- **Attention**: Competitive selection dynamics

### **Development**
- **Developmental Stages**: Age-dependent plasticity
- **Structural Plasticity**: Synaptogenesis and pruning
- **Maturation**: Neural refinement

### **Sensory Processing**
- **Vision**: Visual input processing
- **Audio**: Auditory input processing
- **Internal Signals**: Interoception

### **Motor System**
- **Action Selection**: Behavioral output
- **Motor Commands**: Environmental interaction

## 📊 Integration Status

Phase 6 successfully integrates all systems:

✅ **Memory Integration**: All memory systems connected to neural processing  
✅ **Neuromodulation Integration**: Neuromodulators affect plasticity and dynamics  
✅ **Prediction Integration**: Prediction system central to learning loop  
✅ **Development Integration**: Developmental stages modulate plasticity rates  
✅ **Checkpointing**: Full state save/load with integrity validation  
✅ **Replay**: Episodic memory replay during simulation  
✅ **Cohesion**: Complete brain loop functions coherently  

## ⚙️ Configuration

### Command Line Options

```bash
nlm [OPTIONS] [MODE]

MODE:
  phase6                    Run Phase 6 integration demo (default)
  phase2                    Run Phase 2: Basic neural computation
  phase3                    Run Phase 3: Sensory-motor loop
  phase4                    Run Phase 4: Advanced cognition
  info                      Show system information
  config                    Test configuration file
  benchmark                 Run performance benchmark

OPTIONS:
  --help, -h                Show help message
  --version, -v             Show version information
  --verbose, -V             Enable verbose output
  --benchmark, -b           Run in benchmark mode
  --config <file>           Configuration file (default: configs/default.cfg)
  --neuron-count <count>    Number of neurons (default: 1000)
  --max-steps <steps>       Maximum simulation steps (default: 10000)
  --checkpoint <path>       Checkpoint file path (default: ./checkpoint_test.bin)
```

### Configuration File Format

NLM uses a flexible configuration system supporting:

**Key=Value format** (traditional):
```
neuron_count = 1000
simulation_timestep = 0.001
random_seed = 42
```

**JSON format**:
```json
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "enable_checkpointing": true
}
```

**YAML format**:
```yaml
neuron_count: 1000
simulation_timestep: 0.001
random_seed: 42
enable_checkpointing: true
```

### Configuration Parameters

#### General
- `neuron_count`: Number of neurons in the network
- `region_count`: Number of neural regions
- `connection_probability`: Probability of connections (0.0-1.0)
- `simulation_timestep`: Time step in seconds
- `random_seed`: Random seed for reproducibility

#### Plasticity
- `stdp_ltp_weight`: STDP LTP weight factor
- `stdp_ltd_weight`: STDP LTD weight factor
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Synapse formation rate
- `pruning_rate`: Synapse pruning rate

#### Memory
- `enable_working_memory`: Enable working memory
- `enable_episodic_memory`: Enable episodic memory
- `enable_associative_memory`: Enable associative memory

#### Development
- `enable_development`: Enable developmental plasticity
- `development_rate`: Development speed

#### Checkpointing
- `enable_checkpointing`: Enable checkpoint saving
- `checkpoint_path`: Checkpoint file path
- `checkpoint_interval`: Checkpoint save interval

#### Environment
- `environment_type`: Environment type (simple, complex)
- `environment_width`, `environment_height`: Environment dimensions

## 🐍 Python API Reference

### Core Classes

#### `nlm.Config`
Configuration management class.

**Methods**:
- `__init__(filepath=None)`: Constructor
- `set(key, value)`: Set configuration value
- `get_int(key)`: Get integer value
- `get_double(key)`: Get floating point value
- `get_string(key)`: Get string value
- `get_bool(key)`: Get boolean value
- `has(key)`: Check if key exists
- `load_from_json(json_string)`: Load from JSON
- `load_from_yaml(yaml_string)`: Load from YAML
- `to_json()`: Export to JSON
- `to_yaml()`: Export to YAML
- `validate()`: Validate configuration

#### `nlm.Brain`
Main brain simulation class.

**Methods**:
- `__init__(config=None)`: Constructor
- `run_steps(steps)`: Run simulation for specified steps
- `get_stats()`: Get brain statistics
- `save_checkpoint(filepath)`: Save brain state
- `load_checkpoint(filepath)`: Load brain state
- `get_working_memory()`: Get working memory info
- `get_episodic_memory()`: Get episodic memory info
- `get_neuromodulation()`: Get neuromodulation levels
- `get_development_stage()`: Get current development stage

#### `nlm.Phase6Experiment`
Phase 6 integration experiment.

**Methods**:
- `verify_integration()`: Verify all systems integration
- `run_experiment(config)`: Run complete integration experiment
- `test_memory_integration()`: Test memory integration
- `test_neuromodulation_integration()`: Test neuromodulation integration
- `test_checkpointing()`: Test checkpoint functionality
- `test_replay()`: Test replay system

#### `nlm.PerformanceMonitor`
Performance monitoring utilities.

**Methods**:
- `get_system_stats()`: Get system performance statistics
- `benchmark_simulation(steps, neurons)`: Benchmark simulation performance

## 🔧 Building from Source

### Prerequisites

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install -y build-essential cmake g++ git

# Install Python dependencies
pip install pybind11 pyyaml nlohmann-json

# Install YAML-CPP
apt-get install -y libyaml-cpp-dev
```

### Build Instructions

```bash
# Create build directory and configure
cd /workspace/app
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build all targets
make -j4

# Install Python bindings (if desired)
make install
```

### Running

```bash
# Run Phase 6 integration demo
./nlm

# Run Phase 2 demo (original functionality)
./nlm_phase2_demo

# Run Phase 3 demo
./nlm_phase3_demo

# Run Phase 4 demo
./nlm_phase4_demo

# Run Phase 6 demo (from original project)
./nlm_phase6_demo
```

## 📈 Performance

NLM includes performance optimizations:

- **Memory Pools**: Efficient memory allocation
- **Event Queues**: Optimized spike processing
- **SIMD Vectorization**: Parallel processing
- **Parallel Processing**: Multi-threading support
- **Checkpoint Compression**: Efficient state storage

### Performance Monitoring

Use the `nlm benchmark` command to measure performance, or monitor system resources with:

```python
stats = nlm.PerformanceMonitor.get_system_stats()
print(f"CPU: {stats['cpu_usage']:.1f}%")
print(f"Memory: {stats['memory_usage']:.1f}%")
print(f"Speed: {stats['simulation_speed']:.0f} steps/sec")
```

## 🧪 Testing

### Running Tests

```bash
# Run unit tests
./tests/test_config
./tests/test_brain
./tests/test_neuron
# ... and other test executables
```

### Test Framework

The project includes comprehensive unit tests:

- **Config Tests**: Configuration validation and parsing
- **Brain Tests**: Neural network functionality
- **Neuron Tests**: Single neuron behavior
- **Synapse Tests**: Synaptic dynamics
- **Plasticity Tests**: STDP and Hebbian learning
- **Memory Tests**: Memory system functionality
- **Integration Tests**: System integration

## 📚 Documentation

### Core Concepts

**Neurons**: Leaky Integrate-and-Fire (LIF) neurons with realistic dynamics

**Synapses**: Event-driven spike propagation with delays

**Plasticity**: STDP, Hebbian, and structural plasticity rules

**Memory**: Working memory for active processing, episodic memory for experience storage

**Neuromodulation**: Dopamine, curiosity, and novelty systems

**Development**: Age-dependent plasticity and maturation

### Scientific Background

NLM investigates whether artificial brains can:

- Develop complex behaviors from simple initial conditions
- Learn from experience through multiple plasticity mechanisms
- Maintain and consolidate memories over time
- Balance exploration and exploitation through neuromodulation
- Adapt through developmental changes
- Integrate multiple cognitive systems into coherent behavior

## 🆘 Troubleshooting

### Common Issues

#### "Configuration file not found"
```bash
# Use default configuration
nlm phase6

# Or create a config file
mkdir -p configs
echo "neuron_count = 500" > configs/default.cfg
nlm phase6 --config configs/default.cfg
```

#### "Python module not found"
```bash
# Install Python bindings
pip install -e .

# Or build and install
cd /workspace/app
cmake -B build -H. -DCMAKE_INSTALL_PREFIX=/usr/local
make -C build install
```

#### "Memory allocation failed"
```bash
# Increase available memory
# Or reduce simulation parameters
nlm phase6 --neuron-count 100 --max-steps 1000
```

### Getting Help

- **Command line help**: `nlm --help`
- **Python examples**: `python3 examples/basic_examples.py`
- **Documentation**: This file and the `docs/` directory
- **Community**: GitHub repository and issue tracker

## 📄 License

MIT License. See `LICENSE` file for details.

## 👥 Authors

Research project — See docs for scientific background.

## 📅 Version History

- **v6.0.0**: Final integration, enhanced CLI, Python bindings, improved configuration
- **v5.0.0**: Advanced cognition integration
- **v4.0.0**: Cognitive systems integration
- **v3.0.0**: World interaction loop
- **v2.0.0**: Real neural computation

## 🎯 Mission

**"To investigate whether memory systems can integrate with neural dynamics, whether neuromodulation can affect plasticity in a coordinated way, whether prediction can become a central organizing principle, whether developmental stages can modulate learning, and whether the complete brain loop functions coherently."**

NLM is a research tool, not a claim of human-like intelligence or consciousness. It investigates computational principles that may inform our understanding of biological brains.

---

*For support and contributions, visit the NLM project repository.*
