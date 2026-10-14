# NLM (Neural Learning Machine) - Advanced API Reference

This document provides comprehensive reference information for the NLM Python API, including advanced features, utilities, and usage patterns.

## Contents

1. Introduction to Advanced Features
2. Convenience Functions and Utilities
3. Error Handling and Validation
4. Visualization and Monitoring
5. Performance Profiling
6. Advanced Configuration
7. Machine Learning Applications
8. Advanced Tutorials

---

## 1. Introduction to Advanced Features

The NLM Python API provides access to a complete neural simulation system. This reference covers advanced usage patterns and features beyond the basic functionality.

### 1.1 Why Use Advanced Features

Advanced features enable:

- **Complex simulation environments** with sophisticated agent behaviors
- **Custom training and learning scenarios** for research applications
- **Performance-critical deployments** with profiling and optimization
- **Integration with external systems** through enhanced I/O capabilities
- **Reproducible experiments** with checkpoint management and parameter validation

### 1.2 Core Advanced Concepts

- **Neuromodulation**: Learning through reward prediction errors
- **Development**: Brain maturation and plasticity changes over time
- **Memory Integration**: Working, episodic, and associative memory systems
- **Prediction Systems**: Forward models and error-based learning
- **Structural Plasticity**: Dynamic synapse creation and pruning

---

## 2. Convenience Functions and Utilities

### 2.1 Simulation Setup Utilities

```python
import pynlm

# Create optimized brain configuration
def createOptimizedBrain(neurons=5000, regions=3):
    """Create a brain with optimized settings for different use cases."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    config.set("brain.region_count", regions)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Run batch simulations
def runBatchSimulation(configs, num_steps=100):
    """Run multiple simulations with different configurations."""
    results = []
    for i, cfg in enumerate(configs):
        brain = pynlm.createBrain(cfg)
        brain.initialize()
        
        # Enable all advanced features
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
            
        results.append({
            'config': cfg,
            'spikes': brain.getTotalSpikeCount(),
            'learning_rate': agent.getCuriosityLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'final_firing_rate': brain.getAverageFiringRate()
        })
    
    return results

# Create a complete simulated environment
def createSimulatedEnvironment(width=50, height=50):
    """Create a rich simulated environment with multiple objects and resources."""
    brain = createOptimizedBrain()
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(width//2, height//2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add environmental resources and hazards
    world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(45, 45, pynlm.WorldObjectType.Resource, value=20.0))
    world.addObject(pynlm.WorldObject(25, 15, pynlm.WorldObjectType.Hazard, value=5.0))
    
    return brain, agent, world

# Performance profiling utility
def profileBrainPerformance(brain, world, agent, num_steps=1000):
    """Profile brain performance and return metrics."""
    import time
    
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
    
    end_time = time.time()
    duration = end_time - start_time
    
    metrics = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'development_stage': brain.getDevelopmentalStage(),
        'simulation_time': duration,
        'steps_per_second': num_steps / duration,
        'spikes_per_second': brain.getTotalSpikeCount() / duration
    }
    
    return metrics
```

### 2.2 Utility Classes

```python
# Agent utilities for common tasks
class AgentSimulator:
    """High-level utilities for agent-based simulations."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def runEpisode(self, max_steps=1000, termination_conditions=None):
        """Run a complete episode with intelligent termination."""
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get perception
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Check termination
            if termination_conditions and termination_conditions(self):
                break
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
        
        return {
            'steps': step + 1,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel()
        }
    
    def exploreEnvironment(self, exploration_steps=500):
        """Run exploration behavior to discover environment."""
        for step in range(exploration_steps):
            # Use curiosity-driven exploration
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Select action based on exploration vs. exploitation
            if self.agent.getCuriosityLevel() > 0.7:  # High curiosity = explore
                action = self.agent.decodeMotorCommand()
            else:  # Normal behavior
                action = self.agent.decodeMotorCommand()
            
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)

# Configuration utilities
class ConfigPresets:
    """Predefined configuration presets for different use cases."""
    
    @staticmethod
    def research_config():
        """Configuration optimized for research and experimentation."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.region_count", 5)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction_error.enable", True)
        config.set("development.initial_plasticity", 1.0)
        config.set("development.maturation_rate", 0.001)
        return config
    
    @staticmethod
    def production_config():
        """Configuration optimized for production deployment."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 3)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", False)  # Reduce complexity
        config.set("plasticity.structural.enable", False)  # Performance optimization
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)  # Keep exploration
        config.set("neuromod.novelty.enable", False)  # Reduce complexity
        config.set("neuromod.prediction_error.enable", True)
        config.set("development.initial_plasticity", 0.5)
        config.set("development.maturation_rate", 0.0005)
        return config
    
    @staticmethod
    def rapid_learning_config():
        """Configuration optimized for fast learning scenarios."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.region_count", 4)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)  # Higher learning rate
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)  # Fast adaptation
        config.set("neuromod.dopamine.scale", 2.0)  # Stronger reward signals
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction_error.enable", True)
        config.set("development.initial_plasticity", 2.0)
        config.set("development.maturation_rate", 0.002)
        return config
```

