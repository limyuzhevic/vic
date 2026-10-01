# Advanced Usage Guide

This document provides advanced usage patterns and examples for the NLM framework, building on the easy usage concepts.

## Advanced Topics

### 1. Advanced Configuration Management

#### Configuration Presets

NLM provides several predefined configuration presets for different use cases:

```python
import pynlm

# Standard configuration (recommended for most users)
standard_config = pynlm.create_default_config()

# Research configuration (more complex, for research)
research_config = pynlm.create_advanced_config()

# Minimal configuration (for quick testing)
minimal_config = pynlm.create_minimal_config()

# Custom configuration
from pynlm import BrainPresets
import json

custom_config = BrainPresets.createResearchBrain()
custom_config["neuron_count"] = 3000
custom_config["enable_structural_plasticity"] = True
```

#### Configuration Validation

```python
import pynlm

# Validate configuration before creating brain
config = pynlm.create_advanced_config()
valid_keys = ["neuron_count", "region_count", "connection_probability", 
              "stdp_ltp_weight", "stdp_ltd_weight", "stdp_time_constant"]

# Check if configuration is valid
for key in valid_keys:
    if not config.has(key):
        print(f"Warning: Missing recommended configuration key: {key}")
    else:
        value = config.getOr(key, 0)
        print(f"Config {key}: {value}")

# Create brain with validation
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
    print("Brain created successfully with validation!")
except Exception as e:
    print(f"Brain creation failed: {e}")
```

### 2. Advanced Learning Scenarios

#### Multi-Episode Learning

```python
import pynlm

class LearningEnvironment:
    def __init__(self, brain_size=500, world_size=(20, 20)):
        self.brain_size = brain_size
        self.world_width, self.world_height = world_size
        
        # Create brain and environment
        config = pynlm.create_advanced_config()
        config.set("neuron_count", brain_size)
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("enable_development", True)
        
        self.brain = pynlm.create_brain(config)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            float(world_size[0]), float(world_size[1]),
            8, 8
        )
        
        self.agent = pynlm.create_agent_brain(self.brain)
        self.agent.initialize(self.world)
        
        # Learning metrics
        self.episode_rewards = []
        self.episode_steps = []
    
    def reset(self):
        """Reset environment for new episode"""
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        return self.get_state()
    
    def get_state(self):
        """Get current state as feature vector"""
        percept = self.world.getSensoryPercept()
        return percept.getAllSignals()
    
    def step(self, action, reward):
        """Take a step in the environment"""
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update world
        self.world.update(0.1)
        
        # Process sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Brain processing
        current_step = int(self.world.getSimulationTime() / 0.1)
        self.brain.step(current_step)
        
        # Apply reward modulation
        predicted_reward = self.agent.getNeuromodulationLevel()
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Return next state
        return self.get_state(), reward, self.world.getSimulationTime() == 0.9
    
    def run_episode(self, max_steps=100, explore=True):
        """Run a single episode"""
        state = self.reset()
        total_reward = 0
        steps = 0
        
        for step in range(max_steps):
            if explore:
                # Simple exploration policy
                action = self.agent.decodeMotorCommand()
            else:
                # Exploitation policy (would use learned policy in real scenario)
                action = self.agent.decodeMotorCommand()
            
            # Get reward based on position (example)
            agent_pos = self.world.getAgentBody()
            if agent_pos.x < 5.0:  # Near left edge
                reward = 0.5
            elif agent_pos.x > 15.0:  # Near right edge
                reward = 1.0
            else:
                reward = 0.1
            
            state, reward, done = self.step(action, reward)
            total_reward += reward
            steps += 1
            
            if done or steps >= max_steps:
                break
        
        self.episode_rewards.append(total_reward)
        self.episode_steps.append(steps)
        
        return total_reward, steps
    
    def train(self, num_episodes=100, exploration_rate=0.3):
        """Train the agent"""
        print(f"Starting training for {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            explore = bool(random.random() < exploration_rate)
            
            reward, steps = self.run_episode(explore=explore)
            
            if episode % 10 == 0:
                avg_reward = sum(self.episode_rewards[-10:]) / 10
                avg_steps = sum(self.episode_steps[-10:]) / 10
                print(f"Episode {episode}: Reward = {reward:.2f}, Steps = {steps}, "
                      f"Avg Reward (last 10) = {avg_reward:.2f}, "
                      f"Avg Steps (last 10) = {avg_steps:.1f}")
        
        print(f"Training completed! Average reward: {sum(self.episode_rewards) / num_episodes:.2f}")
```

### 3. Performance Monitoring and Analysis

