# Enhanced NLM Python API Documentation

# NLM (熙然) Python Bindings - Advanced Usage Guide

This document provides comprehensive guidance for advanced Python users working with NLM's Python bindings.

## Overview

NLM's Python API provides access to the powerful spiking neural network simulator from Python, enabling:
- Easy brain creation and configuration
- Simulation control and monitoring
- Integration with Python data science libraries
- Advanced neural network experimentation
- Research and development workflows

## Quick Start Examples

### Example 1: Basic Neural Network
```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
spike_counts = []
for step in range(1000):
    brain.step(step)
    spike_counts.append(brain.getFiringNeuronCount())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Plot results
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(spike_counts)
plt.xlabel("Step")
plt.ylabel("Firing Neurons")
plt.title("Neural Activity Over Time")
plt.grid(True, alpha=0.3)

plt.subplot(1, 2, 1)
plt.plot(np.diff(spike_counts))
plt.xlabel("Step")
plt.ylabel("Spike Rate Change")
plt.title("Activity Dynamics")
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()
```

### Example 2: Complete Agent Simulation
```python
import pynlm
import numpy as np
from pathlib import Path

class NLMExperiment:
    """Enhanced experiment runner with monitoring and analysis"""
    
    def __init__(self, config_name="default"):
        self.config = self._load_config(config_name)
        self.brain = None
        self.world = None
        self.agent = None
        self.metrics = {}
        self.checkpoint_dir = Path("checkpoints")
        self.checkpoint_dir.mkdir(exist_ok=True)
    
    def _load_config(self, name):
        """Load configuration with error handling"""
        try:
            config = pynlm.createDefaultConfig()
            
            # Apply common configurations
            config.set("brain.neuron_count", 1500)
            config.set("brain.synapse_density", 0.08)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.structural.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            
            # Enable development
            config.set("development.enable", True)
            config.set("development.stage", "critical_period")
            
            return config
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
            raise
    
    def run_episode(self, max_steps=500, save_checkpoint=False):
        """Run a single simulation episode with comprehensive monitoring"""
        
        # Initialize components
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=20, 
            height=20, 
            visionWidth=8, 
            visionHeight=8
        )
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Initialize metrics collection
        self.metrics = {
            'steps': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'rewards': [],
            'actions': [],
            'position_x': [],
            'position_y': [],
            'energy_levels': [],
            'spike_counts': []
        }
        
        print(f"Starting episode simulation...")
        
        for step in range(max_steps):
            try:
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode motor command
                motor_cmd = self.agent.decodeMotorCommand()
                
                # Apply motor command
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Get reward and apply neuromodulation
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect metrics
                self._collect_metrics(step, reward, motor_cmd)
                
                # Print progress
                if step % 50 == 0:
                    self._print_progress(step)
                
                # Save checkpoint periodically
                if save_checkpoint and step % 100 == 0:
                    self.save_checkpoint(step)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue simulation despite errors
                continue
        
        print(f"Episode completed after {max_steps} steps")
        return self.metrics
    
    def _collect_metrics(self, step, reward, action):
        """Collect comprehensive simulation metrics"""
        self.metrics['steps'].append(step)
        self.metrics['firing_rates'].append(self.brain.getFiringNeuronCount())
        self.metrics['curiosity_levels'].append(self.agent.getCuriosityLevel())
        self.metrics['novelty_levels'].append(self.agent.getNoveltyLevel())
        self.metrics['rewards'].append(reward)
        self.metrics['actions'].append(action.getType() if hasattr(action, 'getType') else str(action))
        
        # Get agent position
        body = self.world.getAgentBody()
        self.metrics['position_x'].append(body.getX())
        self.metrics['position_y'].append(body.getY())
        
        # Get energy level
        self.metrics['energy_levels'].append(self.world.getMaxEnergy())
        
        # Total spike count
        self.metrics['spike_counts'].append(self.brain.getTotalSpikeCount())
    
    def _print_progress(self, step):
        """Print simulation progress"""
        avg_firing = np.mean(self.metrics['firing_rates'][-50:])
        avg_reward = np.mean(self.metrics['rewards'][-50:])
        
        print(f"  Step {step:4d} | "
              f"Firing: {self.brain.getFiringNeuronCount():4d} avg: {avg_firing:6.1f} | "
              f"Curiosity: {self.agent.getCuriosityLevel():.3f} | "
              f"Novelty: {self.agent.getNoveltyLevel():.3f} | "
              f"Reward: {avg_reward:+.3f} | "
              f"Position: ({self.metrics['position_x'][-1]:5.1f}, {self.metrics['position_y'][-1]:5.1f})")
    
    def save_checkpoint(self, step):
        """Save simulation checkpoint"""
        try:
            filename = self.checkpoint_dir / f"checkpoint_step_{step}.bin"
            self.brain.save(str(filename))
            print(f"  Checkpoint saved: {filename}")
        except Exception as e:
            print(f"  Error saving checkpoint: {e}")
    
    def load_checkpoint(self, filename):
        """Load simulation from checkpoint"""
        try:
            self.brain.load(filename)
            print(f"Checkpoint loaded: {filename}")
            return True
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def analyze_results(self):
        """Perform comprehensive analysis of simulation results"""
        import pandas as pd
        
        df = pd.DataFrame(self.metrics)
        
        # Calculate derived metrics
        df['spike_rate'] = df['spike_counts'].diff() / 0.1  # steps of 0.1s
        df['firing_rate'] = df['firing_rates'] / df['firing_rates'].max()
        df['distance_traveled'] = np.sqrt(df['position_x'].diff()**2 + df['position_y'].diff()**2).cumsum()
        
        # Identify key patterns
        analysis = {
            'total_steps': len(df),
            'final_position': (df['position_x'].iloc[-1], df['position_y'].iloc[-1]),
            'max_curiosity': df['curiosity_levels'].max(),
            'max_novelty': df['novelty_levels'].max(),
            'avg_reward': df['rewards'].mean(),
            'total_spikes': df['spike_counts'].iloc[-1],
            'distance_traveled': df['distance_traveled'].iloc[-1],
            'firing_stability': df['firing_rates'].std(),
            'reward_variance': df['rewards'].var()
        }
        
        return analysis, df

# Usage example
if __name__ == "__main__":
    # Create experiment
    experiment = NLMExperiment()
    
    # Run episode
    metrics = experiment.run_episode(max_steps=1000, save_checkpoint=True)
    
    # Analyze results
    analysis, df = experiment.analyze_results()
    
    print("\n=== Experiment Analysis ===")
    for key, value in analysis.items():
        if isinstance(value, float):
            print(f"{key:20s}: {value:10.3f}")
        else:
            print(f"{key:20s}: {value}")
    
    # Save results
    df.to_csv("experiment_results.csv", index=False)
    print("\nResults saved to experiment_results.csv")
```

