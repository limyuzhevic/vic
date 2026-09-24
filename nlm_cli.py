#!/usr/bin/env python3
"""
NLM Command-Line Interface Wrapper

This script provides command-line access to the NLM Python API.
It supports configuration management, simulation control, and result analysis.
"""

import sys
import os
import json
import argparse
from typing import Optional, List
import pynlm

class NLMCLI:
    """Command-line interface for NLM brain simulation."""
    
    def __init__(self):
        self.brain = None
        self.config = None
        self.world = None
        self.agent = None
    
    def create_brain(self, config_file: Optional[str] = None, 
                    neuron_count: int = 1000, synapse_density: float = 0.1) -> bool:
        """Create a brain with optional configuration file."""
        try:
            self.config = pynlm.createDefaultConfig()
            
            # Load configuration file if provided
            if config_file:
                if not self.config.loadFromFile(config_file):
                    print(f"Error: Failed to load config file: {config_file}")
                    return False
            
            # Apply basic configuration
            self.config.set("brain.neuron_count", neuron_count)
            self.config.set("brain.synapse_density", synapse_density)
            self.config.set("brain.v_thresh", -50.0f)
            self.config.set("brain.v_rest", -70.0f)
            self.config.set("brain.tau_mem", 20.0f)
            
            # Create brain
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            print(f"Brain created successfully with {neuron_count} neurons")
            return True
            
        except Exception as e:
            print(f"Error creating brain: {e}")
            return False
    
    def create_agent_simulation(self, world_width: int = 20, world_height: int = 20,
                               vision_width: int = 8, vision_height: int = 8,
                               enable_learning: bool = True) -> bool:
        """Create an agent simulation with world."""
        try:
            if not self.brain:
                print("Error: Brain not created yet. Use --create-brain first.")
                return False
            
            # Create world
            self.world = pynlm.createSimpleWorld()
            self.world.configure(world_width, world_height, vision_width, vision_height)
            self.world.reset()
            self.world.setAgentStart(world_width // 2, world_height // 2)
            
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable learning if requested
            if enable_learning:
                self.agent.enableRewardModulation(True)
                self.agent.enableStructuralPlasticity(True)
                self.agent.enableDevelopment(True)
                self.agent.enableCuriosity(True)
            
            print(f"Agent simulation created in {world_width}x{world_height} world")
            return True
            
        except Exception as e:
            print(f"Error creating agent simulation: {e}")
            return False
    
    def run_simulation(self, steps: int = 1000, verbose: bool = False) -> dict:
        """Run the simulation for specified number of steps."""
        try:
            if not self.brain or not self.agent or not self.world:
                print("Error: Complete simulation not set up. Use --create-agent-simulation first.")
                return {}
            
            print(f"Starting simulation for {steps} steps...")
            
            metrics = {
                'total_steps': steps,
                'final_firing_rate': 0,
                'average_firing_rate': 0.0,
                'total_reward': 0.0,
                'curiosity_level': 0.0,
                'novelty_level': 0.0,
                'dopamine_level': 0.0,
                'developmental_stage': 0,
                'success': False
            }
            
            for step in range(steps):
                self.world.update(0.1)
                
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Get reward from internal signals
                internal = percept.getInternal()
                reward = internal[0] if internal else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development periodically
                if step % 100 == 0:
                    self.agent.updateDevelopment(0.1)
                
                if verbose and step % 100 == 0:
                    print(f"Step {step}: Firing neurons={self.brain.getFiringNeuronCount()}, "
                          f"Curiosity={self.agent.getCuriosityLevel():.3f}")
            
            # Collect final metrics
            metrics['final_firing_rate'] = self.brain.getFiringNeuronCount()
            metrics['average_firing_rate'] = self.brain.getAverageFiringRate()
            metrics['curiosity_level'] = self.agent.getCuriosityLevel()
            metrics['novelty_level'] = self.agent.getNoveltyLevel()
            metrics['dopamine_level'] = self.agent.getNeuromodulationLevel()
            metrics['developmental_stage'] = self.agent.getDevelopmentalStage()
            metrics['total_reward'] = 0.0  # Would accumulate from internal signals
            metrics['success'] = True
            
            print(f"Simulation completed in {steps} steps")
            print(f"Final firing rate: {metrics['final_firing_rate']}")
            print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
            print(f"Developmental stage: {metrics['developmental_stage']}")
            
            return metrics
            
        except Exception as e:
            print(f"Error running simulation: {e}")
            return {}
    
    def save_results(self, filepath: str, metrics: dict = None) -> bool:
        """Save simulation results to file."""
        try:
            results = {}
            if metrics:
                results.update(metrics)
            
            # Add brain state if available
            if self.brain:
                results['brain_stats'] = {
                    'neuron_count': self.brain.getTotalNeuronCount(),
                    'synapse_count': self.brain.getTotalSynapseCount(),
                    'firing_count': self.brain.getFiringNeuronCount(),
                    'spike_count': self.brain.getTotalSpikeCount()
                }
            
            if self.agent:
                results['agent_stats'] = {
                    'curiosity': self.agent.getCuriosityLevel(),
                    'novelty': self.agent.getNoveltyLevel(),
                    'dopamine': self.agent.getNeuromodulationLevel(),
                    'development_stage': self.agent.getDevelopmentalStage()
                }
            
            if self.world:
                results['world_stats'] = {
                    'width': self.world.getWidth(),
                    'height': self.world.getHeight(),
                    'simulation_time': self.world.getSimulationTime()
                }
            
            # Save as JSON
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"Results saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving results: {e}")
            return False
    
    def load_configuration(self, filepath: str) -> bool:
        """Load configuration from file."""
        try:
            if not self.config:
                self.config = pynlm.createDefaultConfig()
            
            if self.config.loadFromFile(filepath):
                print(f"Configuration loaded from {filepath}")
                return True
            else:
                print(f"Error: Failed to load configuration from {filepath}")
                return False
                
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return False
    
    def print_status(self):
        """Print current status of all components."""
        print("=== NLM Simulation Status ===")
        
        if self.config:
            print(f"Configuration: {self.config.summary()}")
        else:
            print("Configuration: Not loaded")
        
        if self.brain:
            print(f"Brain: {self.brain.getTotalNeuronCount()} neurons, "
                  f"{self.brain.getFiringNeuronCount()} firing")
        else:
            print("Brain: Not created")
        
        if self.world:
            print(f"World: {self.world.getWidth()}x{self.world.getHeight()}, "
                  f"time={self.world.getSimulationTime():.2f}")
        else:
            print("World: Not created")
        
        if self.agent:
            print(f"Agent: Curiosity={self.agent.getCuriosityLevel():.3f}, "
                  f"Novelty={self.agent.getNoveltyLevel():.3f}")
            print(f"       Development Stage: {self.agent.getDevelopmentalStage()}")
            print(f"       Reward Mod: {'Enabled' if self.agent.isRewardModulationEnabled() else 'Disabled'}")
        else:
            print("Agent: Not created")
    
    def save_brain_state(self, filepath: str) -> bool:
        """Save brain state to checkpoint file."""
        try:
            if not self.brain:
                print("Error: No brain to save")
                return False
            
            self.brain.save(filepath)
            print(f"Brain state saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving brain state: {e}")
            return False
    
    def load_brain_state(self, filepath: str) -> bool:
        """Load brain state from checkpoint file."""
        try:
            if not self.brain:
                print("Error: No brain to load into")
                return False
            
            if self.brain.load(filepath):
                print(f"Brain state loaded from {filepath}")
                return True
            else:
                print(f"Error: Failed to load brain state from {filepath}")
                return False
                
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False