---

## 3. Error Handling and Validation

### 3.1 Exception Types

The NLM Python API provides specific exception types for different error conditions:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 3.2 Input Validation

```python
# Configuration validation
def validateConfig(config):
    """Validate configuration parameters."""
    errors = []
    
    # Check neuron count
    if config.has("brain.neuron_count"):
        neurons = config.get("brain.neuron_count")
        if neurons <= 0:
            errors.append("Neuron count must be positive")
        elif neurons > 100000:
            errors.append("Neuron count too large (max 100,000)")
    
    # Check plasticity settings
    if config.has("plasticity.stdp.learning_rate"):
        rate = config.get("plasticity.stdp.learning_rate")
        if rate < 0 or rate > 1:
            errors.append("STDP learning rate must be between 0 and 1")
    
    # Check neuromodulation settings
    if config.has("neuromod.dopamine.scale"):
        scale = config.get("neuromod.dopamine.scale")
        if scale < 0:
            errors.append("Dopamine scale must be positive")
    
    if errors:
        raise pynlm.ConfigurationError("; ".join(errors))
    
    return True

# Simulation validation
class SimulationValidator:
    """Validate simulation state and parameters."""
    
    @staticmethod
    def validateStep(brain, step, time):
        """Validate step parameters."""
        errors = []
        
        if brain is None:
            errors.append("Brain cannot be None")
        
        if step < 0:
            errors.append("Step number must be non-negative")
        
        if time < 0:
            errors.append("Simulation time must be non-negative")
        
        # Check brain initialization
        if brain and brain.getTotalNeuronCount() == 0:
            errors.append("Brain has no neurons")
        
        if errors:
            raise pynlm.SimulationError("; ".join(errors))
        
        return True
```

### 3.3 Error Recovery

```python
# Error recovery utilities
class ErrorRecovery:
    """Tools for recovering from simulation errors."""
    
    @staticmethod
    def safeStep(brain, step, time, max_retries=3):
        """Attempt to step brain with error recovery."""
        for attempt in range(max_retries):
            try:
                SimulationValidator.validateStep(brain, step, time)
                brain.step(step, time)
                return True
            except pynlm.SimulationError as e:
                if attempt == max_retries - 1:
                    raise
                print(f"Step {step} failed (attempt {attempt + 1}): {e}")
                # Attempt recovery
                try:
                    brain.reset()
                    if brain.getTotalNeuronCount() > 0:
                        brain.initialize()
                except:
                    continue
        
        return False
    
    @staticmethod
    def saveOnError(brain, filepath, error_callback=None):
        """Save brain state if error occurs during simulation."""
        try:
            yield  # Let simulation run
        except Exception as e:
            print(f"Simulation error: {e}")
            try:
                brain.save(filepath)
                print(f"Brain state saved to {filepath}")
            except Exception as save_error:
                print(f"Failed to save brain state: {save_error}")
            
            if error_callback:
                error_callback(e)
```

---

## 4. Visualization and Monitoring

### 4.1 Basic Visualization

