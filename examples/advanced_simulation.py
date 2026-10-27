#!/usr/bin/env python3

"""
Example NLM Python Scripts

This file contains advanced examples for using NLM with Python.
It demonstrates various simulation patterns and techniques.
"""

import json
import time
import numpy as np
from pynlm import NLM, BrainConfig

class SimulationController:
    """Controller class for managing NLM simulations with advanced features"""
    
    def __init__(self, config_path=None):
        """Initialize simulation controller"""
        self.config = BrainConfig()
        
        if config_path:
            self.load_config(config_path)
        else:
            self.setup_default_config()
        
        self.brain = NLM(self.config)
        self.simulation_history = []
        self.reward_history = []
        self.performance_metrics = {}
    
    def setup_default_config(self):
        """Set up default configuration"""
        self.config.random_seed = 42
        self.config.neuron_count = 1000
        self.config.simulation_timestep = 0.001
        self.config.enable_checkpointing = True
        self.config.checkpoint_interval = 1000
        self.config.enable_profiling = True
        self.config.profile_interval = 100
    
    def load_config(self, config_path):
        """Load configuration from JSON file"""
        with open(config_path, 'r') as f:
            config_data = json.load(f)
        
        # Apply configuration
        for key, value in config_data.items():
            if hasattr(self.config, key):
                setattr(self.config, key, value)
    
    def run_simulation(self, steps, custom_reward=None, record_metrics=True):
        """Run simulation with optional custom reward function"""
        print(f"Starting simulation with {steps} steps...")
        print(f"Configuration: {self.config.neuron_count} neurons, "
              f"{self.config.simulation_timestep}s timestep")
        
        start_time = time.time()
        last_log_time = start_time
        
        for step in range(steps):
            current_time = time.time()
            timestep = current_time - last_log_time
            
            # Calculate reward if custom function provided
            reward = custom_reward(step, self.brain) if custom_reward else 0.0
            
            # Apply reward to brain
            if reward != 0.0:
                self.brain.applyRewardModulation(reward, 0.0)
                self.reward_history.append(reward)
            
            # Run brain step
            self.brain.step(step * self.config.simulation_timestep)
            
            # Record metrics if requested
            if record_metrics and step % self.config.profile_interval == 0:
                self.record_performance_metrics(step, timestep)
            
            # Log progress
            if step % 100 == 0:
                self.log_progress(step, current_time)
            
            last_log_time = current_time
        
        total_time = time.time() - start_time
        self.print_simulation_summary(steps, total_time)
        
        return self.simulation_history
    
    def record_performance_metrics(self, step, timestep):
        """Record performance metrics during simulation"""
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'timestep_duration': timestep,
            'spike_rate': self.brain.getTotalSpikeCount() / (step + 1),
            'activity_level': self.brain.getFiringNeuronCount() / 
                            max(1, self.config.neuron_count)
        }
        
        self.simulation_history.append(metrics)
    
    def log_progress(self, step, current_time):
        """Log simulation progress"""
        total_spikes = self.brain.getTotalSpikeCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        
        print(f"Step {step:6d} | "
              f"Spikes: {total_spikes:6d} | "
              f"Firing: {firing_neurons:4d} | "
              f"Time: {current_time-start_time:8.2f}s | "
              f"Rate: {total_spikes/(current_time-start_time):8.0f} spikes/s")
    
    def print_simulation_summary(self, steps, total_time):
        """Print simulation summary"""
        if not self.simulation_history:
            return
        
        total_spikes = self.brain.getTotalSpikeCount()
        avg_spikes_per_step = total_spikes / steps
        avg_step_time = total_time / steps
        
        print(f"\n=== Simulation Summary ===")
        print(f"Total steps: {steps}")
        print(f"Total simulation time: {total_time:.2f}s")
        print(f"Total spikes: {total_spikes}")
        print(f"Average spikes per step: {avg_spikes_per_step:.2f}")
        print(f"Average step time: {avg_step_time*1000:.2f}ms")
        print(f"Steps per second: {steps/total_time:.2f}")
        print(f"Total rewards: {len(self.reward_history)}")
        
        if self.reward_history:
            avg_reward = sum(self.reward_history) / len(self.reward_history)
            print(f"Average reward: {avg_reward:.4f}")
        
        if self.simulation_history:
            avg_spike_rate = np.mean([m['spike_rate'] for m in self.simulation_history])
            max_activity = max(m['activity_level'] for m in self.simulation_history)
            print(f"Average spike rate: {avg_spike_rate:.2f} spikes/step")
            print(f"Maximum activity level: {max_activity:.2%}")
    
    def save_simulation_data(self, filename):
        """Save simulation data to file"""
        data = {
            'config': {
                'random_seed': self.config.random_seed,
                'neuron_count': self.config.neuron_count,
                'simulation_timestep': self.config.simulation_timestep,
                'enable_checkpointing': self.config.enable_checkpointing,
                'checkpoint_interval': self.config.checkpoint_interval
            },
            'reward_history': self.reward_history,
            'simulation_history': self.simulation_history,
            'final_spikes': self.brain.getTotalSpikeCount(),
            'final_firing_neurons': self.brain.getFiringNeuronCount()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Simulation data saved to {filename}")
    
    def load_simulation_data(self, filename):
        """Load simulation data from file"""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.reward_history = data['reward_history']
        self.simulation_history = data['simulation_history']
        
        print(f"Loaded simulation data from {filename}")
        return data

class AdvancedSimulation( SimulationController):
    """Advanced simulation with custom learning algorithms"""
    
    def __init__(self, config_path=None):
        super().__init__(config_path)
        self.adaptation_factor = 1.0
        self.exploration_rate = 0.1
        self.exploitation_rate = 0.9
    
    def adaptive_learning(self, reward, prediction_error):
        """Implement adaptive learning based on reward prediction"""
        # Store experience
        self.reward_history.append(reward)
        
        # Calculate prediction error
        prediction_error = reward - self.get_expected_reward()
        
        # Apply adaptive learning rules
        self.update_learning_rates(prediction_error)
        self.apply_plasticity_rules(prediction_error)
        self adjust_exploration(prediction_error)
    
    def get_expected_reward(self):
        """Calculate expected reward based on recent experience"""
        if len(self.reward_history) < 10:
            return 0.0
        
        return sum(self.reward_history[-10:]) / len(self.reward_history[-10:])
    
    def update_learning_rates(self, prediction_error):
        """Update learning rates based on prediction error"""
        # Higher prediction error = more learning
        learning_rate = min(0.1, abs(prediction_error) * 2.0)
        
        # Adapt exploration and exploitation
        if prediction_error > 0.5:
            self.exploration_rate = min(1.0, self.exploration_rate + 0.1)
            self.exploitation_rate = max(0.0, self.exploitation_rate - 0.1)
        elif prediction_error < -0.5:
            self.exploitation_rate = min(1.0, self.exploitation_rate + 0.1)
            self.exploration_rate = max(0.0, self.exploration_rate - 0.1)
    
    def apply_plasticity_rules(self, prediction_error):
        """Apply synaptic plasticity rules"""
        # Hebbian-like plasticity based on prediction error
        plasticity_strength = abs(prediction_error) * self.adaptation_factor
        
        # This would typically modify synaptic weights in the brain
        # For demonstration, we just log the plasticity
        if plasticity_strength > 0.01:
            print(f"  High plasticity detected: {plasticity_strength:.4f}")
    
    def adjust_exploration(self, prediction_error):
        """Adjust exploration based on prediction error"""
        if abs(prediction_error) > 0.3:
            # High uncertainty = more exploration
            self.exploration_rate = min(1.0, self.exploration_rate + 0.05)
        else:
            # Low uncertainty = more exploitation
            self.exploration_rate = max(0.1, self.exploration_rate - 0.02)
    
    def run_adaptive_simulation(self, steps, environment_func):
        """Run simulation with adaptive learning"""
        print(f"Starting adaptive simulation with learning...")
        print(f"Initial exploration rate: {self.exploration_rate:.2f}")
        
        for step in range(steps):
            # Get environment feedback
            state = environment_func(step)
            
            # Calculate reward (simplified)
            reward = self.calculate_reward(state)
            
            # Apply adaptive learning
            self.adaptive_learning(reward, reward - 0.05)
            
            # Run brain step
            self.brain.step(step * self.config.simulation_timestep)
            
            # Log every 50 steps
            if step % 50 == 0:
                self.log_adaptive_progress(step, reward)
        
        print(f"Adaptive simulation completed. Final exploration rate: {self.exploration_rate:.2f}")
    
    def calculate_reward(self, state):
        """Calculate reward based on state"""
        # Simple reward function for demonstration
        if state.get('target_reached', False):
            return 1.0
        elif state.get('danger_detected', False):
            return -1.0
        elif state.get('novelty', False):
            return 0.5
        else:
            return 0.0
    
    def log_adaptive_progress(self, step, reward):
        """Log adaptive simulation progress"""
        total_spikes = self.brain.getTotalSpikeCount()
        print(f"Step {step:6d} | "
              f"Reward: {reward:6.3f} | "
              f"Exploration: {self.exploration_rate:5.2f} | "
              f"Exploitation: {self.exploitation_rate:5.2f} | "
              f"Spikes: {total_spikes:6d}")

class SimulationBatchRunner:
    """Run multiple simulations in batch mode"""
    
    def __init__(self):
        self.results = []
    
    def run_batch(self, configurations, steps):
        """Run batch of simulations with different configurations"""
        print(f"Running batch simulation with {len(configurations)} configurations...")
        
        for i, config in enumerate(configurations):
            print(f"\n--- Configuration {i+1}/{len(configurations)} ---")
            print(f"Neurons: {config['neurons']}, Steps: {steps}")
            
            # Create simulation
            sim = SimulationController()
            sim.config.random_seed = config['random_seed']
            sim.config.neuron_count = config['neurons']
            
            # Run simulation
            metrics = sim.run_simulation(steps)
            
            # Store results
            result = {
                'configuration': config,
                'metrics': metrics,
                'final_spikes': sim.brain.getTotalSpikeCount(),
                'final_firing_neurons': sim.brain.getFiringNeuronCount()
            }
            
            self.results.append(result)
        
        self.print_batch_summary()
        return self.results
    
    def print_batch_summary(self):
        """Print batch simulation summary"""
        print(f"\n=== Batch Simulation Summary ===")
        print(f"Total configurations: {len(self.results)}")
        
        total_spikes = sum(r['final_spikes'] for r in self.results)
        avg_spikes = total_spikes / len(self.results)
        
        print(f"Total spikes across all configurations: {total_spikes}")
        print(f"Average spikes per configuration: {avg_spikes:.1f}")
        
        # Find best performing configuration
        best_config = max(self.results, key=lambda x: x['final_spikes'])
        print(f"Best performing configuration: {best_config['configuration']}")
        print(f"Best spikes: {best_config['final_spikes']}")

# Example reward functions
def simple_reward_function(step, brain):
    """Simple reward function based on spike activity"""
    spike_count = brain.getTotalSpikeCount()
    
    # Reward spikes if above threshold
    if spike_count % 100 == 0:
        return 0.1
    
    # Small negative reward for every step
    return -0.001

def exploration_reward_function(step, brain):
    """Reward exploration and novelty"""
    firing_neurons = brain.getFiringNeuronCount()
    
    # Encourage exploration (high firing)
    if firing_neurons > brain.config.neuron_count * 0.1:
        return 0.5
    
    # Encourage stability (medium firing)
    elif firing_neurons > brain.config.neuron_count * 0.05:
        return 0.1
    
    # Discourage inactivity (low firing)
    else:
        return -0.2

# Example environment function
def grid_world_environment(step):
    """Simple grid world environment for testing"""
    # Simulate grid world dynamics
    x = (step * 7) % 10
    y = (step * 3) % 10
    
    # Check for goal
    goal_reached = (x == 9 and y == 9)
    
    # Check for danger
    danger_detected = (x == 0 and y == 0)
    
    # Check for novelty (new positions)
    novelty = (step % 20 == 0)
    
    return {
        'x': x,
        'y': y,
        'goal_reached': goal_reached,
        'danger_detected': danger_detected,
        'novelty': novelty
    }

def main():
    """Main function demonstrating various simulation patterns"""
    print("NLM Advanced Python Examples")
    print("=" * 40)
    
    # Example 1: Basic simulation
    print("\n1. Basic Simulation Example")
    sim1 = SimulationController()
    metrics1 = sim1.run_simulation(500)
    sim1.save_simulation_data("basic_simulation.json")
    
    # Example 2: Adaptive learning simulation
    print("\n2. Adaptive Learning Simulation Example")
    sim2 = AdvancedSimulation()
    sim2.run_adaptive_simulation(300, grid_world_environment)
    
    # Example 3: Batch simulation
    print("\n3. Batch Simulation Example")
    runner = SimulationBatchRunner()
    configs = [
        {'random_seed': 42, 'neurons': 500},
        {'random_seed': 123, 'neurons': 1000},
        {'random_seed': 456, 'neurons': 2000}
    ]
    results = runner.run_batch(configs, 1000)
    
    # Save batch results
    with open('batch_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print("\nAll examples completed successfully!")

if __name__ == '__main__':
    main()
