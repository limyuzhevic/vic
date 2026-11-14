# NLM - Comprehensive User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain system. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

### Key Features
- **Real neural computation**: LIF neurons, STDP and Hebbian plasticity
- **Event-driven simulation**: Efficient spike propagation with delays
- **Modular architecture**: Separates sensory, cognitive, motor, and developmental systems
- **Advanced learning**: Neuromodulation, curiosity-driven exploration, prediction systems
- **Memory systems**: Working memory, episodic memory, and associative memory
- **Developmental stages**: Brain matures from initial state to adult functionality

## Getting Started

### Prerequisites

#### C++ Development (for building from source)
- C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- Optional: pybind11 for Python bindings

#### Python Users
- Python 3.8+ (recommended)
- pip (Python package installer)
- Optionally: numpy for advanced array operations

### Quick Installation

#### Option 1: Install Python Bindings (Recommended for most users)

```bash
# Install from source (most reliable)
pip install -e .

# Or install build dependencies and build
pip install scikit-build-core pybind11 numpy
pip install .

# For development mode
pip install -e .[dev]
```

#### Option 2: Build C++ Library

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Quick Start (Python)

#### Basic Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Complete Agent Example

```python
import pynlm

def run_complete_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"DevStage={brain.getDevelopmentalStage()}")
    
    print("Simulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation()
```

### Quick Start (C++)

#### Basic Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("connection_probability", 0.1);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step 
                     << ": " << brain->getFiringNeuronCount() 
                     << " firing neurons" << std::endl;
        }
    }
    
    return 0;
}
```

## Core Concepts

### 1. Brain Architecture

NLM's brain is organized into several interconnected systems:

#### Neural Core
- **Neurons**: Leaky Integrate-and-Fire (LIF) model
- **Synapses**: Event-driven with delays and plasticity
- **Populations**: Grouped neurons with shared properties
- **Regions**: Functional subdivisions of the brain

#### Processing Systems
- **Sensory Processing**: Vision, audio, and internal signal processing
- **Motor Output**: Action selection and execution
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Prediction Systems**: Forward and inverse models for anticipating outcomes
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Development**: Age-dependent plasticity and maturation

#### Cognitive Systems
- **Attention**: Competitive selection of neural representations
- **Planning**: Neural evaluation of action sequences
- **Concept Formation**: Pattern abstraction from experience
- **Self-Model**: Internal representation of the agent's capabilities

### 2. Developmental Stages

NLM brains develop through several stages:

1. **Initial**: Primitive neural substrate with basic properties
2. **CriticalPeriod**: High plasticity, rapid learning
3. **Maturation**: Synaptic pruning and specialization
4. **Adult**: Stable performance with refined capabilities
5. **Aging**: Gradual decline in performance and plasticity

### 3. Learning Mechanisms

#### Plasticity Rules
- **STDP (Spike-Timing-Dependent Plasticity)**: Correlation-based learning
- **Hebbian Learning**: co-activation strengthening
- **Structural Plasticity**: Creation and elimination of synapses

#### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration of novel states
- **Novelty**: Detection of unexpected sensory input
- **Prediction Error**: Mismatch between expected and actual outcomes

### 4. Agent-World Interaction

The agent interacts with its environment through a sensory-motor loop:

1. **Perception**: World updates generate sensory percepts
2. **Processing**: Brain processes sensory input
3. **Action**: Brain produces motor commands
4. **Execution**: World executes actions and produces rewards
5. **Learning**: Neuromodulation updates neural connectivity

## Advanced Features

### 1. Configuration Management

NLM uses a flexible configuration system supporting:

#### Configuration Types
- **Scalar values**: Integers, floats, booleans, strings
- **Arrays**: Lists of numeric or string values
- **Nested configurations**: Dot-notation access (e.g., "brain.neuron_count")
- **File-based configuration**: JSON and key=value formats

#### Configuration Sources
- **Default**: Built-in defaults
- **File**: Loaded from JSON or text files
- **Command Line**: Override via CLI arguments
- **Runtime**: Set programmatically

#### Configuration Examples

```python
# Create configuration
config = nlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("development.initial_plasticity", 0.8)

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 1000)
connection_prob = config.getOr("brain.connection_probability", 0.1)

