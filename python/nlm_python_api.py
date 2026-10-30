# High-Level Python API for NLM

This module provides enhanced Python APIs for the NLM (Neural Learning Machine) framework.
It includes convenience classes for common neural simulation tasks and high-level abstractions.

## Installation

This module is part of the NLM Python bindings. After building the NLM project, you can import these enhanced APIs.

## Quick Start

```python
# Import the enhanced API
from nlm_python_api import HighLevelNLM, NeuralSimulation, ActionPlanner

# Create a simulation
nlm = HighLevelNLM()

# Run a simulation
for step in range(100):
    nlm.step()

print(f"Brain initialized: {nlm.get_brain().getTotalNeuronCount()} neurons")
```

## Core Classes

### HighLevelNLM
Comprehensive interface for NLM simulations with high-level abstractions.

```python
class HighLevelNLM(config=None, environment_config=None):
    """High-level NLM interface for simulation and control."""
```

**Parameters:**
- `config`: Optional configuration dictionary
- `environment_config`: Optional environment configuration

**Key Methods:**
- `initialize()`: Initialize the NLM system
- `reset()`: Reset to initial state
- `step()`: Perform one simulation step
- `get_perception()`: Get current sensory perception
- `choose_action(perception, goal=None)`: Choose action based on perception
- `execute_action(action)`: Execute selected action
- `get_brain()`: Get the underlying brain object
- `get_world()`: Get the underlying world object
- `run_simulation(max_steps=1000, visualize=False)`: Run complete simulation

**Properties:**
- `is_initialized`: Whether the system is initialized
- `current_step`: Current simulation step

### NeuralSimulation
High-level simulation interface with automatic state management.

```python
class NeuralSimulation(config=None, brain=None, world=None):
    """High-level neural simulation interface."""
```

**Key Methods:**
- `initialize()`: Initialize simulation
- `reset()`: Reset to initial state
- `step()`: Perform simulation step
- `step(action)`: Perform step with specific action
- `get_state()`: Get current simulation state
- `get_brain()`: Get brain object
- `get_world()`: Get world object
- `get_reward()`: Get current reward
- `get_done()`: Check if simulation is done

**Properties:**
- `total_reward`: Total accumulated reward
- `step_count`: Number of steps taken

### ActionPlanner
Intelligent action planner with exploration-exploitation balance.

```python
class ActionPlanner(brain=None, exploration_rate=0.1, learning_rate=0.01):
    """Action planner for neural control."""
```

**Key Methods:**
- `act(state)`: Choose action for given state
- `learn(state, action, reward, next_state, done)`: Learn from experience
- `reset()`: Reset planner
- `get_policy(state)`: Get policy for state
- `get_q_values(state)`: Get Q-values for state

**Properties:**
- `exploration_rate`: Current exploration rate
- `action_counts`: Statistics on action usage

### CuriosityDrivenExploration
Curiosity-driven exploration using intrinsic motivation.

```python
class CuriosityDrivenExploration(brain=None, learning_rate=0.01):
    """Curiosity-driven exploration system."""
```

**Key Methods:**
- `select_action(state)`: Select action based on curiosity
- `evaluate_novelty(state)`: Evaluate state novelty
- `get_curiosity_level()`: Get current curiosity

### DevelopmentalInterface
Interface for neural development and plasticity.

```python
class DevelopmentalInterface(brain=None):
    """Developmental control interface."""
```

**Key Methods:**
- `advance_development()`: Advance developmental stage
- `get_developmental_stage()`: Get current stage
- `get_plasticity_rate()`: Get plasticity rate

### NeuromodulationController
Control neuromodulation systems.

```python
class NeuromodulationController(brain=None):
    """Neuromodulation system controller."""
```

**Key Methods:**
- `apply_reward_modulation(reward)`: Apply reward-based modulation
- `apply_curiosity_modulation()`: Apply curiosity-based modulation
- `get_modulation_levels()`: Get current modulation levels

### MemoryManager
Interface for memory system management.

```python
class MemoryManager(brain=None):
    """Memory system interface."""
```

**Key Methods:**
- `store_experience(state, action, reward, next_state)`: Store experience
- `retrieve_experience(state)`: Retrieve similar experiences
- `consolidate_memory()`: Consolidate memories

## Usage Examples

### Basic Simulation

```python
from nlm_python_api import HighLevelNLM

# Create and initialize
nlm = HighLevelNLM()
nlm.initialize()

# Run simulation
for step in range(100):
    nlm.step()
    
    if step % 10 == 0:
        perception = nlm.get_perception()
        print(f"Step {step}, Perception: {perception}")
```

### Action Planning

```python
from nlm_python_api import HighLevelNLM, ActionPlanner

# Setup with planner
nlm = HighLevelNLM()
planner = ActionPlanner(nlm.get_brain())

# Learning loop
for episode in range(50):
    state = nlm.reset()
    done = False
    
    while not done:
        action = planner.act(state)
        next_state, reward, done = nlm.step(action)
        planner.learn(state, action, reward, next_state, done)
        state = next_state
```

### Curiosity-Driven Exploration

```python
from nlm_python_api import HighLevelNLM, CuriosityDrivenExploration

# Setup with curiosity
nlm = HighLevelNLM()
curiosity = CuriosityDrivenExploration(nlm.get_brain())

# Exploration loop
for step in range(100):
    state = nlm.get_perception()
    action = curiosity.select_action(state)
    result = nlm.execute_action(action)
```