```python
# Simple visualization of simulation
import matplotlib.pyplot as plt
import numpy as np

def plotBrainState(brain, world=None, agent=None, save_fig=False):
    """Plot comprehensive brain state visualization."""
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('NLM Brain State Visualization', fontsize=16)
    
    # 1. Firing rate distribution
    ax = axes[0, 0]
    # Simulated firing rate data (replace with actual access)
    firing_rates = np.random.lognormal(0, 1, 100)
    ax.hist(firing_rates, bins=20, alpha=0.7, color='skyblue')
    ax.set_xlabel('Firing Rate (Hz)')
    ax.set_ylabel('Count')
    ax.set_title('Firing Rate Distribution')
    
    # 2. Synaptic weight distribution
    ax = axes[0, 1]
    weights = np.random.beta(2, 5, 100)  # Simulated synaptic weights
    ax.hist(weights, bins=20, alpha=0.7, color='lightgreen')
    ax.set_xlabel('Synaptic Weight')
    ax.set_ylabel('Count')
    ax.set_title('Synaptic Weight Distribution')
    
    # 3. Development stage
    ax = axes[0, 2]
    stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
    current_stage = brain.getDevelopmentalStage()
    stage_values = [0 if s != current_stage else 1 for s in stages]
    ax.bar(stages, stage_values, color=['red', 'orange', 'yellow', 'green', 'blue'])
    ax.set_ylabel('Current Stage')
    ax.set_title('Developmental Stage')
    ax.set_ylim(0, 1)
    
    # 4. Simulation statistics over time (simulated)
    ax = axes[1, 0]
    time_points = np.arange(0, 100, 10)
    spikes = np.cumsum(np.random.poisson(5, 10))
    ax.plot(time_points, spikes, 'b-o', linewidth=2, markersize=4)
    ax.set_xlabel('Simulation Steps')
    ax.set_ylabel('Cumulative Spikes')
    ax.set_title('Spike Accumulation Over Time')
    ax.grid(True, alpha=0.3)
    
    # 5. Curiosity and Novelty levels (simulated)
    ax = axes[1, 1]
    curiosity = np.random.beta(2, 5, 100)
    novelty = np.random.beta(2, 5, 100)
    ax.plot(curiosity, novelty, 'g-', alpha=0.7, linewidth=1)
    ax.scatter(curiosity[-10:], novelty[-10:], c='red', s=50, alpha=0.7)
    ax.set_xlabel('Curiosity Level')
    ax.set_ylabel('Novelty Level')
    ax.set_title('Curiosity-Novelty Dynamics')
    ax.grid(True, alpha=0.3)
    
    # 6. Environment state (if available)
    ax = axes[1, 2]
    if world:
        body = world.getAgentBody()
        ax.scatter([body.x], [body.y], s=200, c='red', marker='s', alpha=0.7)
        ax.set_xlim(0, world.getWidth())
        ax.set_ylim(0, world.getHeight())
        ax.set_xlabel('X Position')
        ax.set_ylabel('Y Position')
        ax.set_title('Agent Position in World')
        ax.set_aspect('equal')
    else:
        ax.text(0.5, 0.5, 'World not available', 
                horizontalalignment='center', verticalalignment='center',
                transform=ax.transAxes, fontsize=12)
        ax.set_title('Environment State')
    
    plt.tight_layout()
    
    if save_fig:
        plt.savefig('brain_state_visualization.png', dpi=300, bbox_inches='tight')
        print("Visualization saved as 'brain_state_visualization.png'")
    
    plt.show()
    return fig
```

### 4.2 Real-time Monitoring