### Example 3: Advanced Configuration Management
```python
import pynlm
import json
import os

class NLMConfigManager:
    """Advanced configuration management for NLM"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config_history = []
        self.configs_dir = "configs"
        os.makedirs(self.configs_dir, exist_ok=True)
    
    def create_preset(self, name, **kwargs):
        """Create a named configuration preset"""
        # Create a copy of default config
        preset_config = pynlm.createDefaultConfig()
        
        # Apply settings
        for key, value in kwargs.items():
            preset_config.set(key, value)
        
        # Save preset
        preset_file = os.path.join(self.configs_dir, f"{name}.json")
        preset_config.saveToFile(preset_file)
        
        # Store in history
        self.config_history.append({
            'name': name,
            'config': preset_config,
            'timestamp': pynlm.createSimpleWorld().getSimulationTime()
        })
        
        print(f"Preset '{name}' created and saved to {preset_file}")
    
    def load_preset(self, name):
        """Load a configuration preset"""
        preset_file = os.path.join(self.configs_dir, f"{name}.json")
        
        if not os.path.exists(preset_file):
            raise FileNotFoundError(f"Preset file not found: {preset_file}")
        
        # Create new config from file
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(preset_file)
        
        # Replace current config
        self.config = new_config
        
        print(f"Preset '{name}' loaded successfully")
        return self.config
    
    def get_config_summary(self):
        """Get human-readable configuration summary"""
        summary = self.config.summary()
        
        # Extract key information
        lines = summary.split('\n')
        important_keys = ['brain.neuron_count', 'brain.synapse_density', 'plasticity.stdp.enable', 
                         'neuromod.curiosity.enable', 'neuromod.novelty.enable']
        
        important_info = []
        for line in lines:
            for key in important_keys:
                if key in line:
                    important_info.append(line.strip())
                    break
        
        return "\n".join(important_info)
    
    def validate_config(self):
        """Validate current configuration"""
        errors = []
        
        # Check neuron count
        neuron_count = self.config.getOr<int64_t>("brain.neuron_count", 1000)
        if neuron_count < 1 or neuron_count > 1000000:
            errors.append(f"Invalid neuron_count: {neuron_count}. Must be between 1 and 1,000,000.")
        
        # Check synapse density
        synapse_density = self.config.getOr<float>("brain.synapse_density", 0.1)
        if synapse_density < 0.0 or synapse_density > 1.0:
            errors.append(f"Invalid synapse_density: {synapse_density}. Must be between 0.0 and 1.0.")
        
        # Check plasticity settings
        if not self.config.getOr<bool>("plasticity.stdp.enable", True):
            print("Warning: STDP plasticity is disabled")
        
        # Check neuromodulation
        if not self.config.getOr<bool>("neuromod.curiosity.enable", True):
            print("Warning: Curiosity neuromodulation is disabled")
        
        if not self.config.getOr<bool>("neuromod.novelty.enable", True):
            print("Warning: Novelty detection is disabled")
        
        return errors

# Usage example
if __name__ == "__main__":
    # Create configuration manager
    config_manager = NLMConfigManager()
    
    # Create some presets
    config_manager.create_preset("small", 
                                 brain.neuron_count=500,
                                 brain.synapse_density=0.05,
                                 plasticity.stdp.enable=True)
    
    config_manager.create_preset("medium",
                                 brain.neuron_count=2000,
                                 brain.synapse_density=0.1,
                                 plasticity.stdp.enable=True,
                                 plasticity.hebbian.enable=True)
    
    config_manager.create_preset("large",
                                 brain.neuron_count=5000,
                                 brain.synapse_density=0.15,
                                 plasticity.stdp.enable=True,
                                 plasticity.hebbian.enable=True,
                                 plasticity.structural.enable=True,
                                 neuromod.curiosity.enable=True,
                                 neuromod.novelty.enable=True)
    
    # Load a preset and validate
    config_manager.load_preset("medium")
    validation_errors = config_manager.validate_config()
    
    if validation_errors:
        print("Configuration validation errors:")
        for error in validation_errors:
            print(f"  - {error}")
    else:
        print("Configuration is valid")
    
    # Show config summary
    print("\nConfiguration Summary:")
    print(config_manager.get_config_summary())
```

