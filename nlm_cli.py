#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Command Line Interface

Advanced command-line interface for NLM neural simulation framework.
Provides comprehensive control over brain simulations, experiments,
and system management.

Usage:
    nlm [command] [options]

Commands:
    run           Run a brain simulation
    experiment    Run an experiment
    config        Configuration management
    checkpoint    Checkpoint/save state management
    info          Display system information
    benchmark     Run performance benchmarks
    profile        Run profiling
    help          Show help information
"""

import sys
import os
import argparse
import json
from pathlib import Path
from typing import Optional, Dict, Any

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm as nlm
except ImportError:
    print("Error: pynlm module not found. Please install the NLM Python bindings.")
    print("Run: pip install .")
    sys.exit(1)
class NLMCLI:
    """Main CLI class for NLM operations."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        
    def create_default_config(self, **kwargs) -> nlm.Config:
        """Create a default configuration with optional overrides."""
        config = nlm.createDefaultConfig()
        
        # Default configuration values
        defaults = {
            "brain.neuron_count": 1000,
            "brain.region_count": 2,
            "brain.connection_probability": 0.1,
            "simulation.timestep": 0.001,
            "random.seed": 42,
            "environment.name": "GridWorld",
            "environment.width": 10,
            "environment.height": 10,
            "log.level": "INFO",
            "log.to_file": False,
            "log.filename": "nlm.log",
            "max.steps": 10000,
            "visualization.enabled": False,
        }
        
        # Apply defaults
        for key, value in defaults.items():
            config.set(key, value)
            
        # Apply overrides
        for key, value in kwargs.items():
            if key.startswith("nlm."):
                key = key[4:]  # Remove nlm. prefix
            config.set(key, value)
            
        return config
    
    def create_brain(self, config_path: Optional[str] = None, **kwargs) -> nlm.Brain:
        """Create and initialize a brain."""
        try:
            # Load configuration from file if specified
            if config_path:
                config = nlm.createDefaultConfig()
                if config.loadFromFile(config_path):
                    print(f"Loaded configuration from: {config_path}")
                else:
                    print(f"Warning: Could not load configuration from {config_path}, using defaults")
            else:
                config = self.create_default_config(**kwargs)
                
            # Create brain
            brain = nlm.createBrain(config)
            brain.initialize()
            
            print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions")
            print(f"Total synapses: {brain.getTotalSynapseCount()}")
            
            return brain
            
        except Exception as e:
            print(f"Error creating brain: {e}")
            raise
    
    def create_simulation_environment(self, width: int = 20, height: int = 20) -> nlm.SimpleWorld:
        """Create a simple world environment."""
        world = nlm.createSimpleWorld()
        world.configure(width, height, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(width // 2, height // 2)
        print(f"Created world environment: {width}x{height}")
        return world
    
    def create_agent(self, brain: nlm.Brain, world: nlm.SimpleWorld) -> nlm.AgentBrain:
        """Create an agent brain interface."""
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        print("Agent created and initialized")
        return agent
    
    def run_simulation(self, steps: int, config_path: Optional[str] = None, 
                      world: bool = False, brain_only: bool = False, **kwargs) -> Dict[str, Any]:
        """Run a simulation."""
        print(f"Starting simulation: {steps} steps")
        
        # Create brain
        self.brain = self.create_brain(config_path, **kwargs)
        
        results = {
            'total_steps': steps,
            'total_spikes': 0,
            'firing_neurons': 0,
            'avg_firing_rate': 0.0,
            'total_reward': 0.0,
            'max_spike_count': 0,
            'start_time': None,
            'end_time': None,
            'completed_steps': 0
        }
        
        if world:
            # Create world and agent
            self.world = self.create_simulation_environment()
            self.agent = self.create_agent(self.brain, self.world)
            
            # Enable learning systems
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            
            # Run simulation with world interaction
            for step in range(steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Get action from brain
                action = self.agent.decodeMotorCommand()
                
                # Apply action to world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect metrics
                results['total_spikes'] += self.brain.getTotalSpikeCount()
                results['firing_neurons'] += self.brain.getFiringNeuronCount()
                results['total_reward'] += reward
                
                if step % 100 == 0:
                    print(f"Step {step}: Spikes={self.brain.getTotalSpikeCount()}, "
                          f"Firing={self.brain.getFiringNeuronCount()}, "
                          f"Reward={reward:.3f}")
        
        elif brain_only:
            # Run simulation without world (brain-only processing)
            for step in range(steps):
                self.brain.step(step)
                
                if step % 100 == 0:
                    print(f"Step {step}: Spikes={self.brain.getTotalSpikeCount()}, "
                          f"Firing={self.brain.getFiringNeuronCount()}")
        
        # Calculate final statistics
        results['avg_firing_rate'] = (results['firing_neurons'] / results['total_steps']) if results['total_steps'] > 0 else 0.0
        results['max_spike_count'] = self.brain.getTotalSpikeCount()
        
        print(f"\nSimulation completed: {steps} steps")
        print(f"Total spikes: {results['total_spikes']}")
        print(f"Average firing rate: {results['avg_firing_rate']:.3f} Hz")
        print(f"Total reward: {results['total_reward']:.3f}")
        
        return results
    
    def save_checkpoint(self, filepath: str) -> bool:
        """Save brain state to checkpoint file."""
        if not self.brain:
            print("Error: No brain created. Create a brain first.")
            return False
            
        try:
            print(f"Saving checkpoint to: {filepath}")
            if self.brain.save(filepath):
                print("Checkpoint saved successfully")
                return True
            else:
                print("Error: Failed to save checkpoint")
                return False
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state from checkpoint file."""
        if not self.brain:
            print("Error: No brain created. Create a brain first.")
            return False
            
        try:
            print(f"Loading checkpoint from: {filepath}")
            if self.brain.load(filepath):
                print("Checkpoint loaded successfully")
                return True
            else:
                print("Error: Failed to load checkpoint")
                return False
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def display_brain_status(self):
        """Display detailed brain status information."""
        if not self.brain:
            print("Error: No brain created. Create a brain first.")
            return
            
        print("=== NLM Brain Status ===")
        self.brain.logStatus()
        
        # Additional Python-specific status
        if self.agent:
            print("\n=== Agent Status ===")
            print(f"Reward modulation: {'Enabled' if self.agent.isRewardModulationEnabled() else 'Disabled'}")
            print(f"Curiosity: {'Enabled' if self.agent.isCuriosityEnabled() else 'Disabled'}")
            print(f"Development: {'Enabled' if self.agent.isDevelopmentEnabled() else 'Disabled'}")
            print(f"Structural plasticity: {'Enabled' if self.agent.isStructuralPlasticityEnabled() else 'Disabled'}")
            print(f"Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"Curiosity level: {self.agent.getCuriosityLevel():.3f}")
            print(f"Novelty level: {self.agent.getNoveltyLevel():.3f}")
            print(f"Prediction error: {self.agent.getPredictionError():.3f}")
            print(f"Developmental stage: {self.agent.getDevelopmentalStage()}")
            
        if self.world:
            print("\n=== World Status ===")
            print(f"Width: {self.world.getWidth()}, Height: {self.world.getHeight()}")
            print(f"Simulation time: {self.world.getSimulationTime():.3f}")
            print(f"Max energy: {self.world.getMaxEnergy()}")
            body = self.world.getAgentBody()
            if body:
                print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
                print(f"Agent orientation: {body.orientation:.1f}°")
                print(f"Agent energy: {body.energy:.1f}/{body.health:.1f}")
    
    def run_experiment(self, experiment_name: str, config_path: Optional[str] = None, 
                      **kwargs) -> Dict[str, Any]:
        """Run an experiment."""
        print(f"Running experiment: {experiment_name}")
        
        # For now, create a simple experiment
        # In a full implementation, this would load experiment definitions
        config = self.create_default_config(**kwargs)
        
        # Run the simulation
        results = self.run_simulation(
            steps=config.getOr("max.steps", 1000),
            config_path=config_path,
            **kwargs
        )
        
        results['experiment'] = experiment_name
        results['config_path'] = config_path
        
        return results
    
    def list_config_keys(self):
        """List all available configuration keys."""
        if not self.config:
            print("Error: No configuration loaded. Create a config first.")
            return
            
        print("Available configuration keys:")
        keys = self.config.getKeys()
        for key in sorted(keys):
            print(f"  {key}")
    
    def set_config_value(self, key: str, value: str):
        """Set a configuration value."""
        if not self.config:
            print("Error: No configuration loaded. Create a config first.")
            return
            
        try:
            # Try to parse value as int, float, or bool
            if value.lower() in ['true', 'false']:
                value_bool = value.lower() == 'true'
                self.config.set(key, value_bool)
            elif '.' in value:
                try:
                    value_float = float(value)
                    self.config.set(key, value_float)
                except ValueError:
                    self.config.set(key, value)
            else:
                try:
                    value_int = int(value)
                    self.config.set(key, value_int)
                except ValueError:
                    self.config.set(key, value)
                    
            print(f"Set {key} = {value}")
        except Exception as e:
            print(f"Error setting configuration: {e}")
    
    def cleanup(self):
        """Clean up resources."""
        if self.agent:
            self.agent.reset()
            
        if self.brain:
            self.brain.reset()
            
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        print("Cleaned up resources")
def main():
    """Main CLI entry point."""
    cli = NLMCLI()
    
    # Define commands and their arguments
    subparsers = [
        ('run', 'Run a simulation', 'run_simulation'),
        ('experiment', 'Run an experiment', 'run_experiment'),
        ('checkpoint', 'Checkpoint management', 'checkpoint'),
        ('info', 'Display system information', 'display_info'),
        ('config', 'Configuration management', 'config'),
        ('help', 'Show help information', 'show_help'),
    ]
    
    # For simplicity, we'll use argparse directly
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Global arguments
    parser.add_argument('--version', action='store_true', help='Show version information')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
    
    # Create subparsers
    subparsers_obj = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Run command
    run_parser = subparsers_obj.add_parser('run', help='Run a simulation')
    run_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    run_parser.add_argument('--config', help='Configuration file path')
    run_parser.add_argument('--world', action='store_true', help='Run simulation with world interaction')
    run_parser.add_argument('--brain-only', action='store_true', help='Run simulation without world')
    
    # Experiment command
    exp_parser = subparsers_obj.add_parser('experiment', help='Run an experiment')
    exp_parser.add_argument('name', help='Experiment name')
    exp_parser.add_argument('--config', help='Configuration file path')
    exp_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    
    # Checkpoint command
    checkpoint_parser = subparsers_obj.add_parser('checkpoint', help='Checkpoint management')
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest='action', help='Checkpoint action')
    
    save_parser = checkpoint_subparsers.add_parser('save', help='Save checkpoint')
    save_parser.add_argument('filepath', help='Checkpoint file path')
    
    load_parser = checkpoint_subparsers.add_parser('load', help='Load checkpoint')
    load_parser.add_argument('filepath', help='Checkpoint file path')
    
    # Info command
    info_parser = subparsers_obj.add_parser('info', help='Display system information')
    info_parser.add_argument('--verbose', '-v', action='store_true', help='Show verbose information')
    
    # Config command
    config_parser = subparsers_obj.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='action', help='Configuration action')
    
    list_parser = config_subparsers.add_parser('list', help='List configuration keys')
    
    set_parser = config_subparsers.add_parser('set', help='Set configuration value')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')
    
    # Help command
    help_parser = subparsers_obj.add_parser('help', help='Show help information')
    
    # Parse arguments
    args = parser.parse_args()
    
    try:
        if args.command == 'run':
            results = cli.run_simulation(
                steps=args.steps,
                config_path=args.config,
                world=args.world,
                brain_only=args.brain_only
            )
            
            # Save results to file
            if args.verbose:
                print("\nSimulation results:")
                for key, value in results.items():
                    print(f"  {key}: {value}")
                    
        elif args.command == 'experiment':
            results = cli.run_experiment(
                experiment_name=args.name,
                config_path=args.config,
                **{'max.steps': args.steps}
            )
            
        elif args.command == 'checkpoint':
            if args.action == 'save':
                if not cli.save_checkpoint(args.filepath):
                    sys.exit(1)
            elif args.action == 'load':
                if not cli.load_checkpoint(args.filepath):
                    sys.exit(1)
                
        elif args.command == 'info':
            cli.display_brain_status()
            
        elif args.command == 'config':
            if args.action == 'list':
                cli.list_config_keys()
            elif args.action == 'set':
                cli.set_config_value(args.key, args.value)
                
        elif args.command == 'help':
            parser.print_help()
            
        elif args.version:
            print("NLM (Neural Learning Machine) - Version 0.1.0")
            print("Phase 6 - Final Integration")
            
        else:
            parser.print_help()
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        cli.cleanup()
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        cli.cleanup()
        sys.exit(1)
    
    # Cleanup
    cli.cleanup()
if __name__ == '__main__':
    main()