# Enhanced API Documentation for NLM

## Overview

This document provides comprehensive documentation for the Enhanced Python API for NLM (Neural Learning Machine). This enhanced API builds upon the basic bindings to provide a more intuitive, feature-rich interface for working with NLM neural simulations.

## Key Features

### 1. Enhanced Error Handling
- Custom exception hierarchy (`NLMError` → `ConfigurationError`, `SimulationError`, etc.)
- Detailed error messages with context and recovery suggestions
- Graceful error recovery mechanisms

### 2. Real-time Visualization and Plotting
- Multiple plotting backends (matplotlib, plotly, custom)
- Network graph visualization for neural connectivity
- Activity heatmaps and temporal visualizations
- Real-time animation capabilities

### 3. Advanced Simulation Control
- Pause/resume/step simulation control
- Time scaling (0.1x to 1000x speed)
- Event-driven callbacks for simulation events
- Performance monitoring and optimization controls

### 4. Model Export/Import
- Multiple export formats (JSON, binary, checkpoint)
- Model metadata and versioning support
- Automatic checkpointing with retention policies
- Model comparison and validation tools

### 5. Memory Management Utilities
- Automatic memory monitoring and cleanup
- Memory leak detection and reporting
- Performance profiling tools
- Resource usage optimization

### 6. Advanced Querying and Analysis
- Filter-based neuron queries by type, region, activity
- Pattern detection algorithms
- Connectivity analysis tools
- Statistical analysis and visualization

### 7. Context Management
- Safe context managers for resource handling
- Automatic cleanup on context exit
- Nested context support
- Thread-safe operations

## Core Classes

### 1. `BrainStats`

**Purpose:** Comprehensive statistics container with performance metrics and comparison capabilities

**Key Features:**
- Real-time metrics tracking
- Performance benchmarking
- Memory usage analysis
- Neural activity statistics

**Main Methods:**
```python
# Create brain statistics
stats = BrainStats(brain)

# Get comprehensive statistics
data = stats.get_all()

# Compare with another brain
diff = stats.compare_with(other_brain)

# Get performance score
score = stats.get_performance_score()
```

### 2. `SimulationControls`

**Purpose:** Advanced simulation control with pause/resume/step modes, callbacks, and time scaling

**Key Features:**
- Simulation state management (running, paused, stopped)
- Time control (speed scaling, time jumps)
- Event-driven callback system
- Performance monitoring

**Main Methods:**
```python
# Create simulation controls
controls = SimulationControls(brain, world, agent)

# Control simulation
controls.pause()
controls.resume()
controls.step()  # Single step
controls.set_speed(10.0)  # 10x speed

# Add callbacks
def on_step(step_number):
    print(f"Step: {step_number}")

controls.add_callback(Events.STEP, on_step)

# Get simulation state
state = controls.get_state()
```

### 3. `ModelManager`

**Purpose:** Full model export/import with multiple formats, versioning, and comparison

**Key Features:**
- Multiple export formats (JSON, binary, checkpoint)
- Version control and metadata management
- Automatic checkpointing with retention
- Model comparison and validation

**Main Methods:**
```python
# Create model manager
manager = ModelManager(brain)

# Export model
manager.save_model("my_brain.json", format="json")
manager.save_checkpoint("checkpoint_001.bin")

# Import model
brain2 = manager.load_model("my_brain.json")
brain3 = manager.load_checkpoint("checkpoint_001.bin")

# Model comparison
diff = manager.compare_models("model1.json", "model2.json")

# Version management
manager.set_version("2.0.0")
manager.add_metadata("author", "John Doe")
```

### 4. `VisualizationInterface`

**Purpose:** Real-time visualization with plotting, network graphs, and customizable callbacks

**Key Features:**
- Multiple plot types (network, activity, time series)
- Real-time updates with customizable refresh rates
- Interactive visualization with zoom/pan
- Multiple output formats (screen, file, web)

**Main Methods:**
```python
# Create visualization interface
vis = VisualizationInterface(brain)

# Enable and configure visualization
vis.enable()
vis.set_refresh_rate(30)  # Hz
vis.set_plot_type("network")  # or "activity", "time_series"

# Show visualization
vis.show()

# Get plot data
network_data = vis.get_network_data()
activity_data = vis.get_activity_data()

# Add custom callback
def on_plot_update(data):
    print(f"Plot updated with {len(data)} data points")

vis.add_callback(Events.PLOT_UPDATE, on_plot_update)
```

### 5. `MemoryManager`

**Purpose:** Memory monitoring, cleanup, and efficiency analysis with profiling capabilities

**Key Features:**
- Real-time memory usage monitoring
- Automatic garbage collection triggers
- Memory leak detection and reporting
- Performance optimization suggestions

**Main Methods:**
```python
# Create memory manager
mem_manager = MemoryManager()

# Start monitoring
mem_manager.start_monitoring()

# Get memory statistics
stats = mem_manager.get_stats()
print(f"Memory usage: {stats['memory_mb']:.2f} MB")

# Check for memory leaks
leaks = mem_manager.detect_leaks()
if leaks:
    print(f"Potential memory leaks detected: {leaks}")

# Force cleanup
mem_manager.force_cleanup()

# Set memory limits
mem_manager.set_memory_limit(500.0)  # 500 MB
```