### Example 4: Integration with Machine Learning
```python
import pynlm
import numpy as np
import torch
import torch.nn as nn
from sklearn.preprocessing import StandardScaler

class NLMFeatureExtractor(nn.Module):
    """Extract features from NLM simulation for machine learning"""
    
    def __init__(self, sequence_length=100):
        super().__init__()
        self.sequence_length = sequence_length
        self.scaler = StandardScaler()
        
        # Feature extraction layers
        self.feature_net = nn.Sequential(
            nn.Linear(15, 32),  # Input: 15 features
            nn.ReLU(),
            nn.Linear(32, 64),
            nn.ReLU(),
            nn.Linear(64, 32),
            nn.ReLU(),
            nn.Linear(32, 8)
        )
    
    def extract_features(self, brain, world, agent, num_steps):
        """Extract features from simulation"""
        features = []
        
        for step in range(num_steps):
            # Get current state features
            state_features = self._get_state_features(brain, world, agent, step)
            features.append(state_features)
        
        return np.array(features)
    
    def _get_state_features(self, brain, world, agent, step):
        """Extract features from current simulation state"""
        # Get basic brain metrics
        firing_neuron_count = brain.getFiringNeuronCount()
        total_spike_count = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        developmental_stage = brain.getDevelopmentalStage()
        
        # Get agent metrics
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        neuromodulation_level = agent.getNeuromodulationLevel()
        prediction_error = agent.getPredictionError()
        
        # Get world metrics
        body = world.getAgentBody()
        position_x, position_y = body.getX(), body.getY()
        energy = world.getMaxEnergy()
        
        # Normalize features
        features = np.array([
            firing_neuron_count / 1000.0,  # Normalize neuron count
            total_spike_count / 100000.0,  # Normalize spike count
            avg_firing_rate / 100.0,  # Normalize firing rate
            e_i_ratio,  # E/I ratio (already normalized)
            curiosity_level,
            novelty_level,
            neuromodulation_level,
            prediction_error,
            position_x / 20.0,  # Normalize position (world size 20x20)
            position_y / 20.0,
            energy / 100.0,  # Normalize energy
            float(developmental_stage),  # Developmental stage
            np.sin(step / 100.0),  # Time-based features
            np.cos(step / 100.0),
            np.exp(-step / 1000.0)  # Decay feature
        ])
        
        # Apply scaler if fitted
        if hasattr(self, 'fitted_'):
            features = self.scaler.transform(features.reshape(1, -1)).flatten()
        
        return features
    
    def process_features(self, features):
        """Process features with neural network"""
        # Convert to tensor
        x = torch.FloatTensor(features)
        
        # Process through network
        with torch.no_grad():
            output = self.feature_net(x)
        
        return output

class NLMControlledEnvironment:
    """Environment for reinforcement learning with NLM"""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.feature_extractor = NLMFeatureExtractor()
        self.current_step = 0
        self.episode_length = 0
        
    def reset(self):
        """Reset environment"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        self.current_step = 0
        self.episode_length = 0
        
        return self._get_observation()
    
    def step(self, action):
        """Take a step in the environment"""
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        self.brain.step(self.current_step)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        
        # Get reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        self.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        self.current_step += 1
        self.episode_length += 1
        
        # Check if episode is done
        done = self.episode_length >= 500 or self.brain.getTotalSpikeCount() > 100000
        
        return self._get_observation(), reward, done, {}
    
    def _get_observation(self):
        """Get current state as observation"""
        # Extract features
        features = self.feature_extractor.extract_features(
            self.brain, self.world, self.agent, self.current_step
        )
        
        if len(features) > 0:
            # Return last observation
            return features[-1]
        else:
            # Return default observation
            return np.zeros(15)

# Usage example for reinforcement learning
if __name__ == "__main__":
    # Create environment
    env = NLMControlledEnvironment()
    
    # Test environment
    state = env.reset()
    print(f"Initial state shape: {state.shape}")
    
    # Run a few steps
    for step in range(10):
        # Sample random action (in real RL, this would be from policy)
        action_type = pynlm.ActionType.MoveForward  # Example action
        
        # Take step
        next_state, reward, done, info = env.step(action_type)
        
        print(f"Step {step}: reward={reward:.3f}, done={done}, state_shape={next_state.shape}")
        
        if done:
            state = env.reset()
```

