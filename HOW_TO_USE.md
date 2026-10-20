# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source (Modern CMake)
2. Installing the Python library
3. Using the NLM Python API (Advanced examples)

---

## Part 1: Building the C++ Project from Source (Modern CMake)

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps (Recommended)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with modern CMake features (Recommended)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCLANG_TIDY=ON \
    -DENABLE_TESTS=ON \
    -DENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_JSON_CONFIG=ON

# Build all targets with parallel jobs
make -j$(nproc)

# Install to system (optional)
sudo make install
```

### Traditional CMake Configuration

```bash
# Legacy build (still supported)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test (NEW) |
| `nlm_test` | Executable | Unit test suite |

### Advanced Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG_SYMBOLS=ON

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with JSON configuration support
cmake .. -DNLM_ENABLE_JSON_CONFIG=ON

# Build with advanced Python bindings
cmake .. -DENABLE_PYTHON_BINDINGS=ON -DPYTHON_EXECUTABLE=$(which python3)
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install with modern Python packaging
pip install -e . --no-build-isolation

# Or install in development mode
pip install -e .

# Build only (without installing)
python -m build
pip install dist/*.whl
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Using Advanced Python Features

```bash
# Install with optional dependencies for advanced features
pip install .[dev]

# For development with all dependencies
pip install -e .[dev]
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Test JSON configuration support
if hasattr(config, 'loadFromJson'):
    print("✓ JSON configuration support available")
    # Test JSON loading
    config.loadFromFile("configs/default.json")
    print("✓ JSON config loaded successfully")
```

---

## Part 3: Using the Python Library (Advanced Examples)

### Advanced Basic Usage

#### Creating and Configuring a Brain with JSON

```python
import pynlm
import json

# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure using JSON-style parameters (if supported)
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.02)
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.novelty.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Save configuration in modern JSON format
config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")
```

#### Complete Agent Simulation with Advanced Features

```python
import pynlm
import time

def advanced_agent_simulation(num_steps=2000, world_size=30):
    """Run a complete agent simulation with advanced features."""
    
    # 1. Create and configure with JSON-style parameters
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 3)
    config.set("simulation.timestep", 0.005)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("neuromodulation.curiosity.baseline", 0.7)
    config.set("neuromodulation.novelty.enable", True)
    config.set("neuromodulation.novelty.sensitivity", 0.9)
    config.set("neuromodulation.reward.discount_factor", 0.95)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
    config.set("development.enable", True)
    config.set("development.maturation_rate", 0.001)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create advanced agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world with advanced settings
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=12,
        visionHeight=12,
        max_energy=200.0
    )
    world.reset()
    world.setAgentStart(world_size/2, world_size/2)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePrediction(True)
    agent.enableAttention(True)
    
    # 7. Setup advanced monitoring
    metrics = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'reward_sums': [],
        'development_stages': []
    }
    
    print(f"Starting advanced agent simulation ({num_steps} steps, {world_size}x{world_size} world)...")
    print(f"Features enabled: Reward={agent.enableRewardModulation()}, "
          f"Curiosity={agent.enableCuriosity()}, "
          f"Novelty={agent.enableNoveltyDetection()}, "
          f"Development={agent.enableDevelopment()}, "
          f"Structural Plasticity={agent.enableStructuralPlasticity()}")
    
    # 8. Run simulation loop with advanced analytics
    for step in range(num_steps):
        # Update world with variable timestep
        timestep = config.getOr<double>("simulation.timestep", 0.01)
        world.update(timestep)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain with prediction
        agent.processSensoryInput(percept)
        
        # Run brain step with advanced timing
        brain.step(step, step * timestep)
        
        # Get predicted action with confidence
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world with timing
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply advanced reward modulation with prediction error
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = 0.0  # Could be from agent's prediction system
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development with time passed
        agent.updateDevelopment(timestep)
        
        # Collect advanced metrics
        if step % 10 == 0:
            metrics['steps'].append(step)
            metrics['firing_rates'].append(brain.getFiringNeuronCount() / brain.getTotalNeuronCount())
            metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            metrics['novelty_levels'].append(agent.getNoveltyLevel())
            metrics['reward_sums'].append(reward)
            metrics['development_stages'].append(brain.getDevelopmentalStage())
            
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Firing rate: {metrics['firing_rates'][-1]:.3f}")
                print(f"  Curiosity: {metrics['curiosity_levels'][-1]:.3f}")
                print(f"  Novelty: {metrics['novelty_levels'][-1]:.3f}")
                print(f"  Reward: {metrics['reward_sums'][-1]:.3f}")
                print(f"  Development: {metrics['development_stages'][-1]}")
    
    # 9. Save advanced metrics
    save_metrics(metrics, f"advanced_simulation_metrics_{num_steps}_steps.json")
    
    # 10. Return brain state for analysis
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'metrics': metrics,
        'config': config
    }

# Save metrics function (simplified)
def save_metrics(metrics, filename):
    with open(filename, 'w') as f:
        json.dump(metrics, f, indent=2)
    print(f"Metrics saved to {filename}")

# Advanced agent example using the function above
if __name__ == "__main__":
    result = advanced_agent_simulation(1000, 25)
    print("Advanced simulation completed!")
    print(f"Final metrics: {len(result['metrics']['steps'])} data points collected")
```

#### Configuration Management with JSON

```python
import pynlm
import json

# Load modern JSON configuration
config = pynlm.createDefaultConfig()

# Load from JSON file (if supported)
try:
    config.loadFromFile("configs/default.json")
    print("✓ JSON configuration loaded successfully")
except Exception as e:
    print(f"Note: JSON loading not fully implemented yet: {e}")

# Create configuration from scratch using nested keys
config = pynlm.createDefaultConfig()

# Set nested configuration values
config.set("simulation.timestep", 0.001)
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromodulation.dopamine.scale", 1.0)
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.curiosity.baseline", 0.5)
config.set("neuromodulation.novelty.enable", True)
config.set("neuromodulation.novelty.sensitivity", 0.8)

# Export to JSON format
config.saveToFile("my_advanced_config.json")
print("Advanced configuration saved to my_advanced_config.json")

# Display configuration summary
print("\nConfiguration Summary:")
print(config.summary())
```

---

## Part 4: Configuration Options (Modern JSON Format)

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep in seconds |
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 2 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Synapse connectivity probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.01 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromodulation.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromodulation.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromodulation.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromodulation.curiosity.baseline` | float | 0.5 | Baseline curiosity level |
| `neuromodulation.novelty.enable` | bool | true | Enable novelty detection |
| `neuromodulation.novelty.sensitivity` | float | 0.8 | Novelty detection sensitivity |
| `neuromodulation.reward.discount_factor` | float | 0.99 | Reward discount factor |
| `neuromodulation.reward.prediction_error` | bool | true | Enable reward prediction error |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 50 | Working memory capacity |
| `memory.working_memory.decay_rate` | float | 0.95 | Working memory decay rate |
| `memory.episodic.consolidation_interval` | int | 1000 | Episodic memory consolidation interval |
| `memory.associative.learning_rate` | float | 0.02 | Associative memory learning rate |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate |
| `development.pruning_rate` | float | 0.0001 | Pruning rate |
| `development.maturation_rate` | float | 0.0005 | Maturation rate |
| `development.enable` | bool | true | Enable development |

### Sensory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `sensory.vision.width` | int | 8 | Vision width |
| `sensory.vision.height` | int | 8 | Vision height |
| `sensory.audio.channels` | int | 1 | Audio channels |
| `sensory.audio.sample_rate` | int | 1000 | Audio sample rate |

### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment.name` | string | GridWorld | Environment name |
| `environment.width` | int | 10 | Environment width |
| `environment.height` | int | 10 | Environment height |
| `environment.max_energy` | float | 100.0 | Maximum energy |

### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `logging.level` | string | INFO | Log level (DEBUG, INFO, WARNING, ERROR) |
| `logging.to_file` | bool | false | Log to file |
| `logging.filename` | string | nlm.log | Log filename |
| `logging.console` | bool | true | Log to console |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction (Advanced)

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Performance Benchmark

```python
import pynlm
import time

def benchmark_brain_performance(neuron_counts=[100, 500, 1000, 2000]):
    """Benchmark brain performance with different neuron counts."""
    
    results = {}
    
    for neuron_count in neuron_counts:
        print(f"Benchmarking {neuron_count} neurons...")
        
        # Create brain with specific neuron count
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        init_start = time.time()
        brain.initialize()
        init_time = time.time() - init_start
        
        # Measure step time
        step_start = time.time()
        for i in range(100):
            brain.step(i)
        step_time = time.time() - step_start
        
        results[neuron_count] = {
            'initialization_time': init_time,
            'step_time_per_step': step_time / 100,
            'total_time': init_time + step_time,
            'spikes_per_step': brain.getTotalSpikeCount() / 100
        }
        
        print(f"  Initialization: {init_time:.3f}s")
        print(f"  Step rate: {1/(step_time/100):.1f} steps/s")
        print(f"  Spikes per step: {results[neuron_count]['spikes_per_step']:.1f}")
    
    return results

# Run benchmark
benchmark_results = benchmark_brain_performance()
```

### Configuration Validation

```python
import pynlm

def validate_configuration(config_file):
    """Validate configuration file."""
    
    config = pynlm.createDefaultConfig()
    
    try:
        # Try to load configuration
        if config.loadFromFile(config_file):
            print(f"✓ Configuration loaded from {config_file}")
            
            # Validate configuration
            is_valid = True
            for key in config.getKeys():
                std::optional<pynlm.ConfigValue> value = config.get<pynlm.ConfigValue>(key)
                if (value.has_value() && !config.validate(key, value.value())) {
                    print(f"  ✗ Invalid value for key '{key}': {value.value()}")
                    is_valid = false
                }
            
            if (is_valid) {
                print("✓ Configuration is valid")
            } else {
                print("✗ Configuration has validation errors")
            }
            
            return is_valid
        else {
            print(f"✗ Failed to load configuration from {config_file}")
            return false
        }
    } catch (const std::exception& e) {
        print(f"✗ Error loading configuration: {e.what()}")
        return false
    }

# Validate configuration
validate_configuration("configs/default.json")
validate_configuration("configs/default.cfg")
```

---

## Part 6: Advanced Command Line Interface

### Usage Examples

```bash
# Show help with all available commands
./nlm --help

# Run with advanced options
./nlm --run-tests --enable-curiosity --export-metrics=simulation_stats.json

# Configure advanced features
./nlm --config=advanced_config.json --enable-structural-plasticity --enable-reward-modulation

# Run with different timestep
./nlm --simulation_timestep=0.005 --enable-curiosity --enable-novelty

# Export brain state for research
./nlm --export-brain=checkpoint1.bin --export-metrics=simulation_stats.json --export-neurons=neuron_data.csv

# Run validation
./nlm --validate-config --config=my_config.json

# Show available commands
./nlm --show-commands
```

### Command Line Options

#### Configuration Commands
- `--config=<file>` Load configuration from file (supports JSON and text)
- `--config-help` Display supported config formats and examples
- `--validate-config` Validate current configuration
- `--export-config=<file>` Export configuration to file
- `--import-config=<file>` Import configuration from file

#### Simulation Commands
- `--run-tests` Run comprehensive neural network tests
- `--run-basic-test` Run basic connectivity test
- `--run-plasticity-test` Run plasticity learning experiment
- `--run-stdp-test` Run STDP verification test
- `--run-integration-test` Run complete integration test

#### Data Export Commands
- `--export-brain=<file>` Save brain state to checkpoint file
- `--export-neurons=<file>` Export neuron data
- `--export-synapses=<file>` Export synapse data
- `--export-metrics=<file>` Export simulation metrics

#### Advanced Features
- `--enable-curiosity` Enable curiosity-driven exploration
- `--enable-novelty` Enable novelty detection
- `--enable-development` Enable developmental stages
- `--enable-structural-plasticity` Enable structural plasticity
- `--enable-reward-modulation` Enable reward-based learning
- `--enable-neuromodulation` Enable neuromodulation

#### Analysis Commands
- `--analyze-learning` Analyze learning patterns
- `--analyze-plasticity` Analyze plasticity effects
- `--analyze-connectivity` Analyze network connectivity
- `--analyze-performance` Analyze performance metrics

#### System Commands
- `--help` Show this help message
- `--version` Show NLM version
- `--debug` Enable debug logging
- `--benchmark` Run performance benchmarks
- `--update` Check for updates
- `--license` Show license information
- `--about` Show about information

---

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

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Runtime Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected to brain

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure agent's motor system is enabled

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters (neuron count, connection probability)

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Key Improvements in This Version

1. **Modern JSON Configuration**: Native support for JSON configuration files with validation
2. **Enhanced Documentation**: Comprehensive examples and advanced usage patterns
3. **Improved Build System**: Modern CMake with advanced features
4. **Advanced CLI**: Comprehensive command-line interface for research workflows
5. **Performance Optimizations**: Parallel building, SIMD support, and memory pools
6. **Python Integration**: Full Python bindings with advanced features
7. **Configuration Validation**: Automatic validation of configuration files
8. **Backward Compatibility**: Legacy text configs still supported
9. **Research Features**: Advanced analysis and export tools for researchers
10. **Modern Development**: C++20, modern CMake practices, and comprehensive testing

This represents a significant improvement in the NLM project, making it more accessible, powerful, and suitable for both beginners and advanced researchers in computational neuroscience.
