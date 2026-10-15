# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Improvements and New Features

### Enhanced Configuration System

#### JSON and YAML Support
The configuration system now supports modern configuration formats:

**JSON Configuration (nlm.json):**
```json
{
  "neuron_count": 2000,
  "region_count": 2,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "simulation_timestep": 0.001,
  "random_seed": 42
}
```

**YAML Configuration (nlm.yaml):**
```yaml
neuron_count: 2000
region_count: 2
connection_probability: 0.15
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
simulation_timestep: 0.001
random_seed: 42
```

#### Backward Compatibility
The system still supports the original key=value format for existing configurations:

**Key=Value Configuration (nlm.cfg):**
```
# NLM Configuration File
neuron_count = 2000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
simulation_timestep = 0.001
random_seed = 42
```

### Advanced Command Line Interface

The enhanced NLM CLI provides comprehensive control over the neural simulation:

#### Basic Usage
```bash
# Run demo mode with 1000 steps
./nlm_enhanced --mode demo --steps 1000 --verbose

# Run specific experiment
./nlm_enhanced --mode experiment --experiment plasticity --save brain_state

# Run benchmark with profiling
./nlm_enhanced --mode benchmark --steps 5000 --profile --visualize

# Load saved brain state
./nlm_enhanced --mode demo --load saved_brain.json

# Custom configuration
./nlm_enhanced --mode demo --custom neuron_count=2000 --custom region_count=3
```

#### Available Options

**Mode Options:**
- `demo` - Demonstration mode (default)
- `experiment` - Scientific experiment mode
- `benchmark` - Performance benchmarking mode

**Experiment Types:**
- `plasticity` - Synaptic plasticity learning
- `stdp` - Spike-Timing-Dependent Plasticity
- `learning` - General learning behavior
- `all` - Run all experiment types

**Configuration Options:**
- `--config <file>` - Configuration file path (default: configs/default.cfg)
- `--custom <key>=<value>` - Override configuration parameter
- `--save <file>` - Save brain state to file
- `--load <file>` - Load brain state from file

**Performance Options:**
- `--enable-GPU` - Enable GPU acceleration (if available)
- `--no-SIMD` - Disable SIMD optimizations
- `--threads <n>` - Number of threads (default: 4)

**Debug & Visualization Options:**
- `--verbose` - Enable verbose output
- `--debug` - Enable debug mode
- `--visualize` - Enable visualization
- `--profile` - Enable performance profiling

**API Mode Options:**
- `--api-mode <mode>` - API access mode: `full`, `minimal`, `expert`
- `--inference-steps <n>` - Number of inference steps (default: 100)
- `--inference-interval <t>` - Inference interval in seconds (default: 0.1)

### Enhanced Brain Interface

The NLMBrain class provides a clean, user-friendly interface to the neural simulation:

#### Key Methods

```cpp
// Initialize brain with configuration
NLMBrain nlmBrain;
CLIOptions options;
options.maxNeurons = 2000;
options.maxRegions = 2;
options.mode = "experiment";
options.experimentType = "plasticity";

if (nlmBrain.initialize(options)) {
    // Run experiment
    nlmBrain.runExperiment("plasticity");
    
    // Save results
    nlmBrain.saveState("brain_state");
    
    // Print statistics
    nlmBrain.printStatistics();
}
```

#### Brain Features

- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Prediction System**: Forward models and prediction error signals
- **Cognition Systems**: Neural planning, concept formation, attention
- **Development System**: Synaptogenesis, pruning, maturation
- **Plasticity Rules**: STDP, Hebbian learning, structural plasticity

### Performance Improvements

#### Multi-threading Support
The enhanced CLI supports parallel processing:

```bash
./nlm_enhanced --mode benchmark --steps 10000 --threads 8
```

#### SIMD Optimizations
Vectorized operations for improved performance:
- Neuron state updates
- Synaptic weight calculations
- Memory operations

#### GPU Acceleration (if available)
```bash
./nlm_enhanced --mode benchmark --steps 50000 --enable-GPU
```

### Visualization and Debugging

#### Real-time Visualization
Enable visualization for monitoring simulation progress:

```bash
./nlm_enhanced --mode demo --steps 1000 --visualize
```

#### Debug Mode
Enable detailed debugging information:

```bash
./nlm_enhanced --mode demo --steps 100 --debug
```

#### Performance Profiling
Monitor performance bottlenecks:

```bash
./nlm_enhanced --mode benchmark --steps 5000 --profile
```

### Configuration Examples

