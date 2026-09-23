# NLM Python Bindings API Reference

This document provides comprehensive API documentation for the enhanced NLM Python bindings.

## Overview

The enhanced NLM Python bindings provide a modern, Pythonic interface to the NLM (Neural Learning Machine) C++ neural simulation framework. This API reference covers all classes, functions, and methods available in the `pynlm` module.

## Module: pynlm

### Core Classes

#### AdvancedBrain
A high-level interface for NLM neural simulation with intelligent defaults and comprehensive simulation control.

**Constructor**
```python
AdvancedBrain(config: Optional[Config] = None, enable_optimization: bool = True, verbose: bool = False)
```

**Properties**
- `config`: The configuration object used for simulation setup
- `verbose`: Whether verbose logging is enabled
- `monitoring`: The monitoring system instance
- `visualization`: The visualization integration instance

**Methods**

##### run_episode()
```python
run_episode(max_steps: Optional[int] = None, render: bool = False, 
           verbose: bool = None, callbacks: Optional[Dict[str, Callable]] = None) -> Dict[str, Any]
```

Run a complete simulation episode with advanced controls and monitoring.

**Parameters**
- `max_steps`: Maximum number of simulation steps (uses configured default if None)
- `render`: If True, render the simulation state for visualization
- `verbose`: If True, provide detailed progress output (uses instance verbose if None)
- `callbacks`: Dictionary of callback functions for simulation events

**Returns**
Dictionary containing episode results with keys:
- `steps_completed`: Number of steps actually executed
- `total_reward`: Sum of rewards received during episode
- `success`: Boolean indicating if episode completed successfully
- `diagnostics`: Dictionary of performance and state metrics
- `history`: List of step-wise state snapshots (if verbose=True)
- `metrics`: Performance metrics collected during simulation

##### configure_simulation()
```python
configure_simulation(**kwargs: Any) -> 'AdvancedBrain'
```

Configure simulation parameters with validation.

**Parameters**
- `**kwargs`: Simulation parameters to configure

**Returns**
Self for method chaining

##### run_episodes()
```python
run_episodes(num_episodes: int, max_steps_per_episode: int = 1000) -> List[Dict[str, Any]]
```

Run multiple episodes with aggregated results.

**Parameters**
- `num_episodes`: Number of episodes to run
- `max_steps_per_episode`: Maximum steps per episode

**Returns**
List of results dictionaries for each episode

##### start_simulation()/pause_simulation()/resume_simulation()/stop_simulation()
Control simulation execution state.

##### run_continuous()
```python
run_continuous(max_steps: int = 0, duration: float = 0)
```

Run continuous simulation with optional limits.

**Parameters**
- `max_steps`: Maximum number of steps (0 for unlimited)
- `duration`: Maximum duration in seconds (0 for unlimited)

##### get_diagnostics()
```python
get_diagnostics() -> Dict[str, Any]
```

Get comprehensive system diagnostics including simulation state, performance metrics, configuration, and custom metrics.

##### reset_diagnostics()
Reset diagnostic counters and monitoring system.

##### add_callback()/remove_callback()
Add or remove callback functions for simulation events.

##### Context Managers

###### simulation_context()
```python
@contextmanager
def simulation_context(self, duration: float = 0):
```

Context manager for simulation with automatic cleanup.

**Parameters**
- `duration`: Maximum duration for the simulation (0 for unlimited)

#### AgentBrain
Complete agent-environment interface for intelligent agents.

**Constructor**
```python
AgentBrain(brain: AdvancedBrain)
```

**Methods**

##### process_sensory_input()
```python
process_sensory_input(percept: SensoryPercept)
```

Process sensory percept and inject into brain.

##### decode_motor_command()
```python
decode_motor_command() -> Action
```

Decode brain motor activity into motor command.

##### apply_reward_modulation()
```python
apply_reward_modulation(reward: float, predicted_reward: float)
```

Apply reward-based neuromodulation.

##### update_development()
```python
update_development(timestep: float)
```

Update development system.

##### get_developmental_stage()/get_neuromodulation_level()/get_curiosity_level()/get_novelty_level()/get_prediction_error()
Get various agent state metrics.

##### enable_reward_modulation()/enable_structural_plasticity()/enable_development()/enable_curiosity()
Enable or disable specific agent capabilities.

