#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine CLI Tool

A comprehensive command-line interface for running, configuring, and analyzing
NLM brain simulations with both beginner-friendly and power-user features.
"""

import argparse
import json
import logging
import os
import sys
from pathlib import Path
from typing import Dict, List, Optional, Union
import time
import pandas as pd
import matplotlib.pyplot as plt

# Add the project root to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the Python bindings.")
    print("Run: pip install -e .")
    sys.exit(1)

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class NLMCLI:
    """Main CLI handler for NLM operations."""
    
    def __init__(self):
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.config_file = "configs/default.cfg"
        self.output_dir = "./nlm_output"
        
    def create_brain(self, config_file: Optional[str] = None, 
                     neuron_count: int = 1000, region_count: int = 2,
                     seed: int = 42) -> bool:
        """Create and initialize an NLM brain."""
        try:
            logger.info("Creating NLM brain...")
            
            # Load configuration
            config = pynlm.createDefaultConfig()
            
            # Apply configuration overrides
            if config_file:
                config.loadFromFile(config_file)
                
            # Set key parameters
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.region_count", region_count)
            config.set("random_seed", seed)
            
            # Create and initialize brain
            self.current_brain = pynlm.createBrain(config)
            self.current_brain.initialize()
            
            logger.info(f"Brain created successfully: {neuron_count} neurons, {region_count} regions")
            logger.info(f"Total spikes: {self.current_brain.getTotalSpikeCount()}")
            logger.info(f"Firing neurons: {self.current_brain.getFiringNeuronCount()}")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to create brain: {e}")
            return False
    
    def run_simulation(self, steps: int, timestep: float = 0.001,
                      enable_features: Dict[str, bool] = None) -> Dict:
        """Run a simulation for the specified number of steps."""
        if not self.current_brain:
            logger.error("No brain initialized. Use --create-brain first.")
            return {}
        
        try:
            logger.info(f"Running simulation for {steps} steps with timestep {timestep}")
            
            # Default features
            if enable_features is None:
                enable_features = {
                    'reward_modulation': True,
                    'structural_plasticity': True,
                    'development': True,
                    'curiosity': True
                }
            
            # Create world and agent if not exists
            if self.current_world is None:
                self.current_world = pynlm.createSimpleWorld()
                self.current_world.configure(20, 20, 8, 8)
                self.current_world.reset()
                self.current_world.setAgentStart(10.0, 10.0)
            
            if self.current_agent is None and self.current_world:
                self.current_agent = pynlm.createAgentBrain(self.current_brain)
                self.current_agent.initialize(self.current_world)
                
                # Enable requested features
                if enable_features.get('reward_modulation'):
                    self.current_agent.enableRewardModulation(True)
                if enable_features.get('structural_plasticity'):
                    self.current_agent.enableStructuralPlasticity(True)
                if enable_features.get('development'):
                    self.current_agent.enableDevelopment(True)
                if enable_features.get('curiosity'):
                    self.current_agent.enableCuriosity(True)
            
            # Simulation statistics
            stats = {
                'steps_completed': 0,
                'total_spikes': self.current_brain.getTotalSpikeCount(),
                'firing_neurons': self.current_brain.getFiringNeuronCount(),
                'avg_firing_rate': self.current_brain.getAverageFiringRate(),
                'reward_modulation': self.current_agent.getNeuromodulationLevel() if self.current_agent else 0.0,
                'curiosity': self.current_agent.getCuriosityLevel() if self.current_agent else 0.0,
                'novelty': self.current_agent.getNoveltyLevel() if self.current_agent else 0.0,
                'max_memory_episodes': 0
            }
            
            # Run simulation loop
            for step in range(steps):
                # Update world
                if self.current_world:
                    self.current_world.update(timestep)
                
                # Get sensory input
                if self.current_world:
                    percept = self.current_world.getSensoryPercept()
                    
                    # Process sensory input
                    if self.current_agent:
                        self.current_agent.processSensoryInput(percept)
                    
                    # Inject into brain
                    if self.current_agent:
                        self.current_agent.getBrain().receiveSensoryInput(percept)
                
                # Step brain
                self.current_brain.step(step)
                
                # Get action from agent
                action = None
                if self.current_agent:
                    action = self.current_agent.decodeMotorCommand()
                elif self.current_brain:
                    action = self.current_brain.produceAction()
                
                # Apply action to world
                if action and self.current_world:
                    self.current_world.applyMotorCommand(action, step * timestep)
                
                # Log progress periodically
                if step % 100 == 0:
                    logger.info(f"Step {step}/{steps}: "
                              f"Firing: {self.current_brain.getFiringNeuronCount()}, "
                              f"Spikes: {self.current_brain.getTotalSpikeCount()}")
                
                # Update statistics
                stats.update({
                    'steps_completed': step + 1,
                    'total_spikes': self.current_brain.getTotalSpikeCount(),
                    'firing_neurons': self.current_brain.getFiringNeuronCount(),
                    'avg_firing_rate': self.current_brain.getAverageFiringRate(),
                    'reward_modulation': self.current_agent.getNeuromodulationLevel() if self.current_agent else 0.0,
                    'curiosity': self.current_agent.getCuriosityLevel() if self.current_agent else 0.0,
                    'novelty': self.current_agent.getNoveltyLevel() if self.current_agent else 0.0,
                })
            
            logger.info(f"Simulation completed: {steps} steps")
            return stats
            
        except Exception as e:
            logger.error(f"Simulation error: {e}")
            return {}
    
    def analyze_results(self, stats: Dict, output_file: str = None) -> bool:
        """Analyze simulation results and generate reports."""
        if not stats:
            logger.error("No simulation statistics to analyze")
            return False
        
        try:
            logger.info("Analyzing simulation results...")
            
            # Create analysis directory
            os.makedirs(self.output_dir, exist_ok=True)
            
            # Generate CSV report
            report_file = f"{self.output_dir}/simulation_report.csv"
            
            # Create DataFrame from stats
            df = pd.DataFrame([stats])
            
            # Save CSV
            df.to_csv(report_file, index=False)
            logger.info(f"Report saved to {report_file}")
            
            # Generate JSON summary
            if output_file is None:
                output_file = f"{self.output_dir}/simulation_summary.json"
            
            with open(output_file, 'w') as f:
                json.dump(stats, f, indent=2)
            logger.info(f"JSON summary saved to {output_file}")
            
            # Generate simple text summary
            summary_file = f"{self.output_dir}/simulation_summary.txt"
            with open(summary_file, 'w') as f:
                f.write("=== NLM Simulation Summary ===\n\n")
                f.write(f"Steps Completed: {stats.get('steps_completed', 'N/A')}\n")
                f.write(f"Total Spikes: {stats.get('total_spikes', 'N/A')}\n")
                f.write(f"Firing Neurons: {stats.get('firing_neurons', 'N/A')}\n")
                f.write(f"Average Firing Rate: {stats.get('avg_firing_rate', 'N/A'):.2f} Hz\n")
                f.write(f"Neuromodulation Level: {stats.get('reward_modulation', 'N/A'):.3f}\n")
                f.write(f"Curiosity Level: {stats.get('curiosity', 'N/A'):.3f}\n")
                f.write(f"Novelty Level: {stats.get('novelty', 'N/A'):.3f}\n")
                
            logger.info(f"Text summary saved to {summary_file}")
            
            return True
            
        except Exception as e:
            logger.error(f"Analysis error: {e}")
            return False
    
    def create_experiment(self, name: str, config_overrides: Dict = None) -> bool:
        """Create a new experiment configuration."""
        try:
            logger.info(f"Creating experiment: {name}")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply overrides if provided
            if config_overrides:
                for key, value in config_overrides.items():
                    config.set(key, value)
            
            # Save configuration
            os.makedirs(self.output_dir, exist_ok=True)
            config_file = f"{self.output_dir}/experiment_{name}_config.json"
            config.saveToFile(config_file)
            
            logger.info(f"Experiment configuration saved to {config_file}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to create experiment: {e}")
            return False
    
    def load_experiment(self, name: str) -> bool:
        """Load a saved experiment configuration."""
        try:
            config_file = f"{self.output_dir}/experiment_{name}_config.json"
            
            if not os.path.exists(config_file):
                logger.error(f"Experiment configuration not found: {config_file}")
                return False
            
            config = pynlm.createDefaultConfig()
            config.loadFromFile(config_file)
            
            logger.info(f"Loaded experiment configuration from {config_file}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to load experiment: {e}")
            return False
    
    def export_results(self, format: str = 'csv') -> bool:
        """Export simulation results to file."""
        if not self.current_brain:
            logger.error("No brain available for export")
            return False
        
        try:
            logger.info(f"Exporting results in {format} format...")
            
            os.makedirs(self.output_dir, exist_ok=True)
            
            if format.lower() == 'csv':
                # Generate comprehensive CSV
                csv_file = f"{self.output_dir}/results.csv"
                
                # Create data for CSV export
                import time
                timestamp = time.strftime("%Y%m%d_%H%M%S")
                
                with open(csv_file, 'w') as f:
                    f.write("Step,Time,NeuronCount,SynapseCount,FiringCount,AvgFiringRate,TotalSpikes\n")
                    
                    for step in range(0, min(1000, 100)):  # Sample first 100 steps
                        f.write(f"{step},{step*0.001},{self.current_brain.getTotalNeuronCount()},"
                               f"{self.current_brain.getTotalSynapseCount()},"
                               f"{self.current_brain.getFiringNeuronCount()},"
                               f"{self.current_brain.getAverageFiringRate()}"
                               f"{self.current_brain.getTotalSpikeCount()}\n")
                
                logger.info(f"CSV results saved to {csv_file}")
                
            elif format.lower() == 'json':
                # Generate JSON results
                json_file = f"{self.output_dir}/results.json"
                
                results = {
                    'metadata': {
                        'timestamp': time.strftime("%Y-%m-%d %H:%M:%S"),
                        'brain_type': 'NLM - Neural Learning Machine',
                        'version': 'Phase 6.0'
                    },
                    'statistics': {
                        'neuron_count': self.current_brain.getTotalNeuronCount(),
                        'synapse_count': self.current_brain.getTotalSynapseCount(),
                        'firing_count': self.current_brain.getFiringNeuronCount(),
                        'total_spikes': self.current_brain.getTotalSpikeCount(),
                        'avg_firing_rate': self.current_brain.getAverageFiringRate()
                    }
                }
                
                with open(json_file, 'w') as f:
                    json.dump(results, f, indent=2)
                
                logger.info(f"JSON results saved to {json_file}")
                
            else:
                logger.error(f"Unsupported export format: {format}")
                return False
            
            return True
            
        except Exception as e:
            logger.error(f"Export error: {e}")
            return False
    
    def help(self, command: str = None):
        """Show help information."""
        help_text = {
            'general': """
