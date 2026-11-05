# NLM Phase 6 Integration Improvements: Python API Enhancements

## Executive Summary

This document outlines the critical Python API improvements needed for NLM Phase 6. The current Python bindings have good functionality but lack Pythonic convenience methods, comprehensive error handling, and advanced features.

## Current State

**Strengths:**
- Comprehensive core API (Config, Brain, SimpleWorld, etc.)
- 42+ types and methods exposed
- Good integration with C++ brain functionality

**Weaknesses:**
- Limited type safety in Python
- Minimal error handling
- No convenience methods
- Missing numpy support in many places
- Basic documentation
- No advanced features for power users

## Python API Enhancement Strategy

### 1. Enhanced Type Safety and Error Handling

**Add Python exception wrapping for C++ errors:**

```python
class NLMError(Exception):
    """Base exception for all NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Error in configuration handling"""
    pass

class BrainError(NLMError):
    """Error in brain operations"""
    pass

class MemoryError(NLMError):
    """Error in memory system operations"""
    pass

# Wrap C++ exceptions in Python exceptions
def wrap_pybind_exceptions(py_func):
    """Decorator to wrap C++ exceptions in Python exceptions"""
    @functools.wraps(py_func)
    def wrapper(*args, **kwargs):
        try:
            return py_func(*args, **kwargs)
        except std::runtime_error& e:
            raise NLMError(f"NLM runtime error: {e.what()}")
        except std::invalid_argument& e:
            raise ConfigurationError(f"Invalid argument: {e.what()}")
        except std::out_of_range& e:
            raise NLMError(f"Out of range: {e.what()}")
        except ...  # Catch all other C++ exceptions
    return wrapper
```

**Add input validation:**
```python
class Config:
    def __init__(self):
        self._config = pynlm.createDefaultConfig()
    
    def set(self, key, value, source=ConfigSource.Runtime):
        # Validate inputs
        if not isinstance(key, str) or not key:
            raise ValueError("Key must be a non-empty string")
        
        # Type-specific validation
        if isinstance(value, str):
            self._config.set(key, value, source)
        elif isinstance(value, int):
            self._config.set(key, value, source)
        elif isinstance(value, float):
            self._config.set(key, value, source)
        elif isinstance(value, bool):
            self._config.set(key, value, source)
        else:
            raise TypeError(f"Unsupported value type: {type(value)}")
    
    def get_int(self, key, default=None):
        """Get integer value with type safety"""
        try:
            result = self._config.getInt(key)
            return result if result is not None else default
        except Exception as e:
            raise ConfigurationError(f"Failed to get integer from config: {e}")
    
    def get_float(self, key, default=None):
        """Get float value with type safety"""
        try:
            result = self._config.getFloat(key)
            return result if result is not None else default
        except Exception as e:
            raise ConfigurationError(f"Failed to get float from config: {e}")
    
    def get_string(self, key, default=None):
        """Get string value with type safety"""
        try:
            result = self._config.get(key)
            return result if result is not None else default
        except Exception as e:
            raise ConfigurationError(f"Failed to get string from config: {e}")
```

### 2. Enhanced Sensory Data Access

**Add numpy support for vision data:**
```python
class Vision:
    def __init__(self, width, height, channels=3):
        # Create vision object using pynlm
        pass
    
    def set_data_numpy(self, data):
        """
        Set vision data from numpy array.
        
        Args:
            data: numpy array with shape (height, width, channels) or 
                  (height, width) for grayscale
                  
        Raises:
            ValueError: If data shape is invalid
            TypeError: If data is not a numpy array
        """
        if not isinstance(data, np.ndarray):
            raise TypeError("Data must be a numpy array")
        
        if data.ndim not in [2, 3]:
            raise ValueError(f"Vision data must be 2D or 3D, got {data.ndim}D")
        
        height, width = data.shape[:2]
        if len(data.shape) == 3:
            channels = data.shape[2]
        else:
            channels = 1
        
        # Update object dimensions
        self._vision.configure(width, height, channels)
        
        # Convert to flat vector for C++ interface
        if data.ndim == 3:
            flat_data = data.transpose(2, 0, 1).flatten().tolist()
        else:
            flat_data = data.flatten().tolist()
        
        self._vision.setData(flat_data)
    
    def get_data_numpy(self, as_grayscale=False):
        """
        Get vision data as numpy array.
        
        Args:
            as_grayscale: If True and channels=3, convert to grayscale
            
        Returns:
            numpy array with shape (height, width, channels) or (height, width)
        """
        height = self._vision.getHeight()
        width = self._vision.getWidth()
        channels = self._vision.getChannels()
        
        # Get raw data from C++
        raw_data = self._vision.getData()
        
        # Reshape to numpy array
        if channels == 1:
            data = np.array(raw_data).reshape(height, width)
        else:
            data = np.array(raw_data).reshape(channels, height, width).transpose(1, 2, 0)
        
        if as_grayscale and channels == 3:
            # Convert RGB to grayscale
            data = np.dot(data[..., :3], [0.299, 0.587, 0.114]).reshape(height, width)
        
        return data
```

