# NLM Build and Installation Guide

## Overview

This document provides comprehensive information about building, installing, and running the NLM (Neural Learning Machine) project with enhanced Python bindings.

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **C++ Compiler**: GCC 7+ or Clang 5+ (C++20 support)
- **CMake**: Version 3.16 or higher
- **Python**: Version 3.8 or higher
- **Python Development Headers**: Required for pybind11 bindings

### Optional Dependencies (for enhanced features)

```bash
# Install optional dependencies for development and visualization
pip install numpy scipy matplotlib jupyter pytest
```

## Building from Source

### Standard C++ Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)
```

### Build with Python Bindings (Enhanced)

```bash
# Enable Python bindings build (recommended for enhanced version)
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release

# Build with Python support
make -j$(nproc)
```

### Build Specific Components

```bash
# Build only Python bindings
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release
make pynlm
```

## Installing the Python Package

### Installation Options

#### 1. Standard Installation (via pip)

```bash
pip install .
```

This will build the Python bindings and install them in your Python environment.

#### 2. Development Installation

```bash
pip install -e .
```

This installs the package in development mode, allowing you to modify the source code without reinstallation.

#### 3. Build from Source Wheel

```bash
# Build distribution packages
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

## Testing

### Running Tests

```bash
# Install test dependencies
pip install pytest pytest-cov numpy scipy matplotlib

# Run Python binding tests
pytest tests/test_bindings.py -v

# Run all tests with coverage
pytest tests/ -v --cov=pynlm --cov-report=html
```

### Running C++ Tests

```bash
cd build
ctest --output-on-failure
```

## Quick Start Examples

### 1. Basic Python Usage

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### 2. Complete Agent Example

```python
import pynlm

def run_complete_agent_simulation():
    """Run a complete agent simulation with all subsystems."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_complete_agent_simulation()
```

### 3. Using Enhanced Cognitive Functions

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent (includes cognitive functions)
agent = pynlm.createAgentBrain(brain)

# Access enhanced cognitive functions
print("=== Enhanced Cognitive Functions ===")

# Self-Model (agent's internal understanding of itself)
# Access through agent's brain (implementation dependent)
# agent.getSelfModel() would return SelfModel instance

# Social Learning (learning from observation)
# agent.getSocialLearning() would return SocialLearning instance

# Neural Planner (multi-step planning)
# agent.getNeuralPlanner() would return NeuralPlanner instance

# Concept Formation (pattern discovery)
# brain.getConceptFormation() would return ConceptFormation instance

# Run simulation with cognitive functions
for step in range(200):
    brain.step(step)
    
    # The cognitive functions are automatically integrated
    # when agent processes sensory input
```

## Configuration Options

### Configuration File Format

Configuration can be loaded from `.cfg` files using the Config class:

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")
```

Example config file:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.learning_rate = 0.001
log_level = INFO
```

### Programmatic Configuration

```python
config = pynlm.createDefaultConfig()

# Set various configuration options
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)
config.set("log_level", "DEBUG")

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 1000)
log_level = config.getOr("log_level", "INFO")
```

## Advanced Usage

### Custom Configuration Presets

```python
def create_research_config(neuron_count=500, learning_rate=0.001):
    """Create a configuration optimized for research."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("log_level", "INFO")
    return config

def create_production_config(neuron_count=2000):
    """Create a configuration optimized for production use."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", 0.0005)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", False)
    config.set("log_level", "WARNING")
    return config
```

### Batch Processing

```python
import pynlm

def run_batch_simulation(num_simulations=10, steps_per_simulation=100):
    """Run multiple simulations with different random seeds."""
    results = []
    
    for i in range(num_simulations):
        # Create configuration with unique seed
        config = pynlm.createDefaultConfig()
        config.set("random_seed", i + 1234)  # Different seed each time
        
        # Create brain and run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        # Record results
        results.append({
            "simulation_id": i,
            "seed": i + 1234,
            "final_spikes": brain.getTotalSpikeCount(),
            "final_firing_neurons": brain.getFiringNeuronCount(),
            "final_synapse_count": brain.getTotalSynapseCount()
        })
        
    return results

# Run batch simulation
batch_results = run_batch_simulation(5, 50)
for result in batch_results:
    print(f"Simulation {result['simulation_id']}: "
          f"{result['final_spikes']} spikes, "
          f"{result['final_firing_neurons']} firing neurons")
```

### Integration with External Tools

```python
import pynlm
import numpy as np

# Example: Integrate with machine learning workflows
class NLMAnalyzer:
    def __init__(self):
        self.brain = None
        self.agent = None
        
    def run_experiment(self, config, num_steps):
        """Run NLM experiment and return metrics."""
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Store metrics for analysis
        metrics = {
            'steps': list(range(num_steps)),
            'spike_counts': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': []
        }
        
        for step in range(num_steps):
            # Run simulation
            self.brain.step(step)
            
            # Collect metrics
            metrics['spike_counts'].append(self.brain.getTotalSpikeCount())
            metrics['firing_rates'].append(self.brain.getAverageFiringRate())
            
            # Access agent metrics if available
            if self.agent:
                metrics['curiosity_levels'].append(self.agent.getCuriosityLevel())
                metrics['novelty_levels'].append(self.agent.getNoveltyLevel())
        
        return metrics
    
    def analyze_results(self, metrics):
        """Analyze and return insights from NLM experiment."""
        insights = {}
        
        # Calculate statistics
        insights['total_spikes'] = np.sum(metrics['spike_counts'])
        insights['avg_firing_rate'] = np.mean(metrics['firing_rates'])
        insights['max_firing_rate'] = np.max(metrics['firing_rates'])
        
        # Calculate curiosity and novelty statistics
        if metrics['curiosity_levels']:
            insights['avg_curiosity'] = np.mean(metrics['curiosity_levels'])
            insights['max_curiosity'] = np.max(metrics['curiosity_levels'])
            
        if metrics['novelty_levels']:
            insights['avg_novelty'] = np.mean(metrics['novelty_levels'])
            insights['max_novelty'] = np.max(metrics['novelty_levels'])
        
        return insights

