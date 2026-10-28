# NLM Python Module
# High-level interface for the NLM (Neural Learning Machine) framework

import sys
import time
from typing import List, Dict, Optional, Any, Union, Callable
import numpy as np

# Import the core NLM classes from the C++ bindings
# Note: These are forward declarations for type hints
Brain = None
Config = None
AgentBrain = None
SimpleWorld = None
SensoryInput = None
Vision = None
Audio = None
Action = None
ActionType = None
MotorCommand = None
NeuronType = None
SynapseType = None
DevelopmentalStage = None
FiringState = None
WorldObject = None
WorldObjectType = None
AgentBody = None
ActionResult = None
SensoryPercept = None

# Helper functions
__version__ = "0.1.0"
__all__ = [
    # Core classes (stubs - actual implementations come from C++ bindings)
    "Brain",
    "Config",
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Vision",
    "Audio",
    "Action",
    "ActionType",
    "MotorCommand",
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "WorldObject",
    "WorldObjectType",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    # Helper functions
    "create_brain",
    "create_default_config",
    "create_simple_world",
    "create_agent_brain",
    "run_demo",
    "run_complete_simulation",
    "get_system_info",
]

def create_default_config() -> Config:
    """Create a default configuration for NLM.
    
    Returns:
        Config: A default configuration object
        
    Example:
        >>> config = create_default_config()
        >>> brain = create_brain(config)
    """
    if Config is None:
        raise ImportError("NLM C++ extension not available. Cannot create config.")
    return Config()

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a neural simulation brain.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        Brain: A new brain instance
        
    Example:
        >>> brain = create_brain()
        >>> brain = create_brain(create_default_config())
    """
    if Brain is None:
        raise ImportError("NLM C++ extension not available. Cannot create brain.")
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world() -> SimpleWorld:
    """Create a simple 2D world for NLM simulation.
    
    Returns:
        SimpleWorld: A new world instance
        
    Example:
        >>> world = create_simple_world()
        >>> world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    """
    if SimpleWorld is None:
        raise ImportError("NLM C++ extension not available. Cannot create world.")
    return SimpleWorld()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface.
    
    Args:
        brain: The brain instance to wrap
        
    Returns:
        AgentBrain: An agent brain instance
        
    Example:
        >>> brain = create_brain()
        >>> agent = create_agent_brain(brain)
    """
    if AgentBrain is None:
        raise ImportError("NLM C++ extension not available. Cannot create agent.")
    return AgentBrain(brain)

def get_system_info() -> Dict[str, Any]:
    """Get system information about the NLM installation.
    
    Returns:
        Dict: System information including version, capabilities, etc.
    """
    return {
        "version": __version__,
        "description": "NLM (Neural Learning Machine) - Brain Simulation Framework",
        "capabilities": [
            "Neural simulation with spiking neurons",
            "Memory systems (working, episodic, associative)",
            "Plasticity mechanisms (STDP, Hebbian, structural)",
            "Neuromodulation (dopamine, curiosity, novelty)",
            "Prediction systems",
            "Cognition (planning, concept formation, attention)",
            "Development stages",
            "Sensory processing (vision, audio, proprioception)",
            "Motor control",
            "Environment simulation",
            "Agent learning and adaptation",
        ],
        "neuron_types": ["Sensory", "Motor", "Internal", "Excitatory", "Inhibitory"],
        "action_types": [
            "MoveForward", "MoveBackward", "MoveLeft", "MoveRight",
            "TurnLeft", "TurnRight", "Look", "LookUp", "LookDown",
            "Interact", "Eat", "Drink", "Rest", "Wait", "Custom"
        ],
        "development_stages": ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"],
        "status": "C++ extension loading..." if any([
            Brain is None, Config is None, AgentBrain is None, 
            SimpleWorld is None
        ]) else "C++ extension loaded successfully"
    }

def run_demo():
    """Run a basic NLM demonstration.
    
    This function runs a simple example showing the core NLM functionality.
    """
    print("=== NLM Basic Demonstration ===\n")
    
    try:
        # Create a simple brain
        print("1. Creating brain...")
        brain = create_brain()
        brain.initialize()
        
        print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run some simulation steps
        print("\n2. Running simulation...")
        steps = 100
        for step in range(steps):
            brain.step(step)
            if step % 20 == 0:
                print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        # Show results
        print(f"\n3. Results after {steps} steps:")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"   E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"   Development stage: {brain.getDevelopmentalStage()}")
        
        print("\n=== Demonstration Complete ===\n")
        
    except ImportError as e:
        print(f"   Error: {e}")
        print("   This is expected if the C++ extension is not built.")
        print("   To build the extension, run:")
        print("     pip install -e .")
        print("   or:")
        print("     python setup.py build_ext --inplace")

