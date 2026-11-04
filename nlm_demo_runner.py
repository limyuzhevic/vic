#!/usr/bin/env python3
"""
Unified NLM Demo Runner

This script provides a unified interface for running NLM demos with consistent
command-line arguments, configuration management, and output formatting.
"""

import sys
import os
from pathlib import Path

# Add the src directory to the path so we can import NLM modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

# Import NLM modules
try:
    from core.Config.Config import Config
    from core.Logger.Logger import Logger
    from brain.Brain import Brain
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from sensory.SensoryPercept import SensoryPercept
    from motor.MotorCommand import MotorCommand
    from motor.Action import Action
    from environment.ActionResult import ActionResult
    from neuromodulation.Novelty import Novelty
    from neuromodulation.Curiosity import Curiosity
    from neuromodulation.Dopamine import Dopamine
    from neuromodulation.Reward import Reward
    from neuromodulation.PredictionError import PredictionError
    from prediction.PredictionSystem import PredictionSystem
    from prediction.NeuralPrediction import NeuralPrediction
    from memory.NeuralWorkingMemory import NeuralWorkingMemory
    from memory.NeuralEpisodicMemory import NeuralEpisodicMemory
    from cognition.ConceptFormation import ConceptFormation
    from cognition.NeuralPlanner import NeuralPlanner
    from development.DevelopmentSystem import DevelopmentSystem
    from development.Synaptogenesis import Synaptogenesis
    from development.Pruning import Pruning
    from development.Maturation import Maturation
    from dynamics.NeuralDynamics import NeuralDynamics
    from dynamics.SpikeSystem import SpikeSystem
    from plasticity.PlasticityRule import PlasticityRule
    from plasticity.Hebbian import Hebbian
    from plasticity.STDP import STDP
    from plasticity.StructuralPlasticity import StructuralPlasticity
    from visualization.VisualizationInterface import VisualizationInterface

    NLM_MODULES_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import all NLM modules: {e}")
    NLM_MODULES_AVAILABLE = False


def create_default_config():
    """Create a default NLM configuration."""
    config = Config()
    
    # Core parameters
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Plasticity parameters
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    config.set("stdp_time_constant", 20.0)
    config.set("plasticity_learning_rate", 0.01)
    
    # Development parameters
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    
    # Neuromodulation parameters
    config.set("dopamine_baseline", 0.0)
    config.set("reward_discount_factor", 0.99)
    
    # Environment parameters
    config.set("environment_name", "GridWorld")
    config.set("environment_width", 10)
    config.set("environment_height", 10)
    
    # Logging parameters
    config.set("log_level", "INFO")
    config.set("log_to_file", False)
    config.set("log_filename", "nlm.log")
    
    # Simulation parameters
    config.set("max_simulation_steps", 10000)
    config.set("simulation_time_limit", 0.0)
    
    # Visualization parameters
    config.set("visualization_enabled", False)
    config.set("visualization_update_rate", 30.0)
    
    return config


def print_banner():
    """Print the NLM banner."""
    print("=" * 70)
    print("                    NLM — 熙然 Neural Learning Machine")
    print("                   An Experimental Artificial Developmental Brain")
    print("=" * 70)
    print()


def print_config_summary(config):
    """Print a summary of the configuration."""
    print("Configuration Summary:")
    print("-" * 40)
    
    key_params = [
        ("neuron_count", "Neurons"),
        ("region_count", "Regions"),
        ("connection_probability", "Connection Probability"),
        ("environment_width", "World Width"),
        ("environment_height", "World Height"),
        ("vision_width", "Vision Width"),
        ("vision_height", "Vision Height"),
        ("simulation_timestep", "Time Step"),
    ]
    
    for key, name in key_params:
        if config.has(key):
            value = config.getOr(key, 0)
            print(f"  {name:25s}: {value}")
    
    print()


