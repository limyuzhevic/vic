#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface (CLI)

Advanced CLI tool for managing NLM brain simulations, experiments, and configurations.
This provides enhanced functionality beyond the basic demo and command-line arguments.
"""

import argparse
import json
import sys
import os
import time
from typing import Dict, Any, Optional
import subprocess

# Add the project root to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some CLI features will be limited.")

class NLMCLI:
    """Advanced NLM CLI with enhanced functionality."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig() if PYNLM_AVAILABLE else None
        self.brain = None
        self.world = None
        self.agent = None
        
    def print_banner(self):
        """Print the NLM banner."""
        print("=" * 70)
        print("NLM — Advanced Command Line Interface")
        print("Neural Learning Machine - Phase 6 Integration")
        print("=" * 70)
        print()
    
    def create_brain(self, args):
        """Create and initialize a brain with specified configuration."""
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm not available. Cannot create brain.")
            return False
            
        print(f"Creating brain with {args.neurons} neurons...")
        
        # Set configuration
        self.config.set("brain.neuron_count", args.neurons)
        self.config.set("brain.synapse_density", args.synapse_density)
        self.config.set("brain.connection_probability", args.connection_probability)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"✓ Brain created successfully")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        
        return True
    
    def run_simulation(self, args):
        """Run a simulation with specified parameters."""
        if not self.brain:
            print("ERROR: No brain initialized. Use 'create' command first.")
            return False
            
        print(f"Running simulation for {args.steps} steps...")
        
        # Create world if not exists
        if args.world:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(
                width=args.world_width,
                height=args.world_height,
                visionWidth=args.vision_width,
                vision_height=args.vision_height
            )
            self.world.reset()
            
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable features based on arguments
            if args.reward_modulation:
                self.agent.enableRewardModulation(True)
            if args.structural_plasticity:
                self.agent.enableStructuralPlasticity(True)
            if args.development:
                self.agent.enableDevelopment(True)
            if args.curiosity:
                self.agent.enableCuriosity(True)
        
        # Run simulation
        start_time = time.time()
        max_steps = args.steps
        
        for step in range(max_steps):
            # World update
            if self.world:
                self.world.update(0.1)  # 10ms timestep
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Get motor command
                motor_cmd = self.agent.decodeMotorCommand()
                
                # Apply motor command
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply reward modulation if enabled
                if args.reward_modulation:
                    reward = 0.0
                    # Get reward from world (simplified)
                    self.agent.applyRewardModulation(reward, 0.0)
            
            # Brain step
            self.brain.step(step)
            
            # Log progress
            if step % args.log_interval == 0:
                print(f"  Step {step:5d}: "
                      f"Firing {self.brain.getFiringNeuronCount():4d} "
                      f"(Avg: {self.brain.getAverageFiringRate():6.2f} Hz)")
                
                if self.agent:
                    print(f"    Curiosity: {self.agent.getCuriosityLevel():5.3f}, "
                          f"Novelty: {self.agent.getNoveltyLevel():5.3f}, "
                          f"Dopamine: {self.agent.getNeuromodulationLevel():5.3f}")
        
        end_time = time.time()
        print(f"\n✓ Simulation completed in {end_time - start_time:.2f} seconds")
        print(f"  Final firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        
        return True
    
    def run_experiment(self, args):
        """Run a Phase 6 integration experiment."""
        if not PYNLM_AVAILABLE:
            print("ERROR: pynlm not available. Cannot run experiment.")
            return False
            
        print("Running Phase 6 integration experiment...")
        
        from src.experiments.Phase6IntegratedExperiment import Phase6IntegratedExperiment, Phase6Config
        
        # Create experiment
        experiment = Phase6IntegratedExperiment()
        
        # Configure experiment
        config = Phase6Config()
        config.neuronCount = args.neurons
        config.maxSteps = args.steps
        config.enableCheckpointing = args.checkpoint
        config.enableReplay = args.replay
        config.enableDevelopment = args.development
        config.checkpointPath = args.checkpoint_path
        
        # Run experiment
        result = experiment.run(config)
        
        print("\n=== Experiment Results ===")
        print(f"Total Reward: {result.totalReward:.3f}")
        print(f"Avg Firing Rate: {result.avgFiringRate:.2f} Hz")
        print(f"Memory Episodes: {result.memoryEpisodesStored:.0f}")
        print(f"Dopamine Level: {result.dopamineLevel:.3f}")
        print(f"Wall Clock Time: {result.totalWallClockTime:.2f}s")
        
        print("\n=== Integration Status ===")
        print(f"Working Memory: {'CONNECTED' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
        print(f"Episodic Memory: {'CONNECTED' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
        print(f"Neuromodulation: {'CONNECTED' if result.neuromodulationIntegrated else 'DISCONNECTED'}")
        print(f"Prediction: {'CONNECTED' if result.predictionIntegrated else 'DISCONNECTED'}")
        print(f"Development: {'CONNECTED' if result.developmentIntegrated else 'DISCONNECTED'}")
        print(f"Checkpointing: {'WORKING' if result.checkpointingWorks else 'NOT WORKING'}")
        
        return True
    
    def save_brain(self, args):
        """Save brain state to file."""
        if not self.brain:
            print("ERROR: No brain to save.")
            return False
            
        print(f"Saving brain state to {args.output}...")
        
        if self.brain.save(args.output):
            print("✓ Brain state saved successfully")
            return True
        else:
            print("✗ Failed to save brain state")
            return False
    
    def load_brain(self, args):
        """Load brain state from file."""
        print(f"Loading brain state from {args.input}...")
        
        # Create new brain
        if not self.create_brain(args):
            return False
            
        if self.brain.load(args.input):
            print("✓ Brain state loaded successfully")
            return True
        else:
            print("✗ Failed to load brain state")
            return False
    
    def list_config(self, args):
        """List configuration keys."""
        if not self.config:
            print("ERROR: No configuration available.")
            return False
            
        print("Available configuration keys:")
        keys = self.config.getKeys()
        for key in sorted(keys):
            print(f"  {key}")
        
        return True
    
    def run_command(self, cmd, args):
        """Run a specific command."""
        commands = {
            'create': self.create_brain,
            'run': self.run_simulation,
            'experiment': self.run_experiment,
            'save': self.save_brain,
            'load': self.load_brain,
            'list-config': self.list_config,
        }
        
        if cmd not in commands:
            print(f"ERROR: Unknown command '{cmd}'")
            return False
            
        return commands[cmd](args)
    
    def parse_args(self):
        """Parse command line arguments."""
        parser = argparse.ArgumentParser(
            description="NLM Advanced Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm create --neurons 1000 --connection_probability 0.1
  nlm run --steps 500 --log-interval 100
  nlm experiment --neurons 500 --steps 1000
  nlm save --output brain_state.bin
  nlm load --input brain_state.bin

For more information about specific commands, use:
  nlm <command> --help
            """
        )
        
        # Main command
        parser.add_argument(
            'command',
            choices=['create', 'run', 'experiment', 'save', 'load', 'list-config'],
            help='Command to execute'
        )
        
        # Global arguments
        parser.add_argument(
            '--verbose', '-v',
            action='store_true',
            help='Enable verbose output'
        )
        
        # Create command arguments
        create_parser = parser.add_argument_group('create command options')
        create_parser.add_argument(
            '--neurons', '-n',
            type=int,
            default=1000,
            help='Number of neurons to create (default: 1000)'
        )
        create_parser.add_argument(
            '--synapse-density', '-sd',
            type=float,
            default=0.1,
            help='Synapse density (default: 0.1)'
        )
        create_parser.add_argument(
            '--connection-probability', '-cp',
            type=float,
            default=0.1,
            help='Connection probability (default: 0.1)'
        )
        
        # Run command arguments
        run_parser = parser.add_argument_group('run command options')
        run_parser.add_argument(
            '--steps', '-s',
            type=int,
            default=1000,
            help='Number of simulation steps (default: 1000)'
        )
        run_parser.add_argument(
            '--log-interval', '-li',
            type=int,
            default=100,
            help='Log interval in steps (default: 100)'
        )
        run_parser.add_argument(
            '--world', '-w',
            action='store_true',
            help='Enable world simulation'
        )
        run_parser.add_argument(
            '--world-width', '-ww',
            type=int,
            default=20,
            help='World width (default: 20)'
        )
        run_parser.add_argument(
            '--world-height', '-wh',
            type=int,
            default=20,
            help='World height (default: 20)'
        )
        run_parser.add_argument(
            '--vision-width', '-vw',
            type=int,
            default=8,
            help='Vision width (default: 8)'
        )
        run_parser.add_argument(
            '--vision-height', '-vh',
            type=int,
            default=8,
            help='Vision height (default: 8)'
        )
        run_parser.add_argument(
            '--reward-modulation',
            action='store_true',
            help='Enable reward modulation'
        )
        run_parser.add_argument(
            '--structural-plasticity',
            action='store_true',
            help='Enable structural plasticity'
        )
        run_parser.add_argument(
            '--development',
            action='store_true',
            help='Enable development'
        )
        run_parser.add_argument(
            '--curiosity',
            action='store_true',
            help='Enable curiosity'
        )
        
        # Experiment command arguments
        experiment_parser = parser.add_argument_group('experiment command options')
        experiment_parser.add_argument(
            '--neurons', '-n',
            type=int,
            default=500,
            help='Number of neurons (default: 500)'
        )
        experiment_parser.add_argument(
            '--steps', '-s',
            type=int,
            default=2000,
            help='Number of steps (default: 2000)'
        )
        experiment_parser.add_argument(
            '--checkpoint', '-c',
            action='store_true',
            help='Enable checkpointing'
        )
        experiment_parser.add_argument(
            '--replay', '-r',
            action='store_true',
            help='Enable replay'
        )
        experiment_parser.add_argument(
            '--development', '-d',
            action='store_true',
            help='Enable development'
        )
        experiment_parser.add_argument(
            '--checkpoint-path', '-cpath',
            type=str,
            default='./checkpoint_test.bin',
            help='Checkpoint file path (default: ./checkpoint_test.bin)'
        )
        
        # Save command arguments
        save_parser = parser.add_argument_group('save command options')
        save_parser.add_argument(
            '--output', '-o',
            type=str,
            required=True,
            help='Output file path'
        )
        
        # Load command arguments
        load_parser = parser.add_argument_group('load command options')
        load_parser.add_argument(
            '--input', '-i',
            type=str,
            required=True,
            help='Input file path'
        )
        
        # Parse arguments
        args = parser.parse_args()
        
        # Set verbose mode
        if args.verbose:
            print("Verbose mode enabled")
        
        return args
    
    def run(self):
        """Run the CLI."""
        self.print_banner()
        
        # Parse arguments
        args = self.parse_args()
        
        # Run command
        success = self.run_command(args.command, args)
        
        if success:
            print("\n✓ Command completed successfully")
        else:
            print("\n✗ Command failed")
            
        return 0 if success else 1


def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    return cli.run()


if __name__ == '__main__':
    sys.exit(main())
