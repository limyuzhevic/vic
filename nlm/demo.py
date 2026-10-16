#!/usr/bin/env python3
"""
NLM Demo Script

This script demonstrates the basic usage of the NLM Python bindings.
It's used as a command-line tool and can also be imported as a module.
"""

import sys
import os
import argparse
import time

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not found.")
    print("Please build the project first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make pynlm")
    sys.exit(1)
def print_banner():
    """Print the NLM demo banner."""
    print("=" * 60)
    print("NLM Python Bindings - Demo")
    print("=" * 60)
    print()
    print("This demo showcases the enhanced NLM Python bindings")
    print("with new convenience methods, advanced functionality,")
    print("and improved Pythonic APIs.")
    print()
def create_basic_demo():
    """Create and run a basic demo simulation."""
    print("1. Basic Brain Creation Demo")
    print("-" * 40)
    
    # Create default configuration
    config = nlm.create_default_config()
    print(f"Created default config with keys: {config.getKeys()}")
    
    # Create brain
    brain = nlm.createBrain(config)
    print("Created brain instance")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    # Create world
    world = nlm.create_simple_demo_world()
    print("Created demo world (100x100 with vision range 10x10)")
    
    # Run simulation
    print("Running simulation for 100 steps...")
    result = nlm.run_simulation(brain, world, 100)
    print(f"Simulation completed: {result}")
    
    # Get statistics
    stats = brain.get_statistics()
    print(f"\nBrain Statistics:")
    print(f"  Total Neurons: {stats['total_neurons']}")
    print(f"  Total Synapses: {stats['total_synapses']}")
    print(f"  Active Neurons: {stats['active_neurons']}")
    print(f"  Total Spikes: {stats['total_spikes']}")
    print(f"  Firing Rate: {stats['avg_firing_rate']:.2f} Hz")
    
    return brain, world
def create_action_demo():
    """Demonstrate action creation and manipulation."""
    print("\n2. Action Creation and Manipulation Demo")
    print("-" * 40)
    
    # Create different types of actions
    actions_demo = [
        ("Move Forward", nlm.ACTION_TYPE_MOVE_FORWARD),
        ("Move Backward", nlm.ACTION_TYPE_MOVE_BACKWARD),
        ("Turn Left", nlm.ACTION_TYPE_TURN_LEFT),
        ("Turn Right", nlm.ACTION_TYPE_TURN_RIGHT),
    ]
    
    for name, action_type in actions_demo:
        print(f"\n{name} Action:")
        
        # Create action using Pythonic APIs
        action = nlm.Action(action_type)
        action.add_param(1.0)  # Default distance
        action.add_param(0.5)  # Default speed
        
        # Modify using convenience methods
        action.set_param(0, 2.0)  # Change distance to 2.0
        
        # Convert to dictionary
        action_dict = action.to_dict()
        print(f"  Type: {name}")
        print(f"  Parameters: {action_dict['parameters']}")
        print(f"  Name: {action_dict['name']}")
        
        # Clean up
        del action

def create_config_demo():
    """Demonstrate configuration management."""
    print("\n3. Configuration Management Demo")
    print("-" * 40)
    
    # Create demo configuration
    config = nlm.create_demo_config()
    print(f"Created demo config with {len(config.getKeys())} keys")
    
    # Show initial values
    print("\nInitial configuration values:")
    important_keys = [
        "brain.neuron_count",
        "brain.simulation_steps",
        "world.width",
        "world.height",
    ]
    
    for key in important_keys:
        if config.has(key):
            value = config.get_or(key, None)
            print(f"  {key}: {value}")
    
    # Modify configuration using Pythonic APIs
    print("\nModifying configuration...")
    config.set_int("brain.neuron_count", 2000)
    config.set_float("world.width", 50.0)
    config.set_bool("simulation.verbose", True)
    
    # Get modified values
    print("\nModified configuration values:")
    for key in important_keys:
        if config.has(key):
            if key.endswith("neuron_count"):
                value = config.get_int(key, 0)
            elif key.endswith("width"):
                value = config.get_float(key, 0.0)
            elif key.endswith("height"):
                value = config.get_float(key, 0.0)
            else:
                value = config.get(key, None)
            print(f"  {key}: {value}")
    
    # Validate configuration
    print("\nValidating configuration...")
    required_keys = [
        "brain.neuron_count",
        "brain.simulation_steps",
        "world.width",
        "world.height",
    ]
    
    errors = config.validate(required_keys)
    if not errors:
        print("✓ Configuration is valid!")
    else:
        print(f"✗ Configuration has {len(errors)} errors:")
        for error in errors:
            print(f"  - {error}")