#### Real-time Performance Tracking

```python
import pynlm
import time
import json
from datetime import datetime

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics = {
            'steps_completed': 0,
            'total_spike_count': 0,
            'average_firing_rate': 0.0,
            'time_per_step': [],
            'memory_usage': [],
            'learning_progress': []
        }
        self.start_time = time.time()
        self.step_count = 0
    
    def record_step(self, start_time, end_time):
        """Record metrics for a single step"""
        step_duration = end_time - start_time
        self.metrics['steps_completed'] += 1
        self.metrics['time_per_step'].append(step_duration)
        
        # Record brain statistics
        stats = pynlm.get_brain_stats(self.brain)
        self.metrics['total_spike_count'] = stats['total_spikes']
        self.metrics['average_firing_rate'] = stats['average_firing_rate']
        
        # Calculate learning progress (simplified)
        if self.step_count > 0:
            learning_progress = (stats['average_firing_rate'] - self.metrics['average_firing_rate']) / self.metrics['average_firing_rate']
        else:
            learning_progress = 0.0
        self.metrics['learning_progress'].append(learning_progress)
        
        self.step_count += 1
    
    def get_performance_report(self):
        """Get comprehensive performance report"""
        total_time = time.time() - self.start_time
        
        if len(self.metrics['time_per_step']) > 0:
            avg_step_time = sum(self.metrics['time_per_step']) / len(self.metrics['time_per_step'])
            steps_per_second = 1.0 / avg_step_time if avg_step_time > 0 else 0
        else:
            avg_step_time = 0
            steps_per_second = 0
        
        report = {
            'performance_summary': {
                'total_steps': self.metrics['steps_completed'],
                'total_time_seconds': total_time,
                'steps_per_second': steps_per_second,
                'average_step_duration_ms': avg_step_time * 1000,
                'total_spikes': self.metrics['total_spike_count'],
                'current_firing_rate': self.metrics['average_firing_rate']
            },
            'learning_trends': {
                'final_learning_progress': self.metrics['learning_progress'][-1] if self.metrics['learning_progress'] else 0,
                'average_learning_progress': sum(self.metrics['learning_progress']) / len(self.metrics['learning_progress']) if self.metrics['learning_progress'] else 0,
                'learning_stagnation_detected': self._detect_stagnation()
            },
            'resource_usage': {
                'estimated_memory_mb': self._estimate_memory_usage(),
                'cpu_usage_percent': self._estimate_cpu_usage()
            }
        }
        
        return report
    
    def _detect_stagnation(self):
        """Detect if learning has stagnated"""
        if len(self.metrics['learning_progress']) < 10:
            return False
        
        recent_progress = self.metrics['learning_progress'][-10:]
        avg_recent = sum(recent_progress) / len(recent_progress)
        
        # Check if recent progress is below threshold
        return avg_recent < 0.01
    
    def _estimate_memory_usage(self):
        """Estimate memory usage in MB"""
        # Simplified estimation based on brain statistics
        stats = pynlm.get_brain_stats(self.brain)
        neuron_count = stats['total_neurons']
        synapse_count = stats['total_synapses']
        
        # Rough estimate: ~100 bytes per neuron, ~200 bytes per synapse
        estimated_mb = (neuron_count * 100 + synapse_count * 200) / (1024 * 1024)
        return max(estimated_mb, 1.0)  # Minimum 1 MB
    
    def _estimate_cpu_usage(self):
        """Estimate CPU usage percentage"""
        if len(self.metrics['time_per_step']) == 0:
            return 0.0
        
        # Based on step timing (simplified)
        avg_step_time = sum(self.metrics['time_per_step']) / len(self.metrics['time_per_step'])
        if avg_step_time > 0:
            # Assuming 1 CPU core, convert to percentage
            return min(100.0, (1.0 / avg_step_time) * 100)
        return 0.0
    
    def save_metrics(self, filepath):
        """Save performance metrics to file"""
        report = self.get_performance_report()
        report['timestamp'] = datetime.now().isoformat()
        
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Performance metrics saved to {filepath}")
```

### 4. Advanced Error Handling and Recovery

#### Robust Simulation with Error Recovery

