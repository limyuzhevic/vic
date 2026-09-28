#!/usr/bin/env python3
"""
Simple NLM Python Example - Demonstrating Improved API

This example shows the improved NLM Python API after refactoring the codebase
for better maintainability and user-friendliness.
"""

import pynlm

def basic_brain_example():
    """Basic brain creation and simulation example."""
    print("=== Basic Brain Example ===")
    
    # Create a default brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    return brain

def world_agent_example():
    """Complete world-agent simulation example."""
    print("\n=== World-Agent Example ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Created complete agent-world system")
    print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start position: (10.0, 10.0)")
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        agent.applyRewardModulation(0.1, 0.0)
        
        print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    print("World-agent simulation complete!")
    return brain, world, agent

def config_example():
    """Configuration management example."""
    print("\n=== Configuration Example ===")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Modify configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    
    print("Custom configuration created:")
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 0)}")
    print(f"  Connection probability: {config.getOr('brain.connection_probability', 0.0)}")
    print(f"  STDP enabled: {config.getOr('plasticity.stdp.enable', false)}")
    print(f"  Hebbian enabled: {config.getOr('plasticity.hebbian.enable', false)}")
    print(f"  Dopamine scale: {config.getOr('neuromod.dopamine.scale', 0.0)}")
    
    # Save configuration (example)
    print("Configuration can be saved to file for later use")
    
    return config

def demonstration_complete():
    """Complete demonstration of all examples."""
    print("NLM Python API Demonstration")
    print("=" * 40)
    
    try:
        # Example 1: Basic brain
        brain1 = basic_brain_example()
        
        # Example 2: World-agent system
        brain2, world2, agent2 = world_agent_example()
        
        # Example 3: Configuration
        config = config_example()
        
        print("\n" + "=" * 40)
        print("All examples completed successfully!")
        print("\nKey improvements demonstrated:")
        print("  ✓ Clean, intuitive API for basic operations")
        print("  ✓ Complete agent-world simulation support")
        print("  ✓ Flexible configuration management")
        print("  ✓ Comprehensive documentation and examples")
        print("  ✓ Separation of experiments and library code")
        
        return True
        
    except Exception as e:
        print(f"\nError in demonstration: {e}")
        return False

if __name__ == "__main__":
    success = demonstration_complete()
    exit(0 if success else 1)
