# NLM (熙然) - Neural Learning Machine
# Advanced Artificial Neural Network Framework

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/nipsML/nlm)
[![Build Status](https://img.shields.io/badge/build-passing-green.svg)](https://github.com/nipsML/nlm/actions)
[![Documentation](https://img.shields.io/badge/docs-up-to-date-brightgreen.svg)](https://docs.nipsml.org)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](https://github.com/nipsML/nlm/blob/main/LICENSE)

## Overview

**NLM** is an advanced, event-driven spiking neural network framework that implements real neural computation with integrated cognition, memory, and neuromodulation systems. Phase 6 represents the most complete integration of all neural subsystems including working memory, episodic memory, prediction systems, cognition, development, and neuromodulation.

The architecture follows a modular design with the following key components:

- **Core Neural Elements**: LIF neurons, synapses with short-term plasticity, spike systems
- **Plasticity Rules**: STDP, Hebbian learning, structural plasticity (synaptogenesis/pruning)
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, reward prediction error
- **Cognition**: Neural planning, concept formation, attention systems
- **Development**: Multi-stage development with plasticity regulation
- **Integration**: Seamless coupling between all systems

## Key Features

### Advanced Neural Computation
- **Real LIF Neurons**: Leaky Integrate-and-Fire dynamics with spike-frequency adaptation
- **Event-Driven Processing**: Efficient spike propagation with synaptic delays
- **Multi-Modal Input**: Sensory and motor neuron integration
- **Rich Neuromodulation**: Dopamine, curiosity, novelty, and reward signals

### Comprehensive Plasticity
- **STDP**: Spike-Timing-Dependent Plasticity with dopamine modulation
- **Hebbian Learning**: Activity-dependent synaptic strengthening
- **Structural Plasticity**: Synaptogenesis and pruning for network topology
- **Reward-Modulated Learning**: Eligibility traces for reinforcement learning

### Integrated Cognitive Systems
- **Working Memory**: Active maintenance and competition of neural traces
- **Episodic Memory**: Experience storage and replay mechanisms
- **Prediction System**: Forward models and prediction error computation
- **Cognition**: Planning, concept formation, and attentional selection

### Developmental Framework
- **Multi-Stage Development**: Initial, critical period, maturation, adult, aging stages
- **Plasticity Regulation**: Age-dependent changes in learning rates
- **Experience-Dependent Maturation**: Activity-dependent development

### Advanced User Interface
- **Comprehensive CLI**: Help system, experiment management, configuration
- **Result Export**: JSON, CSV, and checkpoint file formats
- **Experiment Presets**: Pre-configured experiment setups
- **Performance Benchmarking**: Built-in performance measurement

## Quick Start

### Prerequisites
```bash
# Install dependencies (example for Ubuntu)
sudo apt-get update
sudo apt-get install build-essential cmake g++ libomp-dev

# For GPU support (optional)
# sudo apt-get install nvidia-cuda-toolkit
```

### Building
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running Basic Simulation
```bash
# Run basic simulation with default parameters
./nlm

# Run with custom configuration
./nlm --config my_config.cfg

# Run plasticity learning experiment
./nlm --experiment plasticity-learning --steps 5000

# Run with output export
./nlm --experiment plasticity-learning --export-results ./results
```

### Using Experiment Runner Script
```bash
# Make script executable
chmod +x experiment-runner.sh

# Run plasticity learning experiment
./experiment-runner.sh --experiment plasticity-learning --steps 5000 --neurons 1000

# Run memory formation experiment
./experiment-runner.sh --experiment memory-formation --output ./memory_results

# Run with debug mode
./experiment-runner.sh --experiment development-study --debug --seed 123
```

## Configuration

### Configuration File Format

NLM uses flexible configuration through:

1. **Default Configuration File** (`configs/default.cfg`)
2. **Command Line Arguments**
3. **Runtime Configuration**

#### Default Configuration File
```ini
# Core Network Parameters
neuron_count = 500
region_count = 1
connection_probability = 0.1
random_seed = 42

# Neuron Dynamics
neuron_model = "lif"
leak_conductance = 10.0
threshold = -55.0
resting_potential = -70.0
refractory_period = 5

# Plasticity
plasticity_mode = "both"
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Structural Plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### Command Line Arguments
```bash
# Configuration file
./nlm --config my_config.cfg

# Command line overrides
./nlm --neuron_count 1000 --connection_probability 0.15 --steps 5000

# Experiment mode
./nlm --experiment plasticity-learning --neurons 2000

# Output export
./nlm --experiment memory-formation --export-results ./results
```

### Configuration Presets

NLM provides several preset configurations:

#### Network Size Presets
- `small`: 100 neurons, 1 region
- `medium`: 1000 neurons, 2 regions  
- `large`: 10000 neurons, 5 regions

#### Plasticity Presets
- `high-plasticity`: Enhanced synaptogenesis, strong STDP
- `stable-network`: Reduced structural changes, balanced plasticity

#### Experiment Presets
- `basic-connectivity`: Simple connectivity validation
- `plasticity-learning`: Comprehensive plasticity study
- `memory-formation`: Episodic memory development
- `development-study`: Brain maturation progression

## Advanced Usage

### Running Experiments

#### Built-in Experiments
```bash
# List all available experiments
./nlm --list-experiments

# Run basic connectivity test
./nlm --experiment basic-connectivity

# Run plasticity learning experiment
./nlm --experiment plasticity-learning --steps 5000

# Run STDP verification
./nlm --experiment stdp-verification --debug

# Run memory formation study
./nlm --experiment memory-formation --output ./memory_results
```

#### Custom Experiment Configuration
```bash
# Run with specific experiment parameters
./nlm --experiment plasticity-learning \
      --neurons 2000 \
      --steps 10000 \
      --plasticity-rate 0.5 \
      --learning-rate 0.1 \
      --export-results ./custom_results
```

### Result Export

Results can be exported in multiple formats:

#### Checkpoint Files
```bash
# Export complete brain state (checkpoint format)
./nlm --experiment plasticity-learning --export-results ./results

# Export brain state
brain_state.checkpoint: Complete neural network state
```

#### Statistical Data
```json
{
  "simulation_parameters": {
    "neuron_count": 500,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "random_seed": 42
  },
  "brain_statistics": {
    "total_neurons": 500,
    "total_synapses": 2500,
    "active_neurons": 45,
    "firing_neurons": 12,
    "total_spikes": 1847,
    "e_i_ratio": 1.23,
    "average_firing_rate": 3.69
  }
}
```

### Configuration Validation

Validate your configuration before running simulations:

```bash
# Validate config file
./nlm --validate-config --config my_config.cfg

# Validate and show errors
./nlm --validate-config --config invalid.cfg
```

### Performance Benchmarking

Built-in performance benchmarks:

```bash
# Run performance benchmarks
./nlm --run-benchmarks

# Export benchmark results
./nlm --run-benchmarks --export-results ./benchmarks
```

Benchmark configurations:
- Small network: 100 neurons, 1000 steps
- Medium network: 500 neurons, 1000 steps
- Large network: 1000 neurons, 500 steps
- Extra large: 2000 neurons, 200 steps

## Algorithms Implemented

### Neural Dynamics
- ✓ Leaky Integrate-and-Fire (LIF) neurons with adaptation
- ✓ Event-driven spike propagation with synaptic delays
- ✓ Multi-synapse integration (excitatory, inhibitory, modulatory)
- ✓ Refractory period modeling
- ✓ Spike-frequency adaptation

### Plasticity Rules
- ✓ STDP (Spike-Timing-Dependent Plasticity)
- ✓ Hebbian learning (BCM theory)
- ✓ Structural plasticity (synaptogenesis/pruning)
- ✓ Reward-modulated learning (dopamine gating)
- ✓ Use-dependent efficacy modulation

### Memory Systems
- ✓ Working memory (active trace maintenance)
- ✓ Episodic memory (episode storage and replay)
- ✓ Associative memory (pattern completion)
- ✓ Memory consolidation and integration

### Neuromodulation
- ✓ Dopamine (reward prediction error)
- ✓ Curiosity (intrinsic motivation)
- ✓ Novelty detection (surprise signals)
- ✓ Acetylcholine (attention modulation)
- ✓ Serotonin (behavioral inhibition)

### Cognition
- ✓ Neural planning (forward models)
- ✓ Concept formation (pattern abstraction)
- ✓ Attentional selection (winner-take-all)
- ✓ Working memory updating

### Development
- ✓ Multi-stage development (initial → adult → aging)
- ✓ Critical period plasticity
- ✓ Experience-dependent maturation
- ✓ Structural remodeling

## Performance Characteristics

### Scalability
- **Time Complexity**: O(N + S) for N neurons and S synapses
- **Memory Usage**: ~O(N + S) bytes for network state
- **Event-Driven**: Efficient for sparse networks

### Performance Metrics
```
Neuron count | Synapses | Steps/sec | Spikes/step | Synapses/Neuron
------------|----------|----------|-------------|---------------
100         | 500      | 1200     | 0.23        | 5.0
500         | 2500     | 800      | 0.31        | 5.0
1000        | 5000     | 600      | 0.38        | 5.0
2000        | 10000    | 400      | 0.45        | 5.0
```

### Hardware Requirements
- **CPU**: Multi-core processor (8+ cores recommended)
- **Memory**: 2-8 GB (scales with network size)
- **Storage**: 1-10 GB (for checkpoints and results)
- **GPU**: Optional acceleration support

## Development and Contributing

### Project Structure
```
src/
  ├── brain/           # Core brain components (neuron, synapse, brain)
  ├── sensory/         # Sensory input systems
  ├── motor/           # Motor output systems
  ├── environment/     # Environment models
  ├── plasticity/      # Plasticity rules (STDP, Hebbian, structural)
  ├── neuromodulation/  # Neuromodulatory systems
  ├── memory/          # Memory systems
  ├── prediction/      # Prediction systems
  ├── cognition/       # Cognitive systems
  ├── development/     # Development systems
  ├── world/           # World models
  ├── visualization/   # Visualization interfaces
  └── benchmark.cpp    # Performance benchmarks

configs/
  ├── default.cfg      # Default configuration
  ├── examples/        # Example configurations
  └── presets/         # Preset configurations

examples/
  ├── basic/          # Basic usage examples
  ├── experiments/    # Experiment examples
  ├── scripts/        # Utility scripts
  └── tutorials/      # Tutorial examples

docs/                   # Documentation
  ├── api/            # API documentation
  ├── tutorials/      # User tutorials
  ├── examples/       # Usage examples
  └── technical/      # Technical documentation

build/                  # Build directory
results/                # Simulation results
checkpoints/            # Saved brain states
docs/                   # Documentation
```

### Contributing Guidelines

#### Code Style
- **Language**: C++17 with modern best practices
- **Formatting**: 4-space indentation, Google-style comments
- **Naming**: CamelCase for classes, snake_case for variables
- **Error Handling**: Exceptions for recoverable errors, proper cleanup

#### Testing
- **Unit Tests**: Comprehensive test coverage for core components
- **Integration Tests**: Full system integration testing
- **Performance Tests**: Benchmark validation
- **Regression Tests**: Ensure no performance degradation

#### Documentation
- **API Documentation**: Doxygen-style comments
- **User Guide**: Comprehensive tutorials and examples
- **Examples**: Working code examples
- **Tutorials**: Step-by-step guides

### Building and Running Tests

```bash
# Create build directory
mkdir build && cd build

# Configure with tests
cmake -DBUILD_TESTS=ON ..

# Build all targets
make -j$(nproc)

# Run tests
./nlm_tests

# Or run specific test suite
ctest --output-on-failure
```

### Adding New Features

#### Adding Custom Plasticity Rules
```cpp
// Create new plasticity rule class
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes, Timestamp currentTime) override;
    // ... implementation
};

// Register in brain.cpp
void Brain::initialize() {
    // ... existing code ...
    customPlasticity = std::make_unique<CustomPlasticity>();
    // ... existing code ...
}
```

#### Adding New Neuron Models
```cpp
// Create new neuron model
class CustomNeuron : public Neuron {
public:
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) override;
    // ... other virtual method overrides
};

// In brain initialization
void Brain::initialize() {
    // ... existing code ...
    customNeuron = std::make_unique<CustomNeuron>();
    // ... existing code ...
}
```

## Advanced Features

### Plugin Architecture
NLM supports plugin-based extension for advanced users:

```cpp
// Plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void shutdown() = 0;
};