### Example 5: Advanced Python Utilities
```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor
import time

class NLMParallelRunner:
    """Run NLM simulations in parallel"""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
        self.executor = None
    
    def _run_single_simulation(self, config_params):
        """Run a single simulation (worker function)"""
        try:
            import pynlm
            
            # Create config
            config = pynlm.createDefaultConfig()
            
            # Apply parameters
            for key, value in config_params.items():
                config.set(key, value)
            
            # Create and run simulation
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            spike_counts = []
            for step in range(100):
                brain.step(step)
                spike_counts.append(brain.getFiringNeuronCount())
            
            return {
                'success': True,
                'final_spike_count': brain.getTotalSpikeCount(),
                'max_firing': max(spike_counts),
                'avg_firing': sum(spike_counts) / len(spike_counts),
                'spike_variance': np.var(spike_counts) if len(spike_counts) > 1 else 0.0
            }
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e)
            }
    
    def run_parallel_simulations(self, param_list):
        """Run multiple simulations in parallel"""
        with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
            # Submit all tasks
            futures = [executor.submit(self._run_single_simulation, params) 
                      for params in param_list]
            
            # Collect results
            results = []
            for future in futures:
                results.append(future.result())
            
        return results
    
    def parameter_search(self, param_space):
        """Search over parameter space"""
        print(f"Running parameter search with {len(param_space)} combinations")
        print(f"Using {self.num_workers} workers")
        
        # Generate all parameter combinations
        from itertools import product
        
        param_names = list(param_space.keys())
        param_values = list(param_space.values())
        
        combinations = list(product(*param_values))
        
        # Convert to parameter dictionaries
        param_list = []
        for combo in combinations:
            param_dict = {name: value for name, value in zip(param_names, combo)}
            param_list.append(param_dict)
        
        # Run simulations
        results = self.run_parallel_simulations(param_list)
        
        # Filter successful results
        successful_results = [r for r in results if r['success']]
        
        if successful_results:
            # Find best result
            best_result = max(successful_results, 
                            key=lambda x: x['avg_firing'] * x['max_firing'])
            
            print(f"Best result: avg_firing={best_result['avg_firing']:.1f}, "
                  f"max_firing={best_result['max_firing']}")
            
            # Find parameters that produced best result
            best_params = param_list[results.index(best_result)]
            print(f"Best parameters: {best_params}")
        
        return results

class NLMPerformanceProfiler:
    """Profile NLM performance and identify bottlenecks"""
    
    def __init__(self):
        self.timings = []
        self.memory_usage = []
        self.spike_counts = []
    
    def profile_simulation(self, brain, world, agent, num_steps=1000):
        """Profile a simulation run"""
        import time
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        print(f"Starting performance profiling ({num_steps} steps)...")
        
        for step in range(num_steps):
            step_start_time = time.time()
            
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            step_time = time.time() - step_start_time
            
            # Record metrics
            self.timings.append(step_time)
            self.memory_usage.append(process.memory_info().rss / 1024 / 1024)  # MB
            self.spike_counts.append(brain.getFiringNeuronCount())
            
            # Print progress
            if step % 100 == 0:
                avg_time = np.mean(self.timings[-100:])
                avg_memory = np.mean(self.memory_usage[-100:])
                print(f"  Step {step:4d} | "
                      f"Time: {step_time*1000:6.1f}ms avg: {avg_time*1000:6.1f}ms | "
                      f"Memory: {process.memory_info().rss / 1024 / 1024:6.1f}MB avg: {avg_memory:6.1f}MB | "
                      f"Firing: {brain.getFiringNeuronCount():4d}")
        
        # Generate performance report
        self._generate_report()
        
        return {
            'total_time': sum(self.timings),
            'avg_step_time': np.mean(self.timings),
            'std_step_time': np.std(self.timings),
            'min_step_time': np.min(self.timings),
            'max_step_time': np.max(self.timings),
            'total_memory': max(self.memory_usage),
            'avg_memory': np.mean(self.memory_usage),
            'peak_memory': max(self.memory_usage),
            'total_spikes': brain.getTotalSpikeCount(),
            'spike_rate': brain.getTotalSpikeCount() / num_steps,
            'stability': 1.0 / (1.0 + np.std(self.spike_counts))
        }
    
    def _generate_report(self):
        """Generate performance report"""
        print("\n=== Performance Report ===")
        
        if len(self.timings) < 100:
            print("Not enough data for statistical analysis")
            return
        
        # Time analysis
        print(f"Time Analysis:")
        print(f"  Total steps: {len(self.timings)}")
        print(f"  Average step time: {np.mean(self.timings)*1000:.2f} ms")
        print(f"  Standard deviation: {np.std(self.timings)*1000:.2f} ms")
        print(f"  Minimum step time: {np.min(self.timings)*1000:.2f} ms")
        print(f"  Maximum step time: {np.max(self.timings)*1000:.2f} ms")
        print(f"  95th percentile: {np.percentile(self.timings, 95)*1000:.2f} ms")
        
        # Memory analysis
        print(f"\nMemory Analysis:")
        print(f"  Average memory usage: {np.mean(self.memory_usage):.2f} MB")
        print(f"  Peak memory usage: {np.max(self.memory_usage):.2f} MB")
        print(f"  Memory standard deviation: {np.std(self.memory_usage):.2f} MB")
        
        # Spike analysis
        print(f"\nSpike Analysis:")
        print(f"  Average firing rate: {np.mean(self.spike_counts):.1f} neurons")
        print(f"  Firing rate std: {np.std(self.spike_counts):.1f} neurons")
        print(f"  Coefficient of variation: {np.std(self.spike_counts)/np.mean(self.spike_counts):.3f}")
        
        # Performance indicators
        avg_time_ms = np.mean(self.timings) * 1000
        avg_memory_mb = np.mean(self.memory_usage)
        spike_rate = np.mean(self.spike_counts)
        
        print(f"\nPerformance Indicators:")
        print(f"  Steps per second: {1000/avg_time_ms:.1f}")
        print(f"  Memory efficiency: {spike_rate/avg_memory_mb:.1f} spikes per MB")
        print(f"  Stability index: {1.0/(1.0 + np.std(self.spike_counts)/np.mean(self.spike_counts)):.3f}")

# Usage example
if __name__ == "__main__":
    # Example 1: Parallel parameter search
    print("=== Parallel Parameter Search ===")
    
    runner = NLMParallelRunner(num_workers=4)
    
    param_space = {
        'brain.neuron_count': [1000, 2000, 5000],
        'brain.synapse_density': [0.05, 0.1, 0.15],
        'plasticity.stdp.enable': [True, False]
    }
    
    results = runner.parameter_search(param_space)
    
    # Example 2: Performance profiling
    print("\n=== Performance Profiling ===")
    
    # Create a simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Profile the simulation
    profiler = NLMPerformanceProfiler()
    performance = profiler.profile_simulation(brain, world, agent, num_steps=500)
    
    print(f"\nSimulation completed in {performance['total_time']:.2f} seconds")
    print(f"Average step time: {performance['avg_step_time']*1000:.2f} ms")
    print(f"Performance score: {performance['stability']:.3f}")
```

