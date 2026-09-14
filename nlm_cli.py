#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive CLI for NLM (Neural Learning Machine) users.
Provides advanced functionality for brain simulation, analysis, and management.

Usage: nlm-cli [OPTIONS] COMMAND [ARGS]...
"""

import argparse
import sys
import logging
import json
import os
from pathlib import Path
from typing import Optional, List, Dict, Any
import time
import argparse
from datetime import datetime
from enum import Enum

# Try to import optional dependencies
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm package not available. Some commands will not work.", file=sys.stderr)

try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False

try:
    from tqdm import tqdm
    TQDM_AVAILABLE = True
except ImportError:
    TQDM_AVAILABLE = False

# Set up logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_cli.log'),
        logging.StreamHandler(sys.stderr)
    ]
)

logger = logging.getLogger(__name__)

class ExperimentType(Enum):
    STANDARD = "standard"
    CURIOSITY = "curiosity"
    NOVELTY = "novelty"
    REWARD = "reward"
    DEVELOPMENT = "development"

class NLMCLI:
    """Main CLI class for NLM operations."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.world = None
        self.agent = None
        self.setup_default_config()
    
    def setup_default_config(self):
        """Setup default NLM configuration."""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = None
    
    def create_brain(self, neuron_count: int = 1000, config_file: str = None, 
                     enable_learning: bool = True):
        """Create a brain with the given configuration."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        if config_file and os.path.exists(config_file):
            self.config = pynlm.createDefaultConfig()
            self.config.loadFromFile(config_file)
        else:
            if self.config is None:
                self.setup_default_config()
            self.config.set("brain.neuron_count", neuron_count)
            self.config.set("plasticity.stdp.enable", enable_learning)
            self.config.set("plasticity.hebbian.enable", enable_learning)
            self.config.set("neuromod.dopamine.scale", 1.0 if enable_learning else 0.0)
            self.config.set("neuromod.curiosity.enable", enable_learning)
            self.config.set("neuromod.novelty.enable", enable_learning)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        logger.info(f"Brain created with {self.brain.getTotalNeuronCount()} neurons")
        return self.brain
    
    def create_agent(self, world, brain=None):
        """Create an agent with the brain and world."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        if brain is not None:
            self.brain = brain
        
        if self.brain is None:
            raise RuntimeError("Brain not created. Call create_brain() first.")
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(world)
        logger.info("Agent created and initialized")
        return self.agent
    
    def create_simple_world(self, width: int = 20, height: int = 20, 
                           vision_width: int = 8, vision_height: int = 8):
        """Create a simple world."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, 
                           visionWidth=vision_width, visionHeight=vision_height)
        logger.info(f"Simple world created with dimensions {width}x{height}")
        return self.world
    
    def run_simulation(self, num_steps: int, interactive: bool = False, 
                      enable_learning: bool = True, progress_bar: bool = False):
        """Run a simulation."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        # Create components if not already created
        if self.brain is None:
            self.create_brain(enable_learning=enable_learning)
        
        if self.world is None:
            self.create_simple_world()
        
        if self.agent is None:
            self.create_agent(self.world)
        
        logger.info(f"Starting simulation for {num_steps} steps")
        
        if interactive:
            self._run_interactive_simulation(num_steps, progress_bar)
        else:
            self._run_batch_simulation(num_steps, progress_bar)
        
        logger.info("Simulation completed")
    
    def _run_batch_simulation(self, num_steps: int, progress_bar: bool = False):
        """Run simulation in batch mode."""
        iterator = range(num_steps)
        if progress_bar and TQDM_AVAILABLE:
            iterator = tqdm(iterator, desc="Simulation Progress", unit="steps")
        elif progress_bar:
            print("Note: Install 'tqdm' for progress bar support")
        
        for step in iterator:
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept and hasattr(percept, 'getInternal'):
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                logger.info(f"Step {step}: Firing neurons: {self.brain.getFiringNeuronCount()}")
    
    def _run_interactive_simulation(self, num_steps: int, progress_bar: bool = False):
        """Run simulation with periodic status output."""
        iterator = range(num_steps)
        if progress_bar and TQDM_AVAILABLE:
            iterator = tqdm(iterator, desc="Interactive Simulation", unit="steps")
        
        for step in iterator:
            try:
                self.world.update(0.1)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                reward = 0.0
                if percept and hasattr(percept, 'getInternal'):
                    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                self.agent.updateDevelopment(0.1)
                
                if step % 50 == 0:
                    print(f"\nStep {step}:")
                    print(f"  Neurons firing: {self.brain.getFiringNeuronCount()}")
                    if hasattr(self.agent, 'getCuriosityLevel'):
                        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
                    if hasattr(self.agent, 'getNoveltyLevel'):
                        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
                    print(f"  Dev Stage: {self.brain.getDevelopmentalStage()}")
                    print(f"  E/I Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
                    print("  (Press Ctrl+C to stop early)")
            except KeyboardInterrupt:
                print("\nSimulation interrupted by user")
                break
    
    def save_checkpoint(self, filename: str, compress: bool = False):
        """Save brain state to a checkpoint file."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        if self.brain is None:
            raise RuntimeError("Brain not created. Call create_brain() first.")
        
        self.brain.save(filename)
        logger.info(f"Checkpoint saved to {filename}")
    
    def load_checkpoint(self, filename: str):
        """Load brain state from a checkpoint file."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        if self.brain is None:
            raise RuntimeError("Brain not created. Call create_brain() first.")
        
        self.brain.load(filename)
        logger.info(f"Checkpoint loaded from {filename}")
    
    def analyze_brain(self, output_file: str = None, verbose: bool = True) -> Dict[str, Any]:
        """Analyze brain state and return statistics."""
        if not PYNLM_AVAILABLE:
            raise RuntimeError("pynlm package not available")
        
        if self.brain is None:
            raise RuntimeError("Brain not created. Call create_brain() first.")
        
        stats = {
            'timestamp': datetime.now().isoformat(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'config_keys': list(self.config.getKeys()) if self.config else None
        }
        
        # Add agent-specific stats if available
        if self.agent:
            agent_stats = {}
            if hasattr(self.agent, 'getCuriosityLevel'):
                agent_stats['curiosity'] = self.agent.getCuriosityLevel()
            if hasattr(self.agent, 'getNoveltyLevel'):
                agent_stats['novelty'] = self.agent.getNoveltyLevel()
            if hasattr(self.agent, 'getPredictionError'):
                agent_stats['prediction_error'] = self.agent.getPredictionError()
            if hasattr(self.agent, 'getNeuromodulationLevel'):
                agent_stats['neuromodulation_level'] = self.agent.getNeuromodulationLevel()
            stats['agent_stats'] = agent_stats
        
        if verbose:
            logger.info(f"Brain analysis completed. Stats: {stats}")
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(stats, f, indent=2)
            logger.info(f"Analysis saved to {output_file}")
        
        return stats
    
    def run_batch_simulation(self, num_steps: int, count: int = 1, 
                           enable_learning: bool = True, progress_bar: bool = False):
        """Run batch simulation (multiple simulations)."""
        logger.info(f"Running batch simulation: {count} runs of {num_steps} steps each")
        
        results = []
        for i in range(count):
            logger.info(f"Batch simulation {i+1}/{count}")
            self.run_simulation(num_steps, interactive=False, 
                              enable_learning=enable_learning, progress_bar=progress_bar)
            
            # Collect results from each simulation
            stats = self.analyze_brain(verbose=False)
            stats['batch_run'] = i + 1
            results.append(stats)
        
        # Analyze batch results
        batch_stats = self.analyze_batch_results(results)
        
        logger.info(f"Batch simulation completed. Average stats: {batch_stats}")
        return results
    
    def analyze_batch_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze results from multiple batch simulations."""
        if not NUMPY_AVAILABLE:
            return {'error': 'NumPy not available for batch analysis'}
        
        avg_firing_rate = np.mean([r['average_firing_rate'] for r in results])
        avg_neurons = np.mean([r['total_neurons'] for r in results])
        avg_spikes = np.mean([r['total_spikes'] for r in results])
        
        return {
            'runs_completed': len(results),
            'average_firing_rate': avg_firing_rate,
            'average_total_neurons': avg_neurons,
            'average_total_spikes': avg_spikes,
            'firing_rate_std': np.std([r['average_firing_rate'] for r in results]),
            'min_firing_rate': np.min([r['average_firing_rate'] for r in results]),
            'max_firing_rate': np.max([r['average_firing_rate'] for r in results])
        }
    
    def run_learning_experiment(self, num_steps: int, experiment_type: str = "standard",
                               enable_all_subsystems: bool = True, progress_bar: bool = False):
        """Run a learning experiment."""
        logger.info(f"Starting learning experiment of type '{experiment_type}' for {num_steps} steps")
        
        # Configure experiment-specific settings
        if experiment_type == ExperimentType.CURIOSITY.value:
            self.config.set("neuromod.curiosity.enable", True)
            self.config.set("neuromod.novelty.enable", True)
            self.config.set("neuromod.dopamine.scale", 0.5)
        elif experiment_type == ExperimentType.REWARD.value:
            self.config.set("neuromod.dopamine.scale", 2.0)
            self.config.set("neuromod.curiosity.enable", False)
        elif experiment_type == ExperimentType.DEVOLUTION.value:
            self.config.set("development.initial_stage", 0.1)
        elif experiment_type == ExperimentType.NOVELTY.value:
            self.config.set("neuromod.novelty.enable", True)
            self.config.set("neuromod.curiosity.enable", True)
        
        # Recreate brain with experiment settings
        if self.brain is not None:
            self.brain = None
        
        self.run_simulation(num_steps, interactive=False, 
                          enable_learning=enable_all_subsystems, progress_bar=progress_bar)
        logger.info(f"Learning experiment '{experiment_type}' completed")
    
    def visualize_brain(self, output_file: str = None, plot_type: str = "spike_raster"):
        """Create brain visualizations."""
        if not MATPLOTLIB_AVAILABLE:
            raise RuntimeError("matplotlib not available for visualization")
        
        if self.brain is None:
            raise RuntimeError("Brain not created. Call create_brain() first.")
        
        logger.info(f"Creating brain visualization: {plot_type}")
        
        if plot_type == "spike_raster":
            self._create_spike_raster_plot(output_file)
        elif plot_type == "firing_rate":
            self._create_firing_rate_plot(output_file)
        elif plot_type == "network":
            self._create_network_plot(output_file)
        elif plot_type == "development":
            self._create_development_plot(output_file)
        else:
            raise ValueError(f"Unknown plot type: {plot_type}")
        
        logger.info(f"Brain visualization saved to {output_file}")
    
    def _create_spike_raster_plot(self, output_file: str = None):
        """Create spike raster plot."""
        if self.brain is None:
            raise RuntimeError("Brain not available for visualization")
        
        # This would require access to spike data which might not be directly available
        # For now, create a placeholder plot
        plt.figure(figsize=(10, 6))
        plt.title("Spike Raster Plot (Placeholder)")
        plt.xlabel("Time (steps)")
        plt.ylabel("Neuron ID")
        plt.grid(True, alpha=0.3)
        plt.savefig(output_file or 'spike_raster.png', dpi=150, bbox_inches='tight')
        plt.close()
    
    def _create_firing_rate_plot(self, output_file: str = None):
        """Create firing rate plot."""
        plt.figure(figsize=(10, 6))
        plt.title("Average Firing Rate Over Time")
        plt.xlabel("Time (steps)")
        plt.ylabel("Firing Rate (Hz)")
        plt.grid(True, alpha=0.3)
        plt.savefig(output_file or 'firing_rate.png', dpi=150, bbox_inches='tight')
        plt.close()
    
    def _create_network_plot(self, output_file: str = None):
        """Create network connectivity plot."""
        plt.figure(figsize=(12, 8))
        plt.title("Neural Network Connectivity (Placeholder)")
        plt.xlabel("Source Neuron")
        plt.ylabel("Target Neuron")
        plt.grid(True, alpha=0.3)
        plt.savefig(output_file or 'network_connectivity.png', dpi=150, bbox_inches='tight')
        plt.close()
    
    def _create_development_plot(self, output_file: str = None):
        """Create development stage plot."""
        plt.figure(figsize=(10, 6))
        plt.title("Developmental Stage Progression")
        plt.xlabel("Time (steps)")
        plt.ylabel("Development Stage")
        plt.grid(True, alpha=0.3)
        plt.savefig(output_file or 'development_stage.png', dpi=150, bbox_inches='tight')
        plt.close()
    
    def run_checkpoint_processing(self, pattern: str, output_dir: str = None):
        """Process multiple checkpoints."""
        logger.info(f"Processing checkpoints matching pattern: {pattern}")
        
        import glob
        checkpoint_files = glob.glob(pattern)
        
        if not checkpoint_files:
            logger.warning(f"No files found matching pattern: {pattern}")
            return
        
        results = []
        for checkpoint_file in checkpoint_files:
            logger.info(f"Processing checkpoint: {checkpoint_file}")
            try:
                # Create new brain instance for each checkpoint
                temp_cli = NLMCLI()
                temp_cli.load_checkpoint(checkpoint_file)
                stats = temp_cli.analyze_brain(verbose=False)
                stats['checkpoint_file'] = checkpoint_file
                results.append(stats)
            except Exception as e:
                logger.error(f"Failed to process {checkpoint_file}: {e}")
        
        # Save processing results
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
            output_file = os.path.join(output_dir, 'checkpoint_processing_results.json')
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2)
            logger.info(f"Processing results saved to {output_file}")
        
        return results


def create_parser():
    """Create argument parser for the CLI."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Advanced brain simulation tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli create --neurons 1000                    # Create a brain with 1000 neurons
  nlm-cli simulate --steps 100 --interactive       # Run interactive simulation
  nlm-cli analyze --output brain_stats.json         # Analyze current brain state
  nlm-cli save --file brain.pkl                    # Save brain state to file
  nlm-cli load --file brain.pkl                    # Load brain state from file
  nlm-cli batch --steps 1000 --count 5              # Run 5 batch simulations
  nlm-cli experiment --steps 500 --type curiosity   # Run curiosity experiment

Run 'nlm-cli COMMAND --help' for more information about a specific command.
        """
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--quiet', '-q',
        action='store_true',
        help='Suppress non-essential output'
    )
    
    parser.add_argument(
        '--config', '-c',
        type=str,
        help='Path to configuration file'
    )
    
    parser.add_argument(
        '--progress', '-p',
        action='store_true',
        help='Show progress bar for long operations'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    create_parser = subparsers.add_parser(
        'create', 
        help='Create a new brain or agent setup',
        description='Create a brain and optionally set up agent and world'
    )
    create_parser.add_argument(
        '--neurons', '-n',
        type=int,
        default=1000,
        help='Number of neurons for the brain (default: 1000)'
    )
    create_parser.add_argument(
        '--world-width', '-w',
        type=int,
        default=20,
        help='Width of the simple world (default: 20)'
    )
    create_parser.add_argument(
        '--world-height', '-h',
        type=int,
        default=20,
        help='Height of the simple world (default: 20)'
    )
    create_parser.add_argument(
        '--world-vision-width', '--vw',
        type=int,
        default=8,
        help='Vision width of the world (default: 8)'
    )
    create_parser.add_argument(
        '--world-vision-height', '--vh',
        type=int,
        default=8,
        help='Vision height of the world (default: 8)'
    )
    create_parser.add_argument(
        '--save', '-s',
        type=str,
        help='Save the brain configuration to a file'
    )
    create_parser.add_argument(
        '--enable-learning',
        action='store_true',
        default=True,
        help='Enable learning subsystems (default: True)'
    )
    
    # Simulate command
    simulate_parser = subparsers.add_parser(
        'simulate',
        help='Run a simulation',
        description='Run NLM simulation with or without world interaction'
    )
    simulate_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=100,
        help='Number of simulation steps (default: 100)'
    )
    simulate_parser.add_argument(
        '--interactive', '-i',
        action='store_true',
        help='Run in interactive mode with periodic status updates'
    )
    simulate_parser.add_argument(
        '--no-world',
        action='store_true',
        help='Run without a world (brain-only simulation)'
    )
    simulate_parser.add_argument(
        '--config-file',
        type=str,
        help='Path to brain configuration file'
    )
    simulate_parser.add_argument(
        '--enable-learning',
        action='store_true',
        default=True,
        help='Enable learning subsystems (default: True)'
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser(
        'analyze',
        help='Analyze brain state and performance',
        description='Analyze current brain state, metrics, and configuration'
    )
    analyze_parser.add_argument(
        '--output', '-o',
        type=str,
        help='Output file for analysis results (JSON format)'
    )
    analyze_parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Verbose analysis output'
    )
    
    # Save command
    save_parser = subparsers.add_parser(
        'save',
        help='Save brain state to a file',
        description='Save the current brain state to a checkpoint file'
    )
    save_parser.add_argument(
        '--file', '-f',
        required=True,
        type=str,
        help='Path to the checkpoint file'
    )
    save_parser.add_argument(
        '--compress',
        action='store_true',
        help='Compress checkpoint file'
    )
    
    # Load command
    load_parser = subparsers.add_parser(
        'load',
        help='Load brain state from a file',
        description='Load brain state from a checkpoint file'
    )
    load_parser.add_argument(
        '--file', '-f',
        required=True,
        type=str,
        help='Path to the checkpoint file'
    )
    
    # Batch command
    batch_parser = subparsers.add_parser(
        'batch',
        help='Run batch simulation',
        description='Run multiple simulations with different configurations'
    )
    batch_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of steps per simulation (default: 1000)'
    )
    batch_parser.add_argument(
        '--count', '-c',
        type=int,
        default=5,
        help='Number of simulations to run (default: 5)'
    )
    batch_parser.add_argument(
        '--config-dir',
        type=str,
        help='Directory containing configuration files'
    )
    batch_parser.add_argument(
        '--enable-learning',
        action='store_true',
        default=True,
        help='Enable learning subsystems (default: True)'
    )
    
    # Experiment command
    experiment_parser = subparsers.add_parser(
        'experiment',
        help='Run learning experiments',
        description='Run controlled learning experiments with specific configurations'
    )
    experiment_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of steps for the experiment (default: 1000)'
    )
    experiment_parser.add_argument(
        '--type', '--exp-type',
        type=str,
        choices=['standard', 'curiosity', 'novelty', 'reward', 'development'],
        default='standard',
        help='Type of experiment (default: standard)'
    )
    experiment_parser.add_argument(
        '--enable-all-subsystems',
        action='store_true',
        default=True,
        help='Enable all learning subsystems (default: True)'
    )
    
    # Process command
    process_parser = subparsers.add_parser(
        'process',
        help='Process checkpoints and files',
        description='Process and analyze multiple checkpoint files'
    )
    process_parser.add_argument(
        '--pattern', '-p',
        type=str,
        required=True,
        help='Pattern for checkpoint files (e.g., "*.ckpt\")'
    )
    process_parser.add_argument(
        '--output-dir', '-o',
        type=str,
        help='Output directory for processed results'
    )
    
    # List command
    list_parser = subparsers.add_parser(
        'list',
        help='List available brains, checkpoints, or resources',
        description='List available saved brains or checkpoints'
    )
    list_parser.add_argument(
        '--checkpoints', '-c',
        action='store_true',
        help='List available checkpoint files'
    )
    list_parser.add_argument(
        '--directory', '-d',
        type=str,
        default='.',
        help='Directory to search for files (default: current directory)'
    )
    
    # Help command
    help_parser = subparsers.add_parser(
        'help',
        help='Show help for a specific command',
        description='Show detailed help for a specific NLM command'
    )
    help_parser.add_argument(
        'command_name',
        type=str,
        help='Command name to show help for'
    )
    
    return parser


def main():
    """Main entry point for the CLI."""
    parser = create_parser()
    
    # Handle special case for help
    if len(sys.argv) == 1 or (len(sys.argv) == 2 and sys.argv[1] == 'help'):
        parser.print_help()
        return
    
    # Parse arguments
    args = parser.parse_args()
    
    # Initialize CLI
    cli = NLMCLI()
    
    # Configure logging based on verbosity
    if args.verbose:
        logger.setLevel(logging.DEBUG)
    elif args.quiet:
        logger.setLevel(logging.WARNING)
    
    # Execute commands
    try:
        if args.command == 'create':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the create command", file=sys.stderr)
                sys.exit(1)
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", args.neurons)
            
            cli.create_brain(args.neurons, enable_learning=args.enable_learning)
            
            # Create world if not specified to skip
            if not args.no_world:
                cli.create_simple_world(args.world_width, args.world_height, 
                                      args.world_vision_width, args.world_vision_height)
                cli.create_agent(cli.world)
                
                # Enable learning subsystems
                if args.enable_learning:
                    cli.agent.enableRewardModulation(True)
                    cli.agent.enableStructuralPlasticity(True)
                    cli.agent.enableDevelopment(True)
                    cli.agent.enableCuriosity(True)
            
            if args.save:
                cli.save_checkpoint(args.save)
                print(f"✓ Brain created with {args.neurons} neurons and saved to {args.save}")
                
        elif args.command == 'simulate':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the simulate command", file=sys.stderr)
                sys.exit(1)
            
            cli.run_simulation(
                args.steps, 
                interactive=args.interactive,
                enable_learning=args.enable_learning,
                progress_bar=args.progress
            )
                
        elif args.command == 'analyze':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the analyze command", file=sys.stderr)
                sys.exit(1)
            
            stats = cli.analyze_brain(args.output, verbose=args.verbose)
            print(f"✓ Brain analyzed. Total neurons: {stats['total_neurons']}")
            if args.output:
                print(f"Results saved to {args.output}")
               
                
        elif args.command == 'save':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the save command", file=sys.stderr)
                sys.exit(1)
            
            cli.save_checkpoint(args.file, compress=args.compress)
            print(f"✓ Brain state saved to {args.file}")
                
        elif args.command == 'load':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the load command", file=sys.stderr)
                sys.exit(1)
            
            cli.load_checkpoint(args.file)
            print(f"✓ Brain state loaded from {args.file}")
                
        elif args.command == 'batch':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the batch command", file=sys.stderr)
                sys.exit(1)
            
            results = cli.run_batch_simulation(
                args.steps, 
                count=args.count,
                enable_learning=args.enable_learning,
                progress_bar=args.progress
            )
            print(f"✓ Batch simulation completed. Ran {len(results)} simulations.")
                
        elif args.command == 'experiment':
            if not PYNLM_AVAILABLE:
                print("Error: pynlm package is required for the experiment command", file=sys.stderr)
                sys.exit(1)
            
            cli.run_learning_experiment(
                args.steps, 
                experiment_type=args.type,
                enable_all_subsystems=args.enable_all_subsystems,
                progress_bar=args.progress
            )
            print(f"✓ Learning experiment '{args.type}' completed.")
                
        elif args.command == 'process':
            results = cli.run_checkpoint_processing(args.pattern, args.output_dir)
            print(f"✓ Processed {len(results)} checkpoint(s).")
                
        elif args.command == 'list':
            # List checkpoints
            results = cli.run_checkpoint_processing('*.ckpt')  # Default pattern
            print(f"✓ Found {len(results)} checkpoint(s).")
            
        elif args.command == 'help':
            # Show help for specific command
            subparsers = parser._subparsers._group_actions[0]
            if args.command_name in subparsers.choices:
                subparsers.choices[args.command_name].print_help()
            else:
                print(f"Unknown command: {args.command_name}", file=sys.stderr)
                sys.exit(1)
                
        else:
            print(f"Unknown command: {args.command}", file=sys.stderr)
            parser.print_help()
            sys.exit(1)
            
    except Exception as e:
        logger.error(f"Error executing command '{args.command}': {e}", exc_info=True)
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()