```python
# Real-time monitoring class
class RealTimeMonitor:
    """Monitor simulation in real-time with plotting updates."""
    
    def __init__(self, update_interval=1.0):
        self.update_interval = update_interval
        self.time_series = {
            'step': [],
            'spikes': [],
            'firing_rate': [],
            'curiosity': [],
            'novelty': [],
            'prediction_error': [],
            'reward': []
        }
        self.last_update = 0
    
    def update(self, brain, agent=None, world=None, step=0, time=0.0, reward=0.0):
        """Update monitoring data with current simulation state."""
        current_time = time
        
        # Record data
        self.time_series['step'].append(step)
        self.time_series['spikes'].append(brain.getTotalSpikeCount())
        self.time_series['firing_rate'].append(brain.getAverageFiringRate())
        
        if agent:
            self.time_series['curiosity'].append(agent.getCuriosityLevel())
            self.time_series['novelty'].append(agent.getNoveltyLevel())
            self.time_series['prediction_error'].append(agent.getPredictionError())
        
        self.time_series['reward'].append(reward)
        self.last_update = current_time
        
        # Generate plot if interval exceeded
        if current_time - self.last_update >= self.update_interval:
            self.plotCurrentState()
    
    def plotCurrentState(self, brain, agent=None, world=None):
        """Plot current state in real-time."""
        import matplotlib.pyplot as plt
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle(f'NLM Real-Time Monitoring - Step {self.time_series["step"][-1]}', 
                    fontsize=14)
        
        # 1. Spike count over time
        ax1.plot(self.time_series['step'], self.time_series['spikes'], 'b-')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Total Spikes')
        ax1.set_title('Spike Count Over Time')
        ax1.grid(True, alpha=0.3)
        
        # 2. Firing rate
        ax2.plot(self.time_series['step'], self.time_series['firing_rate'], 'r-')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Average Firing Rate (Hz)')
        ax2.set_title('Firing Rate Over Time')
        ax2.grid(True, alpha=0.3)
        
        # 3. Curiosity and Novelty
        if agent and self.time_series['curiosity']:
            ax3.plot(self.time_series['step'], self.time_series['curiosity'], 'g-', label='Curiosity')
            ax3.plot(self.time_series['step'], self.time_series['novelty'], 'orange', label='Novelty')
            ax3.set_xlabel('Step')
            ax3.set_ylabel('Level')
            ax3.set_title('Neuromodulation Levels')
            ax3.legend()
            ax3.grid(True, alpha=0.3)
        
        # 4. Reward
        ax4.plot(self.time_series['step'], self.time_series['reward'], 'purple', alpha=0.7)
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Reward')
        ax4.set_title('Reward Over Time')
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()
    
    def saveData(self, filepath):
        """Save monitoring data to file."""
        import pandas as pd
        
        df = pd.DataFrame(self.time_series)
        df.to_csv(filepath, index=False)
        print(f"Monitoring data saved to {filepath}")
```

---

## 5. Performance Profiling

### 5.1 Basic Profiling

```python
import time
import cProfile
import pstats
from io import StringIO

class PerformanceProfiler:
    """Profile NLM simulation performance."""
    
    def __init__(self):
        self.profiler = cProfile.Profile()
        self.timings = {}
    
    def startProfiling(self):
        """Start performance profiling."""
        self.profiler.enable()
        self.start_time = time.time()
    
    def stopProfiling(self):
        """Stop profiling and return results."""
        self.profiler.disable()
        end_time = time.time()
        duration = end_time - self.start_time
        
        # Get profiling statistics
        s = StringIO()
        ps = pstats.Stats(self.profiler, stream=s).sort_stats('cumulative')
        ps.print_stats()
        
        profile_results = {
            'total_time': duration,
            'profile_output': s.getvalue(),
            'memory_usage': self.getMemoryUsage(),
            'cpu_usage': ps.total_calls
        }
        
        return profile_results
    
    def profileFunction(self, func, *args, **kwargs):
        """Profile a specific function."""
        start_time = time.time()
        
        def wrapper():
            self.profiler.enable()
            result = func(*args, **kwargs)
            self.profiler.disable()
            return result
        
        result = wrapper()
        
        # Get timing for this function
        self.timings[func.__name__] = time.time() - start_time
        
        return result
    
    def getMemoryUsage(self):
        """Get current memory usage."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024  # MB
    
    def compareConfigurations(self, configs, simulation_func, num_runs=5):
        """Compare performance of different configurations."""
        results = {}
        
        for config_name, config in configs.items():
            times = []
            memory_usages = []
            
            for run in range(num_runs):
                self.startProfiling()
                simulation_func(config)
                profile_result = self.stopProfiling()
                
                times.append(profile_result['total_time'])
                memory_usages.append(profile_result['memory_usage'])
            
            results[config_name] = {
                'avg_time': sum(times) / len(times),
                'std_time': (sum((t - sum(times)/len(times))**2 for t in times) / len(times))**0.5,
                'avg_memory': sum(memory_usages) / len(memory_usages),
                'min_time': min(times),
                'max_time': max(times)
            }
        
        return results
```