### 6. `QueryBuilder`

**Purpose:** Advanced querying with filters, pattern detection, and connectivity analysis

**Key Features:**
- Powerful filtering system for neurons
- Pattern detection algorithms
- Connectivity analysis tools
- Result visualization

**Main Methods:**
```python
# Create query builder
query = QueryBuilder(brain)

# Build queries with fluent API
results = (query
    .filter_by_type(NeuronType.Excitatory)
    .filter_by_region("Cortex")
    .filter_by_activity(min_activity=0.5)
    .limit(100)
    .execute()
)

# Pattern detection
patterns = query.detect_patterns("burst", threshold=0.8)

# Connectivity analysis
connectivity = query.analyze_connectivity(neuron1, neuron2, distance=5)

# Save/restore queries
query.save("my_query.json")
query.load("saved_query.json")
```

### 7. `SimulationContext`

**Purpose:** Safe context manager for simulation resources

**Key Features:**
- Automatic resource cleanup
- Nested context support
- Thread-safe operations
- Exception safety

**Main Methods:**
```python
# Using simulation context manager
with SimulationContext(brain, world, agent) as sim:
    # Simulation runs in this context
    # Resources are automatically cleaned up
    sim.run_simulation(1000)

# Context manager with custom cleanup
def custom_cleanup(sim):
    sim.save_state("cleanup_state.bin")

with SimulationContext(brain, cleanup=custom_cleanup):
    # Custom cleanup logic
    pass
```

### 8. `BrainContext`

**Purpose:** Safe context manager for brain state management

**Key Features:**
- Brain state save/restore
- Transaction-like state management
- Automatic checkpointing
- Rollback capabilities

**Main Methods:**
```python
# Using brain context manager
with BrainContext(brain, auto_checkpoint=True) as brain_ctx:
    # Brain operations are atomic
    brain_ctx.initialize()
    brain_ctx.run_simulation(100)
    
    # State can be rolled back
    if something_goes_wrong:
        brain_ctx.rollback()

# Manual state management
brain_ctx = BrainContext(brain)
brain_ctx.save_state("important_state")
# Do operations
brain_ctx.rollback()  # Restore to saved state
```

## Custom Exceptions

### `NLMError`

Base exception for all NLM errors

**Attributes:**
- `message`: Error message
- `code`: Error code
- `details`: Additional error details

### `ConfigurationError`

Raised for configuration-related errors

### `SimulationError`

Raised for simulation-related errors

### `VisualizationError`

Raised for visualization-related errors

### `MemoryError`

Raised for memory-related errors

### `ValidationError`

Raised for validation-related errors

## Factory Functions

### `create_enhanced_brain(config=None)`

Create an enhanced brain with optional configuration

**Parameters:**
- `config`: Optional configuration object

**Returns:**
- Enhanced brain instance

### `create_enhanced_world(width, height, vision_width=16, vision_height=16)`

Create an enhanced world

**Parameters:**
- `width`, `height`: World dimensions
- `vision_width`, `height`: Vision dimensions

**Returns:**
- Enhanced world instance

### `create_enhanced_agent(brain, enable_features=True)`

Create an enhanced agent brain

**Parameters:**
- `brain`: Brain instance
- `enable_features`: Whether to enable advanced features

**Returns:**
- Enhanced agent instance

### `run_basic_simulation(config=None, steps=100)`

Run a basic simulation with optional configuration

**Parameters:**
- `config`: Optional simulation configuration
- `steps`: Number of simulation steps

**Returns:**
- Simulation results

### `run_agent_simulation(config=None, steps=1000)`

Run an agent-based simulation

**Parameters:**
- `config`: Optional simulation configuration
- `steps`: Number of simulation steps

**Returns:**
- Agent simulation results

### `run_enhanced_simulation(config=None, steps=1000)`

Run an enhanced simulation with all features

**Parameters:**
- `config`: Optional simulation configuration
- `steps`: Number of simulation steps

**Returns:**
- Enhanced simulation results

### `export_model_to_json(brain, filepath, include_metadata=True)`

Export brain model to JSON format

**Parameters:**
- `brain`: Brain instance
- `filepath`: Output file path
- `include_metadata`: Whether to include metadata

### `import_model_from_json(filepath)`

Import brain model from JSON format

**Parameters:**
- `filepath`: Input file path

**Returns:**
- Brain instance

### `validate_inputs(brain, world, agent)`

Validate simulation inputs

**Parameters:**
- `brain`: Brain instance
- `world`: World instance
- `agent`: Agent instance

**Returns:**
- Validation result (True/False) with details

### `get_version_info()`

Get version information for the enhanced API

**Returns:**
- Dictionary with version information

### `create_error_handler()`

Create an error handler for catching and processing NLM errors

**Returns:**
- Error handler instance

## Backward Compatibility

The enhanced API maintains full backward compatibility with the original bindings through:

