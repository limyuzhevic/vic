"""
NLM Python Bindings - Quick Usage Guide

This file provides a quick reference guide for using the NLM Python bindings.
It includes common patterns and examples for getting started with NLM.
"""

# Quick Start Examples

print("=== NLM Python Bindings Quick Start ===")
print()

print("1. Import the library")
print("   import pynlm")
print()

print("2. Create a brain with default configuration")
print("   config = pynlm.createDefaultConfig()")
print("   brain = pynlm.createBrain(config)")
print("   brain.initialize()")
print()

print("3. Run a simple simulation")
print("   for step in range(100):")
print("       brain.step(step)")
print()

print("4. Create a world")
print("   world = pynlm.createSimpleWorld()")
print("   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)")
print("   world.reset()")
print()

print("5. Create an agent brain")
print("   agent = pynlm.createAgentBrain(brain)")
print("   agent.initialize(world)")
print()

print("6. Run a complete episode")
print("   for step in range(100):")
print("       world.update(0.1)")
print("       agent.processSensoryInput(world.getSensoryPercept())")
print("       brain.step(step)")
print("       action = agent.decodeMotorCommand()")
print("       world.applyMotorCommand(action, world.getSimulationTime())")
print()

print("7. Enable learning features")
print("   agent.enableRewardModulation(True)")
print("   agent.enableCuriosity(True)")
print("   agent.enableDevelopment(True)")
print()

print("8. Get brain statistics")
print("   print(f'Firing neurons: {brain.getFiringNeuronCount()}')")
print("   print(f'Avg firing rate: {brain.getAverageFiringRate():.2f} Hz')")
print("   print(f'Development stage: {brain.getDevelopmentalStage()}')")
print()

print("=== Common Patterns ===")
print()

print("Pattern 1: Silent Brain Test")
print("""
def silent_brain_test():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(10):
        brain.step(i)
    return brain
""")

print("Pattern 2: World Observation")
print("""
def observe_world():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
""")

print("Pattern 3: Complete Agent Episode")
print("""
def run_complete_episode():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}")
""")

print("=== Error Handling ===")
print()

print("Always check that initialize() is called before step():")
print("""
try:
    brain = pynlm.createBrain(config)
    brain.initialize()  # Always call initialize first!
    
    for step in range(100):
        brain.step(step)  # This will fail if initialize() wasn't called
        
except Exception as e:
    print(f"Error: {e}")
    print("Make sure to call brain.initialize() before running simulation")
""")

print()
print("=== Common Issues and Solutions ===")
print()

print("Issue: Brain isn't doing anything")
print("Solution: Call brain.initialize() before brain.step()")
print()

print("Issue: Agent isn't moving")
print("Solution: Make sure world.applyMotorCommand() is being called")
print()

print("Issue: Everything is 0")
print("Solution: Brains need time to 'warm up' - try more steps")
print()

print("=== Quick Reference ===")
print()

print("Function                | Description")
print("-" * 50)
print("createDefaultConfig()   | Create default configuration")
print("createBrain(config)     | Create a new brain")
print("brain.initialize()      | Initialize the brain")
print("brain.step(step)        | Run one simulation step")
print("brain.save(filename)    | Save brain state")
print("brain.load(filename)    | Load brain state")
print("createSimpleWorld()     | Create a simple world")
print("world.configure(...)    | Configure world dimensions")
print("world.update(dt)        | Update world state")
print("world.getSensoryPercept() | Get what the agent sees")
print("createAgentBrain(brain) | Create agent interface")
print("agent.initialize(world) | Initialize agent with world")
print("agent.processSensoryInput(percept) | Process what the agent sees")
print("agent.decodeMotorCommand() | Get action from brain")
print()

print("=== Getting Help ===")
print()
print("For more detailed documentation, see:")
print("  - docs/ARCHITECTURE.md")
print("  - docs/SCIENCE.md")
print("  - HOW_TO_USE.md")
print("  - easy_usage.md")
print()

print("Run examples with:")
print("  python -m pynlm.cli basic    # Run basic examples")
print("  python -m pynlm.cli agent    # Run agent examples")
print("  python -m pynlm.cli tests    # Check tests")
print()

print("To run the quick test:")
print("  python test_bindings.py")
print()

print("To run the demo:")
print("  python demo.py")
print()

print("=== End of Quick Reference ===")
