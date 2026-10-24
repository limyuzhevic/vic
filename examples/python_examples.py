#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python Interface Examples

This file provides examples of how to use the NLM Python API.
These examples demonstrate the intended interface for the Python bindings.

To use NLM with Python, you'll need to build the Python bindings first:
1. Install dependencies: pip install scikit-build-core pybind11 numpy
2. Build the bindings: pip install .
3. Then import and use the examples below
"""

import pynlm

def example_basic_brain_creation():
    """Example 1: Create a basic brain"""
    print("=== Example 1: Basic Brain Creation ===")
    
    # Step 1: Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Step 3: Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Brain fired {brain.getTotalSpikeCount()} times")
    print()

def example_brain_in_world():
    """Example 2: Brain interacting with a world"""
    print("=== Example 2: Brain in a World ===")
    
    # Create everything needed for agent-world interaction
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created agent with world ({world.getWidth()}x{world.getHeight()} cells)")
    print(f"Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run simulation loop
    for step in range(100):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input from world
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Simulation complete!")
    print()

def example_complete_agent_simulation():
    """Example 3: Complete agent simulation"""
    print("=== Example 3: Complete Agent Simulation ===")
    
    def run_agent_simulation(num_steps=500):
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
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.0, 7.0)
        
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
            world.update(0.1)
            
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
    brain, agent, world = run_agent_simulation(500)
    
    print("Agent simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print()

def example_configuration():
    """Example 4: Using configuration options"""
    print("=== Example 4: Configuration Options ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Query configuration parameters
    print(f"Default neuron count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"Default connection probability: {config.getOr('brain.connection_probability', 0.05)}")
    print(f"Default threshold potential: {config.getOr('brain.v_thresh', -50.0)}")
    
    # Modify configuration
    config.set('brain.neuron_count', 500)
    config.set('brain.connection_probability', 0.1)
    config.set('plasticity.stdp.learning_rate', 0.002)
    
    print("Modified configuration:")
    print(f"  neuron_count: {config.getOr('brain.neuron_count', 1000)}")
    print(f"  connection_probability: {config.getOr('brain.connection_probability', 0.05)}")
    print(f"  stdp.learning_rate: {config.getOr('plasticity.stdp.learning_rate', 0.001)}")
    
    # Create brain with modified config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print()

def example_key_classes_and_methods():
    """Example 5: Key classes and methods"""
    print("=== Example 5: Key Classes and Methods ===")
    
    print("Brain Class:")
    print("  Creation: brain = pynlm.createBrain(config)")
    print("  Initialization: brain.initialize()")
    print("  Simulation: brain.step(step_number)")
    print("  Statistics:")
    print("    getTotalNeuronCount() -> int")
    print("    getTotalSynapseCount() -> int")
    print("    getFiringNeuronCount() -> int")
    print("    getAverageFiringRate() -> float")
    print("    getExcitationInhibitionRatio() -> float")
    print("  State management:")
    print("    save('checkpoint.bin') -> bool")
    print("    load('checkpoint.bin') -> bool")
    print()
    
    print("Config Class:")
    print("  Creation: config = pynlm.createDefaultConfig()")
    print("  Query: if config.has('brain.neuron_count'):")
    print("  Modify: config.set('brain.neuron_count', 1000)")
    print("  File I/O:")
    print("    config.loadFromFile('config.json') -> bool")
    print("    config.saveToFile('config.json') -> bool")
    print()
    
    print("AgentBrain Class:")
    print("  Creation: agent = pynlm.createAgentBrain(brain)")
    print("  Initialization: agent.initialize(world)")
    print("  Sensory processing: agent.processSensoryInput(percept)")
    print("  Motor decoding: motor_cmd = agent.decodeMotorCommand()")
    print("  Neuromodulation: agent.applyRewardModulation(reward, predicted)")
    print("  Development: agent.updateDevelopment(timestep)")
    print("  Statistics:")
    print("    getCuriosityLevel() -> float")
    print("    getNoveltyLevel() -> float")
    print("    getPredictionError() -> float")
    print("    getNeuromodulationLevel() -> float")
    print("  Subsystem control:")
    print("    enableRewardModulation(True/False)")
    print("    enableStructuralPlasticity(True/False)")
    print("    enableDevelopment(True/False)")
    print("    enableCuriosity(True/False)")
    print()
    
    print("SimpleWorld Class:")
    print("  Creation: world = pynlm.createSimpleWorld()")
    print("  Configuration: world.configure(width, height, visionWidth, visionHeight)")
    print("  Control: world.reset()")
    print("  Update: world.update(timestep)")
    print("  Agent interaction:")
    print("    world.setAgentStart(x, y)")
    print("    world.applyMotorCommand(motor_cmd, time)")
    print("  State access:")
    print("    percept = world.getSensoryPercept()")
    print("    body = world.getAgentBody()")
    print("  World objects:")
    print("    world.addObject(pynlm.WorldObject(x, y, type, value))")
    print("    world.removeObject(x, y)")
    print()

def example_enumerations():
    """Example 6: Enumerations"""
    print("=== Example 6: Enumerations ===")
    
    print("Neuron Types:")
    print(f"  pynlm.NeuronType.Excitatory = {pynlm.NeuronType.Excitatory}")
    print(f"  pynlm.NeuronType.Inhibitory = {pynlm.NeuronType.Inhibitory}")
    print(f"  pynlm.NeuronType.Sensory = {pynlm.NeuronType.Sensory}")
    print(f"  pynlm.NeuronType.Motor = {pynlm.NeuronType.Motor}")
    print(f"  pynlm.NeuronType.Modulatory = {pynlm.NeuronType.Modulatory}")
    print(f"  pynlm.NeuronType.Internal = {pynlm.NeuronType.Internal}")
    print()
    
    print("Developmental Stages:")
    print(f"  pynlm.DevelopmentalStage.Initial = {pynlm.DevelopmentalStage.Initial}")
    print(f"  pynlm.DevelopmentalStage.CriticalPeriod = {pynlm.DevelopmentalStage.CriticalPeriod}")
    print(f"  pynlm.DevelopmentalStage.Maturation = {pynlm.DevelopmentalStage.Maturation}")
    print(f"  pynlm.DevelopmentalStage.Adult = {pynlm.DevelopmentalStage.Adult}")
    print(f"  pynlm.DevelopmentalStage.Aging = {pynlm.DevelopmentalStage.Aging}")
    print()
    
    print("Action Types:")
    print(f"  pynlm.ActionType.MoveForward = {pynlm.ActionType.MoveForward}")
    print(f"  pynlm.ActionType.TurnLeft = {pynlm.ActionType.TurnLeft}")
    print(f"  pynlm.ActionType.TurnRight = {pynlm.ActionType.TurnRight}")
    print(f"  pynlm.ActionType.Interact = {pynlm.ActionType.Interact}")
    print(f"  pynlm.ActionType.Eat = {pynlm.ActionType.Eat}")
    print()

def main():
    """Run all examples"""
    print("NLM (Neural Learning Machine) Python Interface Examples")
    print("=" * 60)
    print()
    
    # Check if pynlm module is available
    try:
        # These are just examples - the actual bindings need to be implemented
        print("Note: The actual pybind11 bindings for pynlm are not yet implemented.")
        print("These examples show the intended API based on the documentation.")
        print()
        print("To use NLM with Python:")
        print("1. Build the Python bindings: pip install .")
        print("2. Import pynlm: import pynlm")
        print("3. Use the examples below as a guide")
        print()
        
        # Run simplified examples that don't require the actual bindings
        print("=== Simplified Usage Examples ===")
        print("The following shows how NLM would be used in Python:")
        print()
        print(">>> import pynlm")
        print(">>> config = pynlm.createDefaultConfig()")
        print(">>> brain = pynlm.createBrain(config)")
        print(">>> brain.initialize()")
        print(">>> for i in range(100):")
        print("...     brain.step(i)")
        print(">>> print(f\"Total spikes: {brain.getTotalSpikeCount()}\")")
        print()
        
    except ImportError as e:
        print(f"Error: {e}")
        print("The pynlm module is not available.")
        print("Please build the Python bindings first.")
        print("Run: pip install scikit-build-core pybind11 numpy")
        print("Then: pip install .")

if __name__ == "__main__":
    main()