NLM CLI - Neural Learning Machine Command Line Interface

USAGE:
    nlm-cli <command> [options]

COMMANDS:
    create-brain      Create and initialize a new brain
    run-simulation    Run a simulation with current brain
    analyze-results   Analyze and report simulation results
    create-experiment Create a new experiment configuration
    load-experiment   Load saved experiment configuration
    export-results    Export results to file
    help             Show help information
    exit             Exit the program

OPTIONS:
    --help           Show this help message
    --version        Show version information
    --verbose        Enable verbose logging
    --quiet          Suppress output
""",
            'create-brain': """
Create a new NLM brain:

USAGE:
    nlm-cli create-brain [--config FILE] [--neurons N] [--regions R] [--seed S]

OPTIONS:
    --config FILE     Configuration file to load (default: configs/default.cfg)
    --neurons N       Number of neurons (default: 1000)
    --regions R       Number of brain regions (default: 2)
    --seed S          Random seed for reproducible results (default: 42)
""",
            'run-simulation': """
Run a simulation:

USAGE:
    nlm-cli run-simulation <steps> [--timestep T] [--no-features]

OPTIONS:
    <steps>           Number of simulation steps to run
    --timestep T      Simulation timestep in seconds (default: 0.001)
    --no-features     Disable all advanced features (faster execution)
    --features FEATURES  Comma-separated list of features to enable (reward,curiosity,development,plasticity)
