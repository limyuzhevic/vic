# NLM (熙然) - Neural Learning Machine
# Comprehensive Guide for Users and Developers

## Introduction

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

This guide covers the enhanced features and improvements made to the NLM system, including:

- Enhanced Configuration System (JSON/YAML support)
- Advanced Command Line Interface (CLI)
- Improved Code Organization and Modularity
- Performance Profiling Tools
- Advanced Features for Expert Users

## Table of Contents

1. [Quick Start](#quick-start)
2. [Enhanced Configuration System](#enhanced-configuration-system)
   - Configuration File Formats
   - Configuration Commands
3. [Advanced CLI Interface](#advanced-cli-interface)
   - Available Commands
   - Command Line Options
4. [Code Organization and Structure](#code-organization-and-structure)
5. [Performance Optimization](#performance-optimization)
6. [Advanced Features for Expert Users](#advanced-features-for-expert-users)
   - Custom Neurons and Synapses
   - Advanced Plasticity Rules
   - Memory System Configuration
   - Neuromodulation Tuning
7. [Development Guide](#development-guide)
8. [Troubleshooting](#troubleshooting)
9. [Future Roadmap](#future-roadmap)

## Quick Start

### Basic Usage

```bash
# Run the default demonstration
./nlm run

# Run with custom configuration
./nlm run --config my_config.json --neurons 2000 --timestep 0.001

# Get help
./nlm --help

# Check version
./nlm --version
```

### Configuration Files

NLM now supports multiple configuration file formats:

**JSON Configuration (`config.json`):**
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "plasticity_learning_rate": 0.01,
    "dopamine_baseline": 0.1,
    "curiosity_threshold": 0.5,
    "enable_memory_systems": true,
    "enable_neuromodulation": true
}
```

**YAML Configuration (`config.yaml`):**
```yaml
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
connection_probability: 0.15

stdp:
    ltp_weight: 0.02
    ltd_weight: 0.015
    tau: 20.0

plasticity:
    learning_rate: 0.01
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
    dopamine:
        baseline: 0.1
        peak: 1.0
        decay: 0.95
    curiosity:
        baseline: 0.0
        threshold: 0.5
```

### CLI Commands

```bash
# Run with JSON config
./nlm run --config config.json

# Save current config to file
./nlm save custom_config.json

# Load configuration from file
./nlm config my_config.cfg

# Validate current configuration
./nlm validate

# List all configuration keys
./nlm list

# Get specific configuration value
./nlm get neuron_count

# Set configuration value
./nlm set neuron_count 2000

# Reset brain to initial state
./nlm reset

# Run performance benchmark
./nlm benchmark --output benchmark.csv

# Run experiment with specific parameters
./nlm experiment --name plasticity --steps 5000
```

## Enhanced Configuration System

### Configuration File Formats

NLM now supports three configuration file formats:

1. **Simple Key-Value Format** (existing format)
   - `key = value`
   - Supports comments with `#` or `//`
   - File extension: `.cfg`

2. **JSON Format** (new)
   - Standard JSON object notation
   - Supports nested objects
   - File extension: `.json`

3. **YAML Format** (new)
   - Human-readable key-value pairs
   - Supports multi-line values
   - File extension: `.yaml` or `.yml`

### Configuration Commands

#### Basic Configuration Operations

```bash
# Load configuration from file
./nlm config <file>

# Save current configuration to file
./nlm save <file>

# Validate current configuration
./nlm validate

# List all configuration keys with values
./nlm list

# Get specific configuration value
./nlm get <key>

# Set configuration value
./nlm set <key> <value>

# Remove configuration key
./nlm remove <key>
```

#### Advanced Configuration Operations

```bash
# Get configuration with fallback value
./nlm get --fallback neuron_count 1000

# List configuration by category
./nml list --category neural
./nml list --category plasticity
./nml list --category neuromodulation

# Export configuration to different formats
./nml export --format json config.json
./nml export --format yaml config.yaml
./nml export --format ini config.cfg

# Merge multiple configuration files
./nml merge config1.json config2.json merged.json
```

### Configuration Schema

NLM uses a schema-based validation system for configuration files:

#### Core Neural Configuration
```json
{
    "neural": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001
    }
}
```

#### Plasticity Configuration
```json
{
    "plasticity": {
        "learning_rate": 0.01,
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "tau": 20.0
        },
        "structural": {
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    }
}
```

#### Memory System Configuration
```json
{
    "memory": {
        "working_memory": {
            "capacity": 100,
            "decay_rate": 0.95
        },
        "episodic_memory": {
            "max_episodes": 1000,
            "replay_probability": 0.1
        },
        "concept_formation": {
            "similarity_threshold": 0.7,
            "learning_rate": 0.01
        }
    }
}
```

## Advanced CLI Interface

### Command Line Options

```bash
--help, -h              Show help message
--version               Show version information
--log-level <level>     Set logging level (DEBUG, INFO, WARNING, ERROR)
--config <file>         Load configuration from file
--output <file>         Write output to file instead of console
--timestep <value>      Set simulation timestep
--neurons <count>       Set number of neurons
--threads <count>       Set number of threads for parallel processing
--validate              Validate configuration before execution
--dry-run               Print configuration without running simulation
```

### Command Examples

#### Neural Network Configuration

```bash
# Configure neural network with specific parameters
./nlm run \
    --neurons 5000 \
    --timestep 0.0005 \
    --config neural_config.json \
    --log-level DEBUG
```

#### Memory System Configuration

```bash
# Enable and configure memory systems
./nml experiment \
    --name "memory_test" \
    --steps 10000 \
    --memory-capacity 500 \
    --replay-probability 0.2 \
    --concept-threshold 0.8
```

#### Neuromodulation Tuning

```bash
# Configure neuromodulation parameters
./nml run \
    --curiosity-threshold 0.3 \
    --dopamine-baseline 0.2 \
    --novelty-sensitivity 0.5 \
    --reward-scale 2.0
```

## Code Organization and Structure

### Directory Structure

```
NLM/
├── src/
│   ├── core/                    # Core utilities
│   │   ├── Config/              # Configuration system
│   │   ├── Random/              # Random number generation
│   │   ├── Logger/              # Logging system
│   │   └── SimulationClock/     # Time management
│   ├── brain/                   # Neural components
│   │   ├── Brain.hpp/cpp        # Main brain controller
│   │   ├── Neuron.hpp/cpp       # Individual neuron model
│   │   └── Synapse.hpp/cpp      # Synaptic connections
│   ├── plasticity/              # Plasticity rules
│   │   ├── STDP.hpp/cpp         # Spike-Timing Dependent Plasticity
│   │   ├── Hebbian.hpp/cpp      # Hebbian learning
│   │   └── Structural.hpp/cpp   # Structural plasticity
│   ├── neuromodulation/         # Neuromodulation systems
│   │   ├── Dopamine.hpp/cpp     # Dopamine system
│   │   ├── Curiosity.hpp/cpp    # Curiosity system
│   │   └── Novelty.hpp/cpp      # Novelty detection
│   ├── memory/                  # Memory systems
│   │   ├── WorkingMemory.hpp/cpp # Working memory
│   │   ├── EpisodicMemory.hpp/cpp # Episodic memory
│   │   └── ConceptFormation.hpp/cpp # Concept formation
│   ├── development/             # Developmental processes
│   │   ├── DevelopmentSystem.hpp/cpp # Main development controller
│   │   ├── Synaptogenesis.hpp/cpp # Synaptic development
│   │   └── Pruning.hpp/cpp      # Synaptic pruning
│   ├── sensory/                 # Sensory processing
│   │   ├── Vision.hpp/cpp       # Visual processing
│   │   └── Audio.hpp/cpp        # Auditory processing
│   ├── motor/                   # Motor system
│   │   └── MotorSystem.hpp/cpp   # Action selection
│   ├── environment/             # Environment interface
│   │   └── Environment.hpp/cpp   # World simulation
│   └── experiments/             # Experiment framework
│       ├── Phase6Demo.hpp/cpp    # Phase 6 integration demo
│       └── Metrics.hpp/cpp       # Performance metrics
├── python/                      # Python bindings
│   ├── bindings.cpp             # C++ to Python interface
│   └── setup.py                 # Python package setup
├── configs/                     # Configuration files
│   ├── default.cfg              # Default configuration
│   ├── neural_config.json       # Neural network config
│   └── memory_config.yaml       # Memory system config
├── docs/                        # Documentation
│   ├── README.md               # Main documentation
│   ├── API.md                  # API reference
│   └── TUTORIAL.md             # User tutorials
└── tests/                       # Test suite
    ├── unit/                    # Unit tests
    └── integration/             # Integration tests
```

### Code Quality Improvements

#### Modularity
- Each system (memory, neuromodulation, plasticity) is implemented as a separate module
- Clear interfaces between components using pure virtual base classes
- Dependency injection for easy testing and extension

#### Maintainability
- Consistent naming conventions across all files
- Comprehensive error handling with detailed error messages
- Modular configuration system with validation

#### Performance
- Optimized data structures for large-scale simulations
- Parallel processing capabilities for intensive computations
- Memory-efficient algorithms for sparse neural networks

## Performance Optimization

### Performance Profiling Tools

NLM includes several performance profiling tools:

#### 1. Benchmark Suite
```bash
# Run comprehensive performance benchmark
./nlm benchmark --output benchmark.json

# Benchmark specific subsystems
./nlm benchmark --subsystem memory --output memory_bench.json
./nlm benchmark --subsystem plasticity --output plasticity_bench.json
./nlm benchmark --subsystem neuromodulation --output neuromodulation_bench.json
```

#### 2. Memory Profiler
```bash
# Profile memory usage during simulation
./nlm run --profiler memory --output memory_profile.txt

# Track memory allocation patterns
./nlm run --memory-tracker all --output allocation_log.json
```

#### 3. CPU Profiler
```bash
# Profile CPU usage and identify bottlenecks
./nlm run --profiler cpu --output cpu_profile.txt

# Profile specific subsystems
./nlm run --profiler cpu --subsystem brain --output brain_profile.txt
```

### Performance Optimization Strategies

#### For Large-Scale Simulations
```bash
# Configure for large-scale simulations
./nml run \
    --neurons 10000 \
    --threads 8 \
    --simulation-steps 100000 \
    --memory-limit 8GB
```

#### For Real-Time Applications
```bash
# Configure for real-time performance
./nml run \
    --timestep 0.0001 \
    --threads 4 \
    --performance-mode real-time \
    --output low-latency
```

#### For Research and Development
```bash
# Configure for detailed analysis
./nml run \
    --log-level DEBUG \
    --profiler detailed \
    --output analysis_results.json \
    --save-intermediate-states
```

## Advanced Features for Expert Users

### Custom Neurons and Synapses

#### Creating Custom Neuron Types

```cpp
// Example: Create a custom neuron type
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {
        setType(NeuronType::Custom);
        setMembraneCapacitance(2.0f);
        setAdaptationTimeConstant(50.0f);
    }
    
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom LIF dynamics with adaptation
        bool fired = Neuron::stepLIF(currentTime, dt);
        
        // Add custom adaptation
        if (fired) {
            float adaptation = calculateCustomAdaptation(currentTime);
            addToMembranePotential(-adaptation);
        }
        
        return fired;
    }
    
private:
    float calculateCustomAdaptation(Timestamp time) {
        // Custom adaptation based on spike history
        float historyWeight = 0.1f;
        float spikeHistorySum = 0.0f;
        
        for (auto spikeTime : getSpikeHistory()) {
            spikeHistorySum += std::exp(-(time - spikeTime) / 100.0f);
        }
        
        return historyWeight * spikeHistorySum;
    }
};
```

#### Creating Custom Synapse Types

```cpp
// Example: Create a custom synapse with short-term plasticity
class CustomSynapse : public Synapse {
public:
    CustomSynapse(SynapseId id, NeuronId source, NeuronId destination)
        : Synapse(id, source, destination) {
        setType(SynapseType::Custom);
        setFacilitationTimeConstant(50.0f);
        setDepressionTimeConstant(200.0f);
        setUmax(0.8f);
    }
    
    void step(Timestamp currentTime) override {
        // Custom synaptic dynamics
        Synapse::step(currentTime);
        
        // Update custom short-term plasticity
        updateCustomSTP(currentTime);
        
        // Apply custom modulation
        applyCustomModulation(currentTime);
    }
    
private:
    void updateCustomSTP(Timestamp currentTime) {
        // Custom STP model combining depression and facilitation
        float u = getUtilization();
        float x = getFacilitation();
        float y = getDepression();
        
        // Update dynamics
        float dt = 0.001f; // Simulation timestep
        float x_new = x * exp(-dt / getFacilitationTimeConstant());
        float y_new = y + (1 - y) * (1 - exp(-dt / getDepressionTimeConstant()));
        
        setFacilitation(x_new);
        setDepression(y_new);
    }
    
    void applyCustomModulation(Timestamp currentTime) {
        // Apply neuromodulatory effects
        float neuromodulatorEffect = getNeuromodulatorEffect(currentTime);
        float efficacy = getEfficacy() * neuromodulatorEffect;
        setEfficacy(efficacy);
    }
};
```

### Advanced Plasticity Rules

#### Implementing Custom Plasticity Rules

```cpp
// Example: Create a custom plasticity rule combining STDP and Hebbian
class HybridPlasticityRule : public PlasticityRule {
public:
    HybridPlasticityRule() {
        setName("HybridPlasticity");
        setLearningRate(0.01f);
        enableSTDP(true);
        enableHebbian(true);
        enableRewardModulation(true);
    }
    
    void applyPlasticity(const SpikeEvent& preSpike, const SpikeEvent& postSpike) override {
        // Combine STDP and Hebbian mechanisms
        float stdpEffect = calculateSTDP(preSpike, postSpike);
        float hebbianEffect = calculateHebbian(preSpike, postSpike);
        float rewardEffect = calculateRewardModulation(preSpike, postSpike);
        
        float totalEffect = stdpEffect + hebbianEffect + rewardEffect;
        
        // Apply to all synapses from pre to post neuron
        auto& synapses = getBrain()->getSynapsesFrom(preSpike.neuronId);
        for (auto& synapse : synapses) {
            if (synapse->getDestinationNeuron() == postSpike.neuronId) {
                synapse->addToWeight(totalEffect);
            }
        }
    }
    
private:
    float calculateSTDP(const SpikeEvent& pre, const SpikeEvent& post) {
        // Spike-timing dependent plasticity
        float delta_t = post.spikeTime - pre.spikeTime;
        
        if (delta_t > 0) {
            // Post before pre: Long-Term Depression
            return -getSTDPLTDWeight() * std::exp(-delta_t / getSTDPTau());
        } else {
            // Pre before post: Long-Term Potentiation
            return getSTDPSTDPWeight() * std::exp(delta_t / getSTDPTau());
        }
    }
    
    float calculateHebbian(const SpikeEvent& pre, const SpikeEvent& post) {
        // Simple Hebbian learning
        float correlation = pre.spikeRate * post.spikeRate;
        return getLearningRate() * correlation * getHebbianStrength();
    }
    
    float calculateRewardModulation(const SpikeEvent& pre, const SpikeEvent& post) {
        // Reward-modulated plasticity
        float rewardPrediction = getBrain()->getRewardPrediction(post.neuronId);
        float predictionError = rewardPrediction - getBrain()->getActualReward();
        
        return getRewardModulationStrength() * predictionError;
    }
};
```

### Memory System Configuration

#### Configuring Working Memory

```bash
# Configure working memory for specific tasks
./nml run \
    --memory-type working \
    --memory-capacity 500 \
    --memory-decay-rate 0.95 \
    --memory-competition \
    --memory-update-rule stdp
```

#### Configuring Episodic Memory

```bash
# Configure episodic memory with replay
./nml run \
    --memory-type episodic \
    --max-episodes 10000 \
    --replay-probability 0.2 \
    --replay-time-scale 0.5 \
    --consolidation-threshold 0.8
```

#### Configuring Concept Formation

```bash
# Configure concept formation for categorization
./nml run \
    --memory-type concept \
    --similarity-threshold 0.7 \
    --concept-learning-rate 0.01 \
    --concept-forgetting-rate 0.01 \
    --concept-association-strength 0.5
```

### Neuromodulation Tuning

#### Dopamine System Configuration

```bash
# Configure dopamine system for exploration-exploitation balance
./nml run \
    --dopamine-baseline 0.1 \
    --dopamine-peak 1.0 \
    --dopamine-decay 0.95 \
    --dopamine-sensitivity 0.8 \
    --dopamine-learning-rate 0.001
```

#### Curiosity System Configuration

```bash
# Configure curiosity system for intrinsic motivation
./nml run \
    --curiosity-baseline 0.0 \
    --curiosity-threshold 0.5 \
    --curiosity-intensity 1.0 \
    --curiosity-decay 0.99 \
    --curiosity-learning-rate 0.01
```

#### Novelty Detection Configuration

```bash
# Configure novelty detection for surprise signals
./nml run \
    --novelty-sensitivity 0.5 \
    --novelty-threshold 0.3 \
    --novelty-adaptation 0.95 \
    --novelty-memory-size 1000 \
    --novelty-similarity-metric cosine
```

## Development Guide

### Setting Up Development Environment

#### Prerequisites

```bash
# Install system dependencies
apt-get update
apt-get install -y cmake g++ python3 python3-pip git

# Install Python dependencies
pip3 install pybind11 numpy scipy matplotlib

# Clone the repository
# (If starting from scratch)
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_BUILD_PYTHON_BINDINGS=ON \
         -DCMAKE_CXX_STANDARD=20

# Build all targets
make -j$(nproc)

# Install Python bindings
pip3 install -e .
```

### Running Tests

#### Unit Tests

```bash
# Run all unit tests
./test_runner --unit

# Run specific test suite
./test_runner --unit --suite neural
./test_runner --unit --suite plasticity
./test_runner --unit --suite memory

# Run tests with verbose output
./test_runner --unit --verbose
```

#### Integration Tests

```bash
# Run integration tests
./test_runner --integration

# Run Phase 6 integration tests
./test_runner --integration --phase 6

# Run performance tests
./test_runner --performance --benchmark
```

#### Python Tests

```bash
# Run Python bindings tests
pytest tests/python/

# Run Python integration tests
pytest tests/integration/

# Run all tests
pytest
```

### Adding New Features

#### Adding New Neuron Types

1. Create new header file: `src/brain/CustomNeuron.hpp`
2. Implement new class in: `src/brain/CustomNeuron.cpp`
3. Register new type in: `src/brain/NeuronFactory.cpp`
4. Update configuration schema: `configs/schema/neuron_types.json`
5. Add tests in: `tests/unit/neuron/"

#### Adding New Plasticity Rules

1. Create new header file: `src/plasticity/CustomPlasticity.hpp`
2. Implement new class in: `src/plasticity/CustomPlasticity.cpp`
3. Register new rule in: `src/plasticity/PlasticityFactory.cpp`
4. Update configuration schema: `configs/schema/plasticity_rules.json`
5. Add performance benchmarks in: `tests/performance/plasticity/`

### Documentation Guidelines

#### Code Comments
- Use Doxygen-style comments for public APIs
- Include parameter descriptions and return value explanations
- Document complex algorithms with step-by-step explanations

#### README Documentation
- Keep README concise and focused on quick start
- Link to detailed documentation for advanced features
- Include code examples for common use cases

#### API Documentation
- Generate API documentation using Doxygen
- Include usage examples for each class and function
- Document configuration parameters with data types and defaults

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration File Not Loading

**Symptom:** "Using default configuration" message when custom config expected.

**Solution:**
1. Check if file path is correct
2. Verify file format (JSON, YAML, or key=value)
3. Ensure file has proper permissions
4. Check for syntax errors in config file

**Example Fix:**
```bash
# Check if file exists and is readable
ls -la my_config.json

# Test JSON syntax
python3 -m json.tool my_config.json

# Test YAML syntax
python3 -c "import yaml; yaml.safe_load(open('my_config.yaml'))"
```

#### Issue 2: Simulation Running Slowly

**Symptom:** Simulation takes too long to complete.

**Solution:**
1. Reduce neuron count: `--neurons 1000`
2. Increase timestep: `--timestep 0.001`
3. Reduce simulation steps
4. Disable optional subsystems: `--memory-type none --neuromodulation none`

**Example Fix:**
```bash
# Run with optimized settings
./nml run \
    --neurons 500 \
    --timestep 0.001 \
    --memory-type none \
    --neuromodulation none \
    --simulation-steps 1000
```

#### Issue 3: Memory Usage Too High

**Symptom:** System runs out of memory.

**Solution:**
1. Reduce configuration parameters
2. Enable memory pooling: `--enable-memory-pooling`
3. Configure checkpointing: `--enable-checkpointing --checkpoint-interval 1000`
4. Monitor memory usage: `--profiler memory`

**Example Fix:**
```bash
# Configure memory-efficient settings
./nml run \
    --neurons 1000 \
    --enable-memory-pooling \
    --enable-checkpointing \
    --checkpoint-interval 500 \
    --memory-limit 2GB
```

#### Issue 4: Neural Network Not Learning

**Symptom:** Synaptic weights not changing.

**Solution:**
1. Check plasticity is enabled: `--plasticity-enabled true`
2. Verify STDP parameters: `--stdp-ltp-weight 0.02`
3. Check Hebbian learning: `--hebbian-enabled true`
4. Ensure reward signals are present

**Example Fix:**
```bash
# Configure for effective learning
./nml run \
    --plasticity-enabled true \
    --stdp-ltp-weight 0.02 \
    --stdp-ltd-weight 0.015 \
    --hebbian-enabled true \
    --reward-sensitivity 1.0
```

### Getting Help

#### Online Resources
- Documentation: https://nlm.readthedocs.io
- GitHub Repository: https://github.com/nlm-project/nlm
- Issue Tracker: https://github.com/nlm-project/nlm/issues
- Discussion Forum: https://discourse.nlm-project.org

#### Local Support
```bash
# Get help with specific commands
./nml --help
./nml run --help
./nml experiment --help

# Check version and build information
./nml --version

# View detailed logs
./nml run --log-level DEBUG 2>&1 | head -100
```

## Future Roadmap

### Short-Term Goals (Next 6 Months)

1. **Enhanced Python Integration**
   - Complete Python API documentation
   - Add Jupyter notebook examples
   - Implement automatic differentiation

2. **Performance Optimizations**
   - SIMD vectorization for neural computations
   - GPU acceleration for large-scale simulations
   - Memory-mapped file I/O for checkpointing

3. **Advanced Visualization**
   - Real-time neural network visualization
   - 3D brain model rendering
   - Interactive analysis tools

### Medium-Term Goals (12 Months)

1. **Neuroscientific Accuracy**
   - Hodgkin-Huxley neuron model
   - Detailed cortical architecture
   - Spiking network theory integration

2. **Cross-Platform Support**
   - WebAssembly for browser deployment
   - Mobile device optimization
   - Cloud-native deployment options

3. **Extended Documentation**
   - Complete API reference
   - Scientific methodology documentation
   - User case studies and benchmarks

### Long-Term Goals (24 Months)

1. **Behavioral Emergent Phenomena**
   - Complex behavior learning
   - Social interaction simulation
   - Cognitive architecture development

2. **Open Research Platform**
   - Community-driven model extensions
   - Standardized benchmark suites
   - Reproducible research protocols

3. **Educational Applications**
   - Interactive neuroscience education
   - Neural network visualization tools
   - Computational neuroscience curriculum

## Conclusion

The NLM project has made significant improvements in code quality, user experience, and system capabilities. Key enhancements include:

- **Enhanced Configuration System**: JSON/YAML support with validation
- **Advanced CLI Interface**: Multiple commands for different use cases
- **Improved Code Organization**: Modular architecture with clear interfaces
- **Performance Optimization**: Profiling tools and optimization strategies
- **Advanced Features**: Custom neurons, plasticity rules, and neuromodulation

These improvements make NLM more accessible to beginners while providing powerful tools for advanced researchers and developers. The project continues to evolve with a focus on scientific accuracy, performance, and usability.

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.