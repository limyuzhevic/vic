#!/usr/bin/env python3
"""
Advanced NLM Command Line Interface

This CLI provides enhanced functionality for advanced NLM users, including:
- Batch simulations with custom configurations
- Performance monitoring and profiling
- Experiment management
- Visualization and analysis
- Advanced configuration options
"""

import argparse
import json
import logging
import sys
import time
import os
from typing import Dict, List, Optional, Any
import numpy as np

# Import NLM modules
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features will be limited.")

class NLMAdvancedCLI:
    """Advanced NLM CLI with batch processing, analysis, and experiment management."""
    
    def __init__(self):
        self.setup_logging()
        self.experiments = []
        self.batch_results = []
        
    def setup_logging(self):
        """Setup advanced logging for the CLI."""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('nlm_advanced_cli.log'),
                logging.StreamHandler(sys.stdout)
            ]
        )
        self.logger = logging.getLogger(__name__)
        
    def create_advanced_config(self, args):
        """Create advanced configuration from CLI arguments."""
        config = pynlm.createDefaultConfig()
        
        # Try to set basic config values if the API supports it
        if hasattr(config, 'set'):
            # Basic settings
            config.set("random_seed", args.random_seed)
            config.set("simulation_timestep", args.timestep)
            
            # Advanced settings
            if hasattr(args, 'neuron_count') and args.neuron_count:
                config.set("neuron_count", args.neuron_count)
            if hasattr(args, 'region_count') and args.region_count:
                config.set("region_count", args.region_count)
                
            # Plasticity settings
            if hasattr(args, 'plasticity_rate') and args.plasticity_rate:
                config.set("plasticity_learning_rate", args.plasticity_rate)
                
            # Neuromodulation settings
            if hasattr(args, 'dopamine_baseline') and args.dopamine_baseline:
                config.set("dopamine_baseline", args.dopamine_baseline)
                
            # Environment settings
            if hasattr(args, 'world_width') and args.world_width:
                config.set("environment_width", args.world_width)
            if hasattr(args, 'world_height') and args.world_height:
                config.set("environment_height", args.world_height)
                
            # Advanced neural settings
            if hasattr(args, 'connection_prob') and args.connection_prob:
                config.set("connection_probability", args.connection_prob)
                
            # Simulation settings
            if hasattr(args, 'max_steps') and args.max_steps:
                config.set("max_simulation_steps", args.max_steps)
                
        return config
    
    def run_batch_simulation(self, args):
        """Run multiple simulations with different configurations."""
        self.logger.info(f"Starting batch simulation with {args.iterations} iterations")
        
        results = []
        for i in range(args.iterations):
            self.logger.info(f"Running iteration {i + 1}/{args.iterations}")
            
            # Create config with potential variation
            config = pynlm.createDefaultConfig()
            
            # Set basic config values if the API supports it
            if hasattr(config, 'set'):
                config.set("random_seed", args.random_seed + i)
                config.set("simulation_timestep", args.timestep)
            
            # Apply any configuration variations
            if hasattr(args, 'noise_level') and args.noise_level > 0:
                # Add some randomness to parameters
                if args.noise_level > 0:
                    perturbed_neuron_count = int(args.neuron_count * (1 + np.random.normal(0, args.noise_level)))
                    if hasattr(config, 'set'):
                        config.set("neuron_count", max(100, perturbed_neuron_count))
            
            # Create brain and initialize
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(args.max_steps):
                brain.step(step)
            end_time = time.time()
            
            # Collect results
            result = {
                'iteration': i + 1,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'simulation_time': end_time - start_time,
                'steps_per_second': args.max_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0,
                'final_plasticity_modifier': brain.getDevelopmentalStage(),
                'config_used': {
                    'neuron_count': args.neuron_count,
                    'random_seed': args.random_seed + i
                }
            }
            
            results.append(result)
            self.logger.info(f"Iteration {i + 1} completed: {result['total_spikes']} spikes, "
                           f"{result['steps_per_second']:.2f} steps/sec")
        
        self.batch_results = results
        self.logger.info(f"Batch simulation completed. Average spikes: {np.mean([r['total_spikes'] for r in results]):.2f}")
        
        return results
    
    def analyze_results(self, args):
        """Analyze batch results and generate statistics."""
        if not self.batch_results:
            self.logger.warning("No batch results available. Run a batch simulation first.")
            return
            
        self.logger.info("Analyzing batch results...")
        
        spikes = [r['total_spikes'] for r in self.batch_results]
        firing_rates = [r['firing_neurons'] for r in self.batch_results]
        perf_per_sec = [r['steps_per_second'] for r in self.batch_results]
        
        stats = {
            'total_iterations': len(self.batch_results),
            'total_spikes_mean': np.mean(spikes),
            'total_spikes_std': np.std(spikes),
            'total_spikes_min': np.min(spikes),
            'total_spikes_max': np.max(spikes),
            'total_spikes_median': np.median(spikes),
            'firing_neurons_mean': np.mean(firing_rates),
            'firing_neurons_std': np.std(firing_rates),
            'performance_mean': np.mean(perf_per_sec),
            'performance_std': np.std(perf_per_sec),
            'performance_min': np.min(perf_per_sec),
            'performance_max': np.max(perf_per_sec),
        }
        
        # Save results to file
        with open('batch_results.json', 'w') as f:
            json.dump({
                'individual_results': self.batch_results,
                'statistics': stats,
                'parameters': {
                    'neuron_count': args.neuron_count,
                    'max_steps': args.max_steps,
                    'timestep': args.timestep
                }
            }, f, indent=2)
        
        self.logger.info(f"Results saved to batch_results.json")
        self.logger.info(f"Statistics: Mean spikes: {stats['total_spikes_mean']:.2f}, "
                        f"Std: {stats['total_spikes_std']:.2f}, "
                        f"Range: [{stats['total_spikes_min']}, {stats['total_spikes_max']}]")
        
        return stats
    
    def run_advanced_simulation(self, args):
        """Run a complete advanced simulation with world, agent, and monitoring."""
        self.logger.info("Starting advanced simulation with world and agent")
        
        # Create brain with advanced config
        config = self.create_advanced_config(args)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world with custom settings
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.world_width,
            height=args.world_height,
            visionWidth=args.vision_width,
            visionHeight=args.vision_height
        )
        world.reset()
        
        # Create agent with world
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features if requested
        if args.enable_all_features:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        else:
            # Enable based on args
            if args.enable_reward: agent.enableRewardModulation(True)
            if args.enable_plasticity: agent.enableStructuralPlasticity(True)
            if args.enable_development: agent.enableDevelopment(True)
            if args.enable_curiosity: agent.enableCuriosity(True)
        
        # Monitoring variables
        monitoring_data = {
            'steps': [],
            'spike_counts': [],
            'firing_rates': [],
            'neuron_counts': [],
            'brain_states': [],
            'action_counts': {
                'move_forward': 0,
                'move_backward': 0,
                'turn_left': 0,
                'turn_right': 0,
                'interact': 0,
                'wait': 0
            },
            'rewards': [],
            'novelty_levels': [],
            'curiosity_levels': []
        }
        
        # Run simulation with monitoring
        start_time = time.time()
        for step in range(args.max_steps):
            # Update world
            world.update(args.timestep)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
            
            # Record monitoring data
            monitoring_data['steps'].append(step)
            monitoring_data['spike_counts'].append(brain.getTotalSpikeCount())
            monitoring_data['firing_rates'].append(brain.getFiringNeuronCount())
            monitoring_data['neuron_counts'].append(brain.getTotalNeuronCount())
            monitoring_data['brain_states'].append({
                'stage': brain.getDevelopmentalStage(),
                'age': agent.getDevelopmentalStage()
            })
            monitoring_data['rewards'].append(action_result.reward)
            monitoring_data['novelty_levels'].append(agent.getNoveltyLevel())
            monitoring_data['curiosity_levels'].append(agent.getCuriosityLevel())
            
            # Count actions
            if action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward,
                         pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight,
                         pynlm.MotorCommand.Interact, pynlm.MotorCommand.Wait]:
                action_name = action.name.lower()
                if action_name in monitoring_data['action_counts']:
                    monitoring_data['action_counts'][action_name] += 1
            
            # Log progress periodically
            if args.log_interval > 0 and step % args.log_interval == 0:
                self.logger.info(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
                               f"Firing={brain.getFiringNeuronCount()}, "
                               f"Action={action_name}")
        
        end_time = time.time()
        
        # Save monitoring data
        with open('monitoring_data.json', 'w') as f:
            json.dump(monitoring_data, f, indent=2)
        
        self.logger.info(f"Advanced simulation completed in {end_time - start_time:.2f} seconds")
        self.logger.info(f"Total steps: {args.max_steps}, Steps/sec: {args.max_steps / (end_time - start_time):.2f}")
        
        # Print summary
        self.print_advanced_summary(monitoring_data, brain, agent, world)
        
        return monitoring_data
    
    def print_advanced_summary(self, monitoring_data, brain, agent, world):
        """Print detailed summary of the advanced simulation."""
        print("\n" + "="*60)
        print("ADVANCED SIMULATION SUMMARY")
        print("="*60)
        
        print(f"\nPerformance:")
        print(f"  Total simulation time: {monitoring_data['steps'][-1] * args.timestep:.2f}s")
        print(f"  Average firing rate: {np.mean(monitoring_data['firing_rates']):.2f} neurons")
        print(f"  Total spikes: {monitoring_data['spike_counts'][-1]}")
        
        print(f"\nBrain State:")
        print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Final age: {agent.getDevelopmentalStage()}")
        
        print(f"\nAgent Levels:")
        print(f"  Final novelty level: {monitoring_data['novelty_levels'][-1]:.4f}")
        print(f"  Final curiosity level: {monitoring_data['curiosity_levels'][-1]:.4f}")
        print(f"  Final prediction error: {agent.getPredictionError():.4f}")
        
        print(f"\nAction Distribution:")
        total_actions = sum(monitoring_data['action_counts'].values())
        for action, count in monitoring_data['action_counts'].items():
            percentage = (count / total_actions * 100) if total_actions > 0 else 0
            print(f"  {action:15}: {count:4d} ({percentage:5.1f}%)")
        
        print(f"\nReward Statistics:")
        rewards = monitoring_data['rewards']
        print(f"  Mean reward: {np.mean(rewards):.4f}")
        print(f"  Max reward: {np.max(rewards):.4f}")
        print(f"  Min reward: {np.min(rewards):.4f}")
        
        print("="*60)
    
    def save_experiment(self, name):
        """Save current experiment configuration."""
        experiment = {
            'name': name,
            'timestamp': time.time(),
            'batch_results': self.batch_results,
            'monitoring_data': getattr(self, 'current_monitoring_data', None)
        }
        
        self.experiments.append(experiment)
        
        # Save to file
        with open(f'experiment_{name}.json', 'w') as f:
            json.dump(experiment, f, indent=2)
        
        self.logger.info(f"Experiment '{name}' saved")
    
    def load_experiment(self, name):
        """Load a saved experiment."""
        try:
            with open(f'experiment_{name}.json', 'r') as f:
                experiment = json.load(f)
            
            # Restore experiment data
            if 'batch_results' in experiment:
                self.batch_results = experiment['batch_results']
            if 'monitoring_data' in experiment and experiment['monitoring_data']:
                self.current_monitoring_data = experiment['monitoring_data']
                
            self.logger.info(f"Experiment '{name}' loaded successfully")
            return experiment
            
        except FileNotFoundError:
            self.logger.error(f"Experiment '{name}' not found")
            return None
    
    def compare_experiments(self, experiment_names):
        """Compare multiple experiments."""
        if len(experiment_names) < 2:
            self.logger.error("Need at least 2 experiments to compare")
            return
            
        experiments = []
        for name in experiment_names:
            exp = self.load_experiment(name)
            if exp:
                experiments.append(exp)
        
        if len(experiments) < 2:
            self.logger.error("Could not load all experiments for comparison")
            return
        
        print("\nEXPERIMENT COMPARISON")
        print("="*60)
        
        for i, exp in enumerate(experiments):
            print(f"\nExperiment {i + 1}: {exp.get('name', f'Exp_{i+1}')}")
            if 'batch_results' in exp and exp['batch_results']:
                spikes = [r['total_spikes'] for r in exp['batch_results']]
                print(f"  Mean spikes: {np.mean(spikes):.2f}")
                print(f"  Std spikes: {np.std(spikes):.2f}")
        
        print("="*60)