```python
import pynlm
import traceback

class RobustSimulation:
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.brain = None
        self.world = None
        self.agent = None
        self.max_retries = 3
        self.error_count = 0
    
    def initialize(self):
        """Initialize simulation with error handling"""
        try:
            if self.config_file:
                # Load configuration from file
                self.brain = pynlm.load_config_from_json(self.config_file)
            else:
                # Create default configuration
                self.brain = pynlm.create_default_config()
            
            self.brain = pynlm.create_brain(self.brain)
            
            # Enable error logging
            self.brain.enableDebugMode(True)
            
            # Create environment
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            
            self.agent = pynlm.create_agent_brain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable learning
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            
            print("Simulation initialized successfully!")
            return True
            
        except Exception as e:
            print(f"Simulation initialization failed: {e}")
            print("Attempting recovery...")
            
            # Try to recover with minimal configuration
            try:
                self.brain = pynlm.create_minimal_brain({})
                self.brain = pynlm.create_brain(self.brain)
                self.brain.initialize()
                
                self.world = pynlm.createSimpleWorld()
                self.world.configure(10, 10, 6, 6)
                
                self.agent = pynlm.create_agent_brain(self.brain)
                self.agent.initialize(self.world)
                
                print("Recovery successful! Using minimal configuration.")
                return True
                
            except Exception as recovery_error:
                print(f"Recovery failed: {recovery_error}")
                return False
    
    def run_step(self, action):
        """Run a single step with error handling"""
        retry_count = 0
        
        while retry_count < self.max_retries:
            try:
                # Validate action
                if not isinstance(action, int):
                    raise ValueError(f"Invalid action type: {type(action)}")
                
                # Get current simulation time
                current_time = self.world.getSimulationTime()
                
                # Apply action
                result = self.world.applyMotorCommand(action, current_time)
                
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Brain processing
                step_number = int(current_time / 0.1)
                self.brain.step(step_number)
                
                # Get action from agent
                agent_action = self.agent.decodeMotorCommand()
                
                # Return result
                return {
                    'success': True,
                    'result': result,
                    'action': agent_action,
                    'simulation_time': self.world.getSimulationTime()
                }
                
            except Exception as e:
                retry_count += 1
                print(f"Step failed (attempt {retry_count}/{self.max_retries}): {e}")
                
                if retry_count >= self.max_retries:
                    # Log error and return error result
                    print(f"Step failed after {self.max_retries} attempts: {e}")
                    return {
                        'success': False,
                        'error': str(e),
                        'retry_count': retry_count
                    }
                
                # Small delay before retry
                time.sleep(0.01)
        
        return {
            'success': False,
            'error': f"Failed after {self.max_retries} attempts",
            'retry_count': self.max_retries
        }
    
    def run_episode(self, max_steps=100):
        """Run a complete episode with error recovery"""
        print(f"Starting episode with up to {max_steps} steps...")
        
        episode_start_time = time.time()
        total_reward = 0
        steps_completed = 0
        errors_occurred = 0
        
        for step in range(max_steps):
            try:
                # Simple exploration policy
                action = self.agent.decodeMotorCommand()
                
                # Run step
                step_result = self.run_step(action)
                
                if step_result['success']:
                    # Calculate reward (simplified)
                    reward = 0.1  # Base reward
                    if step % 10 == 0:  # Bonus for every 10 steps
                        reward += 0.5
                    
                    total_reward += reward
                    steps_completed += 1
                    
                    if step % 20 == 0:
                        print(f"Step {step}: Reward = {reward:.2f}, Total = {total_reward:.2f}")
                else:
                    errors_occurred += 1
                    print(f"Step {step}: Error occurred - {step_result['error']}")
                
            except Exception as e:
                errors_occurred += 1
                print(f"Episode error at step {step}: {e}")
                traceback.print_exc()
                
                # Try to continue despite error
                if errors_occurred > max_steps // 10:
                    print(f"Too many errors ({errors_occurred}), stopping episode")
                    break
        
        episode_duration = time.time() - episode_start_time
        
        print(f"Episode completed!")
        print(f"  Steps: {steps_completed}/{max_steps}")
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Errors: {errors_occurred}")
        print(f"  Duration: {episode_duration:.2f}s")
        
        return {
            'total_reward': total_reward,
            'steps_completed': steps_completed,
            'errors': errors_occurred,
            'duration': episode_duration,
            'success_rate': steps_completed / max_steps
        }
```

### 5. Advanced Configuration Management

#### Dynamic Configuration Adjustment

