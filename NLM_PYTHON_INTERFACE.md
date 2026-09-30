/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d36b4516-9598-423b-a111-045888d2ef5f/NLM_PYTHON_INTERFACE.md
Name: NLM Python Interface Documentation
Summary: Comprehensive documentation for NLM Python interface

# NLM Python Interface - Comprehensive Documentation

## Overview

The NLM (Neural Learning Machine) Python Interface provides a comprehensive set of tools for runtime control of the NLM brain system. This interface enables advanced users to create complex interactive experiments, develop custom neural architectures, and integrate the brain system into larger Python-based applications.

This document provides a complete overview of the NLM Python interface, including all available modules, features, and usage examples.

## Table of Contents

1. [Introduction](#introduction)
2. [Core Interface Modules](#core-interface-modules)
   - [Interface Module](#interface-module)
   - [Brain Control Module](#brain-control-module)
   - [Simulation Module](#simulation-module)
   - [Visualization Module](#visualization-module)
   - [Dashboard Module](#dashboard-module)
   - [Plugin System Module](#plugin-system-module)
   - [REPL Module](#repl-module)
   - [Async Control Module](#async-control-module)
   - [Event System Module](#event-system-module)
   - [Integration Module](#integration-module)
   - [Safety Module](#safety-module)
3. [Key Features](#key-features)
   - [Core Python Bindings](#core-python-bindings)
   - [Advanced Scripting Capabilities](#advanced-scripting-capabilities)
   - [Interactive Features](#interactive-features)
   - [Scripting Extensions](#scripting-extensions)
   - [Integration Features](#integration-features)
   - [Safety and Security](#safety-and-security)
4. [Usage Examples](#usage-examples)
   - [Basic Usage](#basic-usage)
   - [Advanced Usage](#advanced-usage)
   - [Plugin System](#plugin-system)
   - [Interactive REPL](#interactive-repl)
5. [API Reference](#api-reference)
   - [Main Interface Class](#main-interface-class)
   - [Brain Controller Class](#brain-controller-class)
   - [Simulation Controller Class](#simulation-controller-class)
   - [Event System Classes](#event-system-classes)
   - [Plugin System Classes](#plugin-system-classes)
   - [Safety Classes](#safety-classes)
6. [Configuration](#configuration)
7. [Installation](#installation)
8. [Testing](#testing)
9. [Troubleshooting](#troubleshooting)
10. [Future Enhancements](#future-enhancements)

## Introduction

The NLM Python interface is designed to provide comprehensive control over the NLM brain system from Python. It includes:

- **Core Python API**: Complete API for brain control and monitoring
- **Event-driven programming**: Callback system for reactive programming
- **Asynchronous operations**: Non-blocking control for complex workflows
- **Interactive features**: REPL and visualization tools
- **Plugin system**: Extensible architecture for custom modules
- **Integration**: Support for popular Python libraries
- **Safety**: Sandboxed execution and access control
- **Monitoring**: Resource monitoring and alerting

The interface is built on top of the existing C++ NLM bindings and provides a high-level Python API for easy interaction with the brain system.

## Core Interface Modules

### Interface Module (`pynlm/interface.py`)

The main interface module provides the core functionality for NLM brain control:

- **ScriptingInterface**: Main interface class for brain control
- **ScriptingContext**: Context for maintaining state across operations
- **ScriptingEvent**: Event data structure for event-driven programming
- **EventType**: Enumeration of event types
- **ScriptingMode**: Enumeration of scripting modes

**Key Classes:**

- `ScriptingInterface`: Main interface for brain control
- `ScriptingContext`: Context management
- `ScriptingEvent`: Event handling

### Brain Control Module (`pynlm/brain_control.py`)

Provides advanced brain control and monitoring capabilities:

- **BrainController**: Advanced brain controller with real-time monitoring
- **ConfigManager**: Configuration management for NLM system

**Key Classes:**

- `BrainController`: Real-time brain state monitoring and control
- `ConfigManager`: Configuration management

### Simulation Module (`pynlm/simulation.py`)

Provides comprehensive simulation control:

- **SimulationController**: Advanced simulation controller
- **StepController**: Step-by-step simulation control
- **SimulationExporter**: Export simulation data

**Key Classes:**

- `SimulationController`: Comprehensive simulation control
- `StepController`: Step-by-step simulation
- `SimulationExporter`: Data export functionality

### Visualization Module (`pynlm/visualization.py`)

Provides visualization and plotting capabilities:

- **PlotManager**: Manage plotting operations
- **InteractivePlot**: Interactive plotting with events
- **VisualizationDashboard**: Comprehensive visualization dashboard
- **ConsoleDashboard**: Console-based dashboard
- **WebDashboard**: Web-based dashboard

**Key Classes:**

- `PlotManager`: Plotting operations
- `InteractivePlot`: Interactive plotting
- `VisualizationDashboard`: Dashboard management
- `ConsoleDashboard`: Console dashboard
- `WebDashboard`: Web dashboard

### Dashboard Module (`pynlm/dashboard.py`)

Provides real-time monitoring with customizable dashboards:

- **Dashboard**: Main dashboard controller
- **DashboardWidget**: Base class for dashboard widgets
- **MetricsWidget**: Widget for displaying key metrics
- **PlotWidget**: Widget for displaying plots
- **TableWidget**: Widget for displaying tabular data
- **GaugeWidget**: Widget for displaying gauge indicators
- **HistoricalWidget**: Widget for displaying historical data
- **RealTimeWidget**: Widget for displaying real-time data
- **AlarmWidget**: Widget for displaying alarms
- **LogWidget**: Widget for displaying log messages

**Key Classes:**

- `Dashboard`: Dashboard controller
- `DashboardWidget`: Base widget class
- Various widget types for different data displays

### Plugin System Module (`pynlm/plugin_system.py`)

Provides comprehensive plugin system for custom Python modules:

- **PluginType**: Plugin type enumeration
- **PluginStatus**: Plugin status enumeration
- **PluginInfo**: Plugin metadata
- **Plugin**: Base class for all NLM plugins
- **PluginManager**: Plugin manager for loading and managing plugins

**Key Classes:**

- `PluginType`: Plugin type enumeration
- `PluginStatus`: Plugin status enumeration
- `PluginInfo`: Plugin metadata
- `Plugin`: Base plugin class
- `PluginManager`: Plugin manager
- Various plugin implementations (BrainExtensionPlugin, NeuralNetworkPlugin, etc.)

### REPL Module (`pynlm/repl.py`)

Provides interactive REPL (Read-Eval-Print Loop):

- **REPLMode**: REPL mode enumeration
- **CommandInfo**: Command metadata
- **REPL**: Interactive REPL for NLM brain control

**Key Classes:**

- `REPLMode`: REPL mode enumeration
- `CommandInfo`: Command metadata
- `REPL`: Interactive REPL implementation

### Async Control Module (`pynlm/async_control.py`)

Provides asynchronous operations for non-blocking control:

- **AsyncMode**: Async operation mode enumeration
- **AsyncTask**: Asynchronous task definition
- **AsyncTaskManager**: Manager for asynchronous operations
- **AsyncBrainController**: Asynchronous brain controller
- **AsyncSimulationController**: Asynchronous simulation controller
- **AsyncSimulation**: Asynchronous simulation runner

**Key Classes:**

- `AsyncMode`: Async operation mode enumeration
- `AsyncTask`: Asynchronous task
- `AsyncTaskManager`: Task manager
- `AsyncBrainController`: Async brain controller
- `AsyncSimulationController`: Async simulation controller
- `AsyncSimulation`: Async simulation runner

### Event System Module (`pynlm/event_system.py`)

Provides event-driven programming with callbacks:

- **EventFilter**: Event filter for filtering events
- **EventEmitter**: Event emitter for emitting events
- **EventBus**: Event bus for managing event emitters
- **EventDrivenInterface**: Event-driven interface for NLM brain control

**Key Classes:**

- `EventFilter`: Event filtering
- `EventEmitter`: Event emission
- `EventBus`: Event bus
- `EventDrivenInterface`: Event-driven interface

### Integration Module (`pynlm/integration.py`)

Provides integration with popular Python libraries:

- **NumPyIntegration**: NumPy integration
- **PandasIntegration**: Pandas integration
- **MatplotlibIntegration**: Matplotlib integration

**Key Classes:**

- `NumPyIntegration`: NumPy integration
- `PandasIntegration`: Pandas integration
- `MatplotlibIntegration`: Matplotlib integration

### Safety Module (`pynlm/safety.py`)

Provides safety and security features:

- **AccessLevel**: Access level enumeration
- **SecurityLevel**: Security level enumeration
- **SecurityPolicy**: Security policy for NLM operations
- **Sandbox**: Sandbox environment for safe code execution
- **AccessControl**: Access control system
- **InputValidator**: Input validation for NLM operations
- **ResourceMonitor**: Resource monitor for NLM system

**Key Classes:**

- `AccessLevel`: Access level enumeration
- `SecurityLevel`: Security level enumeration
- `SecurityPolicy`: Security policy
- `Sandbox`: Sandbox environment
- `AccessControl`: Access control
- `InputValidator`: Input validation
- `ResourceMonitor`: Resource monitoring

## Key Features

### Core Python Bindings

The core Python bindings provide complete API for brain control and monitoring:

- **Real-time brain state access**: Get current brain state, neuron activity, synaptic connections
- **Brain manipulation**: Inject stimuli, control neurons, monitor activity
- **Configuration management**: Load, save, and modify brain configuration
- **Event-driven programming**: Callback system for reactive programming
- **Asynchronous operations**: Non-blocking control for complex workflows

### Advanced Scripting Capabilities

The advanced scripting capabilities enable complex neural system control:

- **Brain simulation control**: Step, pause, reset, save/load operations
- **Neural network manipulation**: Inject stimuli, monitor neurons
- **Memory system access**: Read and write memory state
- **Neuromodulation system control**: Control neuromodulation effects
- **Plasticity rule management**: Configure learning rules and synaptic plasticity

### Interactive Features

The interactive features provide rich user experience:

- **Live REPL**: Interactive Read-Eval-Print Loop for exploration
- **Visualization and plotting**: Comprehensive plotting capabilities
- **Real-time monitoring**: Customizable dashboards
- **Command-line interface**: Scriptable control via command line

### Scripting Extensions

The scripting extensions provide extensibility:

- **Plugin system**: Load custom Python modules
- **Custom neural network topologies**: Define custom network architectures
- **Custom learning rules**: Implement custom plasticity models
- **Custom neuromodulation effects**: Create custom neuromodulation
- **Custom sensory and motor processing**: Define custom input/output

### Integration Features

The integration features enable seamless integration with other Python ecosystems:

- **NumPy integration**: Export brain data to NumPy arrays
- **Matplotlib integration**: Create comprehensive visualizations
- **Pandas integration**: Analyze brain data with Pandas
- **Jupyter notebook support**: Interactive computing environments
- **Database integration**: Persistent brain state storage
- **Cloud connectivity**: Distributed computing capabilities

### Safety and Security

The safety and security features ensure safe operation:

- **Sandboxed execution**: Safe code execution in isolated environment
- **Access control**: Permission-based access control
- **Input validation**: Validate all inputs before processing
- **Error handling**: Comprehensive error handling and reporting
- **Resource limits**: Resource monitoring and enforcement

## Usage Examples

### Basic Usage

This example demonstrates basic NLM Python interface usage:

```python
import pynlm

# Initialize system
interface = pynlm.initialize_nlm()

# Create brain
brain = interface.create_brain()
brain.initialize()

# Create world
world = interface.create_world()

# Run simulation
interface.run_loop(duration=10, dt=0.1)

# Inject stimulus
interface.inject_stimulus("current", {"neuron_id": 0, "current": 1.0})

# Save state
interface.save_brain_state("brain_state.json")

# Clean up
interface.reset_system()
```

### Advanced Usage

This example demonstrates advanced NLM Python interface usage:

```python
import pynlm
import asyncio

async def advanced_example():
    # Initialize system
    interface = pynlm.initialize_nlm()
    
    # Create custom configuration
    config = pynlm.Config()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synaptic_density", 0.2)
    
    # Create brain with configuration
    brain = pynlm.Brain(config)
    brain.initialize()
    interface.set_brain(brain)
    
    # Setup event monitoring
    def event_handler(event_type, data):
        print(f"Event: {event_type}")
    
    interface.add_event_handler(event_handler)
    
    # Run complex experiment
    await interface.run_loop(duration=5, dt=0.01)
    
    # Analyze results
    state = interface.get_state()
    # ... analyze state ...
    
    # Clean up
    interface.reset_system()

# Run example
asyncio.run(advanced_example())
```

### Plugin System

This example demonstrates plugin system usage:

```python
import pynlm

class CustomPlugin(pynlm.Plugin):
    def _initialize_plugin(self):
        self.register_event_handler(pynlm.EventType.STATE_CHANGED, self._on_state_change)
    
    def _enable_plugin(self):
        print(f"Plugin enabled: {self.info.name}")
    
    def _on_state_change(self, event):
        print(f"State changed: {event.event_type}")

# Create plugin
plugin = CustomPlugin(
    name='custom_plugin',
    version='1.0.0',
    description='Custom plugin',
    author='Author',
    plugin_type=pynlm.PluginType.UTILITY
)

# Use plugin
plugin.initialize(interface)
plugin.enable()
```

### Interactive REPL

This example demonstrates REPL usage:

```python
import pynlm

# Initialize system
interface = pynlm.initialize_nlm()

# Create REPL
repl = pynlm.REPL(interface)

# Start REPL
repl.start()
```

## API Reference

### Main Interface Class

The `ScriptingInterface` class provides the main API for NLM brain control:

- `initialize_system(config_file=None, world_params=None)`: Initialize NLM system
- `create_brain(config_file=None)`: Create brain instance
- `create_world(width=100, height=100, vision_width=32, vision_height=32)`: Create world
- `run_step(dt=1.0, auto=True)`: Run a single simulation step
- `run_loop(duration, dt=1.0, auto=True)`: Run simulation loop
- `inject_stimulus(stimulus_type, parameters)`: Inject stimulus
- `get_state()`: Get brain state
- `save_brain_state(filepath, include_plasticities=True)`: Save brain state
- `load_brain_state(filepath)`: Load brain state
- `set_mode(mode)`: Set scripting mode
- `reset_system()`: Reset system

### Brain Controller Class

The `BrainController` class provides advanced brain control:

- `start_monitoring(interval=1.0, callbacks=None)`: Start brain monitoring
- `stop_monitoring()`: Stop brain monitoring
- `subscribe_to_neuron(neuron_id, callback, event_types=None)`: Subscribe to neuron events
- `subscribe_to_region(region_id, callback, event_types=None)`: Subscribe to region events
- `inject_stimulus_advanced(stimulus)`: Advanced stimulus injection
- `get_brain_statistics()`: Get detailed brain statistics
- `get_synaptic_activity()`: Get synaptic activity statistics

### Simulation Controller Class

The `SimulationController` class provides simulation control:

- `start_simulation(duration=None, dt=1.0, auto=True)`: Start simulation
- `pause_simulation(reason=None)`: Pause simulation
- `resume_simulation()`: Resume simulation
- `step_simulation(dt=1.0)`: Take a single step
- `reset_simulation(keep_config=True)`: Reset simulation
- `save_checkpoint(name, metadata=None)`: Save checkpoint
- `load_checkpoint(name)`: Load checkpoint
- `start_recording()`: Start recording
- `stop_recording()`: Stop recording

### Event System Classes

The event system classes provide event-driven programming:

- `EventFilter`: Filter events based on criteria
- `EventEmitter`: Emit events to handlers
- `EventBus`: Manage event emitters and routing
- `EventDrivenInterface`: Event-driven interface for NLM

### Plugin System Classes

The plugin system classes provide plugin management:

- `PluginType`: Plugin type enumeration
- `PluginStatus`: Plugin status enumeration
- `PluginInfo`: Plugin metadata
- `Plugin`: Base plugin class
- `PluginManager`: Plugin manager

### Safety Classes

The safety classes provide security and validation:

- `AccessLevel`: Access level enumeration
- `SecurityLevel`: Security level enumeration
- `SecurityPolicy`: Security policy
- `Sandbox`: Sandbox environment
- `AccessControl`: Access control
- `InputValidator`: Input validation
- `ResourceMonitor`: Resource monitoring

## Configuration

The NLM Python interface supports various configuration options:

### Default Configuration

```python
config = {
    "simulation": {
        "dt": 1.0,
        "duration": 10.0,
        "auto": True
    },
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "learning_rate": 0.1
    },
    "visualization": {
        "backend": "matplotlib",
        "theme": "default"
    }
}
```

### Plugin Configuration

```python
plugin_config = {
    "custom_plugin": {
        "name": "Custom Plugin",
        "version": "1.0.0",
        "description": "Custom plugin for NLM",
        "author": "Author",
        "type": "utility",
        "enabled": True,
        "config": {
            "setting1": "value1",
            "setting2": "value2"
        }
    }
}
```

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler (for building C++ extensions)
- pybind11 (for Python bindings)

### Installation via pip

```bash
pip install pynlm
```

### Building from source

```bash
# Clone the repository
# git clone <repository_url>
# cd nlm

# Install Python package
pip install -e python/
```

### Running tests

```bash
cd python/
pytest tests/
```

## Testing

The NLM Python interface includes comprehensive tests:

### Unit Tests

- `tests/test_interface.py`: Interface tests
- `tests/test_brain_controller.py`: Brain controller tests
- `tests/test_simulation.py`: Simulation tests
- `tests/test_visualization.py`: Visualization tests
- `tests/test_plugin_system.py`: Plugin system tests
- `tests/test_event_system.py`: Event system tests
- `tests/test_integration.py`: Integration tests
- `tests/test_safety.py`: Safety tests

### Integration Tests

- Integration tests for all major components
- End-to-end tests for complex workflows
- Performance tests for critical paths
- Security tests for safety features

### Example Tests

- `examples/test_basic_usage.py`: Basic usage example
- `examples/test_advanced_usage.py`: Advanced usage example
- `examples/test_plugin_example.py`: Plugin system example
- `examples/test_interactive_repl.py`: REPL example

## Troubleshooting

### Common Issues

1. **Import Errors**
   - Check if pynlm is installed correctly
   - Verify all dependencies are installed
   - Check Python version compatibility

2. **Simulation Errors**
   - Check system configuration
   - Verify brain initialization
   - Monitor system resources

3. **Plugin Errors**
   - Check plugin dependencies
   - Verify plugin configuration
   - Review plugin code

4. **Visualization Errors**
   - Check matplotlib installation
   - Verify available backends
   - Check data format

### Debug Commands

```bash
# Enable debug mode
python -m pynlm.interface --debug

# Check logs
python -m pynlm.interface --log-level debug

# Profile performance
python -m pynlm.interface --profile
```

## Future Enhancements

### Planned Features

1. **Enhanced Plugin System**
   - Plugin marketplaces
   - Plugin version management
   - Plugin dependency resolution

2. **Advanced Visualization**
   - 3D neural visualizations
   - Interactive network graphs
   - Custom plot templates

3. **Machine Learning Integration**
   - Neural network training
   - Model serving
   - Automated experiments

4. **Cloud Integration**
   - Distributed computing
   - Cloud storage
   - Collaboration features

5. **Advanced Safety**
   - Formal verification
   - Penetration testing
   - Security audits

### Research Directions

1. **Neuromorphic Computing**
   - Spike-based neural networks
   - Brain-inspired algorithms
   - Neuromorphic hardware integration

2. **Cognitive Modeling**
   - Learning and memory
   - Decision making
   - Cognitive architectures

3. **Systems Biology**
   - Neural circuit modeling
   - Brain connectivity
   - Computational neuroscience

## License

This project is licensed under the MIT License.

## Acknowledgements

The NLM Python interface is developed as part of the NLM (Neural Learning Machine) project. We acknowledge the contributions of all developers and researchers involved in this project.

## Contact

For questions, issues, or feedback, please contact:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://docs.nlm.org
- Community Forum: https://community.nlm.org

---

*This documentation was generated automatically by the NLM Python interface.*
