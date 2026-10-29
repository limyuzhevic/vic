# NLM Advanced Usage Guide

## Overview

This guide covers advanced topics for experienced NLM users who want to get the most out of the neural simulation framework. It builds upon the basic concepts and introduces sophisticated techniques, performance optimizations, and expert-level features.

## Advanced Configuration Management

### Configuration Schemas

For complex simulations, you might want to use configuration schemas that validate and organize your settings:

```python
import pynlm
import json
import os

class BrainConfiguration:
    """Configuration schema for brain simulations."""
    
    DEFAULT_CONFIG = {
        "brain": {
            "neuron_count": 1000,
            "region_count": 2,
            "synapse_density": 0.1,
            "connection_probability": 0.05,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "v_reset": -75.0,
            "tau_mem": 20.0,
            "tau_ref": 2.0
        },
        "plasticity": {
            "stdp": {
                "enabled": True,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enabled": True,
                "learning_rate": 0.001
            },
            "structural": {
                "enabled": True,
                "growth_rate": 0.01
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0
            },
            "curiosity": {
                "enabled": True,
                "exploration_rate": 0.1
            },
            "novelty": {
                "enabled": True,
                "detection_threshold": 0.5
            }
        },
        "memory": {
            "working_memory_size": 100,
            "episodic_capacity": 1000,
            "associative_strength": 0.5
        },
        "prediction": {
            "prediction_horizon": 10,
            "prediction_error_weight": 0.5
        },
        "development": {
            "enable": True,
            "critical_period": 100,
            "plasticity_modifier": 1.0
        },
        "world": {
            "width": 20,
            "height": 20,
            "vision_width": 8,
            "vision_height": 8,
            "max_energy": 100.0
        }
    }
    
    @staticmethod
    def create_config(config_dict=None):
        """Create a validated configuration from a dictionary."""
        config = pynlm.createDefaultConfig()
        
        if config_dict is None:
            config_dict = BrainConfiguration.DEFAULT_CONFIG
        
        # Flatten configuration
        for section, settings in config_dict.items():
            for key, value in settings.items():
                config.set(f"{section}.{key}", value)
        
        return config
    
    @staticmethod
    def load_config_from_file(filepath):
        """Load configuration from JSON file with validation."""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        return BrainConfiguration.create_config(config_dict)
    
    @staticmethod
    def save_config_to_file(config, filepath):
        """Save configuration to JSON file."""
        # Extract configuration by sections
        config_dict = {}
        
        keys = config.getKeys()
        for key in keys:
            # Parse section.key format
            if '.' in key:
                section, subkey = key.split('.', 1)
                if section not in config_dict:
                    config_dict[section] = {}
                
                # Convert value to appropriate JSON type
                value = config.get(key)
                config_dict[section][subkey] = value
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        return True

# Example usage
if __name__ == "__main__":
    # Create custom configuration
    config = BrainConfiguration.create_config({
        "brain": {
            "neuron_count": 2000,
            "synapse_density": 0.15
        },
        "world": {
            "width": 30,
            "height": 30
        }
    })
    
    # Save configuration
    BrainConfiguration.save_config_to_file(config, "my_brain_config.json")
    
    # Load configuration
    loaded_config = BrainConfiguration.load_config_from_file("my_brain_config.json")
```

### Advanced Configuration Validation

