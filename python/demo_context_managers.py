"""
Simple demonstration of NLM Python context managers.

This script shows a basic example of using context managers for resource management
in NLM (Neural Learning Machine) simulations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

def main():
    print("NLM Python Context Managers - Basic Demo")
    print("=" * 50)
    
    # Example 1: Create and use a configuration
    print("\n1. Creating and using a Config context manager:")
    with nlm.Config() as config:
        config.set("simulation.max_steps", 100)
        config.set("simulation.learning_rate", 0.01)
        config.set("brain.neuron_count", 1000)
        
        print(f"   ✓ Config loaded with {config.getOr('simulation.max_steps', 0)} steps")
    
    print("   ✓ Config cleaned up automatically")
    
    # Example 2: Create and use a brain
    print("\n2. Creating and using a Brain context manager:")
    with nlm.Config() as config:
        config.set("brain.neuron_count", 500)
        with nlm.Brain(config) as brain:
            brain.initialize()
            print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    print("   ✓ Brain cleaned up automatically")
    
    # Example 3: Create and use a world
    print("\n3. Creating and using a SimpleWorld context manager:")
    with nlm.SimpleWorld() as world:
        world.configure(100.0, 100.0, 10, 10)
        world.setAgentStart(50.0, 50.0)
        print(f"   ✓ World configured: {world.getWidth()} x {world.getHeight()}")
    
    print("   ✓ World cleaned up automatically")
    
    # Example 4: Create and use an agent brain
    print("\n4. Creating and using an AgentBrain context manager:")
    with nlm.Config() as config:
        with nlm.Brain(config) as brain:
            brain.initialize()
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 5, 5)
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    action = agent.decodeMotorCommand()
                    print(f"   ✓ AgentBrain created and action decoded: {action.getName()}")
    
    print("   ✓ All nested context managers cleaned up")
    
    # Example 5: Create and use sensory input
    print("\n5. Creating and using SensoryInput context managers:")
    with nlm.Vision(10, 10) as vision:
        print(f"   ✓ Vision created: {vision.getType()}")
    
    with nlm.Audio(44100, 44100) as audio:
        print(f"   ✓ Audio created: {audio.getNumSamples()} samples")
    
    print("   ✓ All SensoryInput cleaned up")
    
    # Example 6: Create and use actions
    print("\n6. Creating and using Action context managers:")
    actions = [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft, nlm.ActionType.Interact]
    for action_type in actions:
        with nlm.Action(action_type) as action:
            print(f"   ✓ {action.getName()} action created")
    
    print("   ✓ All Action objects cleaned up")
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
    print("=" * 50)

if __name__ == "__main__":
    main()