### Original Factory Functions (Still Available)
- `createBrain(config)`
- `createSimpleWorld()`
- `createAgentBrain(brain)`
- `createDefaultConfig()`

### Original Enums (Still Available)
- `INVALID_NEURON_ID`
- `INVALID_SYNAPSE_ID`
- `INVALID_REGION_ID`
- `INVALID_POPULATION_ID`

### Original Type Classes (Still Available)
- `NeuronId`
- `SynapseId`
- `RegionId`
- `PopulationId`
- `NeuronType`
- `SynapseType`
- `DevelopmentalStage`
- `FiringState`
- `ActionType`
- `MotorCommand`
- `WorldObjectType`

## Usage Examples

### Basic Enhanced Usage
```python
import pynlm

# Create enhanced brain and related objects
brain = pynlm.create_enhanced_brain()
world = pynlm.create_enhanced_world(width=30, height=30)
agent = pynlm.create_enhanced_agent(brain, reward_modulation=True)

# Use simulation controls
controls = SimulationControls(brain, world, agent)
controls.pause()
controls.set_step_mode(True)

# Use visualization
vis = VisualizationInterface(brain)
vis.enable().plot_brain_activity(show=True)

# Run enhanced simulation
results = pynlm.run_enhanced_simulation(steps=1000)
```

### Advanced Usage
```python
import pynlm

# Create enhanced brain with configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.create_enhanced_brain(config)

# Set up visualization and analysis
vis = VisualizationInterface(brain)
query = QueryBuilder(brain)
mem_manager = MemoryManager()

# Create model manager for export/import
manager = ModelManager(brain)

# Run simulation with error handling
try:
    with SimulationContext(brain) as sim:
        sim.run_simulation(500)
        
        # Get statistics
        stats = BrainStats(brain)
        print(f"Simulation completed: {stats.get_performance_score()}")
        
        # Export model
        manager.save_model("simulation_result.json")
        
except NLMError as e:
    print(f"Simulation error: {e}")
```

### Machine Learning Workflow
```python
import pynlm

# Create enhanced brain for machine learning
brain = pynlm.create_enhanced_brain()
world = pynlm.create_enhanced_world(20, 20)
agent = pynlm.create_enhanced_agent(brain, enable_features=True)

# Set up visualization for monitoring
vis = VisualizationInterface(brain)
vis.set_plot_type("network")
vis.enable()

# Set up memory manager for optimization
mem_manager = MemoryManager()
mem_manager.start_monitoring()

# Create query builder for analysis
query = QueryBuilder(brain)

# Create simulation controls
controls = SimulationControls(brain, world, agent)

# Define custom callback for analysis
def analysis_callback(step_data):
    # Analyze step data
    firing_rate = step_data['firing_rate']
    if firing_rate > 0.8:
        print(f"High activity detected at step {step_data['step']}")

# Add callback
controls.add_callback(Events.STEP, analysis_callback)

# Run simulation with analysis
results = pynlm.run_enhanced_simulation(steps=1000)

# Export results
with BrainContext(brain) as ctx:
    manager = ModelManager(brain)
    manager.save_model("ml_results.json")
    
    # Get comprehensive statistics
    stats = BrainStats(brain)
    print(f"Training completed: {stats.get_performance_score():.2f}")
```

## Performance Considerations

### Memory Usage
- The enhanced API uses additional memory for caching and monitoring
- Monitor memory usage with `MemoryManager`
- Regular cleanup recommended for long-running simulations

### CPU Usage
- Advanced features may increase CPU usage
- Time scaling can help balance performance
- Visualization can be computationally intensive

### I/O Operations
- Export/import operations can be slow for large models
- Use checkpointing for frequent state saving
- Consider disk space requirements for large simulations

## System Requirements

### Minimum Requirements
- Python 3.8+
- matplotlib or plotly for visualization
- numpy for numerical operations
- pybind11 for C++ bindings

### Recommended Requirements
- 8+ CPU cores for parallel processing
- 16+ GB RAM for large simulations
- SSD storage for fast I/O operations
- Dedicated GPU for visualization (optional)

## Installation

### Basic Installation
```bash
pip install pynlm[enhanced]
```

### Development Installation
```bash
pip install -e .
```

### With All Dependencies
```bash
pip install pynlm[enhanced,all]
```

## Getting Help

### Documentation
- This documentation file
- Enhanced API API reference
- Tutorial and examples
- FAQ and troubleshooting guide

### Support
- GitHub repository issues
- Email support: contact@nlm-project.org
- Discord community: [link]

### Examples
- [examples/basic_usage.py]
- [examples/advanced_usage.py]
- [examples/machine_learning.py]
- [examples/visualization.py]

## Future Enhancements

### Planned Features
- Distributed computing support
- GPU acceleration
- Web-based visualization
- Advanced machine learning integration
- Real-time collaboration tools

### Plugin Architecture
The enhanced API includes a plugin architecture for extending functionality:
- [Plugin documentation]
- [Plugin API reference]
- [Plugin examples]

## License

This enhanced API is licensed under the MIT License. See the LICENSE file for details.

## Copyright

© 2026 NLM Project. All rights reserved.