### 5.2 Advanced Performance Analysis

```python
# Advanced performance analysis
class AdvancedPerformanceAnalysis:
    """Advanced performance analysis and optimization recommendations."""
    
    @staticmethod
    def analyzeBrainComplexity(brain):
        """Analyze brain complexity and provide optimization insights."""
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        
        analysis = {
            'neuron_count': neurons,
            'synapse_count': synapses,
            'synapses_per_neuron': synapses / neurons if neurons > 0 else 0,
            'complexity_score': min(neurons / 1000, 10) + min(synapses / 5000, 10),
            'recommendations': []
        }
        
        # Generate recommendations
        if neurons > 5000:
            analysis['recommendations'].append(
                "Consider reducing neuron count for better performance"
            )
        
        if synapses / neurons > 10:
            analysis['recommendations'].append(
                "High synapse-to-neuron ratio may impact performance"
            )
        
        if brain.getAverageFiringRate() > 100:
            analysis['recommendations'].append(
                "Very high firing rate detected - consider tuning neuron parameters"
            )
        
        return analysis
    
    @staticmethod
    def optimizeConfiguration(config, performance_target):
        """Optimize configuration based on performance targets."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Copy existing settings
        for key in config.getKeys():
            optimized_config.set(key, config.get(key))
        
        # Apply optimizations based on target
        if performance_target == 'speed':
            optimized_config.set("brain.neuron_count", min(config.get("brain.neuron_count"), 1000))
            optimized_config.set("plasticity.structural.enable", False)
            optimized_config.set("neuromod.novelty.enable", False)
        
        elif performance_target == 'learning':
            optimized_config.set("brain.neuron_count", min(config.get("brain.neuron_count"), 5000))
            optimized_config.set("plasticity.stdp.learning_rate", 0.005)
            optimized_config.set("neuromod.curiosity.enable", True)
        
        elif performance_target == 'memory':
            optimized_config.set("brain.neuron_count", config.get("brain.neuron_count"))
            optimized_config.set("plasticity.structural.enable", True)
            optimized_config.set("neuromod.novelty.enable", True)
        
        return optimized_config
```

---

## 6. Advanced Configuration

### 6.1 Dynamic Configuration Updates

```python
# Dynamic configuration management
class DynamicConfiguration:
    """Manage dynamic configuration updates during simulation."""
    
    def __init__(self, brain, agent=None, world=None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.config_history = []
    
    def updateParameter(self, parameter_path, value, validate=True):
        """Update a configuration parameter dynamically."""
        # Get current config
        config = self.brain.getConfig()
        
        # Set new value
        if validate:
            self._validateParameter(parameter_path, value)
        
        config.set(parameter_path, value)
        self.config_history.append({
            'time': self.world.getSimulationTime() if self.world else 0,
            'parameter': parameter_path,
            'value': value,
            'brain': self.brain.getTotalNeuronCount()
        })
        
        print(f"Updated {parameter_path} = {value}")
    
    def _validateParameter(self, parameter_path, value):
        """Validate parameter update."""
        # Implement validation logic based on parameter path
        if parameter_path == "brain.neuron_count" and value < 0:
            raise ValueError("Neuron count must be positive")
        
        # Add more validation rules as needed
    
    def getParameter(self, parameter_path):
        """Get current value of a parameter."""
        config = self.brain.getConfig()
        return config.get(parameter_path)
    
    def revertParameter(self, parameter_path, timestamp=None):
        """Revert a parameter to a previous value."""
        if timestamp:
            # Find the most recent update before timestamp
            for entry in reversed(self.config_history):
                if entry['parameter'] == parameter_path and entry['time'] <= timestamp:
                    self.updateParameter(parameter_path, entry['value'])
                    return
        else:
            # Revert to last value
            for entry in reversed(self.config_history):
                if entry['parameter'] == parameter_path:
                    self.updateParameter(parameter_path, entry['value'])
                    return
    
    def exportHistory(self, filepath):
        """Export configuration history to file."""
        import pandas as pd
        
        df = pd.DataFrame(self.config_history)
        df.to_csv(filepath, index=False)
        print(f"Configuration history exported to {filepath}")
```

### 6.2 Configuration Templates