""",
            'analyze-results': """
Analyze simulation results:

USAGE:
    nlm-cli analyze-results [--output FILE]

OPTIONS:
    --output FILE      Output file for analysis report (default: ./nlm_output/simulation_summary.json)
"""
        }
        
        if command is None:
            print(help_text['general'])
        elif command in help_text:
            print(help_text[command])
        else:
            print(f"Unknown command: {command}")


def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    
    # Create argument parser
    parser = argparse.ArgumentParser(
        description="NLM (熙然) - Neural Learning Machine CLI Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Add commands as subparsers
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    create_parser = subparsers.add_parser('create-brain', help='Create and initialize a brain')
    create_parser.add_argument('--config', help='Configuration file to load')
    create_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    create_parser.add_argument('--regions', type=int, default=2, help='Number of brain regions')
    create_parser.add_argument('--seed', type=int, default=42, help='Random seed')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run-simulation', help='Run a simulation')
    run_parser.add_argument('steps', type=int, help='Number of simulation steps')
    run_parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    run_parser.add_argument('--no-features', action='store_true', help='Disable all features')
    run_parser.add_argument('--features', help='Comma-separated list of features to enable')
    
    # Analyze results command
    analyze_parser = subparsers.add_parser('analyze-results', help='Analyze simulation results')
    analyze_parser.add_argument('--output', help='Output file for analysis report')
    
    # Create experiment command
    exp_parser = subparsers.add_parser('create-experiment', help='Create experiment configuration')
    exp_parser.add_argument('name', help='Experiment name')
    exp_parser.add_argument('--config', help='JSON config file with overrides')
    
    # Load experiment command
    load_parser = subparsers.add_parser('load-experiment', help='Load experiment configuration')
    load_parser.add_argument('name', help='Experiment name')
    
    # Export results command
    export_parser = subparsers.add_parser('export-results', help='Export results to file')
    export_parser.add_argument('--format', choices=['csv', 'json'], default='csv', help='Export format')
    
    # Help command
    help_parser = subparsers.add_parser('help', help='Show help information')
    help_parser.add_argument('command', nargs='?', help='Specific command to show help for')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute commands
    if args.command == 'create-brain':
        success = cli.create_brain(args.config, args.neurons, args.regions, args.seed)
        sys.exit(0 if success else 1)
        
    elif args.command == 'run-simulation':
        features = {}
        if not args.no_features:
            if args.features:
                for feature in args.features.split(','):
                    features[feature.strip()] = True
            else:
                # Default features
                features = {'reward_modulation': True, 'curiosity': True, 'development': True, 'structural_plasticity': True}
        
        stats = cli.run_simulation(args.steps, args.timestep, features if features else None)
        if stats:
            cli.analyze_results(stats)
        sys.exit(0 if stats else 1)
        
    elif args.command == 'analyze-results':
        success = cli.analyze_results({}, args.output)
        sys.exit(0 if success else 1)
        
    elif args.command == 'create-experiment':
        config_overrides = {}
        if args.config:
            try:
                with open(args.config, 'r') as f:
                    config_overrides = json.load(f)
            except Exception as e:
                print(f"Error loading config file: {e}")
                sys.exit(1)
        
        success = cli.create_experiment(args.name, config_overrides)
        sys.exit(0 if success else 1)
        
    elif args.command == 'load-experiment':
        success = cli.load_experiment(args.name)
        sys.exit(0 if success else 1)
        
    elif args.command == 'export-results':
        success = cli.export_results(args.format)
        sys.exit(0 if success else 1)
        
    elif args.command == 'help':
        cli.help(args.command)
        sys.exit(0)
        
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == '__main__':
    main()