**Add high-level vision utilities:**
```python
def process_vision_data(visual_data, operation='normalize'):
    """
    High-level vision data processing.
    
    Args:
        visual_data: Vision data as numpy array
        operation: Type of processing ('normalize', 'threshold', 'edges', 'blur')
        
    Returns:
        Processed vision data
    """
    if operation == 'normalize':
        # Normalize to [0, 1] range
        return np.clip(visual_data / 255.0, 0, 1)
    
    elif operation == 'threshold':
        # Simple thresholding
        return (visual_data > 0.5).astype(float)
    
    elif operation == 'edges':
        # Simple edge detection using Sobel filter
        if visual_data.ndim == 3:
            gray = np.dot(visual_data[..., :3], [0.299, 0.587, 0.114])
        else:
            gray = visual_data
        
        # Simple Sobel edge detection
        sobel_x = np.array([[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]])
        sobel_y = np.array([[-1, -2, -1], [0, 0, 0], [1, 2, 1]])
        
        edges_x = scipy.signal.convolve2d(gray, sobel_x, mode='same')
        edges_y = scipy.signal.convolve2d(gray, sobel_y, mode='same')
        
        return np.sqrt(edges_x**2 + edges_y**2)
    
    elif operation == 'blur':
        # Simple blur using averaging filter
        kernel = np.ones((3, 3)) / 9
        blurred = scipy.signal.convolve2d(visual_data, kernel, mode='same')
        return blurred
    
    else:
        raise ValueError(f"Unknown operation: {operation}")
```

### 3. Convenience Methods and Workflows

**Add higher-level API methods:**
```python
class BrainSimulation:
    """
    High-level interface for brain simulation with sensible defaults.
    Provides convenient methods for typical use cases.
    """
    
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        # Set reasonable defaults for quick experiments
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        self._config = config
        self._brain = pynlm.createBrain(config)
        self._brain.initialize()
    
    def run_episode(self, duration=1000):
        """
        Run a complete simulation episode.
        
        Args:
            duration: Number of simulation steps
            
        Returns:
            Simulation statistics
        """
        stats = {
            'spike_count': self._brain.getTotalSpikeCount(),
            'active_neurons': self._brain.getFiringNeuronCount(),
            'average_rate': self._brain.getAverageFiringRate(),
            'steps': duration
        }
        
        for step in range(duration):
            self._brain.step(step)
        
        return stats
    
    def run_with_world(self, world_config=None, enable_learning=True):
        """
        Run simulation with world interaction and learning.
        
        Args:
            world_config: Configuration for the world
            enable_learning: Whether to enable plasticity systems
            
        Returns:
            Agent performance metrics
        """
        # Create world
        world = pynlm.createSimpleWorld()
        if world_config:
            world.configure(**world_config)
        
        # Create agent
        agent = pynlm.createAgentBrain(self._brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        if enable_learning:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(1000):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            self._brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Return performance metrics
        return {
            'spike_count': self._brain.getTotalSpikeCount(),
            'learning_rate': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'episodes_completed': 1
        }
```

### 4. Advanced Configuration Builder