```python
# Configuration templates for different applications
class ConfigurationTemplates:
    """Predefined configuration templates for common applications."""
    
    @staticmethod
    def educational_demo():
        """Configuration for educational demonstrations."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.region_count", 2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.initial_plasticity", 0.8)
        return config
    
    @staticmethod
    def research_experiment():
        """Configuration for research experiments."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.region_count", 6)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction_error.enable", True)
        config.set("development.initial_plasticity", 1.2)
        config.set("development.maturation_rate", 0.002)
        return config
    
    @staticmethod
    def production_system():
        """Configuration for production systems."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("brain.region_count", 3)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", False)
        config.set("plasticity.structural.enable", False)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", False)
        config.set("development.initial_plasticity", 0.6)
        config.set("development.maturation_rate", 0.001)
        return config
    
    @staticmethod
    def high_performance():
        """High-performance configuration for speed-critical applications."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)
        config.set("brain.region_count", 2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", False)
        config.set("neuromod.dopamine.scale", 0.8)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", False)
        config.set("neuromod.prediction_error.enable", True)
        config.set("development.initial_plasticity", 0.4)
        return config
```

---

## 7. Machine Learning Applications

### 7.1 Neural Network Training

```python
# Train NLM with custom data
class NeuralNetworkTrainer:
    """Train NLM with custom data for machine learning applications."""
    
    def __init__(self, config):
        self.config = config
        self.experiments = []
    
    def createTrainingEpisode(self, sensory_data, target_actions):
        """Create a training episode with sensory data and target actions."""
        # Create brain and agent
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        
        # Run episode
        for step, (sensory, target) in enumerate(zip(sensory_data, target_actions)):
            # Process sensory input
            agent.processSensoryInput(sensory)
            
            # Brain processing
            brain.step(step)
            
            # Get current action
            current_action = agent.decodeMotorCommand()
            
            # Calculate reward based on target
            reward = self._calculateReward(target, current_action)
            agent.applyRewardModulation(reward, 0.0)
            
            # Apply target action (if we have control)
            if step < len(target_actions) - 1:
                world.applyMotorCommand(target[step], world.getSimulationTime())
        
        return brain, agent, world
    
    def _calculateReward(self, target_action, current_action):
        """Calculate reward based on target vs current action."""
        # Implement custom reward function
        if target_action == current_action:
            return 1.0
        else:
            return -0.1
    
    def trainModel(self, training_data, epochs=100):
        """Train NLM model on training data."""
        for epoch in range(epochs):
            epoch_loss = 0
            epoch_spikes = 0
            
            for sensory_data, target_actions in training_data:
                brain, agent, world = self.createTrainingEpisode(
                    sensory_data, target_actions
                )
                
                # Calculate metrics
                loss = self._calculateLoss(brain, agent, target_actions)
                epoch_loss += loss
                epoch_spikes += brain.getTotalSpikeCount()
            
            print(f"Epoch {epoch + 1}/{epochs}: Loss = {epoch_loss/len(training_data):.4f}, "
                  f"Avg Spikes = {epoch_spikes/len(training_data):.2f}")
            
            self.experiments.append({
                'epoch': epoch + 1,
                'loss': epoch_loss / len(training_data),
                'spikes': epoch_spikes / len(training_data)
            })
    
    def _calculateLoss(self, brain, agent, target_actions):
        """Calculate loss function for training."""
        # Implement custom loss function
        prediction_error = agent.getPredictionError()
        curiosity_penalty = 1.0 - agent.getCuriosityLevel()
        return prediction_error + 0.1 * curiosity_penalty
```

### 7.2 Reinforcement Learning