# Save configuration
config.saveToFile("my_config.json")
```

### 2. Memory Systems

#### Working Memory
- **Active traces**: Maintain recent neural activity patterns
- **Competitive dynamics**: Select dominant representations
- **Capacity limits**: Limited number of active traces

#### Episodic Memory
- **Experience encoding**: Store sequences of states and actions
- **Replay mechanisms**: Consolidate learned patterns during rest
- **Pattern completion**: Retrieve similar past experiences

#### Associative Memory
- **Hebbian associations**: Strengthen co-occurring patterns
- **Pattern completion**: Retrieve full patterns from partial input
- **Forgetting**: Gradual decay of unused associations

### 3. Performance and Scalability

#### Optimization Features
- **Memory pools**: Efficient allocation/deallocation
- **Event queues**: Optimized spike scheduling
- **SIMD vectorization**: Parallel processing of neuron computations
- **Parallel processing**: Multi-core support
- **Checkpointing**: Save/load simulation state

#### Performance Monitoring

```python
# Monitor brain performance
brain = nlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        stats = {
            'firing_rate': brain.getAverageFiringRate(),
            'spike_count': brain.getTotalSpikeCount(),
            'active_neurons': brain.getFiringNeuronCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        }
        print(f"Performance at step {step}: {stats}")
```

### 4. Debugging and Development Tools

#### Logging and Debugging

```python
import pynlm

# Enable verbose logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create brain with detailed logging
config = nlm.createDefaultConfig()
config.set("logger.level", "Debug")

brain = nlm.createBrain(config)
brain.initialize()

# Run with debugging
for step in range(50):
    brain.step(step)
    
    # Check for issues
    if brain.getFiringNeuronCount() == 0 and step > 10:
        print(f"Warning: No neurons firing at step {step}")
    
    # Log development
    if step % 10 == 0:
        brain.logStatus()
```

#### Checkpoint and Debugging

```python
# Save simulation state
brain.save("debug_checkpoint.bin")

# Restore simulation state (useful for debugging)
brain.load("debug_checkpoint.bin")

# Test different configurations
for config_variant in ["config1.json", "config2.json", "config3.json"]:
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.loadConfig(config_variant)
    brain.initialize()
    # ... run simulation ...
```

## Building and Development

### Building from Source

#### CMake Options

```bash
# Configure with build options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PERFORMANCE_TOOLS=ON \
    -DNLM_ENABLE_PROFILING=ON \
    -DPYTHON_EXECUTABLE=/usr/bin/python3

# Build with specific targets
make -j$(nproc) nlm_core nlm_agent nlm_world
make nlm_benchmark      # Performance benchmark
make nlm_phase3_demo    # Phase 3 demonstration
make nlm_phase6_demo    # Phase 6 integration demo
```

#### Building Python Bindings

```bash
# Method 1: Using pip (recommended)
pip install -e .

# Method 2: Using scikit-build-core directly
pip install scikit-build-core pybind11
python -m pip install --no-build-isolation .

# Method 3: Using CMake directly
cmake -S . -B build -DNLM_BUILD_PYTHON_BINDINGS=ON
cmake --build build --target pynlm
```

### Running Tests

#### C++ Tests

```bash
# Run unit tests
./build/nlm_test

# Or use CTest
ctest --output-on-failure
```

#### Python Tests

```bash
# Install test dependencies
pip install pytest numpy

# Run Python tests
pytest tests/

# Run specific test file
pytest tests/test_basic.py
```

### Development Best Practices

#### Coding Standards
- **C++**: C++20 with explicit headers and forward declarations
- **Python**: PEP 8 compliance, type hints, comprehensive docstrings
- **Documentation**: Markdown with code examples

#### Testing Strategy
- **Unit tests**: Test individual components
- **Integration tests**: Test system-wide interactions
- **Performance tests**: Benchmark key operations
- **Regression tests**: Ensure no behavioral changes

#### Profiling and Optimization

```bash
# Profile C++ application
gdb ./nlm_benchmark
# (use 'profile' command in gdb)

# Profile Python application
python -m cProfile -o profile.stats test_script.py

# Visualize Python profiling results
snakeviz profile.stats
```

## Examples and Tutorials

### Tutorial 1: Simple Neural Network

Learn how to create and run a basic neural network:

```python
import pynlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### Tutorial 2: Agent in Environment

Learn how to create an agent that interacts with its environment:

```python
import pynlm

# Setup
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run episode
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Tutorial 3: Advanced Learning

Learn how to enable and use advanced learning features:

```python
import pynlm

# Create and configure agent
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = world.getSensoryPercept().getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

### Tutorial 4: Experiment Design

Learn how to design and run scientific experiments:

```python
import pynlm
import json
from datetime import datetime

class NLMScienceExperiment:
    def __init__(self, name):
        self.name = name
        self.results = []
        self.start_time = datetime.now()
    
    def run_experiment(self, config_file, num_steps=1000):
        # Load configuration
        with open(config_file, 'r') as f:
            config_params = json.load(f)
        
        config = nlm.createDefaultConfig()
        for key, value in config_params.items():
            config.set(key, value)
        
        # Setup
        brain = nlm.createBrain(config)
        brain.initialize()
        
        agent = nlm.createAgentBrain(brain)
        world = nlm.createSimpleWorld()
        world.configure(**config_params.get('world', {}))
        world.reset()
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        
        # Run experiment
        for step in range(num_steps):
            self.run_step(world, agent, brain, step)
            
            if step % 100 == 0:
                self.log_progress(step, brain, agent)
        
        # Save results
        self.save_results()
        return self.results
    
    def run_step(self, world, agent, brain, step):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    def log_progress(self, step, brain, agent):
        progress = {
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        }
        self.results.append(progress)
        print(f"Step {step}: {progress}")
    
    def save_results(self):
        filename = f"{self.name}_results.json"
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"Results saved to {filename}")

# Run an experiment
experiment = NLMScienceExperiment("neural_development")
results = experiment.run_experiment("experiment_config.json", 1000)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain doesn't initialize

**Problem**: Brain fails to initialize with "Failed to initialize brain!"

**Solution**:

1. Check configuration values:
   ```python
   config = nlm.createDefaultConfig()
   print(config.getKeys())  # Verify keys exist
   ```

2. Ensure required configuration is set:
   ```python
   config.set("brain.neuron_count", 100)
   config.set("region_count", 1)
   config.set("connection_probability", 0.05)
   ```

3. Check system resources (memory/CPU)

#### Issue: Agent isn't moving

**Problem**: Agent produces no actions or doesn't move

**Solution**:

1. Verify agent is initialized:
   ```python
   agent = nlm.createAgentBrain(brain)
   agent.initialize(world)  # Must call initialize
   ```

2. Check world is being updated:
   ```python
   world.update(0.1)  # Must update world before getting percept
   ```

3. Verify sensory input is connected:
   ```python
   percept = world.getSensoryPercept()
   agent.processSensoryInput(percept)  # Must process input
   ```

4. Check motor command decoding:
   ```python
   action = agent.decodeMotorCommand()
   world.applyMotorCommand(action, world.getSimulationTime())
   ```

#### Issue: Configuration file not found

**Problem**: `loadFromFile` returns false

**Solution**:

1. Verify file path exists:
   ```python
   import os
   print(os.path.exists("configs/my_config.json"))
   ```

2. Check file format (JSON or key=value):
   ```json
   // JSON format
   {
       "brain.neuron_count": 1000,
       "brain.connection_probability": 0.1
   }
   ```

   ```
   # key=value format
   brain.neuron_count = 1000
   brain.connection_probability = 0.1
   # Other comments are ignored
   ```

3. Ensure file is readable:
   ```python
   try:
       config.loadFromFile("configs/my_config.json")
   except Exception as e:
       print(f"Error loading config: {e}")
   ```

#### Issue: Performance is too slow

**Problem**: Simulation runs very slowly

**Solution**:

1. Reduce complexity:
   ```python
   config = nlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Fewer neurons
   config.set("region_count", 1)           # Fewer regions
   ```

2. Use smaller simulation time:
   ```python
   for step in range(100):  # Fewer steps
       brain.step(step)     # With appropriate timestep
   ```

3. Check configuration for optimizations:
   ```python
   config.set("performance.optimize_memory", True)
   config.set("performance.use_simd", True)
   ```

4. Profile performance:
   ```python
   import cProfile
   cProfile.run('run_simulation()', 'profile_stats')
   ```

#### Issue: Python bindings not installed

**Problem**: `import pynlm` fails

**Solution**:

1. Check if package is installed:
   ```bash
   pip list | grep pynlm
   ```

2. Install or reinstall:
   ```bash
   pip install -e .
   ```

3. Check build:
   ```bash
   python -c "import sys; sys.path.append('build'); import pynlm"
   ```

4. Build from source:
   ```bash
   mkdir build && cd build
   cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
   cmake --build . --target pynlm
   ```

### Error Messages Reference

#### Initialization Errors
- "Failed to initialize brain!" → Check configuration and system resources
- "No neurons found!" → Increase neuron count or check configuration
- "Invalid configuration!" → Check configuration values

#### Runtime Errors
- "World boundary collision!" → Check agent movement logic
- "Memory allocation failed!" → Increase system resources or reduce complexity
- "Configuration file not found!" → Verify file path and permissions

#### Warning Messages
- "Using default configuration." → Configuration file not found, using defaults
- "Checkpoint save/load not implemented!" → Using basic checkpointing
- "No objects in environment!" → Environment may need setup

## Performance Guidelines

### Recommended Configuration for Different Use Cases

#### Development and Testing
```python
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 100)      # Small for quick testing
config.set("region_count", 1)
config.set("max_steps", 1000)              # Short simulation
config.set("performance.optimize_memory", True)
```

#### Research and Development
```python
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)     # Moderate size
config.set("region_count", 4)
config.set("max_steps", 10000)             # Longer simulation
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("development.enable", True)
```

#### Production and Deployment
```python
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)    # Large scale
config.set("region_count", 8)
config.set("max_steps", 50000)             # Long-running
config.set("performance.use_simd", True)
config.set("performance.parallel", True)
config.set("checkpointing.enable", True)
config.set("checkpointing.interval", 1000)
```

### Performance Monitoring

```python
import time
import pynlm