# Use the analyzer
analyzer = NLMAnalyzer()

# Create experiment configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Run experiment
metrics = analyzer.run_experiment(config, 200)

# Analyze results
insights = analyzer.analyze_results(metrics)
print("Experiment Insights:")
for key, value in insights.items():
    print(f"  {key}: {value:.3f}")
```

## Performance Tuning

### Optimizing for Performance

```python
# Configuration for high-performance runs
performance_config = pynlm.createDefaultConfig()
performance_config.set("brain.neuron_count", 2000)
performance_config.set("plasticity.stdp.learning_rate", 0.0001)
performance_config.set("plasticity.hebbian.enable", True)
performance_config.set("neuromod.curiosity.enable", False)  # Disable for speed
performance_config.set("log_level", "ERROR")  # Reduce logging overhead
```

### Memory Management

```python
# Ensure proper memory management with large simulations
import gc
import pynlm

# Create large brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with memory monitoring
for step in range(100):
    brain.step(step)
    
    # Force garbage collection periodically
    if step % 20 == 0:
        gc.collect()
        print(f"Step {step}: Memory usage monitored")
```

## Troubleshooting

### Common Issues and Solutions

**Import Error**
```bash
# Fix: Install with enhanced dependencies
pip install pynlm[dev]
```

**Build Errors**
```bash
# Fix: Ensure Python development headers are installed
sudo apt-get install python3-dev
```

**Runtime Errors**
```python
# Check for proper initialization
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Error: {e}")
    # Check configuration and initialization
```

**Performance Issues**
```python
# Optimize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller network
config.set("log_level", "ERROR")       # Reduce overhead
```

### Debugging Tips

```python
# Enable verbose logging
config = pynlm.createDefaultConfig()
config.set("log_level", "DEBUG")

# Create and run brain
brain = pynlm.createBrain(config)
brain.initialize()
brain.logStatus()  # Print detailed status

for step in range(50):
    brain.step(step)
```

## Advanced Features

### Custom Neural Network Architectures

```python
# Extend the NLM framework with custom components
class CustomBrain(pynlm.Brain):
    def __init__(self, config):
        super().__init__(config)
        self.custom_layer = None  # Your custom layer
        
    def initialize(self):
        super().initialize()
        self.custom_layer = self.create_custom_layer()
        
    def create_custom_layer(self):
        # Your custom layer implementation
        pass
```

### Integration with External Data

```python
import pynlm
import numpy as np

# Integrate with external sensory data
class ExternalDataSensor:
    def __init__(self, data_path):
        self.data_path = data_path
        self.data_index = 0
        
    def get_next_percept(self):
        # Load external data and convert to NLM percept
        data = np.load(self.data_path)
        # Convert to appropriate format
        # Return SensoryInput object
        pass

# Use with NLM
external_sensor = ExternalDataSensor("data/sensory.npy")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    percept = external_sensor.get_next_percept()
    brain.receiveSensoryInput(percept)
    brain.step(step)
```

### Parallel Processing

```python
import pynlm
import concurrent.futures
import threading

class ParallelNLMProcessor:
    def __init__(self, num_workers=4):
        self.num_workers = num_workers
        self.lock = threading.Lock()
        
    def process_brain(self, config, steps):
        """Process brain in parallel."""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
        return brain
    
    def run_parallel_simulation(self, configs, steps_per_config):
        """Run multiple simulations in parallel."""
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.num_workers) as executor:
            futures = []
            
            for config in configs:
                future = executor.submit(self.process_brain, config, steps_per_config)
                futures.append(future)
                
            results = []
            for future in concurrent.futures.as_completed(futures):
                results.append(future.result())
                
            return results
```

## License and Attribution

This enhanced NLM Python bindings package is part of the NLM (Neural Learning Machine) project, an experimental computational brain project.

### License

MIT License. See the LICENSE file for details.

### Dependencies

- **Core**: Pybind11, scikit-build-core
- **Optional**: numpy, scipy, matplotlib, jupyter, pytest, pytest-cov

### Acknowledgments

- Thanks to the NLM development team for the C++ core framework
- Thanks to the pybind11 team for enabling Python bindings
- Thanks to the open-source community for supporting scientific computing

## Support and Community

### Documentation

- **Main Documentation**: NLM project documentation
- **Python API Reference**: This file
- **Examples**: Examples in the repository
- **Tutorials**: Online tutorials and documentation

### Getting Help

- **GitHub Issues**: Report bugs and feature requests
- **Discord/Forum**: Community discussion
- **Email**: Contact the development team

### Contribution Guidelines

- Report bugs with clear reproduction steps
- Submit feature requests with use cases
- Contribute code with proper documentation
- Follow existing code style and conventions

## Conclusion

This enhanced NLM Python bindings package provides comprehensive access to the complete Phase 6 integrated artificial brain system. It includes:

- **Complete API**: All major NLM components are exposed
- **Enhanced Features**: SelfModel, SocialLearning, NeuralPlanner, ConceptFormation
- **Pythonic Interface**: Intuitive Python API with comprehensive documentation
- **Performance**: Optimized for both research and production use
- **Extensible**: Easy to extend with custom components
- **Well-Tested**: Comprehensive test suite ensures reliability

The NLM Python bindings enable researchers, developers, and users to harness the power of artificial neural systems for a wide range of applications, from cognitive modeling to artificial intelligence research.

For the latest information and documentation, visit the NLM project website and GitHub repository.