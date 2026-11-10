# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Quick Start

### 1. Building and Running

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 2 demo (basic neural computation)
./nlm

# Run Phase 3 demo (sensorimotor integration)
./nlm_phase3_demo

# Run Phase 6 demo (full integration test)
./nlm_phase6_demo

# Run benchmark
./nlm_benchmark
```

### 2. Basic Configuration

Create a configuration file `config.cfg`:

```
# NLM Configuration File
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_curiosity = true
enable_novelty = true
enable_reward_modulation = true
developmental_stage = adult
checkpoint_dir = ./checkpoints
log_level = info
```

### 3. Running with Configuration

```bash
# Run with configuration file
./nlm --config config.cfg

# Run with custom options
./nlm --neuron-count 2000 --region-count 2 --enable-development

# Run with output and profiling
./nlm --config config.cfg --output results.json --enable-profiling

# Validate configuration
./nlm --config config.cfg --validate-only

# List available configuration options
./nlm --list-config

# Get help
./nlm --help
```

## Advanced Usage

### 1. Custom Python Integration

```python
import pynlm

# Create and initialize brain
brain = pynlm.Brain(neuron_count=500)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate()} Hz")
print(f"E/I ratio: {brain.get_excitation_inhibition_ratio()}")
```

### 2. Batch Processing

Create a batch script `run_batch.py`:

```python
import subprocess
import time

def run_batch(input_file, output_file):
    cmd = [
        "./nlm",
        "--input", input_file,
        "--output", output_file,
        "--enable-profiling",
        "--log-level", "info"
    ]
    
    start_time = time.time()
    result = subprocess.run(cmd, capture_output=True, text=True)
    end_time = time.time()
    
    print(f"Batch completed in {end_time - start_time:.2f} seconds")
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
    else:
        print(f"Output: {result.stdout}")

# Example usage
run_batch("inputs/demo.cfg", "outputs/demo_results.json")
```

### 3. Interactive Mode

```bash
# Start interactive session
./nlm --interactive

# Configure interactively
> set neuron-count 2000
> set developmental-stage maturation
> set enable-curiosity true
> run
> save results.json
```

### 4. Development Mode

```bash
# Enable development features
./nlm --dev --debug --verbose

# Development configuration
> load dev-config.cfg
> enable memory-tracking
> enable step-by-step
> set developmental-stage critical
```

### 5. Performance Benchmarking

```bash
# Performance benchmark with profiling
./nlm --neuron-count 5000 --enable-profiling --output benchmark.json

# Compare different configurations
cat > compare_configs.sh << 'EOF'
#!/bin/bash
echo "=== Configuration Comparison ==="

echo "Configuration 1:"
./nlm --neuron-count 1000 --region-count 1 --config cfg1.cfg --output out1.json

echo "Configuration 2:"
./nlm --neuron-count 2000 --region-count 2 --config cfg2.cfg --output out2.json

echo "Configuration 3:"
./nlm --neuron-count 5000 --region-count 3 --config cfg3.cfg --output out3.json
EOF

chmod +x compare_configs.sh
./compare_configs.sh
```

## Configuration Options

### Core Neural Parameters

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--neuron-count` | Number of neurons | 1000 | int |
| `--region-count` | Number of neural regions | 1 | int |
| `--connection-prob` | Connection probability (0.0-1.0) | 0.1 | float |
| `--stdp-weights` | STDP learning rates (LTP,LTD) | 0.01,0.012 | string |

### Plasticity Settings

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--synaptogenesis-rate` | Synapse creation rate | 0.0001 | float |
| `--pruning-rate` | Synapse removal rate | 0.00001 | float |
| `--plasticity` | Enable synaptic plasticity | true | bool |

### Memory Systems

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--working-memory` | Enable working memory | true | bool |
| `--episodic-memory` | Enable episodic memory | true | bool |
| `--memory-capacity` | Memory capacity | 100 | int |
| `--memory-max-episodes` | Max episodic memory episodes | 1000 | int |

### Neuromodulation

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--curiosity` | Enable curiosity-driven learning | true | bool |
| `--novelty` | Enable novelty detection | true | bool |
| `--reward-modulation` | Enable reward modulation | true | bool |

### Development

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--enable-development` | Enable developmental stages | true | bool |
| `--stage` | Development stage (initial, critical, maturation, adult) | adult | string |

### Checkpointing

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--checkpoint-dir` | Directory for checkpoints | ./checkpoints | string |
| `--checkpoint-interval` | Checkpoint save interval | 100 | int |
| `--load-checkpoint` | Load from checkpoint file | - | string |
| `--save-checkpoint` | Save to checkpoint file | - | string |

### Advanced Features

| Option | Description | Default | Type |
|--------|-------------|---------|------|
| `--enable-profiling` | Enable performance profiling | false | bool |
| `--log-level` | Set logging level (off, error, warning, info, debug) | info | string |
| `--verbose` | Enable verbose logging | false | bool |
| `--debug` | Enable debug mode | false | bool |
| `--enable-memory-tracking` | Enable memory tracking | false | bool |
| `--step-by-step` | Step-by-step execution | false | bool |

## Configuration File Format

### Basic Configuration File (`config.cfg`):

```
# NLM Configuration File
# Generated at: 2026-10-31