```python
import pynlm
import json

class DynamicConfigurator:
    def __init__(self, brain):
        self.brain = brain
        self.config_history = []
        self.performance_metrics = {}
    
    def adjust_configuration(self, target_performance=None):
        """Dynamically adjust configuration based on performance"""
        current_config = self.brain.getConfigAsJson()
        
        # Record current configuration
        self.config_history.append({
            'timestamp': time.time(),
            'config': current_config.copy(),
            'performance': self.performance_metrics
        })
        
        # Get current performance
        stats = pynlm.get_brain_stats(self.brain)
        
        # Simple adaptive configuration adjustment
        adjustments = []
        
        # Adjust neuron count based on performance
        if stats['average_firing_rate'] > 20.0:  # Too high firing rate
            current_config['neuron_count'] = max(100, current_config['neuron_count'] - 50)
            adjustments.append(f"Reduced neuron count to {current_config['neuron_count']}")
        
        elif stats['average_firing_rate'] < 5.0:  # Too low firing rate
            current_config['neuron_count'] = min(2000, current_config['neuron_count'] + 50)
            adjustments.append(f"Increased neuron count to {current_config['neuron_count']}")
        
        # Adjust connection probability
        if stats['excitatory_inhibition_ratio'] > 2.0:  # Too much excitation
            current_config['connection_probability'] = max(0.05, current_config['connection_probability'] - 0.02)
            adjustments.append(f"Reduced connection probability to {current_config['connection_probability']:.2f}")
        
        elif stats['excitatory_inhibition_ratio'] < 0.5:  # Too much inhibition
            current_config['connection_probability'] = min(0.3, current_config['connection_probability'] + 0.02)
            adjustments.append(f"Increased connection probability to {current_config['connection_probability']:.2f}")
        
        # Update brain configuration
        if adjustments:
            print("Configuration adjustments:")
            for adjustment in adjustments:
                print(f"  {adjustment}")
            
            # Apply new configuration
            self.brain.updateConfig(current_config)
            
            # Reset and re-initialize with new configuration
            self.brain.reset()
            # Note: In practice, you might want to save and reload the brain
            
            print("Configuration dynamically adjusted!")
        
        return adjustments
    
    def create_config_variants(self):
        """Create different configuration variants for comparison"""
        variants = {}
        
        # Standard variant
        standard = pynlm.create_default_config()
        variants['standard'] = standard
        
        # Research variant
        research = pynlm.create_advanced_config()
        research.set("neuron_count", 2000)
        research.set("enable_structural_plasticity", True)
        variants['research'] = research
        
        # Minimal variant
        minimal = pynlm.create_minimal_config()
        variants['minimal'] = minimal
        
        # Custom variant
        custom = pynlm.create_default_config()
        custom.set("neuron_count", 1000)
        custom.set("enable_reward_modulation", True)
        custom.set("enable_curiosity", True)
        custom.set("enable_development", True)
        variants['custom'] = custom
        
        return variants
    
    def compare_variants(self, variants):
        """Compare performance of different configuration variants"""
        print("Comparing configuration variants...")
        
        results = {}
        
        for name, config in variants.items():
            print(f"\nTesting variant: {name}")
            
            # Create brain with variant configuration
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            # Run test simulation
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            # Collect statistics
            stats = pynlm.get_brain_stats(brain)
            
            results[name] = {
                'firing_rate': stats['average_firing_rate'],
                'total_spikes': stats['total_spikes'],
                'neurons_active': stats['active_neurons'],
                'simulation_time': end_time - start_time,
                'steps_per_second': 100 / (end_time - start_time)
            }
            
            print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
            print(f"  Total spikes: {stats['total_spikes']}")
            print(f"  Simulation time: {end_time - start_time:.2f}s")
            print(f"  Steps per second: {100 / (end_time - start_time):.1f}")
        
        # Find best variant
        best_variant = max(results.items(), key=lambda x: x[1]['steps_per_second'])
        print(f"\nBest performing variant: {best_variant[0]}")
        
        return results, best_variant[0]
```

### 6. Advanced Visualization and Analysis

