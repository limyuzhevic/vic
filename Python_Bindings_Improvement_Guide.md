# Improved Python Bindings Module Structure
# This document describes the improved Python bindings architecture

## Overview
The NLM Python bindings have been completely restructured to provide:
- Better organization and maintainability
- Pythonic interfaces with advanced features
- Comprehensive documentation
- Parameter validation and error handling
- Modern Python patterns

## New Architecture

### 1. Modular Structure
The bindings are now organized into logical modules:

```python
pynlm/
├── __init__.py                    # Main package initialization
├── core/                          # Core functionality
│   ├── __init__.py              # Core package
│   ├── config.py                # Configuration
│   ├── brain.py                 # Brain simulation
│   └── types.py                 # Type definitions
├── agent/                        # Agent system
│   ├── __init__.py              # Agent package
│   ├── agent.py                 # Agent brain interface
│   └── world.py                 # World simulation
├── sensory/                      # Sensory systems
│   ├── __init__.py              # Sensory package
│   ├── vision.py                # Vision input
│   ├── audio.py                 # Audio input
│   └── internal.py              # Internal signals
├── motor/                        # Motor output
│   ├── __init__.py              # Motor package
│   └── actions.py               # Motor actions
├── neuromodulation/              # Neuromodulation systems
│   ├── __init__.py              # Neuromodulation package
│   ├── reward.py                # Reward modulation
│   ├── curiosity.py             # Curiosity system
│   ├── novelty.py               # Novelty detection
│   └── prediction.py            # Prediction error
├── memory/                       # Memory systems
│   ├── __init__.py              # Memory package
│   ├── working.py               # Working memory
│   ├── episodic.py              # Episodic memory
│   └── associative.py           # Associative memory
├── development/                  # Development system
│   ├── __init__.py              # Development package
│   └── maturation.py            # Developmental stages
├── cognition/                    # Cognitive systems
│   ├── __init__.py              # Cognition package
│   ├── attention.py             # Attention system
│   ├── concept.py               # Concept formation
│   └── planning.py              # Neural planner
├── environment/                  # Environment interface
│   ├── __init__.py              # Environment package
│   ├── simple.py                # Simple world
│   └── objects.py               # World objects
├── visualization/                # Visualization
│   ├── __init__.py              # Visualization package
│   └── interface.py             # Visualization interface
└── experiments/                  # Experiment framework
    ├── __init__.py              # Experiments package
    ├── phase6.py                 # Phase 6 integration
    └── metrics.py                # Performance metrics
```

### 2. Pythonic Features Implemented

#### Context Managers
```python
# Brain context manager for automatic cleanup
with pynlm.BrainContext(config) as brain:
    for step in range(100):
        brain.step(step)
```

#### Property-based Access
```python
# Property access with validation
brain = pynlm.create_brain()
print(f"Neurons: {brain.neuron_count}")  # Read-only property

brain.development_rate = 0.5  # Validated property setter
```

#### Iterator Support
```python
# Iterate over brain regions
for region in brain.regions:
    print(f"Region: {region.name}, Neurons: {region.neuron_count}")

# Iterate over active neurons
for neuron in brain.active_neurons:
    print(f"Neuron {neuron.id} firing at {neuron.activity} Hz")
```

#### Error Handling with Custom Exceptions
```python
import pynlm

try:
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Parameter validation example
    if not 100 <= config.neuron_count <= 10000:
        raise pynlm.ConfigurationError("Invalid neuron count")
        
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### 3. Advanced Features

#### Callback System
```python
# Callback for monitoring brain activity
class BrainMonitor:
    def __init__(self):
        self.spike_count = 0
        self.firing_rates = []
    
    def on_spike(self, neuron_id, region_name):
        self.spike_count += 1
        print(f"Spike in neuron {neuron_id} ({region_name})")
    
    def on_simulation_step(self, step, brain):
        self.firing_rates.append(brain.get_average_firing_rate())
        if step % 1000 == 0:
            print(f"Step {step}, Avg firing rate: {brain.get_average_firing_rate():.2f} Hz")

# Register callbacks
monitor = BrainMonitor()
brain.register_spike_callback(monitor.on_spike)
brain.register_step_callback(monitor.on_simulation_step)
```

#### Visualization Integration
```python
import pynlm
import matplotlib.pyplot as plt

# Create brain with visualization
brain = pynlm.create_brain(config)
brain.initialize()

# Visualize neural activity
with pynlm.VisualizationContext(brain) as viz:
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            activity = viz.get_neural_activity()
            plt.plot(activity)
            plt.title(f"Neural Activity - Step {step}")
            plt.show()
```

#### Performance Monitoring
```python
import pynlm
import time
import psutil

