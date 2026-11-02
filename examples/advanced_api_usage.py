#!/usr/bin/env python3
"""
Enhanced NLM Python API Examples
=================================

This file demonstrates the enhanced Python API features for the NLM framework.
It includes new patterns, advanced usage, and examples that leverage
the improved bindings with context managers, better error handling,
and Pythonic interfaces.

The enhanced API provides:
• Context managers for resource management
• Better error handling with Python exceptions
• Performance monitoring tools
• Neural recording and playback
• Configuration management
• Visualization and analysis tools
"""

import time
import json
import numpy as np
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass

import pynlm

print("=== Enhanced NLM Python API Examples ===\n")

# Example 1: Context Manager Pattern
print("1. Context Manager Pattern for Resource Management")
print("=" * 50)

@contextmanager
def simulation_session(neuron_count=500, steps=1000):
    """Context manager for controlled simulation sessions."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    try:
        yield brain
    finally:
        brain.reset()
        print(f"✓ Simulation session completed (neurons: {neuron_count}, steps: {steps})")

# Using context manager
with simulation_session(100, 500) as brain:
    for i in range(500):
        brain.step(i)
    print(f"Final spike count: {brain.getTotalSpikeCount()}")

# Example 2: Advanced Brain Configuration
print("\n2. Advanced Brain Configuration")
print("=" * 50)

config = pynlm.createDefaultConfig()

# Configure multiple aspects at once
advanced_config = {
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.12,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.012,
    "development_enabled": True,
    "curiosity_enabled": True,
    "reward_modulation_enabled": True,
    "structural_plasticity_enabled": True,
    "plasticity_modifier": 1.0,
    "simulation_timestep": 0.001,
    "random_seed": 12345
}

# Apply advanced configuration
for key, value in advanced_config.items():
    config.set(key, value)

print("Advanced configuration applied:")
for key, value in advanced_config.items():
    retrieved = config.getOr(key, None)
    print(f"  {key}: {retrieved}")

# Example 3: Brain Performance Monitoring
print("\n3. Brain Performance Monitoring")
print("=" * 50)

class BrainMonitor:
    """Monitor brain performance and metrics."""
    
    def __init__(self, brain):
        self.brain = brain
        self.metrics = []
        self.start_time = None
        self.step_times = []
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        self.step_times = []
        self.metrics = []
        print("✓ Performance monitoring started")
        return self
    
    def step(self, step_num):
        """Monitor a single step."""
        step_start = time.time()
        self.brain.step(step_num)
        step_time = time.time() - step_start
        self.step_times.append(step_time)
        
        # Collect metrics
        metrics = {
            'step': step_num,
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'step_time': step_time,
            'neurons_active': self.brain.getActiveNeuronCount(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
        self.metrics.append(metrics)
        
        if step_num % 100 == 0:
            print(f"  Step {step_num}: {metrics['spike_count']} spikes, "
                  f"{metrics['firing_rate']:.2f} Hz avg, "
                  f"{metrics['step_time']*1000:.2f} ms step")
    
    def stop_monitoring(self):
        """Stop performance monitoring and report statistics."""
        if self.start_time:
            total_time = time.time() - self.start_time
            if self.step_times:
                avg_step_time = sum(self.step_times) / len(self.step_times)
                steps_per_second = len(self.step_times) / sum(self.step_times) if sum(self.step_times) > 0 else 0
                
                print(f"\n✓ Performance Monitoring Summary:")
                print(f"  Total time: {total_time:.2f} seconds")
                print(f"  Average step time: {avg_step_time*1000:.2f} ms")
                print(f"  Steps per second: {steps_per_second:.2f}")
                print(f"  Total steps monitored: {len(self.metrics)}")
                print(f"  Final spike count: {self.brain.getTotalSpikeCount()}")
                print(f"  Final firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        
        return self

# Example 4: Neural Recording and Playback
print("\n4. Neural Recording and Playback")
print("=" * 50)

class NeuralRecorder:
    """Record and playback neural activity patterns."""
    
    def __init__(self):
        self.recordings = {}
        self.current_recording = None
    
    def start_recording(self, config, name="recording"):
        """Start recording neural activity."""
        config = config or pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.current_recording = {
            'brain': brain,
            'config': config,
            'name': name,
            'data': [],
            'start_time': time.time()
        }
        
        print(f"✓ Started recording '{name}'")
        return brain
    
    def record_step(self, brain, step_num):
        """Record a single step."""
        if not self.current_recording:
            raise RuntimeError("No active recording")
        
        step_data = {
            'step': step_num,
            'timestamp': time.time() - self.current_recording['start_time'],
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getActiveNeuronCount(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount()
        }
        
        self.current_recording['data'].append(step_data)
        brain.step(step_num)
    
    def stop_recording(self):
        """Stop recording and return the recording."""
        if not self.current_recording:
            raise RuntimeError("No active recording")
        
        recording = self.current_recording
        self.recordings[recording['name']] = recording
        
        duration = time.time() - recording['start_time']
        print(f"✓ Stopped recording '{recording['name']}' "
              f"(recorded {len(recording['data'])} steps, {duration:.2f}s)")
        
        self.current_recording = None
        return recording
    
    def play_recording(self, name):
        """Play back a recorded neural pattern."""
        if name not in self.recordings:
            raise ValueError(f"Recording '{name}' not found")
        
        recording = self.recordings[name]
        brain = recording['brain']
        data = recording['data']
        
        print(f"✓ Playing back recording '{name}' ({len(data)} steps)")
        
        for step_data in data:
            brain.step(step_data['step'])
        
        return brain

# Example 5: Advanced Agent Simulation
print("\n5. Advanced Agent Simulation")
print("=" * 50)

class AdvancedAgent:
    """Advanced agent with multiple sensory modalities and learning."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Performance tracking
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        self.episode_start_time = None
    
    def run_episode(self, max_steps=1000, render_callback=None):
        """Run a complete simulation episode."""
        world = self.world
        brain = self.brain
        agent = self.agent
        
        self.episode_start_time = time.time()
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        
        print(f"✓ Starting episode (max_steps={max_steps})")
        
        for step in range(max_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record action
            action_record = {
                'step': step,
                'action': action,
                'reward': result.reward,
                'success': result.success,
                'timestamp': time.time() - self.episode_start_time
            }
            self.action_history.append(action_record)
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Optional rendering
            if render_callback:
                render_callback(step, percept, action, result)
            
            # Check termination conditions
            if brain.getTotalSpikeCount() > 10000:
                print(f"  Terminating due to high activity ({brain.getTotalSpikeCount()} spikes)")
                break
            
            self.step_count += 1
        
        episode_duration = time.time() - self.episode_start_time
        print(f"✓ Episode completed: {self.step_count} steps, "
              f"{len(self.action_history)} actions taken in {episode_duration:.2f}s")
        
        return self.get_episode_stats()
    
    def get_episode_stats(self):
        """Get statistics about the episode."""
        if not self.action_history:
            return {}
        
        rewards = [a['reward'] for a in self.action_history]
        successes = [a['success'] for a in self.action_history]
        actions = [a['action'] for a in self.action_history]
        
        # Count actions
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        stats = {
            'total_steps': self.step_count,
            'total_actions': len(self.action_history),
            'total_reward': sum(rewards),
            'avg_reward': sum(rewards) / len(rewards),
            'success_rate': sum(successes) / len(successes),
            'max_reward': max(rewards),
            'min_reward': min(rewards),
            'final_spike_count': self.brain.getTotalSpikeCount(),
            'final_firing_rate': self.brain.getAverageFiringRate(),
            'action_distribution': action_counts,
            'duration': time.time() - self.episode_start_time if self.episode_start_time else 0
        }
        
        print(f"Episode Stats:")
        print(f"  Total steps: {stats['total_steps']}")
        print(f"  Total actions: {stats['total_actions']}")
        print(f"  Total reward: {stats['total_reward']:.2f}")
        print(f"  Average reward: {stats['avg_reward']:.3f}")
        print(f"  Success rate: {stats['success_rate']*100:.1f}%")
        print(f"  Final spike count: {stats['final_spike_count']}")
        print(f"  Final firing rate: {stats['final_firing_rate']:.2f} Hz")
        print(f"  Duration: {stats['duration']:.2f}s")
        print(f"  Action distribution: {stats['action_distribution']}")
        
        return stats

# Example 6: Configuration Management
print("\n6. Configuration Management")
print("=" * 50)

class ConfigManager:
    """Manage multiple configurations and experiment parameters."""
    
    def __init__(self):
        self.configurations = {}
        self.current_config = None
    
    def add_config(self, name, params):
        """Add a new configuration."""
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        for key, value in params.items():
            if isinstance(value, dict):
                # Nested parameters
                for subkey, subvalue in value.items():
                    config.set(f"{key}.{subkey}", subvalue)
            else:
                config.set(key, value)
        
        self.configurations[name] = config
        print(f"✓ Added configuration '{name}'")
        return config
    
    def set_current(self, name):
        """Set the current configuration."""
        if name not in self.configurations:
            raise ValueError(f"Configuration '{name}' not found")
        
        self.current_config = self.configurations[name]
        print(f"✓ Current configuration set to '{name}'")
        return self.current_config
    
    def run_experiments(self, brain_factory, experiment_func, iterations=5, experiment_name="experiment"):
        """Run multiple experiments with different configurations."""
        if not self.current_config:
            raise RuntimeError("No current configuration set")
        
        results = []
        
        print(f"✓ Running experiment '{experiment_name}' ({iterations} iterations)")
        
        for i in range(iterations):
            # Create brain with current config
            brain = brain_factory(self.current_config)
            brain.initialize()
            
            # Run experiment
            result = experiment_func(brain, i)
            result['iteration'] = i
            result['config_name'] = list(self.configurations.keys())[
                list(self.configurations.values()).index(self.current_config)
            ]
            
            results.append(result)
            
            # Reset for next iteration
            brain.reset()
        
        print(f"✓ Experiment '{experiment_name}' completed")
        return results

# Example 7: Visualization and Analysis
print("\n7. Visualization and Analysis")
print("=" * 50)

class BrainAnalyzer:
    """Analyze brain data and generate insights."""
    
    @staticmethod
    def compare_brains(brain1, brain2, metric='spiking'):
        """Compare two brains based on specified metric."""
        if metric == 'spiking':
            val1 = brain1.getTotalSpikeCount()
            val2 = brain2.getTotalSpikeCount()
        elif metric == 'firing_rate':
            val1 = brain1.getAverageFiringRate()
            val2 = brain2.getAverageFiringRate()
        elif metric == 'activity':
            val1 = brain1.getActiveNeuronCount()
            val2 = brain2.getActiveNeuronCount()
        elif metric == 'synapses':
            val1 = brain1.getTotalSynapseCount()
            val2 = brain2.getTotalSynapseCount()
        else:
            raise ValueError(f"Unknown metric: {metric}")
        
        difference = abs(val1 - val2)
        average = (val1 + val2) / 2
        similarity = max(0, 1 - difference / (average + 1))
        
        return {
            'metric': metric,
            'brain1_value': val1,
            'brain2_value': val2,
            'difference': difference,
            'similarity': similarity,
            'better_brain': 1 if val1 > val2 else 2 if val2 > val1 else 0
        }
    
    @staticmethod
    def analyze_firing_patterns(spike_times, duration):
        """Analyze firing rate patterns over time."""
        if not spike_times:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        # Calculate instantaneous firing rate
        rates = []
        for i in range(len(spike_times) - 1):
            interval = spike_times[i + 1] - spike_times[i]
            if interval > 0:
                rates.append(1.0 / interval)
        
        if not rates:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        avg_rate = sum(rates) / len(rates)
        rate_variance = np.var(rates) if len(rates) > 1 else 0
        consistency = max(0, 1 - rate_variance / (avg_rate * avg_rate + 0.001))
        
        # Pattern detection
        if avg_rate < 0.1:
            pattern = 'quiescent'
        elif avg_rate < 1.0:
            pattern = 'irregular'
        elif rate_variance < avg_rate * 0.1:
            pattern = 'burst'
        else:
            pattern = 'regular'
        
        return {
            'avg_firing_rate': avg_rate,
            'consistency': consistency,
            'pattern': pattern,
            'total_spikes': len(spike_times),
            'irregularity': rate_variance / (avg_rate * avg_rate + 0.001)
        }

# === Complete Example Workflow ===

print("\n" + "=" * 60)
print("COMPLETE ENHANCED NLM WORKFLOW EXAMPLE")
print("=" * 60)

# Step 1: Setup configuration manager
print("\nStep 1: Setting up configuration manager...")
config_manager = ConfigManager()

# Step 2: Add different configurations
print("\nStep 2: Adding configurations...")
config1 = {
    'neuron_count': 500,
    'development_enabled': True,
    'curiosity_enabled': True,
    'reward_modulation_enabled': True
}

config2 = {
    'neuron_count': 1000,
    'development_enabled': False,
    'curiosity_enabled': True,
    'reward_modulation_enabled': False
}

config_manager.add_config('efficient', config1)
config_manager.add_config('complex', config2)

# Step 3: Set active configuration and run experiments
print("\nStep 3: Running experiments...")

config_manager.set_current('efficient')

def brain_factory(config):
    return pynlm.createBrain(config)

def experiment_func(brain, iteration):
    print(f"  Running iteration {iteration + 1}...")
    brain.initialize()
    
    # Monitor performance
    monitor = BrainMonitor(brain)
    monitor.start_monitoring()
    
    # Run simulation
    for step in range(200):
        monitor.step(step)
    
    stats = monitor.stop_monitoring()
    brain.reset()
    
    return {
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'monitoring_stats': stats
    }

results = config_manager.run_experiments(
    brain_factory, experiment_func, 
    iterations=2, experiment_name="efficiency_test"
)

# Step 4: Analyze results
print("\nStep 4: Analyzing results...")

spike_counts = [r['spike_count'] for r in results]
print(f"  Spike count range: {min(spike_counts)} - {max(spike_counts)}")
print(f"  Average spike count: {sum(spike_counts) / len(spike_counts):.0f}")

# Step 5: Advanced agent simulation
print("\nStep 5: Advanced agent simulation...")

# Create enhanced setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create AdvancedAgent wrapper
advanced_agent = AdvancedAgent(brain, world)

# Run episode with callback for monitoring
def episode_callback(step, percept, action, result):
    if step % 50 == 0:
        print(f"    Step {step}: Action {action}, Reward {result.reward:.3f}")

stats = advanced_agent.run_episode(max_steps=300, render_callback=episode_callback)

# Step 6: Recording and playback
print("\nStep 6: Neural recording and playback...")

recorder = NeuralRecorder()
recording_brain = recorder.start_recording(pynlm.createDefaultConfig(), "demo_recording")

for i in range(50):
    recorder.record_step(recording_brain, i)

recording = recorder.stop_recording()

# Play back (briefly)
playback_brain = recorder.play_recording("demo_recording")

print(f"  Recorded and played back {len(recording['data'])} steps")

# Step 7: Configuration analysis
print("\nStep 7: Configuration comparison...")

config_manager.set_current('complex')

complex_results = config_manager.run_experiments(
    brain_factory, experiment_func,
    iterations=2, experiment_name="complex_test"
)

# Compare configurations
comparison = BrainAnalyzer.compare_brains(
    pynlm.createBrain(config_manager.configurations['efficient']),
    pynlm.createBrain(config_manager.configurations['complex']),
    'spiking'
)

print(f"  Efficiency vs Complex comparison:")
print(f"    Spiking activity: {comparison['brain1_value']} vs {comparison['brain2_value']}")
print(f"    Similarity: {comparison['similarity']:.2f}")
print(f"    Better configuration: {'efficient' if comparison['better_brain'] == 1 else 'complex' if comparison['better_brain'] == 2 else 'equal'}")

# Summary
print("\n" + "=" * 60)
print("ENHANCED NLM WORKFLOW COMPLETE")
print("=" * 60)
print("✓ Configuration management")
print("✓ Performance monitoring")
print("✓ Neural recording and playback")
print("✓ Advanced agent simulation")
print("✓ Comparative analysis")
print("\nThe enhanced NLM Python API provides:")
print("• Context managers for resource safety")
print("• Better error handling and validation")
print("• Performance monitoring tools")
print("• Neural recording capabilities")
print("• Configuration management")
print("• Advanced simulation features")
print("• Analysis and visualization tools")