def main():
    """Main entry point for the CLI."""
    parser = argparse.ArgumentParser(
        description="NLM Command-Line Interface for Brain Simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s --create-brain --neurons 2000
  %(prog)s --create-agent-simulation --world-size 30x30
  %(prog)s --run-simulation --steps 500
  %(prog)s --save-results results.json
  %(prog)s --status
  %(prog)s --config my_config.json
        """)
    
    # Global options
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--help', '-h', action='store_true',
                       help='Show help message')
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    brain_parser = subparsers.add_parser('create-brain', help='Create a brain')
    brain_parser.add_argument('--neurons', type=int, default=1000,
                             help='Number of neurons (default: 1000)')
    brain_parser.add_argument('--density', type=float, default=0.1,
                             help='Synapse density (default: 0.1)')
    brain_parser.add_argument('--config', type=str,
                             help='Configuration file to load')
    
    # Create agent simulation command
    agent_parser = subparsers.add_parser('create-agent-simulation', help='Create agent simulation')
    agent_parser.add_argument('--width', type=int, default=20,
                             help='World width (default: 20)')
    agent_parser.add_argument('--height', type=int, default=20,
                             help='World height (default: 20)')
    agent_parser.add_argument('--vision-width', type=int, default=8,
                             help='Vision width (default: 8)')
    agent_parser.add_argument('--vision-height', type=int, default=8,
                             help='Vision height (default: 8)')
    agent_parser.add_argument('--no-learning', action='store_true',
                             help='Disable learning features')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run-simulation', help='Run simulation')
    run_parser.add_argument('--steps', type=int, default=1000,
                           help='Number of simulation steps (default: 1000)')
    
    # Save results command
    save_parser = subparsers.add_parser('save-results', help='Save results to file')
    save_parser.add_argument('filepath', type=str, help='Output file path')
    save_parser.add_argument('--metrics', type=str, help='JSON file with metrics')
    
    # Load configuration command
    config_parser = subparsers.add_parser('load-config', help='Load configuration')
    config_parser.add_argument('filepath', type=str, help='Configuration file path')
    
    # Save brain state command
    save_brain_parser = subparsers.add_parser('save-brain', help='Save brain state')
    save_brain_parser.add_argument('filepath', type=str, help='Output file path')
    
    # Load brain state command
    load_brain_parser = subparsers.add_parser('load-brain', help='Load brain state')
    load_brain_parser.add_argument('filepath', type=str, help='Input file path')
    
    # Status command
    status_parser = subparsers.add_parser('status', help='Show current status')
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command and not args.help:
        parser.print_help()
        return 1
    
    cli = NLMCLI()
    
    try:
        # Handle commands
        if args.command == 'create-brain':
            success = cli.create_brain(args.config, args.neurons, args.density)
            return 0 if success else 1
            
        elif args.command == 'create-agent-simulation':
            success = cli.create_agent_simulation(args.width, args.height,
                                                 args.vision_width, args.vision_height,
                                                 not args.no_learning)
            return 0 if success else 1
            
        elif args.command == 'run-simulation':
            success = cli.run_simulation(args.steps, args.verbose)
            return 0 if success else 1
            
        elif args.command == 'save-results':
            metrics = {}
            if args.metrics:
                try:
                    with open(args.metrics, 'r') as f:
                        metrics = json.load(f)
                except Exception as e:
                    print(f"Error loading metrics file: {e}")
            
            success = cli.save_results(args.filepath, metrics)
            return 0 if success else 1
            
        elif args.command == 'load-config':
            success = cli.load_configuration(args.filepath)
            return 0 if success else 1
            
        elif args.command == 'save-brain':
            success = cli.save_brain_state(args.filepath)
            return 0 if success else 1
            
        elif args.command == 'load-brain':
            success = cli.load_brain_state(args.filepath)
            return 0 if success else 1
            
        elif args.command == 'status':
            cli.print_status()
            return 0
            
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Unexpected error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