```python
import pynlm
import re

class ConfigurationValidator:
    """Advanced configuration validation with custom rules."""
    
    # Configuration schemas
    BRAIN_CONFIG_SCHEMA = {
        "neuron_count": {"type": "int", "min": 100, "max": 10000},
        "region_count": {"type": "int", "min": 1, "max": 100},
        "synapse_density": {"type": "float", "min": 0.0, "max": 1.0},
        "connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
        "v_thresh": {"type": "float", "min": -100.0, "max": 0.0},
        "v_rest": {"type": "float", "min": -100.0, "max": 0.0},
        "v_reset": {"type": "float", "min": -100.0, "max": 0.0},
        "tau_mem": {"type": "float", "min": 1.0, "max": 100.0},
        "tau_ref": {"type": "float", "min": 0.1, "max": 10.0}
    }
    
    PLASTICITY_SCHEMA = {
        "stdp.enabled": {"type": "bool"},
        "stdp.learning_rate": {"type": "float", "min": 0.0, "max": 1.0},
        "stdp.tau_plus": {"type": "float", "min": 1.0, "max": 100.0},
        "stdp.tau_minus": {"type": "float", "min": 1.0, "max": 100.0},
        "hebbian.enabled": {"type": "bool"},
        "hebbian.learning_rate": {"type": "float", "min": 0.0, "max": 1.0},
        "structural.enabled": {"type": "bool"},
        "structural.growth_rate": {"type": "float", "min": 0.0, "max": 1.0}
    }
    
    NEUROMODULATION_SCHEMA = {
        "neuromod.dopamine.scale": {"type": "float", "min": 0.0, "max": 10.0},
        "neuromod.curiosity.enabled": {"type": "bool"},
        "neuromod.curiosity.exploration_rate": {"type": "float", "min": 0.0, "max": 1.0},
        "neuromod.novelty.enabled": {"type": "bool"},
        "neuromod.novelty.detection_threshold": {"type": "float", "min": 0.0, "max": 1.0}
    }
    
    def __init__(self, config):
        self.config = config
    
    def validate_brain_config(self):
        """Validate brain configuration parameters."""
        errors = []
        
        for key, schema in self.BRAIN_CONFIG_SCHEMA.items():
            full_key = f"brain.{key}"
            if self.config.has(full_key):
                value = self.config.get(full_key, schema["type"])
                if value is None:
                    errors.append(f"Invalid type for {full_key}")
                
                # Check range constraints
                if schema.get("min") is not None and value < schema["min"]:
                    errors.append(f"{full_key} value {value} is below minimum {schema['min']}")
                
                if schema.get("max") is not None and value > schema["max"]:
                    errors.append(f"{full_key} value {value} is above maximum {schema['max']}")
        
        # Check for required relationships
        if self.config.has("brain.neuron_count") and self.config.has("brain.region_count"):
            neurons = self.config.get("brain.neuron_count", "int")
            regions = self.config.get("brain.region_count", "int")
            if neurons > 0 and regions > neurons:
                errors.append("Region count cannot exceed neuron count")
        
        return errors
    
    def validate_plasticity_config(self):
        """Validate plasticity configuration."""
        errors = []
        
        for key, schema in self.PLASTICITY_SCHEMA.items():
            if self.config.has(key):
                value = self.config.get(key.split('.')[0], schema["type"])
                if value is None:
                    errors.append(f"Invalid type for {key}")
                
                # Check range constraints
                if schema.get("min") is not None and value < schema["min"]:
                    errors.append(f"{key} value {value} is below minimum {schema['min']}")
                
                if schema.get("max") is not None and value > schema["max"]:
                    errors.append(f"{key} value {value} is above maximum {schema['max']}")
        
        return errors
    
    def validate_neuromodulation_config(self):
        """Validate neuromodulation configuration."""
        errors = []
        
        for key, schema in self.NEUROMODULATION_SCHEMA.items():
            if self.config.has(key):
                value = self.config.get(key.split('.')[0], schema["type"])
                if value is None:
                    errors.append(f"Invalid type for {key}")
                
                # Check range constraints
                if schema.get("min") is not None and value < schema["min"]:
                    errors.append(f"{key} value {value} is below minimum {schema['min']}")
                
                if schema.get("max") is not None and value > schema["max"]:
                    errors.append(f"{key} value {value} is above maximum {schema['max']}")
        
        return errors
    
    def validate(self):
        """Run all validation checks."""
        errors = []
        
        errors.extend(self.validate_brain_config())
        errors.extend(self.validate_plasticity_config())
        errors.extend(self.validate_neuromodulation_config())
        
        return errors
    
    def is_valid(self):
        """Check if configuration is valid."""
        return len(self.validate()) == 0

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 5)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enabled", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.curiosity.enabled", True)
    
    # Validate configuration
    validator = ConfigurationValidator(config)
    
    if validator.is_valid():
        print("Configuration is valid!")
    else:
        errors = validator.validate()
        print("Configuration errors:")
        for error in errors:
            print(f"  - {error}")
```

### Configuration Templates

```python
import pynlm

class ConfigurationTemplates:
    """Pre-built configuration templates for different use cases."""
    
    @staticmethod
    def create_research_brain(neuron_count=1000, learning_rate=0.001):
        """Create a brain configuration for research purposes."""
        config = pynlm.createDefaultConfig()
        
        # Research-oriented settings
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 3)
        config.set("brain.synapse_density", 0.1)
        
        # Advanced learning mechanisms
        config.set("plasticity.stdp.enabled", True)
        config.set("plasticity.stdp.learning_rate", learning_rate)
        config.set("plasticity.hebbian.enabled", True)
        config.set("plasticity.structural.enabled", True)
        
        # Comprehensive neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enabled", True)
        config.set("neuromod.curiosity.exploration_rate", 0.2)
        config.set("neuromod.novelty.enabled", True)
        config.set("neuromod.novelty.detection_threshold", 0.3)
        
        # Memory systems
        config.set("memory.working_memory_size", 200)
        config.set("memory.episodic_capacity", 2000)
        config.set("memory.associative_strength", 0.7)
        
        # Development
        config.set("development.enable", True)
        config.set("development.critical_period", 200)
        
        return config
    
    @staticmethod
    def create_fast_simulation(neuron_count=500, learning_rate=0.01):
        """Create a brain configuration optimized for fast simulation."""
        config = pynlm.createDefaultConfig()
        
        # Smaller brain for faster simulation
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 1)
        config.set("brain.synapse_density", 0.05)
        
        # Simplified learning
        config.set("plasticity.stdp.enabled", False)  # Disable STDP for speed
        config.set("plasticity.hebbian.enabled", True)
        config.set("plasticity.hebbian.learning_rate", learning_rate)
        config.set("plasticity.structural.enabled", False)  # Disable structural plasticity
        
        # Minimal neuromodulation
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enabled", False)  # Disable curiosity
        config.set("neuromod.novelty.enabled", False)   # Disable novelty
        
        # No development
        config.set("development.enable", False)
        
        return config
    
    @staticmethod
    def create_developmental_brain(neuron_count=2000, developmental_period=500):
        """Create a brain with extended developmental period."""
        config = pynlm.createDefaultConfig()
        
        # Larger brain
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 5)
        config.set("brain.synapse_density", 0.2)
        
        # Standard learning
        config.set("plasticity.stdp.enabled", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enabled", True)
        config.set("plasticity.structural.enabled", True)
        
        # Active neuromodulation
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enabled", True)
        config.set("neuromod.curiosity.exploration_rate", 0.3)
        config.set("neuromod.novelty.enabled", True)
        
        # Extended development
        config.set("development.enable", True)
        config.set("development.critical_period", developmental_period)
        config.set("development.plasticity_modifier", 2.0)
        
        return config

# Example usage
if __name__ == "__main__":
    # Create different types of brains
    research_brain = ConfigurationTemplates.create_research_brain(1500, 0.002)
    print("Research brain created with 1500 neurons")
    
    fast_brain = ConfigurationTemplates.create_fast_simulation(500, 0.02)
    print("Fast simulation brain created with 500 neurons")
    
    developmental_brain = ConfigurationTemplates.create_developmental_brain(2000, 800)
    print("Developmental brain created with 2000 neurons and 800-day critical period")
```

