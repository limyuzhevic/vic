#!/usr/bin/env python3
"""
Example script demonstrating the NLM Python bindings.

This script shows basic usage patterns for the NLM Python bindings,
demonstrating the main classes and their methods.
"""

import sys
import pynlm

def basic_brain_simulation():
    """Demonstrate basic brain simulation."""
    print("=== Basic Brain Simulation ===")
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(10):
        brain.step(step)
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"  Step {step:2d}: {firing:4d} neurons firing, {rate:6.2f} Hz")


def agent_world_simulation():
    """Demonstrate agent-brain and world integration."""
    print("\n=== Agent-World Simulation ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running agent simulation...")
    
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log status periodically
        if step % 5 == 0:
            body = world.getAgentBody()
            print(f"  Step {step:2d}: Agent at ({body.x:4.1f}, {body.y:4.1f}), "
                  f"Energy: {body.energy:4.2f}")
            print(f"    Curiosity: {agent.getCuriosityLevel():5.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():5.3f}")
    
    print("Agent simulation complete!")


def configuration_example():
    """Demonstrate configuration management."""
    print("\n=== Configuration Example ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    print("Default configuration:")
    keys = config.getKeys()
    for key in sorted(keys)[:10]:  # Show first 10 keys
        value = config.getOr(key, "N/A")
        print(f"  {key}: {value}")
    
    if len(keys) > 10:
        print(f"  ... and {len(keys) - 10} more keys")
    
    # Customize configuration
    print("\nCustomizing configuration...")
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    
    print(f"Custom neuron count: {config.getOr('brain.neuron_count', 0)}")
    print(f"Custom synapse density: {config.getOr('brain.synapse_density', 0.0)}")
    print(f"STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")


def world_objects_example():
    """Demonstrate world object management."""
    print("\n=== World Objects Example ===")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    print("Adding objects to world...")
    
    # Add different types of objects
    resource = pynlm.WorldObject(
        x=3.0, y=3.0,
        type=pynlm.WorldObjectType.Resource,
        value=10.0,
        radius=0.8
    )
    world.addObject(resource)
    print(f"  Added resource at ({resource.x}, {resource.y}) worth {resource.value}")
    
    hazard = pynlm.WorldObject(
        x=12.0, y=8.0,
        type=pynlm.WorldObjectType.Hazard,
        value=-3.0,
        radius=0.5
    )
    world.addObject(hazard)
    print(f"  Added hazard at ({hazard.x}, {hazard.y}) damage {hazard.value}")
    
    wall = pynlm.WorldObject(
        x=7.0, y=7.0,
        type=pynlm.WorldObjectType.Wall,
        radius=1.0
    )
    world.addObject(wall)
    print(f"  Added wall at ({wall.x}, {wall.y})")
    
    # Check agent position
    world.setAgentStart(2.0, 2.0)
    agent_body = world.getAgentBody()
    print(f"\nAgent starts at ({agent_body.x}, {agent_body.y})")
    
    # Check valid positions
    print("\nPosition validation:")
    print(f"  (2.0, 2.0) is valid: {world.isValidPosition(2.0, 2.0)}")
    print(f"  (3.0, 3.0) is valid: {world.isValidPosition(3.0, 3.0)} (resource)")
    print(f"  (7.0, 7.0) is valid: {world.isValidPosition(7.0, 7.0)} (wall)")
    print(f"  (99.0, 99.0) is valid: {world.isValidPosition(99.0, 99.0)} (off-map)")


def enum_examples():
    """Demonstrate enumeration usage."""
    print("\n=== Enumeration Examples ===")
    
    print("Neuron Types:")
    for neuron_type in pynlm.NeuronType:
        print(f"  {neuron_type.name:12s} = {neuron_type.value}")
    
    print("\nDevelopmental Stages:")
    for stage in pynlm.DevelopmentalStage:
        print(f"  {stage.name:12s} = {stage.value}")
    
    print("\nAction Types:")
    for action in [pynlm.ActionType.MoveForward, 
                   pynlm.ActionType.TurnLeft, 
                   pynlm.ActionType.Interact,
                   pynlm.ActionType.Eat]:
        print(f"  {action.name:12s} = {action.value}")
    
    print("\nWorld Object Types:")
    for obj_type in [pynlm.WorldObjectType.Resource,
                     pynlm.WorldObjectType.Hazard,
                     pynlm.WorldObjectType.Wall]:
        print(f"  {obj_type.name:12s} = {obj_type.value}")


def complete_example():
    """Run a complete NLM simulation example."""
    print("=== Complete NLM Example ===")
    print("This example demonstrates a full simulation with all components.\n")
    
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Simulation started!")
    print(f"  World size: {world.getWidth()} x {world.getHeight()}")
    print(f"  Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"  Max brain neurons: {brain.getTotalNeuronCount()}")
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log every 10 steps
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"\nStep {step}:")
            print(f"  Agent: ({body.x:6.1f}, {body.y:6.1f}), "
                  f"Energy: {body.energy:5.2f}, Age: {body.age:4.0}")
            print(f"  Brain: {brain.getFiringNeuronCount():5d} firing, "
                  f"{brain.getAverageFiringRate():6.2f} Hz")
            print(f"  Agent: Curiosity={agent.getCuriosityLevel():5.3f}, "
                  f"Novelty={agent.getNoveltyLevel():5.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    print(f"\nSimulation complete!")


def main():
    """Run all example demonstrations."""
    print("NLM Python Bindings Examples")
    print("==========================\n")
    
    # Set up logging to see debug messages
    import logging
    logging.basicConfig(level=logging.WARNING)
    
    try:
        # Run examples
        basic_brain_simulation()
        configuration_example()
        world_objects_example()
        enum_examples()
        
        # Run complete example
        complete_example()
        
        print("\n" + "="*50)
        print("All examples completed successfully!")
        print("="*50)
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    sys.exit(main())
