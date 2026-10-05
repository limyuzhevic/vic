import pynlm
from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

def run_quick_test():
    """Run a quick smoke test to verify NLM installation."""
    print("Testing NLM installation...")
    print(f"NLM Version: {pynlm.__version__}")
    
    # Test basic functionality
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a few simulation steps
    for i in range(10):
        brain.step(i)
    
    print(f"Generated {brain.getTotalSpikeCount()} spikes in 10 steps")
    print("✓ Basic functionality test passed!")
    return True

def create_complete_agent(config=None):
    """
    Create a complete NLM agent with brain, world, and agent interface.
    
    Args:
        config: Optional configuration for the brain. If None, uses default.
        
    Returns:
        tuple: (brain, world, agent)
    """
    if config is None:
        config = createDefaultConfig()
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    # Create world
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, world, agent

def run_example_simulation(steps=100):
    """
    Run a complete agent simulation example.
    
    This example demonstrates the typical NLM agent workflow:
    1. Create brain, world, and agent
    2. Run simulation loop
    3. Process sensory input and generate actions
    4. Learn from experience
    """
    print(f"Running NLM agent simulation for {steps} steps...")
    
    brain, world, agent = create_complete_agent()
    
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Optional: Apply reward modulation
        # reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        # agent.applyRewardModulation(reward, 0.0)
        
        # Print progress periodically
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Total spikes: {brain.getTotalSpikeCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Dev Stage: {brain.getDevelopmentalStage().name}")
    
    print(f"✓ Simulation complete! {steps} steps processed.")
    return brain, world, agent

def demonstrate_brain_features():
    """
    Demonstrate various NLM brain features.
    """
    print("NLM Brain Features Demonstration")
    print("=" * 40)
    
    brain, world, agent = create_complete_agent()
    
    print("\n1. Memory Systems:")
    print(f"   - Working Memory: {type(agent.getBrain().getWorkingMemory()).__name__}")
    print(f"   - Episodic Memory: {type(agent.getBrain().getEpisodicMemory()).__name__}")
    print(f"   - Associative Memory: {type(agent.getBrain().getAssociativeMemory()).__name__}")
    
    print("\n2. Prediction Systems:")
    prediction_system = agent.getBrain().getPredictionSystem()
    print(f"   - Neural Prediction: {type(prediction_system).__name__}")
    
    print("\n3. Cognitive Systems:")
    print(f"   - Neural Planner: {type(agent.getBrain().getPlanner()).__name__}")
    print(f"   - Concept Formation: {type(agent.getBrain().getConceptFormation()).__name__}")
    print(f"   - Attention: {type(agent.getBrain().getAttention()).__name__}")
    
    print("\n4. Neuromodulation Systems:")
    print(f"   - Dopamine: {type(agent.getBrain().getDopamine()).__name__}")
    print(f"   - Curiosity: {type(agent.getBrain().getCuriosity()).__name__}")
    print(f"   - Novelty: {type(agent.getBrain().getNovelty()).__name__}")
    print(f"   - Prediction Error: {type(agent.getBrain().getPredictionErrorSignal()).__name__}")
    
    print("\n5. Development System:")
    dev_system = agent.getBrain().getDevelopmentSystem()
    print(f"   - Development Stage: {agent.getDevelopmentalStage().name}")
    
    print("\n✓ All brain features accessible and functional!")

if __name__ == "__main__":
    run_quick_test()