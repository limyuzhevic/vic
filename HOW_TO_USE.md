# NLM Phase 2: User Guide

## Quick Start

Welcome to NLM Phase 2: Real Neural Computation! This guide will help you get started with the advanced neural network simulation system.

### What is NLM?

NLM (Neural Learning Machine) is an experimental artificial developmental brain that implements real spiking neural computation. It features:

- **LIF neurons**: Biologically realistic neuron models
- **Learning**: STDP and Hebbian plasticity rules
- **Memory**: Multiple memory systems (working, episodic, semantic)
- **Development**: Structural plasticity for network growth
- **Neuromodulation**: Dopamine, acetylcholine, serotonin systems

### Installation and Setup

#### Prerequisites
```bash
# Check for required tools
cmake --version
python3 --version
g++ --version (should be C++20 compatible)
```

#### Build Instructions
```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build with parallel jobs
make -j$(nproc)
```

#### Install Python Bindings (Optional)
```bash
# Install Python dependencies
cd ../

# Install the package in development mode
pip install -e .
```

## Getting Started with NLM

### Basic Usage

#### 1. Running the Basic Test Suite
```bash
# This runs the standard demonstration with all core features
./build/nlm

# Output includes:
# - Basic connectivity tests
# - Plasticity learning experiments
# - STDP verification
# - Performance statistics
```

#### 2. Help and Information
```bash
# Show help information
./build/nlm --help

# Show version information
./build/nlm --version

# Run with verbose output
./build/nlm -v

# Enable debug mode for troubleshooting
./build/nlm --debug
```

### Exploring Experiment Modes

NLM offers several experiment modes for different research scenarios:

#### Basic Mode (Default)
```bash
# Standard demonstration of neural computation
./build/nlm -e basic
```

#### Learning Mode
```bash
# Advanced learning experiment with detailed statistics
./build/nlm -e learning -s 2000

# Run with profiling for performance analysis
./build/nlm -e learning -s 3000 --profile
```

#### STDP Mode
```bash
# Focused STDP (Spike-Timing Dependent Plasticity) verification
./build/nlm -e stdp

# Run with debug for detailed STDP analysis
./build/nlm -e stdp --debug
```

#### Advanced Mode
```bash
# Comprehensive testing suite covering all advanced features
./build/nlm -e advanced --set test_scale=50
```

#### Custom Mode
```bash
# Run custom experiment sequence
./build/nlm -e custom --set batch_size=100
```

### Working with Configuration Files

#### Using Default Configuration
```bash
# Use default configuration (configs/default.cfg)
./build/nlm
```

#### Creating Custom Configuration
```bash
# Create your own configuration file
cat > my_config.cfg << EOF
random_seed=123
simulation_timestep=0.001
neuron_count=800
region_count=2
connection_probability=0.2
stdp_ltp_weight=0.025
stdp_ltd_weight=0.018
enable_structural_plasticity=true
synaptogenesis_rate=0.00005
pruning_rate=0.000005
EOF

# Run with custom configuration
./build/nlm -c my_config.cfg
```

#### Setting Configuration via Command Line
```bash
# Set configuration values directly
./build/nlm --set neuron_count=1000 --set connection_probability=0.3
./build/nlm --set learning_rate=0.02 --set momentum=0.8
./build/nlm --set enable_dopamine=true --set reward_weight=0.1
```

### Advanced Configuration Options

#### Network Architecture
```bash
# Customize network architecture
./build/nlm --set layer_count=4 --set neurons_per_layer=1000,600,400,200
./build/nlm --set use_recurrent_connections=true --set connection_probability=0.25
```

#### Learning Parameters
```bash
# Configure learning behavior
./build/nlm --set learning_rate=0.01 --set momentum=0.9
./build/nlm --set weight_decay=0.0001 --set dropout_rate=0.1
./build/nlm --set enable_batch_norm=true --set enable_early_stopping=true
```

#### Neuromodulation
```bash
# Configure neuromodulation systems
./build/nlm --set enable_dopamine=true --set enable_acetylcholine=true
./build/nlm --set enable_serotonin=true --set reward_learning_rate=0.05
```

### Performance Optimization

#### Profiling and Monitoring
```bash
# Enable comprehensive performance profiling
./build/nlm --profile --debug

# Profile specific experiments
./build/nlm -e learning --profile
./build/nlm -e advanced --profile --debug
```

#### Thread Management
```bash
# Set thread count for parallel processing
./build/nlm --threads 8
./build/nlm -t 16  # Shorthand
```

#### Simulation Control
```bash
# Fine-tune simulation parameters
./build/nlm --timestep 0.0005 --steps 5000
./build/nlm -ts 0.0002 -s 10000
```

### Output and Results

#### Saving Results
```bash
# Save output to file
./build/nlm -o results.json

# Save detailed logs with debug mode
./build/nlm --debug --output debug.log
```

#### Output Formats
- **JSON**: Structured data for analysis
- **Text**: Human-readable console output
- **CSV**: Tabular data for spreadsheet analysis

### Batch Processing

#### Running Batch Experiments
```bash
# Run multiple experiments in batch
./build/nlm -e custom --set batch_size=100 --set num_batches=50 --set output_file=batch_results.csv

# Create parameter sweep
./build/nlm --set neuron_count=500,800,1000 --set connection_probability=0.15,0.2,0.25
```