# Neural network configuration
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.000001

# Memory system configuration
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Neuromodulation
enable_curiosity = true
enable_novelty = true
enable_reward_modulation = true

# Development
developmental_stage = adult
enable_development = true

# Checkpointing
checkpoint_dir = ./checkpoints
checkpoint_interval = 100
replay_interval = 100
consolidation_interval = 1000

# Logging
log_level = info
enable_profiling = false
```

### Configuration File Comments:

```
# Use # for comments
# Sections in brackets: [SECTION]
# Key = Value format
# Strings in quotes: "string"
# Lists in brackets: [value1, value2, value3]
```

## Debug and Profiling

### 1. Performance Profiling

```bash
# Run with profiling
./nlm --enable-profiling --output profile.json

# View performance report
./nlm --enable-profiling --output profile.txt
```

### 2. Memory Analysis

```bash
# Run with memory tracking
./nlm --enable-memory-tracking --output memory_report.json
```

### 3. Debug Logging

```bash
# Enable debug logging
./nlm --debug --log-level debug

# Debug output example
DEBUG: Step 0: 0.012s
DEBUG: Neuron 0: membrane potential = -69.2 mV
DEBUG: Synapse 0: weight = 0.8
DEBUG: Spike detected at neuron 42
```

### 4. Event Tracking

```bash
# Event logging for debugging
./nlm --log-level debug --enable-profiling
```

## Examples

### Example 1: Basic Neural Network

```bash
./nlm --neuron-count 500 --region-count 1 --connection-prob 0.15
```

### Example 2: Development-focused Network

```bash
./nlm --neuron-count 2000 --region-count 2 \
      --enable-development --stage maturation \
      --enable-curiosity --enable-novelty
```

### Example 3: Memory-intensive Network

```bash
./nlm --neuron-count 1000 --region-count 1 \
      --working-memory --episodic-memory \
      --memory-capacity 500 --memory-max-episodes 5000
```

### Example 4: High-performance Simulation

```bash
./nlm --neuron-count 5000 --region-count 4 \
      --connection-prob 0.05 --enable-profiling \
      --checkpoint-dir ./checkpoints --checkpoint-interval 500 \
      --output performance_results.json
```

### Example 5: Comparative Analysis

```bash
#!/bin/bash

echo "=== Neural Network Performance Comparison ==="

echo "Configuration A: Small network"
./nlm --neuron-count 500 --region-count 1 --enable-profiling --output results_a.json

echo "Configuration B: Medium network"
./nlm --neuron-count 1000 --region-count 2 --enable-profiling --output results_b.json

echo "Configuration C: Large network"
./nlm --neuron-count 2000 --region-count 4 --enable-profiling --output results_c.json

echo "\nAnalysis:"
echo "- Configuration A: Fast, simple network"
echo "- Configuration B: Balanced performance"
echo "- Configuration C: High performance, complex dynamics"
```

## Troubleshooting

### Common Issues

1. **Build Errors**
   ```bash
   # Ensure CMake version >= 3.10
   cmake --version
   
   # Check for C++17 support
   g++ --version
   
   # Clean build
   rm -rf build
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Runtime Errors**
   ```bash
   # Check configuration file syntax
   ./nlm --config config.cfg --validate-only
   
   # Enable debug logging
   ./nlm --debug --log-level debug
   ```

3. **Performance Issues**
   ```bash
   # Reduce network size for testing
   ./nlm --neuron-count 100 --region-count 1
   
   
   # Disable debug features
   ./nlm --neuron-count 1000 --log-level info
   ```

### Getting Help

```bash
# Command line help
./nlm --help

# Configuration options
./nlm --list-config

# Configuration validation
./nlm --config config.cfg --validate-only

# Documentation
https://nlm.readthedocs.io

# GitHub repository
https://github.com/nlm-project/nlm
```

## Advanced Features

### 1. Custom Plugins

NLM supports custom plugins for extended functionality:

```cpp
// Plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual void initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
};
```

### 2. Extension Modules

NLM can be extended with custom modules:

```cpp
// Module interface
class NLMModule {
public:
    virtual ~NLMModule() = default;
    virtual std::string getName() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual bool update(double timestep) = 0;
};
```

### 3. Custom Learning Algorithms

Implement custom learning algorithms:

```cpp
class CustomLearningAlgorithm {
public:
    virtual ~CustomLearningAlgorithm() = default;
    virtual void learn(const Brain* brain, double timestep) = 0;
    virtual std::string getName() const = 0;
};
```

## Installation

### From Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install (optional)
sudo make install
```

### From Package Manager

```bash
# Ubuntu/Debian
# Package not yet available

# Clone and install locally

# For development versions
pip install -e .
```

### Development Installation

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install dependencies (if needed)
# Create virtual environment (for Python bindings)
python3 -m venv venv
source venv/bin/activate
pip install -e .

# Build C++ components
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## Contributing

### Code Standards

- **C++17** or higher
- **CMake 3.10** or higher
- **Google Style Guide** for code formatting
- **Comprehensive documentation**
- **Unit tests**

### Contributing Guidelines

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests
5. Update documentation
6. Submit a pull request

### Code Quality

```cpp
// Example of good NLM code style
namespace nlm {

class ExampleComponent {
public:
    ExampleComponent(const Config& config) : config_(config) {
        // Initialize component
    }
    
    ~ExampleComponent() = default;
    
    bool initialize() {
        // Component initialization
        return true;
    }
    
    void update(double timestep) {
        // Component update logic
    }
    
private:
    Config config_;
    // Component state
};

} // namespace nlm
```

## Testing

### Running Tests

```bash
# Run unit tests (if available)
./test_runner

# Run integration tests
cd tests
./run_tests.sh

# Run performance tests
./performance_test --neuron-count 1000
```

### Test Coverage

The NLM project includes:
- **Unit tests** for individual components
- **Integration tests** for system integration
- **Performance tests** for benchmarking
- **Regression tests** for quality assurance

### Test Examples

```cpp
// Example test case
TEST(NeuronTest, LIFDynamics) {
    Neuron neuron(NeuronId(1));
    neuron.initializeRandom(rng);
    
    // Test LIF dynamics
    EXPECT_FALSE(neuron.stepLIF(0.0, 0.001));
    
    // Inject current
    neuron.injectCurrent(10.0f);
    EXPECT_TRUE(neuron.stepLIF(0.0, 0.001));
}
```

## Future Development

### Planned Features

1. **Advanced Learning Algorithms**
   - Reinforcement learning
   - Deep neural networks
   - Evolutionary algorithms

2. **Enhanced Memory Systems**
   - Hierarchical memory
   - Memory compression
   - Forgetting mechanisms

3. **Advanced Neuromodulation**
   - Multiple neuromodulators
   - Cross-modulation effects
   - Context-dependent modulation

4. **Development Extensions**
   - Epigenetic factors
   - Environmental influences
   - Social learning

5. **Performance Optimizations**
   - GPU acceleration
   - Parallel processing
   - Streaming computation

### Research Directions

- **Biological plausibility**
- **Energy efficiency**
- **Scalability**
- **Real-time applications**
- **Human-brain interfaces**

## Contact and Support

### Documentation
- **Main documentation**: https://nlm.readthedocs.io
- **API reference**: https://nlm.readthedocs.io/en/latest/api/
- **Tutorials**: https://nlm.readthedocs.io/en/latest/tutorials/

### Community
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions
- **Email**: support@nlm-project.org

### Support
- **Bug reports**: Please provide detailed information
- **Feature requests**: Submit through GitHub issues
- **Technical support**: Available during business hours
- **Emergency support**: Available for critical issues

## Acknowledgments

### Project Contributors
- **Lead Developers**: [Names]
- **Core Contributors**: [Names]
- **Documentation Writers**: [Names]
- **Test Engineers**: [Names]
- **Performance Engineers**: [Names]

### Funding
- **Research Grants**: [Names]
- **Development Funding**: [Names]
- **Infrastructure Support**: [Names]

### Open Source Contributions
- **Third-party libraries**: See LICENSE file
- **Dependencies**: Listed in requirements.txt
- **Contributors**: See CONTRIBUTORS file

## License

MIT License

Copyright (c) 2026 NLM Project

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

## Version History

### Version 0.1.0 (Development)
- Initial release
- Core neural computation
- Basic plasticity
- Simple memory systems

### Version 0.2.0 (Planned)
- Advanced cognitive systems
- Enhanced development
- Improved integration
- Python bindings

### Version 1.0.0 (Future)
- Full-featured artificial brain
- Production-ready
- Extensive testing
- Documentation complete

## Conclusion

NLM represents a significant step forward in computational neuroscience and artificial intelligence. By combining real neural dynamics, integrated memory systems, and advanced cognitive capabilities, NLM provides a comprehensive platform for studying and implementing brain-like systems.

The project continues to evolve with new features, improved performance, and expanded functionality. We invite you to contribute to this exciting research project and help shape the future of artificial neural systems.

---

*Last updated: 2026-11-10*
*Version: 0.1.0 (Development)*
*Documentation: https://nlm.readthedocs.io*