##### is_reward_modulation_enabled()/is_structural_plasticity_enabled()/is_development_enabled()/is_curiosity_enabled()
Check if specific agent capabilities are enabled.

#### World
Advanced world simulation with visualization.

**Constructor**
```python
World()
```

**Methods**

##### configure()
```python
configure(width: float, height: float, vision_width: float = 10.0, vision_height: float = 10.0)
```

Configure world dimensions and vision parameters.

##### update()
```python
update(timestep: float)
```

Update world state based on time step.

##### apply_motor_command()
```python
apply_motor_command(action: Action, current_time: float)
```

Apply motor command to world.

##### get_sensory_percept()/get_agent_body()
Get sensory percept or agent body state.

##### add_object()/remove_object()/is_valid_position()
Manage world objects and position validation.

#### Config
Enhanced configuration with validation and validation rules.

**Constructor**
```python
Config()
```

**Methods**

##### configure()
```python
configure(**kwargs: Any) -> 'Config'
```

Configure simulation parameters with validation and error handling.

##### validate_parameter()
```python
validate_parameter(name: str, value: Any) -> List[str]
```

Validate a parameter value against rules.

##### add_validation_rule()/add_description()
Add custom validation rules or parameter descriptions.

##### get()/has()/get_keys()/clear()/summary()
Basic configuration access methods.

#### MonitoringSystem
Advanced monitoring and diagnostics system.

**Constructor**
```python
MonitoringSystem()
```

**Methods**

##### collect_metrics()
```python
collect_metrics(brain: 'Brain')
```

Collect performance and state metrics from brain.

##### get_performance_stats()/get_system_status()
Get performance statistics and comprehensive system status.

##### generate_report()
```python
generate_report() -> str
```

Generate human-readable performance report.

#### VisualizationIntegration
Visualization integration for NLM simulations.

**Constructor**
```python
VisualizationIntegration()
```

**Methods**

##### setup_plotting()
```python
setup_plotting(backend: str = 'matplotlib') -> bool
```

Setup plotting backend ('matplotlib', 'plotly', 'bokeh').

##### create_brain_network_plot()/create_training_monitor()
Create visualization plots for brain network and training monitoring.

#### NLMTypes
Python type definitions for NLM entities.

Contains nested classes for IDs, enumerations, and data types.

#### WorldObject
World object with spatial and type information.

#### AgentBody
Agent body state including position, velocity, and attributes.

#### SensoryPercept
Sensory percept data from world sensors.

#### Action
Action representation for motor output.

### Utility Functions

#### create_default_config()
```python
create_default_config() -> Config
```

Create a default configuration.

#### create_brain()
```python
create_brain(config: Optional[Config] = None) -> AdvancedBrain
```

Create a new brain with configuration.

#### create_simple_world()
```python
create_simple_world() -> World
```

Create a new simple world.

#### create_agent_brain()
```python
create_agent_brain(brain: AdvancedBrain) -> AgentBrain
```

Create a new agent brain interface.

#### create_simulation()
```python
create_simulation()
```

Create a complete simulation with brain, world, and agent brain.

### Exception Classes

#### ValidationError
```python
class ValidationError(Exception):
    """Exception raised when configuration validation fails."""
```

#### NLMError
```python
class NLMError(Exception):
    """Base exception for NLM-specific errors."""
```

## Type Hints

All classes and functions include comprehensive type hints for better IDE support and static type checking.

## Module Variables

- `__version__`: Package version string
- `__author__`: Package author information
- `__description__`: Package description

## Dependencies

Required dependencies:
- Standard library: `asyncio`, `logging`, `time`, `contextlib`, `dataclasses`, `datetime`, `enum`, `typing`, `pathlib`

Optional dependencies for enhanced functionality:
- `matplotlib`: For visualization
- `plotly`: For interactive visualizations
- `bokeh`: For web-based visualizations
- `psutil`: For system monitoring

## Examples

Refer to the `examples/` directory for complete usage examples:

- `basic_usage.py`: Basic simulation usage
- `advanced_simulation.py`: Advanced simulation techniques
- `visualization.py`: Integration with visualization
- `configuration.py`: Configuration and customization
- `custom_agents.py`: Custom agent implementation

## Installation

```bash
pip install pynlm
```

The package includes the C++ NLM bindings as optional components for enhanced performance.

## License

MIT License. See LICENSE file for details.

## Copyright

© 2026 Enhanced NLM Python Bindings Team