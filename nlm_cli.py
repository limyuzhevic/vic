#!/usr/bin/env python3

"""
NLM (Neural Learning Machine) Command Line Interface

A command-line interface for the NLM neural simulation framework.
Provides easy access to simulation, checkpoint management, and analysis.
"""

import sys
import os
import json
import argparse
from pathlib import Path

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not available. Please install the Python bindings.")
    print("Run: pip install pynlm")

class NLMCLI:
    """Command line interface for NLM operations."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.world = None
        self.agent = None
        
    def create_brain(self, config_file=None):
        """Create a brain with optional configuration file."""
        try:
            if config_file and os.path.exists(config_file):
                self.config = pynlm.Config()
                self.config.loadFromFile(config_file)
                print(f"Configuration loaded from {config_file}")
            else:
                self.config = pynlm.createDefaultConfig()
                # Set demo-friendly defaults
                self.config.set("neuron_count", 1000)
                self.config.set("region_count", 1)
                self.config.set("connection_probability", 0.1f)
                self.config.set("simulation_timestep", 0.001)
                print("Default configuration created")
            
            self.brain = pynlm.createBrain(self.config)
            if self.brain.initialize():
                print("Brain initialized successfully")
                return True
            else:
                print("Failed to initialize brain")
                return False
                
        except Exception as e:
            print(f"Error creating brain: {e}")
            return False
    
    def create_world(self, width=16, height=16, vision_width=8, vision_height=8):
        """Create a world with specified dimensions."""
        try:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width, height, vision_width, vision_height)
            print(f"World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
            return True
        except Exception as e:
            print(f"Error creating world: {e}")
            return False
    
    def run_simulation(self, steps=1000, headless=False):
        """Run a simulation for specified number of steps."""
        if not self.brain or not self.world:
            print("Brain or world not initialized")
            return False
            
        try:
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Configure agent for demo
            self.agent.enableCuriosity(True)
            self.agent.enableRewardModulation(True)
            
            total_reward = 0.0
            successful_steps = 0
            
            for i in range(steps):
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Brain step
                self.brain.step(i, i * 0.001)
                
                # Get motor command
                cmd = self.agent.decodeMotorCommand()
                
                # Apply action to world
                result = self.world.applyMotorCommand(cmd, i * 0.001)
                
                # Apply reward modulation
                self.agent.applyRewardModulation(result.reward, 0.0)
                
                total_reward += result.reward
                successful_steps += 1
                
                if i % 100 == 0 and not headless:
                    self._print_status(i, total_reward / (i + 1), result.success)
            
            avg_reward = total_reward / steps if steps > 0 else 0.0
            print(f"\nSimulation completed: {successful_steps}/{steps} steps")
            print(f"Average reward: {avg_reward:.4f}")
            print(f"Success rate: {result.success}")
            
            # Log final status
            self.brain.logStatus()
            
            return True
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return False
    
    def save_checkpoint(self, filepath):
        """Save brain state to checkpoint file."""
        if not self.brain:
            print("Brain not initialized")
            return False
            
        try:
            if self.brain.save(filepath):
                print(f"Checkpoint saved to {filepath}")
                return True
            else:
                print(f"Failed to save checkpoint to {filepath}")
                return False
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    def load_checkpoint(self, filepath):
        """Load brain state from checkpoint file."""
        if not self.brain:
            print("Brain not initialized")
            return False
            
        try:
            if self.brain.load(filepath):
                print(f"Checkpoint loaded from {filepath}")
                return True
            else:
                print(f"Failed to load checkpoint from {filepath}")
                return False
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def analyze_brain(self, output_file=None):
        """Analyze brain state and optionally save to file."""
        if not self.brain:
            print("Brain not initialized")
            return False
            
        try:
            print("=== Brain Analysis ===")
            print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"Active neurons: {self.brain.getActiveNeuronCount()}")
            print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
            print(f"Average firing rate: {self.brain.getAverageFiringRate():.4f}")
            print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio():.4f}")
            
            # Get neuromodulation levels
            if self.agent:
                print(f"Dopamine level: {self.agent.getNeuromodulationLevel():.4f}")
                print(f"Curiosity level: {self.agent.getCuriosityLevel():.4f}")
                print(f"Novelty level: {self.agent.getNoveltyLevel():.4f}")
                print(f"Prediction error: {self.agent.getPredictionError():.4f}")
            
            # Save analysis to file if requested
            if output_file:
                analysis = {
                    "total_neurons": self.brain.getTotalNeuronCount(),
                    "total_synapses": self.brain.getTotalSynapseCount(),
                    "active_neurons": self.brain.getActiveNeuronCount(),
                    "total_spikes": self.brain.getTotalSpikeCount(),
                    "average_firing_rate": self.brain.getAverageFiringRate(),
                    "excitatory_inhibition_ratio": self.brain.getExcitationInhibitionRatio(),
                }
                
                if self.agent:
                    analysis["dopamine_level"] = self.agent.getNeuromodulationLevel()
                    analysis["curiosity_level"] = self.agent.getCuriosityLevel()
                    analysis["novelty_level"] = self.agent.getNoveltyLevel()
                    analysis["prediction_error"] = self.agent.getPredictionError()
                
                with open(output_file, 'w') as f:
                    json.dump(analysis, f, indent=2)
                
                print(f"Analysis saved to {output_file}")
            
            return True
            
        except Exception as e:
            print(f"Error analyzing brain: {e}")
            return False
    
    def _print_status(self, step, avg_reward, success):
        """Print status every 100 steps."""
        print(f"Step {step:5d}: Reward={avg_reward:.4f}, Success={success}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s init --config my_config.json
  %(prog)s run --steps 2000 --width 20 --height 20
  %(prog)s checkpoint save brain_state.bin
  %(prog)s analyze --output analysis.json
  %(prog)s run --checkpoint brain_state.bin --steps 1000

For more help on specific commands, use:
  %(prog)s <command> --help
        """.strip()
    )
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Init command
    init_parser = subparsers.add_parser("init", help="Initialize brain and world")
    init_parser.add_argument("--config", help="Configuration file path")
    init_parser.add_argument("--width", type=int, default=16, help="World width")
    init_parser.add_argument("--height", type=int, default=16, help="World height")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run simulation")
    run_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    run_parser.add_argument("--width", type=int, default=16, help="World width")
    run_parser.add_argument("--height", type=int, default=16, help="World height")
    run_parser.add_argument("--checkpoint", help="Load checkpoint before running")
    run_parser.add_argument("--output", help="Save checkpoint after running")
    run_parser.add_argument("--headless", action="store_true", help="Run without status output")
    
    # Checkpoint command
    checkpoint_parser = subparsers.add_parser("checkpoint", help="Manage checkpoints")
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest="checkpoint_action", help="Checkpoint actions")
    
    save_parser = checkpoint_subparsers.add_parser("save", help="Save brain state")
    save_parser.add_argument("filepath", help="Checkpoint file path")
    
    load_parser = checkpoint_subparsers.add_parser("load", help="Load brain state")
    load_parser.add_argument("filepath", help="Checkpoint file path")
    
    # Analyze command
    analyze_parser = subparsers.add_parser("analyze", help="Analyze brain state")
    analyze_parser.add_argument("--output", help="Output file for analysis")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run a quick demo")
    demo_parser.add_argument("--steps", type=int, default=500, help="Number of steps")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module not available. Please install the Python bindings.")
        sys.exit(1)
    
    # Initialize CLI
    cli = NLMCLI()
    
    # Execute command
    if args.command == "init":
        success = cli.create_brain(args.config)
        if success:
            success = cli.create_world(args.width, args.height)
        sys.exit(0 if success else 1)
        
    elif args.command == "run":
        # Initialize brain and world
        if not cli.create_brain(args.config):
            sys.exit(1)
        if not cli.create_world(args.width, args.height):
            sys.exit(1)
        
        # Load checkpoint if specified
        if args.checkpoint:
            if not cli.load_checkpoint(args.checkpoint):
                sys.exit(1)
        
        # Run simulation
        success = cli.run_simulation(args.steps, args.headless)
        
        # Save checkpoint if specified
        if args.output and success:
            cli.save_checkpoint(args.output)
        
        sys.exit(0 if success else 1)
        
    elif args.command == "checkpoint":
        if not cli.create_brain():
            sys.exit(1)
        
        if args.checkpoint_action == "save":
            success = cli.save_checkpoint(args.filepath)
            sys.exit(0 if success else 1)
        elif args.checkpoint_action == "load":
            success = cli.load_checkpoint(args.filepath)
            sys.exit(0 if success else 1)
        else:
            parser.print_help()
            sys.exit(1)
            
    elif args.command == "analyze":
        if not cli.create_brain():
            sys.exit(1)
        
        success = cli.analyze_brain(args.output)
        sys.exit(0 if success else 1)
        
    elif args.command == "demo":
        if not cli.create_brain():
            sys.exit(1)
        if not cli.create_world():
            sys.exit(1)
        
        success = cli.run_simulation(args.steps, True)
        sys.exit(0 if success else 1)
        
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()
