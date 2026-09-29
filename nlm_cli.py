#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Command Line Interface

A comprehensive command-line interface for NLM Python bindings.
Supports brain simulation, experimentation, and configuration management.
"""

import sys
import os
import json
import argparse
from pathlib import Path

# Add the current directory to Python path to import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not found. Please build the Python bindings first.")

class NLMCLI:
    """NLM Command Line Interface class"""
    
    def __init__(self):
        self.current_brain = None
        self.current_world = None
        self.current_agent = None
        self.current_step = 0
        
    def create_advanced_brain(self, neuron_count=1000, region_count=4, 
                              connection_prob=0.15, save_config=None):
        """Create an advanced brain with Phase 6 configuration"""
        if not PYNLM_AVAILABLE:
            return False
            
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", neuron_count)
        config.set("region_count", region_count)
        config.set("connection_probability", connection_prob)
        
        self.current_brain = pynlm.createBrain(config)
        self.current_brain.initialize()
        
        if save_config:
            self.current_brain.getConfig().saveToFile(save_config)
            
        print(f"✓ Created brain with {neuron_count} neurons")
        print(f"  Configuration saved to: {save_config}" if save_config else "")
        return True
        
    def create_agent_simulation(self, world_size=(20, 20), 
                               vision_size=(8, 8), config_file="",
                               enable_all_features=True):
        """Create a complete agent simulation"""
        if not PYNLM_AVAILABLE:
            return False
            
        brain, world, agent = pynlm.createAgentSimulation(
            configFile=config_file,
            worldWidth=world_size[0],
            worldHeight=world_size[1],
            visionWidth=vision_size[0],
            visionHeight=vision_size[1]
        )
        
        self.current_brain = brain
        self.current_world = world
        self.current_agent = agent
        self.current_step = 0
        
        if enable_all_features:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
        print(f"✓ Created agent simulation")
        print(f"  World: {world_size[0]}x{world_size[1]}")
        print(f"  Vision: {vision_size[0]}x{vision_size[1]}")
        return True
        
    def run_simulation(self, steps, pause_between=None):
        """Run a simulation for specified steps"""
        if not PYNLM_AVAILABLE or not self.current_agent:
            print("Error: No simulation setup. Use --create-agent first.")
            return False
            
        print(f"Running simulation for {steps} steps...")
        
        for step in range(steps):
            # Get sensory input
            percept = self.current_world.getSensoryPercept()
            
            # Process sensory input
            self.current_agent.processSensoryInput(percept)
            
            # Brain step
            self.current_brain.step(step)
            
            # Get motor command
            action = self.current_agent.decodeMotorCommand()
            
            # Apply action
            self.current_world.applyMotorCommand(action, self.current_world.getSimulationTime())
            
            if pause_between:
                input(f"Step {step}, press Enter to continue...")
                
            self.current_step = step
            
            # Print progress every 100 steps
            if step % 100 == 0:
                firing = self.current_brain.getFiringNeuronCount()
                avg_rate = self.current_brain.getAverageFiringRate()
                print(f"  Step {step}: {firing} neurons firing, avg rate: {avg_rate:.2f}")
                
        print(f"✓ Simulation completed. Final step: {self.current_step}")
        return True
        
    def run_quick_test(self, neurons=100, steps=500):
        """Run a quick test simulation"""
        if not PYNLM_AVAILABLE:
            return False
            
        print(f"Running quick test: {neurons} neurons, {steps} steps")
        
        avg_reward, avg_firing = pynlm.runQuickTest(neurons, steps)
        
        print(f"✓ Quick test results:")
        print(f"  Average reward: {avg_reward:.4f}")
        print(f"  Average firing rate: {avg_firing:.4f}")
        return True
        
    def create_experimental_setup(self, experiment_type, neurons=500, steps=2000):
        """Create an experimental setup for specific brain studies"""
        if not PYNLM_AVAILABLE:
            return False
            
        if not self.current_brain:
            print("Error: No brain created. Use --create-advanced-brain first.")
            return False
            
        print(f"Configuring brain for {experiment_type} experiment...")
        pynlm.configureBrainForExperiment(self.current_brain, experiment_type, neurons, steps)
        return True
        
    def save_brain(self, filepath):
        """Save brain state to file"""
        if not PYNLM_AVAILABLE or not self.current_brain:
            return False
            
        if self.current_brain.save(filepath):
            print(f"✓ Brain saved to: {filepath}")
            return True
        else:
            print(f"✗ Failed to save brain to: {filepath}")
            return False
            
    def load_brain(self, filepath):
        """Load brain state from file"""
        if not PYNLM_AVAILABLE:
            return False
            
        # For simplicity, create a new brain and load
        config = pynlm.createDefaultConfig()
        self.current_brain = pynlm.createBrain(config)
        
        if self.current_brain.load(filepath):
            print(f"✓ Brain loaded from: {filepath}")
            return True
        else:
            print(f"✗ Failed to load brain from: {filepath}")
            return False
            
    def create_grid_world(self, width=10, height=10, resource_density=0.2, hazard_density=0.1):
        """Create a grid world with resources and hazards"""
        if not PYNLM_AVAILABLE:
            return False
            
        self.current_world = pynlm.createGridWorld(width, height, resource_density, hazard_density)
        print(f"✓ Created grid world: {width}x{height}")
        print(f"  Resources: {int(width * height * resource_density)}")
        print(f"  Hazards: {int(width * height * hazard_density)}")
        return True
        
    def display_status(self):
        """Display current simulation status"""
        if not self.current_brain:
            print("No brain created yet.")
            return
            
        print("Current Status:")
        print(f"  Brain: {self.current_brain.getTotalNeuronCount()} neurons")
        print(f"  Active: {self.current_brain.getFiringNeuronCount()} firing")
        print(f"  Total spikes: {self.current_brain.getTotalSpikeCount()}")
        print(f"  Avg firing rate: {self.current_brain.getAverageFiringRate():.4f}")
        
        if self.current_agent:
            print(f"  Neuromodulation: {self.current_agent.getNeuromodulationLevel():.4f}")
            print(f"  Curiosity: {self.current_agent.getCuriosityLevel():.4f}")
            print(f"  Novelty: {self.current_agent.getNoveltyLevel():.4f}")
            
        if self.current_world:
            print(f"  World time: {self.current_world.getSimulationTime():.3f}")
            print(f"  World size: {self.current_world.getWidth()}x{self.current_world.getHeight()}")
            
    def help(self):
        """Display help information"""
        help_text = """
