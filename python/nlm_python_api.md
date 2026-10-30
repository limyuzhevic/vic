# High-Level Convenience API for NLM Python Bindings

This module provides high-level convenience methods for common neural simulation tasks.
It builds upon the existing pynlm bindings to provide an enhanced, more Pythonic API.

## Installation

```python
import pynlm as nlm
from nlm_python_api import HighLevelNLM, NeuralSimulation, ActionPlanner
```

## Quick Start Examples

### Simple Neural Simulation

```python
from nlm_python_api import NeuralSimulation

# Create a simple simulation
sim = NeuralSimulation()

# Run a simulation loop
for step in range(100):
    sim.step()
    
    # Get current state
    state = sim.get_state()
    neuromod_levels = sim.get_brain().getNeuromodulationLevels()
    
    if step % 10 == 0:
        print(f"Step {step}: State: {state}, Neuromod: {neuromod_levels}")
```

### Action Planning and Learning

```python
from nlm_python_api import ActionPlanner

# Create an action planner
planner = ActionPlanner(exploration_rate=0.1, learning_rate=0.01)

# Run learning simulation
for episode in range(50):
    state = sim.reset()
    done = False
    
    while not done:
        action = planner.act(state)
        next_state, reward, done = sim.step(action)
        planner.learn(state, action, reward, next_state, done)
        
        state = next_state
```

### Environment Interaction

```python
from nlm_python_api import HighLevelNLM

# Create a high-level NLM interface
hl_nlm = HighLevelNLM()

# Set up the environment
config = hl_nlm.setup_environment(
    width=20,
    height=20,
    vision_width=10,
    vision_height=10
)

# Run goal-directed behavior
for step in range(200):
    # Get current perception
    perception = hl_nlm.get_perception()
    
    # Choose action based on goal
    action = hl_nlm.choose_action(perception, goal="find_resource")
    
    # Execute action
    result = hl_nlm.execute_action(action)
    
    # Check if goal achieved
    if hl_nlm.check_goal_achieved("find_resource"):
        print("Goal achieved!")
        break
```

### Visualization and Monitoring

```python
from nlm_python_api import HighLevelNLM
import matplotlib.pyplot as plt

# Create and run simulation
nlm = HighLevelNLM()
simulation_data = nlm.run_simulation(max_steps=1000, visualize=True)

# Plot results
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.plot(simulation_data['rewards'])
plt.title('Reward Over Time')
plt.xlabel('Step')
plt.ylabel('Reward')

plt.subplot(2, 3, 2)
plt.hist(simulation_data['action_distribution'], bins=range(len(ActionType)))
plt.title('Action Distribution')
plt.xlabel('Action Type')
plt.ylabel('Frequency')

plt.subplot(2, 3, 3)
for neuromod_name, levels in simulation_data['neuromod_levels']:
    plt.plot(levels, label=neurom_name)
plt.title('Neuromodulation Levels')
plt.legend()

plt.tight_layout()
plt.show()
```

## Class Definitions

### NeuralSimulation
High-level interface for neural simulation with automatic step management and state tracking.

**Constructor:**
```python
class NeuralSimulation(config=None, world=None, brain=None, enable_visualization=False)
```

**Parameters:**
- `config`: Optional configuration object
- `world`: Optional world object
- `brain`: Optional brain object  
- `enable_visualization`: Whether to enable visualization

**Key Methods:**
- `initialize()` - Initialize the simulation
- `reset()` - Reset to initial state
- `step()` - Perform one simulation step
- `step(action)` - Perform step with specific action
- `get_state()` - Get current simulation state
- `get_brain()` - Get underlying brain object
- `get_world()` - Get underlying world object
- `get_sensory_percept()` - Get current sensory percept
- `get_motor_action()` - Get current motor action
- `get_reward()` - Get current reward

**Properties:**
- `is_running` - Whether simulation is running
- `current_step` - Current simulation step
- `total_reward` - Total reward accumulated
- `episode_count` - Number of episodes completed