## Advanced Simulation Techniques

### Custom Time Integration Methods

```python
import pynlm
import numpy as np

class CustomTimeIntegration:
    """Custom time integration methods for advanced simulations."""
    
    @staticmethod
    def euler_step(brain, dt):
        """Custom Euler integration step."""
        # Custom implementation of Euler integration
        # This is an example - actual implementation would depend on brain internals
        for step in range(int(dt * 1000)):  # 1ms resolution
            brain.step(step)
        
        return True
    
    @staticmethod
    def runge_kutta_step(brain, dt):
        """Custom Runge-Kutta integration step."""
        # Custom implementation of Runge-Kutta integration
        # This would require access to brain's internal dynamics
        for step in range(int(dt * 1000)):
            brain.step(step)
        
        return True

class AdaptiveTimeSimulation:
    """Simulation with adaptive time steps."""
    
    def __init__(self, brain, max_step=0.1, min_step=0.001):
        self.brain = brain
        self.max_step = max_step
        self.min_step = min_step
        self.current_time = 0.0
        self.step_count = 0
    
    def adaptive_step(self):
        """Perform adaptive time step based on neural activity."""
        # Get current neural activity metrics
        firing_rate = self.brain.getAverageFiringRate()
        active_neurons = self.brain.getFiringNeuronCount()
        
        # Determine appropriate time step
        if firing_rate > 50.0 or active_neurons > 100:
            # High activity - use smaller time step
            dt = self.min_step
        elif firing_rate < 5.0 or active_neurons < 10:
            # Low activity - can use larger time step
            dt = self.max_step
        else:
            # Moderate activity - use default
            dt = 0.01
        
        # Perform simulation step
        self.brain.step(self.step_count, self.current_time)
        self.current_time += dt
        self.step_count += 1
        
        return dt

# Example usage
if __name__ == "__main__":
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create adaptive simulation
    sim = AdaptiveTimeSimulation(brain)
    
    # Run adaptive simulation
    for i in range(1000):
        dt = sim.adaptive_step()
        if i % 100 == 0:
            print(f"Step {i}: time={sim.current_time:.2f}, dt={dt:.4f}")
```

### Parallel Simulation

```python
import pynlm
import threading
import queue
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

class ParallelSimulation:
    """Parallel simulation using multiple threads or processes."""
    
    def __init__(self, config, num_brains):
        self.brains = []
        for i in range(num_brains):
            # Create brain with slight variations
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.neuron_count", config.get("brain.neuron_count", "int") // num_brains)
            brain_config.set("brain.region_count", config.get("brain.region_count", "int"))
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            self.brains.append(brain)
    
    def run_threads(self, steps_per_brain):
        """Run simulation using threads."""
        results = []
        
        def simulate_brain(brain, brain_id):
            """Simulate single brain."""
            spike_counts = []
            for step in range(steps_per_brain):
                brain.step(step)
                spike_counts.append(brain.getTotalSpikeCount())
            
            return brain_id, spike_counts
        
        with ThreadPoolExecutor(max_workers=len(self.brains)) as executor:
            futures = []
            for i, brain in enumerate(self.brains):
                future = executor.submit(simulate_brain, brain, i)
                futures.append(future)
            
            for future in futures:
                results.append(future.result())
        
        return results
    
    def run_processes(self, steps_per_brain):
        """Run simulation using separate processes."""
        # This would require pickling brains, which might not work with pynlm
        # For now, we'll use threading instead
        return self.run_threads(steps_per_brain)
    
    @staticmethod
    def merge_results(results):
        """Merge results from multiple parallel simulations."""
        merged = {
            'total_spikes': 0,
            'max_spikes_per_brain': [],
            'avg_spikes_per_brain': [],
            'final_spike_counts': []
        }
        
        for brain_id, spike_counts in results:
            merged['total_spikes'] += sum(spike_counts)
            merged['max_spikes_per_brain'].append(max(spike_counts))
            merged['avg_spikes_per_brain'].append(sum(spike_counts) / len(spike_counts))
            merged['final_spike_counts'].append(spike_counts[-1])
        
        return merged

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    
    # Create parallel simulation with 4 brains
    parallel_sim = ParallelSimulation(config, 4)
    
    # Run simulation
    results = parallel_sim.run_threads(100)
    
    # Merge results
    merged = ParallelSimulation.merge_results(results)
    
    print(f"Total spikes across all brains: {merged['total_spikes']}")
    print(f"Average spikes per brain: {np.mean(merged['avg_spikes_per_brain']):.2f}")
    print(f"Max spikes per brain: {merged['max_spikes_per_brain']}")
```

