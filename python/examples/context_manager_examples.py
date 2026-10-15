"""
Real-world example demonstrating the use of NLM Python context managers

This script shows how to use context managers for proper resource management
in a realistic neural simulation scenario.

The context managers ensure automatic cleanup and prevent resource leaks
by using Python's context manager protocol (__enter__ and __exit__ methods).

Example usage patterns:
    1. Single resource management
    2. Multiple nested resource management
    3. Error handling with resource cleanup
    4. Complex simulation workflows
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
import time

def example1_single_resource_management():
    """Example 1: Single resource management with Config context manager."""
    print("=" * 60)
    print("Example 1: Single Resource Management")
    print("=" * 60)
    
    # Use Config context manager for configuration
    with nlm.Config() as config:
        # Configure simulation parameters
        config.set("simulation.steps", 1000)
        config.set("simulation.learning_rate", 0.01)
        config.set("simulation.neuron_count", 500)
        
        print("✓ Configuration loaded and managed")
        print(f"  Steps: {config.getOr('simulation.steps', 0)}")
        print(f"  Learning Rate: {config.getOr('simulation.learning_rate', 0.0)}")
    
    # Config is automatically cleaned up here
    print("✓ Configuration cleaned up\n")


def example2_nested_resource_management():
    """Example 2: Nested resource management with multiple context managers."""
    print("=" * 60)
    print("Example 2: Nested Resource Management")
    print("=" * 60)
    
    # Use multiple nested context managers
    with nlm.Config() as config, \
         nlm.SimpleWorld() as world, \
         nlm.Brain(config) as brain:
        
        # Configure the simulation
        config.set("world.width", 100.0)
        config.set("world.height", 100.0)
        world.configure(100.0, 100.0, 10, 10)
        world.setAgentStart(50.0, 50.0)
        
        brain.initialize()
        
        print("✓ World and Brain initialized")
        print(f"  World dimensions: {world.getWidth()} x {world.getHeight()}")
        print(f"  Brain neuron count: {brain.getTotalNeuronCount()}")
        
        # Add an object to the world
        obj = nlm.WorldObject(25.0, 25.0, nlm.WorldObjectType.Resource, 5.0)
        world.addObject(obj)
        print("✓ Added object to world")
        
    # All resources are cleaned up here
    print("✓ World and Brain cleaned up\n")


def example3_error_handling_with_cleanup():
    """Example 3: Error handling with automatic resource cleanup."""
    print("=" * 60)
    print("Example 3: Error Handling with Automatic Cleanup")
    print("=" * 60)
    
    try:
        with nlm.Config() as config:
            config.set("simulation.max_steps", 100)
            print("✓ Configuration created")
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 5, 5)
                print("✓ World created")
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    print("✓ Brain created")
                    
                    # Simulate an error
                    raise ValueError("Simulated simulation error")
                    
    except ValueError as e:
        print(f"✓ Error caught and handled: {e}")
        print("✓ All resources (Config, World, Brain) were cleaned up automatically")
    
    print("✓ Error handling test completed\n")


def example4_complex_simulation_workflow():
    """Example 4: Complex simulation workflow with multiple agents."""
    print("=" * 60)
    print("Example 4: Complex Simulation Workflow")
    print("=" * 60)
    
    # Create simulation with multiple agents
    with nlm.Config() as config:
        config.set("agent.count", 3)
        config.set("agent.exploration_rate", 0.2)
        
        with nlm.SimpleWorld() as world:
            world.configure(200.0, 200.0, 20, 20)
            world.setMaxEnergy(100.0)
            
            # Add some environmental objects
            for x in range(20, 180, 40):
                for y in range(20, 180, 40):
                    obj = nlm.WorldObject(
                        float(x), float(y),
                        nlm.WorldObjectType.Resource,
                        10.0
                    )
                    world.addObject(obj)
            
            print(f"✓ World configured with {world.getWidth() * world.getHeight()} potential positions")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
                
                with nlm.AgentBrain(brain) as agent_brain:
                    agent_brain.initialize(world)
                    agent_brain.enableCuriosity(True)
                    agent_brain.enableRewardModulation(True)
                    
                    print("✓ Agent brain configured with neuromodulation")
                    
                    # Run simulation steps
                    for step in range(10):
                        # Update world
                        world.update(0.1)
                        
                        # Get agent's action
                        action = agent_brain.decodeMotorCommand()
                        print(f"  Step {step + 1}: Agent selected action {action.getName()}")
                        
                        # Apply action to world
                        result = world.applyMotorCommand(
                            nlm.MotorCommand.MoveForward,  # Simplified for demo
                            world.getSimulationTime()
                        )
                        print(f"    Action result: reward={result.reward}, success={result.success}")
                        
    print("✓ Complex simulation workflow completed\n")


def example5_sensory_input_context_manager():
    """Example 5: Sensory input context manager usage."""
    print("=" * 60)
    print("Example 5: Sensory Input Context Manager")
    print("=" * 60)
    
    # Create and process sensory inputs
    with nlm.Vision(10, 10) as vision:
        # Simulate visual data
        visual_data = []
        for i in range(10 * 10 * 3):
            visual_data.append(0.5 + (0.5 * ((i % 10) / 10.0)))
        
        vision.setData(visual_data)
        vision.setTimestamp(time.time())
        
        print("✓ Vision sensory input created")
        print(f"  Dimensions: {vision.getDimensions()}")
        print(f"  Timestamp: {vision.getTimestamp()}")
        
    print("✓ Vision sensory input cleaned up\n")
    
    # Process audio input
    with nlm.Audio(44100, 44100) as audio:
        audio_data = [0.1 * i for i in range(44100)]
        audio.setData(audio_data)
        audio.setSampleRate(44100)
        
        print("✓ Audio sensory input created")
        print(f"  Duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
    
    print("✓ Audio sensory input cleaned up\n")


def example6_action_management():
    """Example 6: Action context manager usage."""
    print("=" * 60)
    print("Example 6: Action Management")
    print("=" * 60)
    
    # Create different types of actions
    actions = [
        nlm.ActionType.MoveForward,
        nlm.ActionType.TurnLeft,
        nlm.ActionType.Interact,
        nlm.ActionType.Eat
    ]
    
    with nlm.Config() as config:
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            for action_type in actions:
                with nlm.Action(action_type) as action:
                    print(f"✓ Created action: {action.getName()}")
                    
                    # Add parameters
                    if action_type == nlm.ActionType.MoveForward:
                        action.setParameters([1.0, 0.0])  # speed, direction
                    elif action_type == nlm.ActionType.TurnLeft:
                        action.setParameters([0.0, -1.0])  # turn angle
                    elif action_type == nlm.ActionType.Interact:
                        action.setParameters([1.0])  # interaction strength
                    elif action_type == nlm.ActionType.Eat:
                        action.setParameters([0.5])  # food amount
                    
                    # Process action
                    print(f"  Parameters: {action.getParameters()}")
                    
                print(f"✓ Action {action_type} cleaned up\n")
    
    print("✓ All actions cleaned up\n")


def main():
    """Run all example demonstrations."""
    print("NLM Python Context Manager Examples")
    print("=" * 60)
    print()
    
    examples = [
        example1_single_resource_management,
        example2_nested_resource_management,
        example3_error_handling_with_cleanup,
        example4_complex_simulation_workflow,
        example5_sensory_input_context_manager,
        example6_action_management
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"❌ Example {i} failed: {e}")
            import traceback
            traceback.print_exc()
            return False
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
    return True


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
