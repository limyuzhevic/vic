# NLM - Complete User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project in Phase 6: Final Integration. It creates a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**IMPORTANT**: NLM is NOT:
- A transformer or LLM
- A chatbot  
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start

### Phase 6 Demo (Integration Test)

1. **Build the system**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the integration test**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Use the Python interface** (if available)
   ```bash
   # From Python, import and use NLM
   import pynlm
   interface = pynlm.initialize_nlm()
   brain = interface.create_brain()
   interface.run_loop(duration=10, dt=0.1)
   ```

## System Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Core Components

### 1. Neural System
- **Real LIF neuron dynamics** with event-driven spike propagation
- **Structural plasticity** rules for connectivity changes
- **Neuromodulation** affecting excitability and plasticity (dopamine, curiosity, etc.)

### 2. Memory Systems
- **Working memory** with persistent activity and competition
- **Episodic memory** with experience encoding and replay
- **Associative memory** with Hebbian pattern associations

### 3. Prediction System
- Predictive coding integrated into brain loop
- Prediction error signals affecting learning
- Confidence tracking implemented

### 4. Cognition Systems
- **Neural planner** with action sequence evaluation
- **Concept formation** from experience patterns
- **Attention** with competitive selection dynamics

### 5. Development System
- Developmental stages affecting plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### 6. Environment Interface
- Sensory input processing
- Motor output generation
- World interaction loop