### Memory-Efficient Simulation

```python
import pynlm
import gc
import psutil
import os

class MemoryEfficientSimulation:
    """Memory-efficient simulation with checkpointing and cleanup."""
    
    def __init__(self, config, checkpoint_interval=1000):
        self.config = config
        self.checkpoint_interval = checkpoint_interval
        self.sim_step = 0
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
    
    def run_efficient_simulation(self, total_steps):
        """Run simulation with memory management."""
        print(f"Starting simulation with {total_steps} steps")
        
        # Memory monitoring
        process = psutil.Process(os.getpid())
        
        for step in range(total_steps):
            # Run simulation step
            self.brain.step(step)
            
            # Checkpoint periodically
            if step > 0 and step % self.checkpoint_interval == 0:
                self.checkpoint(f"checkpoint_step_{step}")
                self.cleanup_memory()
            
            # Monitor memory usage
            if step % 500 == 0:
                memory_mb = process.memory_info().rss / 1024 / 1024
                print(f"Step {step}: Memory usage = {memory_mb:.2f} MB")
        
        return self.brain
    
    def checkpoint(self, filepath):
        """Save brain state to checkpoint file."""
        try:
            self.brain.save(filepath)
            print(f"Checkpoint saved to {filepath}")
            return True
        except Exception as e:
            print(f"Failed to save checkpoint: {e}")
            return False
    
    def cleanup_memory(self):
        """Force garbage collection and cleanup."""
        # Force garbage collection
        gc.collect()
        
        # Clear any large temporary objects
        # (This is framework-specific)
        
        print("Memory cleanup completed")
    
    def load_checkpoint(self, filepath):
        """Load brain state from checkpoint."""
        try:
            # Create new brain
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            
            # Load saved state
            new_brain.load(filepath)
            
            print(f"Checkpoint loaded from {filepath}")
            return new_brain
        except Exception as e:
            print(f"Failed to load checkpoint: {e}")
            return None

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    # Create memory-efficient simulation
    sim = MemoryEfficientSimulation(config, checkpoint_interval=2000)
    
    # Run long simulation
    brain = sim.run_efficient_simulation(10000)
    
    # Save final state
    sim.checkpoint("final_simulation.bin")
    
    print("Simulation completed successfully")
```

### Advanced Sensor Processing

```python
import pynlm
import numpy as np
import time

class AdvancedSensorProcessing:
    """Advanced sensor processing with custom transformations."""
    
    def __init__(self, config):
        self.config = config
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=config.get("world.width", "int"),
            height=config.get("world.height", "int"),
            visionWidth=config.get("world.vision_width", "int"),
            visionHeight=config.get("world.vision_height", "int")
        )
    
    def create_custom_vision_sensor(self, data, preprocessing="normalize"):
        """Create custom vision sensor with preprocessing."""
        vision = pynlm.Vision(
            self.world.getWidth(),
            self.world.getHeight(),
            3  # RGB
        )
        
        # Apply preprocessing
        if preprocessing == "normalize":
            # Normalize data to [0, 1]
            data = np.array(data)
            data = (data - data.min()) / (data.max() - data.min() + 1e-8)
        elif preprocessing == "threshold":
            # Apply threshold
            data = np.array(data)
            data[data < 0.5] = 0
            data[data >= 0.5] = 1
        elif preprocessing == "histogram_equalization":
            # Apply histogram equalization
            data = np.array(data)
            # Simple histogram equalization
            hist, bins = np.histogram(data, bins=256, range=(0, 1))
            cdf = hist.cumsum()
            cdf = cdf / cdf[-1]
            # Map original data
            data = np.interp(data, bins[:-1], cdf)
        
        # Set data
        vision.setData(data.flatten().tolist())
        
        return vision
    
    def create_advanced_audio_sensor(self, audio_data, feature_extraction="mfcc"):
        """Create advanced audio sensor with feature extraction."""
        audio = pynlm.Audio(
            sampleRate=self.config.get("audio.sample_rate", "int"),
            numSamples=len(audio_data)
        )
        
        # Extract features based on method
        if feature_extraction == "mfcc":
            # MFCC (Mel-frequency cepstral coefficients)
            # This would require scipy - simplified version here
            mfcc = self.extract_mfcc(audio_data)
            audio.setData(mfcc)
        elif feature_extraction == "spectrogram":
            # Spectrogram features
            spec = self.extract_spectrogram(audio_data)
            audio.setData(spec)
        elif feature_extraction == "zero_crossing":
            # Zero-crossing rate
            zcr = self.calculate_zero_crossing_rate(audio_data)
            audio.setData([zcr])
        
        return audio
    
    def extract_mfcc(self, audio_data):
        """Extract MFCC features (simplified)."""
        # Simplified MFCC extraction
        # In practice, this would use librosa or similar
        return [np.mean(audio_data)]
    
    def extract_spectrogram(self, audio_data):
        """Extract spectrogram features (simplified)."""
        # Simplified spectrogram extraction
        return [np.std(audio_data)]
    
    def calculate_zero_crossing_rate(self, audio_data):
        """Calculate zero-crossing rate."""
        crossings = np.sum(np.abs(np.diff(np.sign(audio_data)))) / (2 * len(audio_data))
        return crossings
    
    def create_sensor_fusion(self, vision_data, audio_data, fusion_method="weighted"):
        """Combine multiple sensor inputs using fusion method."""
        # Create sensory percept
        percept = pynlm.SensoryPercept()
        
        # Add vision data
        if vision_data is not None:
            if isinstance(vision_data, pynlm.Vision):
                percept.setVision(vision_data)
            else:
                # Convert to vision
                vision = pynlm.Vision(
                    self.world.getWidth(),
                    self.world.getHeight(),
                    3
                )
                vision.setData(vision_data.flatten().tolist() if hasattr(vision_data, 'flatten') else vision_data)
                percept.setVision(vision)
        
        # Add audio data
        if audio_data is not None:
            if isinstance(audio_data, pynlm.Audio):
                percept.setAudio(audio_data)
            else:
                # Convert to audio
                audio = pynlm.Audio(
                    self.config.get("audio.sample_rate", "int"),
                    len(audio_data)
                )
                audio.setData(audio_data)
                percept.setAudio(audio)
        
        return percept
    
    def run_advanced_simulation(self, steps):
        """Run simulation with advanced sensor processing."""
        # Create brain
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        self.world.reset()
        agent.initialize(self.world)
        
        # Enable advanced learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation with sensor processing
        for step in range(steps):
            # Get raw sensor data
            raw_vision = np.random.rand(self.world.getWidth(), self.world.getHeight(), 3)
            raw_audio = np.random.randn(self.config.get("audio.sample_rate", "int"))
            
            # Apply preprocessing
            vision = self.create_custom_vision_sensor(raw_vision, "normalize")
            audio = self.create_advanced_audio_sensor(raw_audio, "mfcc")
            
            # Fuse sensors
            percept = self.create_sensor_fusion(vision, audio, "weighted")
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = np.random.uniform(0, 1)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
        
        return brain, agent, self.world

# Example usage
if __name__ == "__main__":
    # Create configuration with sensor parameters
    config = pynlm.createDefaultConfig()
    config.set("world.width", 20)
    config.set("world.height", 20)
    config.set("world.vision_width", 8)
    config.set("world.vision_height", 8)
    config.set("audio.sample_rate", 44100)
    
    # Create advanced sensor processing
    sensor_system = AdvancedSensorProcessing(config)
    
    # Run simulation
    brain, agent, world = sensor_system.run_advanced_simulation(500)
    
    print("Advanced sensor simulation completed")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
```

