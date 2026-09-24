#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface

This script provides advanced command-line interface features for NLM.
It allows for complex configuration, multiple scenarios, and batch processing.
"""

import argparse
import json
import sys
import os
import time
from typing import Dict, Any, Optional
from pathlib import Path

# Add the project root to path so we can import NLM modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please build the Python bindings first.")
    sys.exit(1)

class NLMCli:
    """Advanced NLM command line interface with scenario support."""
    
    def __init__(self):
        self.config_templates = {
            'simple': self._get_simple_config(),
            'complex': self._get_complex_config(),
            'experimental': self._get_experimental_config(),
            'production': self._get_production_config()
        }
        
    def _get_simple_config(self) -> Dict[str, Any]:
        """Get simple configuration for beginners."""
        return {
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.1,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'development_synaptogenesis_rate': 0.001,
            'development_pruning_rate': 0.0001,
            'dopamine_baseline': 0.1,
            'reward_discount_factor': 0.99,
            'log_level': 'INFO',
            'visualization_enabled': False,
            'max_simulation_steps': 10000
        }
    
    def _get_complex_config(self) -> Dict[str, Any]:
        """Get complex configuration for advanced users."""
        return {
            'neuron_count': 5000,
            'region_count': 4,
            'connection_probability': 0.15,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.018,
            'stdp_time_constant': 20.0,
            'development_synaptogenesis_rate': 0.0005,
            'development_pruning_rate': 0.00005,
            'development_maturation_rate': 0.001,
            'dopamine_baseline': 0.2,
            'dopamine_sensitivity': 0.5,
            'reward_discount_factor': 0.995,
            'curiosity_beta': 0.1,
            'novelty_threshold': 0.3,
            'working_memory_capacity': 100,
            'episodic_memory_capacity': 1000,
            'log_level': 'DEBUG',
            'visualization_enabled': True,
            'visualization_update_rate': 30.0,
            'checkpoint_interval': 1000,
            'enable_checkpointing': True,
            'enable_replay': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_novelty': True,
            'max_simulation_steps': 50000
        }
    
    def _get_experimental_config(self) -> Dict[str, Any]:
        """Get experimental configuration for research."""
        return {
            'neuron_count': 10000,
            'region_count': 6,
            'connection_probability': 0.12,
            'stdp_ltp_weight': 0.03,
            'stdp_ltd_weight': 0.025,
            'stdp_time_constant': 15.0,
            'development_synaptogenesis_rate': 0.0001,
            'development_pruning_rate': 0.00001,
            'development_maturation_rate': 0.002,
            'dopamine_baseline': 0.3,
            'dopamine_sensitivity': 0.8,
            'curiosity_beta': 0.05,
            'novelty_threshold': 0.5,
            'working_memory_capacity': 200,
            'episodic_memory_capacity': 5000,
            'log_level': 'INFO',
            'visualization_enabled': False,
            'checkpoint_interval': 5000,
            'enable_checkpointing': True,
            'enable_replay': True,
            'enable_development': True,
            'enable_curiosity': True,
            'enable_novelty': True,
            'max_simulation_steps': 100000,
            'simulation_time_limit': 3600.0
        }
    
    def _get_production_config(self) -> Dict[str, Any]:
        """Get production configuration for stable operation."""
        return {
            'neuron_count': 2000,
            'region_count': 3,
            'connection_probability': 0.08,
            'stdp_ltp_weight': 0.015,
            'stdp_ltd_weight': 0.013,
            'development_synaptogenesis_rate': 0.0002,
            'development_pruning_rate': 0.00002,
            'dopamine_baseline': 0.15,
            'reward_discount_factor': 0.99,
            'log_level': 'WARNING',
            'visualization_enabled': False,
            'max_simulation_steps': 20000,
            'enable_checkpointing': True,
            'enable_replay': False,
            'enable_development': True,
            'enable_curiosity': False,
            'enable_novelty': False
        }
    
    def create_config_from_args(self, args) -> Any:
        """Create NLM configuration from command line arguments."""
        # Start with template if specified
        if args.template:
            if args.template not in self.config_templates:
                raise ValueError(f"Unknown template: {args.template}")
            config = self.config_templates[args.template]
        else:
            config = self._get_simple_config()
        
        # Override with command line arguments
        if args.neuron_count:
            config['neuron_count'] = args.neuron_count
        if args.region_count:
            config['region_count'] = args.region_count
        if args.connection_probability:
            config['connection_probability'] = args.connection_probability
        if args.stdp_ltp_weight:
            config['stdp_ltp_weight'] = args.stdp_ltp_weight
        if args.stdp_ltd_weight:
            config['stdp_ltd_weight'] = args.stdp_ltd_weight
        if args.log_level:
            config['log_level'] = args.log_level
            
        # Additional settings
        if args.enable_development:
            config['enable_development'] = True
        if args.disable_development:
            config['enable_development'] = False
            
        if args.enable_curiosity:
            config['enable_curiosity'] = True
        if args.disable_curiosity:
            config['enable_curiosity'] = False
            
        if args.enable_novelty:
            config['enable_novelty'] = True
        if args.disable_novelty:
            config['enable_novelty'] = False
        
        # Convert to NLM config object
        nlm_config = pynlm.createDefaultConfig()
        
        # Set all config values
        for key, value in config.items():
            if isinstance(value, bool):
                nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, int):
                nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, float):
                nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, str):
                nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
        
        return nlm_config
    
    def run_scenario(self, brain, world, agent, steps: int, scenario_name: str) -> Dict[str, Any]:
        """Run a complete scenario and collect metrics."""
        print(f"\n{'='*60}")
        print(f"RUNNING SCENARIO: {scenario_name}")
        print(f"{'='*60}")
        
        start_time = time.time()
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                firing = brain.getFiringNeuronCount()
                spikes = brain.getTotalSpikeCount()
                avg_firing = brain.getAverageFiringRate()
                print(f"Step {step:5d}: Firing={firing:4d}, Spikes={spikes:6d}, AvgFiring={avg_firing:5.2f} Hz")
        
        end_time = time.time()
        
        # Collect final metrics
        metrics = {
            'scenario': scenario_name,
            'total_steps': steps,
            'total_firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'working_memory_used': agent.getBrain().getWorkingMemory().getActiveTraces() if agent.getBrain().getWorkingMemory() else 0,
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'duration_seconds': end_time - start_time
        }
        
        print(f"\n{'='*60}")
        print(f"SCENARIO COMPLETE: {scenario_name}")
        print(f"{'='*60}")
        print(f"Total steps: {metrics['total_steps']}")
        print(f"Final firing neurons: {metrics['total_firing_neurons']}")
        print(f"Total spikes: {metrics['total_spikes']}")
        print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
        print(f"Development stage: {metrics['development_stage']}")
        print(f"Working memory traces: {metrics['working_memory_used']}")
        print(f"Curiosity level: {metrics['curiosity_level']:.3f}")
        print(f"Novelty level: {metrics['novelty_level']:.3f}")
        print(f"Duration: {metrics['duration_seconds']:.2f} seconds")
        
        return metrics
    
    def run_comparison(self, configs: list, steps: int, output_file: str = "comparison_results.json"):
        """Run comparison between different configurations."""
        print(f"\n{'='*60}")
        print("RUNNING CONFIGURATION COMPARISON")
        print(f"{'='*60}")
        
        results = []
        
        for i, config_dict in enumerate(configs):
            print(f"\n--- Configuration {i+1}/{len(configs)} ---")
            
            # Create config
            nlm_config = pynlm.createDefaultConfig()
            for key, value in config_dict.items():
                if isinstance(value, bool):
                    nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
                elif isinstance(value, int):
                    nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
                elif isinstance(value, float):
                    nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
            
            # Create brain, world, and agent
            brain = pynlm.createBrain(nlm_config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features based on config
            if config_dict.get('enable_reward_modulation', False):
                agent.enableRewardModulation(True)
            if config_dict.get('enable_structural_plasticity', False):
                agent.enableStructuralPlasticity(True)
            if config_dict.get('enable_development', False):
                agent.enableDevelopment(True)
            if config_dict.get('enable_curiosity', False):
                agent.enableCuriosity(True)
            if config_dict.get('enable_novelty', False):
                agent.enableNovelty(True)
            
            # Run scenario
            config_name = config_dict.get('name', f'Config_{i+1}')
            metrics = self.run_scenario(brain, world, agent, steps, config_name)
            metrics['config_name'] = config_name
            results.append(metrics)
        
        # Save results
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\n{'='*60}")
        print("COMPARISON COMPLETE")
        print(f"{'='*60}")
        print(f"Results saved to: {output_file}")
        
        # Print summary
        print(f"\n{'='*60}")
        print("SUMMARY")
        print(f"{'='*60}")
        for result in results:
            print(f"{result['config_name']}: {result['total_spikes']} spikes, "
                  f"{result['average_firing_rate']:.2f} avg firing, "
                  f"{result['curiosity_level']:.3f} curiosity, "
                  f"{result['duration_seconds']:.1f}s")
        
        return results

def main():
    """Main CLI function."""
    parser = argparse.ArgumentParser(
        description='NLM Advanced Command Line Interface',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s run --template experimental --steps 1000
  %(prog)s compare --template1 simple --template2 complex --steps 500
  %(prog)s create-config --template experimental > config.json
  %(prog)s scenario --name exploration --steps 2000 --vision-width 16
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run command
    parser_run = subparsers.add_parser('run', help='Run NLM with specified configuration')
    parser_run.add_argument('--template', choices=['simple', 'complex', 'experimental', 'production'],
                           default='simple', help='Configuration template')
    parser_run.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser_run.add_argument('--neuron-count', type=int, help='Override neuron count')
    parser_run.add_argument('--region-count', type=int, help='Override region count')
    parser_run.add_argument('--connection-probability', type=float, help='Override connection probability')
    parser_run.add_argument('--enable-development', action='store_true', help='Enable development')
    parser_run.add_argument('--disable-development', action='store_false', dest='enable_development', help='Disable development')
    parser_run.add_argument('--enable-curiosity', action='store_true', help='Enable curiosity')
    parser_run.add_argument('--disable-curiosity', action='store_false', dest='enable_curiosity', help='Disable curiosity')
    parser_run.add_argument('--enable-novelty', action='store_true', help='Enable novelty')
    parser_run.add_argument('--disable-novelty', action='store_false', dest='enable_novelty', help='Disable novelty')
    parser_run.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'], help='Log level')
    
    # Create config command
    parser_create = subparsers.add_parser('create-config', help='Create configuration file')
    parser_create.add_argument('--template', choices=['simple', 'complex', 'experimental', 'production'],
                              default='simple', help='Configuration template')
    parser_create.add_argument('--output', '-o', help='Output file (default: stdout)')
    
    # Compare command
    parser_compare = subparsers.add_parser('compare', help='Compare multiple configurations')
    parser_compare.add_argument('--template1', choices=['simple', 'complex', 'experimental', 'production'],
                               default='simple', help='First configuration template')
    parser_compare.add_argument('--template2', choices=['simple', 'complex', 'experimental', 'production'],
                               default='complex', help='Second configuration template')
    parser_compare.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser_compare.add_argument('--output', '-o', default='comparison_results.json', help='Output file for comparison results')
    
    # Scenario command
    parser_scenario = subparsers.add_parser('scenario', help='Run specific scenario')
    parser_scenario.add_argument('--name', required=True, help='Scenario name')
    parser_scenario.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser_scenario.add_argument('--vision-width', type=int, default=8, help='Vision width')
    parser_scenario.add_argument('--vision-height', type=int, default=8, help='Vision height')
    parser_scenario.add_argument('--world-width', type=int, default=20, help='World width')
    parser_scenario.add_argument('--world-height', type=int, default=20, help='World height')
    parser_scenario.add_argument('--enable-features', nargs='+', 
                               choices=['reward', 'structural', 'development', 'curiosity', 'novelty'],
                               help='Enable additional features')
    
    # List command
    parser_list = subparsers.add_parser('list', help='List available templates and options')
    
    # Default action (when no command specified)
    parser.set_defaults(command='help')
    
    args = parser.parse_args()
    
    cli = NLMCli()
    
    try:
        if args.command == 'run':
            # Create configuration
            config = cli.create_config_from_args(args)
            
            # Create brain, world, and agent
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, 
                          visionWidth=args.vision_width if hasattr(args, 'vision_width') else 8,
                          visionHeight=args.vision_height if hasattr(args, 'vision_height') else 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features based on command line arguments
            if args.enable_development:
                agent.enableDevelopment(True)
            if args.enable_curiosity:
                agent.enableCuriosity(True)
            if args.enable_novelty:
                agent.enableNovelty(True)
            
            # Run scenario
            print(f"Running NLM with {args.template} template for {args.steps} steps...")
            metrics = cli.run_scenario(brain, world, agent, args.steps, f"{args.template}_scenario")
            
        elif args.command == 'create-config':
            # Create configuration file
            template_name = args.template
            if template_name not in cli.config_templates:
                print(f"Error: Unknown template: {template_name}")
                return 1
            
            config_data = cli.config_templates[template_name]
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(config_data, f, indent=2)
                print(f"Configuration saved to: {args.output}")
            else:
                json.dump(config_data, sys.stdout, indent=2)
                
        elif args.command == 'compare':
            # Run comparison
            config1 = cli.config_templates[args.template1].copy()
            config2 = cli.config_templates[args.template2].copy()
            
            config1['name'] = args.template1
            config2['name'] = args.template2
            
            results = cli.run_comparison([config1, config2], args.steps, args.output)
            
        elif args.command == 'scenario':
            # Run specific scenario
            config = pynlm.createDefaultConfig()
            
            # Set basic config
            config.set('neuron_count', 1000, pynlm.ConfigSource.Runtime)
            config.set('region_count', 2, pynlm.ConfigSource.Runtime)
            config.set('connection_probability', 0.1, pynlm.ConfigSource.Runtime)
            config.set('log_level', 'INFO', pynlm.ConfigSource.Runtime)
            
            # Create brain, world, and agent
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=args.world_width, height=args.world_height,
                          visionWidth=args.vision_width, visionHeight=args.vision_height)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features based on arguments
            if args.enable_features:
                feature_map = {
                    'reward': lambda: agent.enableRewardModulation(True),
                    'structural': lambda: agent.enableStructuralPlasticity(True),
                    'development': lambda: agent.enableDevelopment(True),
                    'curiosity': lambda: agent.enableCuriosity(True),
                    'novelty': lambda: agent.enableNovelty(True)
                }
                
                for feature in args.enable_features:
                    if feature in feature_map:
                        feature_map[feature]()
            
            # Run scenario
            metrics = cli.run_scenario(brain, world, agent, args.steps, args.name)
            
        elif args.command == 'list':
            # List available templates
            print("Available Templates:")
            print("=" * 40)
            for template_name, config in cli.config_templates.items():
                print(f"\n{template_name.upper()}:")
                print(f"  Neurons: {config.get('neuron_count', 'N/A')}")
                print(f"  Regions: {config.get('region_count', 'N/A')}")
                print(f"  Connection Probability: {config.get('connection_probability', 'N/A')}")
                print(f"  Development: {config.get('enable_development', 'N/A')}")
                print(f"  Curiosity: {config.get('enable_curiosity', 'N/A')}")
                print(f"  Novelty: {config.get('enable_novelty', 'N/A')}")
            
            print(f"\nAvailable Commands:")
            print("=" * 40)
            for action in parser._actions:
                if action.dest != 'help' and action.dest != 'command':
                    print(f"  {action.option_strings}: {action.help}")
        
        else:
            parser.print_help()
            
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == '__main__':
    exit(main())