#!/usr/bin/env python3
"""
NLM Phase 6 Python Examples - Advanced Usage

This file demonstrates comprehensive examples of NLM Phase 6 features.
Copy and paste these examples to try out the integrated brain systems.

Each example shows different aspects of the Phase 6 brain integration:
- Memory systems (working and episodic memory)
- Neuromodulation (dopamine, curiosity, novelty)
- Development and maturation
- Prediction and planning
- Structural plasticity
- Checkpoint save/load and replay
- Complete world interaction with agents
"""

import pynlm
import time

def example_1_basic_phase6_simulation():
    """
    Example 1: Basic Phase 6 simulation with all brain systems
    Demonstrates working memory, neuromodulation, and development
    """
    print("=== Example 1: Basic Phase 6 Simulation ===")
    
    # Create a Phase 6-configured brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    
    # Enable all Phase 6 features
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("plasticity.structural.enable", True)
    config.set("brain.development.enabled", True)
    config.set("brain.working_memory.capacity", 100)
    config.set("brain.episodic_memory.capacity", 500)
    config.set("prediction.enabled", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all agent features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Initialized: {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Memory: Working={brain.getWorkingMemory() is not None}, "
          f"Episodic={brain.getEpisodicMemory() is not None}")
    print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"Development: {brain.getDevelopmentalStage()}")
    
    # Run simulation with monitoring
    print("\nRunning Phase 6 simulation...")
    start_time = time.time()
    
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Monitor progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    
    print(f"\nSimulation complete!")
    print(f"Duration: {end_time - start_time:.2f}s")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Total reward: {agent.getNeuromodulationLevel():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")


def example_2_learning_with_memory():
    """
    Example 2: Learning with memory systems
    Shows how working memory and episodic memory work together
    """
    print("\n=== Example 2: Learning with Memory Systems ===")
    
    # Configure for memory-intensive learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.working_memory.capacity", 200)
    config.set("brain.episodic_memory.capacity", 1000)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Creating learning experiences...")
    
    # Create multiple episodes to learn from
    for episode in range(8):
        print(f"\nEpisode {episode + 1}:")
        
        # Reset world for each episode
        world.reset()
        agent.reset()
        
        # Run episode with sensory exploration
        for step in range(150):
            world.update(0.1)
            
            # Check for novel stimuli (curiosity-driven behavior)
            percept = world.getSensoryPercept()
            if step % 50 == 0:
                novelty = agent.getNoveltyLevel()
                print(f"  Step {step}: Novelty={novelty:.3f}, "
                      f"Reward={percept.getInternal()[0] if percept.getInternal() else 0.0:.3f}")
            
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Check memory after each episode
        wm = brain.getWorkingMemory()
        em = brain.getEpisodicMemory()
        
        if wm:
            print(f"  Working Memory: {wm.getActiveTraces()} active traces")
        
        if em:
            print(f"  Episodic Memory: {em.getEpisodeCount()} episodes stored")
    
    # Test memory recall
    print(f"\nFinal memory status:")
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    
    if wm:
        print(f"Working Memory capacity: {wm.getCapacity()}")
        print(f"Active traces: {wm.getActiveTraces()}")
    
    if em:
        print(f"Episodic Memory capacity: {em.getCapacity()}")
        print(f"Stored episodes: {em.getEpisodeCount()}")
        
        # Test replay
        if em.getEpisodeCount() > 0:
            replay_episodes = em.getEpisodesForReplay(3)
            print(f"Can replay: {len(replay_episodes)} episodes")
    
    print("\nMemory learning complete!")


def example_3_development_and_planning():
    """
    Example 3: Development and neural planning
    Shows how the brain develops and plans actions over time
    """
    print("\n=== Example 3: Development and Neural Planning ===")
    
    # Configure for developmental learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 400)
    config.set("brain.development.critical_period_start", 0.1)
    config.set("brain.development.critical_period_end", 2.0)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("prediction.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=18, height=18, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    print("Monitoring brain development over time...")
    
    # Developmental timeline
    for phase in range(5):
        print(f"\nDevelopment Phase {phase + 1}: {brain.getDevelopmentalStage()}")
        
        # Start new world for each development phase
        world.reset()
        agent.reset()
        
        # Phase-specific simulation
        for step in range(100):
            world.update(0.1)
            
            # Track development progress
            if step % 25 == 0:
                print(f"  Step {step}:")
                print(f"    Developmental age: {agent.getDevelopmentalStage()}")
                print(f"    Plasticity modifier: {brain.getBrain()->getDevelopmentSystem()->getPlasticityModifier():.3f}")
                print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
                print(f"    Prediction confidence: {agent.getPredictionError():.3f}")
            
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and update development
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Check if development stage changed
            current_stage = brain.getDevelopmentalStage()
            if current_stage != agent.getDevelopmentalStage():
                print(f"    Development stage changed to: {current_stage}")
    
    print(f"\nFinal development status: {brain.getDevelopmentalStage()}")
    print("Developmental learning complete!")


def example_4_checkpoint_and_replay():
    """
    Example 4: Checkpoint save/load and memory replay
    Demonstrates persistence and replay capabilities
    """
    print("\n=== Example 4: Checkpoint and Replay ===")
    
    # Create brain with checkpointing enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.episodic_memory.capacity", 500)
    config.set("plasticity.structural.enable", True)
    config.set("brain.development.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Running initial learning phase...")
    
    # Initial learning phase
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Check memory after learning
    em = brain.getEpisodicMemory()
    if em:
        print(f"Episodic memory has {em.getEpisodeCount()} episodes")
    
    # Save checkpoint
    print("\nSaving checkpoint...")
    checkpoint_path = "/tmp/nlm_phase6_checkpoint.bin"
    if brain.save(checkpoint_path):
        print(f"✓ Checkpoint saved to {checkpoint_path}")
    else:
        print("✗ Failed to save checkpoint")
        return
    
    # Create new brain and load checkpoint
    print("Creating new brain and loading checkpoint...")
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 200)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    if brain2.load(checkpoint_path):
        print("✓ Checkpoint loaded successfully")
        
        # Check if loaded brain has similar state
        print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
        print(f"Loaded brain neurons: {brain2.getTotalNeuronCount()}")
        print(f"Original brain spikes: {brain.getTotalSpikeCount()}")
        print(f"Loaded brain spikes: {brain2.getTotalSpikeCount()}")
        
        # Test replay with loaded brain
        if em:
            print(f"\nTesting replay with {em.getEpisodeCount()} stored episodes...")
            replay_episodes = em.getEpisodesForReplay(2)
            print(f"Can retrieve {len(replay_episodes)} episodes for replay")
            
    else:
        print("✗ Failed to load checkpoint")
    
    print("\nCheckpoint and replay test complete!")


def example_5_advanced_agent_behavior():
    """
    Example 5: Advanced agent with curiosity and exploration
    Demonstrates complex agent behavior with all neuromodulation systems
    """
    print("\n=== Example 5: Advanced Agent Behavior ===")
    
    # Create sophisticated agent configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 350)
    config.set("brain.region_count", 3)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.3)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.2)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("brain.development.enabled", True)
    config.set("prediction.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running advanced agent with curiosity-driven exploration...")
    print(f"Initial neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Initial curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Initial novelty: {agent.getNoveltyLevel():.3f}")
    
    # Advanced agent loop with exploration/exploitation balance
    exploration_rate = 0.3
    exploitation_rate = 0.7
    
    for step in range(500):
        world.update(0.1)
        
        # Curvilinear exploration: balance exploration vs exploitation
        if agent.getCuriosityLevel() > 0.5 or (step % 50 == 0 and step > 0):
            # Exploration phase: try novel actions
            print(f"  Step {step}: Exploring (curiosity={agent.getCuriosityLevel():.3f})")
            action = agent.decodeMotorCommand()  # Let curiosity guide action selection
        else:
            # Exploitation phase: use learned actions
            action = agent.decodeMotorCommand()
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply complex reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = 0.0  # Simplified prediction
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Monitor agent state
        if step % 100 == 0:
            print(f"\nStep {step} Summary:")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    print(f"\nAdvanced agent simulation complete!")
    print(f"Final performance:")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction accuracy: {1.0 - agent.getPredictionError():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")


def main():
    """
    Run all examples to demonstrate Phase 6 capabilities
    """
    print("NLM Phase 6 Python Examples")
    print("=" * 50)
    print("This script demonstrates the advanced features of NLM Phase 6.")
    print("Each example shows different aspects of the integrated brain systems.")
    print()
    
    try:
        example_1_basic_phase6_simulation()
        example_2_learning_with_memory()
        example_3_development_and_planning()
        example_4_checkpoint_and_replay()
        example_5_advanced_agent_behavior()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nKey takeaways from these examples:")
        print("1. Phase 6 integrates memory, neuromodulation, and development")
        print("2. Working memory tracks current processing")
        print("3. Episodic memory stores past experiences")
        print("4. Neuromodulation (dopamine, curiosity, novelty) drives learning")
        print("5. Development affects plasticity over time")
        print("6. Prediction systems anticipate future events")
        print("7. Structural plasticity allows brain growth")
        print("8. Checkpointing enables persistent learning")
        print("9. Replay strengthens important memories")
        print("10. Curiosity drives exploration and discovery")
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        print("Please check your installation and try again.")


if __name__ == "__main__":
    main()