## Performance Optimization

### Brain Optimization

```python
import pynlm

class BrainOptimizer:
    """Brain optimization for performance and accuracy."""
    
    @staticmethod
    def optimize_for_speed(config):
        """Optimize brain for fast simulation."""
        # Create copy of config
        opt_config = pynlm.createDefaultConfig()
        
        # Get original values
        neuron_count = config.get("brain.neuron_count", "int")
        region_count = config.get("brain.region_count", "int")
        
        # Optimize for speed
        opt_config.set("brain.neuron_count", min(500, neuron_count))
        opt_config.set("brain.region_count", max(1, region_count // 2))
        opt_config.set("brain.synapse_density", 0.05)
        
        # Disable expensive features
        opt_config.set("plasticity.structural.enabled", False)
        opt_config.set("development.enable", False)
        
        # Enable performance optimizations
        opt_config.set("performance.enable_parallel", True)
        opt_config.set("memory.enable_pools", True)
        
        return opt_config
    
    @staticmethod
    def optimize_for_accuracy(config):
        """Optimize brain for accuracy."""
        # Create copy of config
        opt_config = pynlm.createDefaultConfig()
        
        # Get original values
        neuron_count = config.get("brain.neuron_count", "int")
        region_count = config.get("brain.region_count", "int")
        
        # Optimize for accuracy
        opt_config.set("brain.neuron_count", max(2000, neuron_count))
        opt_config.set("brain.region_count", min(10, region_count * 2))
        opt_config.set("brain.synapse_density", 0.2)
        
        # Enable all features
        opt_config.set("plasticity.structural.enabled", True)
        opt_config.set("development.enable", True)
        opt_config.set("plasticity.stdp.enabled", True)
        opt_config.set("plasticity.hebbian.enabled", True)
        
        # Enable comprehensive neuromodulation
        opt_config.set("neuromod.dopamine.scale", 1.0)
        opt_config.set("neuromod.curiosity.enabled", True)
        opt_config.set("neuromod.novelty.enabled", True)
        
        return opt_config
    
    @staticmethod
    def optimize_for_memory(config):
        """Optimize brain for memory efficiency."""
        # Create copy of config
        opt_config = pynlm.createDefaultConfig()
        
        # Get original values
        neuron_count = config.get("brain.neuron_count", "int")
        region_count = config.get("brain.region_count", "int")
        
        # Optimize for memory
        opt_config.set("brain.neuron_count", neuron_count // 2)
        opt_config.set("brain.region_count", 1)  # Single region saves memory
        opt_config.set("brain.synapse_density", 0.03)
        
        # Disable memory-intensive features
        opt_config.set("plasticity.structural.enabled", False)
        opt_config.set("development.enable", False)
        opt_config.set("memory.working_memory_size", 50)
        opt_config.set("memory.episodic_capacity", 500)
        
        # Enable memory optimizations
        opt_config.set("memory.enable_pools", True)
        opt_config.set("memory.enable_compression", True)
        
        return opt_config

# Example usage
if __name__ == "__main__":
    # Create original configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("plasticity.structural.enabled", True)
    config.set("development.enable", True)
    
    # Create optimized versions
    fast_config = BrainOptimizer.optimize_for_speed(config)
    accurate_config = BrainOptimizer.optimize_for_accuracy(config)
    memory_config = BrainOptimizer.optimize_for_memory(config)
    
    print("Brain optimization completed:")
    print(f"  Fast: {fast_config.get('brain.neuron_count', 'int')} neurons")
    print(f"  Accurate: {accurate_config.get('brain.neuron_count', 'int')} neurons")
    print(f"  Memory-efficient: {memory_config.get('brain.neuron_count', 'int')} neurons")
```