## Advanced Features

### 1. Custom Configuration Schemas
```python
import jsonschema
import json

# Define configuration schema
CONFIG_SCHEMA = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {
            "type": "integer",
            "minimum": 1,
            "maximum": 1000000,
            "description": "Number of neurons in the brain"
        },
        "brain.synapse_density": {
            "type": "number",
            "minimum": 0.0,
            "maximum": 1.0,
            "description": "Synapse density between neurons"
        },
        "plasticity.stdp.enable": {
            "type": "boolean",
            "description": "Enable Spike-Timing-Dependent Plasticity"
        }
    },
    "required": ["brain.neuron_count", "brain.synapse_density"]
}

# Validate configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.1)

# Convert to dict for validation
config_dict = {}
for key in ["brain.neuron_count", "brain.synapse_density", "plasticity.stdp.enable"]:
    config_dict[key] = config.getOr(config.get_type(key), None)

# Validate against schema
jsonschema.validate(config_dict, CONFIG_SCHEMA)
print("Configuration is valid!")
```

### 2. Enhanced Error Handling
```python
def safe_brain_operation(config, operation="create"):
    """Safe brain operation with comprehensive error handling"""
    try:
        if operation == "create":
            brain = pynlm.createBrain(config)
            brain.initialize()
            return brain
        elif operation == "step":
            # Assume brain already exists
            brain.step(0)
            return brain
        else:
            raise ValueError(f"Unknown operation: {operation}")
            
    except Exception as e:
        # Log error
        print(f"Error in brain operation: {e}")
        
        # Try to recover
        if operation == "create":
            try:
                # Try with default config
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                return brain
            except Exception as e2:
                print(f"Recovery failed: {e2}")
                raise NLMError(f"Failed to create brain: {e}")
        
        raise NLMError(f"Brain operation failed: {e}")
```