### ActionPlanner
High-level action planner with exploration-exploitation balance and learning capabilities.

**Constructor:**
```python
class ActionPlanner(brain=None, exploration_rate=0.1, learning_rate=0.01, discount_factor=0.99)
```

**Parameters:**
- `brain`: Optional brain object
- `exploration_rate`: Rate of exploration (epsilon-greedy)
- `learning_rate`: Rate of learning updates
- `discount_factor`: Discount factor for future rewards

**Key Methods:**
- `act(state)` - Choose action for given state
- `learn(state, action, reward, next_state, done)` - Learn from experience
- `reset()` - Reset planner to initial state
- `get_policy(state)` - Get policy for given state
- `get_q_values(state)` - Get Q-values for given state

**Properties:**
- `exploration_rate` - Current exploration rate
- `learning_rate` - Current learning rate
- `action_counts` - Statistics on action usage
- `reward_history` - History of received rewards

### HighLevelNLM
Comprehensive interface combining all NLM systems with high-level abstractions.

**Constructor:**
```python
class HighLevelNLM(config=None, environment_config=None)
```

**Key Methods:**
- `setup_environment(**kwargs)` - Setup environment with parameters
- `get_perception()` - Get current perception
- `choose_action(perception, goal=None, context=None)` - Choose action
- `execute_action(action)` - Execute action and return result
- `check_goal_achieved(goal)` - Check if goal is achieved
- `get_neural_dynamics()` - Get neural dynamics information
- `get_memory_contents()` - Get memory system contents
- `run_simulation(max_steps=1000, visualize=False)` - Run complete simulation

### CuriosityDrivenExploration
Curiosity-driven exploration using novelty detection and intrinsic motivation.

**Constructor:**
```python
class CuriosityDrivenExploration(brain=None, learning_rate=0.01, novelty_threshold=0.5)
```

**Key Methods:**
- `select_action(state)` - Select action based on curiosity
- `evaluate_novelty(state)` - Evaluate novelty of current state
- `update_model(state, action, next_state)` - Update curiosity model
- `get_curiosity_level()` - Get current curiosity level

### DevelopmentalInterface
Interface for developmental stages and neural growth.

**Constructor:**
```python
class DevelopmentalInterface(brain=None)
```

**Key Methods:**
- `advance_development()` - Advance developmental stage
- `get_developmental_stage()` - Get current developmental stage
- `get_plasticity_rate()` - Get current plasticity rate
- `schedule_critical_period(duration)` - Schedule critical period

### NeuromodulationController
Interface for neuromodulation systems.

**Constructor:**
```python
class NeuromodulationController(brain=None)
```

**Key Methods:**
- `apply_reward_modulation(reward, prediction_error)` - Apply reward-based modulation
- `apply_curiosity_modulation()` - Apply curiosity-based modulation
- `apply_novelty_modulation()` - Apply novelty-based modulation
- `get_modulation_levels()` - Get current modulation levels
- `enable_modulation(modulation_type, enable=True)` - Enable/disable specific modulation

### MemoryManager
Interface for memory systems management.

**Constructor:**
```python
class MemoryManager(brain=None)
```

**Key Methods:**
- `store_experience(state, action, reward, next_state)` - Store experience
- `retrieve_experience(state)` - Retrieve similar experiences
- `consolidate_memory()` - Consolidate important memories
- `get_working_memory()` - Get working memory contents
- `get_episodic_memory()` - Get episodic memory contents
- `get_associative_memory()` - Get associative memory contents

## Advanced Usage Examples

### Custom Simulation Loop

