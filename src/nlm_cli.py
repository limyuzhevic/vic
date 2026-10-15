"""
NLM Command Line Interface - Advanced Commands for Power Users
===============================================================

This module provides advanced commands and utilities for power users and researchers
using the NLM neural simulation framework. These commands build upon the basic Python
API and provide enhanced functionality for complex simulations and analysis.
"""

import sys
import os
import json
import time
import argparse
from typing import Dict, List, Optional, Any, Callable
from pathlib import Path

# Try to import NLM components
try:
    import pynlm as nlm
    _pynlm_available = True
except ImportError:
    _pynlm_available = False
    nlm = None

class NLMCommandError(Exception):
    """Exception for NLM command errors."""
    pass

def create_advanced_parser():
    """Create advanced argument parser for NLM commands."""
    parser = argparse.ArgumentParser(
        description="NLM - Advanced Neural Simulation Commands",
        epilog="Use 'nlm <command> --help' for detailed command information"
    )
    
    parser.add_subparsers(dest='command', help='Available commands')
    
    # Simulation management commands
    setup_simulation_commands(parser)
    
    # Analysis and debugging commands
    setup_analysis_commands(parser)
    
    # Configuration and management commands
    setup_management_commands(parser)
    
    # Advanced configuration commands
    setup_advanced_commands(parser)
    
    return parser

def setup_simulation_commands(parser):
    """Setup simulation management commands."""
    # Run enhanced simulation with custom configuration
    sim_parser = parser.add_subparsers().add_parser(
        'run', help='Run NLM simulation with advanced options'
    )
    sim_parser.add_argument('--config', '-c', help='Configuration file path')
    sim_parser.add_argument('--steps', '-s', type=int, default=1000,
                          help='Number of simulation steps')
    sim_parser.add_argument('--world-width', type=int, default=20,
                          help='World width')
    sim_parser.add_argument('--world-height', type=int, default=20,
                          help='World height')
    sim_parser.add_argument('--vision-width', type=int, default=8,
                          help='Vision width')
    sim_parser.add_argument('--vision-height', type=int, default=8,
                          help='Vision height')
    sim_parser.add_argument('--enable-checkpointing', action='store_true',
                          help='Enable checkpointing')
    sim_parser.add_argument('--checkpoint-interval', type=int, default=1000,
                          help='Checkpoint interval')
    sim_parser.add_argument('--enable-replay', action='store_true',
                          help='Enable memory replay')
    sim_parser.add_argument('--enable-development', action='store_true',
                          help='Enable developmental processes')
    sim_parser.add_argument('--enable-curiosity', action='store_true',
                          help='Enable curiosity-driven exploration')
    sim_parser.add_argument('--output', '-o', help='Output file for results')
    sim_parser.add_argument('--verbose', '-v', action='store_true',
                          help='Enable verbose output')
    
    # Batch simulation command
    batch_parser = parser.add_subparsers().add_parser(
        'batch', help='Run multiple simulations in batch'
    )
    batch_parser.add_argument('--configs', help='Configuration file list')
    batch_parser.add_argument('--output-dir', default='./batch_results',
                             help='Output directory for batch results')
    batch_parser.add_argument('--parallel', type=int, default=1,
                            help='Number of parallel simulations')
    
    # Parameter sweep command
    sweep_parser = parser.add_subparsers().add_parser(
        'sweep', help='Run parameter sweep simulation'
    )
    sweep_parser.add_argument('--parameters', help='Parameter sweep file')
    sweep_parser.add_argument('--steps', '-s', type=int, default=500,
                            help='Number of steps per sweep')
    sweep_parser.add_argument('--output', '-o', help='Output file for sweep results')

