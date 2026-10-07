import pynlm

# Example 1: Simplest Brain
"""Create a brain with default configuration and run basic simulation"""

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Example 2: Brain in a Simple World
"""Create a complete agent-world simulation"""

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Example 3: Complete Agent with Learning
"""Full agent simulation with all features enabled"""

def run_complete_agent_simulation(num_steps=1000):
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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation(500)
print("Agent simulation complete!")

# Example 4: Performance Monitoring
"""Demonstrate advanced monitoring capabilities"""

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Get comprehensive statistics
stats = pynlm.getAgentStats(agent)

print("Agent Performance Statistics:")
for key, value in stats.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.3f}")
    else:
        print(f"  {key}: {value}")

# Example 5: Advanced Brain Control
"""Demonstrate advanced brain manipulation features"""

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add a brain region
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

print(f"Created brain region with ID: {region_id}")
print(f"Total regions: {brain.getRegionCount()}")

# Save and load brain state
brain.save("test_checkpoint.bin")
print("Saved brain state")

# Create a new brain and load the saved state
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("test_checkpoint.bin")
print("Loaded brain state successfully")

# Clean up
import os
if os.path.exists("test_checkpoint.bin"):
    os.remove("test_checkpoint.bin")

print("All examples completed successfully!")
