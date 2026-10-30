#!/usr/bin/env python3

#!/usr/bin/env python3

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: Could not import pynlm: {e}")
    print("The NLM Python bindings may need to be compiled first.")

# Default configuration for different modes
DEFAULT_CONFIGS = {
    "quick": {
        "neuron_count": 100,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "max_simulation_steps": 1000,
        "connection_probability": 0.1,
        "environment_width": 20,
        "environment_height": 20,
        "visualization_enabled": False,
        "reward_discount_factor": 0.9,
        "dopamine_baseline": 0.1,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
    },
    "detailed": {
        "neuron_count": 500,
        "region_count": 2,
        "simulation_timestep": 0.001,
        "max_simulation_steps": 10000,
        "connection_probability": 0.15,
        "environment_width": 30,
        "environment_height": 30,
        "visualization_enabled": True,
        "visualization_update_rate": 10,
        "reward_discount_factor": 0.95,
        "dopamine_baseline": 0.2,
        "enable_reward_modulation": True,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_curiosity": True,
    },
    "research": {
        "neuron_count": 1000,
        "region_count": 3,
        "simulation_timestep": 0.0001,
        "max_simulation_steps": 100000,
        "connection_probability": 0.08,
        "environment_width": 50,
        "environment_height": 50,
        "visualization_enabled": True,
        "visualization_update_rate": 30,
        "reward_discount_factor": 0.99,
        "dopamine_baseline": 0.15,
        "enable_reward_modulation": True,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_curiosity": True,
        "enable_prediction": True,
    }
}