```python
from nlm_python_api import NeuralSimulation, ActionPlanner

class CustomSimulation:
    def __init__(self):
        self.sim = NeuralSimulation()
        self.planner = ActionPlanner(exploration_rate=0.2)
        self.episode_rewards = []
        self.action_history = []
    
    def run_episode(self, max_steps=100):
        state = self.sim.reset()
        total_reward = 0
        
        for step in range(max_steps):
            # Choose action
            action = self.planner.act(state)
            self.action_history.append(action)
            
            # Execute action
            next_state, reward, done = self.sim.step(action)
            
            # Learn from experience
            self.planner.learn(state, action, reward, next_state, done)
            
            # Update state
            state = next_state
            total_reward += reward
            
            if done:
                break
        
        self.episode_rewards.append(total_reward)
        return total_reward
    
    def run_training(self, num_episodes=1000):
        for episode in range(num_episodes):
            reward = self.run_episode()
            
            if episode % 100 == 0:
                avg_reward = sum(self.episode_rewards[-100:]) / 100
                print(f"Episode {episode}, Average Reward: {avg_reward:.2f}")
    
    def evaluate(self, num_episodes=10):
        total_rewards = []
        
        for _ in range(num_episodes):
            total_rewards.append(self.run_episode())
        
        return sum(total_rewards) / len(total_rewards)
```

### Multi-Objective Learning

```python
from nlm_python_api import HighLevelNLM, NeuromodulationController

class MultiObjectiveLearner:
    def __init__(self):
        self.nlm = HighLevelNLM()
        self.neuromod = NeuromodulationController(self.nlm.get_brain())
        self.objectives = ['efficiency', 'exploration', 'conservation']
        self.objective_weights = {'efficiency': 0.5, 'exploration': 0.3, 'conservation': 0.2}
    
    def compute_objective_rewards(self, state, action, next_state):
        """Compute rewards for different objectives."""
        rewards = {}
        
        # Efficiency: maximize reward per step
        rewards['efficiency'] = self.nlm.get_reward()
        
        # Exploration: novelty detection
        rewards['exploration'] = self.neuromod.evaluate_novelty(next_state)
        
        # Conservation: resource preservation
        rewards['conservation'] = self.nlm.get_resource_level()
        
        # Weighted combination
        total_reward = sum(rewards[obj] * self.objective_weights[obj] 
                          for obj in self.objectives)
        
        return rewards, total_reward
    
    def train(self, num_steps=5000):
        for step in range(num_steps):
            state = self.nlm.get_perception()
            action = self.nlm.choose_action(state)
            
            objective_rewards, total_reward = self.compute_objective_rewards(
                state, action, self.nlm.get_next_state(action)
            )
            
            # Store objective rewards for analysis
            if step % 100 == 0:
                print(f"Step {step} Objective Rewards: {objective_rewards}")
            
            # Execute action
            self.nlm.execute_action(action)
```

### Development and Learning Integration

```python
from nlm_python_api import HighLevelNLM, DevelopmentalInterface

class DevelopmentalLearning:
    def __init__(self):
        self.nlm = HighLevelNLM()
        self.development = DevelopmentalInterface(self.nlm.get_brain())
        self.learner = ActionPlanner(self.nlm.get_brain())
    
    def run_developmental_simulation(self, num_epochs=10):
        for epoch in range(num_epochs):
            print(f"\n=== Developmental Epoch {epoch + 1} ===")
            print(f"Stage: {self.development.get_developmental_stage()}")
            
            # Adjust learning parameters based on development
            plasticity = self.development.get_plasticity_rate()
            self.learner.set_learning_rate(0.01 * plasticity)
            
            # Run simulation for this epoch
            self.run_epoch()
    
    def run_epoch(self, max_steps=500):
        self.nlm.reset()
        
        for step in range(max_steps):
            # Get current state
            state = self.nlm.get_perception()
            
            # Choose action with developmental-aware exploration
            action = self.choose_developmental_action(state)
            
            # Execute action
            result = self.nlm.execute_action(action)
            
            # Learn from experience
            self.learner.learn(state, action, result.reward, 
                             self.nlm.get_next_state(action), result.done)
            
            # Advance development periodically
            if step % 100 == 99:
                self.development.advance_development()
                
                # Log developmental progress
                print(f"  Step {step+1}: Advanced to {self.development.get_developmental_stage()}")
    
    def choose_developmental_action(self, state):
        """Choose action based on developmental stage."""
        stage = self.development.get_developmental_stage()
        
        if stage.name == 'CriticalPeriod':
            # During critical period, encourage exploration
            return self.learner.act(state, exploration_rate=0.8)
        elif stage.name == 'Maturation':
            # During maturation, balance exploration and exploitation
            return self.learner.act(state, exploration_rate=0.5)
        else:
            # In adult stage, favor exploitation
            return self.learner.act(state, exploration_rate=0.2)
```