```python
# Reinforcement learning utilities
class ReinforcementLearning:
    """Implement reinforcement learning with NLM."""
    
    def __init__(self, config):
        self.config = config
        self.q_table = {}  # State-action value table
        self.epsilon = 0.1  # Exploration rate
        self.alpha = 0.1    # Learning rate
        self.gamma = 0.99   # Discount factor
    
    def selectAction(self, state, available_actions):
        """Select action using epsilon-greedy policy."""
        if np.random.random() < self.epsilon:
            return np.random.choice(available_actions)
        else:
            state_key = str(state)
            if state_key not in self.q_table:
                self.q_table[state_key] = np.zeros(len(available_actions))
            
            # Get best action from Q-table
            q_values = self.q_table[state_key]
            max_q = np.max(q_values)
            best_actions = np.where(q_values == max_q)[0]
            return np.random.choice(best_actions)
    
    def updateQTable(self, state, action, reward, next_state, done):
        """Update Q-table using Bellman equation."""
        state_key = str(state)
        next_state_key = str(next_state)
        
        if state_key not in self.q_table:
            self.q_table[state_key] = np.zeros(len(self.config.get("action_space", [])))
        
        if next_state_key not in self.q_table:
            self.q_table[next_state_key] = np.zeros(len(self.config.get("action_space", [])))
        
        # Q-learning update
        current_q = self.q_table[state_key][action]
        max_next_q = np.max(self.q_table[next_state_key]) if not done else 0
        
        self.q_table[state_key][action] = current_q + self.alpha * (
            reward + self.gamma * max_next_q - current_q
        )
    
    def trainAgent(self, env, num_episodes=1000):
        """Train agent using reinforcement learning."""
        episode_rewards = []
        episode_lengths = []
        
        for episode in range(num_episodes):
            state = env.reset()
            done = False
            total_reward = 0
            steps = 0
            
            while not done and steps < 1000:
                # Select action
                action = self.selectAction(state, env.action_space)
                
                # Execute action
                next_state, reward, done = env.step(action)
                
                # Update Q-table
                self.updateQTable(state, action, reward, next_state, done)
                
                # Update state
                state = next_state
                total_reward += reward
                steps += 1
            
            episode_rewards.append(total_reward)
            episode_lengths.append(steps)
            
            if (episode + 1) % 100 == 0:
                avg_reward = np.mean(episode_rewards[-100:])
                print(f"Episode {episode + 1}/{num_episodes}: "
                      f"Avg Reward = {avg_reward:.2f}")
        
        return episode_rewards, episode_lengths
```

---

## 8. Advanced Tutorials

### 8.1 Tutorial 1: Building a Cognitive Agent

This tutorial demonstrates how to build a cognitive agent using NLM with memory, learning, and decision-making capabilities.

```python
import pynlm
import numpy as np

def buildCognitiveAgent():
    """Build a cognitive agent with memory and learning capabilities."""
    
    # 1. Create agent with optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15, 15)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictionError(True)
    
    # 4. Implement cognitive control loop
    class CognitiveAgent:
        def __init__(self, brain, agent, world):
            self.brain = brain
            self.agent = agent
            self.world = world
            self.memory_buffer = []
            self.planning_horizon = 5
        
        def planAction(self):
            """Plan action using neural prediction and attention."""
            # Get current state
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Neural planning
            action = self.agent.decodeMotorCommand()
            
            # Attention mechanism
            attention_weight = self.agent.getCuriosityLevel()
            
            return action, attention_weight
        
        def learnFromExperience(self, reward, predicted_reward):
            """Learn from experience using neuromodulation."""
            self.agent.applyRewardModulation(reward, predicted_reward)
            self.agent.updateDevelopment(0.1)
            
            # Store in memory
            self.memory_buffer.append({
                'state': self.brain.getTotalNeuronCount(),
                'reward': reward,
                'curiosity': self.agent.getCuriosityLevel()
            })
            
            # Consolidate important memories
            if len(self.memory_buffer) > 100:
                self._consolidateMemories()
        
        def _consolidateMemories(self):
            """Consolidate important memories."""
            high_curiosity_memories = [
                m for m in self.memory_buffer 
                if m['curiosity'] > 0.7
            ]
            
            if high_curiosity_memories:
                # Replay important memories
                for memory in high_curiosity_memories[-5:]:
                    print(f"Consolidating memory with curiosity level {memory['curiosity']:.2f}")
                
                # Keep only recent memories
                self.memory_buffer = self.memory_buffer[-50:]
        
        def runCycle(self, step):
            """Run one cognitive control cycle."""
            # Update world
            self.world.update(0.1)
            
            # Perceive environment
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Cognitive processing
            action, attention_weight = self.planAction()
            self.brain.step(step)
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from experience
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.learnFromExperience(reward, 0.0)
            
            # Get metrics
            metrics = {
                'step': step,
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'reward': reward
            }
            
            return metrics
    
    # 5. Run cognitive agent simulation
    cognitive_agent = CognitiveAgent(brain, agent, world)
    
    # Enable visualization
    # plotBrainState(brain, world, agent)
    
    return cognitive_agent

# Example usage
if __name__ == "__main__":
    cognitive_agent = buildCognitiveAgent()
    
    # Run simulation
    for step in range(500):
        metrics = cognitive_agent.runCycle(step)
        
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Firing Rate: {metrics['firing_rate']:.2f}Hz, "
                  f"Curiosity: {metrics['curiosity']:.2f}, "
                  f"Novelty: {metrics['novelty']:.2f}")
```