### 3. Advanced Monitoring
```python
class NLMMonitor:
    """Advanced monitoring of NLM simulation"""
    
    def __init__(self):
        self.metrics = {}
        self.alerts = []
        self.thresholds = {
            'max_firing_rate': 1000,
            'max_memory_usage': 1024,  # MB
            'max_step_time': 100,  # ms
            'min_reward': -10
        }
    
    def record_metric(self, name, value, timestamp=None):
        """Record a metric with timestamp"""
        if name not in self.metrics:
            self.metrics[name] = []
        
        self.metrics[name].append({
            'value': value,
            'timestamp': timestamp or time.time()
        })
        
        # Check for alerts
        self._check_alerts(name, value)
    
    def _check_alerts(self, name, value):
        """Check if value exceeds thresholds"""
        if name in self.thresholds:
            threshold = self.thresholds[name]
            
            if name == 'max_firing_rate' and value > threshold:
                self.alerts.append({
                    'type': 'warning',
                    'message': f"High firing rate detected: {value} Hz (threshold: {threshold} Hz)",
                    'timestamp': time.time()
                })
            
            elif name == 'max_memory_usage' and value > threshold:
                self.alerts.append({
                    'type': 'critical',
                    'message': f"High memory usage detected: {value} MB (threshold: {threshold} MB)",
                    'timestamp': time.time()
                })
    
    def get_metrics_summary(self):
        """Get summary of recorded metrics"""
        summary = {}
        
        for name, data in self.metrics.items():
            if data:
                values = [d['value'] for d in data]
                summary[name] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'count': len(values)
                }
        
        return summary
    
    def generate_report(self):
        """Generate comprehensive monitoring report"""
        report = f"NLM Monitoring Report\n"
        report += f"Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
        report += f"Total metrics recorded: {sum(len(d) for d in self.metrics.values())}\n"
        report += f"Active alerts: {len(self.alerts)}\n"
        
        if self.alerts:
            report += "\nAlerts:\n"
            for alert in self.alerts[-10:]:  # Show last 10 alerts
                report += f"  {alert['timestamp']}: {alert['type'].upper()} - {alert['message']}\n"
        
        report += "\nMetrics Summary:\n"
        for name, stats in self.get_metrics_summary().items():
            report += f"  {name}: mean={stats['mean']:.2f}, "
                      f"std={stats['std']:.2f}, "
                      f"min={stats['min']}, "
                      f"max={stats['max']}, "
                      f"count={stats['count']}\n"
        
        return report
```