#### High-Performance Configuration
```json
{
  "neuron_count": 5000,
  "region_count": 4,
  "connection_probability": 0.2,
  "simulation_timestep": 0.0005,
  "random_seed": 12345,
  "stdp_ltp_weight": 0.03,
  "stdp_ltd_weight": 0.025,
  "synaptogenesis_rate": 0.0005,
  "pruning_rate": 0.00005,
  "replay_interval": 200,
  "consolidation_interval": 5000,
  "checkpoint_dir": "./checkpoints",
  "enable_gpu": true,
  "enable_simd": true,
  "thread_count": 8
}
```

#### Scientific Research Configuration
```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.1
simulation_timestep: 0.001
random_seed: 42

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0

structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

development:
  stages: ["initial", "critical_period", "maturation", "adult"]
  plasticity_modifiers:
    initial: 1.0
    critical_period: 0.8
    maturation: 0.5
    adult: 0.2

memory:
  working_capacity: 100
  episodic_max_episodes: 1000
  associative_capacity: 500

neuromodulation:
  dopamine_baseline: 0.1
  curiosity_sensitivity: 0.5
  novelty_threshold: 0.3

experiments:
  plasticity_duration: 1000
  stdp_trials: 100
  learning_epochs: 500
```

### Building and Installation

#### Build with CMake
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j4
```

#### Build Options
```bash
cmake .. -DNLM_ENABLE_GPU=ON -DNLM_ENABLE_SIMD=ON -DNLM_THREAD_COUNT=8
```

#### Installation
```bash
cmake --install . --prefix /usr/local
```

### Testing

Run the test suite to verify functionality:

```bash
cd tests
./nlm_test_suite
```

Or run individual test executables:

```bash
./nlm_test_neuron
./nlm_test_synapse
./nlm_test_stdp
./nlm_test_config
./nlm_test_brain
```

### API Reference

#### Configuration Management
- `Config::loadFromFile()` - Load configuration from JSON/YAML/key=value files
- `Config::loadFromArgs()` - Load configuration from command line arguments
- `Config::saveToFile()` - Save configuration to file
- `Config::get<T>()` - Get configuration value
- `Config::getOr<T>()` - Get configuration value with default
- `Config::set()` - Set configuration value

#### Brain Interface
- `NLMBrain::initialize()` - Initialize brain with configuration
- `NLMBrain::runSimulation()` - Run simulation
- `NLMBrain::runExperiment()` - Run experiment
- `NLMBrain::saveState()` - Save brain state
- `NLMBrain::loadState()` - Load brain state
- `NLMBrain::printStatistics()` - Print brain statistics

### Performance Monitoring

The enhanced CLI includes built-in performance monitoring:

#### Real-time Statistics
- Total spike count
- Active neuron count
- Firing rate
- E/I ratio
- Memory usage
- CPU utilization

#### Profiling Support
- Step-by-step timing
- Memory allocation tracking
- Network performance metrics
- Bottleneck identification

### System Requirements

#### Minimum Requirements
- 2 GHz processor
- 4 GB RAM
- C++20 compatible compiler

#### Recommended for Full Performance
- 4+ GHz processor (8+ cores recommended)
- 16+ GB RAM
- GPU with CUDA support (optional)
- SSD storage

#### Supported Platforms
- Linux (all distributions)
- macOS (Intel and Apple Silicon)
- Windows (with WSL2 or native compilation)

### Troubleshooting

#### Common Issues

**Configuration File Not Found**
```bash
# Create default configuration
mkdir -p configs
cp configs/default.cfg configs/nlm.json
```

**GPU Not Available**
```bash
# Disable GPU acceleration
./nlm_enhanced --no-GPU --steps 1000
```

**Memory Usage Too High**
```bash
# Reduce neuron count
./nlm_enhanced --neurons 500 --steps 100
```

**Performance Issues**
```bash
# Disable SIMD optimizations
./nlm_enhanced --no-SIMD --steps 1000
```

#### Error Messages

**Configuration Parser Errors**
- "Failed to parse JSON file"
- "YAML parsing failed"
- "Invalid configuration format"

**Brain Initialization Errors**
- "Failed to initialize brain!"
- "Memory allocation failed"
- "Invalid configuration parameters"

**File I/O Errors**
- "Permission denied"
- "File not found"
- "Disk full"

### Acknowledgements

This project builds on the foundation of previous NLM phases:

- **Phase 1**: Project skeleton and core types
- **Phase 2**: Real neural computation (LIF neurons, STDP, Hebbian plasticity)
- **Phase 3**: World interaction loop and basic learning
- **Phase 4**: Prediction systems and cognitive mechanisms
- **Phase 5**: Performance optimizations and checkpoint system
- **Phase 6**: Integration of all systems into coherent artificial brain

### License

MIT License

Copyright (c) 2026 NLM Project Authors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.