### Simulation Optimization

```python
import pynlm
import numpy as np
import time

class SimulationOptimizer:
    """Simulation optimization for performance and resource usage."""
    
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.world = None
        self.agent = None
    
    def create_optimal_setup(self):
        """Create optimal simulation setup based on configuration."""
        # Get configuration parameters
        neuron_count = self.config.get("brain.neuron_count", "int")
        world_width = self.config.get("world.width", "int")
        world_height = self.config.get("world.height", "int")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=world_width,
            height=world_height,
            visionWidth=min(8, world_width // 2),
            visionHeight=min(8, world_height // 2)
        )
        self.world.reset()
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable appropriate features based on configuration
        if self.config.get("neuromod.curiosity.enabled", "bool"):
            self.agent.enableCuriosity(True)
        
        if self.config.get("neuromod.reward.enabled", "bool"):
            self.agent.enableRewardModulation(True)
        
        if self.config.get("development.enable", "bool"):
            self.agent.enableDevelopment(True)
        
        if self.config.get("plasticity.structural.enabled", "bool"):
            self.agent.enableStructuralPlasticity(True)
        
        return self.brain, self.agent, self.world
    
    def optimize_step_parameters(self, step):
        """Optimize step parameters based on simulation progress."""
        # Get neural activity metrics
        firing_rate = self.brain.getAverageFiringRate()
        active_neurons = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        
        # Adjust parameters based on activity
        if firing_rate > 50.0:
            # High activity - use simpler processing
            return {
                'enable_plasticity': False,
                'enable_curiosity': False,
                'time_step': 0.05
            }
        elif firing_rate < 5.0:
            # Low activity - enable more features
            return {
                'enable_plasticity': True,
                'enable_curiosity': True,
                'time_step': 0.1
            }
        else:
            # Moderate activity - use default parameters
            return {
                'enable_plasticity': True,
                'enable_curiosity': True,
                'time_step': 0.1
            }
    
    def run_optimized_simulation(self, total_steps):
        """Run optimized simulation with dynamic parameter adjustment."""
        print(f"Starting optimized simulation with {total_steps} steps")
        
        # Create optimal setup
        brain, agent, world = self.create_optimal_setup()
        
        # Record start time
        start_time = time.time()
        
        # Run simulation
        for step in range(total_steps):
            # Get optimized parameters for this step
            params = self.optimize_step_parameters(step)
            
            # Update world
            world.update(params['time_step'])
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation if enabled
            if params['enable_plasticity']:
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            # Update development if enabled
            if params['enable_curiosity']:
                agent.updateDevelopment(params['time_step'])
            
            # Monitor performance
            if step % 100 == 0:
                elapsed = time.time() - start_time
                steps_per_second = step / elapsed
                print(f"Step {step}: {steps_per_second:.1f} steps/sec")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        
        return brain, agent, world
    
    def cleanup(self):
        """Clean up resources."""
        # Clear references
        self.brain = None
        self.world = None
        self.agent = None

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("world.height", 20)
    config.set("neuromod.curiosity.enabled", True)
    config.set("neuromod.reward.enabled", True)
    config.set("development.enable", True)
    
    # Create simulator
    simulator = SimulationOptimizer(config)
    
    # Run optimized simulation
    brain, agent, world = simulator.run_optimized_simulation(1000)
    
    # Cleanup
    simulator.cleanup()
    
    print("Optimized simulation completed successfully")
```

## Advanced Error Handling

### Robust Simulation Framework