def main():
    """Main CLI entry point."""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module is required for advanced CLI functionality.")
        print("Please install pynlm: pip install pynlm")
        sys.exit(1)
    
    parser = argparse.ArgumentParser(
        description="Advanced NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple advanced simulation
  python nlm_advanced_cli.py --simulation --max-steps 1000 --neuron-count 500
  
  # Run batch simulation for comparison
  python nlm_advanced_cli.py --batch --iterations 10 --neuron-count 1000
  
  # Compare experiments
  python nlm_advanced_cli.py --compare exp1 exp2 exp3
  
  # Save experiment
  python nlm_advanced_cli.py --save my_experiment
        """
    )
    
    # Basic simulation arguments
    sim_group = parser.add_argument_group('Simulation Settings')
    sim_group.add_argument('--simulation', action='store_true', 
                          help='Run advanced simulation with world and agent')
    sim_group.add_argument('--max-steps', type=int, default=1000,
                          help='Maximum number of simulation steps (default: 1000)')
    sim_group.add_argument('--timestep', type=float, default=0.1,
                          help='Simulation timestep in seconds (default: 0.1)')
    sim_group.add_argument('--neuron-count', type=int, default=500,
                          help='Number of neurons in brain (default: 500)')
    sim_group.add_argument('--world-width', type=float, default=15.0,
                          help='Width of world (default: 15.0)')
    sim_group.add_argument('--world-height', type=float, default=15.0,
                          help='Height of world (default: 15.0)')
    sim_group.add_argument('--vision-width', type=int, default=8,
                          help='Vision width (default: 8)')
    sim_group.add_argument('--vision-height', type=int, default=8,
                          help='Vision height (default: 8)')
    
    # Feature flags
    feature_group = parser.add_argument_group('Feature Flags')
    feature_group.add_argument('--enable-all-features', action='store_true',
                              help='Enable all learning and adaptation features')
    feature_group.add_argument('--enable-reward', action='store_true',
                              help='Enable reward modulation')
    feature_group.add_argument('--enable-plasticity', action='store_true',
                              help='Enable structural plasticity')
    feature_group.add_argument('--enable-development', action='store_true',
                              help='Enable development system')
    feature_group.add_argument('--enable-curiosity', action='store_true',
                              help='Enable curiosity-driven exploration')
    
    # Batch simulation arguments
    batch_group = parser.add_argument_group('Batch Simulation')
    batch_group.add_argument('--batch', action='store_true',
                            help='Run batch simulation for comparison')
    batch_group.add_argument('--iterations', type=int, default=5,
                            help='Number of batch iterations (default: 5)')
    batch_group.add_argument('--noise-level', type=float, default=0.0,
                            help='Noise level for parameter variation (default: 0.0)')
    
    # Analysis arguments
    analysis_group = parser.add_argument_group('Analysis')
    analysis_group.add_argument('--log-interval', type=int, default=100,
                               help='Log interval for simulation (default: 100)')
    
    # Experiment management
    exp_group = parser.add_argument_group('Experiment Management')
    exp_group.add_argument('--save', metavar='NAME',
                          help='Save current experiment with given name')
    exp_group.add_argument('--load', metavar='NAME',
                          help='Load experiment by name')
    exp_group.add_argument('--compare', nargs='+', metavar='NAME',
                          help='Compare multiple saved experiments')
    
    # Configuration
    config_group = parser.add_argument_group('Configuration')
    config_group.add_argument('--random-seed', type=int, default=42,
                             help='Random seed for reproducibility (default: 42)')
    config_group.add_argument('--plasticity-rate', type=float, default=0.01,
                             help='Plasticity learning rate (default: 0.01)')
    config_group.add_argument('--dopamine-baseline', type=float, default=0.1,
                             help='Dopamine baseline level (default: 0.1)')
    config_group.add_argument('--connection-prob', type=float, default=0.15,
                             help='Connection probability (default: 0.15)')
    
    args = parser.parse_args()
    
    # Initialize CLI
    cli = NLMAdvancedCLI()
    
    # Load experiment if requested
    if args.load:
        cli.load_experiment(args.load)
        return
    
    # Run batch simulation if requested
    if args.batch:
        results = cli.run_batch_simulation(args)
        if args.save:
            cli.save_experiment(args.save)
        stats = cli.analyze_results(args)
        return
    
    # Run comparison if requested
    if args.compare:
        cli.compare_experiments(args.compare)
        return
    
    # Save experiment if requested
    if args.save:
        cli.save_experiment(args.save)
        return
    
    # Default: run advanced simulation
    if args.simulation or not (args.save or args.load or args.compare):
        cli.run_advanced_simulation(args)
        if args.save:
            cli.save_experiment(args.save)
    
    print("\nAdvanced NLM CLI completed successfully!")
    print("Check log file: nlm_advanced_cli.log")
    print("Check output files: batch_results.json, monitoring_data.json")

if __name__ == '__main__':
    main()