def initialize_logging(config):
    """Initialize the logging system."""
    logger = Logger()
    
    # Set log level
    log_level = config.getOr("log_level", "INFO")
    if log_level == "DEBUG":
        logger.setLevel(LogLevel::Debug)
    elif log_level == "WARN":
        logger.setLevel(LogLevel::Warn)
    elif log_level == "ERROR":
        logger.setLevel(LogLevel::Error)
    else:
        logger.setLevel(LogLevel::Info)
    
    # Add console logger
    console_logger = ConsoleLogger(logger.getLevel())
    logger.addLogger(console_logger)
    
    # Set global logger
    Logger::setGlobal(logger)
    
    return logger


def parse_command_line_args():
    """Parse command line arguments."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Unified NLM Demo Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                           # Run with default configuration
  %(prog)s --episodes 10           # Run 10 episodes
  %(prog)s --steps 500              # Run 500 steps per episode
  %(prog)s --brain-size 2000         # Create brain with 2000 neurons
  %(prog)s --config config.json      # Load configuration from file
  %(prog)s --save-config my_config.json  # Save configuration after run
  %(prog)s --help                    # Show this help message

Configuration can be specified in:
  - Command line arguments
  - Config file (JSON or key=value format)
  - Default values

For detailed help on specific demos, see individual demo documentation.
        """
    )
    
    # Core simulation parameters
    parser.add_argument("--episodes", type=int, default=5,
                       help="Number of episodes to run (default: 5)")
    parser.add_argument("--steps", type=int, default=200,
                       help="Steps per episode (default: 200)")
    parser.add_argument("--timestep", type=float, default=0.1,
                       help="World timestep in seconds (default: 0.1)")
    
    # Brain configuration
    parser.add_argument("--brain-size", type=int, default=1000,
                       help="Number of neurons in the brain (default: 1000)")
    parser.add_argument("--regions", type=int, default=2,
                       help="Number of brain regions (default: 2)")
    parser.add_argument("--connection-prob", type=float, default=0.1,
                       help="Connection probability (default: 0.1)")
    parser.add_argument("--seed", type=int, default=42,
                       help="Random seed (default: 42)")
    
    # World configuration
    parser.add_argument("--world-width", type=float, default=10.0,
                       help="World width (default: 10.0)")
    parser.add_argument("--world-height", type=float, default=10.0,
                       help="World height (default: 10.0)")
    parser.add_argument("--vision-width", type=int, default=8,
                       help="Vision width (default: 8)")
    parser.add_argument("--vision-height", type=int, default=8,
                       help="Vision height (default: 8)")
    
    # Feature toggles
    parser.add_argument("--reward-modulation", action="store_true", default=True,
                       help="Enable reward modulation (default: enabled)")
    parser.add_argument("--structural-plasticity", action="store_true", default=True,
                       help="Enable structural plasticity (default: enabled)")
    parser.add_argument("--development", action="store_true", default=True,
                       help="Enable development (default: enabled)")
    parser.add_argument("--curiosity", action="store_true", default=True,
                       help="Enable curiosity (default: enabled)")
    
    # File and output options
    parser.add_argument("--config", type=str,
                       help="Load configuration from file")
    parser.add_argument("--save-config", type=str,
                       help="Save configuration after run")
    parser.add_argument("--output-dir", type=str, default=".",
                       help="Output directory for logs and results")
    parser.add_argument("--log-level", choices=["DEBUG", "INFO", "WARN", "ERROR"],
                       default="INFO", help="Log level (default: INFO)")
    
    # Performance and debugging
    parser.add_argument("--neural-steps", type=int, default=10,
                       help="Neural steps per world step (default: 10)")
    parser.add_argument("--enable-checkpointing", action="store_true",
                       help="Enable checkpointing")
    parser.add_argument("--checkpoint-path", type=str, default="/tmp/nlm_checkpoint",
                       help="Checkpoint file path (default: /tmp/nlm_checkpoint)")
    
    return parser


def create_world(config):
    """Create and configure the world."""
    world = SimpleWorld()
    
    # Configure world
    world.configure(
        config.getOr("environment_width", 10.0),
        config.getOr("environment_height", 10.0),
        config.getOr("vision_width", 8),
        config.getOr("vision_height", 8)
    )
    
    # Set random seed
    world.setRandomSeed(config.getOr("random_seed", 42))
    
    return world