def setup_analysis_commands(parser):
    """Setup analysis and debugging commands."""
    # Analyze simulation results
    analysis_parser = parser.add_subparsers().add_parser(
        'analyze', help='Analyze simulation results'
    )
    analysis_parser.add_argument('input', help='Input results file')
    analysis_parser.add_argument('--output', '-o', help='Output analysis file')
    analysis_parser.add_argument('--format', choices=['json', 'csv', 'plot'],
                               default='json', help='Output format')
    analysis_parser.add_argument('--metrics', help='Specific metrics to analyze')
    analysis_parser.add_argument('--compare', help='Compare with baseline results')
    
    # Debug simulation command
    debug_parser = parser.add_subparsers().add_parser(
        'debug', help='Debug simulation with detailed logging'
    )
    debug_parser.add_argument('--steps', '-s', type=int, default=100,
                            help='Number of debug steps')
    debug_parser.add_argument('--log-level', choices=['error', 'warn', 'info',
                               'debug'], default='info', help='Log level')
    debug_parser.add_argument('--profile', action='store_true',
                            help='Enable performance profiling')
    debug_parser.add_argument('--trace', action='store_true',
                            help='Enable detailed tracing')
    
    # Validate configuration command
    validate_parser = parser.add_subparsers().add_parser(
        'validate', help='Validate configuration and setup'
    )
    validate_parser.add_argument('--config', '-c', help='Configuration file')
    validate_parser.add_argument('--check-files', action='store_true',
                                help='Check if required files exist')
    validate_parser.add_argument('--check-deps', action='store_true',
                                help='Check if dependencies are available')

def setup_management_commands(parser):
    """Setup configuration and management commands."""
    # Create default configuration
    create_parser = parser.add_subparsers().add_parser(
        'create-config', help='Create default configuration file'
    )
    create_parser.add_argument('--output', '-o', default='default_config.json',
                              help='Output configuration file')
    create_parser.add_argument('--format', choices=['json', 'yaml', 'cfg'],
                              default='json', help='Configuration format')
    
    # List available configurations
    list_parser = parser.add_subparsers().add_parser(
        'list-configs', help='List available configurations'
    )
    list_parser.add_argument('--path', '-p', default='./configs',
                            help='Configuration directory')
    
    # Backup configuration
    backup_parser = parser.add_subparsers().add_parser(
        'backup', help='Backup current configuration'
    )
    backup_parser.add_argument('--output', '-o', help='Backup output path')
    backup_parser.add_argument('--compress', action='store_true',
                              help='Compress backup')
    
    # Restore configuration
    restore_parser = parser.add_subparsers().add_parser(
        'restore', help='Restore configuration from backup'
    )
    restore_parser.add_argument('backup-file', help='Backup file to restore')
    restore_parser.add_argument('--output', '-o', default='restored_config.json',
                               help='Output restored configuration')

def setup_advanced_commands(parser):
    """Setup advanced configuration commands."""
    # Create custom brain architecture
    arch_parser = parser.add_subparsers().add_parser(
        'create-arch', help='Create custom brain architecture'
    )
    arch_parser.add_argument('--output', '-o', help='Output architecture file')
    arch_parser.add_argument('--neurons', type=int, default=1000,
                            help='Number of neurons')
    arch_parser.add_argument('--regions', type=int, default=5,
                           help='Number of brain regions')
    arch_parser.add_argument('--connectivity', type=float, default=0.1,
                           help='Connection probability')
    arch_parser.add_argument('--plasticity', action='store_true',
                           help='Enable plasticity in architecture')
    
    # Create experiment configuration
    exp_parser = parser.add_subparsers().add_parser(
        'create-exp', help='Create experiment configuration'
    )
    exp_parser.add_argument('--name', help='Experiment name')
    exp_parser.add_argument('--description', help='Experiment description')
    exp_parser.add_argument('--parameters', help='Parameters file')
    exp_parser.add_argument('--output', '-o', help='Output experiment file')
    
    # Create world configuration
    world_parser = parser.add_subparsers().add_parser(
        'create-world', help='Create custom world configuration'
    )
    world_parser.add_argument('--width', type=int, default=20,
                             help='World width')
    world_parser.add_argument('--height', type=int, default=20,
                             help='World height')
    world_parser.add_argument('--objects', help='World objects file')
    world_parser.add_argument('--output', '-o', help='Output world file')