**Add configuration builder for common scenarios:**
```python
class BrainConfigBuilder:
    """
    Builder for creating common brain configurations.
    Simplifies configuration for typical use cases.
    """
    
    @staticmethod
    def create_learning_brain(neuron_count=500, enable_curiosity=True):
        """
        Create a brain configuration optimized for learning.
        
        Args:
            neuron_count: Number of neurons in the brain
            enable_curiosity: Whether to enable curiosity-driven exploration
            
        Returns:
            Configured brain object
        """
        config = pynlm.createDefaultConfig()
        
        # Core brain parameters
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.connection_probability", 0.05)
        
        # Learning parameters
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        config.set("plasticity.structural.enable", True)
        
        # Neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", enable_curiosity)
        config.set("neuromod.novelty.enable", True)
        
        # Development
        config.set("development.enable", True)
        config.set("development.critical_period_duration", 1000)
        
        # Memory
        config.set("memory.working_memory.capacity", neuron_count // 10)
        config.set("memory.episodic.capacity", 1000)
        
        return config
    
    @staticmethod
    def create_exploratory_brain(neuron_count=300):
        """
        Create a brain configuration optimized for exploration.
        
        Args:
            neuron_count: Number of neurons in the brain
            
        Returns:
            Configured brain object
        """
        config = BrainConfigBuilder.create_learning_brain(neuron_count, enable_curiosity=True)
        
        # Boost exploration
        config.set("neuromod.curiosity.strength", 2.0)
        config.set("neuromod.novelty.sensitivity", 0.5)
        
        # Reduce stability for more exploration
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("development.maturation_rate", 0.001)
        
        return config
    
    @staticmethod
    def create_stable_brain(neuron_count=1000):
        """
        Create a brain configuration optimized for stability.
        
        Args:
            neuron_count: Number of neurons in the brain
            
        Returns:
            Configured brain object
        """
        config = BrainConfigBuilder.create_learning_brain(neuron_count, enable_curiosity=False)
        
        # Boost stability
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.stabilization_factor", 0.9)
        
        # Reduce learning rates
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.learning_rate", 0.005)
        config.set("development.maturation_rate", 0.01)
        
        return config
```

### 5. Example and Tutorial System

**Add example utilities for common patterns:**
```python
class NLMSamples:
    """
    Sample code and examples for common NLM usage patterns.
    Provides ready-to-run examples for various scenarios.
    """
    
    @staticmethod
    def simple_learning_example():
        """
        Simple example showing basic brain learning.
        Run this to see a brain learning to respond to stimuli.
        """
        import pynlm
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Inject different stimuli
        stimuli = [
            lambda b: b.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f),  # Reward
            lambda b: b.injectCurrentToNeurons(pynlm.NeuronType.Sensory, -5.0f),  # Penalty
            lambda b: b.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.0f),   # Neutral
        ]
        
        # Learn associations
        for step in range(100):
            stimuli[step % 3](brain)
            brain.step(step)
            
            # Print activity
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        print("Learning complete!")
    
    @staticmethod
    def agent_world_example():
        """
        Example showing agent-world interaction.
        Demonstrates how an agent can interact with and learn from a world.
        """
        import pynlm
        import numpy as np
        
        # Setup agent-world system
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run episode
        print("Running agent-world episode...")
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Agent acts
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Provide reward
            if step % 20 == 0:
                reward = 1.0 if step % 40 == 0 else 0.0  # Random reward
                agent.applyRewardModulation(reward, 0.0)
        
        # Print final statistics
        print(f"Episode completed:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    @staticmethod
    def memory_demo_example():
        """
        Example demonstrating memory system usage.
        Shows how working memory and episodic memory work.
        """
        import pynlm
        
        # Create brain with memory systems
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        if working_memory and episodic_memory:
            print("Memory systems available")
            
            # Store some information in working memory
            for i in range(10):
                # Inject some activity
                brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 5.0f)
                brain.step(i)
                
                # Store in working memory
                working_memory.storeToNeuron(brain.getRegions()[0].getAllNeurons()[i], 
                                           brain.getFiringRate())
            
            print(f"Working memory active traces: {working_memory.getActiveTraces()}")
            
            # Store an episode
            episode = nlm.EpisodicMemoryItem()
            episode.timestamp = 100
            episode.reward = 0.5
            episode.metadata = "Demo episode"
            
            episodic_memory.storeEpisode(episode)
            
            print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
        else:
            print("Warning: Memory systems not available")
```

### 6. Performance Monitoring and Debugging