```python
import pynlm
import traceback
import logging
from typing import Dict, Any, Optional

class RobustSimulationFramework:
    """Robust simulation framework with comprehensive error handling and recovery."""
    
    def __init__(self, config, max_retries=3, checkpoint_interval=1000):
        self.config = config
        self.max_retries = max_retries
        self.checkpoint_interval = checkpoint_interval
        self.logger = self._setup_logging()
        self.simulation_state = {
            'step': 0,
            'brain': None,
            'world': None,
            'agent': None,
            'errors': [],
            'recovery_actions': []
        }
    
    def _setup_logging(self):
        """Setup logging system."""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('simulation.log'),
                logging.StreamHandler()
            ]
        )
        return logging.getLogger(__name__)
    
    def initialize_simulation(self):
        """Initialize simulation with error handling."""
        try:
            # Create brain
            self.simulation_state['brain'] = pynlm.createBrain(self.config)
            self.simulation_state['brain'].initialize()
            
            # Create world
            self.simulation_state['world'] = pynlm.createSimpleWorld()
            self.simulation_state['world'].configure(
                width=self.config.get("world.width", "int"),
                height=self.config.get("world.height", "int"),
                visionWidth=self.config.get("world.vision_width", "int"),
                visionHeight=self.config.get("world.vision_height", "int")
            )
            self.simulation_state['world'].reset()
            
            # Create agent
            self.simulation_state['agent'] = pynlm.createAgentBrain(
                self.simulation_state['brain']
            )
            self.simulation_state['agent'].initialize(
                self.simulation_state['world']
            )
            
            self.logger.info("Simulation initialized successfully")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to initialize simulation: {e}")
            self.simulation_state['errors'].append({
                'step': self.simulation_state['step'],
                'error': str(e),
                'traceback': traceback.format_exc(),
                'timestamp': time.time()
            })
            return False
    
    def safe_step(self, step):
        """Perform a single simulation step with comprehensive error handling."""
        max_retries = self.max_retries
        
        for attempt in range(max_retries):
            try:
                # Get current components
                brain = self.simulation_state['brain']
                world = self.simulation_state['world']
                agent = self.simulation_state['agent']
                
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Checkpoint if needed
                if step > 0 and step % self.checkpoint_interval == 0:
                    self.checkpoint(step)
                
                return True
                
            except Exception as e:
                self.logger.warning(f"Step {step} failed (attempt {attempt + 1}/{max_retries}): {e}")
                
                # Record error
                self.simulation_state['errors'].append({
                    'step': step,
                    'attempt': attempt + 1,
                    'error': str(e),
                    'traceback': traceback.format_exc(),
                    'timestamp': time.time()
                })
                
                # Recovery actions based on error type
                recovery_result = self._attempt_recovery(e, step)
                
                if recovery_result['success']:
                    self.logger.info(f"Recovery successful for step {step}")
                    return True
                else:
                    self.logger.error(f"Recovery failed for step {step}: {recovery_result['error']}")
                    self.simulation_state['recovery_actions'].append(recovery_result)
        
        self.logger.error(f"Step {step} failed after {max_retries} attempts")
        return False
    
    def _attempt_recovery(self, error, step):
        """Attempt to recover from an error."""
        error_type = type(error).__name__
        error_message = str(error)
        
        recovery_actions = []
        
        try:
            if "brain" in error_message.lower():
                # Brain-related error
                recovery_actions.append("Re-initializing brain")
                
                # Re-initialize brain
                new_brain = pynlm.createBrain(self.config)
                new_brain.initialize()
                self.simulation_state['brain'] = new_brain
                
                # Reset world
                self.simulation_state['world'].reset()
                
                # Create new agent
                new_agent = pynlm.createAgentBrain(new_brain)
                new_agent.initialize(self.simulation_state['world'])
                self.simulation_state['agent'] = new_agent
                
                return {
                    'success': True,
                    'message': 'Brain recovered successfully'
                }
            
            elif "world" in error_message.lower():
                # World-related error
                recovery_actions.append("Resetting world")
                self.simulation_state['world'].reset()
                return {
                    'success': True,
                    'message': 'World recovered successfully'
                }
            
            elif "agent" in error_message.lower():
                # Agent-related error
                recovery_actions.append("Re-initializing agent")
                
                # Create new agent with current brain
                new_agent = pynlm.createAgentBrain(self.simulation_state['brain'])
                new_agent.initialize(self.simulation_state['world'])
                self.simulation_state['agent'] = new_agent
                
                return {
                    'success': True,
                    'message': 'Agent recovered successfully'
                }
            
            elif "memory" in error_message.lower():
                # Memory error
                recovery_actions.append("Triggering garbage collection")
                import gc
                gc.collect()
                return {
                    'success': True,
                    'message': 'Memory recovered successfully'
                }
            
            else:
                # Unknown error type - attempt reset
                recovery_actions.append("Resetting simulation state")
                return self._reset_simulation()
                
        except Exception as recovery_error:
            return {
                'success': False,
                'error': str(recovery_error),
                'actions': recovery_actions
            }
    
    def _reset_simulation(self):
        """Reset entire simulation state."""
        try:
            # Create new simulation
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            
            new_world = pynlm.createSimpleWorld()
            new_world.configure(
                width=self.config.get("world.width", "int"),
                height=self.config.get("world.height", "int"),
                visionWidth=self.config.get("world.vision_width", "int"),
                visionHeight=self.config.get("world.vision_height", "int")
            )
            new_world.reset()
            
            new_agent = pynlm.createAgentBrain(new_brain)
            new_agent.initialize(new_world)
            
            self.simulation_state = {
                'step': self.simulation_state['step'],
                'brain': new_brain,
                'world': new_world,
                'agent': new_agent,
                'errors': self.simulation_state['errors'],
                'recovery_actions': self.simulation_state['recovery_actions']
            }
            
            return {
                'success': True,
                'message': 'Simulation reset successfully'
            }
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e)
            }
    
    def checkpoint(self, step):
        """Save simulation checkpoint."""
        try:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            
            # Save brain state
            self.simulation_state['brain'].save(checkpoint_file)
            
            # Save simulation state
            state_file = f"simulation_state_step_{step}.json"
            import json
            with open(state_file, 'w') as f:
                json.dump({
                    'step': step,
                    'config': self.config.getKeys(),
                    'errors': len(self.simulation_state['errors']),
                    'recovery_actions': len(self.simulation_state['recovery_actions'])
                }, f, indent=2)
            
            self.logger.info(f"Checkpoint saved at step {step}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to save checkpoint: {e}")
            return False
    
    def load_checkpoint(self, checkpoint_file):
        """Load simulation from checkpoint."""
        try:
            # Load brain state
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            new_brain.load(checkpoint_file)
            
            # Load simulation state
            state_file = checkpoint_file.replace('.bin', '.json')
            import json
            with open(state_file, 'r') as f:
                state = json.load(f)
            
            # Create new world
            new_world = pynlm.createSimpleWorld()
            new_world.configure(
                width=self.config.get("world.width", "int"),
                height=self.config.get("world.height", "int"),
                visionWidth=self.config.get("world.vision_width", "int"),
                visionHeight=self.config.get("world.vision_height", "int")
            )
            new_world.reset()
            
            # Create new agent
            new_agent = pynlm.createAgentBrain(new_brain)
            new_agent.initialize(new_world)
            
            # Update simulation state
            self.simulation_state = {
                'step': state['step'],
                'brain': new_brain,
                'world': new_world,
                'agent': new_agent,
                'errors': [],  # Reset errors
                'recovery_actions': []  # Reset recovery actions
            }
            
            self.logger.info(f"Checkpoint loaded from {checkpoint_file}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to load checkpoint: {e}")
            return False
    
    def run_simulation(self, total_steps):
        """Run complete simulation with robust error handling."""
        print(f"Starting robust simulation with {total_steps} steps")
        
        # Initialize simulation
        if not self.initialize_simulation():
            print("Failed to initialize simulation")
            return False
        
        # Run simulation
        successful_steps = 0
        failed_steps = 0
        
        for step in range(total_steps):
            self.simulation_state['step'] = step
            
            if self.safe_step(step):
                successful_steps += 1
                
                # Log progress
                if step % 100 == 0:
                    brain = self.simulation_state['brain']
                    agent = self.simulation_state['agent']
                    print(f"Step {step}: Success | "
                          f"Neurons: {brain.getTotalNeuronCount()}, "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Curiosity: {agent.getCuriosityLevel():.3f}")
            else:
                failed_steps += 1
                print(f"Step {step}: Failed")
        
        # Final checkpoint
        self.checkpoint(total_steps)
        
        # Summary
        print(f"\nSimulation completed!")
        print(f"Successful steps: {successful_steps}")
        print(f"Failed steps: {failed_steps}")
        print(f"Success rate: {successful_steps/(total_steps-failed_steps)*100:.1f}%")
        print(f"Total errors: {len(self.simulation_state['errors'])}")
        print(f"Total recovery actions: {len(self.simulation_state['recovery_actions'])}")
        
        return True
    
    def get_simulation_stats(self):
        """Get simulation statistics."""
        brain = self.simulation_state['brain']
        agent = self.simulation_state['agent']
        
        return {
            'total_steps': self.simulation_state['step'],
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'developmental_stage': agent.getDevelopmentalStage(),
            'total_errors': len(self.simulation_state['errors']),
            'total_recovery_actions': len(self.simulation_state['recovery_actions'])
        }

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("world.width", 20)
    config.set("world.height", 20)
    
    # Create robust simulation framework
    framework = RobustSimulationFramework(config, max_retries=3, checkpoint_interval=1000)
    
    # Run simulation
    success = framework.run_simulation(5000)
    
    if success:
        # Get final statistics
        stats = framework.get_simulation_stats()
        print("\nFinal simulation statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
    else:
        print("Simulation failed")
```

