"""Comprehensive NLM Python Usage Examples

This module provides example scripts demonstrating how to use the enhanced
NLM Python bindings with all available functionality.
"""

import pynlm as nlm

def basic_brain_example():
    """Example 1: Basic brain simulation with neural elements"""
    print("=== Basic Brain Example ===")
    
    # Create brain with neural elements
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Add neural regions
    cortex_id = brain.addRegion("Cortex")
    hippocampus_id = brain.addRegion("Hippocampus")
    
    # Create populations in regions
    layer_2_id = brain.getRegion(cortex_id).addPopulation(100, nlm.NeuronType.Excitatory)
    layer_5_id = brain.getRegion(cortex_id).addPopulation(50, nlm.NeuronType.Inhibitory)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
    print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes fired")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def working_memory_example():
    """Example 2: Working memory with neural dynamics"""
    print("=== Working Memory Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create working memory
    from nlm import NeuralWorkingMemory
    working_memory = NeuralWorkingMemory()
    working_memory.initialize(brain.get_ptr())  # Assuming get_ptr() method exists
    
    # Store pattern in working memory
    pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
    working_memory.store(pattern, strength=1.0)
    
    # Retrieve and modify
    retrieved = working_memory.retrieve()
    print(f"Stored pattern: {pattern}")
    print(f"Retrieved pattern: {retrieved}")
    
    working_memory.update(0.1)  # Update with decay
    print(f"After update: {working_memory.retrieve()}")
    print()

def neuromodulation_example():
    """Example 3: Neuromodulation systems"""
    print("=== Neuromodulation Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain for neuromodulation
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation with neuromodulation
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.1 if step % 10 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print neuromodulation state
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    
    print()

def prediction_system_example():
    """Example 4: Prediction system"""
    print("=== Prediction System Example ===")
    
    from nlm import PredictionSystem
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create prediction system
    prediction_system = PredictionSystem()
    
    # Create dummy sensory inputs for demonstration
    vision1 = nlm.Vision(8, 8)
    vision1.setData([0.1] * 64)
    
    vision2 = nlm.Vision(8, 8)
    vision2.setData([0.2] * 64)
    
    # Make prediction
    predicted = prediction_system.predictNextState(vision1)
    print(f"Predicted state: {predicted.getData()[:5]}...")  # First 5 values
    
    # Update with actual observation
    prediction_system.updatePredictions(predicted, vision2)
    print(f"Prediction error: {prediction_system.getPredictionError():.3f}")
    print(f"Prediction confidence: {prediction_system.getConfidence():.3f}")
    
    # Train prediction
    prediction_system.train(vision2)
    print()

def plasticity_example():
    """Example 5: Plasticity systems"""
    print("=== Plasticity Systems Example ===")
    
    config = nlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Enable plasticity through agent brain
    agent = nlm.createAgentBrain(brain)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation to demonstrate plasticity
    for step in range(30):
        brain.step(step)
        
    print(f"Total synapses after plasticity: {brain.getTotalSynapseCount()}")
    print(f"E/I balance ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()

def complete_agent_example():
    """Example 6: Complete agent with all systems"""
    print("=== Complete Agent Example ===")
    
    def run_complete_agent(num_steps=200):
        """Run a complete agent simulation with all NLM systems"""
        
        # 1. Create configuration with all systems enabled
        config = nlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # 2. Create brain and initialize
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent brain interface
        agent = nlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = nlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        world.setRandomSeed(42)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        
        # 6. Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
        print(f"World: {world.getWidth()}x{world.getHeight()}")
        
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
            
            # Apply reward modulation (simulate reward from world interaction)
            reward = 0.0
            if world.getAgentBody().x > 5 and world.getAgentBody().x < 25:
                reward = 0.1  # Small reward for staying in bounds
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress periodically
            if step % 20 == 0:
                print(f"\nStep {step}:")
                print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Prediction Error: {agent.getPredictionError():.3f}")
                print(f"  Development Stage: {brain.getDevelopmentalStage()}")
                print(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        print(f"\n=== Simulation Complete ===")
        print(f"Final statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Final development stage: {brain.getDevelopmentalStage()}")
        
        return brain, agent, world
    
    # Run the simulation
    brain, agent, world = run_complete_agent(200)
    print()

def interactive_example():
    """Example 7: Interactive usage with real-time feedback"""
    print("=== Interactive Example ===")
    print("This example shows how to use NLM in an interactive manner.")
    print("In practice, you would integrate this with a game engine or visualization system.")
    print()
    
    # Create simple interactive setup
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    world = nlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Interactive setup complete!")
    print("You can now:")
    print("  1. Process sensory input: agent.processSensoryInput(world.getSensoryPercept())")
    print("  2. Run brain step: brain.step(step)")
    print("  3. Get action: agent.decodeMotorCommand()")
    print("  4. Apply action: world.applyMotorCommand(action, time)")
    print("  5. Get brain statistics for visualization")
    print()

if __name__ == "__main__":
    print("NLM Python Bindings - Comprehensive Examples")
    print("=" * 50)
    print()
    
    basic_brain_example()
    working_memory_example()
    neuromodulation_example()
    prediction_system_example()
    plasticity_example()
    complete_agent_example()
    interactive_example()
    
    print("All examples completed successfully!")
    print()
    print("To use these examples in your own code:")
    print("  1. Import pynlm: import pynlm as nlm")
    print("  2. Choose an example function")
    print("  3. Customize parameters as needed")
    print("  4. Integrate with your application")