# Create brain with performance monitoring
brain = pynlm.create_brain(config)
brain.initialize()

# Performance monitoring
def profile_simulation(brain, steps):
    start_time = time.time()
    process = psutil.Process()
    
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    for step in range(steps):
        brain.step(step)
        
        # Log performance every 1000 steps
        if step % 1000 == 0:
            current_memory = process.memory_info().rss / 1024 / 1024
            memory_usage = current_memory - initial_memory
            
            print(f"Step {step}:")
            print(f"  Time: {time.time() - start_time:.2f}s")
            print(f"  Memory: {memory_usage:.2f}MB")
            print(f"  Firing rate: {brain.get_average_firing_rate():.2f} Hz")
    
    end_time = time.time()
    return {
        'total_time': end_time - start_time,
        'memory_usage': process.memory_info().rss / 1024 / 1024 - initial_memory,
        'avg_firing_rate': brain.get_average_firing_rate()
    }

# Run profiled simulation
results = profile_simulation(brain, 10000)
print(f"Simulation complete: {results}")
```

### 4. API Design Principles

#### Principle 1: Pythonic Naming
- Use snake_case for Python methods and attributes
- Use camelCase only for compatibility with C++ core
- Provide both direct and chainable interfaces

#### Principle 2: Explicit Over Implicit
- Require explicit configuration where possible
- Provide clear error messages for invalid parameters
- Make side effects explicit

#### Principle 3: Simple Is Better Than Complex
- Keep most common operations simple
- Hide complexity in advanced functions
- Provide multiple levels of abstraction

#### Principle 4: Flat Is Better Than Nested
- Reduce deeply nested object hierarchies
- Use composition over inheritance where appropriate
- Provide direct access to commonly used functionality

### 5. Implementation Details

#### Type Conversion
- Automatic conversion between Python and C++ types
- Numpy array support for large datasets
- JSON serialization for configuration

#### Memory Management
- Automatic garbage collection
- Reference counting for C++ objects
- Memory pooling for performance

#### Thread Safety
- Thread-safe Python bindings
- Internal locking mechanisms
- Parallel processing support

### 6. Migration Guide

#### From Old to New API

```python
# OLD: Complex C++ style
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)

# NEW: Pythonic style
import pynlm

with pynlm.BrainContext() as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)

# NEW: Advanced features
import pynlm

brain = pynlm.BrainBuilder() \
    .with_neuron_count(1000) \
    .with_development(True) \
    .with_reward_modulation(True) \
    .build()

brain.initialize()
brain.simulate(100, callbacks=my_callbacks)
```

### 7. Testing Strategy

#### Unit Tests
- Individual component testing
- Mock object testing
- Parameter validation testing

#### Integration Tests
- Subsystem interaction testing
- End-to-end simulation testing
- Performance regression testing

#### Property-Based Tests
- Random input testing
- Edge case testing
- Stress testing

#### Performance Tests
- Benchmark testing
- Memory usage testing
- Throughput testing

### 8. Documentation Strategy

#### API Documentation
- Comprehensive docstrings with examples
- Parameter descriptions and types
- Return value documentation
- Exception documentation

#### User Guide
- Installation and setup guide
- Tutorial for beginners
- Advanced usage guide
- Performance optimization guide

#### Reference Manual
- Complete API reference
- Configuration options
- System architecture
- Troubleshooting guide

### 9. Development Workflow

#### Continuous Integration
- Automated testing on each commit
- Performance regression detection
- Documentation validation
- API compatibility checking

#### Code Quality
- Automated style checking
- Type checking
- Documentation generation
- Performance profiling

## Benefits of the New Architecture

1. **Better Maintainability**: Modular structure makes code easier to understand and modify
2. **Improved Usability**: Pythonic interfaces make the API more intuitive
3. **Enhanced Performance**: Optimized for common use cases
4. **Better Testing**: Comprehensive testing suite ensures reliability
5. **Professional Documentation**: Complete documentation for all features
6. **Future Extensibility**: Plugin system for easy extension

## Implementation Timeline

### Phase 1: Core Structure (2 weeks)
- Create modular package structure
- Implement basic Pythonic interfaces
- Add context managers and properties

### Phase 2: Advanced Features (3 weeks)
- Implement callback system
- Add visualization integration
- Add performance monitoring

### Phase 3: Testing and Documentation (4 weeks)
- Create comprehensive test suite
- Generate API documentation
- Write user guides

### Phase 4: Refinement and Optimization (2 weeks)
- Optimize performance
- Refine user experience
- Final bug fixes

This improved architecture will transform the NLM Python bindings from a basic C++ binding into a modern, professional Python API that provides excellent user experience, maintainability, and extensibility.