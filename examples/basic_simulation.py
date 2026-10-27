#!/usr/bin/env python3

"""
Example NLM Python Scripts

These examples demonstrate advanced usage of NLM with Python scripting.
"""

import json
import time
import argparse
from pathlib import Path
from pynlm import NLM, BrainConfig

class NLMSimulation:
    """Advanced NLM simulation with custom learning and monitoring"""
    
    def __init__(self, config_file=None):
        """Initialize simulation with configuration"""
        self.config = BrainConfig()
        
        if config_file:
            self.load_config(config_file)
        else:
            # Default configuration
            self.config.random_seed = 42
            self.config.neuron_count = 1000
            self.config.simulation_timestep = 0.001
        
        self.brain = NLM(self.config)
        self.step_count = 0
        self.metrics = []
    
    def load_config(self, config_file):
        """Load configuration from JSON file"""
        with open(config_file, 'r') as f:
            config_dict = json.load(f)
        
        # Apply configuration
        for key, value in config_dict.items():
            if hasattr(self.config, key):
                setattr(self.config, key, value)
    
    def run_simulation(self, steps, timestep=None):
        """Run simulation with specified number of steps"""
        if timestep:
            self.config.simulation_timestep = timestep
        
        print(f"Running simulation for {steps} steps...")
        print(f"Configuration: {self.config.neuron_count} neurons, "
              f"{self.config.simulation_timestep}s timestep")
        
        start_time = time.time()
        
        for i in range(steps):
            step_start = time.time()
            
            # Run brain step
            self.brain.step(i * self.config.simulation_timestep)
            
            # Collect metrics
            metrics = self.collect_metrics(i, step_start)
            self.metrics.append(metrics)
            
            # Log progress
            if i % 100 == 0:
                self.log_progress(i, metrics)
        
        end_time = time.time()
        
        # Print summary
        self.print_summary(start_time, end_time)
        
        return self.metrics
    
    def collect_metrics(self, step, step_start_time):
        """Collect simulation metrics"""
        return {
            'step': step,
            'timestamp': time.time(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'step_duration': time.time() - step_start_time,
            'total_duration': time.time() - step * self.config.simulation_timestep
        }
    
    def log_progress(self, step, metrics):
        """Log simulation progress"""
        print(f"Step {step:6d} | "
              f"Spikes: {metrics['total_spikes']:6d} | "
              f"Firing: {metrics['firing_neurons']:4d} | "
              f"Step time: {metrics['step_duration']*1000:6.2f}ms | "
              f"Total time: {metrics['total_duration']:8.2f}s")
    
    def print_summary(self, start_time, end_time):
        """Print simulation summary"""
        total_spikes = self.brain.getTotalSpikeCount()
        total_time = end_time - start_time
        avg_spikes_per_step = total_spikes / len(self.metrics) if self.metrics else 0
        
        print(f"\n=== Simulation Summary ===")
        print(f"Total steps: {len(self.metrics)}")
        print(f"Total simulation time: {total_time:.2f}s")
        print(f"Total spikes: {total_spikes}")
        print(f"Average spikes per step: {avg_spikes_per_step:.2f}")
        print(f"Steps per second: {len(self.metrics)/total_time:.2f}")
    
    def save_results(self, filename):
        """Save simulation results to file"""
        with open(filename, 'w') as f:
            json.dump({
                'config': {
                    'random_seed': self.config.random_seed,
                    'neuron_count': self.config.neuron_count,
                    'simulation_timestep': self.config.simulation_timestep
                },
                'metrics': self.metrics
            }, f, indent=2)
        
        print(f"Results saved to {filename}")
    
    def load_results(self, filename):
        """Load simulation results from file"""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.metrics = data['metrics']
        print(f"Loaded results from {filename}")
        return self.metrics

class CustomLearningSimulation(NLMSimulation):
    """Simulation with custom learning algorithms"""
    
    def __init__(self, config_file=None):
        super().__init__(config_file)
        self.reward_history = []
        self.error_history = []
        self.learning_rate = 0.01
    
    def adaptive_learning(self, reward, prediction_error):
        """Implement adaptive learning based on reward and prediction error"""
        # Store history
        self.reward_history.append(reward)
        self.error_history.append(prediction_error)
        
        # Apply learning rules
        self.apply_adaptive_rules(reward, prediction_error)
    
    def apply_adaptive_rules(self, reward, prediction_error):
        """Apply adaptive learning rules"""
        # Calculate learning rate based on recent performance
        if len(self.reward_history) >= 10:
            recent_rewards = self.reward_history[-10:]
            avg_reward = sum(recent_rewards) / len(recent_rewards)
            
            # Adjust learning rate based on reward stability
            if avg_reward > 0.5:
                self.learning_rate = 0.005
            elif avg_reward < -0.5:
                self.learning_rate = 0.02
            else:
                self.learning_rate = 0.01
        
        # Apply synaptic changes based on prediction error
        synaptic_change = prediction_error * self.learning_rate
        # This would typically call into NLM's plasticity system
    
    def run_custom_simulation(self, steps, reward_func):
        """Run simulation with custom reward function"""
        print(f"Running custom simulation with adaptive learning...")
        print(f"Initial learning rate: {self.learning_rate}")
        
        for i in range(steps):
            # Get reward from custom function
            reward = reward_func(i, self.brain)
            
            # Get prediction error (placeholder)
            prediction_error = reward - 0.05
            
            # Apply adaptive learning
            self.adaptive_learning(reward, prediction_error)
            
            # Run brain step
            self.brain.step(i * self.config.simulation_timestep)
            
            # Log progress every 100 steps
            if i % 100 == 0:
                print(f"Step {i:6d} | "
                      f"Reward: {reward:6.3f} | "
                      f"Learning rate: {self.learning_rate:6.3f} | "
                      f"Spikes: {self.brain.getTotalSpikeCount():6d}")

class BenchmarkSimulation(NLMSimulation):
    """Simulation for performance benchmarking"""
    
    def run_benchmark(self, neurons_list, steps_list):
        """Run benchmark tests with different configurations"""
        print("Starting NLM performance benchmark...")
        
        results = []
        
        for neurons in neurons_list:
            for steps in steps_list:
                print(f"\nBenchmark: {neurons} neurons, {steps} steps")
                
                # Create brain with specific configuration
                config = BrainConfig()
                config.random_seed = 42
                config.neuron_count = neurons
                config.simulation_timestep = 0.001
                
                brain = NLM(config)
                
                # Measure performance
                start_time = time.time()
                
                for i in range(steps):
                    brain.step(i * config.simulation_timestep)
                
                end_time = time.time()
                
                duration = end_time - start_time
                spikes_per_second = brain.getTotalSpikeCount() / duration
                steps_per_second = steps / duration
                
                result = {
                    'neurons': neurons,
                    'steps': steps,
                    'duration': duration,
                    'spikes': brain.getTotalSpikeCount(),
                    'spikes_per_second': spikes_per_second,
                    'steps_per_second': steps_per_second,
                    'time_per_step': duration / steps
                }
                
                results.append(result)
                
                print(f"  Duration: {duration:.2f}s")
                print(f"  Spikes: {brain.getTotalSpikeCount()}")
                print(f"  Spikes/sec: {spikes_per_second:.0f}")
                print(f"  Steps/sec: {steps_per_second:.0f}")
                print(f"  Time per step: {duration/steps*1000:.2f}ms")
        
        return results

def main():
    parser = argparse.ArgumentParser(description="NLM Python Examples")
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Basic simulation
    basic_parser = subparsers.add_parser('basic', help='Run basic simulation')
    basic_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    basic_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    basic_parser.add_argument('--config', help='Configuration file')
    basic_parser.add_argument('--output', help='Output file for results')
    
    # Custom learning simulation
    learning_parser = subparsers.add_parser('learning', help='Run custom learning simulation')
    learning_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    learning_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    
    # Benchmark simulation
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
    benchmark_parser.add_argument('--neurons', nargs='+', type=int, default=[100, 500, 1000], help='List of neuron counts')
    benchmark_parser.add_argument('--steps', nargs='+', type=int, default=[100, 1000], help='List of step counts')
    
    args = parser.parse_args()
    
    if args.command == 'basic':
        simulation = NLMSimulation(args.config)
        simulation.config.neuron_count = args.neurons
        
        metrics = simulation.run_simulation(args.steps)
        
        if args.output:
            simulation.save_results(args.output)
    
    elif args.command == 'learning':
        simulation = CustomLearningSimulation()
        simulation.config.neuron_count = args.neurons
        
        # Custom reward function
        def custom_reward(step, brain):
            # Example reward function
            if step % 50 == 0:
                return 0.1  # Small positive reward
            elif step % 25 == 0:
                return -0.1  # Small negative reward
            else:
                return 0.0  # No reward
        
        simulation.run_custom_simulation(args.steps, custom_reward)
    
    elif args.command == 'benchmark':
        simulation = BenchmarkSimulation()
        results = simulation.run_benchmark(args.neurons, args.steps)
        
        # Save benchmark results
        with open('benchmark_results.json', 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Benchmark results saved to benchmark_results.json")
    
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
