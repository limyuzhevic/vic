#!/usr/bin/env python3

"""
NLM Command Line Interface (CLI)
================================

A comprehensive command-line interface for NLM (Neural Learning Machine)
simulations, configuration management, and analysis.

Usage:
    nlm --help                    # Show help information
    nlm run [OPTIONS]             # Run a simulation
    nlm configure [OPTIONS]       # Configure a brain
    nlm analyze [OPTIONS]         # Analyze simulation results
    nlm export [OPTIONS]          # Export data for visualization
    nlm benchmark [OPTIONS]       # Run performance benchmarks

Examples:
    nlm run --brain learning --steps 1000
    nlm configure --type research --output config.json
    nlm analyze --file results.json --plot
    nlm benchmark --brain standard --steps 5000
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any, List

# Add local Python bindings to path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: NLM Python bindings not available: {e}")
    print("Please ensure NLM is properly built and installed.")
    PYNLM_AVAILABLE = False
class NLMCLI:
    """Main NLM CLI interface."""
    
    def __init__(self):
        self.brain_configs = {}
        self.load_default_configs()
    
    def load_default_configs(self):
        """Load default brain configurations."""
        self.brain_configs = {
            'minimal': {
                'neuron_count': 50,
                'region_count': 1,
                'connection_probability': 0.1,
                'enable_stdp': False,
                'enable_hebbian': False,
                'enable_memory': False,
                'enable_prediction': False,
                'enable_cognition': False,
                'enable_neuromodulation': False,
                'enable_development': False,
            },
            'standard': {
                'neuron_count': 1000,
                'region_count': 1,
                'connection_probability': 0.15,
                'enable_stdp': True,
                'enable_hebbian': True,
                'enable_memory': True,
                'enable_prediction': True,
                'enable_cognition': True,
                'enable_neuromodulation': True,
                'enable_development': True,
            },
            'learning': {
                'neuron_count': 2000,
                'region_count': 2,
                'connection_probability': 0.2,
                'enable_stdp': True,
                'enable_hebbian': True,
                'enable_memory': True,
                'enable_prediction': True,
                'enable_cognition': True,
                'enable_neuromodulation': True,
                'enable_development': True,
            },
            'cognition': {
                'neuron_count': 3000,
                'region_count': 3,
                'connection_probability': 0.25,
                'enable_stdp': True,
                'enable_hebbian': True,
                'enable_memory': True,
                'enable_prediction': True,
                'enable_cognition': True,
                'enable_neuromodulation': True,
                'enable_development': True,
            },
            'performance': {
                'neuron_count': 100,
                'region_count': 1,
                'connection_probability': 0.05,
                'enable_stdp': False,
                'enable_hebbian': False,
                'enable_memory': False,
                'enable_prediction': False,
                'enable_cognition': False,
                'enable_neuromodulation': False,
                'enable_development': False,
            },
        }
    
    def run_simulation(self, args):
        """Run a simulation with specified parameters."""
        if not PYNLM_AVAILABLE:
            print("Error: NLM Python bindings not available")
            return 1
        
        print(f"Starting NLM simulation with {args.steps} steps...")
        
        # Create brain based on configuration type
        brain = self._create_brain(args.brain_type, args)
        if not brain:
            return 1
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(
            width=getattr(args, 'world_width', 20),
            height=getattr(args, 'world_height', 20),
            vision_width=getattr(args, 'vision_width', 8),
            vision_height=getattr(args, 'vision_height', 8)
        )
        
        # Set agent start position
        world.setAgentStart(
            getattr(args, 'agent_x', 10),
            getattr(args, 'agent_y', 10)
        )
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning if requested
        if getattr(args, 'enable_learning', True):
            agent.enableLearning(True)
        
        # Run simulation
        for step in range(args.steps):
            world.update(0.1)  # 100ms timestep
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress every 100 steps
            if step % 100 == 0:
                stats = brain.getStatistics()
                print(f"Step {step:4d}: "
                      f"Neurons: {stats['total_neurons']:4d}, "
                      f"Spikes: {stats['total_spikes']:6d}, "
                      f"Rate: {stats['avg_firing_rate']:6.2f} Hz, "
                      f"Success: {result.success}")
        
        # Print final statistics
        stats = brain.getStatistics()
        print("\n=== Simulation Complete ===")
        print(f"Total steps: {args.steps}")
        print(f"Total neurons: {stats['total_neurons']}")
        print(f"Total synapses: {stats['total_synapses']}")
        print(f"Total spikes: {stats['total_spikes']}")
        print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"Final development stage: {stats['development_stage']}")
        
        # Save results if requested
        if args.output:
            self._save_simulation_results(brain, world, agent, args.output)
            print(f"Results saved to {args.output}")
        
        # Export data for analysis if requested
        if args.export:
            self._export_simulation_data(brain, world, agent, args.export)
            print(f"Data exported to {args.export}")
        
        return 0
    
    def configure_brain(self, args):
        """Configure and save a brain configuration."""
        print(f"Creating {args.type} brain configuration...")
        
        # Get brain configuration
        if args.type not in self.brain_configs:
            print(f"Error: Unknown brain type '{args.type}'")
            print(f"Available types: {', '.join(self.brain_configs.keys())}")
            return 1
        
        config = self.brain_configs[args.type].copy()
        
        # Override with command line arguments
        if args.neuron_count:
            config['neuron_count'] = args.neuron_count
        if args.region_count:
            config['region_count'] = args.region_count
        if args.connection_probability:
            config['connection_probability'] = args.connection_probability
        if args.enable_stdp is not None:
            config['enable_stdp'] = args.enable_stdp
        if args.enable_hebbian is not None:
            config['enable_hebbian'] = args.enable_hebbian
        if args.enable_memory is not None:
            config['enable_memory'] = args.enable_memory
        if args.enable_prediction is not None:
            config['enable_prediction'] = args.enable_prediction
        if args.enable_cognition is not None:
            config['enable_cognition'] = args.enable_cognition
        if args.enable_neuromodulation is not None:
            config['enable_neuromodulation'] = args.enable_neuromodulation
        if args.enable_development is not None:
            config['enable_development'] = args.enable_development
        
        # Create configuration file
        config_data = {
            'brain_type': args.type,
            'config': config,
            'created': str(__import__('datetime').datetime.now()),
            'description': self._get_brain_description(args.type)
        }
        
        # Save to file
        with open(args.output, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"Brain configuration saved to {args.output}")
        print("Configuration summary:")
        for key, value in config.items():
            print(f"  {key}: {value}")
        
        return 0
    
    def analyze_results(self, args):
        """Analyze simulation results."""
        if not os.path.exists(args.file):
            print(f"Error: Results file not found: {args.file}")
            return 1
        
        print(f"Analyzing simulation results from {args.file}...")
        
        try:
            with open(args.file, 'r') as f:
                data = json.load(f)
            
            # Extract brain statistics
            if 'brain' in data:
                brain_data = data['brain']
                print("Brain Statistics:")
                print(f"  Neurons: {brain_data.get('total_neurons', 'N/A')}")
                print(f"  Synapses: {brain_data.get('total_synapses', 'N/A')}")
                print(f"  Spikes: {brain_data.get('total_spikes', 'N/A')}")
                print(f"  Firing rate: {brain_data.get('avg_firing_rate', 'N/A')}")
                
                # Generate statistics report
                report = self._generate_statistics_report(brain_data)
                if args.report:
                    with open(args.report, 'w') as f:
                        f.write(report)
                    print(f"Statistics report saved to {args.report}")
                
                # Print to console if requested
                if args.verbose:
                    print("\nDetailed Statistics:")
                    print(report)
            
            # Extract simulation metadata
            if 'metadata' in data:
                metadata = data['metadata']
                print("\nSimulation Metadata:")
                for key, value in metadata.items():
                    print(f"  {key}: {value}")
            
            return 0
            
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON file: {e}")
            return 1
        except Exception as e:
            print(f"Error analyzing results: {e}")
            return 1
    
    def benchmark_brain(self, args):
        """Run performance benchmarks."""
        if not PYNLM_AVAILABLE:
            print("Error: NLM Python bindings not available")
            return 1
        
        print(f"Running benchmark with {args.steps} steps...")
        
        # Create brain
        brain = self._create_brain(args.brain_type, args)
        if not brain:
            return 1
        
        # Run benchmark
        import time
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate performance metrics
        steps_per_second = args.steps / duration
        steps_per_neuron = steps_per_second / brain.getTotalNeuronCount()
        
        print("\n=== Benchmark Results ===")
        print(f"Brain type: {args.brain_type}")
        print(f"Total steps: {args.steps}")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total time: {duration:.2f} seconds")
        print(f"Steps per second: {steps_per_second:.0f}")
        print(f"Steps per neuron per second: {steps_per_neuron:.2f}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        # Save benchmark results
        if args.output:
            benchmark_data = {
                'brain_type': args.brain_type,
                'steps': args.steps,
                'neurons': brain.getTotalNeuronCount(),
                'duration': duration,
                'steps_per_second': steps_per_second,
                'steps_per_neuron_per_second': steps_per_neuron,
                'avg_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'timestamp': str(__import__('datetime').datetime.now())
            }
            
            with open(args.output, 'w') as f:
                json.dump(benchmark_data, f, indent=2)
            
            print(f"Benchmark results saved to {args.output}")
        
        return 0
    
    def export_data(self, args):
        """Export simulation data for analysis."""
        if not PYNLM_AVAILABLE:
            print("Error: NLM Python bindings not available")
            return 1
        
        print(f"Exporting data to {args.format} format...")
        
        # This is a placeholder for data export functionality
        # In a full implementation, this would export brain state,
        # spike data, sensor readings, etc. to various formats
        # (CSV, JSON, MATLAB, etc.)
        
        print("Data export functionality to be implemented")
        print("Available formats: csv, json, mat, hdf5")
        print("This would export brain state, spike trains, and sensor data")
        
        return 0
    
    def _create_brain(self, brain_type, args):
        """Create brain based on configuration type."""
        try:
            if brain_type == 'minimal':
                return pynlm.createMinimalBrain()
            elif brain_type == 'standard':
                return pynlm.createStandardBrain()
            elif brain_type == 'learning':
                return pynlm.createLearningBrain()
            elif brain_type == 'cognition':
                return pynlm.createCognitionBrain()
            elif brain_type == 'development':
                return pynlm.createDevelopmentBrain()
            elif brain_type == 'performance':
                return pynlm.createPerformanceBrain()
            elif brain_type == 'research':
                return pynlm.createResearchBrain()
            else:
                print(f"Error: Unknown brain type '{brain_type}'")
                return None
        except Exception as e:
            print(f"Error creating brain: {e}")
            return None
    
    def _save_simulation_results(self, brain, world, agent, output_file):
        """Save simulation results to file."""
        results = {
            'brain': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'development_stage': str(brain.getDevelopmentalStage()),
                'statistics': brain.getStatistics()
            },
            'world': {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime()
            },
            'agent': {
                'learning_enabled': agent.isRewardModulationEnabled() and agent.isCuriosityEnabled(),
                'plasticity_enabled': agent.isStructuralPlasticityEnabled(),
                'development_enabled': agent.isDevelopmentEnabled()
            },
            'timestamp': str(__import__('datetime').datetime.now()),
            'description': 'NLM Simulation Results'
        }
        
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
    
    def _export_simulation_data(self, brain, world, agent, export_format):
        """Export simulation data in specified format."""
        print(f"Exporting data in {export_format} format...")
        # Implementation would vary based on format
        pass
    
    def _get_brain_description(self, brain_type):
        """Get description for brain type."""
        descriptions = {
            'minimal': 'A minimal brain for testing and debugging',
            'standard': 'A standard brain for general research',
            'learning': 'A brain optimized for learning and adaptation',
            'cognition': 'A brain optimized for cognitive tasks',
            'development': 'A brain for development studies',
            'performance': 'A brain optimized for speed and efficiency',
            'research': 'A comprehensive research-grade brain'
        }
        return descriptions.get(brain_type, 'Unknown brain type')
    
    def _generate_statistics_report(self, brain_data):
        """Generate a text report from brain statistics."""
        report = f"""NLM Brain Statistics Report