NLM Command Line Interface

Commands:
  --create-advanced-brain [neurons] [regions] [config]
    Create an advanced brain with Phase 6 configuration
    
  --create-agent [width] [height] [vision_width] [vision_height] [config_file]
    Create a complete agent simulation with world and agent
    
  --run-sim [steps] [pause]
    Run simulation for specified steps
    
  --quick-test [neurons] [steps]
    Run a quick test simulation
    
  --experiment [type] [neurons] [steps]
    Configure brain for specific experiment (memory, prediction, development)
    
  --create-world [width] [height] [resources] [hazards]
    Create a grid world with resources and hazards
    
  --save [filepath]
    Save brain state to file
    
  --load [filepath]
    Load brain state from file
    
  --status
    Display current simulation status
    
  --help
    Show this help message
    
Examples:
  nlm_cli --create-advanced-brain 1000
  nlm_cli --create-agent 20 20 8 8
  nlm_cli --run-sim 1000
  nlm_cli --quick-test
  nlm_cli --experiment memory
  nlm_cli --save my_brain.bin
  nlm_cli --load my_brain.bin
"""
        print(help_text)

def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create advanced brain parser
    parser_brain = subparsers.add_parser("create-advanced-brain", 
                                          help="Create an advanced brain with Phase 6 configuration")
    parser_brain.add_argument("neurons", nargs="?", type=int, default=1000,
                              help="Number of neurons (default: 1000)")
    parser_brain.add_argument("regions", nargs="?", type=int, default=4,
                              help="Number of regions (default: 4)")
    parser_brain.add_argument("config", nargs="?", help="Configuration file to save")
    
    # Create agent parser
    parser_agent = subparsers.add_parser("create-agent",
                                         help="Create a complete agent simulation")
    parser_agent.add_argument("width", nargs="?", type=int, default=20,
                             help="World width (default: 20)")
    parser_agent.add_argument("height", nargs="?", type=int, default=20,
                             help="World height (default: 20)")
    parser_agent.add_argument("vision_width", nargs="?", type=int, default=8,
                             help="Vision width (default: 8)")
    parser_agent.add_argument("vision_height", nargs="?", type=int, default=8,
                             help="Vision height (default: 8)")
    parser_agent.add_argument("config_file", nargs="?", help="Configuration file")
    
    # Run simulation parser
    parser_sim = subparsers.add_parser("run-sim",
                                       help="Run simulation for specified steps")
    parser_sim.add_argument("steps", nargs="?", type=int, default=1000,
                           help="Number of simulation steps (default: 1000)")
    parser_sim.add_argument("pause", nargs="?", type=int, help="Pause between steps")
    
    # Quick test parser
    parser_test = subparsers.add_parser("quick-test",
                                        help="Run a quick test simulation")
    parser_test.add_argument("neurons", nargs="?", type=int, default=100,
                            help="Number of neurons (default: 100)")
    parser_test.add_argument("steps", nargs="?", type=int, default=500,
                            help="Number of steps (default: 500)")
    
    # Experiment parser
    parser_exp = subparsers.add_parser("experiment",
                                       help="Configure brain for specific experiment")
    parser_exp.add_argument("type", choices=["memory", "prediction", "development"],
                           help="Type of experiment")
    parser_exp.add_argument("neurons", nargs="?", type=int, default=500,
                           help="Number of neurons (default: 500)")
    parser_exp.add_argument("steps", nargs="?", type=int, default=2000,
                           help="Number of steps (default: 2000)")
    
    # Create world parser
    parser_world = subparsers.add_parser("create-world",
                                         help="Create a grid world with resources and hazards")
    parser_world.add_argument("width", nargs="?", type=int, default=10,
                             help="World width (default: 10)")
    parser_world.add_argument("height", nargs="?", type=int, default=10,
                             help="World height (default: 10)")
    parser_world.add_argument("resources", nargs="?", type=float, default=0.2,
                             help="Resource density (default: 0.2)")
    parser_world.add_argument("hazards", nargs="?", type=float, default=0.1,
                             help="Hazard density (default: 0.1)")
    
    # Save/Load parsers
    parser_save = subparsers.add_parser("save", help="Save brain state to file")
    parser_save.add_argument("filepath", help="File path to save to")
    
    parser_load = subparsers.add_parser("load", help="Load brain state from file")
    parser_load.add_argument("filepath", help="File path to load from")
    
    # Status parser
    parser_status = subparsers.add_parser("status", help="Display current status")
    
    # Help parser
    parser_help = subparsers.add_parser("help", help="Show help message")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Initialize CLI
    cli = NLMCLI()
    
    # Execute command based on subparser
    if args.command == "create-advanced-brain":
        cli.create_advanced_brain(args.neurons, args.regions, args.config)
    elif args.command == "create-agent":
        cli.create_agent_simulation((args.width, args.height),
                                   (args.vision_width, args.vision_height),
                                   args.config_file)
    elif args.command == "run-sim":
        cli.run_simulation(args.steps, args.pause)
    elif args.command == "quick-test":
        cli.run_quick_test(args.neurons, args.steps)
    elif args.command == "experiment":
        cli.create_experimental_setup(args.type, args.neurons, args.steps)
    elif args.command == "create-world":
        cli.create_grid_world(args.width, args.height, args.resources, args.hazards)
    elif args.command == "save":
        cli.save_brain(args.filepath)
    elif args.command == "load":
        cli.load_brain(args.filepath)
    elif args.command == "status":
        cli.display_status()
    elif args.command == "help":
        cli.help()
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