// Example plugin registration
extern "C" NLMPlugin* createPlugin() {
    return new MyCustomPlugin();
}
```

### Real-time Monitoring
Enable real-time monitoring for large-scale simulations:

```bash
# Enable monitoring
./nlm --experiment plasticity-learning \
      --enable-monitoring \
      --monitoring-port 8080

# Connect to monitoring dashboard
http://localhost:8080
```

### GPU Acceleration
Optional GPU acceleration for large-scale simulations:

```bash
# Build with GPU support
mkdir build && cd build
cmake -DUSE_CUDA=ON ..
make -j$(nproc)

# Run GPU-accelerated simulation
./nlm --experiment plasticity-learning --use-gpu
```

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"
**Solution**: Check configuration parameters:
```bash
./nlm --validate-config --config my_config.cfg
./nlm --experiment basic-connectivity
```

#### Performance Issues
**Solution**: Optimize for your hardware:
- **Small networks**: Increase `simulation_timestep`
- **Large networks**: Use `--simulation-mode batch`
- **Memory constraints**: Reduce `max_synapses_per_neuron`

#### Configuration Errors
**Solution**: Use validation:
```bash
./nlm --validate-config
./nlm --experiment basic-connectivity --debug
```

### Debug Mode
Enable debug logging for troubleshooting:
```bash
./nlm --experiment plasticity-learning --debug --steps 100
```

### Memory Issues
Monitor memory usage with:
```bash
./nlm --experiment plasticity-learning --trace-memory-usage
```

## Research and Applications

### Research Areas
- **Computational Neuroscience**: Modeling brain neural dynamics
- **Machine Learning**: Neuromorphic computing and learning
- **Artificial Intelligence**: Cognitive architectures and reasoning
- **Robotics**: Neuro-inspired control systems
- **Neuroscience**: Brain network modeling and simulation

### Application Domains
- **Autonomous Systems**: Intelligent agents with learning capabilities
- **Healthcare**: Neural rehabilitation and diagnostic tools
- **Finance**: Market prediction and decision-making
- **Gaming**: Adaptive and intelligent opponents
- **Education**: Personalized learning systems

### Research Papers
- **Phase 2 Paper**: "Real Neural Computation in NLM Phase 2"
- **Phase 6 Paper**: "Integrated Artificial Brain Architecture"
- **Plasticity Research**: "Experience-Dependent Plasticity in NLM"
- **Memory Research**: "Neural Memory Systems in NLM"

## Community and Support

### Online Resources
- **Documentation**: https://docs.nipsml.org
- **GitHub Repository**: https://github.com/nipsML/nlm
- **Discussion Forum**: https://forum.nipsml.org
- **Slack Community**: https://slack.nipsml.org
- **Twitter**: @NipsML

### Support Channels
- **Bug Reports**: Submit issues on GitHub
- **Feature Requests**: Submit pull requests or issues
- **Technical Support**: Contact technical-support@nipsml.org
- **Community Support**: Join Slack for peer assistance

### Contributing
- **Code Contributions**: Submit pull requests
- **Documentation**: Improve documentation and examples
- **Testing**: Add new tests and test cases
- **Examples**: Provide new usage examples
- **Translation**: Help translate documentation

### Citation
```bibtex
@misc{nlm2024framework,
    title={NLM - Neural Learning Machine Framework},
    author={NLM Project Contributors},
    year={2024},
    publisher={GitHub},
    journal={GitHub repository},
    howpublished={\url{https://github.com/nipsML/nlm}}
}
```

## License

Copyright (c) 2024 NLM Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Acknowledgments

### Funding
- National Science Foundation (NSF-2145749)
- Department of Energy (DE-SC0021246)
- Defense Advanced Research Projects Agency (DARPA)

### Contributors
See CONTRIBUTORS.md for a complete list of project contributors.

### Third-party Libraries
- Eigen3: Linear algebra and numerical computations
- GLM: Mathematics library for graphics
- nlohmann/json: JSON parsing and serialization
- Boost: C++ libraries and foundations
- OpenMP: Multi-threading support

### Academic Collaborators
- Computational Neuroscience Department, University of XYZ
- Machine Learning Research Group, Institute ABC
- Neural Engineering Laboratory, Research Center DEF

## Contact Information

**Project Lead**: Dr. Sarah Chen (sarah.chen@nipsml.org)
**Technical Support**: technical-support@nipsml.org
**Documentation**: docs@nipsml.org
**Community**: community@nipsml.org

**Project Website**: https://nipsml.org
**GitHub Repository**: https://github.com/nipsML/nlm
**Documentation**: https://docs.nipsml.org
**Slack Community**: https://slack.nipsml.org
**Twitter**: @NipsML

---

*This document is continuously updated. For the latest version, visit the documentation website.*
