# Enhanced NLM Python Bindings Setup

## Prerequisites

To use the enhanced NLM Python bindings, you need:

1. **NLM C++ Framework**: The NLM source code with all components
2. **pybind11**: Python bindings library
3. **C++ Compiler**: Support for C++17
4. **Python Development Headers**: For building Python extensions

## Quick Start Installation

### 1. Clone the NLM repository
```bash
git clone https://github.com/yourusername/nlm.git
cd nlm
```

### 2. Install dependencies
```bash
# Install pybind11
pip install pybind11

# Install other dependencies as needed
pip install numpy matplotlib scipy
```

### 3. Configure and build
```bash
# Create build directory
mkdir build
cd build

# Configure with enhanced features
cmake .. -DNLM_PERFORMANCE_ENABLED=ON \
          -DNLM_USE_MEMORY_POOL=ON \
          -DNLM_USE_SOA=ON \
          -DNLM_USE_EVENT_QUEUE=ON \
          -DNLM_USE_SPARSE_CONNECTIVITY=ON \
          -DNLM_USE_MULTITHREADING=ON \
          -DNLM_USE_SIMD=ON \
          -DNLM_SIMD_AVX2=ON

# Build the library
make -j$(nproc)
```

### 4. Install Python module
```bash
# Install the enhanced bindings
cd ../python
pip install -e .
```

## Manual Installation

If you prefer manual compilation, follow these steps:

### Compile Python bindings
```bash
cd /path/to/nlm/python

# Compile with enhanced features
g++ -std=c++17 -I/usr/include/python3.10 -I../include -I../external/pybind11 -I../src \
    -DNLM_PERFORMANCE_ENABLED \
    -DNLM_USE_MEMORY_POOL \
    -DNLM_USE_SOA \
    -DNLM_USE_EVENT_QUEUE \
    -DNLM_USE_SPARSE_CONNECTIVITY \
    -DNLM_USE_MULTITHREADING \
    -DNLM_USE_SIMD \
    -DNLM_SIMD_AVX2 \
    -shared \
    -fPIC \
    -o pynlm.cpython-310-x86_64-linux-gnu.so \
    bindings.cpp \
    -lpython3.10
```

### Install the module
```bash
pip install pynlm
```

## Testing the Enhanced Bindings

### Run example scripts
```bash
# Run the comprehensive examples
python examples/enhanced_bindings_example.py
```

### Run unit tests (if available)
```bash
# Look for test files in the tests directory
test_enhanced_bindings.py
```

## API Reference

For detailed API documentation, see the `docs/` directory:

- `EnhancedPythonBindings.md`: Comprehensive API documentation
- `APIReference.md`: Detailed method descriptions
- `Examples.md`: Practical usage examples

## Configuration Examples

### Environment-based configuration
```bash
# Set environment variables
export NLM_BRAIN_NEURON_COUNT=2000
export NLM_WORLD_WIDTH=150.0
export NLM_WORLD_HEIGHT=120.0
export NLM_BRAIN_PLACILITYITY_ENABLE=true
export NLM_BRAIN_DEVELOPMENT_ENABLE=true
```

### YAML configuration file
```yaml
# config.yaml
brain:
  neuronCount: 2000
  synapseDensity: 0.1
  plasticity:
    enable: true
  development:
    enable: true
  rewardModulation:
    enable: true
  curiosity:
    enable: true
  novelty:
    enable: true
  prediction:
    enable: true

world:
  width: 150.0
  height: 120.0
  visionWidth: 20
  visionHeight: 20

agent:
  energy:
    max: 100.0
    decay: 0.01
  simulation:
    timestep: 0.01
```

### JSON configuration file
```json
{
    "brain": {
        "neuronCount": 2000,
        "synapseDensity": 0.1,
        "plasticity": {
            "enable": true
        },
        "development": {
            "enable": true
        }
    },
    "world": {
        "width": 150.0,
        "height": 120.0,
        "visionWidth": 20,
        "visionHeight": 20
    }
}
```

## Performance Optimization

### For large-scale simulations
1. **Enable performance features during compilation**
   ```bash
   cmake .. -DNLM_PERFORMANCE_ENABLED=ON -DNLM_USE_MULTITHREADING=ON -DNLM_SIMD_AVX2=ON
   ```