def create_agent_demo():
    """Demonstrate agent-based simulation."""
    print("\n4. Agent-Based Simulation Demo")
    print("-" * 40)
    
    # Create brain and world
    print("Creating brain and world...")
    config = nlm.create_demo_config()
    brain = nlm.create_simple_demo_brain(config)
    world = nlm.create_simple_demo_world()
    
    # Create agent
    print("Creating agent...")
    agent = nlm.create_agent(brain, world)
    agent.initialize(world)
    print("Agent initialized and connected to world")
    
    # Run agent episode
    print("\nRunning agent episode (50 steps)...")
    for step in range(50):
        # Get sensory perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Simulate world step
        world.update(0.01)
        
        # Get brain action
        action = brain.produceAction()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 10 == 0:
            print(f"  Step {step}: Active neurons = {brain.getActiveNeuronCount()}, "
                  f"Firing neurons = {brain.getFiringNeuronCount()}")
    
    print("\nAgent episode completed successfully!")

def create_advanced_demo():
    """Demonstrate advanced features."""
    print("\n5. Advanced Features Demo")
    print("-" * 40)
    
    # Create multiple scenarios for comparison
    print("Creating multiple research scenarios...")
    
    scenarios = {
        "Baseline": {
            "brain.neuron_count": 1000,
            "brain.simulation_steps": 500,
        },
        "High Plasticity": {
            "plasticity.learning_rate": 0.1,
            "brain.simulation_steps": 300,
        },
        "Low Connectivity": {
            "brain.connection_probability": 0.01,
            "brain.simulation_steps": 700,
        },
        "Fast Development": {
            "brain.development_rate": 5,
            "brain.simulation_steps": 400,
        },
    }
    
    results = {}
    
    for scenario_name, params in scenarios.items():
        print(f"\nRunning '{scenario_name}' scenario...")
        
        # Create configuration
        config = nlm.create_demo_config()
        
        # Apply parameters
        for key, value in params.items():
            if isinstance(value, int):
                config.set_int(key, value)
            elif isinstance(value, float):
                config.set_float(key, value)
        
        # Create and run brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        nlm.run_simulation(brain, world, params.get("brain.simulation_steps", 500))
        
        # Collect results
        stats = brain.get_statistics()
        results[scenario_name] = {
            "parameters": params,
            "statistics": stats,
            "simulation_id": len(results),
        }
        
        print(f"  - Neurons: {stats['total_neurons']}")
        print(f"  - Spikes: {stats['total_spikes']}")
        print(f"  - Firing rate: {stats['avg_firing_rate']:.2f} Hz")
    
    print(f"\nCompleted {len(results)} scenarios!")
    
    # Performance comparison
    print("\nPerformance Analysis:")
    print("-" * 40)
    
    best_spike = max(results.items(), key=lambda x: x[1]['statistics']['total_spikes'])
    fastest = min(results.items(), key=lambda x: x[1]['statistics']['avg_firing_rate'])
    
    print(f"Scenario with most spikes: {best_spike[0]} ({best_spike[1]['statistics']['total_spikes']} spikes)")
    print(f"Scenario with fastest firing: {fastest[0]} ({fastest[1]['statistics']['avg_firing_rate']:.2f} Hz)")

def create_performance_demo():
    """Demonstrate performance profiling."""
    print("\n6. Performance Profiling Demo")
    print("-" * 40)
    
    # Create brain
    brain = nlm.createBrain(nlm.create_default_config())
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Profile different simulation lengths
    simulation_lengths = [100, 500, 1000]
    
    for steps in simulation_lengths:
        print(f"\nProfiling {steps} simulation steps...")
        start_time = time.time()
        
        # Run simulation with profiling
        nlm.profile_brain(brain, steps)
        
        end_time = time.time()
        elapsed_ms = (end_time - start_time) * 1000
        
        print(f"  Completed in {elapsed_ms:.2f} ms")
        print(f"  Average time per step: {elapsed_ms/steps:.4f} ms")
    
    print("\nPerformance profiling completed!")

def main():
    """Main demo function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Demo",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python demo.py                    # Run all demos
  python demo.py --quick             # Quick demo only
  python demo.py --all               # Run all demos and save results
  python demo.py --help              # Show help information
        """
    )
    
    parser.add_argument(
        "--quick", "-q",
        action="store_true",
        help="Run quick demo only (skips time-intensive demos)",
    )
    
    parser.add_argument(
        "--all", "-a",
        action="store_true",
        help="Run all demos and save results to files",
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of steps for simulations (default: 100)",
    )
    
    args = parser.parse_args()
    
    # Print banner
    print_banner()
    
    # Run demos
    try:
        # Basic demo
        brain, world = create_basic_demo()
        
        # Action demo
        create_action_demo()
        
        # Configuration demo
        create_config_demo()
        
        # Agent demo
        if not args.quick:
            create_agent_demo()
        
        # Advanced demo
        if not args.quick:
            create_advanced_demo()
        
        # Performance demo
        if not args.quick:
            create_performance_demo()
        
        # Show summary
        print("\n" + "=" * 60)
        print("Demo completed successfully!")
        print("=" * 60)
        print("\nKey improvements demonstrated:")
        print("✓ New Pythonic APIs for configuration management")
        print("✓ Convenience methods for action manipulation")
        print("✓ Advanced simulation utilities")
        print("✓ Performance profiling tools")
        print("✓ Research simulation framework")
        print("\nFor more information, visit the documentation or examples.")
        
    except Exception as e:
        print(f"\nError during demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()
