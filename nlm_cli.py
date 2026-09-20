#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive CLI for controlling NLM neural simulations, experiments, and brain management.
"""

import argparse
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any

# Import NLM modules
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm Python bindings not available. Some commands will not work.")

# Version
__version__ = "0.1.0"

# Global configuration
DEFAULT_CONFIG_PATH = "nlm_config.json"
DEFAULT_LOG_LEVEL = "info"


def setup_logging(log_level: str = DEFAULT_LOG_LEVEL):
    """Setup logging configuration."""
    import logging
    
    level_map = {
        'trace': logging.DEBUG,
        'debug': logging.DEBUG,
        'info': logging.INFO,
        'warn': logging.WARNING,
        'error': logging.ERROR
    }
    
    level = level_map.get(log_level.lower(), logging.INFO)
    
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[logging.StreamHandler(sys.stdout)]
    )


class NLMCommandError(Exception):
    """Base exception for NLM command errors."""
    pass


class ConfigManager:
    """Manage NLM configuration files."""
    
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path or DEFAULT_CONFIG_PATH
        self.config = {}
        self.load()
    
    def load(self):
        """Load configuration from file."""
        if os.path.exists(self.config_path):
            try:
                import json
                with open(self.config_path, 'r') as f:
                    self.config = json.load(f)
            except Exception as e:
                raise NLMCommandError(f"Failed to load config file: {e}")
        else:
            self.config = {}
    
    def save(self):
        """Save configuration to file."""
        try:
            import json
            with open(self.config_path, 'w') as f:
                json.dump(self.config, f, indent=2)
        except Exception as e:
            raise NLMCommandError(f"Failed to save config file: {e}")
    
    def get(self, key: str, default=None):
        """Get configuration value by key."""
        keys = key.split('.')
        value = self.config
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        return value
    
    def set(self, key: str, value):
        """Set configuration value by key."""
        keys = key.split('.')
        config = self.config
        
        # Navigate to the right nested level
        for k in keys[:-1]:
            if k not in config:
                config[k] = {}
            config = config[k]
        
        # Set the value
        config[keys[-1]] = value
        self.save()
    
    def list_keys(self, prefix=""):
        """List all configuration keys."""
        keys = []
        
        def _list_keys(config, prefix=""):
            for key, value in config.items():
                full_key = f"{prefix}.{key}" if prefix else key
                
                if isinstance(value, dict):
                    keys.extend(_list_keys(value, full_key))
                else:
                    keys.append(full_key)
            
            return keys
        
        return _list_keys(self.config, prefix)


class BrainManager:
    """Manage NLM brain operations."""
    
    def __init__(self, config: ConfigManager):
        self.config = config
        self.brain = None
    
    def create_brain(self, name: str, neuron_count: int = 1000):
        """Create a new brain."""
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        try:
            # Create configuration
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.neuron_count", neuron_count)
            
            # Create brain
            self.brain = pynlm.createBrain(brain_config)
            self.brain.initialize()
            
            print(f"Brain created with {neuron_count} neurons")
            return True
        except Exception as e:
            raise NLMCommandError(f"Failed to create brain: {e}")
    
    def load_brain(self, filepath: str):
        """Load brain from checkpoint file."""
        if not os.path.exists(filepath):
            raise NLMCommandError(f"Checkpoint file not found: {filepath}")
        
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        try:
            # Create new brain
            brain_config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(brain_config)
            self.brain.initialize()
            
            # Load state
            self.brain.load(filepath)
            print(f"Brain loaded from {filepath}")
            return True
        except Exception as e:
            raise NLMCommandError(f"Failed to load brain: {e}")
    
    def save_brain(self, filepath: str):
        """Save brain to checkpoint file."""
        if not self.brain:
            raise NLMCommandError("No brain loaded")
        
        try:
            self.brain.save(filepath)
            print(f"Brain saved to {filepath}")
            return True
        except Exception as e:
            raise NLMCommandError(f"Failed to save brain: {e}")
    
    def show_stats(self):
        """Show brain statistics."""
        if not self.brain:
            raise NLMCommandError("No brain loaded")
        
        print("=== Brain Statistics ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        
        if self.brain.getWorkingMemory():
            print(f"Working memory traces: {self.brain.getWorkingMemory().getActiveTraces()}")
        
        if self.brain.getEpisodicMemory():
            print(f"Episodic memory episodes: {self.brain.getEpisodicMemory().getEpisodeCount()}")
        
        return True
    
    def reset_brain(self):
        """Reset brain."""
        if not self.brain:
            raise NLMCommandError("No brain loaded")
        
        try:
            self.brain.reset()
            print("Brain reset")
            return True
        except Exception as e:
            raise NLMCommandError(f"Failed to reset brain: {e}")


class SimulationManager:
    """Manage neural simulations."""
    
    def __init__(self, config: ConfigManager, brain_manager: BrainManager):
        self.config = config
        self.brain_manager = brain_manager
    
    def run_simulation(self, steps: int = 1000, dt: float = 0.001, 
                      world: bool = False, agent: bool = False,
                      enable_reward: bool = False, enable_curiosity: bool = False,
                      enable_development: bool = False, checkpoint_interval: int = 0,
                      checkpoint_file: Optional[str] = None, report_interval: int = 0):
        """Run neural simulation."""
        if not self.brain_manager.brain:
            raise NLMCommandError("No brain loaded. Create or load a brain first.")
        
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        print(f"Starting simulation for {steps} steps (dt={dt})")
        
        try:
            # Configure world if requested
            world_obj = None
            agent_obj = None
            
            if world or agent:
                world_obj = pynlm.createSimpleWorld()
                world_width = self.config.get("world.width", 20)
                world_height = self.config.get("world.height", 20)
                vision_width = self.config.get("world.vision_width", 8)
                vision_height = self.config.get("world.vision_height", 8)
                
                world_obj.configure(world_width, world_height, vision_width, vision_height)
                world_obj.reset()
                
                if agent:
                    agent_obj = pynlm.createAgentBrain(self.brain_manager.brain)
                    agent_obj.initialize(world_obj)
                    
                    if enable_reward:
                        agent_obj.enableRewardModulation(True)
                    if enable_curiosity:
                        agent_obj.enableCuriosity(True)
                    if enable_development:
                        agent_obj.enableDevelopment(True)
            
            # Run simulation
            for step in range(steps):
                if world_obj and agent_obj:
                    # Get sensory percept from world
                    percept = world_obj.getSensoryPercept()
                    
                    # Process sensory input in brain
                    agent_obj.processSensoryInput(percept)
                    
                    # Get motor command from brain activity
                    motor_cmd = agent_obj.decodeMotorCommand()
                    
                    # Apply motor command to world
                    world_obj.applyMotorCommand(motor_cmd, world_obj.getSimulationTime())
                    
                    # Update world
                    world_obj.update(dt)
                    
                    # Apply reward modulation
                    if enable_reward:
                        reward = world_obj.getSensoryPercept().getInternal()[0] if world_obj.getSensoryPercept().getInternal() else 0.0
                        agent_obj.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    if enable_development:
                        agent_obj.updateDevelopment(dt)
                
                # Run brain step
                self.brain_manager.brain.step(step, step * dt)
                
                # Save checkpoint if requested
                if checkpoint_interval > 0 and step > 0 and step % checkpoint_interval == 0:
                    if checkpoint_file:
                        filepath = checkpoint_file.replace("{step}", str(step))
                    else:
                        filepath = f"checkpoint_step_{step}.bin"
                    self.brain_manager.save_brain(filepath)
                
                # Report statistics if requested
                if report_interval > 0 and step > 0 and step % report_interval == 0:
                    print(f"Step {step}:")
                    print(f"  Firing neurons: {self.brain_manager.brain.getFiringNeuronCount()}")
                    print(f"  Average firing rate: {self.brain_manager.brain.getAverageFiringRate():.3f}")
                    if agent_obj:
                        print(f"  Curiosity: {agent_obj.getCuriosityLevel():.3f}")
                        print(f"  Novelty: {agent_obj.getNoveltyLevel():.3f}")
            
            print("Simulation completed")
            return True
            
        except Exception as e:
            raise NLMCommandError(f"Simulation failed: {e}")


class ExperimentManager:
    """Manage NLM experiments."""
    
    def __init__(self, config: ConfigManager, brain_manager: BrainManager):
        self.config = config
        self.brain_manager = brain_manager
    
    def run_phase3_demo(self, episodes: int = 5, steps_per_episode: int = 200):
        """Run Phase 3 world interaction demo."""
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        print(f"Running Phase 3 demo with {episodes} episodes, {steps_per_episode} steps each")
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 500)
            config.set("region_count", 2)
            config.set("connection_probability", 0.1f)
            config.set("random_seed", 42u)
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(20.0f, 20.0f, 16, 16)
            world.setRandomSeed(42)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Configure agent
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run episodes
            for episode in range(episodes):
                print(f"--- Episode {episode} ---")
                
                # Reset world
                world.reset()
                agent.reset()
                brain.reset()
                
                # Run steps
                for step in range(steps_per_episode):
                    # Get sensory percept from world
                    percept = world.getSensoryPercept()
                    
                    # Inject sensory input into brain
                    agent.processSensoryInput(percept)
                    
                    # Simulate brain
                    for neural_step in range(10):
                        brain.step(neural_step)
                    
                    # Decode motor command from brain
                    cmd = agent.decodeMotorCommand()
                    
                    # Apply action to world
                    world.applyMotorCommand(cmd, world.getSimulationTime())
                    
                    # Apply reward modulation
                    agent.applyRewardModulation(0.0, 0.0)
                    
                    # Update world
                    world.update(0.01)
                    
                    # Update development
                    agent.updateDevelopment(0.01)
            
            print("Phase 3 demo completed")
            return True
            
        except Exception as e:
            raise NLMCommandError(f"Phase 3 demo failed: {e}")
    
    def run_phase4_experiment(self, trials: int = 50, experiment_type: str = "temporal_prediction"):
        """Run Phase 4 cognitive experiments."""
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        print(f"Running Phase 4 {experiment_type} experiment with {trials} trials")
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 500)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run specific experiment type
            if experiment_type == "temporal_prediction":
                print("Running Temporal Prediction Experiment")
                # This would run the temporal prediction experiment from Phase4Experiment.cpp
                # For now, we'll create a placeholder
                pass
            elif experiment_type == "working_memory":
                print("Running Working Memory Experiment")
                # This would run the working memory experiment from Phase4Experiment.cpp
                pass
            elif experiment_type == "concept_formation":
                print("Running Concept Formation Experiment")
                # This would run the concept formation experiment from Phase4Experiment.cpp
                pass
            else:
                raise NLMCommandError(f"Unknown experiment type: {experiment_type}")
            
            print("Phase 4 experiment completed")
            return True
            
        except Exception as e:
            raise NLMCommandError(f"Phase 4 experiment failed: {e}")
    
    def run_phase6_experiment(self, neurons: int = 500, steps: int = 2000,
                              enable_checkpointing: bool = True,
                              enable_replay: bool = True,
                              enable_development: bool = True):
        """Run Phase 6 integration test."""
        if not PYNLM_AVAILABLE:
            raise NLMCommandError("pynlm Python bindings not available")
        
        print(f"Running Phase 6 integration experiment with {neurons} neurons, {steps} steps")
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.1f)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.initialize(16, 16)
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            total_reward = 0.0
            total_firing_rate = 0.0
            
            for step in range(steps):
                # Get observation
                percept = world.observe(brain.getRegions()[0].get())
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step, step * 0.001)
                
                # Get motor command
                cmd = agent.decodeMotorCommand()
                
                # Apply action to world
                world.applyAction(brain.getRegions()[0].get(), cmd)
                
                # Compute reward
                reward = world.computeReward(brain.getRegions()[0].get())
                total_reward += reward
                
                # Apply reward modulation
                agent.applyRewardModulation(reward, 0.0f)
                
                # Update development
                if enable_development:
                    agent.updateDevelopment(0.001)
                
                # Collect metrics
                total_firing_rate += brain.getAverageFiringRate()
                
                # Periodic status
                if step % 1000 == 0:
                    print(f"Step {step}:")
                    print(f"  Reward: {total_reward / (step + 1):.3f}")
                    print(f"  Firing: {brain.getAverageFiringRate():.3f}")
                    print(f"  WorkingMem: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
            
            # Collect final metrics
            avg_reward = total_reward / steps
            avg_firing_rate = total_firing_rate / steps
            
            print("=== Phase 6 Integration Results ===")
            print(f"Average reward: {avg_reward:.3f}")
            print(f"Average firing rate: {avg_firing_rate:.3f}")
            print(f"Episoδes stored: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
            
            # Test checkpointing if enabled
            if enable_checkpointing:
                checkpoint_path = "./checkpoint_test.bin"
                if brain.save(checkpoint_path):
                    print("Checkpoint saved successfully")
                    
                    # Test loading
                    brain2 = pynlm.createBrain(config)
                    brain2.initialize()
                    
                    if brain2.load(checkpoint_path):
                        print("Checkpoint loaded successfully")
                    else:
                        print("WARNING: Failed to load checkpoint")
                else:
                    print("WARNING: Failed to save checkpoint")
            
            print("Phase 6 experiment completed")
            return True
            
        except Exception as e:
            raise NLMCommandError(f"Phase 6 experiment failed: {e}")


def main():
    """Main entry point for NLM CLI."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm --config my_config.json simulate --steps 1000
  nlm experiment phase3 --episodes 5 --steps-per-episode 200
  nlm brain create --name "my_brain" --neurons 1000
  nlm brain init
  nlm config set brain.neuron_count 1000
        """
    )
    
    parser.add_argument(
        "--config",
        default=DEFAULT_CONFIG_PATH,
        help="Configuration file path (default: nlm_config.json)"
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "--quiet",
        action="store_true",
        help="Suppress non-essential output"
    )
    parser.add_argument(
        "--log-level",
        choices=["trace", "debug", "info", "warn", "error"],
        default=DEFAULT_LOG_LEVEL,
        help="Set logging level (default: info)"
    )
    parser.add_argument(
        "--version",
        action="version",
        version=f"NLM CLI {__version__}"
    )
    
    # Create subparsers
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Initialize subcommands
    setup_init_subparser(subparsers)
    setup_config_subparser(subparsers)
    setup_brain_subparser(subparsers)
    setup_simulate_subparser(subparsers)
    setup_experiment_subparser(subparsers)
    setup_debug_subparser(subparsers)
    setup_admin_subparser(subparsers)
    setup_example_subparser(subparsers)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(args.log_level)
    
    # Create NLM managers
    try:
        config_manager = ConfigManager(args.config)
        brain_manager = BrainManager(config_manager)
        simulation_manager = SimulationManager(config_manager, brain_manager)
        experiment_manager = ExperimentManager(config_manager, brain_manager)
        
        # Execute command
        if args.command == "init":
            execute_init_command(args, config_manager)
        elif args.command == "config":
            execute_config_command(args, config_manager)
        elif args.command == "brain":
            execute_brain_command(args, brain_manager)
        elif args.command == "simulate":
            execute_simulate_command(args, simulation_manager)
        elif args.command == "experiment":
            execute_experiment_command(args, experiment_manager)
        elif args.command == "debug":
            execute_debug_command(args, config_manager)
        elif args.command == "admin":
            execute_admin_command(args, config_manager)
        elif args.command == "example":
            execute_example_command(args, config_manager)
        else:
            parser.error("No command specified or invalid command")
            
    except NLMCommandError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)


def setup_init_subparser(subparsers):
    """Setup the init command subparser."""
    parser = subparsers.add_parser(
        "init",
        help="Initialize NLM project",
        description="Initialize a new NLM project with configuration files"
    )
    parser.add_argument(
        "project_name",
        help="Name of the project directory to create"
    )
    parser.add_argument(
        "--type",
        choices=["demo", "full", "minimal"],
        default="demo",
        help="Type of project to create (default: demo)"
    )


def setup_config_subparser(subparsers):
    """Setup the config command subparser."""
    parser = subparsers.add_parser(
        "config",
        help="Configuration management",
        description="Manage NLM configuration files and settings"
    )
    
    config_subparsers = parser.add_subparsers(dest="config_command", help="Configuration commands")
    
    # Show command
    show_parser = config_subparsers.add_parser(
        "show",
        help="Show current configuration"
    )
    
    # Set command
    set_parser = config_subparsers.add_parser(
        "set",
        help="Set configuration value"
    )
    set_parser.add_argument("key", help="Configuration key (e.g., brain.neuron_count)")
    set_parser.add_argument("value", help="Configuration value")
    
    # Load command
    load_parser = config_subparsers.add_parser(
        "load",
        help="Load configuration from file"
    )
    load_parser.add_argument("filepath", help="Configuration file path")
    
    # Save command
    save_parser = config_subparsers.add_parser(
        "save",
        help="Save configuration to file"
    )
    save_parser.add_argument("filepath", help="Configuration file path")
    
    # List command
    list_parser = config_subparsers.add_parser(
        "list",
        help="List all configuration keys"
    )
    
    # Validate command
    validate_parser = config_subparsers.add_parser(
        "validate",
        help="Validate configuration"
    )


def setup_brain_subparser(subparsers):
    """Setup the brain command subparser."""
    parser = subparsers.add_parser(
        "brain",
        help="Brain manipulation",
        description="Manage neural brains and their states"
    )
    
    brain_subparsers = parser.add_subparsers(dest="brain_command", help="Brain commands")
    
    # Create command
    create_parser = brain_subparsers.add_parser(
        "create",
        help="Create new brain"
    )
    create_parser.add_argument("--name", help="Brain name")
    create_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    
    # Init command
    brain_subparsers.add_parser(
        "init",
        help="Initialize brain"
    )
    
    # Load command
    load_parser = brain_subparsers.add_parser(
        "load",
        help="Load brain from checkpoint"
    )
    load_parser.add_argument("filepath", help="Checkpoint file path")
    
    # Save command
    save_parser = brain_subparsers.add_parser(
        "save",
        help="Save brain to checkpoint"
    )
    save_parser.add_argument("filepath", help="Checkpoint file path")
    
    # Stats command
    brain_subparsers.add_parser(
        "stats",
        help="Show brain statistics"
    )
    
    # Reset command
    brain_subparsers.add_parser(
        "reset",
        help="Reset brain"
    )


def setup_simulate_subparser(subparsers):
    """Setup the simulate command subparser."""
    parser = subparsers.add_parser(
        "simulate",
        help="Run neural simulation",
        description="Run continuous neural simulation with optional world interaction"
    )
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--dt", type=float, default=0.001, help="Time step duration")
    parser.add_argument("--world", action="store_true", help="Enable world simulation")
    parser.add_argument("--agent", action="store_true", help="Enable agent simulation")
    parser.add_argument("--enable-reward", action="store_true", help="Enable reward modulation")
    parser.add_argument("--enable-curiosity", action="store_true", help="Enable curiosity-driven exploration")
    parser.add_argument("--enable-development", action="store_true", help="Enable development")
    parser.add_argument("--checkpoint-interval", type=int, default=0, help="Checkpoint save interval")
    parser.add_argument("--checkpoint-file", help="Checkpoint file path")
    parser.add_argument("--report-interval", type=int, default=0, help="Statistics report interval")
    parser.add_argument("--world-width", type=float, default=20.0, help="World width")
    parser.add_argument("--world-height", type=float, default=20.0, help="World height")


def setup_experiment_subparser(subparsers):
    """Setup the experiment command subparser."""
    parser = subparsers.add_parser(
        "experiment",
        help="Run NLM experiments",
        description="Run predefined NLM experiments and analysis"
    )
    
    experiment_subparsers = parser.add_subparsers(dest="experiment_command", help="Experiment commands")
    
    # Phase 3 command
    phase3_parser = experiment_subparsers.add_parser(
        "phase3",
        help="Run Phase 3 world interaction demo"
    )
    phase3_parser.add_argument("--episodes", type=int, default=5, help="Number of episodes")
    phase3_parser.add_argument("--steps-per-episode", type=int, default=200, help="Steps per episode")
    
    # Phase 4 command
    phase4_parser = experiment_subparsers.add_parser(
        "phase4",
        help="Run Phase 4 cognitive experiments"
    )
    phase4_parser.add_argument("--trials", type=int, default=50, help="Number of trials")
    phase4_parser.add_argument("--experiment", choices=["temporal_prediction", "working_memory", "concept_formation"], 
                             default="temporal_prediction", help="Experiment type")
    
    # Phase 6 command
    phase6_parser = experiment_subparsers.add_parser(
        "phase6",
        help="Run Phase 6 integration test"
    )
    phase6_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    phase6_parser.add_argument("--steps", type=int, default=2000, help="Number of simulation steps")
    phase6_parser.add_argument("--enable-checkpointing", action="store_true", help="Enable checkpointing")
    phase6_parser.add_argument("--enable-replay", action="store_true", help="Enable replay")
    phase6_parser.add_argument("--enable-development", action="store_true", help="Enable development")


def setup_debug_subparser(subparsers):
    """Setup the debug command subparser."""
    parser = subparsers.add_parser(
        "debug",
        help="Debugging tools",
        description="Advanced debugging and analysis tools for NLM development"
    )
    
    debug_subparsers = parser.add_subparsers(dest="debug_command", help="Debug commands")
    
    # Profile command
    profile_parser = debug_subparsers.add_parser(
        "profile",
        help="Monitor system performance"
    )
    profile_parser.add_argument("--seconds", type=int, default=30, help="Monitoring duration")
    profile_parser.add_argument("--output", help="Output file path")
    
    # Visualize command
    visualize_parser = debug_subparsers.add_parser(
        "visualize",
        help="Visualize brain connectivity"
    )
    visualize_parser.add_argument("--output", help="Output file path")
    visualize_parser.add_argument("--type", choices=["connectivity", "spikes", "trajectories"], 
                                 default="connectivity", help="Visualization type")
    
    # Spikes command
    spikes_parser = debug_subparsers.add_parser(
        "spikes",
        help="Analyze spike patterns"
    )
    spikes_parser.add_argument("--threshold", type=float, default=0.5, help="Activation threshold")
    spikes_parser.add_argument("--window", type=int, default=1000, help="Analysis window")
    
    # Memory command
    memory_parser = debug_subparsers.add_parser(
        "memory",
        help="Check memory usage"
    )
    memory_parser.add_argument("--monitor", action="store_true", help="Enable continuous monitoring")


def setup_admin_subparser(subparsers):
    """Setup the admin command subparser."""
    parser = subparsers.add_parser(
        "admin",
        help="Administrative functions",
        description="Administrative and maintenance functions"
    )
    
    admin_subparsers = parser.add_subparsers(dest="admin_command", help="Admin commands")
    
    # Checkpoints command
    checkpoints_parser = admin_subparsers.add_parser(
        "checkpoints",
        help="Manage checkpoints"
    )
    
    checkpoints_subparsers = checkpoints_parser.add_subparsers(dest="checkpoints_command", help="Checkpoint commands")
    
    checkpoints_subparsers.add_parser(
        "list",
        help="List all checkpoints"
    )
    
    clean_parser = checkpoints_subparsers.add_parser(
        "clean",
        help="Clean old checkpoints"
    )
    clean_parser.add_argument("--older-than", help="Clean checkpoints older than this duration")
    
    # Backup command
    backup_parser = admin_subparsers.add_parser(
        "backup",
        help="Create backup"
    )
    backup_parser.add_argument("--output", help="Backup file path")
    
    # Restore command
    restore_parser = admin_subparsers.add_parser(
        "restore",
        help="Restore system"
    )
    restore_parser.add_argument("backup_file", help="Backup file to restore")
    
    # Status command
    admin_subparsers.add_parser(
        "status",
        help="Show system status"
    )


def setup_example_subparser(subparsers):
    """Setup the example command subparser."""
    parser = subparsers.add_parser(
        "example",
        help="Generate examples",
        description="Generate example configurations, scripts, and demos"
    )
    
    example_subparsers = parser.add_subparsers(dest="example_command", help="Example commands")
    
    # Config command
    config_parser = example_subparsers.add_parser(
        "config",
        help="Generate example config"
    )
    config_parser.add_argument("--output", help="Output file path")
    config_parser.add_argument("--template", choices=["standard", "minimal", "advanced"], 
                             default="standard", help="Template type")
    
    # Python command
    python_parser = example_subparsers.add_parser(
        "python",
        help="Generate Python script"
    )
    python_parser.add_argument("--output", help="Output file path")
    
    # Demo command
    demo_parser = example_subparsers.add_parser(
        "demo",
        help="Generate demo script"
    )
    demo_parser.add_argument("--type", choices=["phase3", "phase6"], default="phase3", help="Demo type")
    demo_parser.add_argument("--output", help="Output file path")
    
    # Tutorial command
    tutorial_parser = example_subparsers.add_parser(
        "tutorial",
        help="Generate complete tutorial"
    )
    tutorial_parser.add_argument("--output", help="Output file path")


# Execute command functions (stubs - to be implemented)

def execute_init_command(args, config_manager):
    """Execute the init command."""
    print(f"Initializing project: {args.project_name}")
    print(f"Project type: {args.type}")
    # Implementation will be added


def execute_config_command(args, config_manager):
    """Execute the config command."""
    if not args.config_command:
        print("NLM Configuration Management")
        print("=" * 30)
        print("Available commands: show, set, load, save, list, validate")
        return
    
    if args.config_command == "show":
        print("Current configuration:")
        for key in config_manager.list_keys():
            value = config_manager.get(key)
            print(f"  {key}: {value}")
    
    elif args.config_command == "set":
        config_manager.set(args.key, args.value)
        print(f"Set {args.key} = {args.value}")
    
    elif args.config_command == "load":
        config_manager = ConfigManager(args.filepath)
        print(f"Loaded configuration from {args.filepath}")
    
    elif args.config_command == "save":
        config_manager.save()
        print(f"Saved configuration to {args.filepath}")
    
    elif args.config_command == "list":
        print("Configuration keys:")
        for key in config_manager.list_keys():
            print(f"  {key}")
    
    elif args.config_command == "validate":
        print("Configuration validation (stub)")
        # Implementation will be added


def execute_brain_command(args, brain_manager):
    """Execute the brain command."""
    if not args.brain_command:
        print("NLM Brain Management")
        print("=" * 20)
        print("Available commands: create, init, load, save, stats, reset")
        return
    
    if args.brain_command == "create":
        brain_manager.create_brain(args.name, args.neurons)
    
    elif args.brain_command == "init":
        if brain_manager.brain:
            brain_manager.brain.initialize()
            print("Brain initialized")
        else:
            print("No brain loaded. Create a brain first.")
    
    elif args.brain_command == "load":
        brain_manager.load_brain(args.filepath)
    
    elif args.brain_command == "save":
        brain_manager.save_brain(args.filepath)
    
    elif args.brain_command == "stats":
        brain_manager.show_stats()
    
    elif args.brain_command == "reset":
        brain_manager.reset_brain()


def execute_simulate_command(args, simulation_manager):
    """Execute the simulate command."""
    simulation_manager.run_simulation(
        steps=args.steps,
        dt=args.dt,
        world=args.world,
        agent=args.agent,
        enable_reward=args.enable_reward,
        enable_curiosity=args.enable_curiosity,
        enable_development=args.enable_development,
        checkpoint_interval=args.checkpoint_interval,
        checkpoint_file=args.checkpoint_file,
        report_interval=args.report_interval
    )


def execute_experiment_command(args, experiment_manager):
    """Execute the experiment command."""
    if not args.experiment_command:
        print("NLM Experiments")
        print("=" * 15)
        print("Available commands: phase3, phase4, phase6")
        return
    
    if args.experiment_command == "phase3":
        experiment_manager.run_phase3_demo(args.episodes, args.steps_per_episode)
    
    elif args.experiment_command == "phase4":
        experiment_manager.run_phase4_experiment(args.trials, args.experiment)
    
    elif args.experiment_command == "phase6":
        experiment_manager.run_phase6_experiment(
            neurons=args.neurons,
            steps=args.steps,
            enable_checkpointing=args.enable_checkpointing,
            enable_replay=args.enable_replay,
            enable_development=args.enable_development
        )


def execute_debug_command(args, config_manager):
    """Execute the debug command."""
    if not args.debug_command:
        print("NLM Debug Tools")
        print("=" * 15)
        print("Available commands: profile, visualize, spikes, memory")
        return
    
    print(f"Debug command: {args.debug_command} (stub)")


def execute_admin_command(args, config_manager):
    """Execute the admin command."""
    if not args.admin_command:
        print("NLM Administration")
        print("=" * 18)
        print("Available commands: checkpoints, backup, restore, status")
        return
    
    if args.admin_command == "checkpoints":
        if not args.checkpoints_command:
            print("Checkpoint management commands:")
            print("  list - List all checkpoints")
            print("  clean - Clean old checkpoints")
            return
        
        if args.checkpoints_command == "list":
            print("Checkpoint list (stub)")
        
        elif args.checkpoints_command == "clean":
            print("Cleaning old checkpoints (stub)")
    
    elif args.admin_command == "backup":
        print("Creating backup (stub)")
    
    elif args.admin_command == "restore":
        print("Restoring system (stub)")
    
    elif args.admin_command == "status":
        print("System status (stub)")


def execute_example_command(args, config_manager):
    """Execute the example command."""
    if not args.example_command:
        print("NLM Examples")
        print("=" * 12)
        print("Available commands: config, python, demo, tutorial")
        return
    
    print(f"Example command: {args.example_command} (stub)")


if __name__ == "__main__":
    main()