## Performance Optimization

### Batch Processing
```python
def batch_process(states, actions):
    """Process multiple states and actions efficiently."""
    from nlm_python_api import HighLevelNLM
    
    nlm = HighLevelNLM()
    results = []
    
    for state, action in zip(states, actions):
        nlm.receiveSensoryInput(state)
        next_action = nlm.produceAction()
        results.append(next_action)
    
    return results
```

### Parallel Processing
```python
def parallel_simulation(simulation_configs):
    """Run multiple simulations in parallel."""
    from concurrent.futures import ThreadPoolExecutor
    
    def run_simulation(config):
        from nlm_python_api import HighLevelNLM
        sim = HighLevelNLM(config)
        return sim.run_simulation(max_steps=config.get('max_steps', 100))
    
    with ThreadPoolExecutor(max_workers=4) as executor:
        results = list(executor.map(run_simulation, simulation_configs))
    
    return results
```

## Integration with Scientific Python Ecosystem

### NumPy Integration
```python
import numpy as np
from nlm_python_api import HighLevelNLM

class NumpyIntegratedNLM(HighLevelNLM):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._vision_buffer = np.zeros((84, 84, 3), dtype=np.float32)
    
    def get_numpy_vision(self):
        """Get vision data as NumPy array."""
        percept = self.get_perception()
        if hasattr(percept, 'getData'):
            data = percept.getData()
            if len(data) == 84 * 84 * 3:
                return np.array(data).reshape(84, 84, 3)
        return self._vision_buffer
    
    def set_numpy_vision(self, image_array):
        """Set vision data from NumPy array."""
        if isinstance(image_array, np.ndarray) and image_array.shape == (84, 84, 3):
            self._vision_buffer = image_array
            vision = nlm.Vision(84, 84, 3)
            vision.setData(image_array.flatten())
            self.receiveSensoryInput(vision)
```

### Matplotlib Integration
```python
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from nlm_python_api import HighLevelNLM

class Visualizer:
    def __init__(self, nlm):
        self.nlm = nlm
        self.fig, self.ax = plt.subplots(figsize=(12, 8))
        self.im = None
    
    def update_frame(self, frame):
        """Update visualization for animation."""
        # Get current world state
        world = self.nlm.get_world()
        
        # Get agent position
        agent = world.getAgentBody()
        
        # Create visualization
        if self.im is None:
            self.im = self.ax.imshow(world.getMap(), cmap='binary')
        
        # Update agent position
        self.ax.clear()
        self.ax.imshow(world.getMap(), cmap='binary')
        self.ax.plot(agent.x, agent.y, 'ro', markersize=10)
        
        # Add text overlay
        text = f"Step: {self.nlm.get_brain().getTotalSpikeCount()}"
        self.ax.text(0.01, 0.99, text, transform=self.ax.transAxes, 
                    color='white', verticalalignment='top')
        
        return self.im,
    
    def run_animation(self, frames=100, interval=50):
        """Run visualization animation."""
        anim = animation.FuncAnimation(
            self.fig, self.update_frame, frames=frames, 
            interval=interval, blit=True
        )
        plt.show()
```

## Configuration and Customization

### Custom Configuration
```python
from nlm_python_api import HighLevelNLM

class CustomNLM(HighLevelNLM):
    def __init__(self):
        config = {
            'neuron_count': 2000,
            'region_count': 4,
            'connection_probability': 0.05,
            'plasticity_learning_rate': 0.005,
            'environment_name': 'CustomGridWorld',
            'vision_width': 20,
            'vision_height': 20,
            'enable_curiosity': True,
            'enable_development': True,
            'enable_reward_modulation': True
        }
        
        super().__init__(config=config)
    
    def custom_preprocessing(self, percept):
        """Custom sensory preprocessing."""
        # Apply custom transformations
        # Resize, normalize, enhance, etc.
        return processed_percept
```

