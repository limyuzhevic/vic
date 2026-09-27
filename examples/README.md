# NLM Python Examples - Advanced Usage

This directory contains comprehensive examples for using the NLM (Neural Learning Machine) Python bindings.

## Overview

These examples demonstrate advanced usage patterns and provide building blocks for creating sophisticated neural simulations.

## Available Examples

### Core Examples

1. **basic_examples.py** - Fundamental usage patterns and getting started guide
2. **advanced_examples.py** - Advanced simulation techniques and analysis
3. **agent_behavior_examples.py** - Specialized agent behaviors and task types

### Utility Examples

4. **profiling_examples.py** - Performance profiling and optimization analysis
5. **session_management.py** - Save and load agent sessions
6. **batch_processing.py** - Run multiple simulations efficiently

## Quick Start

```bash
# Run basic examples
cd examples
python basic_examples.py

# Run advanced examples
python advanced_examples.py

# Run profiling examples
python profiling_examples.py
```

## Example Structure

Each example directory follows this pattern:

```
examples/
├── basic_examples.py           # Getting started with NLM
├── advanced_examples.py        # Advanced simulation techniques
├── agent_behavior_examples.py   # Specialized agent behaviors
├── profiling_examples.py       # Performance profiling
├── session_management.py       # Save/load functionality
├── batch_processing.py         # Multiple simulation runs
├── README.md                   # This file
└── utils/                      # Common helper functions
    ├── session_manager.py
    ├── profiler.py
    └── analysis_utils.py
```

## Advanced Features Demonstrated

### 1. Brain Development
- Multi-episode development experiments
- Developmental stage progression
- Plasticity and learning analysis

### 2. Agent Behavior
- Curiosity-driven exploration
- Task-specific agent configurations
- Memory system integration

### 3. Performance Analysis
- Detailed timing and profiling
- Memory usage tracking
- Optimization opportunities

### 4. Session Management
- Save/load agent states
- Experiment reproducibility
- Data persistence

### 5. Batch Processing
- Multiple simulation runs
- Statistical analysis
- Comparative studies

## Key Concepts

### Brain Configuration
```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
```

### Agent Creation
```python
# Create world with specific dimensions
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
```

### Simulation Loop
```python
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Example Output

Each example produces comprehensive output including:

- **Statistical Analysis**: Spike counts, firing rates, development stages
- **Behavioral Data**: Action distributions, curiosity levels, novelty detection
- **Performance Metrics**: Timing, memory usage, optimization insights
- **Visualization Data**: Position tracking, environmental interaction

## Running Examples

### Basic Examples
```bash
# Launch interactive demonstration
python examples/basic_examples.py

# Run specific example with arguments
python examples/basic_examples.py --example advanced_demos
```

### Advanced Examples
```bash
# Run comprehensive demo
python examples/advanced_examples.py

# Run with custom parameters
python examples/advanced_examples.py --episodes 10 --steps 500
```

### Profiling Examples
```bash
# Profile performance
python examples/profiling_examples.py

# Generate performance report
python examples/profiling_examples.py --generate_report
```

## File Format Reference

### JSON Session Format
```json
{
    "session_name": "nlm_session_2024",
    "timestamp": "2024-01-15T10:30:00",
    "brain_state": "brain_state.bin",
    "metadata": {
        "neuron_count": 1500,
        "development_stage": 3,
        "curiosity_level": 0.75
    }
}
```

### Profiling Output Format
```json
{
    "profiler": {
        "total_time": 125.3,
        "average_operation_time": 1.25,
        "memory_samples": 10,
        "peak_memory_mb": 245.6
    },
    "operations": {
        "world_update": {"total_time": 45.1, "count": 100},
        "brain_step": {"total_time": 32.7, "count": 100},
        "action_decode": {"total_time": 12.8, "count": 100}
    }
}
```

## Integration with Existing Code

### Importing Examples
```python
# Import example modules
from examples.basic_examples import basic_brain_setup
from examples.advanced_examples import run_development_experiment
from examples.agent_behavior_examples import create_explorer_agent

# Use helper functions
from examples.utils.session_manager import save_agent_session
from examples.utils.profiler import NLMProfiler
```

### Custom Example Creation
```python
# Create custom example extending existing functionality
from examples.advanced_examples import NLMAdvancedExample

class CustomExample(NLMAdvancedExample):
    def run_example(self):
        # Your custom implementation
        pass
```

## Common Issues and Solutions

### Issue 1: Performance Slowdown
**Solution**: Use profiling examples to identify bottlenecks
```bash
python examples/profiling_examples.py --debug
```

### Issue 2: Memory Usage
**Solution**: Implement session management and use periodic saves
```python
from examples.session_management import save_agent_session
session = save_agent_session(brain, world, agent, "my_session")
```

### Issue 3: Configuration Complexity
**Solution**: Use helper functions for specialized configurations
```python
from examples.agent_behavior_examples import create_survival_agent
agent = create_survival_agent(world_width=20, world_height=20)
```

## Best Practices

### 1. Session Management
- Always save agent states periodically during long simulations
- Use descriptive session names for experiment tracking
- Include metadata for easy session identification

### 2. Performance Monitoring
- Enable profiling for development and optimization
- Monitor memory usage during extended simulations
- Track operation timing for bottleneck identification

### 3. Error Handling
- Use validation helper functions for input checking
- Implement proper error handling for edge cases
- Use try-catch blocks for critical operations

### 4. Code Organization
- Create modular examples for different use cases
- Use helper functions to reduce code duplication
- Document complex examples with inline comments

## Future Enhancements

### Planned Features
1. **Distributed Computing**: Parallel simulation execution
2. **Visualization Integration**: Real-time plotting and analysis
3. **Machine Learning Integration**: Model training and inference
4. **Web Interface**: Interactive visualization and control

### Extension Points
- Add new agent behavior types
- Implement reinforcement learning integration
- Create specialized analysis tools
- Develop export capabilities for different formats

## Acknowledgments

This examples directory was developed as part of the NLM (Neural Learning Machine) Python bindings enhancement project.

## License

MIT License - See LICENSE file for details