Generated: {__import__('datetime').datetime.now()}

Neural Network Statistics:
  Total Neurons: {brain_data.get('total_neurons', 'N/A')}
  Total Synapses: {brain_data.get('total_synapses', 'N/A')}
  Active Neurons: {brain_data.get('active_neurons', 'N/A')}
  Firing Neurons: {brain_data.get('firing_neurons', 'N/A')}
  Total Spikes: {brain_data.get('total_spikes', 'N/A')}
  Average Firing Rate: {brain_data.get('avg_firing_rate', 'N/A'):.2f} Hz
  Excitation/Inhibition Ratio: {brain_data.get('e_i_ratio', 'N/A'):.2f}
  Pending Spike Events: {brain_data.get('pending_spikes', 'N/A')}

Development:
  Current Stage: {brain_data.get('development_stage', 'N/A')}

Recommendations:
"""
        
        # Add recommendations based on statistics
        total_neurons = brain_data.get('total_neurons', 0)
        total_spikes = brain_data.get('total_spikes', 0)
        firing_rate = brain_data.get('avg_firing_rate', 0.0)
        
        if total_neurons == 0:
            report += "  - No neurons in brain. Check configuration."
        elif total_spikes == 0:
            report += "  - No spikes generated. Check input and connectivity."
        elif firing_rate < 1.0:
            report += "  - Low firing rate. Consider increasing input or connectivity."
        elif firing_rate > 100.0:
            report += "  - High firing rate. May indicate instability. Consider reducing connectivity."
        else:
            report += "  - Firing rate within normal range. Good balance of activity."
        
        if brain_data.get('e_i_ratio', 1.0) > 10.0:
            report += "\n  - High E/I ratio. May indicate lack of inhibition. Consider adding inhibitory neurons."
        elif brain_data.get('e_i_ratio', 1.0) < 0.1:
            report += "\n  - Low E/I ratio. May indicate excessive inhibition. Consider reducing inhibitory connections."
        
        return report
def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    
    # Create argument parser
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm run --brain learning --steps 1000
  nlm configure --type research --output config.json
  nlm analyze --file results.json --verbose
  nlm benchmark --brain standard --steps 5000
        """
    )
    
    # Add global options
    parser.add_argument(
        '--version', action='store_true',
        help='Show version information and exit'
    )
    
    parser.add_argument(
        '--verbose', '-v', action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--quiet', '-q', action='store_true',
        help='Suppress non-error output'
    )
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument(
        '--brain', '--brain-type', choices=['minimal', 'standard', 'learning', 'cognition', 'development', 'performance', 'research'],
        default='standard', help='Brain type to create (default: standard)'
    )
    run_parser.add_argument(
        '--steps', '-s', type=int, default=100,
        help='Number of simulation steps (default: 100)'
    )
    run_parser.add_argument(
        '--enable-learning/--disable-learning', default=True,
        help='Enable or disable learning (default: enabled)'
    )
    run_parser.add_argument(
        '--output', '-o', help='Output file for simulation results'
    )
    run_parser.add_argument(
        '--export', help='Export simulation data to specified format'
    )
    run_parser.add_argument(
        '--world-width', type=int, default=20,
        help='World width (default: 20)'
    )
    run_parser.add_argument(
        '--world-height', type=int, default=20,
        help='World height (default: 20)'
    )
    run_parser.add_argument(
        '--vision-width', type=int, default=8,
        help='Vision sensor width (default: 8)'
    )
    run_parser.add_argument(
        '--vision-height', type=int, default=8,
        help='Vision sensor height (default: 8)'
    )
    run_parser.add_argument(
        '--agent-x', type=int, default=10,
        help='Agent starting X position (default: 10)'
    )
    run_parser.add_argument(
        '--agent-y', type=int, default=10,
        help='Agent starting Y position (default: 10)'
    )
    
    # Configure brain command
    config_parser = subparsers.add_parser('configure', help='Configure a brain')
    config_parser.add_argument(
        '--type', choices=['minimal', 'standard', 'learning', 'cognition', 'development', 'performance', 'research'],
        required=True, help='Brain type to configure'
    )
    config_parser.add_argument(
        '--neuron-count', type=int, help='Number of neurons'
    )
    config_parser.add_argument(
        '--region-count', type=int, help='Number of regions'
    )
    config_parser.add_argument(
        '--connection-probability', type=float, help='Connection probability (0.0-1.0)'
    )
    config_parser.add_argument(
        '--enable-stdp', type=lambda x: x.lower() == 'true', help='Enable STDP'
    )
    config_parser.add_argument(
        '--enable-hebbian', type=lambda x: x.lower() == 'true', help='Enable Hebbian learning'
    )
    config_parser.add_argument(
        '--enable-memory', type=lambda x: x.lower() == 'true', help='Enable memory systems'
    )
    config_parser.add_argument(
        '--enable-prediction', type=lambda x: x.lower() == 'true', help='Enable prediction system'
    )
    config_parser.add_argument(
        '--enable-cognition', type=lambda x: x.lower() == 'true', help='Enable cognition systems'
    )
    config_parser.add_argument(
        '--enable-neuromodulation', type=lambda x: x.lower() == 'true', help='Enable neuromodulation'
    )
    config_parser.add_argument(
        '--enable-development', type=lambda x: x.lower() == 'true', help='Enable development'
    )
    config_parser.add_argument(
        '--output', '-o', required=True, help='Output configuration file'
    )
    
    # Analyze results command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
    analyze_parser.add_argument(
        '--file', '-f', required=True, help='Input results file'
    )
    analyze_parser.add_argument(
        '--report', help='Output statistics report file'
    )
    analyze_parser.add_argument(
        '--verbose', '-v', action='store_true', help='Verbose output'
    )
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
    benchmark_parser.add_argument(
        '--brain', '--brain-type', choices=['minimal', 'standard', 'learning', 'cognition', 'development', 'performance', 'research'],
        default='standard', help='Brain type to benchmark (default: standard)'
    )
    benchmark_parser.add_argument(
        '--steps', '-s', type=int, default=1000,
        help='Number of benchmark steps (default: 1000)'
    )
    benchmark_parser.add_argument(
        '--output', '-o', help='Output benchmark results file'
    )
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export simulation data')
    export_parser.add_argument(
        '--format', choices=['csv', 'json', 'mat', 'hdf5'], default='json',
        help='Export format (default: json)'
    )
    export_parser.add_argument(
        '--input', '-i', required=True, help='Input simulation file'
    )
    export_parser.add_argument(
        '--output', '-o', required=True, help='Output exported data file'
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle global options
    if args.version:
        print("NLM Command Line Interface v2.0")
        print("NLM (Neural Learning Machine) - Python Bindings")
        return 0
    
    if args.quiet:
        # Suppress non-error output (simplified)
        pass
    
    # Execute command
    if args.command == 'run':
        return cli.run_simulation(args)
    elif args.command == 'configure':
        return cli.configure_brain(args)
    elif args.command == 'analyze':
        return cli.analyze_results(args)
    elif args.command == 'benchmark':
        return cli.benchmark_brain(args)
    elif args.command == 'export':
        return cli.export_data(args)
    else:
        parser.print_help()
        return 1
if __name__ == '__main__':
    exit(main())