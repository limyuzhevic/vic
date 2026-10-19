#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface

This script provides advanced commands for development users to manage and inspect
the NLM (Neural Learning Machine) neural simulation framework.

Usage:
    python nlm_cli.py [command] [options]

Commands:
    config      - Configuration management
    checkpoint  - Checkpoint management  
    inspect     - Brain state inspection
    development - Development system control
    memory      - Memory system management
    simulation  - Simulation control
    debug       - Debugging and monitoring
    world       - World management
    help        - Show help information

Examples:
    python nlm_cli.py config list
    python nlm_cli.py checkpoint save manual_checkpoint
    python nlm_cli.py inspect brain-state --detailed
    python nlm_cli.py development advance-stage
    python nlm_cli.py memory search --keyword "learning"
"""

import sys
import os
import json
import argparse
from typing import Dict, Any, List
from pathlib import Path

# Add current directory to Python path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install dependencies first.")
    print("Run: pip install -e .")
    sys.exit(1)

class NLMCommandLineInterface:
    def __init__(self):
        self.brain = None
        self.config = None
        self.agent = None
        self.world = None
        
    def create_brain(self, config_file=None):
        """Create a new brain with configuration"""
        self.config = pynlm.createDefaultConfig()
        
        if config_file and os.path.exists(config_file):
            try:
                self.config.loadFromFile(config_file)
                print(f"✓ Loaded configuration from: {config_file}")
            except Exception as e:
                print(f"⚠ Warning: Could not load config file: {e}")
                print("Using default configuration")
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        print(f"✓ Created brain with {self.brain.getTotalNeuronCount()} neurons")
        return self.brain
        
    def config_command(self, args):
        """Handle configuration management commands"""
        if args.subcommand == "list":
            self._list_config()
        elif args.subcommand == "get":
            self._get_config(args.key)
        elif args.subcommand == "set":
            self._set_config(args.key, args.value)
        elif args.subcommand == "save":
            self._save_config(args.filename)
        elif args.subcommand == "load":
            self._load_config(args.filename)
        elif args.subcommand == "validate":
            self._validate_config()
        elif args.subcommand == "template":
            self._show_config_template()
            
    def checkpoint_command(self, args):
        """Handle checkpoint management commands"""
        if args.subcommand == "list":
            self._list_checkpoints()
        elif args.subcommand == "save":
            self._save_checkpoint(args.name)
        elif args.subcommand == "load":
            self._load_checkpoint(args.name)
        elif args.subcommand == "delete":
            self._delete_checkpoint(args.name)
        elif args.subcommand == "stats":
            self._checkpoint_stats(args.name)
            
    def inspect_command(self, args):
        """Handle brain state inspection commands"""
        if args.subcommand == "brain-state":
            self._inspect_brain_state(args.detailed)
        elif args.subcommand == "memory":
            self._inspect_memory(args.memory_type)
        elif args.subcommand == "neuromodulation":
            self._inspect_neuromodulation()
        elif args.subcommand == "development":
            self._inspect_development()
        elif args.subcommand == "prediction":
            self._inspect_prediction()
            
    def development_command(self, args):
        """Handle development system commands"""
        if args.subcommand == "advance-stage":
            self._advance_development_stage()
        elif args.subcommand == "plasticity":
            self._modify_plasticity_rates(args.type, args.value)
        elif args.subcommand == "trigger-event":
            self._trigger_development_event(args.event)
        elif args.subcommand == "set-goals":
            self._set_development_goals(args.goals)
            
    def simulation_command(self, args):
        """Handle simulation control commands"""
        if args.subcommand == "run":
            self._run_simulation(args.steps)
        elif args.subcommand == "pause":
            self._pause_simulation()
        elif args.subcommand == "resume":
            self._resume_simulation()
        elif args.subcommand == "jump":
            self._jump_to_step(args.step)
        elif args.subcommand == "record":
            self._record_simulation(args.filename)
        elif args.subcommand == "replay":
            self._replay_simulation(args.filename)
            
    def memory_command(self, args):
        """Handle memory system commands"""
        if args.subcommand == "search":
            self._search_memories(args.type, args.query)
        elif args.subcommand == "consolidate":
            self._consolidate_memories()
        elif args.subcommand == "replay":
            self._replay_memories(args.episode_id)
        elif args.subcommand == "stats":
            self._memory_statistics()
            
    def debug_command(self, args):
        """Handle debugging and monitoring commands"""
        if args.subcommand == "monitor":
            self._start_monitoring(args.interval)
        elif args.subcommand == "profile":
            self._profile_system(args.duration)
        elif args.subcommand == "validate":
            self._validate_system()
        elif args.subcommand == "analyze":
            self._analyze_network()
            
    def world_command(self, args):
        """Handle world management commands"""
        if args.subcommand == "add":
            self._add_world_object(args.type, args.x, args.y, args.value)
        elif args.subcommand == "remove":
            self._remove_world_object(args.x, args.y)
        elif args.subcommand == "set-agent":
            self._set_agent_start(args.x, args.y)
        elif args.subcommand == "validate":
            self._validate_world()
            
    # Configuration management methods
    def _list_config(self):
        """List all configuration parameters"""
        print("=== Configuration Parameters ===")
        if self.config:
            keys = self.config.getKeys()
            for key in sorted(keys):
                print(f"  {key}")
        else:
            print("No configuration loaded")
            
    def _get_config(self, key):
        """Get configuration parameter value"""
        if self.config and self.config.has(key):
            # Try to get as different types
            try:
                value = self.config.getOrInt(key, 0)
                print(f"{key}: {value} (int)")
            except:
                try:
                    value = self.config.getOrFloat(key, 0.0)
                    print(f"{key}: {value} (float)")
                except:
                    try:
                        value = self.config.getOrBool(key, False)
                        print(f"{key}: {value} (bool)")
                    except:
                        print(f"{key}: (unknown type)")
        else:
            print(f"Parameter '{key}' not found")
            
    def _set_config(self, key, value):
        """Set configuration parameter value"""
        try:
            # Try to parse as int, then float, then bool
            try:
                int_value = int(value)
                self.config.set(key, int_value)
                print(f"✓ Set {key} = {int_value}")
            except ValueError:
                try:
                    float_value = float(value)
                    self.config.set(key, float_value)
                    print(f"✓ Set {key} = {float_value}")
                except ValueError:
                    bool_value = value.lower() in ('true', '1', 'yes', 'on')
                    self.config.set(key, bool_value)
                    print(f"✓ Set {key} = {bool_value}")
        except Exception as e:
            print(f"✗ Error setting {key}: {e}")
            
    def _save_config(self, filename):
        """Save configuration to file"""
        try:
            if self.config:
                self.config.saveToFile(filename)
                print(f"✓ Configuration saved to: {filename}")
            else:
                print("No configuration to save")
        except Exception as e:
            print(f"✗ Error saving configuration: {e}")
            
    def _load_config(self, filename):
        """Load configuration from file"""
        try:
            if self.config:
                self.config.loadFromFile(filename)
                print(f"✓ Configuration loaded from: {filename}")
            else:
                self.config = pynlm.createDefaultConfig()
                self.config.loadFromFile(filename)
                print(f"✓ Configuration loaded from: {filename}")
        except Exception as e:
            print(f"✗ Error loading configuration: {e}")
            
    def _validate_config(self):
        """Validate configuration"""
        print("=== Configuration Validation ===")
        if self.config:
            print("✓ Configuration is loaded")
            
            # Check for required parameters
            required_params = ["brain.neuron_count", "brain.synapse_density"]
            missing = []
            for param in required_params:
                if not self.config.has(param):
                    missing.append(param)
                    
            if missing:
                print(f"⚠ Missing parameters: {', '.join(missing)}")
            else:
                print("✓ All required parameters present")
                
            # Show configuration summary
            print("\nConfiguration summary:")
            print(self.config.summary())
        else:
            print("No configuration to validate")
            
    def _show_config_template(self):
        """Show configuration template"""
        print("=== Configuration Template ===")
        print("""