**Add performance monitoring utilities:**
```python
class NLMBenchmark:
    """
    Performance monitoring and benchmarking for NLM simulations.
    Tracks performance metrics and provides analysis tools.
    """
    
    def __init__(self):
        self.metrics = {
            'steps': 0,
            'spikes': 0,
            'memory_usage': [],
            'cpu_time': [],
            'wall_time': [],
            'custom_metrics': {}
        }
        self.start_time = time.time()
    
    def record_step(self, brain, world=None):
        """Record metrics for current simulation step"""
        self.metrics['steps'] += 1
        self.metrics['spikes'] += brain.getTotalSpikeCount()
        
        if world:
            self.metrics['memory_usage'].append(get_memory_usage())
        
        current_time = time.time()
        self.metrics['cpu_time'].append(get_cpu_time())
        self.metrics['wall_time'].append(current_time - self.start_time)
    
    def add_custom_metric(self, name, value):
        """Add a custom performance metric"""
        self.metrics['custom_metrics'][name] = value
    
    def get_performance_report(self):
        """Generate performance report"""
        if self.metrics['steps'] == 0:
            return "No performance data available"
        
        report = {
            'simulation_duration': self.metrics['wall_time'][-1],
            'total_steps': self.metrics['steps'],
            'total_spikes': self.metrics['spikes'],
            'spike_rate': self.metrics['spikes'] / max(self.metrics['steps'], 1),
            'average_cpu_usage': np.mean(self.metrics['cpu_time']) if self.metrics['cpu_time'] else 0,
            'memory_usage': {
                'max': max(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
                'min': min(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
                'avg': np.mean(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0
            }
        }
        
        # Add custom metrics
        report['custom_metrics'] = self.metrics['custom_metrics']
        
        return report
    
    def print_performance_summary(self):
        """Print concise performance summary"""
        report = self.get_performance_report()
        
        print("=== NLM Performance Summary ===")
        print(f"Duration: {report['simulation_duration']:.2f} seconds")
        print(f"Steps: {report['total_steps']}")
        print(f"Spikes: {report['total_spikes']}")
        print(f"Spike rate: {report['spike_rate']:.2f} spikes/step")
        print(f"Average CPU usage: {report['average_cpu_usage']:.3f}")
        print(f"Memory usage - Max: {report['memory_usage']['max']:.1f} MB, "
              f"Avg: {report['memory_usage']['avg']:.1f} MB")
        print("==============================")
```

### 7. Configuration File Utilities