2. **Configure brain with optimal settings**
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuronCount", 50000)  # Large network
   config.set("brain.plasticity.enable", True)
   config.set("brain.performance.monitoring", True)
   ```

3. **Use batch operations for multiple experiments**
   ```python
   configs = []
   for i in range(10):
       config = pynlm.createDefaultConfig()
       config.set("brain.neuronCount", 1000 * (i + 1))
       configs.append(config)
   ```

### Memory management
- Monitor memory usage with `brain.getPerformanceMetrics()`
- Use `config.validate()` to ensure configuration fits available memory
- Consider using memory pools for long-running simulations

## Troubleshooting

### Common Issues

#### Import errors
```bash
# Error: cannot import name 'EnhancedConfig'
# Solution: Check that the module was installed correctly
pip install pynlm
python -c "import pynlm; print(pynlm.__version__)"
```

#### Missing dependencies
```bash
# Error: No module named 'yaml'
# Solution: Install YAML support
pip install pyyaml
```

#### Compilation errors
```bash
# Error: 'nlohmann/json.hpp' not found
# Solution: Install json development package
apt-get update && apt-get install nlohmann-json3-dev
```

#### Performance issues
```python
# Check performance metrics
brain = pynlm.createEnhancedBrain(config)
perf = brain.getPerformanceMetrics()
if perf.cpuUsage > 80.0:
    print("High CPU usage - consider reducing neuron count or enabling parallelization")
```

### Debugging
```python
# Enable detailed debugging
brain = pynlm.createEnhancedBrain(config)
brain.enableVisualization(True)
debug_info = brain.getDebugInfo()
print(f"Debug info: {debug_info}")
```

## Migration Guide

### From Original to Enhanced API

#### Configuration
**Original:**
```python
config = pynlm.Config()
config.loadFromFile("config.json")
config.set("neuronCount", 1000)
```

**Enhanced:**
```python
config = pynlm.EnhancedConfig()
config.loadFromFile("config.yaml")  # Supports both JSON and YAML
config.set("brain.neuronCount", 1000)
```

#### Brain
**Original:**
```python
brain = pynlm.Brain(config)
brain.initialize()
brain.step(0)
```

**Enhanced:**
```python
brain = pynlm.createEnhancedBrain(config)  # Factory function
if brain.initializeWithDiagnostics():
    brain.stepAdvanced(timestep=0.01)
```

#### Agent
**Original:**
```python
agent = pynlm.AgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
```

**Enhanced:**
```python
agent = pynlm.createEnhancedAgentBrain(brain)
agent.initializeWithWorld(world)
agent.processSensoryInputEnhanced(percept)
```

## Advanced Features

### Custom Configuration Templates
```python
def create_rl_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.rewardModulation.enable", True)
    config.set("brain.curiosity.enable", True)
    config.set("brain.novelty.enable", True)
    config.set("brain.prediction.enable", True)
    return config

def create_developmental_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.development.enable", True)
    config.set("brain.synaptogenesis.enable", True)
    config.set("brain.pruning.enable", True)
    return config
```

### Experiment Automation
```python
def run_parameter_sweep(param_grid):
    """Run parameter sweep over multiple configurations"""
    results = []
    
    for params in param_grid:
        config = pynlm.createDefaultConfig()
        for key, value in params.items():
            config.set(key, value)
        
        brain = pynlm.createEnhancedBrain(config)
        if brain.initializeWithDiagnostics():
            for step in range(100):
                brain.stepAdvanced(timestep=0.01)
            
            debug_info = brain.getDebugInfo()
            results.append({
                'params': params,
                'success': True,
                'final_neurons': debug_info.activeNeurons
            })
        else:
            results.append({
                'params': params,
                'success': False,
                'error': 'Initialization failed'
            })
    
    return results
```

### Visualization and Analysis
```python
def analyze_simulation(config, steps=1000):
    """Run simulation and analyze results"""
    brain = pynlm.createEnhancedBrain(config)
    
    # Enable visualization
    brain.enableVisualization(True)
    
    # Run simulation
    for step in range(steps):
        brain.stepAdvanced(timestep=0.01, collectStats=True)
        brain.updateVisualization()
    
    # Analyze results
    debug_info = brain.getDebugInfo()
    learning_progress = brain.getLearningProgress()
    perf_metrics = brain.getPerformanceMetrics()
    
    return {
        'debug_info': debug_info,
        'learning_progress': learning_progress,
        'performance_metrics': perf_metrics
    }
```

## Future Enhancements

The enhanced bindings will continue to evolve with:

1. **Real-time monitoring**: Web-based dashboards for simulation monitoring
2. **Cloud integration**: Support for distributed simulations
3. **Advanced visualization**: 3D neuron rendering and network analysis
4. **Machine learning integration**: Neural architecture search and hyperparameter optimization
5. **Export capabilities**: Export simulation results to various formats

## License

The enhanced Python bindings are distributed under the same license as the NLM framework.
See the LICENSE file for details.

## Support

For support with the enhanced Python bindings:

1. **Documentation**: Check the `docs/` directory for detailed information
2. **Examples**: Run the example scripts in the `examples/` directory
3. **Issue tracking**: Report issues on the NLM GitHub repository
4. **Community**: Join the NLM community forum for discussions

## Authors

This enhanced Python API was developed as part of the NLM framework enhancement project.

For more information, visit the NLM project website:
https://github.com/yourusername/nlm

---

*Last updated: $(date +'%Y-%m-%d %H:%M:%S')*