### Developmental Learning

```python
from nlm_python_api import HighLevelNLM, DevelopmentalInterface

# Setup with development
nlm = HighLevelNLM()
development = DevelopmentalInterface(nlm.get_brain())

# Developmental simulation
for epoch in range(10):
    stage = development.get_developmental_stage()
    print(f"Developmental stage: {stage}")
    
    for step in range(100):
        nlm.step()
        development.advance_development()
```

## Advanced Features

### Multi-Agent Systems

```python
from nlm_python_api import HighLevelNLM

class MultiAgentSystem:
    def __init__(self):
        self.agents = []
        self.shared_environment = HighLevelNLM()
    
    def add_agent(self, agent_config):
        agent = HighLevelNLM(**agent_config)
        self.agents.append(agent)
    
    def run_step(self):
        for agent in self.agents:
            action = self.select_agent_action(agent)
            agent.execute_action(action)
    
    def select_agent_action(self, agent):
        # Implement agent selection logic
        pass
```

### Hierarchical Control

```python
class HierarchicalController:
    def __init__(self):
        self.low_level_planner = ActionPlanner()
        self.high_level_planner = ActionPlanner(exploration_rate=0.05)
        self.goal_manager = GoalManager()
    
    def plan_action(self, state):
        goal = self.goal_manager.get_current_goal()
        high_action = self.high_level_planner.act(goal)
        
        if high_action.requires_low_level:
            low_action = self.low_level_planner.act(state)
            return self.combine_actions(high_action, low_action)
        
        return high_action
```

## Performance Optimization

### Batch Processing

```python
def batch_simulation(simulation_configs):
    from nlm_python_api import HighLevelNLM
    import concurrent.futures
    
    def run_simulation(config):
        sim = HighLevelNLM(**config)
        for _ in range(config.get('steps', 100)):
            sim.step()
        return sim
    
    with concurrent.futures.ThreadPoolExecutor() as executor:
        results = list(executor.map(run_simulation, simulation_configs))
    
    return results
```

## Configuration

### Configuration Dictionary

```python
base_config = {
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1,
    'plasticity_learning_rate': 0.01,
    'environment': {
        'name': 'GridWorld',
        'width': 20,
        'height': 20
    }
}
```

### Environment Configuration

```python
environment_config = {
    'vision_width': 10,
    'vision_height': 10,
    'vision_channels': 3,
    'enable_sensors': True,
    'sensor_noise': 0.01
}
```

## Error Handling

```python
from nlm_python_api import HighLevelNLM

class RobustSimulation:
    def __init__(self):
        self.sim = HighLevelNLM()
        self.retry_count = 3
    
    def safe_step(self, action=None):
        for attempt in range(self.retry_count):
            try:
                if action is not None:
                    return self.sim.step(action)
                else:
                    return self.sim.step()
            except Exception as e:
                print(f"Step failed (attempt {attempt + 1}): {e}")
                if attempt == self.retry_count - 1:
                    raise
                time.sleep(0.1)  # Brief pause before retry
```

## Testing and Validation

### Unit Tests

```python
import unittest
from nlm_python_api import HighLevelNLM, ActionPlanner

class TestHighLevelNLM(unittest.TestCase):
    def setUp(self):
        self.nlm = HighLevelNLM()
        self.nlm.initialize()
    
    def test_step(self):
        initial_step = self.nlm.current_step
        self.nlm.step()
        self.assertEqual(self.nlm.current_step, initial_step + 1)
    
    def test_planner(self):
        planner = ActionPlanner(self.nlm.get_brain())
        state = self.nlm.get_perception()
        action = planner.act(state)
        self.assertIsNotNone(action)
```

## Serialization and Persistence

```python
import json
from nlm_python_api import HighLevelNLM

class SimulationPersistence:
    def __init__(self, nlm):
        self.nlm = nlm
    
    def save_state(self, filepath):
        state = {
            'brain_state': self.nlm.get_brain().save(),
            'config': self.nlm.get_config(),
            'step_count': self.nlm.current_step,
            'total_reward': self.nlm.total_reward
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
    
    def load_state(self, filepath):
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        self.nlm.get_brain().load(state['brain_state'])
        self.nlm.current_step = state['step_count']
        self.nlm.total_reward = state['total_reward']
```

## Visualization

### Basic Visualization

```python
import matplotlib.pyplot as plt
from nlm_python_api import HighLevelNLM

class Visualizer:
    def __init__(self, nlm):
        self.nlm = nlm
        self.fig, self.ax = plt.subplots()
    
    def update(self, frame):
        brain = self.nlm.get_brain()
        self.ax.clear()
        
        # Visualize neural activity
        neurons = brain.get_neurons()
        for neuron in neurons:
            if neuron.is_active():
                self.ax.plot(neuron.x, neuron.y, 'ro')
        
        plt.title(f"Step: {frame}")
        return self.ax,
    
    def run_animation(self, frames=100):
        anim = animation.FuncAnimation(
            self.fig, self.update, frames=frames,
            interval=50, blit=True
        )
        plt.show()
```

## Future Enhancements

### Planned Features

1. **Distributed Computing**: Parallel simulation across multiple processes
2. **Web Interface**: Real-time visualization and control
3. **Advanced AI Integration**: Integration with transformer models
4. **Custom Neural Architectures**: User-defined network structures
5. **Digital Twin**: Real-time system simulation

## License

This software is provided under the MIT License.