### Plugin Architecture
```python
from abc import ABC, abstractmethod

class NLMMiddleware(ABC):
    """Base class for NLM middleware plugins."""
    
    @abstractmethod
    def process_state(self, state):
        """Process input state."""
        pass
    
    @abstractmethod
    def process_action(self, action):
        """Process output action."""
        pass

class SensoryEnhancementMiddleware(NLMMiddleware):
    """Enhance sensory inputs with filtering and preprocessing."""
    
    def __init__(self, enhancement_function):
        self.enhancement_function = enhancement_function
    
    def process_state(self, state):
        # Apply enhancement to sensory data
        return self.enhancement_function(state)
class ActionFilteringMiddleware(NLMMiddleware):
    """Filter actions based on context or constraints."""
    
    def __init__(self, allowed_actions=None):
        self.allowed_actions = allowed_actions or []
    
    def process_action(self, action):
        if not self.allowed_actions or action in self.allowed_actions:
            return action
        return None  # Reject action
```

## Error Handling and Robustness

### Graceful Degradation
```python
from nlm_python_api import HighLevelNLM

class RobustNLM(HighLevelNLM):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fallback_strategies = []
    
    def execute_action(self, action):
        """Execute action with fallback strategies."""
        try:
            return super().execute_action(action)
        except Exception as e:
            print(f"Action failed: {e}")
            
            # Try fallback strategies
            for strategy in self.fallback_strategies:
                try:
                    result = strategy(action)
                    if result is not None:
                        return result
                except Exception:
                    continue
            
            # If all strategies fail, return safe default
            print("Using fallback action")
            return self.get_safe_fallback_action()
```

### Monitoring and Health Checks
```python
class HealthMonitor:
    def __init__(self, nlm, check_interval=1000):
        self.nlm = nlm
        self.check_interval = check_interval
        self.health_history = []
    
    def check_health(self):
        """Check NLM health and return status."""
        status = {
            'brain_healthy': True,
            'memory_healthy': True,
            'environment_healthy': True,
            'performance_healthy': True
        }
        
        # Check brain components
        brain = self.nlm.get_brain()
        if brain.getTotalNeuronCount() == 0:
            status['brain_healthy'] = False
        
        # Check memory systems
        if brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() > 1000:
            status['memory_healthy'] = False
        
        # Check environment
        if hasattr(self.nlm, 'get_environment') and self.nlm.get_environment().is_corrupted():
            status['environment_healthy'] = False
        
        # Check performance
        current_fps = 1.0 / (self.nlm.get_brain().getAverageFiringRate())
        if current_fps < 10:  # Too slow
            status['performance_healthy'] = False
        
        self.health_history.append({
            'step': self.nlm.get_brain().getTotalSpikeCount(),
            'status': status,
            'timestamp': time.time()
        })
        
        return status
    
    def get_health_report(self):
        """Generate comprehensive health report."""
        if not self.health_history:
            return "No health data available"
        
        report = "Health Report:\n"
        report += f"Total checks: {len(self.health_history)}\n"
        
        unhealthy_steps = [h for h in self.health_history if not all(h['status'].values())]
        if unhealthy_steps:
            report += f"Unhealthy steps: {len(unhealthy_steps)}\n"
        
        # Recent health trend
        recent_health = self.health_history[-10:]
        healthy_ratio = sum(all(h['status'].values()) for h in recent_health) / len(recent_health)
        report += f"Recent health ratio: {healthy_ratio:.2f}\n"
        
        return report
```

## Serialization and Persistence

