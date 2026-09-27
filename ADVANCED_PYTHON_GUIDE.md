# NLM Python Bindings - Advanced Guide
# Comprehensive examples and best practices for NLM Python users

## Table of Contents

1. [Introduction to Advanced Python API](#introduction)
2. [Factory Methods and Convenience Functions](#factory-methods)
3. [Enhanced Error Handling and Type Safety](#error-handling)
4. [Configuration Management](#configuration)
5. [Advanced Agent Behavior](#advanced-agents)
6. [Performance Optimization](#performance)
7. [Integration with External Tools](#integration)
8. [Troubleshooting and Best Practices](#troubleshooting)

---

## Introduction to Advanced Python API<a name="introduction"></a>

The NLM Python bindings provide a comprehensive interface to the NLM neural simulation framework. This guide covers advanced features that go beyond the basic examples found in the beginner documentation.

### Key Improvements in This Version

- **Enhanced Type Safety**: Better validation and error handling
- **Pythonic APIs**: Methods that feel natural in Python
- **Convenience Functions**: Reduced boilerplate for common operations
- **Advanced Configuration**: Flexible configuration management
- **Performance Monitoring**: Built-in metrics and profiling tools

---

## Factory Methods and Convenience Functions<a name="factory-methods"></a>

### 1. Configuration Templates

```python
import pynlm

# Create specialized configurations
def create_training_config(neuron_count=1000, simulation_time=10.0):
    """Create a configuration optimized for learning training."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("simulation_timestep", 0.001)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_structural_plasticity", True)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    return config

# Create experimental configuration
def create_experimental_config(plasticity_rate=0.001):
    """Create configuration for experimental plasticity studies."""
    config = pynlm.createDefaultConfig()
    config.set("synaptogenesis_rate", plasticity_rate)
    config.set("pruning_rate", plasticity_rate * 0.1)
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    return config

# Usage
config = create_training_config(neuron_count=2000)
brain = pynlm.createBrain(config)
```

### 2. Brain Templates

```python
def create_brain_with_memory(config):
    """Create a brain with enhanced memory systems."""
    brain = pynlm.createBrain(config)
    
    # Add multiple regions for different memory types
    region_id = brain.addRegion("working_memory")
    # Configure region properties...
    
    return brain

def create_development_brain(config, initial_stage="critical_period"):
    """Create a brain with specific developmental stage."""
    brain = pynlm.createBrain(config)
    brain.setDevelopmentalStage(
        pynlm.DevelopmentalStage.CriticalPeriod
    )
    return brain
```

### 3. World Templates

```python
def create_complex_world(width=50, height=50):
    """Create a complex world with obstacles and resources."""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 16, 16)  # Higher resolution vision
    
    # Add environmental features
    obstacles = []
    resources = []
    
    # Add walls
    for x in range(width):
        world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall))
        world.addObject(pynlm.WorldObject(x, height-1, pynlm.WorldObjectType.Wall))
    
    # Add resources randomly
    for _ in range(20):
        x = pynlm.random.randint(5, width-5)
        y = pynlm.random.randint(5, height-5)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 1.0, 0.3))
    
    return world

# Using a helper function
import random
pynlm.random = random  # Set random module if needed
```

---

## Enhanced Error Handling and Type Safety<a name="error-handling"></a>

### 1. Custom Exceptions

```python
import pynlm

def safe_operation(operation, *args, **kwargs):
    """Perform operations with enhanced error handling."""
    try:
        result = operation(*args, **kwargs)
        return result, None
    except pynlm.InvalidArgumentError as e:
        return None, f"Invalid argument: {e}"
    except pynlm.OutOfRangeError as e:
        return None, f"Out of range: {e}"
    except pynlm.RuntimeError as e:
        return None, f"Runtime error: {e}"

# Example usage
config, error = safe_operation(pynlm.createDefaultConfig)
if error:
    print(f"Error creating config: {error}")
else:
    print("Config created successfully")
```

### 2. Type Validation

```python
def validate_percept(percept):
    """Validate and normalize a sensory percept."""
    if not isinstance(percept, pynlm.SensoryPercept):
        raise TypeError("Expected SensoryPercept")
    
    # Check that percept has at least one sensory modality
    if (percept.getVisionWidth() == 0 and percept.getTouchSize() == 0 and 
        percept.getInternalSize() == 0 and percept.getAudioSize() == 0):
        raise ValueError("Percept has no sensory data")
    
    return percept

def normalize_vision(vision_data):
    """Normalize vision data to [0, 1] range."""
    if not vision_data:
        return vision_data
    
    min_val = min(vision_data)
    max_val = max(vision_data)
    
    if max_val - min_val < 1e-6:  # All values are the same
        return [0.5] * len(vision_data)
    
    return [(x - min_val) / (max_val - min_val) for x in vision_data]
```

### 3. Safe Dictionary Operations

```python
def percept_from_dict(data):
    """Create a percept from dictionary with validation."""
    try:
        percept = pynlm.SensoryPercept.from_dict(data)
        
        # Additional validation
        if "vision" in data:
            vision_data = data["vision"]
            if len(vision_data.get("data", [])) == 0:
                raise ValueError("Vision data cannot be empty")
        
        return percept
    except (KeyError, ValueError, TypeError) as e:
        raise ValueError(f"Invalid percept data: {e}")

# Safe conversion
def safe_to_dict(obj):
    """Convert object to dictionary with error handling."""
    try:
        return obj.to_dict()
    except AttributeError:
        return {"error": "Object does not support dictionary conversion"}
    except Exception as e:
        return {"error": f"Conversion failed: {e}"}
```

---

## Configuration Management<a name="configuration"></a>

### 1. Configuration Templates

```python
class NLMConfiguration:
    """High-level configuration manager for NLM simulations."""
    
    @staticmethod
    def create_default():
        """Create default configuration with all optimizations."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("simulation_timestep", 0.001)
        config.set("connection_probability", 0.15)
        config.set("stdp_ltp_weight", 0.02)
        config.set("stdp_ltd_weight", 0.015)
        config.set("stdp_tau", 20.0)
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("enable_structural_plasticity", True)
        config.set("enable_development", True)
        config.set("synaptogenesis_rate", 0.0001)
        config.set("pruning_rate", 0.00001)
        return config
    
    @staticmethod
    def create_minimal():
        """Create minimal configuration for basic functionality."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        return config
    
    @staticmethod
    def create_learning(config, reward_type="simple"):
        """Create configuration optimized for learning."""
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("enable_structural_plasticity", True)
        
        if reward_type == "spike_based":
            config.set("reward_function", "spike_based")
            config.set("reward_threshold", 10)
        elif reward_type == "prediction_error":
            config.set("reward_function", "prediction_error")
        
        return config

def compare_configurations(*configs):
    """Compare multiple configurations and return differences."""
    if len(configs) < 2:
        return {}
    
    base_keys = set(configs[0].getKeys())
    for config in configs[1:]:
        base_keys &= set(config.getKeys())
    
    differences = {}
    for key in base_keys:
        values = [config.get(key) for config in configs]
        if not all(v == values[0] for v in values):
            differences[key] = values
    
    return differences
```

### 2. Configuration Validation

```python
def validate_configuration(config):
    """Validate configuration and return list of issues."""
    issues = []
    
    # Check neuron count
    neuron_count = config.get("neuron_count", 0)
    if neuron_count <= 0:
        issues.append("neuron_count must be positive")
    
    # Check timestep
    timestep = config.get("simulation_timestep", 0)
    if timestep <= 0:
        issues.append("simulation_timestep must be positive")
    
    # Check compatibility
    if config.get("region_count", 1) < 1:
        issues.append("region_count must be at least 1")
    
    # Check plasticity settings
    if (config.get("stdp_ltp_weight", 0) < 0 or 
        config.get("stdp_ltd_weight", 0) < 0):
        issues.append("STDP weights must be non-negative")
    
    return issues

def configure_for_task(task_type, config=None):
    """Configure NLM for specific task types."""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    if task_type == "pattern_recognition":
        config.set("neuron_count", 2000)
        config.set("enable_structural_plasticity", True)
        config.set("stdp_ltp_weight", 0.03)
        config.set("stdp_ltd_weight", 0.02)
    
    elif task_type == "sequential_learning":
        config.set("neuron_count", 500)
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("synaptogenesis_rate", 0.001)
        config.set("pruning_rate", 0.0001)
    
    elif task_type == "decision_making":
        config.set("neuron_count", 1500)
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("region_count", 3)
    
    return config
```

---

## Advanced Agent Behavior<a name="advanced-agents"></a>

### 1. Custom Agent Classes

```python
class AdvancedNLM:
    """Advanced wrapper for NLM with enhanced functionality."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.world = None
        self.agent = None
        self.step_count = 0
        self.metrics = {
            'spikes': [],
            'reward': [],
            'curiosity': [],
            'performance': []
        }
    
    def setup_world(self, world_config):
        """Setup world with configuration."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            world_config['width'],
            world_config['height'],
            world_config.get('vision_width', 8),
            world_config.get('vision_height', 8)
        )
        self.world.reset()
        
        # Setup agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        return self
    
    def simulate_step(self, reward=0.0, predicted_reward=0.0):
        """Simulate one step with optional reward."""
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain computation
        self.brain.step(self.step_count)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        
        # Apply action in world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply reward modulation
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        self.agent.updateDevelopment(self.config.get("simulation_timestep", 0.001))
        
        # Collect metrics
        self.metrics['spikes'].append(self.brain.getTotalSpikeCount())
        self.metrics['reward'].append(reward)
        self.metrics['curiosity'].append(self.agent.getCuriosityLevel())
        self.metrics['performance'].append(
            self.agent.getNeuromodulationLevel()
        )
        
        self.step_count += 1
        return action
    
    def run_episode(self, max_steps=1000):
        """Run a complete episode."""
        for step in range(max_steps):
            if self.step_count >= max_steps:
                break
            
            # Simple behavior: explore randomly
            action = self.agent.decodeMotorCommand()
            
            # Update world and get reward
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(self.step_count)
            self.world.update(self.config.get("simulation_timestep", 0.001))
            
            # Apply reward based on environment
            reward = self.calculate_reward()
            predicted_reward = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            yield step, action, reward, self.brain.getTotalSpikeCount()
    
    def calculate_reward(self):
        """Calculate reward based on agent's actions."""
        # Example: reward for exploration
        if self.agent.getCuriosityLevel() > 0.7:
            return 1.0
        elif self.agent.getNeuromodulationLevel() > 0.5:
            return 0.5
        else:
            return 0.1
    
    def get_stats(self):
        """Get comprehensive statistics."""
        return {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'episode_steps': self.step_count,
            'mean_spikes_per_step': (
                sum(self.metrics['spikes']) / len(self.metrics['spikes'])
                if self.metrics['spikes'] else 0
            )
        }
    
    def save_brain_state(self, filepath):
        """Save brain state to file."""
        return self.brain.save(filepath)
    
    def load_brain_state(self, filepath):
        """Load brain state from file."""
        return self.brain.load(filepath)
```

### 2. Agent Strategies

```python
class AgentStrategy:
    """Base class for agent strategies."""
    
    def __init__(self, agent):
        self.agent = agent
    
    def select_action(self, percept):
        """Select action based on strategy."""
        raise NotImplementedError

class RandomExplorationStrategy(AgentStrategy):
    """Agent that explores randomly with some exploitation."""
    
    def __init__(self, agent, exploration_rate=0.3):
        super().__init__(agent)
        self.exploration_rate = exploration_rate
        self.action_history = []
    
    def select_action(self, percept):
        import random
        if random.random() < self.exploration_rate:
            # Random action
            action_type = random.choice(list(pynlm.ActionType))
            action = pynlm.Action(action_type)
        else:
            # Use agent's learned action
            action = self.agent.decodeMotorCommand()
        
        self.action_history.append(action)
        return action

class RewardMaximizingStrategy(AgentStrategy):
    """Agent that tries to maximize cumulative reward."""
    
    def __init__(self, agent, learning_rate=0.1):
        super().__init__(agent)
        self.learning_rate = learning_rate
        self.reward_history = []
        self.value_estimates = {}
    
    def select_action(self, percept):
        # Use prediction error to guide action selection
        prediction_error = self.agent.getPredictionError()
        
        if prediction_error > 0.5:  # High uncertainty
            # Explore more
            import random
            action_type = random.choice(list(pynlm.ActionType))
            action = pynlm.Action(action_type)
        else:
            # Exploit learned knowledge
            action = self.agent.decodeMotorCommand()
        
        self.reward_history.append(prediction_error)
        return action
```

---

## Performance Optimization<a name="performance"></a>

### 1. Batch Processing

```python
class BatchProcessor:
    """Process multiple steps in batch for better performance."""
    
    def __init__(self, brain, config=None):
        self.brain = brain
        self.config = config or brain.getConfig()
        self.batch_size = config.get("batch_size", 100)
        self.steps_per_batch = config.get("steps_per_batch", 10)
    
    def batch_simulation(self, steps):
        """Run simulation in batches."""
        for batch_start in range(0, steps, self.batch_size):
            batch_end = min(batch_start + self.batch_size, steps)
            
            # Process batch
            for step in range(batch_start, batch_end):
                self.brain.step(step)
            
            # Yield batch results
            yield {
                'batch_range': (batch_start, batch_end),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            }
    
    def optimized_simulation(self, steps, parallel=True):
        """Run optimized simulation with optional parallelization."""
        if parallel:
            # Parallel processing (conceptual)
            # In practice, this would use multiprocessing
            yield from self.batch_simulation(steps)
        else:
            # Sequential processing
            yield from self.batch_simulation(steps)
```

### 2. Memory Management

```python
class MemoryManager:
    """Manage memory usage and optimization."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_snapshots = []
        self.optimization_threshold = 0.8  # 80% memory usage
    
    def get_memory_usage(self):
        """Get current memory usage (conceptual)."""
        # This would depend on the actual memory usage tracking
        # Return a value between 0.0 and 1.0
        return 0.5  # Placeholder
    
    def optimize_memory(self):
        """Optimize memory usage."""
        memory_usage = self.get_memory_usage()
        
        if memory_usage > self.optimization_threshold:
            # Reduce memory footprint
            self.brain.logStatus()
            print("Memory usage high, optimizing...")
            
            # Take snapshot for potential rollback
            self.memory_snapshots.append(self.brain.getTotalNeuronCount())
        
        return memory_usage
    
    def checkpoint(self, filepath):
        """Create checkpoint of current state."""
        return self.brain.save(filepath)
    
    def restore(self, filepath):
        """Restore from checkpoint."""
        return self.brain.load(filepath)
```

---

## Integration with External Tools<a name="integration"></a>

### 1. Visualization Integration

```python
class VisualizationIntegration:
    """Integrate NLM with external visualization tools."""
    
    def __init__(self, brain, world=None):
        self.brain = brain
        self.world = world
        self.visualizer = None
    
    def setup_visualization(self, tool='matplotlib'):
        """Setup visualization tool."""
        if tool == 'matplotlib':
            try:
                import matplotlib.pyplot as plt
                self.visualizer = {'type': 'matplotlib', 'plt': plt}
                return True
            except ImportError:
                print("matplotlib not installed")
                return False
        elif tool == 'plotly':
            try:
                import plotly.graph_objs as go
                self.visualizer = {'type': 'plotly', 'go': go}
                return True
            except ImportError:
                print("plotly not installed")
                return False
        return False
    
    def visualize_brain_state(self, filepath=None):
        """Visualize current brain state."""
        if not self.visualizer:
            return False
        
        stats = {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        if self.visualizer['type'] == 'matplotlib':
            self._visualize_matplotlib(stats)
        elif self.visualizer['type'] == 'plotly':
            self._visualize_plotly(stats)
        
        if filepath:
            # Save visualization
            pass
        
        return True
    
    def _visualize_matplotlib(self, stats):
        """Matplotlib visualization."""
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Neuron vs Synapse count
        axes[0, 0].bar(['Neurons', 'Synapses'], 
                      [stats['neurons'], stats['synapses']], 
                      color=['skyblue', 'lightcoral'])
        axes[0, 0].set_title('Brain Network Structure')
        axes[0, 0].set_ylabel('Count')
        
        # Plot 2: Firing rate over time (conceptual)
        axes[0, 1].plot([0, 1], [stats['firing_rate'], stats['firing_rate']], 
                       marker='o', markersize=10)
        axes[0, 1].set_title('Current Firing Rate')
        axes[0, 1].set_ylabel('Hz')
        axes[0, 1].set_ylim(0, max(stats['firing_rate'], 1))
        
        # Plot 3: Developmental stage
        stages = list(pynlm.DevelopmentalStage)
        stage_values = [1 if s == stats['developmental_stage'] else 0 
                       for s in stages]
        axes[1, 0].bar(stages, stage_values, color='lightgreen')
        axes[1, 0].set_title('Developmental Stage')
        axes[1, 0].set_ylabel('Current')
        
        # Plot 4: Statistics summary
        axes[1, 1].axis('off')
        stats_text = f"Total Spikes: {stats['spikes']}\n"
        stats_text += f"Excitatory/Inhibitory: {stats['firing_rate']:.2f}\n"
        stats_text += f"Stage: {stats['developmental_stage']}"
        axes[1, 1].text(0.1, 0.5, stats_text, fontsize=12, 
                       verticalalignment='center', fontfamily='monospace')
        axes[1, 1].set_title('Brain Statistics')
        
        plt.tight_layout()
        plt.show()
    
    def _visualize_plotly(self, stats):
        """Plotly visualization."""
        import plotly.graph_objs as go
        
        # Create dashboard
        fig = go.Figure()
        
        # Add subplots (conceptual)
        fig.update_layout(
            title='NLM Brain State Visualization',
            width=1200,
            height=800
        )
        
        fig.show()
```

### 2. Data Integration

```python
class DataIntegration:
    """Integrate NLM with data pipelines."""
    
    def __init__(self, brain):
        self.brain = brain
        self.data_logger = None
        self.performance_tracker = None
    
    def setup_data_logging(self, log_file, format='json'):
        """Setup data logging."""
        import json
        from datetime import datetime
        
        self.data_logger = {
            'file': open(log_file, 'w'),
            'format': format,
            'start_time': datetime.now()
        }
    
    def log_simulation_data(self, step, action, reward, metrics):
        """Log simulation data."""
        if not self.data_logger:
            return False
        
        data_point = {
            'timestamp': datetime.now().isoformat(),
            'step': step,
            'action': action,
            'reward': reward,
            'metrics': metrics,
            'brain_state': {
                'neurons': self.brain.getTotalNeuronCount(),
                'synapses': self.brain.getTotalSynapseCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            }
        }
        
        if self.data_logger['format'] == 'json':
            self.data_logger['file'].write(json.dumps(data_point) + '\n')
            self.data_logger['file'].flush()
        
        return True
    
    def export_data_for_ml(self, filepath, format='csv'):
        """Export data for machine learning."""
        import pandas as pd
        import numpy as np
        
        # Collect data over time
        data = []
        
        # This would collect data from the simulation
        # For this example, we'll create synthetic data
        for i in range(100):
            data.append({
                'step': i,
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'developmental_stage': self.brain.getDevelopmentalStage()
            })
        
        df = pd.DataFrame(data)
        
        if format == 'csv':
            df.to_csv(filepath, index=False)
        elif format == 'parquet':
            df.to_parquet(filepath)
        
        return True
```

---

## Troubleshooting and Best Practices<a name="troubleshooting"></a>

### 1. Common Issues and Solutions

#### Issue: Brain Not Initializing

```python
def debug_brain_initialization(brain):
    """Debug brain initialization issues."""
    print("=== Brain Initialization Debug ===")
    
    # Check configuration
    config = brain.getConfig()
    print(f"Neuron count: {config.get('neuron_count')}")
    print(f"Region count: {config.get('region_count')}")
    print(f"Timestep: {config.get('simulation_timestep')}")
    
    # Check brain state
    brain.logStatus()
    
    # Try to identify specific issues
    neuron_count = config.get('neuron_count', 0)
    if neuron_count <= 0:
        print("ERROR: Invalid neuron count in configuration")
    
    # Check if brain has been initialized
    if brain.getTotalNeuronCount() == 0:
        print("ERROR: Brain has no neurons")
        print("This could be due to:")
        print("1. Invalid configuration")
        print("2. Configuration file not found")
        print("3. Invalid configuration parameters")
    
    return brain.getTotalNeuronCount() > 0

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
if not debug_brain_initialization(brain):
    print("Brain initialization failed - see debug output above")
```

#### Issue: High Memory Usage

```python
def optimize_memory_usage(brain, threshold=0.8):
    """Optimize memory usage for large brains."""
    print("=== Memory Optimization ===")
    
    # Check current metrics
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    print(f"Current memory usage: ~{(neuron_count * 2 + synapse_count * 4) / 1024 / 1024:.1f} MB")
    
    # Optimization strategies
    if neuron_count > 5000:
        print("High neuron count detected - consider:")
        print("1. Reducing neuron count")
        print("2. Using sparse connectivity")
        print("3. Implementing region-based processing")
    
    if synapse_count > neuron_count * 10:
        print("High synapse-to-neuron ratio - consider:")
        print("1. Pruning weak synapses")
        print("2. Using Hebbian learning with lower rates")
        print("3. Implementing structural plasticity")
    
    return True
```

#### Issue: Poor Performance

```python
class PerformanceProfiler:
    """Profile and optimize NLM performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.timings = []
        self.metrics = []
    
    def profile_step(self, step, action, reward):
        """Profile a single simulation step."""
        import time
        
        start_time = time.time()
        
        # Record brain state before step
        neurons_before = self.brain.getTotalNeuronCount()
        spikes_before = self.brain.getTotalSpikeCount()
        
        # Execute step
        self.brain.step(step)
        
        # Record brain state after step
        neurons_after = self.brain.getTotalNeuronCount()
        spikes_after = self.brain.getTotalSpikeCount()
        
        end_time = time.time()
        
        # Calculate metrics
        timing = end_time - start_time
        spike_count = spikes_after - spikes_before
        firing_rate = spike_count / timing if timing > 0 else 0
        
        self.timings.append({
            'step': step,
            'timing': timing,
            'spike_count': spike_count,
            'firing_rate': firing_rate
        })
        
        return timing, spike_count
    
    def analyze_performance(self):
        """Analyze performance metrics."""
        if not self.timings:
            return {}
        
        timings = [t['timing'] for t in self.timings]
        spike_counts = [t['spike_count'] for t in self.timings]
        
        analysis = {
            'mean_timing': sum(timings) / len(timings),
            'std_timing': (sum((t - sum(timings)/len(timings))**2 for t in timings) / len(timings))**0.5,
            'mean_spikes_per_step': sum(spike_counts) / len(spike_counts),
            'max_spikes': max(spike_counts),
            'min_spikes': min(spike_counts),
            'steps_per_second': len(self.timings) / sum(timings) if sum(timings) > 0 else 0
        }
        
        return analysis
    
    def identify_bottlenecks(self, analysis):
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        if analysis['mean_timing'] > 0.1:  # Steps taking more than 100ms
            bottlenecks.append(
                f"Step timing is slow (mean: {analysis['mean_timing']:.3f}s)"
            )
        
        if analysis['steps_per_second'] < 100:  # Less than 100 steps per second
            bottlenecks.append(
                f"Low step rate ({analysis['steps_per_second']:.1f} steps/sec)"
            )
        
        if analysis['mean_spikes_per_step'] < 0.1:  # Very low firing
            bottlenecks.append(
                f"Very low firing rate ({analysis['mean_spikes_per_step']:.2f} spikes/step)"
            )
        
        return bottlenecks
```

### 2. Best Practices

#### Best Practice: Configuration Management

```python
def best_practice_configuration(config):
    """Apply best practices for configuration."""
    # Ensure required keys are present
    required_keys = [
        'neuron_count',
        'region_count', 
        'simulation_timestep',
        'connection_probability'
    ]
    
    for key in required_keys:
        if not config.has(key):
            print(f"WARNING: Missing required key '{key}'")
    
    # Validate ranges
    neuron_count = config.get('neuron_count', 0)
    if neuron_count < 10 or neuron_count > 100000:
        print(f"WARNING: neuron_count {neuron_count} may be out of optimal range")
    
    # Set sensible defaults
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_structural_plasticity", False)  # Turn off for testing
    
    return config

# Usage
config = pynlm.createDefaultConfig()
config = best_practice_configuration(config)
```

#### Best Practice: Error Handling

```python
def robust_operation(func, *args, **kwargs):
    """Wrapper for robust error handling."""
    try:
        result = func(*args, **kwargs)
        return result, None
    except Exception as e:
        # Log the error
        import traceback
        print(f"ERROR in {func.__name__}: {e}")
        print(traceback.format_exc())
        
        # Return default result
        return None, str(e)

# Usage
def safe_brain_operation(brain, operation, *args):
    """Safe operation on brain."""
    if not brain or brain.getTotalNeuronCount() == 0:
        print("ERROR: Invalid brain object")
        return None, "Invalid brain object"
    
    result, error = robust_operation(operation, *args)
    return result, error
```

#### Best Practice: Testing and Validation

```python
def validate_python_bindings():
    """Validate Python bindings functionality."""
    print("=== Validating Python Bindings ===")
    
    try:
        # Test 1: Import and basic types
        import pynlm
        
        # Test 2: ID classes
        neuron_id = pynlm.NeuronId(1)
        synapse_id = pynlm.SynapseId(2)
        
        # Test 3: Enums
        neuron_type = pynlm.NeuronType.Excitatory
        
        # Test 4: Configuration
        config = pynlm.createDefaultConfig()
        
        # Test 5: Basic objects
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test 6: World
        world = pynlm.createSimpleWorld()
        
        # Test 7: Action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        print("✅ All Python binding tests passed!")
        return True
        
    except Exception as e:
        print(f"❌ Python binding validation failed: {e}")
        import traceback
        print(traceback.format_exc())
        return False

# Run validation
if validate_python_bindings():
    print("Python bindings are ready for use!")
else:
    print("Please check your NLM installation and try again.")
```

---

## Summary

This advanced guide provides comprehensive examples and best practices for using NLM Python bindings. The enhancements include:

1. **Enhanced Type Safety**: Better validation and error handling
2. **Convenience Functions**: Factory methods and templates
3. **Performance Optimization**: Batch processing and memory management
4. **Integration**: Visualization and data pipeline integration
5. **Best Practices**: Robust error handling and testing

These improvements make the NLM Python bindings more powerful, easier to use, and more suitable for production applications and research.

---

*For the latest documentation and examples, visit the NLM documentation website.*