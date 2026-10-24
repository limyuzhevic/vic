#!/usr/bin/env python3
"""
NLM Python Command Line Interface

A command-line interface for the Neural Learning Machine (NLM) framework.
Provides easy access to brain simulation, configuration management, and experiment execution.
"""

import argparse
import json
import os
import sys
import time
from pathlib import Path
from typing import Optional, List

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

from nlm_py import (
    NLMConfig, NLMBrain, NLMExperiment, NLMSimulation,
    ExperimentRunner, PerformanceMonitor
)
class NLMCLI:
    """Command-line interface for NLM operations."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.experiment = None
        
    def create_brain(self, config_file: Optional[str] = None) -> bool:
        """Create a new brain with optional configuration."""
        try:
            if config_file and os.path.exists(config_file):
                self.config = NLMConfig()
                self.config.load_from_file(config_file)
                print(f"Loaded configuration from: {config_file}")
            else:
                self.config = NLMConfig()
                print("Using default configuration")
                
            self.brain = NLMBrain(self.config)
            print(f"Created brain with {self.config.get('neuron_count', 1000)} neurons")
            return True
        except Exception as e:
            print(f"Error creating brain: {e}")
            return False
            
    def run_simple_test(self, steps: int = 100) -> bool:
        """Run a simple connectivity test."""
        if not self.brain:
            print("No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.initialize()
            print(f"Running simple test for {steps} steps...")
            
            for step in range(steps):
                if self.brain._brain:
                    self.brain._brain.step(step, step * 0.001)
                    
            stats = self.brain.get_stats()
            print(f"Test completed:")
            print(f"  Total spikes: {stats.get('total_spike_count', 0)}")
            print(f"  Active neurons: {stats.get('active_neurons', 0)}")
            print(f"  Firing neurons: {stats.get('firing_neurons', 0)}")
            print(f"  Average firing rate: {stats.get('average_firing_rate', 0.0):.3f} Hz")
            return True
        except Exception as e:
            print(f"Error running test: {e}")
            return False
            
    def run_plasticity_test(self, steps: int = 1000) -> bool:
        """Run plasticity learning test."""
        if not self.brain:
            print("No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.reset()
            self.brain.initialize()
            print(f"Running plasticity test for {steps} steps...")
            
            # Enable plasticity on all synapses
            if self.brain._brain:
                for region in self.brain._brain.getRegions():
                    for synapse in region.getSynapses():
                        synapse.enablePlasticity(True, True, False)
                        
            initial_stats = self.brain.get_stats()
            print(f"Initial stats - Total spikes: {initial_stats.get('total_spike_count', 0)}")
            
            for step in range(steps):
                if self.brain._brain:
                    self.brain._brain.injectCurrentToNeurons(
                        NeuronType.Sensory, 30.0f)
                    self.brain._brain.step(step, step * 0.001)
                    
                if step % 100 == 0:
                    current_stats = self.brain.get_stats()
                    print(f"  Step {step}: Spikes={current_stats.get('total_spike_count', 0)}")
                    
            final_stats = self.brain.get_stats()
            print(f"Plasticity test completed:")
            print(f"  Final spikes: {final_stats.get('total_spike_count', 0)}")
            print(f"  Spike change: {final_stats.get('total_spike_count', 0) - initial_stats.get('total_spike_count', 0)}")
            print(f"  Average firing rate: {final_stats.get('average_firing_rate', 0.0):.3f} Hz")
            return True
        except Exception as e:
            print(f"Error running plasticity test: {e}")
            return False
            
    def run_stdp_test(self, trials: int = 50) -> bool:
        """Run STDP verification test."""
        if not self.brain:
            print("No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.reset()
            self.brain.initialize()
            print(f"Running STDP verification for {trials} trials...")
            
            # Get neurons for pre/post testing
            neurons = self.brain.get_neurons()
            if len(neurons) < 2:
                print("Error: Need at least 2 neurons for STDP test")
                return False
                
            pre_neuron = neurons[0]
            post_neuron = neurons[1]
            
            print(f"Using neurons: {pre_neuron.get_id()} (pre), {post_neuron.get_id()} (post)")
            
            # Create correlated activity (pre before post)
            for trial in range(trials):
                if self.brain._brain:
                    # Pre fires first
                    pre_neuron._neuron.injectCurrent(60.0f)
                    self.brain._brain.step(trial * 2, trial * 2 * 0.001)
                    
                    # Post fires second
                    post_neuron._neuron.injectCurrent(60.0f)
                    self.brain._brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
                    
                if (trial + 1) % 10 == 0:
                    print(f"  Completed {trial + 1}/{trials} trials")
                    
            print(f"STDP test completed")
            return True
        except Exception as e:
            print(f"Error running STDP test: {e}")
            return False
            
    def run_experiment(self, steps: int = 500, timestep: float = 0.001) -> bool:
        """Run complete experiment with results tracking."""
        if not self.brain:
            print("No brain created. Use --create-brain first.")
            return False
            
        try:
            self.brain.initialize()
            self.experiment = NLMExperiment(self.brain)
            self.experiment.start()
            
            print(f"Running experiment for {steps} steps with timestep {timestep}...")
            
            for step in range(steps):
                if self.brain._brain:
                    self.brain._brain.step(step, step * timestep)
                    self.experiment.record(step, self.brain.get_stats())
                    
                if step % 100 == 0:
                    print(f"  Progress: {step}/{steps} steps")
                    
            results = self.experiment.end()
            
            print(f"\nExperiment Results:")
            print(f"  Duration: {results.duration:.2f} seconds")
            print(f"  Total steps: {results.get_summary()['total_steps']}")
            print(f"  Total spikes: {results.brain_stats.get('total_spike_count', 0)}")
            print(f"  Average firing rate: {results.brain_stats.get('average_firing_rate', 0.0):.3f} Hz")
            
            return True
        except Exception as e:
            print(f"Error running experiment: {e}")
            return False
            
    def save_brain(self, filepath: str) -> bool:
        """Save brain state to file."""
        if not self.brain:
            print("No brain to save.")
            return False
            
        try:
            self.brain.save_state(filepath)
            print(f"Brain saved to: {filepath}")
            return True
        except Exception as e:
            print(f"Error saving brain: {e}")
            return False
            
    def load_brain(self, filepath: str) -> bool:
        """Load brain state from file."""
        if not self.brain:
            print("Create a brain first, or specify --new-brain to create one.")
            return False
            
        try:
            self.brain.load_state(filepath)
            print(f"Brain loaded from: {filepath}")
            return True
        except Exception as e:
            print(f"Error loading brain: {e}")
            return False
            
    def show_brain_stats(self):
        """Display comprehensive brain statistics."""
        if not self.brain:
            print("No brain created.")
            return
            
        try:
            stats = self.brain.get_stats()
            print("Brain Statistics:")
            print(f"  Total neurons: {stats.get('total_neurons', 0)}")
            print(f"  Total synapses: {stats.get('total_synapses', 0)}")
            print(f"  Active neurons: {stats.get('active_neurons', 0)}")
            print(f"  Firing neurons: {stats.get('firing_neurons', 0)}")
            print(f"  Average firing rate: {stats.get('average_firing_rate', 0.0):.3f} Hz")
            print(f"  Excitation/Inhibition ratio: {stats.get('excitation_inhibition_ratio', 0.0):.3f}")
            print(f"  Total spike count: {stats.get('total_spike_count', 0)}")
            print(f"  Developmental stage: {stats.get('developmental_stage', 'Unknown')}")
            print(f"  Region count: {stats.get('region_count', 0)}")
        except Exception as e:
            print(f"Error getting brain stats: {e}")
            
    def show_config(self):
        """Display current configuration."""
        if not self.config:
            print("No configuration loaded.")
            return
            
        print("Current Configuration:")
        config_dict = self.config.to_dict()
        for key, value in sorted(config_dict.items()):
            print(f"  {key}: {value}")
    
    def run_preset_experiments(self):
        """Run predefined experiment sets."""
        print("Available preset experiments:")
        print("1. Connectivity Test - Basic neural connection test")
        print("2. Plasticity Test - Learning and synaptic modification")
        print("3. STDP Verification - Spike timing dependent plasticity")
        print("4. Full Experiment - Complete simulation with analysis")
        
        # Simple interactive selection
        print("\nNote: For full experiment selection, use the --run-experiment flag.")
        print("Individual experiments can be run with --connectivity-test, --plasticity-test, etc.")
def main():
    parser = argparse.ArgumentParser(
        description="NLM Python Command Line Interface - Neural Learning Machine",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli.py --create-brain configs/default.cfg --run-experiment
  nlm_cli.py --create-brain --connectivity-test 100
  nlm_cli.py --create-brain --plasticity-test 500
  nlm_cli.py --create-brain --stdp-test 50
  nlm_cli.py --create-brain --run-experiment --steps 1000
  nlm_cli.py --create-brain --save-brain my_brain.bin
  nlm_cli.py --create-brain --load-brain my_brain.bin
  nlm_cli.py --create-brain --show-stats
  nlm_cli.py --create-brain --show-config

Configuration Files:
  configs/default.cfg - Default configuration
  Configs can be created/modified using --config-set KEY VALUE
        """
    )
    
    # Main actions
    action_group = parser.add_mutually_exclusive_group()
    action_group.add_argument('--create-brain', action='store_true',
                             help='Create a new brain with optional configuration')
    action_group.add_argument('--run-experiment', action='store_true',
                             help='Run complete experiment')
    action_group.add_argument('--connectivity-test', action='store_true',
                             help='Run connectivity test')
    action_group.add_argument('--plasticity-test', action='store_true',
                             help='Run plasticity learning test')
    action_group.add_argument('--stdp-test', action='store_true',
                             help='Run STDP verification test')
    
    # Configuration options
    config_group = parser.add_argument_group('Configuration Options')
    config_group.add_argument('--config-file', type=str, default='configs/default.cfg',
                             help='Configuration file to load (default: configs/default.cfg)')
    config_group.add_argument('--steps', type=int, default=500,
                             help='Number of simulation steps (default: 500)')
    config_group.add_argument('--timestep', type=float, default=0.001,
                             help='Simulation timestep (default: 0.001)')
    
    # File operations
    file_group = parser.add_argument_group('File Operations')
    file_group.add_argument('--save-brain', type=str, help='Save brain state to file')
    file_group.add_argument('--load-brain', type=str, help='Load brain state from file')
    
    # Information commands
    info_group = parser.add_argument_group('Information Commands')
    info_group.add_argument('--show-stats', action='store_true',
                           help='Show brain statistics')
    info_group.add_argument('--show-config', action='store_true',
                           help='Show configuration')
    info_group.add_argument('--help-experiments', action='store_true',
                           help='Show available preset experiments')
    
    # Development options
    dev_group = parser.add_argument_group('Development Options')
    dev_group.add_argument('--enable-performance-monitoring', action='store_true',
                          help='Enable performance monitoring')
    dev_group.add_argument('--config-set', nargs=2, action='append',
                          help='Set configuration value (KEY VALUE)')
    
    args = parser.parse_args()
    
    # Initialize CLI
    cli = NLMCLI()
    
    # Handle configuration set options
    if args.config_set:
        from nlm_py import NLMConfig
        config = NLMConfig()
        for key, value in args.config_set:
            # Try to convert value to appropriate type
            try:
                if value.lower() in ('true', 'false'):
                    value = value.lower() == 'true'
                elif '.' in value:
                    value = float(value)
                else:
                    value = int(value)
            except ValueError:
                pass  # Keep as string
            config.set(key, value)
        
        print("Configuration:")
        config_dict = config.to_dict()
        for key, value in sorted(config_dict.items()):
            print(f"  {key}: {value}")
        return
    
    # Handle --help-experiments
    if args.help_experiments:
        cli.run_preset_experiments()
        return
    
    # Main execution flow
    if not args.create_brain and not any([
        args.run_experiment, args.connectivity_test, args.plasticity_test, 
        args.stdp_test, args.save_brain, args.load_brain, args.show_stats, 
        args.show_config
    ]):
        parser.print_help()
        return
    
    # Create brain if needed
    if args.create_brain or any([args.run_experiment, args.connectivity_test, 
                                args.plasticity_test, args.stdp_test, args.save_brain, 
                                args.load_brain, args.show_stats, args.show_config]):
        if not cli.create_brain(args.config_file):
            sys.exit(1)
    
    # Execute requested actions
    try:
        if args.run_experiment:
            print(f"Running experiment with {args.steps} steps...")
            cli.run_experiment(args.steps, args.timestep)
            
        elif args.connectivity_test:
            print(f"Running connectivity test with {args.steps} steps...")
            cli.run_simple_test(args.steps)
            
        elif args.plasticity_test:
            print(f"Running plasticity test with {args.steps} steps...")
            cli.run_plasticity_test(args.steps)
            
        elif args.stdp_test:
            print(f"Running STDP test with {args.steps} trials...")
            cli.run_stdp_test(args.steps)
            
        elif args.save_brain:
            cli.save_brain(args.save_brain)
            
        elif args.load_brain:
            cli.load_brain(args.load_brain)
            
        elif args.show_stats:
            cli.show_brain_stats()
            
        elif args.show_config:
            cli.show_config()
            
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)
if __name__ == "__main__":
    main()

# End of CLI script - make sure to set executable permissions:
# chmod +x nlm_cli.py