**Add utilities for configuration file management:**
```python
class ConfigManager:
    """
    Configuration file management for NLM.
    Provides utilities for loading, saving, and managing configuration files.
    """
    
    def __init__(self, config_dir="./configs"):
        self.config_dir = config_dir
        os.makedirs(config_dir, exist_ok=True)
    
    def save_config(self, config, name, format='json'):
        """
        Save configuration to file.
        
        Args:
            config: Configuration object to save
            name: Name for the configuration file
            format: Format ('json', 'yaml', 'ini', 'txt')
            
        Returns:
            Path to saved file
        """
        if format == 'json':
            import json
            config_data = self._config_to_dict(config)
            filepath = os.path.join(self.config_dir, f"{name}.json")
            
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
                
        elif format == 'yaml':
            import yaml
            config_data = self._config_to_dict(config)
            filepath = os.path.join(self.config_dir, f"{name}.yaml")
            
            with open(filepath, 'w') as f:
                yaml.dump(config_data, f)
                
        elif format == 'ini':
            import configparser
            config_obj = configparser.ConfigParser()
            config_data = self._config_to_dict(config)
            
            for section, values in config_data.items():
                config_obj.add_section(section)
                for key, value in values.items():
                    config_obj.set(section, key, str(value))
            
            filepath = os.path.join(self.config_dir, f"{name}.ini")
            config_obj.write(filepath)
            
        elif format == 'txt':
            # Simple key=value format
            filepath = os.path.join(self.config_dir, f"{name}.txt")
            
            with open(filepath, 'w') as f:
                config_data = self._config_to_dict(config)
                for section, values in config_data.items():
                    f.write(f"[{section}]\n")
                    for key, value in values.items():
                        f.write(f"  {key} = {value}\n")
                    f.write("\n")
        
        return filepath
    
    def load_config(self, name, format='json'):
        """
        Load configuration from file.
        
        Args:
            name: Name of configuration file (without extension)
            format: Format ('json', 'yaml', 'ini', 'txt')
            
        Returns:
            Configuration object
        """
        if format == 'json':
            import json
            filepath = os.path.join(self.config_dir, f"{name}.json")
            
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            return self._dict_to_config(config_data)
            
        elif format == 'yaml':
            import yaml
            filepath = os.path.join(self.config_dir, f"{name}.yaml")
            
            with open(filepath, 'r') as f:
                config_data = yaml.safe_load(f)
            
            return self._dict_to_config(config_data)
            
        elif format == 'ini':
            import configparser
            filepath = os.path.join(self.config_dir, f"{name}.ini")
            
            config_obj = configparser.ConfigParser()
            config_obj.read(filepath)
            
            # Convert to our config format
            config_data = {}
            for section in config_obj.sections():
                config_data[section] = {}
                for key in config_obj.options(section):
                    config_data[section][key] = config_obj.get(section, key)
            
            return self._dict_to_config(config_data)
            
        elif format == 'txt':
            filepath = os.path.join(self.config_dir, f"{name}.txt")
            
            with open(filepath, 'r') as f:
                config_data = {}
                current_section = None
                
                for line in f:
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    
                    if line.startswith('[') and line.endswith(']'):
                        current_section = line[1:-1]
                        config_data[current_section] = {}
                    elif '=' in line and current_section:
                        key, value = line.split('=', 1)
                        config_data[current_section][key.strip()] = value.strip()
            
            return self._dict_to_config(config_data)
    
    def list_configs(self):
        """List available configuration files"""
        files = os.listdir(self.config_dir)
        
        configs = []
        for file in files:
            if file.endswith('.json') or file.endswith('.yaml') or file.endswith('.ini') or file.endswith('.txt'):
                config_name = os.path.splitext(file)[0]
                configs.append({
                    'name': config_name,
                    'format': file.split('.')[-1],
                    'path': os.path.join(self.config_dir, file)
                })
        
        return configs
    
    def _config_to_dict(self, config):
        """Convert config object to dictionary"""
        # Implementation depends on config object structure
        # This is a placeholder - actual implementation would need to
        # iterate through config entries and convert to dict
        pass
    
    def _dict_to_config(self, config_data):
        """Convert dictionary to config object"""
        # Implementation depends on config object structure
        # This is a placeholder - actual implementation would need to
        # convert dictionary back to config object
        pass
```

### 8. Advanced Debugging Tools