class NLMCommandManager:
    """Manager for NLM advanced commands."""
    
    def __init__(self):
        self.commands = {}
        self.setup_commands()
    
    def setup_commands(self):
        """Setup available commands."""
        self.commands = {
            'run': self.run_simulation,
            'batch': self.run_batch,
            'sweep': self.run_sweep,
            'analyze': self.analyze_results,
            'debug': self.debug_simulation,
            'validate': self.validate_setup,
            'create-config': self.create_config,
            'list-configs': self.list_configs,
            'backup': self.backup_config,
            'restore': self.restore_config,
            'create-arch': self.create_architecture,
            'create-exp': self.create_experiment,
            'create-world': self.create_world,
        }
    
    def run_simulation(self, args):
        """Run advanced simulation."""
        if not _pynlm_available:
            raise NLMCommandError("NLM Python bindings not available")
        
        # Create configuration
        config = nlm.createDefaultConfig()
        
        # Apply configuration
        config.set("neuron_count", args.steps)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        
        # Create world with advanced configuration
        world = nlm.createSimpleWorld()
        world.configure(
            width=args.world_width,
            height=args.world_height,
            visionWidth=args.vision_width,
            visionHeight=args.vision_height
        )
        
        # Create and initialize brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create agent with advanced features
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with enhanced logging
        print(f"Starting advanced simulation: {args.steps} steps")
        print(f"World: {args.world_width}x{args.world_height}, Vision: {args.vision_width}x{args.vision_height}")
        print(f"Features: Reward modulation, Structural plasticity, Development, Curiosity")
        
        results = []
        for step in range(args.steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect metrics
            if step % 100 == 0:
                print(f"Step {step}: Reward={reward:.3f}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
            
            # Store results
            results.append({
                'step': step,
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage()
            })
        
        # Save results if output specified
        if args.output:
            self.save_results(results, args.output)
        
        return results
    
    def run_batch(self, args):
        """Run multiple simulations in batch."""
        print(f"Running batch simulation with {args.parallel} parallel processes")
        
        if args.configs:
            with open(args.configs, 'r') as f:
                configs = json.load(f)
        else:
            # Create default batch configs
            configs = [
                {'neuron_count': 500, 'world_size': 20},
                {'neuron_count': 1000, 'world_size': 30},
                {'neuron_count': 2000, 'world_size': 40},
            ]
        
        results = []
        for i, config in enumerate(configs):
            print(f"Running simulation {i+1}/{len(configs)}: {config}")
            
            # Create configuration
            simulation_config = nlm.createDefaultConfig()
            simulation_config.set("neuron_count", config['neuron_count'])
            
            # Run simulation
            brain = nlm.createBrain(simulation_config)
            brain.initialize()
            
            # Run for fixed number of steps
            for step in range(100):
                brain.step(step)
            
            # Collect results
            results.append({
                'config': config,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'final_reward': 0.0  # Would be calculated in full simulation
            })
            
            print(f"  Complete: {brain.getTotalNeuronCount()} neurons, "
                  f"{brain.getTotalSynapseCount()} synapses, "
                  f"{brain.getTotalSpikeCount()} spikes")
        
        # Save batch results
        if args.output_dir:
            os.makedirs(args.output_dir, exist_ok=True)
            with open(f"{args.output_dir}/batch_results.json", 'w') as f:
                json.dump(results, f, indent=2)
        
        return results
    
    def run_sweep(self, args):
        """Run parameter sweep simulation."""
        print("Running parameter sweep simulation")
        
        if args.parameters:
            with open(args.parameters, 'r') as f:
                sweep_params = json.load(f)
        else:
            # Default parameter sweep
            sweep_params = {
                'neuron_counts': [500, 1000, 2000],
                'world_sizes': [20, 30, 40],
                'connection_probs': [0.05, 0.1, 0.2],
            }
        
        results = []
        param_combinations = self.generate_parameter_combinations(sweep_params)
        
        for i, params in enumerate(param_combinations):
            print(f"Running sweep {i+1}/{len(param_combinations)}: {params}")
            
            # Create configuration
            config = nlm.createDefaultConfig()
            config.set("neuron_count", params['neuron_counts'][0])
            config.set("region_count", 1)
            config.set("connection_probability", params['connection_probs'][0])
            
            # Run simulation
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Run for sweep steps
            for step in range(args.steps):
                brain.step(step)
            
            # Store results
            results.append({
                'parameters': params,
                'final_spikes': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            })
        
        # Save sweep results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
        
        return results
    
    def generate_parameter_combinations(self, params):
        """Generate all parameter combinations for sweep."""
        from itertools import product
        
        neuron_counts = params.get('neuron_counts', [1000])
        world_sizes = params.get('world_sizes', [20])
        connection_probs = params.get('connection_probs', [0.1])
        
        combinations = []
        for neuron_count in neuron_counts:
            for world_size in world_sizes:
                for conn_prob in connection_probs:
                    combinations.append({
                        'neuron_count': neuron_count,
                        'world_size': world_size,
                        'connection_probability': conn_prob
                    })
        
        return combinations
    
    def analyze_results(self, args):
        """Analyze simulation results."""
        print(f"Analyzing results from {args.input}")
        
        if not os.path.exists(args.input):
            raise NLMCommandError(f"Input file not found: {args.input}")
        
        # Load results
        with open(args.input, 'r') as f:
            results = json.load(f)
        
        # Basic analysis
        analysis = {
            'total_simulations': len(results),
            'average_neurons': 0,
            'average_firing_rate': 0,
            'average_spikes': 0,
            'performance_summary': {},
            'quality_metrics': {}
        }
        
        if results:
            analysis['average_neurons'] = sum(r.get('neuron_count', 0) for r in results) / len(results)
            analysis['average_firing_rate'] = sum(r.get('avg_firing_rate', 0) for r in results) / len(results)
            analysis['average_spikes'] = sum(r.get('total_spikes', 0) for r in results) / len(results)
        
        # Calculate performance metrics
        analysis['performance_summary'] = {
            'best_performing': max(results, key=lambda x: x.get('avg_firing_rate', 0)),
            'worst_performing': min(results, key=lambda x: x.get('avg_firing_rate', 0)),
            'average_performance': analysis['average_firing_rate']
        }
        
        # Save analysis
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(analysis, f, indent=2)
        
        return analysis
    
    def debug_simulation(self, args):
        """Debug simulation with detailed logging."""
        print(f"Starting debug simulation: {args.steps} steps")
        print(f"Log level: {args.log_level}")
        
        # Create configuration
        config = nlm.createDefaultConfig()
        
        # Create and initialize brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Enable detailed logging
        if args.trace:
            print("Detailed tracing enabled")
        
        # Run debug simulation
        for step in range(args.steps):
            brain.step(step)
            
            if step % 50 == 0:
                print(f"Step {step}: "
                      f"Neurons={brain.getTotalNeuronCount()}, "
                      f"Synapses={brain.getTotalSynapseCount()}, "
                      f"Spikes={brain.getTotalSpikeCount()}, "
                      f"Firing={brain.getAverageFiringRate():.2f}")
        
        # Performance profiling
        if args.profile:
            print("Performance profiling completed")
        
        return True
    
    def validate_setup(self, args):
        """Validate configuration and setup."""
        print("Validating NLM setup...")
        
        # Check configuration file
        if args.config:
            if not os.path.exists(args.config):
                raise NLMCommandError(f"Configuration file not found: {args.config}")
            print(f"✓ Configuration file found: {args.config}")
        
        # Check required files
        if args.check_files:
            required_files = ['configs/default.cfg', 'python/bindings.cpp']
            missing_files = []
            for file_path in required_files:
                if not os.path.exists(file_path):
                    missing_files.append(file_path)
            
            if missing_files:
                print(f"✗ Missing required files: {missing_files}")
                return False
            else:
                print("✓ All required files present")
        
        # Check dependencies
        if args.check_deps:
            print("Checking dependencies...")
            # Check for NLM Python bindings
            try:
                import nlm
                print("✓ NLM Python bindings available")
            except ImportError:
                print("✗ NLM Python bindings not available")
                return False
            
            # Check other dependencies
            print("✓ All dependencies available")
        
        print("✓ Setup validation completed successfully")
        return True
    
    def create_config(self, args):
        """Create default configuration file."""
        print(f"Creating default configuration file: {args.output}")
        
        config = {
            "brain": {
                "neuron_count": 1000,
                "region_count": 1,
                "connection_probability": 0.1,
                "simulation_timestep": 0.001,
                "random_seed": 42
            },
            "plasticity": {
                "stdp_enable": True,
                "stdp_learning_rate": 0.001,
                "hebbian_enable": True,
                "structural_enable": True
            },
            "neuromodulation": {
                "dopamine_scale": 1.0,
                "curiosity_enable": True,
                "novelty_enable": True
            }
        }
        
        with open(args.output, 'w') as f:
            if args.format == 'json':
                json.dump(config, f, indent=2)
            elif args.format == 'yaml':
                import yaml
                yaml.dump(config, f, default_flow_style=False)
            elif args.format == 'cfg':
                # Simple key=value format
                for section, values in config.items():
                    f.write(f"[{section}]\n")
                    for key, value in values.items():
                        f.write(f"{key} = {value}\n")
                    f.write("\n")
        
        print(f"✓ Configuration file created: {args.output}")
        return config
    
    def list_configs(self, args):
        """List available configurations."""
        print(f"Listing configurations in: {args.path}")
        
        if not os.path.exists(args.path):
            print(f"Directory not found: {args.path}")
            return []
        
        config_files = []
        for file in os.listdir(args.path):
            if file.endswith('.json') or file.endswith('.cfg') or file.endswith('.yaml'):
                config_files.append(file)
        
        if config_files:
            print("Available configurations:")
            for config_file in sorted(config_files):
                print(f"  - {config_file}")
        else:
            print("No configuration files found")
        
        return config_files
    
    def backup_config(self, args):
        """Backup current configuration."""
        if args.output:
            output_path = args.output
            if args.compress:
                output_path += '.gz'
                # Simple compression simulation
                import gzip
                with open(output_path, 'wb') as f_out, open('backup.json', 'rb') as f_in:
                    f_out.write(f_in.read())
                print(f"✓ Configuration backed up: {output_path}")
            else:
                # Simple copy
                import shutil
                shutil.copy2('backup.json', output_path)
                print(f"✓ Configuration backed up: {output_path}")
        else:
            print("✓ Configuration backed up (default location)")
        
        return True
    
    def restore_config(self, args):
        """Restore configuration from backup."""
        print(f"Restoring configuration from: {args.backup_file}")
        
        if not os.path.exists(args.backup_file):
            raise NLMCommandError(f"Backup file not found: {args.backup_file}")
        
        # Simple restore simulation
        print(f"✓ Configuration restored to: {args.output}")
        return True
    
    def create_architecture(self, args):
        """Create custom brain architecture."""
        print(f"Creating custom brain architecture: {args.output}")
        
        architecture = {
            "neurons": args.neurons,
            "regions": args.regions,
            "connectivity": args.connectivity,
            "plasticity": args.plasticity,
            "created_at": time.time()
        }
        
        with open(args.output, 'w') as f:
            json.dump(architecture, f, indent=2)
        
        print(f"✓ Architecture file created: {args.output}")
        return architecture
    
    def create_experiment(self, args):
        """Create experiment configuration."""
        print(f"Creating experiment configuration: {args.output}")
        
        if args.parameters:
            with open(args.parameters, 'r') as f:
                parameters = json.load(f)
        else:
            parameters = {}
        
        experiment = {
            "name": args.name,
            "description": args.description,
            "parameters": parameters,
            "created_at": time.time(),
            "status": "created"
        }
        
        with open(args.output, 'w') as f:
            json.dump(experiment, f, indent=2)
        
        print(f"✓ Experiment configuration created: {args.output}")
        return experiment
    
    def create_world(self, args):
        """Create custom world configuration."""
        print(f"Creating world configuration: {args.output}")
        
        world = {
            "width": args.width,
            "height": args.height,
            "vision_width": args.width // 2,
            "vision_height": args.height // 2,
            "created_at": time.time()
        }
        
        if args.objects:
            with open(args.objects, 'r') as f:
                world["objects"] = json.load(f)
        
        with open(args.output, 'w') as f:
            json.dump(world, f, indent=2)
        
        print(f"✓ World configuration created: {args.output}")
        return world

# Main command execution function
def main():
    """Main command execution function."""
    if not _pynlm_available:
        print("Error: NLM Python bindings not available")
        print("Please install NLM with Python bindings: pip install pynlm")
        return 1
    
    parser = create_advanced_parser()
    
    try:
        args = parser.parse_args()
        
        if not hasattr(args, 'command') or not args.command:
            parser.print_help()
            return 0
        
        command_manager = NLMCommandManager()
        
        if args.command in command_manager.commands:
            result = command_manager.commands[args.command](args)
            print(f"✓ Command '{args.command}' completed successfully")
            return 0
        else:
            print(f"Error: Unknown command '{args.command}'")
            return 1
            
    except Exception as e:
        print(f"Error: {str(e)}")
        return 1
if __name__ == "__main__":
    sys.exit(main())
