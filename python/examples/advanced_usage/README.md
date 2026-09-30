# Advanced Usage Examples - README

These examples demonstrate advanced NLM functionality using all enhanced features.

## Overview

The advanced usage examples cover:

1. **Context Manager Patterns** - All three context managers with nested usage
2. **Utility Classes** - SimulationRunner for experiment management
3. **ExperimentRunner** - Parameter sweeps and structured experiments
4. **DataCollector and Analysis** - Comprehensive data collection and post-analysis
5. **Pythonic Interfaces** - EnhancedBrain and EnhancedConfig with all features
6. **Advanced Python Features** - Callbacks, observers, decorators, statistical utilities

## Key Features Demonstrated

- **Resource Management**: Context managers ensure proper cleanup
- **Experiment Automation**: SimulationRunner and ExperimentRunner automate common patterns
- **Data Collection**: DataCollector with callback support for real-time processing
- **Statistical Analysis**: Built-in statistical utilities and pattern detection
- **Pythonic Interfaces**: Dictionary-like access, method chaining, type-safe methods
- **Event-Driven Programming**: Callback system and observer pattern
- **Advanced Python**: Decorators, async support, statistical utilities

## How to Use

```bash
# Navigate to this directory
cd advanced_usage

# Run context manager examples
python example1_context_managers.py
python example2_simulation_runner.py
python example3_experiment_runner.py
python example4_data_collector.py
python example5_pythonic_interfaces.py
python example6_advanced_features.py
```

## Example Categories

### Context Managers
- `example1_context_managers.py` - All context manager patterns
- Features `simulation_step()`, `agent_episode()`, and `world_session()`
- Demonstrates nested context managers and cleanup guarantees

### Utility Classes
- `example2_simulation_runner.py` - SimulationRunner for experiment execution
- `example3_experiment_runner.py` - ExperimentRunner for parameter sweeps
- `example4_data_collector.py` - DataCollector and Analysis classes

### Pythonic Interfaces
- `example5_pythonic_interfaces.py` - EnhancedBrain and EnhancedConfig
- Demonstrates all Pythonic interface features
- Method chaining, dictionary-like access, type-safe methods

### Advanced Python Features
- `example6_advanced_features.py` - Callbacks, observers, decorators
- Event-driven programming and statistical utilities
- Observer pattern and callback system

## Dependencies

```python
import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    simulation_step, agent_episode, world_session,
    SimulationRunner, ExperimentRunner, DataCollector,
    EnhancedBrain, EnhancedConfig, CallbackManager,
    EventDrivenSimulation, decorator,
    mean, std, correlation, moving_average, detect_anomalies
)
```

All examples work with:
- The original `pynlm` module
- The enhanced `enhanced_bindings` module
- Both modules simultaneously for comparison

## Learning Path

1. Start with `example1_context_managers.py` to understand resource management
2. Work through `example2_simulation_runner.py` and `example3_experiment_runner.py` for experiment automation
3. Explore `example4_data_collector.py` for data collection and analysis
4. Study `example5_pythonic_interfaces.py` for Pythonic interface patterns
5. Finish with `example6_advanced_features.py` for advanced Python features

## Expected Outputs

Each example includes expected output in its source code. The outputs demonstrate:

- Context manager behavior and cleanup
- Experiment execution and results
- Data collection and statistical analysis
- Pythonic interface functionality
- Event-driven programming capabilities

## Key Takeaways

- **Context managers** ensure proper resource management and cleanup
- **Utility classes** automate common simulation patterns
- **Data collection** enables comprehensive analysis
- **Pythonic interfaces** provide intuitive, Pythonic APIs
- **Event-driven programming** enables flexible simulation control
- **Statistical utilities** provide powerful data analysis tools