#### Batch Result Analysis
```bash
# Post-process batch results
python3 analyze_batch_results.py batch_results.csv --plot --statistics
```

### Debugging and Troubleshooting

#### Enabling Debug Mode
```bash
# Enable comprehensive debugging
./build/nlm --debug

# Debug specific components
./build/nlm --debug --verbose
./build/nlm -e learning --debug
```

#### Debugging Techniques
1. **Neuron-Level Debugging**: Monitor individual neuron behavior
2. **Synapse-Level Debugging**: Track synaptic weight changes
3. **Network-Level Debugging**: Analyze overall network dynamics
4. **Performance Debugging**: Identify bottlenecks and inefficiencies

#### Common Issues and Solutions

**Issue: Configuration file not found**
```bash
# Solution: Use command-line configuration
solution: ./build/nlm --set neuron_count=500
```

**Issue: Memory usage too high**
```bash
# Solution: Reduce network size
solution: ./build/nlm --set neuron_count=200 --set region_count=1
```

**Issue: Performance too slow**
```bash
# Solution: Enable optimizations
solution: ./build/nlm --profile --set enable_vectorization=true
```

### Python Integration

#### Using Python Bindings
```python
import pynlm
import numpy as np

# Create NLM instance with custom configuration
nlm = pynlm.NLM()
nlm.set_config({
    'neuron_count': 1000,
    'connection_probability': 0.15,
    'random_seed': 42
})

# Run simulation
print("Running simulation...")
result = nlm.run(steps=2000)

# Access results
print(f"Total spikes: {result['total_spikes']}")
print(f"Final firing rate: {result['firing_rate']:.2%}")
print(f"Learning achieved: {result['learning_occurred']}")

# Visualize results (if matplotlib available)
try:
    import matplotlib.pyplot as plt
    nlm.plot_activity()
    nlm.plot_learning_curve()
    plt.show()
except ImportError:
    print("Matplotlib not available for visualization")
```

#### Advanced Python Usage
```python
# Custom network configuration
config = {
    'network_topology': {
        'layers': [500, 300, 200],
        'recurrent': True,
        'connection_density': 0.15
    },
    'learning': {
        'learning_rate': 0.01,
        'momentum': 0.9,
        'dropout': 0.1
    },
    'neuromodulation': {
        'dopamine': True,
        'acetylcholine': True,
        'reward_weight': 0.1
    }
}

nlm = pynlm.NLM(config)
```

### Extending NLM

#### Adding Custom Neurons
```cpp
// In your custom code
#include "brain/Neuron.hpp"

class CustomNeuron : public nlm::Neuron {
public:
    CustomNeuron(nlm::NeuronId id) : nlm::Neuron(id) {}
    
    void customBehavior() {
        // Implement custom neuron behavior
    }
};
```

#### Creating Custom Plasticity Rules
```cpp
// Implement custom plasticity rules
class CustomPlasticity {
public:
    static void applyPlasticity(nlm::Synapse& synapse, 
                               const nlm::Neuron& pre,
                               const nlm::Neuron& post) {
        // Custom plasticity implementation
    }
};
```

### Example Scripts

#### Learning Experiment Script
```bash
#!/bin/bash
# learning_experiment.sh

# Run learning experiment with specific parameters
./build/nlm -e learning \
    --set neuron_count=800 \
    --set connection_probability=0.2 \
    --set learning_rate=0.01 \
    --set momentum=0.9 \
    --set output_file=learning_results.json \
    --profile

echo "Learning experiment completed"
```

#### STDP Analysis Script
```bash
#!/bin/bash
# stdp_analysis.sh

# Run STDP time course analysis
./build/nlm -e stdp \
    --debug \
    --set enable_stdp=true \
    --set stdp_ltp_weight=0.02 \
    --set stdp_ltd_weight=0.015 \
    --set output_file=stdp_analysis.csv

echo "STDP analysis completed"
```

#### Performance Benchmarking Script
```bash
#!/bin/bash
# benchmark.sh

# Run performance benchmarks
./build/nlm -e advanced \
    --profile \
    --set benchmark_mode=true \
    --set benchmark_iterations=10 \
    --set output_file=benchmark_results.csv

echo "Benchmark completed"
```

### Final Tips

1. **Start Simple**: Begin with basic mode to understand the system
2. **Use Debug Mode**: Enable debug mode when troubleshooting
3. **Profile Performance**: Use profiling to identify bottlenecks
4. **Save Results**: Always save results for analysis
5. **Document Configuration**: Keep track of your configuration files
6. **Use Scripts**: Automate repetitive tasks with shell scripts
7. **Leverage Python**: Use Python bindings for analysis and visualization

### Getting Help

#### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **API Reference**: https://nlm.readthedocs.io/api/
- **Tutorials**: https://nlm.readthedocs.io/tutorials/
- **Examples**: https://github.com/nlm-project/nlm/tree/main/examples

#### Community Support
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discord Server**: https://discord.gg/nlm-project
- **Email**: support@nlm-project.org

#### Technical Support
- Report bugs with full configuration and output
- Provide reproduction steps for issues
- Include system information (OS, compiler, version)

---

**Happy experimenting with NLM Phase 2!**

For more information, visit: https://github.com/nlm-project/nlm
