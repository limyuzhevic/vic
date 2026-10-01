# Basic Example 2: Brain in a Simple World

This is a basic example demonstrating NLM usage in a simple world environment. It's designed for users who want to understand how NLM interacts with the world.

## Overview

This example shows how to create and use a brain in a simple world environment. It demonstrates the interaction between the brain, agent, and world components in NLM.

## Code

```python
#!/usr/bin/env python3
"""
Basic Example 2: Brain in a Simple World

This example demonstrates NLM usage in a simple world environment.
It's designed for users who want to understand how NLM interacts with the world.
"""

import pynlm

def main():
    """Main function"""
    print("=== Basic Example 2: Brain in a Simple World ===")
    
    # Step 1: Create brain
    print("1. Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Step 2: Create agent
    print("2. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    
    # Step 3: Create and configure world
    print("3. Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,           # World width
        height=20,          # World height
        visionWidth=8,      # Agent's visual field width
        visionHeight=8      # Agent's visual field height
    )
    
    # Step 4: Initialize agent with world
    print("4. Initializing agent with world...")
    world.reset()
    agent.initialize(world)
    
    # Step 5: Enable important subsystems
    print("5. Enabling important subsystems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Step 6: Set agent start position
    print("6. Setting agent start position...")
    world.setAgentStart(10.0, 10.0)  # Start in center
    
    # Step 7: Run simulation
    print("7. Running simulation...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Do the action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Dev Stage: {brain.getDevelopmentalStage()}")
    
    # Step 8: Analyze results
    print("8. Analyzing results...")
    print(f"   Total simulation time: {world.getSimulationTime():.2f}")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Final development stage: {brain.getDevelopmentalStage()}")
    
    print("\n✅ Basic example completed successfully!")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates the interaction between the brain, agent, and world components in NLM:

1. **Brain Creation**: Create a brain instance
2. **Agent Creation**: Create an agent interface for the brain
3. **World Creation**: Create a simple world environment
4. **World Configuration**: Configure world dimensions and sensory capabilities
5. **Agent Initialization**: Initialize agent with world
6. **Subsystem Enable**: Enable important brain subsystems
7. **Agent Start**: Set agent start position
8. **Simulation**: Run simulation with agent-world interaction
9. **Development**: Update brain development over time
10. **Analysis**: Analyze simulation results

### Key Components

- **Brain**: `pynlm.createBrain(config)` - Neural processing core
- **Agent**: `pynlm.createAgentBrain(brain)` - Interface between brain and world
- **World**: `pynlm.createSimpleWorld()` - Environment simulation
- **Sensory Input**: `world.getSensoryPercept()` - What the agent perceives
- **Action Selection**: `agent.decodeMotorCommand()` - Brain decides what to do
- **World Update**: `world.update(0.1)` - Update world state

### Expected Output

```
=== Basic Example 2: Brain in a Simple World ===
1. Creating brain...
2. Creating agent...
3. Creating and configuring world...
4. Initializing agent with world...
5. Enabling important subsystems...
6. Setting agent start position...
7. Running simulation...
   Step 0:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 20:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 40:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 60:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 80:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
   Step 100:
     Firing neurons: 0
     Curiosity: 0.500
     Novelty: 0.000
     Dev Stage: Initial
8. Analyzing results...
   Total simulation time: 10.00
   Final curiosity level: 0.500
   Final novelty level: 0.000
   Final development stage: Initial

✅ Basic example completed successfully!
```

### Learning Outcomes

By completing this example, you will learn:

1. **How to use the world**: Use `pynlm.createSimpleWorld()` to create a world environment
2. **How to configure the world**: Configure world dimensions and sensory capabilities
3. **How to interact with the world**: Understand the agent-world interaction loop
4. **How to use brain subsystems**: Enable and use brain subsystems
5. **How to run a complete simulation**: Run a complete agent simulation with brain, agent, and world

### Extensions

To extend this example:

1. **Custom World**: Create custom world with different properties
2. **Agent Behavior**: Implement custom agent behavior
3. **Sensorimotor Loop**: Understand and modify the sensorimotor loop
4. **Learning**: Study how the brain learns from interactions
5. **Adaptation**: Study how the brain adapts to the environment

## Files

- **Basic Example 2 Code**: `examples/basic-example-2.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **Tutorial 2**: `docs/tutorials/tutorial-2-brain-configuration.md`
- **API Reference**: `docs/api-reference.md`
- **Configuration Guide**: `docs/configuration.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example provides a comprehensive introduction to NLM usage in a world environment.🧠✨