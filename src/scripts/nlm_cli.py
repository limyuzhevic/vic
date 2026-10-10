#!/usr/bin/env python3

"""
NLM Command Line Interface (CLI)

Advanced command-line tools for NLM (Neural Learning Machine) operations.
Provides a unified interface for brain management, experiment running,
visualization, and batch processing.

Usage:
    nlm-cli [command] [options]

Commands:
    brain         Brain management commands
    experiment    Experiment management and running
    config        Configuration management
    visualize     Visualization tools
    benchmark     Performance benchmarking
    batch         Batch processing
    profile       Profiling and debugging

Examples:
    # Run a simple brain simulation
    nlm-cli brain run --neurons 100 --steps 1000
    
    # Run multiple experiments with different configs
    nlm-cli batch run configs/experiment_*.json
    
    # Visualize results
    nlm-cli visualize results/output.json --format png --fps 10
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional
import time
import subprocess
import logging
from datetime import datetime

# Add the current directory to Python path for pynlm imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install pynlm dependencies.")
    sys.exit(1)

class CLIError(Exception):
    """CLI-specific exceptions"""
    pass

class NLMCLI:
    def __init__(self):
        self.setup_logging()
        self.home_dir = Path.home() / ".nlm"
        self.configs_dir = self.home_dir / "configs"
        self.results_dir = self.home_dir / "results"
        
        # Create directories if they don't exist
        self.configs_dir.mkdir(parents=True, exist_ok=True)
        self.results_dir.mkdir(parents=True, exist_ok=True)
    
    def setup_logging(self):
        """Setup logging for CLI operations"""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.StreamHandler(sys.stdout),
                logging.FileHandler(self.home_dir / "nlm_cli.log")
            ]
        )
        self.logger = logging.getLogger(__name__)
    
    def run(self):
        """Run the CLI with command parsing"""
        parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Command Line Interface",
            epilog="Use 'nlm-cli <command> --help' for detailed help on specific commands"
        )
        
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Brain commands
        self._add_brain_commands(subparsers)
        
        # Experiment commands
        self._add_experiment_commands(subparsers)
        
        # Configuration commands
        self._add_config_commands(subparsers)
        
        # Visualization commands
        self._add_visualize_commands(subparsers)
        
        # Benchmark commands
        self._add_benchmark_commands(subparsers)
        
        # Batch commands
        self._add_batch_commands(subparsers)
        
        # Profile commands
        self._add_profile_commands(subparsers)
        
        # Handle version
        parser.add_argument('--version', action='version', 
                          version='NLM CLI v0.1.0')
        
        try:
            args = parser.parse_args()
            
            if not args.command:
                parser.print_help()
                return
                
            command_method = getattr(self, f'cmd_{args.command}', None)
            if command_method:
                command_method(args)
            else:
                self.logger.error(f"Unknown command: {args.command}")
                sys.exit(1)
                
        except Exception as e:
            self.logger.error(f"Error: {e}")
            sys.exit(1)
    
    def _add_brain_commands(self, subparsers):
        """Add brain-related subcommands"""
        brain_parser = subparsers.add_parser('brain', help='Brain management commands')
        
        brain_subparsers = brain_parser.add_subparsers(dest='brain_action', 
                                                     help='Brain actions')
        
        # Brain create command
        create_parser = brain_subparsers.add_parser('create', help='Create a new brain')
        create_parser.add_argument('--neurons', type=int, default=100, 
                                  help='Number of neurons (default: 100)')
        create_parser.add_argument('--regions', type=int, default=1,
                                  help='Number of brain regions (default: 1)')
        create_parser.add_argument('--timestep', type=float, default=0.001,
                                  help='Simulation timestep in seconds (default: 0.001)')
        create_parser.add_argument('--seed', type=int, default=None,
                                  help='Random seed for reproducibility')
        create_parser.add_argument('--config', type=str, default=None,
                                  help='Load configuration from file')
        create_parser.add_argument('--output', type=str, default='brain.nlm',
                                  help='Output file for brain checkpoint')
        
        # Brain run command
        run_parser = brain_subparsers.add_parser('run', help='Run brain simulation')
        run_parser.add_argument('--neurons', type=int, default=100,
                               help='Number of neurons (default: 100)')
        run_parser.add_argument('--steps', type=int, default=1000,
                               help='Number of simulation steps (default: 1000)')
        run_parser.add_argument('--world', action='store_true',
                               help='Run with world simulation')
        run_parser.add_argument('--vision', type=int, nargs=2, default=[8, 8],
                               help='Vision dimensions (width height) (default: 8 8)')
        run_parser.add_argument('--steps-per-visual', type=int, default=10,
                               help='Steps between each visual update (default: 10)')
        run_parser.add_argument('--reward', action='store_true',
                               help='Enable reward-based learning')
        run_parser.add_argument('--curiosity', action='store_true',
                               help='Enable curiosity-driven exploration')
        run_parser.add_argument('--output', type=str, default='simulation_results.json',
                               help='Output file for simulation results')
        
        # Brain status command
        status_parser = brain_subparsers.add_parser('status', help='Show brain status')
        status_parser.add_argument('--brain-file', type=str, default='brain.nlm',
                                  help='Input brain file for status check')
    
    def _add_experiment_commands(self, subparsers):
        """Add experiment-related subcommands"""
        experiment_parser = subparsers.add_parser('experiment', 
                                                 help='Experiment management commands')
        
        experiment_subparsers = experiment_parser.add_subparsers(
            dest='experiment_action',
            help='Experiment actions'
        )
        
        # Run experiment command
        run_parser = experiment_subparsers.add_parser('run', help='Run an experiment')
        run_parser.add_argument('config_file', type=str,
                               help='Experiment configuration file (JSON)')
        run_parser.add_argument('--output-dir', type=str, default='./experiments',
                               help='Output directory for experiment results')
        run_parser.add_argument('--parallel', type=int, default=1,
                               help='Number of parallel experiments to run')
        run_parser.add_argument('--continue', action='store_true',
                               help='Continue interrupted experiment')
        
        # Compare experiments command
        compare_parser = experiment_subparsers.add_parser('compare', 
                                                         help='Compare multiple experiments')
        compare_parser.add_argument('config_files', nargs='+',
                                   help='Configuration files to compare')
        compare_parser.add_argument('--metric', type=str, default='reward',
                                   help='Metric to compare (reward, learning_rate, firing_rate)')
        
        # Template experiment command
        template_parser = experiment_subparsers.add_parser('template', 
                                                          help='Create experiment template')
        template_parser.add_argument('name', type=str, help='Template name')
        template_parser.add_argument('--type', type=str, default='custom',
                                    help='Experiment type (learning, exploration, development, custom)')
    
    def _add_config_commands(self, subparsers):
        """Add configuration-related subcommands"""
        config_parser = subparsers.add_parser('config', help='Configuration management')
        
        config_subparsers = config_parser.add_subparsers(dest='config_action',
                                                      help='Configuration actions')
        
        # Config wizard command
        wizard_parser = config_subparsers.add_parser('wizard', 
                                                    help='Interactive configuration wizard')
        
        # Config list command
        list_parser = config_subparsers.add_parser('list', 
                                                  help='List available configurations')
        
        # Config compare command
        compare_parser = config_subparsers.add_parser('compare', 
                                                     help='Compare two configurations')
        compare_parser.add_argument('config1', type=str, help='First config file')
        compare_parser.add_argument('config2', type=str, help='Second config file')
    
    def _add_visualize_commands(self, subparsers):
        """Add visualization-related subcommands"""
        visualize_parser = subparsers.add_parser('visualize', 
                                                help='Visualization tools')
        
        visualize_subparsers = visualize_parser.add_subparsers(
            dest='visualize_action',
            help='Visualization actions'
        )
        
        # Visualize results command
        results_parser = visualize_subparsers.add_parser('results', 
                                                        help='Visualize experiment results')
        results_parser.add_argument('input_file', type=str,
                                   help='Input results file (JSON)')
        results_parser.add_argument('--format', type=str, default='png',
                                   help='Output format (png, video, interactive)')
        results_parser.add_argument('--fps', type=int, default=10,
                                   help='Frames per second for video output')
        results_parser.add_argument('--width', type=int, default=1920,
                                   help='Output image/video width')
        results_parser.add_argument('--height', type=int, default=1080,
                                   help='Output image/video height')
        
        # Create demo command
        demo_parser = visualize_subparsers.add_parser('demo', 
                                                    help='Create demonstration video')
        demo_parser.add_argument('--duration', type=float, default=30.0,
                                help='Demo duration in seconds')
        demo_parser.add_argument('--output', type=str, default='demo.mp4',
                                help='Output video file')
    
    def _add_benchmark_commands(self, subparsers):
        """Add benchmarking-related subcommands"""
        benchmark_parser = subparsers.add_parser('benchmark', 
                                                help='Performance benchmarking')
        
        benchmark_subparsers = benchmark_parser.add_subparsers(
            dest='benchmark_action',
            help='Benchmark actions'
        )
        
        # Benchmark brain command
        brain_parser = benchmark_subparsers.add_parser('brain', 
                                                     help='Benchmark brain performance')
        brain_parser.add_argument('--neurons', type=int, nargs='+', default=[100, 500, 1000],
                                 help='Neuron counts to test')
        brain_parser.add_argument('--steps', type=int, nargs='+', default=[1000, 5000],
                                 help='Step counts to test')
        brain_parser.add_argument('--runs', type=int, default=3,
                                 help='Number of runs per configuration')
        
        # Compare performance command
        compare_parser = benchmark_subparsers.add_parser('compare', 
                                                       help='Compare different configurations')
        compare_parser.add_argument('configs', nargs='+', help='Configuration files')
    
    def _add_batch_commands(self, subparsers):
        """Add batch processing-related subcommands"""
        batch_parser = subparsers.add_parser('batch', help='Batch processing')
        
        batch_subparsers = batch_parser.add_subparsers(dest='batch_action',
                                                    help='Batch actions')
        
        # Batch run command
        run_parser = batch_subparsers.add_parser('run', help='Run batch of experiments')
        run_parser.add_argument('config_patterns', nargs='+',
                               help='Configuration patterns (e.g., configs/exp_*.json)')
        run_parser.add_argument('--output-dir', type=str, default='./batch_results',
                               help='Output directory')
        run_parser.add_argument('--continue', action='store_true',
                               help='Continue interrupted batch')
        run_parser.add_argument('--max-errors', type=int, default=5,
                               help='Maximum errors before stopping')
    
    def _add_profile_commands(self, subparsers):
        """Add profiling-related subcommands"""
        profile_parser = subparsers.add_parser('profile', help='Profiling and debugging')
        
        profile_subparsers = profile_parser.add_subparsers(dest='profile_action',
                                                       help='Profile actions')
        
        # Profile brain command
        brain_parser = profile_subparsers.add_parser('brain', 
                                                   help='Profile brain execution')
        brain_parser.add_argument('--brain-file', type=str, default='brain.nlm',
                                 help='Brain file to profile')
        brain_parser.add_argument('--steps', type=int, default=1000,
                                 help='Number of steps to profile')
        brain_parser.add_argument('--output', type=str, default='profile_results.json',
                                 help='Output file for profiling results')
        
        # Memory profile command
        memory_parser = profile_subparsers.add_parser('memory', 
                                                    help='Memory usage profiling')
        memory_parser.add_argument('--brain-file', type=str, default='brain.nlm',
                                  help='Brain file to profile')
    
    # Command implementations
    def cmd_brain(self, args):
        """Handle brain command"""
        if not hasattr(args, 'brain_action') or not args.brain_action:
            print("Brain command requires an action (create, run, status)")
            return
        
        if args.brain_action == 'create':
            self._cmd_brain_create(args)
        elif args.brain_action == 'run':
            self._cmd_brain_run(args)
        elif args.brain_action == 'status':
            self._cmd_brain_status(args)
    
    def _cmd_brain_create(self, args):
        """Create a new brain"""
        self.logger.info(f"Creating brain with {args.neurons} neurons")
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons)
        config.set("region_count", args.regions)
        config.set("simulation_timestep", args.timestep)
        
        if args.seed is not None:
            config.set("random_seed", args.seed)
        
        if args.config:
            # Try to load custom config
            try:
                with open(args.config, 'r') as f:
                    custom_config = json.load(f)
                # Merge custom config (simplified)
                for key, value in custom_config.items():
                    config.set(key, value)
            except Exception as e:
                self.logger.warning(f"Failed to load custom config: {e}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Save brain
        brain.save(args.output)
        
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Saved to {args.output}")
    
    def _cmd_brain_run(self, args):
        """Run brain simulation"""
        self.logger.info(f"Running brain simulation with {args.neurons} neurons for {args.steps} steps")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons)
        config.set("simulation_timestep", 0.001)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Setup world if requested
        world = None
        agent = None
        if args.world:
            world = pynlm.createSimpleWorld()
            world.configure(
                width=20, 
                height=20, 
                visionWidth=args.vision[0], 
                visionHeight=args.vision[1]
            )
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            if args.reward:
                agent.enableRewardModulation(True)
            if args.curiosity:
                agent.enableCuriosity(True)
        
        # Run simulation
        print(f"Starting simulation for {args.steps} steps...")
        start_time = time.time()
        
        for step in range(args.steps):
            if world:
                # World simulation loop
                world.update(0.1)
                
                if step % args.steps_per_visual == 0:
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                
                brain.step(step)
                
                if step % args.steps_per_visual == 0:
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
            else:
                brain.step(step)
            
            # Progress update
            if (step + 1) % 100 == 0:
                progress = ((step + 1) / args.steps) * 100
                print(f"Progress: {progress:.1f}% - Step {step + 1}, "
                      f"Firing neurons: {brain.getFiringNeuronCount()}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate statistics
        total_spikes = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"\n✓ Simulation completed in {duration:.2f} seconds")
        print(f"✓ Total spikes: {total_spikes}")
        print(f"✓ Average firing rate: {firing_rate:.2f} Hz")
        print(f"✓ Steps per second: {args.steps / duration:.1f}")
        
        # Save results
        results = {
            'neuron_count': args.neurons,
            'steps': args.steps,
            'duration': duration,
            'total_spikes': total_spikes,
            'firing_rate': firing_rate,
            'steps_per_second': args.steps / duration,
            'timestamp': datetime.now().isoformat(),
            'has_world': args.world,
            'world_dimensions': {
                'width': 20 if args.world else None,
                'height': 20 if args.world else None,
                'vision_width': args.vision[0] if args.world else None,
                'vision_height': args.vision[1] if args.world else None
            }
        }
        
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"✓ Results saved to {output_path}")
    
    def _cmd_brain_status(self, args):
        """Show brain status"""
        # Load and check brain file
        try:
            brain = pynlm.loadBrain(args.brain_file)
            
            print(f"✓ Loaded brain from {args.brain_file}")
            print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
            
            # Show system integration status
            print(f"\n✓ System Integration Status:")
            print(f"  - Working memory: {'✓' if brain.getWorkingMemory() else '✗'}")
            print(f"  - Episodic memory: {'✓' if brain.getEpisodicMemory() else '✗'}")
            print(f"  - Neuromodulation: {'✓' if brain.getDopamine() else '✗'}")
            print(f"  - Prediction system: {'✓' if brain.getPredictionSystem() else '✗'}")
            print(f"  - Planning system: {'✓' if brain.getPlanner() else '✗'}")
            print(f"  - Development system: {'✓' if brain.getDevelopmentSystem() else '✗'}")
            
        except Exception as e:
            self.logger.error(f"Failed to load brain: {e}")
            print(f"✗ Error loading brain: {e}")
            sys.exit(1)
    
    def cmd_experiment(self, args):
        """Handle experiment command"""
        if not hasattr(args, 'experiment_action') or not args.experiment_action:
            print("Experiment command requires an action (run, compare, template)")
            return
        
        if args.experiment_action == 'run':
            self._cmd_experiment_run(args)
        elif args.experiment_action == 'compare':
            self._cmd_experiment_compare(args)
        elif args.experiment_action == 'template':
            self._cmd_experiment_template(args)
    
    def _cmd_experiment_run(self, args):
        """Run an experiment from config file"""
        self.logger.info(f"Running experiment from {args.config_file}")
        
        # Load experiment configuration
        try:
            with open(args.config_file, 'r') as f:
                config = json.load(f)
        except Exception as e:
            raise CLIError(f"Failed to load experiment config: {e}")
        
        # Create output directory
        output_dir = Path(args.output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Run experiment
        results = []
        for i in range(args.parallel):
            print(f"Running experiment {i + 1}/{args.parallel}")
            
            # Create brain
            brain_config = pynlm.createDefaultConfig()
            for key, value in config.items():
                if isinstance(value, dict):
                    # Handle nested config
                    for sub_key, sub_value in value.items():
                        brain_config.set(f"{key}_{sub_key}", sub_value)
                else:
                    brain_config.set(key, value)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Run simulation
            for step in range(config.get('steps', 1000)):
                brain.step(step)
            
            # Save results
            result_file = output_dir / f"experiment_{i}_results.json"
            with open(result_file, 'w') as f:
                json.dump({
                    'config': config,
                    'neuron_count': brain.getTotalNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'completed_steps': config.get('steps', 1000),
                    'experiment_id': i
                }, f, indent=2)
            
            results.append(result_file)
            print(f"✓ Experiment {i + 1} completed")
        
        print(f"✓ All experiments completed. Results saved to {output_dir}")
    
    def cmd_config(self, args):
        """Handle config command"""
        if not hasattr(args, 'config_action') or not args.config_action:
            print("Config command requires an action (wizard, list, compare)")
            return
        
        if args.config_action == 'wizard':
            self._cmd_config_wizard(args)
        elif args.config_action == 'list':
            self._cmd_config_list(args)
        elif args.config_action == 'compare':
            self._cmd_config_compare(args)
    
    def _cmd_config_wizard(self, args):
        """Interactive configuration wizard"""
        print("=== NLM Configuration Wizard ===")
        print("Welcome to the NLM configuration wizard!")
        print("This will help you create a basic configuration for your simulation.\n")
        
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Ask user for basic settings
        print("1. Basic Settings:")
        try:
            neurons = int(input("   Number of neurons (default: 100): ") or "100")
            config.set("neuron_count", neurons)
        except ValueError:
            config.set("neuron_count", 100)
            print("   Using default: 100 neurons")
        
        try:
            steps = int(input("   Number of simulation steps (default: 1000): ") or "1000")
            config.set("steps", steps)
        except ValueError:
            config.set("steps", 1000)
            print("   Using default: 1000 steps")
        
        print("\n2. Simulation Settings:")
        try:
            timestep = float(input("   Simulation timestep (default: 0.001): ") or "0.001")
            config.set("simulation_timestep", timestep)
        except ValueError:
            config.set("simulation_timestep", 0.001)
            print("   Using default: 0.001s timestep")
        
        print("\n3. Learning Settings:")
        enable_reward = input("   Enable reward-based learning? (y/n, default: n): ").lower() == 'y'
        config.set("reward_modulation", enable_reward)
        
        enable_curiosity = input("   Enable curiosity-driven exploration? (y/n, default: n): ").lower() == 'y'
        config.set("curiosity_enabled", enable_curiosity)
        
        print("\n4. Development Settings:")
        enable_development = input("   Enable developmental stages? (y/n, default: n): ").lower() == 'y'
        config.set("development_enabled", enable_development)
        
        # Save configuration
        config_file = input("\nSave configuration to file (default: config.json): ") or "config.json"
        
        with open(config_file, 'w') as f:
            # Convert to JSON-serializable format
            config_data = {
                'neuron_count': config.getOr("neuron_count", 100),
                'steps': config.getOr("steps", 1000),
                'simulation_timestep': config.getOr("simulation_timestep", 0.001),
                'reward_modulation': config.getOr("reward_modulation", False),
                'curiosity_enabled': config.getOr("curiosity_enabled", False),
                'development_enabled': config.getOr("development_enabled", False)
            }
            json.dump(config_data, f, indent=2)
        
        print(f"\n✓ Configuration saved to {config_file}")
        print("\nNext steps:")
        print("  nlm-cli brain run --neurons <neurons> --steps <steps> --config <config_file>")
    
    def _cmd_config_list(self, args):
        """List available configurations"""
        print("=== Available Configurations ===")
        
        # Look for config files
        config_files = list(self.configs_dir.glob("*.json"))
        
        if not config_files:
            print("No configuration files found in:")
            print(f"  {self.configs_dir}")
            print("\nUse 'nlm-cli config wizard' to create a new configuration")
            return
        
        print(f"Found {len(config_files)} configuration files:")
        for config_file in sorted(config_files):
            try:
                with open(config_file, 'r') as f:
                    config = json.load(f)
                
                print(f"\n{config_file.name}:")
                print(f"  Neurons: {config.get('neuron_count', 'N/A')}")
                print(f"  Steps: {config.get('steps', 'N/A')}")
                print(f"  Timestep: {config.get('simulation_timestep', 'N/A')}s")
                print(f"  Reward: {config.get('reward_modulation', 'N/A')}")
                print(f"  Curiosity: {config.get('curiosity_enabled', 'N/A')}")
                print(f"  Development: {config.get('development_enabled', 'N/A')}")
                
            except Exception as e:
                print(f"  ✗ Error loading {config_file.name}: {e}")
    
    def _cmd_config_compare(self, args):
        """Compare two configurations"""
        print(f"=== Comparing {args.config1} and {args.config2} ===")
        
        try:
            with open(args.config1, 'r') as f:
                config1 = json.load(f)
            with open(args.config2, 'r') as f:
                config2 = json.load(f)
        except Exception as e:
            raise CLIError(f"Failed to load config files: {e}")
        
        # Compare keys
        all_keys = set(config1.keys()) | set(config2.keys())
        
        print("\nDifferences:")
        for key in sorted(all_keys):
            val1 = config1.get(key, "N/A")
            val2 = config2.get(key, "N/A")
            
            if val1 != val2:
                print(f"  ✗ {key}:")
                print(f"    {args.config1}: {val1}")
                print(f"    {args.config2}: {val2}")
            else:
                print(f"  ✓ {key}: {val1}")
        
        # Show similarity
        identical = sum(1 for key in all_keys if config1.get(key) == config2.get(key))
        total = len(all_keys)
        
        print(f"\nSimilarity: {identical}/{total} ({identical/total*100:.1f}%)")

if __name__ == "__main__":
    cli = NLMCLI()
    cli.run()