def create_agent(brain, world, config):
    """Create and configure the agent."""
    agent = AgentBrain(brain)
    agent.initialize(world)
    
    # Enable features based on configuration
    if config.getOr("reward_modulation", True):
        agent.enableRewardModulation(True)
    
    if config.getOr("structural_plasticity", True):
        agent.enableStructuralPlasticity(True)
    
    if config.getOr("development", True):
        agent.enableDevelopment(True)
    
    if config.getOr("curiosity", True):
        agent.enableCuriosity(True)
    
    return agent


def run_standard_loop(brain, world, agent, config):
    """Run the standard world interaction loop."""
    num_episodes = config.getOr("episodes", 5)
    steps_per_episode = config.getOr("steps", 200)
    neural_steps_per_action = config.getOr("neural_steps", 10)
    timestep = config.getOr("timestep", 0.1)
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        # Reset
        world.reset()
        agent.reset()
        brain.reset()
        
        # Run episode
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(steps_per_episode):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run neural processing
            for _ in range(neural_steps_per_action):
                brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(timestep)
            
            # Track episode statistics
            episode_reward += result.reward
            episode_steps += 1
            
            # Print periodic updates
            if step % 50 == 0:
                print(f"  Step {step}: reward={result.reward:.3f}, "
                      f"action={action}, neuromod={agent.getNeuromodulationLevel():.3f}")
        
        # Episode summary
        print(f"Episode {episode + 1} Summary:")
        print(f"  Total Reward: {episode_reward:.3f}")
        print(f"  Average Reward: {episode_reward/max(steps_per_episode, 1):.3f}")
        print(f"  Steps Completed: {episode_steps}")
        print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
        print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
    
    print("\n=== Standard Loop Complete ===")


def save_final_config(config, output_path):
    """Save the final configuration to file."""
    if not config:
        return
    
    try:
        output_file = Path(output_path) / "nlm_config.json"
        
        # Convert config to serializable format
        config_data = {}
        for key in config.getKeys():
            value = config.getOr(key, "")
            config_data[key] = value
        
        with open(output_file, 'w') as f:
            import json
            json.dump(config_data, f, indent=2)
        
        print(f"\nConfiguration saved to: {output_file}")
        
    except Exception as e:
        print(f"Warning: Could not save configuration: {e}")


def main():
    """Main function."""
    if not NLM_MODULES_AVAILABLE:
        print("Error: NLM modules are not available. Please check your installation.")
        return 1
    
    # Parse command line arguments
    parser = parse_command_line_args()
    args = parser.parse_args()
    
    # Print banner
    print_banner()
    
    # Create configuration
    config = create_default_config()
    
    # Load configuration from file if specified
    if args.config:
        try:
            config.loadFromFile(args.config)
            print(f"Configuration loaded from: {args.config}")
        except Exception as e:
            print(f"Warning: Could not load configuration file: {e}")
    
    # Override with command line arguments
    config.set("neuron_count", args.brain_size)
    config.set("region_count", args.regions)
    config.set("connection_probability", args.connection_prob)
    config.set("random_seed", args.seed)
    config.set("environment_width", args.world_width)
    config.set("environment_height", args.world_height)
    
    # Print configuration summary
    print_config_summary(config)
    
    # Initialize logging
    logger = initialize_logging(config)
    
    # Create world
    print("Creating world...")
    world = create_world(config)
    
    # Create brain
    print("Creating brain...")
    brain = Brain(config)
    if not brain.initialize():
        print("Error: Failed to initialize brain!")
        return 1
    
    print("Brain initialized successfully!")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    
    # Create agent
    print("Creating agent...")
    agent = create_agent(brain, world, config)
    
    print("Agent created successfully!")
    
    # Run standard loop
    print("\nStarting main loop...")
    run_standard_loop(brain, world, agent, config)
    
    # Save configuration if requested
    if args.save_config:
        save_final_config(config, args.output_dir)
    
    # Print final status
    print("\n=== DEMO COMPLETE ===")
    print("Final brain status:")
    brain.logStatus()
    
    return 0


if __name__ == "__main__":
    sys.exit(main())