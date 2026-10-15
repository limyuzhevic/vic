#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A comprehensive command-line interface for the NLM (Neural Learning Machine) project.
Provides tools for running simulations, managing configurations, and exploring brain states.

Usage:
    python nlm_cli.py <command> [options]

Commands:
    create           Create a new brain or world
    simulate         Run a simulation
    explore          Explore brain state and statistics
    config           Manage configuration files
    checkpoint       Manage brain checkpoints
    demo             Run built-in demos
    help             Show help information
"""

import argparse
import sys
import os
import json
from pathlib import Path
from typing import Optional, Dict, Any, List

# Try to import pynlm, handle import errors gracefully
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    PYNLM_ERROR = str(e)

class NLMCLI:
    """Command-line interface for NLM."""
    
    def __init__(self):
        self.brain = None
        self.agent = None
        self.world = None
    
    def create(self, args):
        """Create NLM components."""
        if not args.component:
            print("Error: --component is required")
            return False
        
        component = args.component.lower()
        
        try:
            if component == "brain":
                config_file = getattr(args, 'config_file', None)
                if config_file and os.path.exists(config_file):
                    self.brain = self._create_brain_with_config(config_file)
                else:
                    self.brain = self._create_default_brain()
                print(f"✓ Created brain with {self.brain.getTotalNeuronCount()} neurons")
                
            elif component == "agent":
                if not self.brain:
                    print("Error: Agent requires a brain. Create a brain first.")
                    return False
                self.agent = pynlm.createAgentBrain(self.brain)
                print("✓ Created agent brain interface")
                
            elif component == "world":
                self.world = pynlm.createSimpleWorld()
                if args.width and args.height:
                    self.world.configure(args.width, args.height, 
                                       getattr(args, 'vision_width', 8), 
                                       getattr(args, 'vision_height', 8))
                print("✓ Created simple world")
                
            elif component == "config":
                config = pynlm.createDefaultConfig()
                if args.config_name:
                    config.saveToFile(args.config_name)
                    print(f"✓ Saved default config to {args.config_name}")
                else:
                    print("Config created (not saved)")
                
            else:
                print(f"Error: Unknown component '{component}'")
                return False
                
        except Exception as e:
            print(f"Error creating {component}: {e}")
            return False
        
        return True
    
    def _create_default_brain(self):
        """Create a default brain with standard configuration."""
        config = pynlm.createDefaultConfig()
        
        # Override with command line arguments if provided
        if self.args.neuron_count:
            config.set("brain.neuron_count", self.args.neuron_count)
        if self.args.region_count:
            config.set("brain.region_count", self.args.region_count)
        if self.args.connection_prob:
            config.set("brain.connection_probability", self.args.connection_prob)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    def _create_brain_with_config(self, config_file):
        """Create a brain using a configuration file."""
        config = pynlm.createDefaultConfig()
        if config.loadFromFile(config_file):
            print(f"✓ Loaded configuration from {config_file}")
        else:
            print(f"Warning: Could not load {config_file}, using defaults")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    def simulate(self, args):
        """Run a simulation."""
        if not self.brain:
            print("Error: No brain created. Create a brain first.")
            return False
        
        # Create agent and world if needed
        if not self.agent:
            self.agent = pynlm.createAgentBrain(self.brain)
            self.world = pynlm.createSimpleWorld()
            self.world.configure(20, 20, 8, 8)
            self.world.reset()
            self.agent.initialize(self.world)
        
        # Enable learning systems if requested
        if args.enable_reward_modulation:
            self.agent.enableRewardModulation(True)
        if args.enable_curiosity:
            self.agent.enableCuriosity(True)
        if args.enable_development:
            self.agent.enableDevelopment(True)
        
        print(f"Starting simulation for {args.steps} steps...")
        
        try:
            for step in range(args.steps):
                # Update world
                self.world.update(args.timestep)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Get action and apply to world
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation if enabled
                if args.enable_reward_modulation:
                    reward = 0.0
                    if percept.getInternal() and len(percept.getInternal()) > 0:
                        reward = percept.getInternal()[0]
                    self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                if args.enable_development:
                    self.agent.updateDevelopment(args.timestep)
                
                # Print progress
                if (step + 1) % args.progress_interval == 0 or step == 0:
                    print(f"  Step {step + 1}/{args.steps}: "
                          f"Firing neurons: {self.brain.getFiringNeuronCount()}, "
                          f"Total spikes: {self.brain.getTotalSpikeCount()}")
            
            print(f"✓ Simulation completed successfully!")
            print(f"  Final stats: {self.brain.getTotalSpikeCount()} total spikes, "
                  f"{self.brain.getFiringNeuronCount()} firing neurons")
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return False
        
        return True
    
    def explore(self, args):
        """Explore brain state and statistics."""
        if not self.brain:
            print("Error: No brain available. Create a brain first.")
            return False
        
        print("=== Brain State Exploration ===\n")
        
        # Basic brain information
        print("1. Basic Statistics:")
        print(f"   Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   Active neurons: {self.brain.getActiveNeuronCount()}")
        print(f"   Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"   Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"   E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        
        # Developmental stage
        stage = self.brain.getDevelopmentalStage()
        print(f"   Developmental stage: {stage}")
        
        # Memory systems (if accessible)
        print("\n2. Memory Systems:")
        if hasattr(self.brain, 'getWorkingMemory') and self.brain.getWorkingMemory():
            wm = self.brain.getWorkingMemory()
            print(f"   Working memory capacity: {wm.getCapacity()}")
            print(f"   Active traces: {wm.getActiveTraces()}")
        
        # Neuromodulation systems
        print("\n3. Neuromodulation Systems:")
        dopamine = self.brain.getDopamine() if hasattr(self.brain, 'getDopamine') else None
        if dopamine:
            print(f"   Dopamine level: {dopamine.getLevel():.3f}")
        
        curiosity = self.brain.getCuriosity() if hasattr(self.brain, 'getCuriosity') else None
        if curiosity:
            print(f"   Curiosity level: {curiosity.getLevel():.3f}")
        
        # Regions
        print("\n4. Brain Regions:")
        for region in self.brain.getRegions():
            print(f"   Region {region.getId().index()} ({region.getName()}): "
                  f"{region.getTotalNeuronCount()} neurons, "
                  f"{region.getSynapseCount()} synapses")
        
        return True
    
    def config(self, args):
        """Manage configuration files."""
        if args.action == "create":
            return self._create_config(args)
        elif args.action == "list":
            return self._list_configs(args)
        elif args.action == "view":
            return self._view_config(args)
        elif args.action == "set":
            return self._set_config_value(args)
        else:
            print(f"Error: Unknown config action '{args.action}'")
            return False
    
    def _create_config(self, args):
        """Create a configuration file."""
        config = pynlm.createDefaultConfig()
        
        # Apply overrides from command line
        if args.neuron_count:
            config.set("brain.neuron_count", args.neuron_count)
        if args.region_count:
            config.set("brain.region_count", args.region_count)
        
        filename = args.filename if args.filename else "nlm_config.json"
        config.saveToFile(filename)
        print(f"✓ Created configuration file: {filename}")
        return True
    
    def _list_configs(self, args):
        """List available configuration files."""
        config_dir = args.directory if args.directory else "."
        if not os.path.exists(config_dir):
            print(f"Error: Directory '{config_dir}' does not exist")
            return False
        
        config_files = [f for f in os.listdir(config_dir) if f.endswith('.cfg') or f.endswith('.json')]
        
        if not config_files:
            print("No configuration files found")
            return True
        
        print(f"Configuration files in '{config_dir}':")
        for f in config_files:
            print(f"  - {f}")
        
        return True
    
    def _view_config(self, args):
        """View configuration file contents."""
        if not args.filename:
            print("Error: --filename is required for viewing config")
            return False
        
        config = pynlm.createDefaultConfig()
        if config.loadFromFile(args.filename):
            print(f"Contents of {args.filename}:")
            print(config.summary())
        else:
            print(f"Error: Could not load configuration from {args.filename}")
            return False
        
        return True
    
    def _set_config_value(self, args):
        """Set a configuration value."""
        if not args.key or not args.value:
            print("Error: --key and --value are required")
            return False
        
        config = pynlm.createDefaultConfig()
        # Parse value based on key type
        try:
            if "neuron_count" in args.key or "region_count" in args.key or "steps" in args.key:
                value = int(args.value)
            elif "probability" in args.key or "rate" in args.key or "timestep" in args.key:
                value = float(args.value)
            else:
                value = args.value
            
            config.set(args.key, value)
            print(f"✓ Set configuration '{args.key}' = {value}")
            
        except ValueError as e:
            print(f"Error: Invalid value '{args.value}' for key '{args.key}': {e}")
            return False
        
        return True
    
    def checkpoint(self, args):
        """Manage brain checkpoints."""
        if not self.brain:
            print("Error: No brain available. Create a brain first.")
            return False
        
        if args.action == "save":
            return self._save_checkpoint(args)
        elif args.action == "load":
            return self._load_checkpoint(args)
        elif args.action == "list":
            return self._list_checkpoints(args)
        else:
            print(f"Error: Unknown checkpoint action '{args.action}'")
            return False
    
    def _save_checkpoint(self, args):
        """Save brain state to checkpoint."""
        filename = args.filename if args.filename else "brain_checkpoint.bin"
        
        try:
            self.brain.save(filename)
            print(f"✓ Saved brain state to: {filename}")
            return True
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    def _load_checkpoint(self, args):
        """Load brain state from checkpoint."""
        filename = args.filename
        if not filename:
            print("Error: --filename is required for loading checkpoint")
            return False
        
        if not os.path.exists(filename):
            print(f"Error: Checkpoint file '{filename}' does not exist")
            return False
        
        try:
            self.brain.load(filename)
            print(f"✓ Loaded brain state from: {filename}")
            return True
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def _list_checkpoints(self, args):
        """List available checkpoint files."""
        checkpoint_dir = args.directory if args.directory else "."
        if not os.path.exists(checkpoint_dir):
            print(f"Error: Directory '{checkpoint_dir}' does not exist")
            return False
        
        checkpoint_files = [f for f in os.listdir(checkpoint_dir) if f.endswith('.bin') or f.endswith('.chk')]
        
        if not checkpoint_files:
            print("No checkpoint files found")
            return True
        
        print(f"Checkpoint files in '{checkpoint_dir}':")
        for f in checkpoint_files:
            print(f"  - {f}")
        
        return True
    
    def demo(self, args):
        """Run built-in demonstrations."""
        demo_name = args.demo.lower() if args.demo else "phase6"
        
        demos = {
            "phase6": self._run_phase6_demo,
            "basic": self._run_basic_demo,
            "learning": self._run_learning_demo,
            "agent": self._run_agent_demo,
        }
        
        if demo_name not in demos:
            print(f"Error: Unknown demo '{demo_name}'")
            print("Available demos:", ", ".join(demos.keys()))
            return False
        
        return demos[demo_name](args)
    
    def _run_phase6_demo(self, args):
        """Run Phase 6 integration demo."""
        print("=== NLM Phase 6 Integration Demo ===")
        print("This demo tests all integrated brain systems...")
        
        # This would call the C++ Phase6Demo, but for now we'll simulate
        # In a real implementation, this would invoke the C++ executable
        print("Phase 6 demo would run here...")
        print("(In the full implementation, this would run the C++ demo)")
        
        return True
    
    def _run_basic_demo(self, args):
        """Run basic brain demonstration."""
        print("=== Basic NLM Demo ===")
        print("Creating a simple brain and running basic tests...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test basic connectivity
        for step in range(min(100, args.steps or 100)):
            brain.step(step)
        
        print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
        return True
    
    def _run_learning_demo(self, args):
        """Run learning demonstration."""
        print("=== NLM Learning Demo ===")
        print("Demonstrating neural learning and adaptation...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        print("Enabled reward modulation and curiosity")
        
        # Run learning simulation
        for step in range(min(500, args.steps or 500)):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"  Step {step}: Dopamine={brain.getDopamine().getLevel() if hasattr(brain, 'getDopamine') else 'N/A'}")
        
        print("Learning demo complete!")
        return True
    
    def _run_agent_demo(self, args):
        """Run agent-environment interaction demo."""
        print("=== NLM Agent Demo ===")
        print("Demonstrating agent in a simulated environment...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent.initialize(world)
        
        print("Created agent in 20x20 world with 8x8 vision")
        
        # Run agent simulation
        for step in range(min(200, args.steps or 200)):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if (step + 1) % 50 == 0:
                print(f"  Step {step + 1}: Action={action}, "
                      f"Firing neurons={brain.getFiringNeuronCount()}")
        
        print("Agent demo complete!")
        return True

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_cli.py create --component=brain --neuron-count=500
  python nlm_cli.py simulate --steps=1000 --enable-reward-modulation
  python nlm_cli.py explore
  python nlm_cli.py config create --filename=my_config.json --neuron-count=1000
  python nlm_cli.py demo basic --steps=200
  python nlm_cli.py help create
        """
    )
    
    # Top-level command
    parser.add_argument(
        'command',
        choices=['create', 'simulate', 'explore', 'config', 'checkpoint', 'demo', 'help'],
        help='Command to execute'
    )
    
    # Global options
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose output'
    )
    
    # Create NLMCLI instance
    cli = NLMCLI()
    cli.args = parser.parse_args()  # Store args for use in methods
    
    # Set up subcommands
    create_parser = parser.add_parser('create', help='Create NLM components')
    create_parser.add_argument(
        '--component', '-c',
        required=True,
        choices=['brain', 'agent', 'world', 'config'],
        help='Component to create'
    )
    create_parser.add_argument(
        '--config-file', '-f',
        help='Configuration file for brain creation'
    )
    create_parser.add_argument(
        '--neuron-count',
        type=int,
        help='Number of neurons for brain creation'
    )
    create_parser.add_argument(
        '--region-count',
        type=int,
        help='Number of brain regions'
    )
    create_parser.add_argument(
        '--connection-prob',
        type=float,
        help='Connection probability between neurons'
    )
    
    simulate_parser = parser.add_parser('simulate', help='Run a simulation')
    simulate_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of simulation steps (default: 1000)'
    )
    simulate_parser.add_argument(
        '--timestep', '-t',
        type=float,
        default=0.1,
        help='Simulation timestep (default: 0.1)'
    )
    simulate_parser.add_argument(
        '--progress-interval', '-p',
        type=int,
        default=100,
        help='Progress reporting interval (default: 100)'
    )
    simulate_parser.add_argument(
        '--enable-reward-modulation',
        action='store_true',
        help='Enable reward modulation system'
    )
    simulate_parser.add_argument(
        '--enable-curiosity',
        action='store_true',
        help='Enable curiosity-driven exploration'
    )
    simulate_parser.add_argument(
        '--enable-development',
        action='store_true',
        help='Enable developmental system'
    )
    
    explore_parser = parser.add_parser('explore', help='Explore brain state')
    
    config_parser = parser.add_parser('config', help='Manage configurations')
    config_subparsers = config_parser.add_subparsers(dest='action', required=True)
    
    config_create_parser = config_subparsers.add_parser('create', help='Create configuration')
    config_create_parser.add_argument('--filename', '-f', help='Configuration filename')
    config_create_parser.add_argument('--neuron-count', type=int, help='Number of neurons')
    config_create_parser.add_argument('--region-count', type=int, help='Number of regions')
    
    config_list_parser = config_subparsers.add_parser('list', help='List configurations')
    config_list_parser.add_argument('--directory', '-d', default='.', help='Configuration directory')
    
    config_view_parser = config_subparsers.add_parser('view', help='View configuration')
    config_view_parser.add_argument('--filename', '-f', required=True, help='Configuration filename')
    
    config_set_parser = config_subparsers.add_parser('set', help='Set configuration value')
    config_set_parser.add_argument('--key', required=True, help='Configuration key')
    config_set_parser.add_argument('--value', required=True, help='Configuration value')
    
    checkpoint_parser = parser.add_parser('checkpoint', help='Manage checkpoints')
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest='action', required=True)
    
    checkpoint_save_parser = checkpoint_subparsers.add_parser('save', help='Save checkpoint')
    checkpoint_save_parser.add_argument('--filename', '-f', help='Checkpoint filename')
    
    checkpoint_load_parser = checkpoint_subparsers.add_parser('load', help='Load checkpoint')
    checkpoint_load_parser.add_argument('--filename', '-f', required=True, help='Checkpoint filename')
    
    checkpoint_list_parser = checkpoint_subparsers.add_parser('list', help='List checkpoints')
    checkpoint_list_parser.add_argument('--directory', '-d', default='.', help='Checkpoint directory')
    
    demo_parser = parser.add_parser('demo', help='Run demonstrations')
    demo_parser.add_argument('demo', nargs='?', default='phase6', help='Demo to run')
    demo_parser.add_argument('--steps', '-s', type=int, help='Number of steps')
    
    help_parser = parser.add_parser('help', help='Show help')
    help_parser.add_argument('topic', nargs='?', help='Help topic')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle help command
    if args.command == 'help' and not args.topic:
        parser.print_help()
        return
    
    if args.command == 'help':
        # Show help for specific topic
        topics = {
            'create': create_parser,
            'simulate': simulate_parser,
            'explore': explore_parser,
            'config': config_parser,
            'checkpoint': checkpoint_parser,
            'demo': demo_parser,
        }
        
        if args.topic in topics:
            topics[args.topic].print_help()
        else:
            print(f"Unknown help topic: {args.topic}")
        return
    
    # Execute command
    if not PYNLM_AVAILABLE:
        print(f"Error: pynlm is not available: {PYNLM_ERROR}")
        print("Please install the pynlm Python bindings:")
        print("  pip install -e .")
        return 1
    
    try:
        if args.command == 'create':
            success = cli.create(args)
        elif args.command == 'simulate':
            success = cli.simulate(args)
        elif args.command == 'explore':
            success = cli.explore(args)
        elif args.command == 'config':
            success = cli.config(args)
        elif args.command == 'checkpoint':
            success = cli.checkpoint(args)
        elif args.command == 'demo':
            success = cli.demo(args)
        else:
            print(f"Error: Unknown command '{args.command}'")
            success = False
        
        return 0 if success else 1
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())