def run_complete_simulation(config: Optional[Dict[str, Any]] = None, 
                            steps: int = 1000,
                            verbose: bool = True) -> Dict[str, Any]:
    """Run a complete NLM simulation with world and agent.
    
    This function runs a full agent simulation showing the integration
    of all NLM components.
    
    Args:
        config: Optional configuration dictionary
        steps: Number of simulation steps to run
        verbose: Whether to print progress information
        
    Returns:
        Dict: Simulation results and statistics
        
    Example:
        >>> results = run_complete_simulation(steps=500)
        >>> print(f"Firing rate: {results['final_firing_rate']:.2f} Hz")
    """
    import time
    
    start_time = time.time()
    
    # Create components
    if config is None:
        config = {}
    
    # Check if extension is available
    if any([Brain is None, Config is None, AgentBrain is None, SimpleWorld is None]):
        raise ImportError("NLM C++ extension not available. Cannot run complete simulation.")
    
    # Create configuration
    nlm_config = create_default_config()
    
    # Configure based on input
    if "neuron_count" in config:
        nlm_config.set("brain.neuron_count", config["neuron_count"])
    if "region_count" in config:
        nlm_config.set("brain.region_count", config["region_count"])
    if "connection_probability" in config:
        nlm_config.set("brain.connection_probability", config["connection_probability"])
    
    # Create brain
    brain = create_brain(nlm_config)
    brain.initialize()
    
    # Create world
    world = create_simple_world()
    world_width = config.get("world_width", 20)
    world_height = config.get("world_height", 20)
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Create agent
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable features based on configuration
    if config.get("enable_reward_modulation", True):
        agent.enableRewardModulation(True)
    if config.get("enable_structural_plasticity", True):
        agent.enableStructuralPlasticity(True)
    if config.get("enable_development", True):
        agent.enableDevelopment(True)
    if config.get("enable_curiosity", True):
        agent.enableCuriosity(True)
    
    # Run simulation
    if verbose:
        print(f"\n=== Complete NLM Simulation ===")
        print(f"Configuration: {brain.getTotalNeuronCount()} neurons, {steps} steps")
        print(f"World: {world_width}x{world_height}")
        print(f"Enabled: Reward={config.get('enable_reward_modulation', True)}, "
              f"Plasticity={config.get('enable_structural_plasticity', True)}, "
              f"Development={config.get('enable_development', True)}, "
              f"Curiosity={config.get('enable_curiosity', True)}\n")
    
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if config.get("enable_reward_modulation", True):
            reward = 0.0
            internal_signals = percept.getInternal()
            if internal_signals and len(internal_signals) > 0:
                reward = internal_signals[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if config.get("enable_development", True):
            agent.updateDevelopment(0.1)
        
        # Print progress
        if verbose and step % 100 == 0 and step > 0:
            print(f"   Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Collect results
    results = {
        "duration": duration,
        "steps": steps,
        "final_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "final_firing_neurons": brain.getFiringNeuronCount(),
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "development_stage": brain.getDevelopmentalStage(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "final_prediction_error": agent.getPredictionError(),
        "final_neuromodulation": agent.getNeuromodulationLevel(),
        "world_time": world.getSimulationTime(),
        "config": config,
    }
    
    if verbose:
        print(f"\n=== Simulation Complete ===")
        print(f"Duration: {duration:.2f} seconds ({duration/steps:.4f} sec/step)")
        print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
        print(f"Total spikes: {results['total_spikes']}")
        print(f"Development stage: {results['development_stage']}")
        print(f"Neuromodulation level: {results['final_neuromodulation']:.3f}")
    
    return results

# Main command-line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s demo                    # Run basic demonstration
  %(prog)s simulate --steps 500    # Run complete simulation
  %(prog)s info                    # Show system information
  %(prog)s --help                 # Show help
        """.strip()
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run basic demonstration")
    
    # Simulate command
    simulate_parser = subparsers.add_parser("simulate", help="Run complete simulation")
    simulate_parser.add_argument("--steps", type=int, default=1000,
                                help="Number of simulation steps")
    simulate_parser.add_argument("--neurons", type=int, default=1000,
                                help="Number of neurons in brain")
    simulate_parser.add_argument("--regions", type=int, default=2,
                                help="Number of brain regions")
    simulate_parser.add_argument("--width", type=int, default=20,
                                help="World width")
    simulate_parser.add_argument("--height", type=int, default=20,
                                help="World height")
    
    # Info command
    info_parser = subparsers.add_parser("info", help="Show system information")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute commands
    if args.command == "demo":
        run_demo()
    elif args.command == "simulate":
        config = {
            "neuron_count": args.neurons,
            "region_count": args.regions,
            "world_width": args.width,
            "world_height": args.height,
        }
        run_complete_simulation(config, steps=args.steps)
    elif args.command == "info":
        import json
        info = get_system_info()
        print(json.dumps(info, indent=2))
    else:
        parser.print_help()