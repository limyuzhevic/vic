#!/usr/bin/env python3
"""
NLM Utility Tools

Collection of command-line utilities and helper scripts for working with NLM (Neural Learning Machine).

This script provides several useful tools for:
- Network analysis and visualization
- Configuration management
- Performance profiling
- Data export/import
- Experiment management

Usage: python nlm_utils.py <command> [options]
"""

import argparse
import json
import sys
import os
import time
from pathlib import Path
import numpy as np

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some commands will be limited.", file=sys.stderr)

class NLMUtils:
    """Collection of NLM utility functions."""
    
    @staticmethod
    def analyze_network(brain):
        """Analyze network properties and return statistics."""
        if not PYNLM_AVAILABLE or brain is None:
            return None
            
        stats = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount()
        }
        
        # Calculate derived metrics
        if stats['neuron_count'] > 0:
            stats['synapses_per_neuron'] = stats['synapse_count'] / stats['neuron_count']
            if stats['firing_neurons'] > 0:
                stats['average_firing_per_active'] = stats['total_spikes'] / stats['firing_neurons']
        
        return stats
    
    @staticmethod
    def export_network_to_csv(brain, filepath):
        """Export network data to CSV format for analysis."""
        if not PYNLM_AVAILABLE or brain is None:
            return False
            
        try:
            with open(filepath, 'w') as f:
                # Write header
                f.write('neuron_id,type,weight,synaptic_delay,region_id,population_id\n')
                
                # This is a simplified version - a full implementation would need
                # to iterate over all neurons and synapses in the brain
                f.write('# Note: Full implementation would export all network details\n')
                f.write(f'# Total neurons: {brain.getTotalNeuronCount()}\n')
                f.write(f'# Total synapses: {brain.getTotalSynapseCount()}\n')
                
            return True
        except Exception as e:
            print(f"Error exporting network: {e}", file=sys.stderr)
            return False
    
    @staticmethod
    def profile_simulation(brain, steps, timestep=0.001):
        """Profile simulation performance and return metrics."""
        if not PYNLM_AVAILABLE or brain is None:
            return None
            
        start_time = time.time()
        start_step = 0
        
        # Collect metrics during simulation
        metrics = {
            'steps': steps,
            'timestep': timestep,
            'start_time': start_time,
            'metrics_collected': []
        }
        
        for step in range(steps):
            brain.step(step, step * timestep)
            
            # Collect metrics every 100 steps
            if step % 100 == 0:
                metrics['metrics_collected'].append({
                    'step': step,
                    'time': step * timestep,
                    'firing_rate': brain.getAverageFiringRate(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio()
                })
        
        end_time = time.time()
        metrics['end_time'] = end_time
        metrics['total_time'] = end_time - start_time
        metrics['steps_per_second'] = steps / metrics['total_time']
        
        # Add final metrics
        metrics['final_firing_rate'] = brain.getAverageFiringRate()
        metrics['final_active_neurons'] = brain.getActiveNeuronCount()
        metrics['final_total_spikes'] = brain.getTotalSpikeCount()
        metrics['final_e_i_ratio'] = brain.getExcitationInhibitionRatio()
        
        return metrics
    
    @staticmethod
    def create_preset_config(profile_name):
        """Create preset configuration profiles."""
        presets = {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.connection_probability': 0.05,
                'brain.initial_weight_mean': 0.5,
                'brain.initial_weight_std': 0.1
            },
            'balanced': {
                'brain.neuron_count': 500,
                'brain.connection_probability': 0.1,
                'brain.initial_weight_mean': 0.0,
                'brain.initial_weight_std': 0.2,
                'plasticity.stdp.learning_rate': 0.01,
                'plasticity.hebbian.learning_rate': 0.005
            },
            'dense': {
                'brain.neuron_count': 1000,
                'brain.connection_probability': 0.2,
                'brain.initial_weight_mean': 0.2,
                'brain.initial_weight_std': 0.3,
                'plasticity.stdp.learning_rate': 0.02,
                'plasticity.hebbian.learning_rate': 0.01
            },
            'sparse': {
                'brain.neuron_count': 200,
                'brain.connection_probability': 0.02,
                'brain.initial_weight_mean': 0.8,
                'brain.initial_weight_std': 0.1,
                'plasticity.stdp.learning_rate': 0.005,
                'plasticity.hebbian.learning_rate': 0.001
            }
        }
        
        if profile_name not in presets:
            raise ValueError(f"Unknown preset profile: {profile_name}")
        
        return presets[profile_name]
    
    @staticmethod
    def generate_random_parameters(param_spec, seed=None):
        """Generate random parameters based on specification."""
        if seed is not None:
            np.random.seed(seed)
        
        params = {}
        for key, spec in param_spec.items():
            if spec['type'] == 'int':
                params[key] = int(np.random.uniform(spec['min'], spec['max']))
            elif spec['type'] == 'float':
                params[key] = float(np.random.uniform(spec['min'], spec['max']))
            elif spec['type'] == 'bool':
                params[key] = bool(np.random.choice([True, False]))
        
        return params
    
    @staticmethod
    def compare_networks(brain1, brain2, metrics=None):
        """Compare two networks and return differences."""
        if not PYNLM_AVAILABLE or brain1 is None or brain2 is None:
            return None
        
        if metrics is None:
            metrics = ['firing_rate', 'e_i_ratio', 'neuron_count', 'synapse_count']
        
        comparison = {}
        
        for metric in metrics:
            val1 = brain1.getTotalNeuronCount() if metric == 'neuron_count' else \
                   brain1.getTotalSynapseCount() if metric == 'synapse_count' else \
                   brain1.getAverageFiringRate() if metric == 'firing_rate' else \
                   brain1.getExcitationInhibitionRatio() if metric == 'e_i_ratio' else \
                   0.0
            
            val2 = brain2.getTotalNeuronCount() if metric == 'neuron_count' else \
                   brain2.getTotalSynapseCount() if metric == 'synapse_count' else \
                   brain2.getAverageFiringRate() if metric == 'firing_rate' else \
                   brain2.getExcitationInhibitionRatio() if metric == 'e_i_ratio' else \
                   0.0
            
            comparison[metric] = {
                'brain1': val1,
                'brain2': val2,
                'difference': val1 - val2,
                'percent_change': ((val1 - val2) / val2 * 100) if val2 != 0 else float('inf')
            }
        
        return comparison

