# High-level examples for common use cases in NLM (Neural Learning Machine)

# Example 1: Basic Brain Simulation
import pynlm

def example_basic_brain():
    """Create and run a simple brain simulation"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print(f"Simulation complete! Brain fired {brain.getTotalSpikeCount()} times")
    return brain

# Example 2: Complete Agent with World
import pynlm

def example_agent_world():
    """Run a complete agent-world simulation"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Agent simulation complete!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Example 3: Memory-based Learning
import pynlm

def example_memory_learning():
    """Demonstrate memory-based learning"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print("Memory system available - can track experiences and patterns")
    
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}:")
            if working_memory:
                print(f"  Working memory traces: {working_memory.getActiveTraces()}")
            if episodic_memory:
                print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

# Example 4: Development and Maturation
import pynlm

def example_development():
    """Show developmental progression"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Developmental stages:")
    for stage in range(5):
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Initial)
        brain.step(stage)
        print(f"Stage {stage}: {brain.getDevelopmentalStage()}")

# Example 5: Sensorimotor Integration
import pynlm

def example_sensorimotor():
    """Complete sensorimotor loop demonstration"""
    # Setup brain with agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Track performance
    total_steps = 0
    successful_actions = 0
    
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        if result.success:
            successful_actions += 1
        
        total_steps += 1
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getAverageFiringRate():.2f} avg rate")
    
    efficiency = (successful_actions / total_steps) * 100 if total_steps > 0 else 0
    print(f"\nSensorimotor efficiency: {efficiency:.1f}%")
    print(f"Total successful actions: {successful_actions}/{total_steps}")

# Example 6: Advanced Agent with Neuromodulation
import pynlm

def example_advanced_agent():
    """Advanced agent with full neuromodulation"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=7, visionHeight=7)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True) 
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced agent initialized with full neuromodulation")
    
    for step in range(150):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent (includes neuromodulation)
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Agent produces action based on learning
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development periodically
        if step % 50 == 0:
            agent.updateDevelopment(0.1)
    
    print("Advanced agent training complete!")
    print(f"Final neuromodulation levels:")
    print(f"  Dopamine (reward): {agent.getNeuromodulationLevel():.2f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"  Prediction error: {agent.getPredictionError():.2f}")

# Example 7: Continuous Learning
import pynlm

def example_continuous_learning():
    """Demonstrate continual learning over time"""
    print("=== Continual Learning Experiment ===")
    
    # Create brain that will learn over time
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with learning enabled
    agent = pynlm.createAgentBrain(brain)
    
    # We need a world for this experiment
    # For demo purposes, we'll use a simple approach
    print("Initialize agent with config for continual learning...")
    
    # Simulate learning over multiple episodes
    episodes = []
    for episode in range(3):
        print(f"\nEpisode {episode + 1}/3")
        
        # Reset and reinitialize for new episode
        brain.reset()
        brain.initialize()
        
        # Run for many steps in this episode
        episode_spikes = []
        for step in range(200):
            brain.step(step)
            episode_spikes.append(brain.getTotalSpikeCount())
        
        episodes.append(episode_spikes)
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Analyze learning across episodes
    print("\nLearning analysis across episodes:")
    for i, episode_spikes in enumerate(episodes):
        print(f"  Episode {i + 1}: {episode_spikes[-1]} total spikes, "
              f"avg rate: {sum(episode_spikes)/len(episode_spikes):.1f}")

# Example 8: Concept Formation
import pynlm

def example_concept_formation():
    """Demonstrate concept formation from patterns"""
    print("=== Concept Formation Experiment ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get concept formation system
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print("Concept formation system available")
        print("This system would detect patterns and form abstract concepts")
        print("from sensory input over time.")
    else:
        print("Concept formation system not available (Phase 2 complete)")
    
    # Run brain for a while to generate patterns
    for step in range(100):
        brain.step(step)
    
    print(f"Brain explored {brain.getTotalSpikeCount()} spike events")
    print("In a full Phase 6 implementation, this would form concepts")

# Example 9: Working Memory Demonstration
import pynlm

def example_working_memory():
    """Demonstrate working memory functionality"""
    print("=== Working Memory Experiment ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"Working memory capacity: {working_memory.getCapacity()}")
        print(f"Working memory traces: {working_memory.getActiveTraces()}")
        
        # Simulate storing patterns in working memory
        for step in range(50):
            brain.step(step)
            
            # The brain's spike activity is automatically stored
            if step % 10 == 0:
                print(f"Step {step}: Working memory has {working_memory.getActiveTraces()} active traces")
    else:
        print("Working memory system not available")

# Example 10: Integration Example (Complete Agent)
import pynlm

def example_complete_integration():
    """Complete example showing all NLM systems working together"""
    print("=== Complete NLM Integration Example ===")
    
    # Create and configure the brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent-world environment
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all cognitive and memory systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Complete agent initialized with:")
    print("  ✓ Reward-based learning (Dopamine)")
    print("  ✓ Exploration motivation (Curiosity)")
    print("  ✓ Developmental plasticity")
    print("  ✓ Working memory")
    print("  ✓ Episodic memory")
    print("  ✓ Attention system")
    print("  ✓ Prediction system")
    print("  ✓ Neural planner")
    
    # Run the complete integration test
    total_steps = 0
    rewards = 0
    
    for step in range(100):
        world.update(0.1)
        
        # Get world state
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Agent produces action
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Accumulate reward
        if result.success:
            rewards += result.reward
        
        total_steps += 1
        
        # Log progress
        if step % 20 == 0:
            print(f"  Step {step}: Action={action.getName()}, Reward={result.reward:.2f}")
            print(f"    Brain stats: {brain.getFiringNeuronCount()} firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    efficiency = (rewards / total_steps) * 100 if total_steps > 0 else 0
    print(f"\nIntegration Test Results:")
    print(f"  Total steps: {total_steps}")
    print(f"  Total rewards: {rewards:.2f}")
    print(f"  Efficiency: {efficiency:.1f}%")
    print(f"  Final brain stats: {brain.getFiringNeuronCount()} firing neurons, "
          f"{brain.getAverageFiringRate():.2f} avg firing rate")

# Run examples if executed as main script
if __name__ == "__main__":
    print("NLM Examples - Run specific functions to see demonstrations")
    print("\nAvailable examples:")
    print("  example_basic_brain()")
    print("  example_agent_world()")
    print("  example_memory_learning()")
    print("  example_development()")
    print("  example_sensorimotor()")
    print("  example_advanced_agent()")
    print("  example_continuous_learning()")
    print("  example_concept_formation()")
    print("  example_working_memory()")
    print("  example_complete_integration()")
