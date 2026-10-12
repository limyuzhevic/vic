#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface

This script provides advanced users with powerful tools for working with NLM.
It includes experiment management, performance monitoring, and configuration utilities.
"""

import argparse
import json
import time
import sys
import os
from datetime import datetime
from pathlib import Path

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install it first.")
    print("pip install -e .[advanced]")
    sys.exit(1)

class NLMCLI:
    """Advanced NLM Command Line Interface"""
    
    def __init__(self):
        self.experiment_history = []
        self.config_dir = "configs/"
        self.results_dir = "results/"
        
        # Create directories if they don't exist
        Path(self.config_dir).mkdir(exist_ok=True)
        Path(self.results_dir).mkdir(exist_ok=True)
    
    def run_experiment(self, args):
        """Run an NLM experiment with various configurations"""
        print(f"Running experiment: {args.name}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply command line arguments
        if args.neuron_count:
            config.set("neuron_count", args.neuron_count)
        if args.region_count:
            config.set("region_count", args.region_count)
        if args.connection_probability:
            config.set("connection_probability", args.connection_probability)
        if args.plasticity_rate:
            config.set("plasticity_learning_rate", args.plasticity_rate)
        if args.random_seed:
            config.set("random_seed", args.random_seed)
        
        # Create brain and other components
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=args.world_width, height=args.world_height,
                       visionWidth=args.vision_width, visionHeight=args.vision_height)
        world.setAgentStart(args.agent_start_x, args.agent_start_y)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on arguments
        if args.reward_modulation:
            agent.enableRewardModulation(True)
        if args.curiosity:
            agent.enableCuriosity(True)
        if args.development:
            agent.enableDevelopment(True)
        if args.structural_plasticity:
            agent.enableStructuralPlasticity(True)
        
        # Run experiment
        results = self._run_simulation(brain, world, agent, args.steps)
        
        # Save results
        if args.save_results:
            self._save_results(args.name, results)
        
        # Print summary
        self._print_results_summary(args.name, results)
        
        return results
    
    def _run_simulation(self, brain, world, agent, steps):
        """Run a single simulation"""
        start_time = time.time()
        
        results = {
            'start_time': datetime.now().isoformat(),
            'steps': steps,
            'total_reward': 0,
            'max_firing_rate': 0,
            'final_firing_rate': 0,
            'avg_step_time': 0,
            'total_wall_time': 0,
            'spike_counts': [],
            'firing_rates': []
        }
        
        for step in range(steps):
            step_start = time.time()
            
            # Get observation
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Compute reward
            reward = world.computeReward(agent.getBrain()->getRegions()[0].get())
            results['total_reward'] += reward
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Collect metrics
            firing_rate = brain.getAverageFiringRate()
            results['spike_counts'].append(brain.getTotalSpikeCount())
            results['firing_rates'].append(firing_rate)
            results['max_firing_rate'] = max(results['max_firing_rate'], firing_rate)
            
            # Update development
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.001)
            
            # Track step time
            step_time = time.time() - step_start
            if step > 0:  # Skip first step to avoid setup time
                results['step_times'].append(step_time)
        
        # Calculate final metrics
        total_time = time.time() - start_time
        results['total_wall_time'] = total_time
        results['final_firing_rate'] = results['firing_rates'][-1] if results['firing_rates'] else 0
        
        if 'step_times' in results and results['step_times']:
            results['avg_step_time'] = sum(results['step_times']) / len(results['step_times'])
            results['steps_per_second'] = steps / sum(results['step_times'])
        else:
            results['steps_per_second'] = steps / total_time if total_time > 0 else 0
        
        results['end_time'] = datetime.now().isoformat()
        
        return results
    
    def _save_results(self, name, results):
        """Save experiment results to file"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"{self.results_dir}/{name}_{timestamp}.json"
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to: {filename}")
    
    def _print_results_summary(self, name, results):
        """Print experiment results summary"""
        print(f"\n=== Experiment Results: {name} ===")
        print(f"Steps completed: {results['steps']}")
        print(f"Total reward: {results['total_reward']:.2f}")
        print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
        print(f"Maximum firing rate: {results['max_firing_rate']:.2f} Hz")
        print(f"Average step time: {results['avg_step_time']*1000:.2f} ms")
        print(f"Steps per second: {results['steps_per_second']:.1f}")
        print(f"Total simulation time: {results['total_wall_time']:.2f} s")
        
        # Print basic spike statistics
        if results['spike_counts']:
            initial_spikes = results['spike_counts'][0]
            final_spikes = results['spike_counts'][-1]
            spike_increase = final_spikes - initial_spikes
            print(f"Spike increase: {spike_increase}")
            
            if results['firing_rates']:
                avg_firing_rate = sum(results['firing_rates']) / len(results['firing_rates'])
                print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    
    def create_config(self, args):
        """Create a new configuration file"""
        print(f"Creating configuration: {args.name}")
        
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        if args.neuron_count:
            config.set("neuron_count", args.neuron_count)
        if args.region_count:
            config.set("region_count", args.region_count)
        if args.connection_probability:
            config.set("connection_probability", args.connection_probability)
        if args.plasticity_rate:
            config.set("plasticity_learning_rate", args.plasticity_rate)
        if args.random_seed:
            config.set("random_seed", args.random_seed)
        
        # Save to file
        filename = f"{self.config_dir}/{args.name}.cfg"
        success = config.saveToFile(filename)
        
        if success:
            print(f"Configuration saved to: {filename}")
            print("Configuration summary:")
            print(config.summary())
        else:
            print(f"Failed to save configuration to: {filename}")
    
    def list_configs(self, args):
        """List available configuration files"""
        print("Available configuration files:")
        
        if not os.path.exists(self.config_dir):
            print("No configs directory found.")
            return
        
        for filename in os.listdir(self.config_dir):
            if filename.endswith('.cfg') or filename.endswith('.json'):
                filepath = os.path.join(self.config_dir, filename)
                print(f"  {filename}")
                
                # Try to load and display info
                try:
                    if filename.endswith('.json'):
                        with open(filepath, 'r') as f:
                            config_data = json.load(f)
                        print(f"    Type: JSON, Entries: {len(config_data)}")
                    else:
                        config = pynlm.createDefaultConfig()
                        if config.loadFromFile(filepath):
                            keys = config.getKeys()
                            print(f"    Type: NLM config, Keys: {len(keys)}")
                        else:
                            print(f"    Type: Unknown (invalid format)")
                except Exception as e:
                    print(f"    Error loading: {e}")
    
    def benchmark(self, args):
        """Benchmark NLM performance"""
        print(f"Benchmarking NLM performance...")
        
        # Run multiple tests with different configurations
        configs = [
            {"neuron_count": 100, "name": "Small"},
            {"neuron_count": 500, "name": "Medium"},
            {"neuron_count": 1000, "name": "Large"}
        ]
        
        results = []
        
        for config_spec in configs:
            print(f"\nBenchmarking {config_spec['name']} configuration...")
            
            # Create config
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", config_spec["neuron_count"])
            
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain_start = time.time()
            brain.initialize()
            brain_init_time = time.time() - brain_start
            
            # Run simulation
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run steps
            start_time = time.time()
            for step in range(args.steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            total_time = time.time() - start_time
            steps_per_second = args.steps / total_time
            
            result = {
                'name': config_spec['name'],
                'neuron_count': config_spec['neuron_count'],
                'initialization_time': brain_init_time,
                'total_time': total_time,
                'steps_per_second': steps_per_second,
                'avg_step_time_ms': (total_time / args.steps) * 1000
            }
            
            results.append(result)
            
            print(f"  Initialization: {brain_init_time:.3f}s")
            print(f"  Total time: {total_time:.3f}s")
            print(f"  Steps per second: {steps_per_second:.1f}")
            print(f"  Average step time: {result['avg_step_time_ms']:.2f}ms")
        
        # Save benchmark results
        if args.save_benchmark:
            filename = f"{self.results_dir}/benchmark_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            with open(filename, 'w') as f:
                json.dump({
                    'timestamp': datetime.now().isoformat(),
                    'parameters': {
                        'steps': args.steps,
                        'world_width': args.world_width,
                        'world_height': args.world_height
                    },
                    'results': results
                }, f, indent=2)
            
            print(f"\nBenchmark results saved to: {filename}")
        
        # Print summary table
        print(f"\n=== Benchmark Summary ===")
        print(f"{'Config':<10} {'Neurons':<10} {'Init (s)':<12} {'Total (s)':<12} {'Steps/s':<12} {'Step (ms)':<12}")
        print("-" * 70)
        
        for result in results:
            print(f"{result['name']:<10} {result['neuron_count']:<10} "
                  f"{result['initialization_time']:<12.3f} {result['total_time']:<12.3f} "
                  f"{result['steps_per_second']:<12.1f} {result['avg_step_time_ms']:<12.2f}")

    def load_history(self, args):
        """Load and display experiment history"""
        history_file = f"{self.results_dir}/experiment_history.json"
        
        if os.path.exists(history_file):
            with open(history_file, 'r') as f:
                self.experiment_history = json.load(f)
            
            print(f"Loaded {len(self.experiment_history)} experiment records")
            
            # Display recent experiments
            print(f"\n=== Recent Experiments ===")
            for i, exp in enumerate(self.experiment_history[-5:]):  # Show last 5
                print(f"\nExperiment {i+1}:")
                print(f"  Name: {exp.get('name', 'Unknown')}")
                print(f"  Timestamp: {exp.get('timestamp', 'Unknown')}")
                print(f"  Steps: {exp.get('steps', 'Unknown')}")
                print(f"  Total reward: {exp.get('total_reward', 0):.2f}")
                print(f"  Final firing rate: {exp.get('final_firing_rate', 0):.2f} Hz")
        else:
            print("No experiment history found.")

def main():
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli experiment --name "SimpleTest" --steps 500
  nlm-cli create-config myconfig
  nlm-cli list-configs
  nlm-cli benchmark --steps 1000
  nlm-cli history
  nlm-cli benchmark --save-benchmark
        ""
    )
    
    # Common arguments
    parser.add_argument("--world-width", type=int, default=10, help="World width")
    parser.add_argument("--world-height", type=int, default=10, help="World height")
    parser.add_argument("--vision-width", type=int, default=5, help="Vision width")
    parser.add_argument("--vision-height", type=int, default=5, help="Vision height")
    parser.add_argument("--agent-start-x", type=int, default=2, help="Agent start X position")
    parser.add_argument("--agent-start-y", type=int, default=2, help="Agent start Y position")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--save-results", action="store_true", help="Save experiment results")
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Experiment command
    exp_parser = subparsers.add_parser("experiment", help="Run an experiment")
    exp_parser.add_argument("--name", required=True, help="Experiment name")
    exp_parser.add_argument("--neuron-count", type=int, help="Number of neurons")
    exp_parser.add_argument("--region-count", type=int, help="Number of regions")
    exp_parser.add_argument("--connection-probability", type=float, help="Connection probability")
    exp_parser.add_argument("--plasticity-rate", type=float, help="Plasticity learning rate")
    exp_parser.add_argument("--random-seed", type=int, help="Random seed")
    exp_parser.add_argument("--reward-modulation", action="store_true", help="Enable reward modulation")
    exp_parser.add_argument("--curiosity", action="store_true", help="Enable curiosity")
    exp_parser.add_argument("--development", action="store_true", help="Enable development")
    exp_parser.add_argument("--structural-plasticity", action="store_true", help="Enable structural plasticity")
    
    # Config commands
    config_parser = subparsers.add_parser("create-config", help="Create a new configuration")
    config_parser.add_argument("name", help="Configuration name")
    config_parser.add_argument("--neuron-count", type=int, help="Number of neurons")
    config_parser.add_argument("--region-count", type=int, help="Number of regions")
    config_parser.add_argument("--connection-probability", type=float, help="Connection probability")
    config_parser.add_argument("--plasticity-rate", type=float, help="Plasticity learning rate")
    config_parser.add_argument("--random-seed", type=int, help="Random seed")
    
    list_parser = subparsers.add_parser("list-configs", help="List available configurations")
    
    # Benchmark command
    bench_parser = subparsers.add_parser("benchmark", help="Run benchmark tests")
    bench_parser.add_argument("--steps", type=int, default=500, help="Number of steps for benchmark")
    bench_parser.add_argument("--save-benchmark", action="store_true", help="Save benchmark results")
    
    # History command
    history_parser = subparsers.add_parser("history", help="Load and display experiment history")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Execute command
    if args.command == "experiment":
        cli.run_experiment(args)
    elif args.command == "create-config":
        cli.create_config(args)
    elif args.command == "list-configs":
        cli.list_configs(args)
    elif args.command == "benchmark":
        cli.benchmark(args)
    elif args.command == "history":
        cli.load_history(args)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