# NLM Configuration File Template

# Brain parameters
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.tau_mem = 20.0

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation parameters
dopamine_baseline = 0.0

# Simulation parameters
simulation_timestep = 0.001
max_simulation_steps = 10000
""")
        
    # Placeholder methods for other commands
    def _save_checkpoint(self, name):
        print(f"Saving checkpoint: {name}")
        if self.brain:
            checkpoint_file = f"checkpoint_{name}.bin"
            if self.brain.save(checkpoint_file):
                print(f"✓ Checkpoint saved to: {checkpoint_file}")
            else:
                print(f"✗ Failed to save checkpoint: {checkpoint_file}")
        else:
            print("No brain to save")
            
    def _load_checkpoint(self, name):
        print(f"Loading checkpoint: {name}")
        checkpoint_file = f"checkpoint_{name}.bin"
        if os.path.exists(checkpoint_file):
            if self.brain:
                if self.brain.load(checkpoint_file):
                    print(f"✓ Checkpoint loaded from: {checkpoint_file}")
                else:
                    print(f"✗ Failed to load checkpoint: {checkpoint_file}")
        else:
            print(f"✗ Checkpoint file not found: {checkpoint_file}")
            
    def _list_checkpoints(self):
        print("=== Checkpoints ===")
        checkpoint_dir = "checkpoints"
        if os.path.exists(checkpoint_dir):
            for file in os.listdir(checkpoint_dir):
                if file.endswith(".bin"):
                    filepath = os.path.join(checkpoint_dir, file)
                    size = os.path.getsize(filepath)
                    print(f"  {file} ({size} bytes)")
        else:
            print("No checkpoint directory found")
            
    def _delete_checkpoint(self, name):
        checkpoint_file = f"checkpoint_{name}.bin"
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
            print(f"✓ Deleted checkpoint: {checkpoint_file}")
        else:
            print(f"✗ Checkpoint not found: {checkpoint_file}")
            
    def _checkpoint_stats(self, name):
        print(f"Checkpoint statistics for {name}")
        
    def _inspect_brain_state(self, detailed):
        print("=== Brain State Inspection ===")
        if self.brain:
            print(f"Neurons: {self.brain.getTotalNeuronCount()}")
            print(f"Synapses: {self.brain.getTotalSynapseCount()}")
            print(f"Regions: {self.brain.getRegionCount()}")
            print(f"Active neurons: {self.brain.getFiringNeuronCount()}")
            print(f"Average firing rate: {self.brain.getAverageFiringRate():.3f}")
            print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
            
            if detailed:
                print("\n=== Detailed Statistics ===")
                print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
                print(f"Development stage: {self.brain.getDevelopmentalStage()}")
                
                # Get brain state vector for neural planner
                state = self.brain.getCurrentBrainState()
                print(f"Brain state vector length: {len(state)}")
                
                # Get last planned action if available
                if hasattr(self.brain, 'getLastPlannedAction'):
                    last_action = self.brain.getLastPlannedAction()
                    print(f"Last planned action: {last_action}")
                    
                # Get last concept ID if available
                if hasattr(self.brain, 'getLastConceptId'):
                    last_concept = self.brain.getLastConceptId()
                    print(f"Last concept ID: {last_concept}")
        else:
            print("No brain to inspect")
            
    def _inspect_memory(self, memory_type):
        print(f"=== Memory Inspection: {memory_type} ===")
        # Placeholder implementation
        print(f"Memory inspection for {memory_type} not yet implemented")
        
    def _inspect_neuromodulation(self):
        print("=== Neuromodulation Inspection ===")
        # Placeholder implementation
        print("Neuromodulation inspection not yet implemented")
        
    def _inspect_development(self):
        print("=== Development Inspection ===")
        if self.brain:
            print(f"Development stage: {self.brain.getDevelopmentalStage()}")
            
            # Try to access development system
            if hasattr(self.brain, 'getDevelopmentSystem'):
                dev_system = self.brain.getDevelopmentSystem()
                print(f"Development system: {dev_system}")
        else:
            print("No brain to inspect")
            
    def _inspect_prediction(self):
        print("=== Prediction Inspection ===")
        # Placeholder implementation
        print("Prediction inspection not yet implemented")
        
    # Additional placeholder methods for other commands
    def _advance_development_stage(self):
        print("Advancing development stage...")
        
    def _modify_plasticity_rates(self, type, value):
        print(f"Modifying {type} plasticity rate to {value}")
        
    def _trigger_development_event(self, event):
        print(f"Triggering development event: {event}")
        
    def _set_development_goals(self, goals):
        print(f"Setting development goals: {goals}")
        
    def _run_simulation(self, steps):
        print(f"Running simulation for {steps} steps...")
        
    def _pause_simulation(self):
        print("Pausing simulation...")
        
    def _resume_simulation(self):
        print("Resuming simulation...")
        
    def _jump_to_step(self, step):
        print(f"Jumping to step: {step}")
        
    def _record_simulation(self, filename):
        print(f"Recording simulation to: {filename}")
        
    def _replay_simulation(self, filename):
        print(f"Replaying simulation from: {filename}")
        
    def _search_memories(self, type, query):
        print(f"Searching {type} memories for: {query}")
        
    def _consolidate_memories(self):
        print("Consolidating memories...")
        
    def _replay_memories(self, episode_id):
        print(f"Replaying memory episode: {episode_id}")
        
    def _memory_statistics(self):
        print("=== Memory Statistics ===")
        # Placeholder implementation
        print("Memory statistics not yet implemented")
        
    def _start_monitoring(self, interval):
        print(f"Starting monitoring with interval: {interval}")
        
    def _profile_system(self, duration):
        print(f"Profiling system for: {duration}")
        
    def _validate_system(self):
        print("=== System Validation ===")
        print("System validation not yet implemented")
        
    def _analyze_network(self):
        print("=== Network Analysis ===")
        print("Network analysis not yet implemented")
        
    def _add_world_object(self, type, x, y, value):
        print(f"Adding {type} at ({x}, {y}) with value {value}")
        
    def _remove_world_object(self, x, y):
        print(f"Removing world object at ({x}, {y})")
        
    def _set_agent_start(self, x, y):
        print(f"Setting agent start position to ({x}, {y})")
        
    def _validate_world(self):
        print("=== World Validation ===")
        print("World validation not yet implemented")
        
    def run_full_example(self):
        """Run a complete example with all systems"""
        print("=== Running Full NLM Integration Example ===")
        
        # Create brain with configuration
        self.create_brain()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Create agent brain
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Run simulation
        print("\nRunning simulation...")
        for step in range(100):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input in brain
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Print progress
            if step % 20 == 0:
                print(f"Step {step}: {self.brain.getFiringNeuronCount()} neurons firing, "
                      f"curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"novelty: {self.agent.getNoveltyLevel():.3f}, "
                      f"dev stage: {self.brain.getDevelopmentalStage()}")
        
        print("\n=== Simulation Complete ===")
        print(f"Final statistics:")
        print(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Final firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  Final curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Final developmental stage: {self.brain.getDevelopmentalStage()}")
        
        if hasattr(self.brain, 'getLastPlannedAction'):
            last_action = self.brain.getLastPlannedAction()
            print(f"  Last planned action: {last_action}")
            
        if hasattr(self.brain, 'getLastConceptId'):
            last_concept = self.brain.getLastConceptId()
            print(f"  Last concept ID: {last_concept}")


def main():
    # Create CLI interface
    cli = NLMCommandLineInterface()
    
    # Parse command line arguments
    if len(sys.argv) < 2:
        print("Usage: python nlm_cli.py [command] [options]")
        print("\nAvailable commands:")
        print("  config      - Configuration management")
        print("  checkpoint  - Checkpoint management")
        print("  inspect     - Brain state inspection")
        print("  development - Development system control")
        print("  memory      - Memory system management")
        print("  simulation  - Simulation control")
        print("  debug       - Debugging and monitoring")
        print("  world       - World management")
        print("  help        - Show help information")
        print("\nFor more information about a specific command, run:")
        print("  python nlm_cli.py help [command]")
        sys.exit(1)
    
    command = sys.argv[1]
    
    # Handle help command
    if command == "help":
        if len(sys.argv) > 2:
            subcommand = sys.argv[2]
            print(f"Help for command: {subcommand}")
            print("(Detailed help for subcommands will be available)")
        else:
            print("=== NLM CLI Help ===")
            print("Available commands: config, checkpoint, inspect, development, memory, simulation, debug, world")
            print("\nEach command can be used with subcommands (e.g., python nlm_cli.py config list)")
        sys.exit(0)
    
    # Parse command and delegate to appropriate handler
    parser = argparse.ArgumentParser(description='NLM Advanced Command Line Interface')
    parser.add_argument('command', choices=['config', 'checkpoint', 'inspect', 'development', 'memory', 'simulation', 'debug', 'world'])
    parser.add_argument('subcommand', help='Subcommand for the specified command')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    parser.add_argument('--output', '-o', help='Output file for results')
    
    try:
        args = parser.parse_args(sys.argv[1:3])
    except SystemExit:
        print(f"\nUsage: python nlm_cli.py {args.command} [subcommand] [options]")
        print(f"For command: {args.command}, available subcommands include:")
        if args.command == 'config':
            print("  list, get, set, save, load, validate, template")
        elif args.command == 'checkpoint':
            print("  list, save, load, delete, stats")
        elif args.command == 'inspect':
            print("  brain-state, memory, neuromodulation, development, prediction")
        elif args.command == 'development':
            print("  advance-stage, plasticity, trigger-event, set-goals")
        elif args.command == 'memory':
            print("  search, consolidate, replay, stats")
        elif args.command == 'simulation':
            print("  run, pause, resume, jump, record, replay")
        elif args.command == 'debug':
            print("  monitor, profile, validate, analyze")
        elif args.command == 'world':
            print("  add, remove, set-agent, validate")
        print("\nRun examples:")
        print(f"  python nlm_cli.py {args.command} list")
        print(f"  python nlm_cli.py {args.command} save my_checkpoint")
        print(f"  python nlm_cli.py {args.command} --verbose --output results.json")
        sys.exit(1)
    
    # Call the appropriate command handler
    if args.command == 'config':
        cli.config_command(args)
    elif args.command == 'checkpoint':
        cli.checkpoint_command(args)
    elif args.command == 'inspect':
        cli.inspect_command(args)
    elif args.command == 'development':
        cli.development_command(args)
    elif args.command == 'memory':
        cli.memory_command(args)
    elif args.command == 'simulation':
        cli.simulation_command(args)
    elif args.command == 'debug':
        cli.debug_command(args)
    elif args.command == 'world':
        cli.world_command(args)
    
    # Run full example if requested
    if '--example' in sys.argv:
        print("\n" + "="*60)
        cli.run_full_example()
        print("="*60)


if __name__ == '__main__':
    main()