**Add debugging and visualization utilities:**
```python
class NLMVisualizer:
    """
    Visualization tools for NLM debugging and analysis.
    Provides utilities for visualizing brain state and simulation results.
    """
    
    @staticmethod
    def plot_brain_state(brain, title="Brain State"):
        """
        Plot brain state as a histogram or visualization.
        
        Args:
            brain: Brain object to visualize
            title: Title for the plot
        """
        import matplotlib.pyplot as plt
        
        # Get neuron statistics
        total_neurons = brain.getTotalNeuronCount()
        active_neurons = brain.getFiringNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        
        # Create histogram
        plt.figure(figsize=(8, 6))
        
        # Plot 1: Neuron count
        plt.subplot(2, 2, 1)
        plt.bar(['Total Neurons', 'Active Neurons'], 
               [total_neurons, active_neurons], 
               color=['blue', 'red'])
        plt.title('Neuron Count')
        plt.ylabel('Count')
        
        # Plot 2: Synapse count
        plt.subplot(2, 2, 2)
        plt.bar(['Total Synapses'], [total_synapses], color='green')
        plt.title('Synapse Count')
        plt.ylabel('Count')
        
        # Plot 3: Firing rate distribution
        plt.subplot(2, 2, 3)
        # This would need access to individual neuron firing rates
        # For now, just show average
        avg_rate = brain.getAverageFiringRate()
        plt.bar(['Average Firing Rate'], [avg_rate], color='orange')
        plt.title('Firing Rate')
        plt.ylabel('Hz')
        
        # Plot 4: Regional information
        plt.subplot(2, 2, 4)
        region_ids = brain.getRegionIds()
        region_counts = [brain.getTotalNeuronCount() // len(region_ids)] * len(region_ids)
        
        plt.bar([f'Region {rid}' for rid in region_ids], region_counts)
        plt.title('Regional Distribution')
        plt.ylabel('Neuron Count')
        
        plt.suptitle(title)
        plt.tight_layout()
        plt.show()
    
    @staticmethod
    def visualize_simulation_log(steps, brain_states):
        """
        Visualize simulation over time.
        
        Args:
            steps: Number of simulation steps
            brain_states: List of brain state snapshots
        """
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle("Simulation Visualization")
        
        # Prepare data
        time_points = list(range(steps))
        
        # Plot 1: Spike count over time
        spike_counts = [state['total_spikes'] for state in brain_states]
        axes[0, 0].plot(time_points, spike_counts)
        axes[0, 0].set_title('Total Spike Count')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Spike Count')
        
        # Plot 2: Active neuron count over time
        active_counts = [state['active_neurons'] for state in brain_states]
        axes[0, 1].plot(time_points, active_counts)
        axes[0, 1].set_title('Active Neuron Count')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Neuron Count')
        
        # Plot 3: Average firing rate over time
        avg_rates = [state['average_rate'] for state in brain_states]
        axes[0, 2].plot(time_points, avg_rates)
        axes[0, 2].set_title('Average Firing Rate')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Hz')
        
        # Plot 4: Memory usage over time
        memory_usages = [state['memory_usage'] for state in brain_states]
        axes[1, 0].plot(time_points, memory_usages)
        axes[1, 0].set_title('Memory Usage')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('MB')
        
        # Plot 5: Neuromodulation levels
        neuromod_levels = [state['neuromodulation_level'] for state in brain_states]
        axes[1, 1].plot(time_points, neuromod_levels)
        axes[1, 1].set_title('Neuromodulation Level')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Level')
        
        # Plot 6: Curiosity levels
        curiosity_levels = [state['curiosity_level'] for state in brain_states]
        axes[1, 2].plot(time_points, curiosity_levels)
        axes[1, 2].set_title('Curiosity Level')
        axes[1, 2].set_xlabel('Step')
        axes[1, 2].set_ylabel('Level')
        
        plt.tight_layout()
        plt.show()
    
    @staticmethod
    def create_interactive_brain_viewer(brain):
        """
        Create an interactive brain viewer.
        
        Args:
            brain: Brain object to visualize
        """
        try:
            import ipywidgets as widgets
            import ipyvolume as ipv
            
            # Create widgets for brain state visualization
            neuron_count_slider = widgets.IntSlider(
                value=10,
                min=5,
                max=100,
                description='Show Top '
            )
            
            firing_threshold_slider = widgets.FloatSlider(
                value=0.5,
                min(0.0, max=1.0),
                description='Firing Threshold'
            )
            
            # Create display
            display(widgets.VBox([
                widgets.HTML('<h3>Interactive Brain Viewer</h3>'),
                neuron_count_slider,
                firing_threshold_slider
            ]))
            
        except ImportError:
            # Handle case where ipywidgets is not available
            print("IPython widgets not available - using matplotlib visualization")
```

## Usage Examples

### Example 1: Creating and Running a Brain
```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Print statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 2: Agent-World Simulation
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Episode completed!")
```

### Example 3: Configuration Builder
```python
import pynlm

# Create learning brain configuration
config = pynlm.BrainConfigBuilder.create_learning_brain(
    neuron_count=500,
    enable_curiosity=True
)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.run_simulation(1000)
```

### Example 4: Memory System Access
```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

if working_memory and episodic_memory:
    # Store information in working memory
    working_memory.storeToNeuron(1, 0.5)
    
    # Create and store episode
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = 100
    episode.reward = 0.8
    
    episodic_memory.storeEpisode(episode)
    
    print(f"Working memory active traces: {working_memory.getActiveTraces()}")
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
```

### Example 5: Performance Monitoring
```python
import pynlm
import time

# Create benchmark
benchmark = pynlm.NLMBenchmark()

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    benchmark.record_step(brain)

# Print performance report
benchmark.print_performance_summary()
```

## Summary

These Python API enhancements provide:

1. **Better Type Safety**: Python type checking and validation
2. **Comprehensive Error Handling**: Python exceptions for C++ errors
3. **Convenience Methods**: High-level API for common operations
4. **Advanced Features**: Configuration builders, visualization, debugging tools
5. **Performance Monitoring**: Benchmarking and profiling capabilities
6. **Documentation and Examples**: Comprehensive examples and tutorials

The enhanced Python API makes NLM more accessible to researchers and developers while providing power users with advanced tools for complex simulations and analysis.