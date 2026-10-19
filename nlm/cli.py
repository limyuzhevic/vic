#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) CLI Interface

Command-line interface for NLM neural simulation framework.
Provides convenient access to NLM functionality from the command line.
"""

import argparse
import sys
from pathlib import Path

# Add the current directory to Python path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import nlm
except ImportError as e:
    print(f"Error importing NLM: {e}")
    print("Make sure the nlm package is properly installed")
    sys.exit(1)
def create_brain(config_file=None):
    """
    Create a new brain with configuration.
    
    Args:
        config_file: Optional configuration file path
    """
    if config_file and Path(config_file).exists():
        config = nlm.Config()
        if config.load_from_file(config_file):
            print(f"Configuration loaded from {config_file}")
        else:
            print(f"Warning: Could not load config file {config_file}, using defaults")
            config = nlm.create_default_config()
    else:
        config = nlm.create_default_config()
    
    brain = nlm.createBrain(config)
    print("Brain created successfully")
    return brain
def initialize_brain(brain):
    """Initialize a brain for simulation."""
    if brain.initialize():
        print("Brain initialized successfully")
        return True
    else:
        print("Brain initialization failed")
        return False
def run_simulation(brain, steps):
    """
    Run a simulation with the given brain for specified steps.
    
    Args:
        brain: Brain instance
        steps: Number of simulation steps to run
    """
    print(f"Running simulation for {steps} steps")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Step {step}/{steps}")
    
    print(f"Simulation completed after {steps} steps")
    
    # Show summary
    brain.logStatus()
    
    return True
def run_complete_example(brain, world, agent, steps):
    """
    Run a complete example with brain, world, and agent.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: AgentBrain instance
        steps: Number of simulation steps to run
    """
    print(f"Running complete example for {steps} steps")
    
    world.reset()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    if not agent.initialize(world):
        print("Failed to initialize agent")
        return False
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"Step {step}/{steps}: "
                  f"Neurons firing: {brain.getFiringNeuronCount()}, "
                  f"Total spikes: {brain.getTotalSpikeCount()}")
    
    print(f"Complete example completed after {steps} steps")
    brain.logStatus()
    
    return True
def demo_basic_neural_network():
    """Demo basic neural network functionality."""
    print("=== NLM Basic Neural Network Demo ===\n")
    
    # Create brain with default configuration
    brain = create_brain()
    initialize_brain(brain)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run basic simulation
    run_simulation(brain, 100)
    
    print("\nBasic demo completed successfully!")
def demo_complete_agent():
    """Demo complete agent with world interaction."""
    print("=== NLM Complete Agent Demo ===\n")
    
    # Create brain
    brain = create_brain()
    initialize_brain(brain)
    
    # Create world
    world = nlm.createSimpleWorld()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run complete example
    run_complete_example(brain, world, agent, 200)
    
    print("\nComplete agent demo completed successfully!")
def demo_phase3_experiment():
    """Demo Phase 3 experiment."""
    print("=== NLM Phase 3 Experiment Demo ===\n")
    
    brain = create_brain()
    initialize_brain(brain)
    
    # Run Phase 3 specific functionality
    print("Phase 3 focuses on world interaction and sensory processing")
    
    # Create world for Phase 3
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    
    # Run with different agent configurations
    configurations = [
        {"reward_modulation": False, "curiosity": False, "development": False},
        {"reward_modulation": True, "curiosity": True, "development": True},
    ]
    
    for i, config in enumerate(configurations):
        print(f"\nConfiguration {i+1}: {config}")
        
        # Reset brain
        brain.reset()
        initialize_brain(brain)
        
        # Set agent configuration
        agent.enableRewardModulation(config["reward_modulation"])
        agent.enableCuriosity(config["curiosity"])
        agent.enableDevelopment(config["development"])
        
        # Run for shorter duration
        run_complete_example(brain, world, agent, 50)
    
    print("\nPhase 3 demo completed successfully!")
def demo_data_analysis():
    """Demo data analysis capabilities."""
    print("=== NLM Data Analysis Demo ===\n")
    
    brain = create_brain()
    initialize_brain(brain)
    
    # Run simulation for data collection
    run_simulation(brain, 500)
    
    # Collect statistics
    print("\n=== Statistics ===")
    print(f"Total simulation steps: 500")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spike count: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage().value}")
    
    print("\nData analysis demo completed successfully!")
def demo_help():
    """Show help information."""
    print("=== NLM Command Line Interface ===\n")
    print("Usage: nlm [OPTIONS] [COMMAND]")
    print("\nCommands:")
    print("  basic        Run basic neural network demo")
    print("  agent        Run complete agent demo")
    print("  phase3       Run Phase 3 experiment demo")
    print("  analyze      Run data analysis demo")
    print("  help         Show this help message")
    print("\nGlobal Options:")
    print("  -h, --help   Show help message")
    print("  -c, --config PATH  Configuration file path")
    print("  -s, --steps STEPS  Number of simulation steps")
    print("\nExamples:")
    print("  nlm basic")
    print("  nlm agent --steps 500")
    print("  nlm phase3 -c my_config.json")
def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) CLI Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  basic        Run basic neural network demo
  agent        Run complete agent demo
  phase3       Run Phase 3 experiment demo
  analyze      Run data analysis demo
  help         Show this help message

Examples:
  nlm basic
  nlm agent --steps 500
  nlm phase3 -c my_config.json
        """
    )
    
    parser.add_argument(
        "command",
        choices=["basic", "agent", "phase3", "analyze", "help"],
        default="basic",
        help="Command to execute"
    )
    
    parser.add_argument(
        "-c", "--config",
        help="Configuration file path"
    )
    
    parser.add_argument(
        "-s", "--steps",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    # Command dispatch
    try:
        if args.command == "basic":
            demo_basic_neural_network()
        elif args.command == "agent":
            demo_complete_agent()
        elif args.command == "phase3":
            demo_phase3_experiment()
        elif args.command == "analyze":
            demo_data_analysis()
        elif args.command == "help":
            demo_help()
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())