class PerformanceMonitor:
    def __init__(self):
        self.times = []
        self.steps = []
    
    def start_timing(self):
        self.start_time = time.time()
    
    def end_timing(self, step):
        elapsed = time.time() - self.start_time
        self.times.append(elapsed)
        self.steps.append(step)
    
    def report(self):
        if not self.times:
            return "No timing data"
        
        avg_time = sum(self.times) / len(self.times)
        min_time = min(self.times)
        max_time = max(self.times)
        steps_per_sec = len(self.times) / sum(self.times) if sum(self.times) > 0 else 0
        
        return f"""
Performance Summary:
  Average step time: {avg_time:.3f}s
  Min step time: {min_time:.3f}s
  Max step time: {max_time:.3f}s
  Steps per second: {steps_per_sec:.1f}
  Total steps: {len(self.steps)}
  Total time: {sum(self.times):.1f}s
        """.strip()

# Use the monitor
monitor = PerformanceMonitor()
for step in range(100):
    monitor.start_timing()
    brain.step(step)
    monitor.end_timing(step)

print(monitor.report())
```

## Conclusion

NLM provides a comprehensive framework for artificial brain development and experimentation. This guide covers:

- **Getting started** with both Python and C++ interfaces
- **Core concepts** of neural computation and learning
- **Advanced features** for research and production use
- **Building and development** with CMake and Python packaging
- **Examples and tutorials** for rapid learning
- **Troubleshooting** for common issues
- **Performance guidelines** for optimal usage

The enhanced NLM system offers significant improvements over the original implementation, including:

1. **Better Python API** with convenience methods and comprehensive documentation
2. **Improved configuration system** with JSON/YAML support
3. **Enhanced error handling** with better debugging information
4. **Advanced learning features** for more sophisticated experiments
5. **Performance optimizations** for large-scale simulations
6. **Comprehensive testing** and monitoring capabilities

This guide should provide everything needed to effectively use NLM for neural computation research, education, and development.

## References

1. Freeman, W. J. (2000). The physiological basis of mental activity. *Annual Review of Neuroscience*, 23, 475-501.
2. Gerstner, W., Kistler, W., Naud, R., & Chancellor, N. (2014). *Spiking Neuron Models*. Cambridge University Press.
3. Bostrom, N. (2014). *Superintelligence: Paths, Dangers, Strategies*. Oxford University Press.
4. Yamins, D. L., & DiCarlo, J. J. (2016). Deep learning in neural systems for visual object recognition. *Proceedings of the National Academy of Sciences*, 113(31), 8767-8776.

For more information, visit the NLM documentation website or the project repository.