### 7. Plugin System
- **10 plugin categories**: Brain, Plasticity, Neuromodulation, Memory, Sensory, Motor, Development, Integration, Analysis, Utility
- Multi-language support (Python, C++, C#)
- Security sandboxing and dependency management

## Getting Started

### Prerequisites

- C++ compiler (for building C++ extensions)
- Python 3.8+ (for Python interface)
- CMake 3.10+ (for building)

### Building and Running

**For C++ Users:**
1. Build with CMake (as shown above)
2. Run `./nlm_phase6_demo` for integration test
3. Explore the C++ source in `src/` directory

**For Python Users:**
1. Install NLM Python interface
2. Use the comprehensive Python API for brain control
3. Leverage visualization and monitoring tools

### Initial Configuration

NLM comes with default configurations. You can customize:
- Neural network parameters
- Plasticity rules
- Neuromodulation effects
- Memory system properties

Configuration files are located in the `configs/` directory.

## Basic Usage

### C++ Basic Usage

```cpp
#include "nlm_interface.h"

int main() {
    NLMSystem system;
    system.initialize();
    
    // Create world
    World world(100, 100);
    
    // Create brain
    Brain brain;
    brain.initialize(system.get_config());
    
    // Run simulation loop
    system.run_loop(world, brain, 10.0, 0.1);
    
    system.shutdown();
    return 0;
}
```

### Python Basic Usage

```python
import pynlm

# Initialize NLM system
interface = pynlm.initialize_nlm()

# Create brain
brain = interface.create_brain()
brain.initialize()

# Create world
world = interface.create_world()

# Run simulation
interface.run_loop(duration=10, dt=0.1)

# Save state
interface.save_brain_state("brain_state.json")

# Reset
interface.reset_system()
```

## Interactive Exploration

### Command Line Interface

Use the built-in command-line interface for:
- System control (start, stop, reset)
- State inspection and modification
- Event monitoring
- Debugging

### Python REPL

The NLM Python interface includes an interactive REPL:

```python
import pynlm

# Initialize system
interface = pynlm.initialize_nlm()

# Start REPL
repl = pynlm.REPL(interface)
repl.start()
```

### Visualization Dashboard

The system includes comprehensive visualization tools:
- Real-time brain state monitoring
- Neural activity visualization
- Memory system displays
- Performance metrics

## Plugin System

The NLM plugin system provides:

### Plugin Categories
1. **Brain Plugins**: Custom neural architectures and dynamics
2. **Plasticity Plugins**: Custom learning rules
3. **Neuromodulation Plugins**: Custom neuromodulators
4. **Memory Plugins**: Custom memory systems
5. **Sensory Plugins**: Custom sensory processing
6. **Motor Plugins**: Custom action selection
7. **Development Plugins**: Custom developmental stages
8. **Integration Plugins**: External system interfaces
9. **Analysis Plugins**: Custom analysis tools
10. **Utility Plugins**: General-purpose utilities

### Plugin Development

The plugin system supports multiple languages and includes comprehensive development tools.

**Python Plugin Example:**
```python
class CustomPlugin(pynlm.Plugin):
    def _initialize_plugin(self):
        self.register_event_handler(
            pynlm.EventType.STATE_CHANGED, 
            self._on_state_change
        )
    
    def _enable_plugin(self):
        print(f"Plugin enabled: {self.info.name}")
    
    def _on_state_change(self, event):
        print(f"State changed: {event.event_type}")
```

## Monitoring and Debugging

### System Monitoring

The NLM system provides comprehensive monitoring:

**Python Interface:**
```python
# Start monitoring
plugin_manager = pynlm.initialize_plugin_system(interface)

# Monitor plugin performance
performance = plugin_manager.get_plugin_performance('custom_plugin')
print(f"Plugin performance: {performance}")

# Monitor system resources
resources = plugin_manager.get_system_resources()
print(f"System resources: {resources}")
```

**C++ Monitoring:**
```cpp
#include "nlm_monitor.h"

int main() {
    Monitor monitor;
    monitor.start();
    
    while (running) {
        auto stats = monitor.get_system_stats();
        monitor.display_stats(stats);
    }
    
    monitor.stop();
}
```

### Debugging Tools

1. **Event Logging**: Detailed event logging for system debugging
2. **State Inspection**: View and modify brain state
3. **Performance Profiling**: Identify performance bottlenecks
4. **Error Reporting**: Comprehensive error reporting and debugging

## Advanced Features

### Custom Neural Architectures

Create custom neuron types and network topologies through plugins:

```python
class CustomNeuron(BrainPlugin):
    def customize_neural_dynamics(self, brain, config):
        # Register custom neuron type
        self.register_neuron_type(CustomNeuronType())
        
    def modify_connectivity(self, brain, config):
        # Modify neural connectivity
        pass
```

### Multi-Modal Processing

Integrate multiple sensory modalities:

```python
class MultiModalPlugin(SensoryPlugin):
    def add_sensory_processor(self, name, processor, config=None):
        # Add multimodal sensory processors
        self.add_sensory_processor(name, processor, config)
```

### Developmental Learning

Implement custom developmental stages:

```python
class CustomDevelopmentPlugin(DevelopmentPlugin):
    def add_development_stage(self, stage_config):
        # Define custom developmental stage
        self.stages.append(CustomStage(stage_config))
```

## Performance Optimization

### Performance Tips

1. **Memory Management**: Use NLM's memory pooling system
2. **Parallel Processing**: Leverage SIMD and multi-core capabilities
3. **Event-Driven Architecture**: Use event-driven programming for efficiency
4. **Plugin Optimization**: Profile and optimize plugins

### Profiling Tools

The system includes profiling tools for:
- Performance analysis
- Memory usage monitoring
- Bottleneck identification
- Optimization recommendations

## System State Management

### Checkpoint System

Save and restore brain states:

```python
# Save brain state
interface.save_brain_state("checkpoint.json")

# Load brain state
interface.load_brain_state("checkpoint.json")

# Create named checkpoints
interface.save_checkpoint("experiment_1", {"epoch": 100, "accuracy": 0.95})
```

### State Persistence

Brain states can be persisted to:
- Files (JSON, binary formats)
- Databases (for long-term storage)
- Cloud storage (for distributed systems)
- Shared memory (for fast access)

## Extensibility

### Plugin Marketplace

The NLM plugin marketplace provides:
- Plugin discovery and search
- Plugin rating and review
- Plugin certification
- Plugin distribution

### Multi-Language Support

Create plugins in:
- **Python**: Full Python support with sandboxing
- **C++**: Native performance and direct integration
- **C#**: .NET ecosystem integration

## Community and Support

### Documentation

This is the complete user guide. For more detailed information:
- **API Reference**: See `NLM_API_REFERENCE.md`
- **Technical Guide**: See `NLM_TECHNICAL_GUIDE.md`
- **Development Guide**: See `NLM_DEVELOPMENT_GUIDE.md`
- **Tutorials**: See `NLM_TUTORIALS.md`

### Examples

Practical examples are available in the `examples/` directory:
- `basic_nlm_usage/` - Basic usage examples
- `advanced_config/` - Configuration examples
- `plugin_development/` - Plugin development examples
- `monitoring_visualization/` - Monitoring and visualization examples
- `python_scripting/` - Python scripting examples
- `benchmarking/` - Benchmarking examples
- `experiments/` - Experiment examples

### Community Resources

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Comprehensive online documentation
- **Forum**: Community discussion and support
- **Contributing**: Guidelines for contributing to NLM

## Troubleshooting

### Common Issues

**1. Build Errors**
- Ensure CMake version is compatible
- Check compiler versions
- Verify system dependencies

**2. Runtime Errors**
- Check system resources
- Verify configuration files
- Review logs for error details

**3. Plugin Errors**
- Check plugin permissions
- Verify plugin dependencies
- Review plugin security policies

**4. Performance Issues**
- Monitor system resources
- Profile plugin performance
- Optimize neural network configurations

### Getting Help

1. **Check Documentation**: Review the relevant documentation files
2. **Reproduce Issue**: Create a minimal reproduction case
3. **Check Logs**: Enable detailed logging
4. **Community Support**: Post on community forums
5. **Direct Contact**: Contact project maintainers for critical issues

## License

This project is licensed under the MIT License.

## Conclusion

NLM provides a comprehensive framework for creating artificial developmental brains. With its neural architecture, plugin system, and development tools, NLM enables researchers and developers to:

- Build complex neural systems
- Experiment with learning algorithms
- Create custom neural architectures
- Integrate with external systems
- Monitor and debug neural behavior
- Share and distribute plugins

The system is designed to be:
- **Flexible**: Support for custom architectures and learning rules
- **Extensible**: Plugin system for customization and extension
- **Performant**: Optimized for efficient neural simulation
- **Secure**: Sandboxed plugin execution and security controls
- **Observable**: Comprehensive monitoring and debugging tools

For questions, issues, or feedback, please visit the NLM project documentation and community resources.