### 8.2 Tutorial 2: Parameter Optimization

This tutorial demonstrates how to optimize NLM parameters for different performance targets.

```python
import pynlm
import numpy as np
from scipy.optimize import minimize

def createOptimizationEnvironment():
    """Create environment for parameter optimization."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 3)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("development.initial_plasticity", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, agent, world

def simulationObjective(params, config, environment):
    """Objective function for parameter optimization."""
    # Unpack parameters
    neuron_count = int(np.clip(params[0], 500, 5000))
    learning_rate = np.clip(params[1], 0.0001, 0.01)
    dopamine_scale = np.clip(params[2], 0.5, 2.0)
    plasticity = np.clip(params[3], 0.5, 2.0)
    
    # Create optimized config
    optimized_config = pynlm.createDefaultConfig()
    optimized_config.set("brain.neuron_count", neuron_count)
    optimized_config.set("brain.region_count", 3)
    optimized_config.set("plasticity.stdp.learning_rate", learning_rate)
    optimized_config.set("neuromod.dopamine.scale", dopamine_scale)
    optimized_config.set("development.initial_plasticity", plasticity)
    
    # Run simulation
    brain = pynlm.createBrain(optimized_config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation and calculate objective
    total_spikes = 0
    total_reward = 0
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        total_spikes += brain.getTotalSpikeCount()
        total_reward += reward if 'reward' in locals() else 0
    
    # Calculate objective: maximize reward, minimize complexity
    objective = -total_reward  # Minimize negative reward (maximize reward)
    complexity_penalty = neuron_count / 5000  # Penalize large networks
    objective += 0.1 * complexity_penalty
    
    return objective

def optimizeParameters():
    """Optimize NLM parameters using scipy."""
    # Create initial environment
    config, environment = createOptimizationEnvironment()
    
    # Define initial guess and bounds
    initial_params = [1500, 0.001, 1.0, 1.0]
    bounds = [(500, 5000), (0.0001, 0.01), (0.5, 2.0), (0.5, 2.0)]
    
    # Run optimization
    result = minimize(
        simulationObjective,
        initial_params,
        args=(config, environment),
        bounds=bounds,
        method='L-BFGS-B',
        options={'maxiter': 50, 'disp': True}
    )
    
    print(f"Optimization successful: {result.success}")
    print(f"Optimal parameters: {result.x}")
    print(f"Optimal objective value: {result.fun}")
    
    return result

# Run parameter optimization
if __name__ == "__main__":
    result = optimizeParameters()
```

---

## 9. Next Steps

### 9.1 Continue Learning

- Read the detailed documentation for each class in the API reference
- Try the example scripts provided in the `examples/` directory
- Join the NLM community for discussions and support
- Contribute to the NLM project with new features and improvements

### 9.2 Research Applications

The NLM framework is particularly well-suited for:

- **Cognitive modeling**: Simulate human-like cognitive processes
- **Neural engineering**: Control real-world systems with neural controllers
- **Machine learning**: Alternative to traditional neural networks
- **Neuroscience**: Test hypotheses about brain function
- **Robotics**: Create adaptive robotic behaviors

### 9.3 Performance Considerations

For production deployment:

- Use `production_config()` for best performance
- Monitor system resources using the profiling tools
- Implement error handling and recovery mechanisms
- Use checkpointing for long-running simulations
- Consider parallelization for large-scale simulations

This advanced reference provides the foundation for building sophisticated applications with the NLM framework. Start with the basic examples, then explore the advanced features as you become comfortable with the system.