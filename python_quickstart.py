"""
NLM Python Beginners Guide
==========================

This is a quick-start guide for using the NLM Python bindings.
If you're new to NLM, start here!
"""

import pynlm

def quick_start():
    """The absolute fastest way to run a simulation"""
    print("NLM Quick Start")
    print("===============")
    
    # Create a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Make it think
    for i in range(100):
        brain.step(i)
    
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
    print()

def step_by_step():
    """Step-by-step guide for complete simulation"""
    print("Step-by-Step Simulation")
    print("=======================")
    
    # 1. Create brain and config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Run simulation
    for step in range(100):
        world.update(0.1)
        
        # Get what you see
        percept = world.getSensoryPercept()
        
        # Tell your brain
        agent.processSensoryInput(percept)
        
        # Your brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Do the action
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Simulation complete!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def common_patterns():
    """Common coding patterns you should know"""
    print("Common Patterns")
    print("===============")
    
    # Pattern 1: Run a simulation
    def run_simulation(brain, world, agent, num_steps):
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Pattern 2: Get brain statistics
    def print_brain_stats(brain):
        print(f"Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Pattern 3: Enable learning
    def enable_learning(agent):
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    print("Patterns defined above can be used in your own code")
    print()

def getting_help():
    """Where to find help and documentation"""
    print("Getting Help")
    print("============")
    
    print("Available in the NLM package:")
    print("  - pynlm.createBrain(config) - Create a neural brain")
    print("  - pynlm.createSimpleWorld() - Create simulation world")
    print("  - pynlm.createAgentBrain(brain) - Connect brain to world")
    print()
    
    print("Key methods:")
    print("  - brain.initialize() - Start the brain")
    print("  - brain.step(step) - Make brain think one step")
    print("  - brain.getTotalNeuronCount() - Get neuron count")
    print("  - world.update(time) - Advance world simulation")
    print("  - agent.processSensoryInput(percept) - Feed sense data")
    print("  - agent.decodeMotorCommand() - Get action")
    print("  - world.applyMotorCommand(action, time) - Do action")
    print()
    
    print("Documentation files:")
    print("  - README.md - Project overview")
    print("  - easy_usage.md - Easy usage guide")
    print("  - HOW_TO_USE.md - Comprehensive manual")
    print("  - python_examples.py - More examples")
    print()

if __name__ == "__main__":
    quick_start()
    step_by_step()
    common_patterns()
    getting_help()