### Model Persistence
```python
import pickle
import json
from nlm_python_api import HighLevelNLM

class ModelPersistence:
    def __init__(self, nlm):
        self.nlm = nlm
    
    def save_model(self, filepath, format='pickle'):
        """Save model to file."""
        model_data = {
            'brain_state': self.nlm.get_brain().save(),
            'config': self.nlm.get_config(),
            'environment': self.nlm.get_environment().save(),
            'statistics': self.get_statistics()
        }
        
        if format == 'pickle':
            with open(filepath, 'wb') as f:
                pickle.dump(model_data, f)
        elif format == 'json':
            # Convert to JSON-serializable format
            json_data = self._to_json_serializable(model_data)
            with open(filepath, 'w') as f:
                json.dump(json_data, f, indent=2)
    
    def load_model(self, filepath, format='pickle'):
        """Load model from file."""
        if format == 'pickle':
            with open(filepath, 'rb') as f:
                model_data = pickle.load(f)
        elif format == 'json':
            with open(filepath, 'r') as f:
                model_data = json.load(f)
            model_data = self._from_json_serializable(model_data)
        
        # Restore model components
        self.nlm.get_brain().load(model_data['brain_state'])
        # Restore other components...
    
    def get_statistics(self):
        """Get model statistics."""
        brain = self.nlm.get_brain()
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage().name,
            'neuromodulation_levels': brain.getNeuromodulationLevels()
        }
```

## Testing and Validation

### Unit Testing
```python
import unittest
from nlm_python_api import HighLevelNLM, ActionPlanner

class TestHighLevelNLM(unittest.TestCase):
    def setUp(self):
        self.nlm = HighLevelNLM()
        self.nlm.initialize()
    
    def test_initialization(self):
        """Test basic initialization."""
        self.assertIsNotNone(self.nlm.get_brain())
        self.assertTrue(self.nlm.get_brain().getTotalNeuronCount() > 0)
    
    def test_simulation_step(self):
        """Test simulation step."""
        initial_spikes = self.nlm.get_brain().getTotalSpikeCount()
        self.nlm.step()
        final_spikes = self.nlm.get_brain().getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_action_planner(self):
        """Test action planner."""
        planner = ActionPlanner(self.nlm.get_brain())
        state = self.nlm.get_perception()
        action = planner.act(state)
        self.assertIsNotNone(action)
    
    def test_neuromodulation_levels(self):
        """Test neuromodulation level retrieval."""
        levels = self.nlm.get_brain().getNeuromodulationLevels()
        self.assertIsInstance(levels, dict)
        # Should have at least dopamine or curiosity if available
        self.assertTrue(len(levels) > 0)

if __name__ == '__main__':
    unittest.main()
```

### Integration Testing
```python
def integration_test():
    """Run integration test for complete workflow."""
    from nlm_python_api import HighLevelNLM, ActionPlanner
    
    # Setup
    nlm = HighLevelNLM()
    planner = ActionPlanner(nlm.get_brain())
    
    # Run multiple episodes
    for episode in range(10):
        state = nlm.reset()
        
        for step in range(100):
            # Act
            action = planner.act(state)
            
            # Step
            next_state, reward, done = nlm.step(action)
            
            # Learn
            planner.learn(state, action, reward, next_state, done)
            
            # Update state
            state = next_state
            
            if done:
                break
    
    print("Integration test completed successfully!")
```

## Performance Monitoring