## Future Enhancements

### Planned Features

#### 1. Machine Learning Integration
- Integration with popular ML frameworks (TensorFlow, PyTorch)
- Transfer learning from trained neural networks
- Generative adversarial networks for synthetic data

#### 2. Advanced Visualization
- Real-time neural activity visualization
- 3D neural network visualization
- Interactive exploration tools

#### 3. Cloud Computing Support
- Distributed simulation across multiple machines
- Cloud storage for checkpointing
- Auto-scaling simulation environments

#### 4. Custom Neuron Models
- Hodgkin-Huxley neuron models
- Morris-Lecar neuron models
- Custom neuron dynamics

#### 9. Integration with Other Frameworks
- ROS (Robot Operating System) integration
- Unity game engine integration
- MATLAB/Simulink integration

## Summary

This advanced usage guide provides comprehensive coverage of advanced NLM features, including:

1. **Advanced Configuration Management**: Configuration schemas, validation, and templates
2. **Advanced Simulation Techniques**: Custom time integration, parallel simulation, memory-efficient simulation
3. **Advanced Sensor Processing**: Custom preprocessing, feature extraction, sensor fusion
4. **Performance Optimization**: Brain optimization, simulation optimization
5. **Robust Error Handling**: Comprehensive error handling and recovery mechanisms
6. **Future Enhancements**: Planned features and roadmap

These advanced features enable experienced users to:

- Build sophisticated neural simulation experiments
- Optimize performance for specific use cases
- Handle complex error scenarios gracefully
- Integrate NLM with other systems and frameworks
- Push the boundaries of neural simulation research

The guide provides both theoretical foundations and practical implementation examples, making it valuable for both research and production environments.

## Next Steps for Advanced Users

1. **Experiment with Configuration Templates**: Use the provided templates to quickly create complex configurations
2. **Explore Performance Optimization**: Apply optimization techniques to improve simulation efficiency
3. **Implement Error Handling**: Add robust error handling to production simulations
4. **Integrate with External Systems**: Connect NLM with other frameworks and tools
5. **Contribute to the Community**: Share your advanced techniques and improvements

The NLM framework offers extensive capabilities for advanced neural simulation, and these advanced features unlock its full potential for expert users.