#### Brain Activity Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class BrainVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.activity_history = []
        self.spike_history = []
    
    def record_activity(self):
        """Record current brain activity"""
        stats = pynlm.get_brain_stats(self.brain)
        
        self.activity_history.append({
            'step': len(self.activity_history),
            'firing_rate': stats['average_firing_rate'],
            'total_spikes': stats['total_spikes'],
            'neurons_active': stats['active_neurons'],
            'excitatory_inhibition_ratio': stats['excitatory_inhibition_ratio']
        })
    
    def create_activity_plots(self):
        """Create comprehensive activity plots"""
        if not self.activity_history:
            print("No activity data to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("Brain Activity Analysis", fontsize=16)
        
        steps = [data['step'] for data in self.activity_history]
        firing_rates = [data['firing_rate'] for data in self.activity_history]
        spike_counts = [data['total_spikes'] for data in self.activity_history]
        active_neurons = [data['neurons_active'] for data in self.activity_history]
        excitation_inhibition = [data['excitatory_inhibition_ratio'] for data in self.activity_history]
        
        # Plot 1: Firing rate over time
        axes[0, 0].plot(steps, firing_rates, 'b-', linewidth=2, alpha=0.7)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Rate (Hz)')
        axes[0, 0].set_title('Firing Rate Over Time')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Total spikes over time
        axes[0, 1].plot(steps, spike_counts, 'r-', linewidth=2, alpha=0.7)
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Total Spikes')
        axes[0, 1].set_title('Cumulative Spike Count')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Active neurons over time
        axes[1, 0].plot(steps, active_neurons, 'g-', linewidth=2, alpha=0.7)
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Active Neurons')
        axes[1, 0].set_title('Active Neurons Over Time')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Excitatory/inhibition ratio
        axes[1, 1].plot(steps, excitation_inhibition, 'm-', linewidth=2, alpha=0.7)
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('E/I Ratio')
        axes[1, 1].set_title('Excitatory/Inhibition Ratio')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()
        
        return fig
    
    def create_stability_analysis(self):
        """Analyze stability of brain activity"""
        if len(self.activity_history) < 10:
            print("Insufficient data for stability analysis")
            return
        
        # Calculate rolling statistics
        window_size = 10
        steps = [data['step'] for data in self.activity_history]
        firing_rates = [data['firing_rate'] for data in self.activity_history]
        
        # Calculate rolling mean and standard deviation
        rolling_means = []
        rolling_stds = []
        
        for i in range(len(firing_rates) - window_size + 1):
            window = firing_rates[i:i + window_size]
            rolling_means.append(np.mean(window))
            rolling_stds.append(np.std(window))
        
        # Create stability plot
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
        
        # Stability over time
        ax1.plot(steps[window_size-1:], rolling_means, 'b-', linewidth=2, label='Rolling Mean')
        ax1.fill_between(steps[window_size-1:], 
                         np.array(rolling_means) - np.array(rolling_stds),
                         np.array(rolling_means) + np.array(rolling_stds),
                         alpha=0.3, label='±1 std')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Firing Rate (Hz)')
        ax1.set_title('Activity Stability Analysis')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Stability score (lower std = more stable)
        stability_scores = [1.0 / (std + 0.01) for std in rolling_stds]
        ax2.plot(steps[window_size-1:], stability_scores, 'g-', linewidth=2)
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Stability Score (higher = more stable)')
        ax2.set_title('Activity Stability Score')
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()
        
        return fig
    
    def create_comprehensive_report(self, output_file=None):
        """Generate comprehensive analysis report"""
        if not self.activity_history:
            print("No activity data available")
            return
        
        stats = pynlm.get_brain_stats(self.brain)
        
        report = {
            'brain_statistics': {
                'total_neurons': stats['total_neurons'],
                'total_synapses': stats['total_synapses'],
                'active_neurons': stats['active_neurons'],
                'firing_neurons': stats['firing_neurons'],
                'average_firing_rate': stats['average_firing_rate'],
                'excitatory_inhibition_ratio': stats['excitatory_inhibition_ratio'],
                'total_spikes': stats['total_spikes']
            },
            'activity_summary': {
                'total_steps_recorded': len(self.activity_history),
                'average_firing_rate': np.mean([d['firing_rate'] for d in self.activity_history]),
                'max_firing_rate': np.max([d['firing_rate'] for d in self.activity_history]),
                'min_firing_rate': np.min([d['firing_rate'] for d in self.activity_history]),
                'average_spike_count': np.mean([d['total_spikes'] for d in self.activity_history]),
                'stability_score': self._calculate_stability_score()
            }
        }
        
        print("Comprehensive Brain Activity Report")
        print("=" * 50)
        print(f"Brain Statistics:")
        for key, value in report['brain_statistics'].items():
            print(f"  {key}: {value}")
        
        print(f"\nActivity Summary:")
        for key, value in report['activity_summary'].items():
            if isinstance(value, float):
                print(f"  {key}: {value:.2f}")
            else:
                print(f"  {key}: {value}")
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
            print(f"\nReport saved to {output_file}")
        
        return report
    
    def _calculate_stability_score(self):
        """Calculate overall stability score"""
        if len(self.activity_history) < 10:
            return 0.0
        
        firing_rates = [d['firing_rate'] for d in self.activity_history]
        
        # Calculate coefficient of variation (std/mean)
        mean_fr = np.mean(firing_rates)
        std_fr = np.std(firing_rates)
        
        if mean_fr > 0:
            cv = std_fr / mean_fr
            # Convert to stability score (0-1, where 1 is most stable)
            stability = max(0.0, 1.0 - cv)
        else:
            stability = 0.0
        
        return stability