### Benchmark
```python
import time
from nlm_python_api import HighLevelNLM

class PerformanceMonitor:
    def __init__(self, nlm):
        self.nlm = nlm
        self.timings = []
        self.step_counts = []
    
    def start_benchmark(self, num_steps=1000):
        """Start performance benchmark."""
        print(f"Starting benchmark: {num_steps} steps")
        start_time = time.time()
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Perform simulation step
            self.nlm.step()
            
            step_end = time.time()
            step_time = step_end - step_start
            self.timings.append(step_time)
            self.step_counts.append(step)
            
            if step % 100 == 0:
                print(f"Step {step}: {step_time*1000:.2f}ms per step")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Calculate statistics
        avg_time = sum(self.timings) / len(self.timings)
        max_time = max(self.timings)
        min_time = min(self.timings)
        
        print(f"\nBenchmark Results:")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Average time per step: {avg_time*1000:.2f}ms")
        print(f"  Max time per step: {max_time*1000:.2f}ms")
        print(f"  Min time per step: {min_time*1000:.2f}ms")
        print(f"  Steps per second: {num_steps/total_time:.1f}")
    
    def plot_performance(self):
        """Plot performance metrics."""
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(12, 8))
        
        plt.subplot(2, 2, 1)
        plt.plot(self.step_counts, self.timings, 'b-', alpha=0.7)
        plt.title('Step Time')
        plt.xlabel('Step')
        plt.ylabel('Time (s)')
        
        plt.subplot(2, 2, 2)
        rolling_avg = []
        window = 100
        for i in range(window, len(self.timings)):
            rolling_avg.append(sum(self.timings[i-window:i]) / window)
        plt.plot(rolling_avg)
        plt.title('Rolling Average (100 steps)')
        plt.xlabel('Step')
        plt.ylabel('Time (s)')
        
        plt.subplot(2, 2, 3)
        plt.hist(self.timings, bins=50, alpha=0.7)
        plt.title('Time Distribution')
        plt.xlabel('Time (s)')
        plt.ylabel('Frequency')
        
        plt.subplot(2, 2, 4)
        plt.plot(self.step_counts, self.nlm.get_brain().getTotalSpikeCount(), 'g-')
        plt.title('Spike Count')
        plt.xlabel('Step')
        plt.ylabel('Spike Count')
        
        plt.tight_layout()
        plt.show()
```

## Documentation Generation

### API Documentation Generator
```python
def generate_api_docs():
    """Generate comprehensive API documentation."""
    from nlm_python_api import (
        HighLevelNLM, NeuralSimulation, ActionPlanner,
        CuriosityDrivenExploration, DevelopmentalInterface,
        NeuromodulationController, MemoryManager
    )
    
    classes = [
        HighLevelNLM, NeuralSimulation, ActionPlanner,
        CuriosityDrivenExploration, DevelopmentalInterface,
        NeuromodulationController, MemoryManager
    ]
    
    documentation = {
        'version': '1.0.0',
        'generated': time.strftime('%Y-%m-%d %H:%M:%S'),
        'classes': []
    }
    
    for cls in classes:
        cls_doc = {
            'name': cls.__name__,
            'docstring': cls.__doc__,
            'methods': []
        }
        
        # Get methods
        for attr_name in dir(cls):
            if not attr_name.startswith('_'):
                attr = getattr(cls, attr_name)
                if callable(attr):
                    cls_doc['methods'].append({
                        'name': attr_name,
                        'docstring': attr.__doc__,
                        'signature': inspect.signature(attr)
                    })
        
        documentation['classes'].append(cls_doc)
    
    # Save documentation
    with open('API_DOCS.json', 'w') as f:
        json.dump(documentation, f, indent=2, default=str)
    
    print("API documentation generated: API_DOCS.json")
```

## Future Enhancements

The following features are planned for future releases:

1. **Distributed Computing**: Support for parallel processing across multiple machines
2. **Web Interface**: Real-time visualization and control via web browser
3. **Advanced AI Integration**: Integration with transformer models and large language models
4. **Custom Neural Architectures**: Support for user-defined neural network architectures
5. **Digital Twin**: Real-time digital representation of physical systems
6. **Quantum Computing**: Integration with quantum algorithms
7. **Edge Computing**: Optimized for edge devices and IoT systems
8. **Safety Verification**: Formal methods for safety-critical applications

## License

This high-level Python API is provided under the same MIT License as the NLM framework.

## Acknowledgments

This API was developed in collaboration with the NLM research team, building upon the foundation of Phase 6 integration.

## References

1. NLM Research Papers - Computational Brain Modeling
2. Phase 6 Integration Documentation
3. Python Bindings Technical Specifications
4. Scientific Literature on Neuromodulation and Learning
5. Best Practices in High-Performance Computing

---

*This documentation is continuously updated as new features are added and the NLM framework evolves.*
