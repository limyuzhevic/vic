#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

Advanced command-line interface for configuring, running, and managing NLM simulations.
"""

import sys
import os
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, Any, Optional
import pynlm

class NLMCLI:
    """Command line interface for NLM operations."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
    
    def create_config(self, args):
        """Create configuration from file or command line."""
        print("Creating configuration...")
        
        if args.file:
            # Load from file
            self.config = pynlm.createConfigFromFile(args.file)
            print(f"Configuration loaded from {args.file}")
        elif args.config_dict:
            # Load from Python dictionary (simplified - for demo)
            self.config = pynlm.createDefaultConfig()
            # Apply basic overrides
            if args.neuron_count:
                self.config.set("neuron_count", args.neuron_count, pynlm.ConfigSource.Runtime)
            if args.region_count:
                self.config.set("region_count", args.region_count, pynlm.ConfigSource.Runtime)
            if args.connection_probability:
                self.config.set("connection_probability", args.connection_probability, pynlm.ConfigSource.Runtime)
            print("Configuration created with command-line parameters")
        else:
            # Use default configuration
            self.config = pynlm.createDefaultConfig()
            print("Using default configuration")
        
        # Display configuration summary
        print("\nConfiguration Summary:")
        print(self.config.summary())
        
        return self.config
    
    def create_brain(self, args):
        """Create and initialize brain."""
        print("Creating brain...")
        
        if not self.config:
            self.config = pynlm.createDefaultConfig()
        
        # Create brain with configuration
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"Brain created with {self.brain.getTotalNeuronCount()} neurons")
        print(f"Brain has {self.brain.getRegionCount()} regions")
        
        return self.brain
    
    def create_world(self, args):
        """Create and configure world."""
        print("Creating world...")
        
        self.world = pynlm.createSimpleWorld()
        
        # Configure world
        width = args.world_width or 10
        height = args.world_height or 10
        vision_width = args.vision_width or 8
        vision_height = args.vision_height or 8
        
        self.world.configure(width, height, vision_width, vision_height)
        
        # Set agent start position if specified
        if args.agent_start_x is not None and args.agent_start_y is not None:
            self.world.setAgentStart(args.agent_start_x, args.agent_start_y)
        
        # Configure world properties
        if args.max_energy:
            self.world.setMaxEnergy(args.max_energy)
        
        if args.energy_decay_rate:
            self.world.setEnergyDecayRate(args.energy_decay_rate)
        
        print(f"World created: {width}x{height} with {vision_width}x{vision_height} vision")
        
        return self.world
    
    def create_agent(self, args):
        """Create agent brain."""
        print("Creating agent...")
        
        if not self.brain:
            self.brain = self.create_brain(args)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable features based on arguments
        if args.enable_reward_modulation:
            self.agent.enableRewardModulation(True)
        
        if args.enable_structural_plasticity:
            self.agent.enableStructuralPlasticity(True)
        
        if args.enable_development:
            self.agent.enableDevelopment(True)
        
        if args.enable_curiosity:
            self.agent.enableCuriosity(True)
        
        print("Agent created with enabled features:")
        if args.enable_reward_modulation:
            print("  - Reward modulation")
        if args.enable_structural_plasticity:
            print("  - Structural plasticity")
        if args.enable_development:
            print("  - Development")
        if args.enable_curiosity:
            print("  - Curiosity")
        
        return self.agent
    
    def run_simulation(self, args):
        """Run simulation with specified parameters."""
        print(f"Running simulation for {args.steps} steps...")
        
        start_time = args.current_time or 0.0
        end_time = start_time + args.simulation_time or (args.steps * 0.1)
        
        for step in range(args.steps):
            current_time = start_time + (step * 0.1)
            
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes
            self.brain.step(step, current_time)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(action, current_time)
            
            # Log progress periodically
            if args.log_interval and step % args.log_interval == 0:
                self.log_simulation_stats(step, current_time)
        
        # Log final statistics
        self.log_simulation_stats(args.steps, end_time)
        
        print("Simulation completed!")
        return self.brain
    
    def log_simulation_stats(self, step, current_time):
        """Log simulation statistics."""
        firing_count = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        avg_firing_rate = self.brain.getAverageFiringRate()
        active_neurons = self.brain.getActiveNeuronCount()
        
        print(f"Step {step:4d} | Time: {current_time:6.2f}s | "
              f"Firing: {firing_count:4d} | Spikes: {total_spikes:6d} | "
              f"Rate: {avg_firing_rate:5.2f} Hz | Active: {active_neurons:4d}")
    
    def analyze_brain(self, args):
        """Analyze brain performance and statistics."""
        print("Analyzing brain...")
        
        if not args.quick:
            # Use BrainAnalyzer if available
            try:
                analyzer = pynlm.BrainAnalyzer(self.brain)
                
                if args.analyze_spike_patterns:
                    print("\nAnalyzing spike patterns...")
                    patterns = analyzer.analyzeSpikePatterns(maxNeurons=100)
                    print(f"Found {len(patterns)} spike patterns")
                
                if args.detect_active_regions:
                    print("\nDetecting active regions...")
                    threshold = args.region_threshold or 0.1
                    active_regions = analyzer.detectActiveRegions(threshold)
                    print(f"Found {len(active_regions)} active regions")
                
                if args.calculate_metrics:
                    print("\nCalculating network metrics...")
                    metrics = analyzer.calculateNetworkMetrics()
                    print("Network metrics:")
                    for key, value in metrics.items():
                        print(f"  {key}: {value}")
                
            except Exception as e:
                print(f"BrainAnalyzer not available: {e}")
                # Fallback to basic stats
                self.log_simulation_stats(0, 0)
        else:
            # Quick analysis
            print(f"\nQuick Analysis:")
            print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {self.brain.getTotalSynapseCount()}")
            print(f"  Active neurons: {self.brain.getActiveNeuronCount()}")
            print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
    
    def save_brain(self, args):
        """Save brain state to file."""
        print(f"Saving brain to {args.output_file}...")
        
        success = self.brain.save(args.output_file)
        if success:
            print("Brain saved successfully!")
        else:
            print("Failed to save brain!")
        
        return success
    
    def load_brain(self, args):
        """Load brain state from file."""
        print(f"Loading brain from {args.input_file}...")
        
        success = self.brain.load(args.input_file)
        if success:
            print("Brain loaded successfully!")
        else:
            print("Failed to load brain!")
        
        return success
    
    def export_data(self, args):
        """Export simulation data."""
        print(f"Exporting data to {args.output_file}...")
        
        # This would use SimulationController's exportSimulationData
        # For now, we'll create a simple export
        output_data = {
            "neurons": self.brain.getTotalNeuronCount(),
            "synapses": self.brain.getTotalSynapseCount(),
            "spike_count": self.brain.getTotalSpikeCount(),
            "firing_rate": self.brain.getAverageFiringRate(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "timestamp": args.current_time or 0.0
        }
        
        if args.output_file.endswith('.json'):
            with open(args.output_file, 'w') as f:
                json.dump(output_data, f, indent=2)
        elif args.output_file.endswith(('.yaml', '.yml')):
            with open(args.output_file, 'w') as f:
                yaml.dump(output_data, f)
        else:
            print(f"Unsupported file format: {args.output_file}")
            return False
        
        print("Data exported successfully!")
        return True
    
    def run_batch_simulation(self, args):
        """Run batch simulation with different parameters."""
        print("Running batch simulation...")
        
        if not args.steps_list:
            print("Please specify steps list with --steps-list")
            return
        
        results = []
        for steps in args.steps_list:
            print(f"\n--- Running {steps} steps ---")
            
            # Reset and recreate for each batch run
            self.brain = None
            self.world = None
            self.agent = None
            
            self.config = pynlm.createDefaultConfig()
            self.config.set("neuron_count", args.neuron_count or 100, pynlm.ConfigSource.Runtime)
            self.config.set("region_count", args.region_count or 1, pynlm.ConfigSource.Runtime)
            
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(args.world_width or 10, args.world_height or 10)
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(steps):
                self.world.update(0.1)
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, step * 0.1)
            
            # Collect statistics
            result = {
                "steps": steps,
                "spike_count": self.brain.getTotalSpikeCount(),
                "firing_rate": self.brain.getAverageFiringRate(),
                "active_neurons": self.brain.getActiveNeuronCount()
            }
            results.append(result)
            
            print(f"Completed: {result}")
        
        print(f"\nBatch simulation completed with {len(results)} results")
        return results
    
    def run(self, args):
        """Main CLI execution method."""
        try:
            if args.action == "create-config":
                self.create_config(args)
            
            elif args.action == "create-brain":
                self.create_brain(args)
            
            elif args.action == "create-world":
                self.create_world(args)
            
            elif args.action == "create-agent":
                self.create_agent(args)
            
            elif args.action == "run":
                if not self.agent:
                    self.create_agent(args)
                self.run_simulation(args)
            
            elif args.action == "analyze":
                if not self.brain:
                    self.create_brain(args)
                self.analyze_brain(args)
            
            elif args.action == "save":
                if not self.brain:
                    self.create_brain(args)
                self.save_brain(args)
            
            elif args.action == "load":
                if not self.brain:
                    self.create_brain(args)
                self.load_brain(args)
            
            elif args.action == "export":
                if not self.brain:
                    self.create_brain(args)
                self.export_data(args)
            
            elif args.action == "batch":
                self.run_batch_simulation(args)
            
            elif args.action == "help":
                self.print_help()
            
            else:
                print(f"Unknown action: {args.action}")
                self.print_help()
                
        except Exception as e:
            print(f"Error: {e}")
            import traceback
            traceback.print_exc()
            return 1
        
        return 0
    
    def print_help(self):
        """Print help information."""
        help_text = """
NLM Command Line Interface

Usage: nlm-cli [action] [options]

Actions:
  create-config    Create configuration
  create-brain    Create brain
  create-world    Create world
  create-agent    Create agent
  run             Run simulation
  analyze         Analyze brain
  save            Save brain to file
  load            Load brain from file
  export          Export data to file
  batch           Run batch simulation
  help            Show this help

Examples:
  nlm-cli create-config --neuron_count 100 --region_count 1
  nlm-cli create-brain
  nlm-cli create-world --width 20 --height 20
  nlm-cli create-agent --enable_reward_modulation --enable_curiosity
  nlm-cli run --steps 1000 --log_interval 100
  nlm-cli analyze --calculate_metrics
  nlm-cli save --output_file brain_checkpoint.ncl
  nlm-cli batch --steps_list 100 500 1000

For more detailed help, run specific actions with --help flag.
"""
        print(help_text)

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_cli.py create-config --neuron_count 100 --region_count 1
  python nlm_cli.py create-brain
  python nlm_cli.py create-world --width 20 --height 20
  python nlm_cli.py create-agent --enable_reward_modulation
  python nlm_cli.py run --steps 1000 --log_interval 100
  python nlm_cli.py analyze --calculate_metrics
  python nlm_cli.py save --output_file brain_checkpoint.ncl
  python nlm_cli.py batch --steps_list 100 500 1000
        """
    )
    
    # Global arguments
    parser.add_argument(
        "action",
        choices=["create-config", "create-brain", "create-world", "create-agent",
                 "run", "analyze", "save", "load", "export", "batch", "help"],
        help="Action to perform"
    )
    
    # Configuration arguments
    config_group = parser.add_argument_group("Configuration Options")
    config_group.add_argument(
        "--file", "-f",
        help="Configuration file (JSON/YAML)"
    )
    config_group.add_argument(
        "--neuron_count", "-n",
        type=int,
        help="Number of neurons"
    )
    config_group.add_argument(
        "--region_count", "-r",
        type=int,
        help="Number of regions"
    )
    config_group.add_argument(
        "--connection_probability",
        type=float,
        help="Connection probability (0.0-1.0)"
    )
    
    # World arguments
    world_group = parser.add_argument_group("World Options")
    world_group.add_argument(
        "--world_width", "--width",
        type=int,
        help="World width"
    )
    world_group.add_argument(
        "--world_height", "--height",
        type=int,
        help="World height"
    )
    world_group.add_argument(
        "--vision_width",
        type=int,
        help="Vision width"
    )
    world_group.add_argument(
        "--vision_height",
        type=int,
        help="Vision height"
    )
    world_group.add_argument(
        "--agent_start_x",
        type=int,
        help="Agent start X position"
    )
    world_group.add_argument(
        "--agent_start_y",
        type=int,
        help="Agent start Y position"
    )
    world_group.add_argument(
        "--max_energy",
        type=float,
        help="Maximum energy"
    )
    world_group.add_argument(
        "--energy_decay_rate",
        type=float,
        help="Energy decay rate"
    )
    
    # Agent arguments
    agent_group = parser.add_argument_group("Agent Options")
    agent_group.add_argument(
        "--enable_reward_modulation",
        action="store_true",
        help="Enable reward modulation"
    )
    agent_group.add_argument(
        "--enable_structural_plasticity",
        action="store_true",
        help="Enable structural plasticity"
    )
    agent_group.add_argument(
        "--enable_development",
        action="store_true",
        help="Enable development"
    )
    agent_group.add_argument(
        "--enable_curiosity",
        action="store_true",
        help="Enable curiosity"
    )
    
    # Simulation arguments
    sim_group = parser.add_argument_group("Simulation Options")
    sim_group.add_argument(
        "--steps", "-s",
        type=int,
        help="Number of simulation steps"
    )
    sim_group.add_argument(
        "--simulation_time",
        type=float,
        help="Total simulation time"
    )
    sim_group.add_argument(
        "--log_interval",
        type=int,
        help="Log interval (steps)"
    )
    sim_group.add_argument(
        "--current_time",
        type=float,
        help="Current simulation time"
    )
    
    # Analysis arguments
    analysis_group = parser.add_argument_group("Analysis Options")
    analysis_group.add_argument(
        "--analyze_spike_patterns",
        action="store_true",
        help="Analyze spike patterns"
    )
    analysis_group.add_argument(
        "--detect_active_regions",
        action="store_true",
        help="Detect active regions"
    )
    analysis_group.add_argument(
        "--region_threshold",
        type=float,
        help="Region detection threshold"
    )
    analysis_group.add_argument(
        "--calculate_metrics",
        action="store_true",
        help="Calculate network metrics"
    )
    analysis_group.add_argument(
        "--quick",
        action="store_true",
        help="Quick analysis"
    )
    
    # File arguments
    file_group = parser.add_argument_group("File Options")
    file_group.add_argument(
        "--output_file", "-o",
        help="Output file"
    )
    file_group.add_argument(
        "--input_file",
        help="Input file"
    )
    file_group.add_argument(
        "--steps_list",
        nargs="+",
        type=int,
        help="List of step counts for batch simulation"
    )
    
    args = parser.parse_args()
    
    # Create and run CLI
    cli = NLMCLI()
    return cli.run(args)

if __name__ == "__main__":
    sys.exit(main())