def create_config_command():
    """Command to create configuration files."""
    parser = argparse.ArgumentParser(description='Create NLM configuration files')
    parser.add_argument('output', help='Output file path')
    parser.add_argument('--profile', choices=['minimal', 'balanced', 'dense', 'sparse'], 
                       default='balanced', help='Configuration profile')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    utils = NLMUtils()
    
    try:
        # Create preset config
        params = utils.create_preset_config(args.profile)
        
        # Save to file
        with open(args.output, 'w') as f:
            json.dump(params, f, indent=2)
        
        print(f"Configuration saved to {args.output}")
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def analyze_command():
    """Command to analyze NLM networks."""
    parser = argparse.ArgumentParser(description='Analyze NLM networks')
    parser.add_argument('steps', type=int, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for analysis results')
    parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Create brain and analyze
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run analysis
        utils = NLMUtils()
        stats = utils.analyze_network(brain)
        
        # Run simulation and collect metrics
        metrics = utils.profile_simulation(brain, args.steps, args.timestep)
        
        # Create results
        results = {
            'network_stats': stats,
            'simulation_metrics': metrics,
            'timestamp': time.time()
        }
        
        # Output results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Analysis results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def export_command():
    """Command to export network data."""
    parser = argparse.ArgumentParser(description='Export NLM network data')
    parser.add_argument('steps', type=int, help='Number of simulation steps')
    parser.add_argument('output', help='Output file path')
    parser.add_argument('--format', choices=['csv', 'json'], default='json', 
                       help='Output format')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Create brain and run simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(args.steps):
            brain.step(step)
        
        utils = NLMUtils()
        
        if args.format == 'csv':
            success = utils.export_network_to_csv(brain, args.output)
        else:  # json
            stats = utils.analyze_network(brain)
            with open(args.output, 'w') as f:
                json.dump(stats, f, indent=2)
            success = True
        
        if success:
            print(f"Network exported to {args.output}")
            return 0
        else:
            print("Error: Failed to export network", file=sys.stderr)
            return 1
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def compare_command():
    """Command to compare two networks."""
    parser = argparse.ArgumentParser(description='Compare NLM networks')
    parser.add_argument('config1', help='First configuration file')
    parser.add_argument('config2', help='Second configuration file')
    parser.add_argument('steps', type=int, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for comparison results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configurations
        with open(args.config1, 'r') as f:
            params1 = json.load(f)
        
        with open(args.config2, 'r') as f:
            params2 = json.load(f)
        
        # Create brains
        config1 = pynlm.createDefaultConfig()
        for key, value in params1.items():
            config1.set(key, value)
        
        config2 = pynlm.createDefaultConfig()
        for key, value in params2.items():
            config2.set(key, value)
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brain1.initialize()
        brain2.initialize()
        
        # Run simulations
        for step in range(args.steps):
            brain1.step(step)
            brain2.step(step)
        
        # Compare networks
        utils = NLMUtils()
        comparison = utils.compare_networks(brain1, brain2)
        
        results = {
            'config1': args.config1,
            'config2': args.config2,
            'steps': args.steps,
            'comparison': comparison,
            'timestamp': time.time()
        }
        
        # Output results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Comparison results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def generate_random_command():
    """Command to generate random parameters."""
    parser = argparse.ArgumentParser(description='Generate random NLM parameters')
    parser.add_argument('output', help='Output file for generated parameters')
    parser.add_argument('--seed', type=int, help='Random seed for reproducibility')
    
    args = parser.parse_args()
    
    # Parameter specification
    param_spec = {
        'brain.neuron_count': {'type': 'int', 'min': 100, 'max': 1000},
        'brain.connection_probability': {'type': 'float', 'min': 0.01, 'max': 0.3},
        'brain.initial_weight_mean': {'type': 'float', 'min': -0.5, 'max': 0.5},
        'brain.initial_weight_std': {'type': 'float', 'min': 0.05, 'max': 0.5},
        'plasticity.stdp.learning_rate': {'type': 'float', 'min': 0.001, 'max': 0.05},
        'plasticity.hebbian.learning_rate': {'type': 'float', 'min': 0.001, 'max': 0.02}
    }
    
    try:
        utils = NLMUtils()
        params = utils.generate_random_parameters(param_spec, args.seed)
        
        with open(args.output, 'w') as f:
            json.dump(params, f, indent=2)
        
        print(f"Generated random parameters saved to {args.output}")
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description='NLM Utility Tools')
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create subparsers for different commands
    create_parser = subparsers.add_parser('create', help='Create configuration files')
    create_parser.set_defaults(func=create_config_command)
    
    analyze_parser = subparsers.add_parser('analyze', help='Analyze NLM networks')
    analyze_parser.set_defaults(func=analyze_command)
    
    export_parser = subparsers.add_parser('export', help='Export network data')
    export_parser.set_defaults(func=export_command)
    
    compare_parser = subparsers.add_parser('compare', help='Compare two networks')
    compare_parser.set_defaults(func=compare_command)
    
    generate_parser = subparsers.add_parser('generate', help='Generate random parameters')
    generate_parser.set_defaults(func=generate_random_command)
    
    # If no command provided, show help
    if len(sys.argv) == 1:
        parser.print_help()
        return 1
    
    try:
        return subparsers.choices[sys.argv[1]].func()
    except KeyError:
        print(f"Unknown command: {sys.argv[1]}", file=sys.stderr)
        return 1

if __name__ == '__main__':
    sys.exit(main())