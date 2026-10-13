"""
NLM Python Example Scripts

These scripts demonstrate the exact usage patterns documented in:
- easy_usage.md
- HOW_TO_USE.md

Each script shows how to use the NLM Python bindings for different use cases.
"""

# Example 1: Simplest Brain (from easy_usage.md)
#!/usr/bin/env python3
"""
Example 1: Simplest Brain
=======================

The simplest possible example - just create a brain and make it think.

From easy_usage.md: "The Simplest Possible Example"
"""

import pynlm

def main():
    print("Running Example 1: Simplest Brain")
    print("=" * 40)
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    
    # Run for 100 steps as in the original example
    for i in range(100):
        brain.step(i)
    
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
    print("Example 1 completed successfully!\n")

if __name__ == "__main__":
    main()

# Example 2: Brain Watching a World (from easy_usage.md)
#!/usr/bin/env python3
"""
Example 2: Brain Watching a World
=================================

From easy_usage.md: "Project 2: Brain Watching a World"
"""

import pynlm

def main():
    print("Running Example 2: Brain Watching a World")
    print("=" * 50)
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
        if i % 10 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    print("Example 2 completed successfully!\n")

if __name__ == "__main__":
    main()

# Example 3: Complete Agent (from HOW_TO_USE.md)
#!/usr/bin/env python3
"""
Example 3: Complete Agent
=========================

From HOW_TO_USE.md: "Complete Agent Example"
"""

import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Starting simulation with {num_steps} steps...")
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print(f"\nSimulation complete after {num_steps} steps!")
    print(f"Final brain state:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

if __name__ == "__main__":
    run_agent_simulation(100)
    print("\nExample 3 completed successfully!\n")

# Example 4: Run Simulation Pattern (from easy_usage.md)
#!/usr/bin/env python3
"""
Example 4: Run a Simulation
===========================

From easy_usage.md: "Pattern 1: Run a Simulation"
"""

import pynlm

def run_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain and world."""
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Get brain stats (from Pattern 2 in easy_usage.md)
    print(f"Final simulation statistics:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Use it like this:
if __name__ == "__main__":
    print("Running Example 4: Run a Simulation Pattern")
    print("=" * 50)
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features as in easy_usage.md
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation pattern
    run_simulation(brain, world, agent, 200)
    
    print("\nExample 4 completed successfully!\n")

# Example 5: Silent Brain (from easy_usage.md)
#!/usr/bin/env python3
"""
Example 5: Silent Brain (Just Neurons)
========================================

From easy_usage.md: "Project 1: Silent Brain (Just Neurons)"
"""

import pynlm

def main():
    print("Running Example 5: Silent Brain")
    print("=" * 40)
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    print("Silent brain test done!")
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    print("Example 5 completed successfully!\n")

if __name__ == "__main__":
    main()

# Example 6: Configuration Usage (from HOW_TO_USE.md)
#!/usr/bin/env python3
"""
Example 6: Configuration Usage
===============================

From HOW_TO_USE.md: "Config Class" section
"""

import pynlm

def main():
    print("Running Example 6: Configuration Usage")
    print("=" * 50)
    
    # Create
    config = pynlm.createDefaultConfig()
    
    # Query
    print(f"Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    
    # Modify
    config.set("brain.neuron_count", 1000)
    config.set("learning.rate", 0.001)
    
    print(f"Has 'brain.neuron_count' after set: {config.has('brain.neuron_count')}")
    print(f"Has 'learning.rate' after set: {config.has('learning.rate')}")
    
    # Get all keys
    keys = config.getKeys()
    print(f"All configuration keys: {keys}")
    
    # File I/O (commented out as files may not exist)
    # config.saveToFile("config.json")
    # loaded_config = pynlm.createDefaultConfig()
    # loaded_config.loadFromFile("config.json")
    
    print("Example 6 completed successfully!\n")

if __name__ == "__main__":
    main()