## Best Practices

### 1. Resource Management
```python
# Proper resource management
import gc

# Create and destroy objects properly
brain = pynlm.createBrain(config)
try:
    # Use brain
    brain.initialize()
    # ... simulation code
    
    # Force garbage collection if memory is high
    if sys.getsizeof(brain) > 100 * 1024 * 1024:  # 100MB
        gc.collect()
finally:
    # Clean up resources
    brain.reset()
    del brain
```

### 2. Configuration Validation
```python
# Validate configuration before use
def validate_config(config):
    # Check for required keys
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    
    for key in required_keys:
        if not config.has(key):
            raise ValueError(f"Missing required configuration key: {key}")
    
    # Check value ranges
    neuron_count = config.getOr<int64_t>("brain.neuron_count")
    if neuron_count < 1 or neuron_count > 1000000:
        raise ValueError(f"Invalid neuron_count: {neuron_count}")
    
    synapse_density = config.getOr<float>("brain.synapse_density")
    if synapse_density < 0.0 or synapse_density > 1.0:
        raise ValueError(f"Invalid synapse_density: {synapse_density}")
    
    return True
```

### 3. Error Recovery
```python
# Implement error recovery strategies
class ErrorRecovery:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.error_count = 0
        self.max_errors = 10
    
    def safe_step(self, step):
        """Take a step with error recovery"""
        try:
            # Run simulation step
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            motor_cmd = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            self.error_count = 0
            return True
            
        except Exception as e:
            self.error_count += 1
            print(f"Error at step {step}: {e}")
            
            if self.error_count >= self.max_errors:
                raise SimulationError(f"Too many errors: {self.error_count}")
            
            # Attempt recovery
            try:
                self._recover_from_error(step)
                return True
            except Exception as e2:
                print(f"Recovery failed: {e2}")
                raise
    
    def _recover_from_error(self, step):
        """Recover from simulation error"""
        # Reset world state
        self.world.reset()
        
        # Reinitialize agent
        self.agent.initialize(self.world)
        
        # Reset brain (preserving learned preferences)
        self.brain.reset()
        
        # Try to continue from current state
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(step)
```

## Conclusion

The enhanced NLM Python API provides:

1. **Advanced Configuration Management**: Preset configurations, validation, and schema support
2. **Comprehensive Error Handling**: Robust error recovery and graceful degradation
3. **Performance Monitoring**: Detailed profiling and performance analysis
4. **Parallel Processing**: Multi-worker simulation runs for parameter search
5. **Resource Management**: Efficient memory and resource usage
6. **Integration Tools**: Support for machine learning and data analysis
7. **Advanced Monitoring**: Real-time metrics and alerting

These enhancements make NLM more powerful, reliable, and suitable for:
- Research and experimentation
- Production deployments
- Performance optimization
- Integration with larger systems
- Automated testing and validation

The Python API now provides enterprise-grade features while maintaining the simplicity and flexibility that makes NLM accessible to users of all experience levels.