def create_config(config_dict: Dict[str, Any]) -> pynlm.Config:
    """Create a configuration object from a dictionary."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm is not available. Please compile the Python bindings first.")
    
    config = pynlm.Config()
    
    # Map Python-friendly config keys to C++ config keys
    key_mapping = {
        "neuron_count": "neuron_count",
        "region_count": "region_count", 
        "simulation_timestep": "simulation_timestep",
        "max_simulation_steps": "max_simulation_steps",
        "connection_probability": "connection_probability",
        "environment_width": "environment_width",
        "environment_height": "environment_height",
        "visualization_enabled": "visualization_enabled",
        "visualization_update_rate": "visualization_update_rate",
        "reward_discount_factor": "reward_discount_factor",
        "dopamine_baseline": "dopamine_baseline",
        "enable_reward_modulation": "enable_reward_modulation",
        "enable_structural_plasticity": "structural_plasticity_enabled",
        "enable_development": "enable_development",
        "enable_curiosity": "enable_curiosity",
        "enable_prediction": "enable_prediction",
        "plasticity_learning_rate": "plasticity_learning_rate",
        "stdp_ltp_weight": "stdp_ltp_weight",
        "stdp_ltd_weight": "stdp_ltd_weight",
        "stdp_time_constant": "stdp_time_constant",
        "synaptogenesis_rate": "synaptogenesis_rate",
        "pruning_rate": "pruning_rate",
    }
    
    for python_key, cpp_key in key_mapping.items():
        if python_key in config_dict:
            value = config_dict[python_key]
            if isinstance(value, bool):
                config.set(cpp_key, value)
            elif isinstance(value, int):
                config.set(cpp_key, value)
            elif isinstance(value, float):
                config.set(cpp_key, value)
            elif isinstance(value, str):
                config.set(cpp_key, value)
    
    return config

def run_simulation(config_name: str, steps: Optional[int] = None, interactive: bool = False):
    """Run a simulation with the given configuration."""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Please compile the Python bindings first.")
        return False
    
    print(f"Running NLM simulation with '{config_name}' configuration...")
    
    # Get configuration
    if config_name not in DEFAULT_CONFIGS:
        print(f"Error: Unknown configuration '{config_name}'. Available: {list(DEFAULT_CONFIGS.keys())}")
        return False
    
    config_dict = DEFAULT_CONFIGS[config_name].copy()
    
    if steps is not None:
        config_dict["max_simulation_steps"] = steps
    
    # Create configuration
    try:
        config = create_config(config_dict)
    except Exception as e:
        print(f"Error creating configuration: {e}")
        return False
    
    # Create brain
    print("Initializing brain...")
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Create world
    print("Creating world...")
    world = pynlm.SimpleWorld()
    world.configure(
        config_dict["environment_width"],
        config_dict["environment_height"],
        20,  # vision width
        20,  # vision height
    )
    
    # Set agent start position
    world.setAgentStart(5, 5)
    
    # Create agent brain
    agent_brain = pynlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Configure agent brain based on settings
    agent_brain.enableRewardModulation(config_dict.get("enable_reward_modulation", True))
    agent_brain.enableStructuralPlasticity(config_dict.get("enable_structural_plasticity", False))
    agent_brain.enableDevelopment(config_dict.get("enable_development", False))
    agent_brain.enableCuriosity(config_dict.get("enable_curiosity", True))
    
    print(f"Simulation started with {config_dict['neuron_count']} neurons")
    print(f"Brain has {brain.getTotalNeuronCount()} total neurons")
    print(f"Environment: {config_dict['environment_width']}x{config_dict['environment_height']}")
    print(f"Max steps: {config_dict['max_simulation_steps']}")
    print()
    print("Simulation controls:")
    print("  'q' - quit simulation")
    print("  's' - save brain state")
    print("  'l' - load brain state")
    print("  't' - show status")
    print()
    
    # Run simulation
    total_reward = 0.0
    step = 0
    
    try:
        while step < config_dict["max_simulation_steps"]:
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Brain step
            brain.step(step, step * config_dict["simulation_timestep"])
            
            # Get motor command
            motor_cmd = agent_brain.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(motor_cmd, step * config_dict["simulation_timestep"])
            
            # Get reward
            reward = world.computeReward()
            total_reward += reward
            
            # Apply reward modulation
            agent_brain.applyRewardModulation(reward, reward * 0.1)  # Simple prediction
            
            # Collect metrics
            if step % 100 == 0:
                print(f"Step {step:6d} | Reward: {reward:8.4f} | "
                      f"Total: {total_reward/(step+1):8.4f} | "
                      f"Firing: {brain.getAverageFiringRate():6.2f} | "
                      f"Neuromod: {agent_brain.getNeuromodulationLevel():6.3f}")
                
                # Get user input in interactive mode
                if interactive:
                    print("(Press Enter to continue, or type command:)")
                    user_input = input()
                    
                    if user_input.lower() == 'q':
                        print("Quitting simulation...")
                        break
                    elif user_input.lower() == 's':
                        print("Saving brain state...")
                        brain.save(f"brain_state_step_{step}.nlm")
                        print(f"Saved to brain_state_step_{step}.nlm")
                    elif user_input.lower() == 'l':
                        print("Loading brain state...")
                        brain.load("brain_state.nlm")
                        print("Loaded brain_state.nlm")
                    elif user_input.lower() == 't':
                        brain.logStatus()
                        
            step += 1
            
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    except Exception as e:
        print(f"Error during simulation: {e}")
        return False
    
    print(f"\nSimulation completed!")
    print(f"Total reward: {total_reward}")
    print(f"Average reward per step: {total_reward/step if step > 0 else 0}")
    print(f"Final neuromodulation level: {agent_brain.getNeuromodulationLevel()}")
    print(f"Final curiosity level: {agent_brain.getCuriosityLevel()}")
    print(f"Final novelty level: {agent_brain.getNoveltyLevel()}")
    
    return True

def run_phase6_demo():
    """Run the comprehensive Phase 6 integration demo."""
    print("Running Phase 6 Integration Demo...")
    print("This demonstrates the complete integration of all NLM brain systems.")
    print()
    
    # Create a comprehensive configuration
    config_dict = DEFAULT_CONFIGS["research"].copy()
    config_dict["max_simulation_steps"] = 5000
    config_dict["enable_prediction"] = True
    config_dict["enable_development"] = True
    
    config = create_config(config_dict)
    
    # Create brain
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.SimpleWorld()
    world.configure(40, 40, 20, 20)
    world.setAgentStart(10, 10)
    
    # Create agent
    agent_brain = pynlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable all systems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    print("Phase 6 Integration Demo")
    print("=" * 50)
    print(f"Configuration: {config_dict['neuron_count']} neurons, {config_dict['region_count']} regions")
    print(f"Duration: {config_dict['max_simulation_steps']} steps")
    print()
    
    # Run demo with simplified output
    total_reward = 0.0
    for step in range(config_dict["max_simulation_steps"]):
        # Get observation
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * config_dict["simulation_timestep"])
        
        # Get motor command
        motor_cmd = agent_brain.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, step * config_dict["simulation_timestep"])
        
        # Compute reward
        reward = world.computeReward()
        total_reward += reward
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(reward, reward * 0.1)
        
        # Update development
        agent_brain.updateDevelopment(config_dict["simulation_timestep"])
        
        # Periodic status output
        if step % 1000 == 0:
            print(f"Step {step:5d}: Reward {reward:7.4f} | "
                  f"Neuromod {agent_brain.getNeuromodulationLevel():6.3f} | "
                  f"Curiosity {agent_brain.getCuriosityLevel():6.3f} | "
                  f"Firing {brain.getAverageFiringRate():6.2f}")
    
    print()
    print("Phase 6 Integration Complete!")
    print(f"Total reward: {total_reward}")
    print(f"Average reward: {total_reward/config_dict['max_simulation_steps']}")
    
    # Show integration status
    print()
    print("Integration Status:")
    print(f"  Working Memory: {'✓' if brain.getWorkingMemory() else '✗'}")
    print(f"  Episodic Memory: {'✓' if brain.getEpisodicMemory() else '✗'}")
    print(f"  Neuromodulation: {'✓' if brain.getDopamine() else '✗'}")
    print(f"  Prediction: {'✓' if brain.getPredictionSystem() else '✗'}")
    print(f"  Development: {'✓' if brain.getDevelopmentSystem() else '✗'}")
    
    return True

def list_configs():
    """List available configurations."""
    print("Available configurations:")
    print()
    for name, config in DEFAULT_CONFIGS.items():
        print(f"{name.upper()} configuration:")
        print(f"  Neurons: {config['neuron_count']}")
        print(f"  Regions: {config['region_count']}")
        print(f"  Max steps: {config['max_simulation_steps']}")
        print(f"  Environment: {config['environment_width']}x{config['environment_height']}")
        print(f"  Reward discount: {config['reward_discount_factor']}")
        print(f"  Dopamine baseline: {config['dopamine_baseline']}")
        print()

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Python CLI Tool - Neural Learning Machine Simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli quick           Run a quick demonstration (100 neurons, 1000 steps)
  nlm_cli detailed 5000    Run detailed simulation with 5000 steps
  nlm_cli research        Run research-grade simulation (1000 neurons, 100000 steps)
  nlm_cli phase6          Run the comprehensive Phase 6 integration demo
  nlm_cli config          List all available configurations
  nlm_cli --interactive   Run interactive simulation with real-time controls
        ""
    )
    
    parser.add_argument(
        "command",
        nargs="?",
        default="quick",
        help="Command to execute (default: quick)"
    )
    
    parser.add_argument(
        "steps",
        nargs="?",
        type=int,
        help="Number of simulation steps to run (overrides config default)"
    )
    
    parser.add_argument(
        "--interactive",
        action="store_true",
        help="Run in interactive mode with real-time controls"
    )
    
    parser.add_argument(
        "--config",
        choices=list(DEFAULT_CONFIGS.keys()),
        help="Configuration to use (overrides command name)"
    )
    
    args = parser.parse_args()
    
    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        print("Please compile the Python bindings first by running:")
        print("  ./build.sh")
        print("  or")
        print("  python -m pip install scikit-build-core pybind11")
        sys.exit(1)
    
    # Handle special commands
    if args.command == "config":
        list_configs()
        return
    
    if args.command == "phase6":
        success = run_phase6_demo()
        sys.exit(0 if success else 1)
    
    # Determine which configuration to use
    config_name = args.config if args.config else args.command
    
    # Run simulation
    success = run_simulation(config_name, args.steps, args.interactive)
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
