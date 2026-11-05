#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python Examples

This script demonstrates the complete NLM Python API with practical examples
of how to use the neural simulation framework for autonomous agent behavior.

The NLM framework provides:
- Event-driven spiking neural network simulation
- Multiple memory systems (working, episodic, associative)
- Neuromodulation (reward, curiosity, novelty)
- Cognitive mechanisms (planning, concept formation, attention)
- Integration with real-world environments

This script covers:
1. Basic brain creation and simulation
2. Environment-agent interaction
3. Learning through experience
4. Memory system usage
5. Development and maturation
6. Curiosity-driven exploration
"""

import pynlm

def create_simple_brain_example():
    """Example 1: Create and initialize a basic brain"""
    print("=== Example 1: Basic Brain Creation ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.connection_probability", 0.1)
    config.set("random_seed", 42)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def basic_simulation_example():
    """Example 2: Run basic simulation steps"""
    print("=== Example 2: Basic Simulation ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    print("Running simulation for 100 steps...")
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print()

def agent_world_example():
    """Example 3: Create agent and environment"""
    print("=== Example 3: Agent-World Integration ===")
    
    # Create components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Configure subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run episode
    print("Running episode for 500 steps...")
    for step in range(1, 501):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Run simulation
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == �มือนกัน:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev stage: {agent.getDevelopmentalState()}")
    
    print()

def learning_example():
    """Example 4: Learning through experience"""
    print("=== Example 4: Learning and Plasticity ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Run a learning session
    print("Running learning session with repeated training...")
    for episode in range(3):
        print(f"Episode {episode + 1}:")
        
        # Reset for new episode
        agent.reset()
        
        # Run training steps
        for step in range(500):
            # Create simple world for training
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=.testing, visionHeight=testing
            world.reset()
            
            # Create training data (simple exploration)
            import random
            x, y = random.uniform(0, 10), random.uniform(0, 10)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
            
            # Process this as sensory input
            agent.initialize(world)
            agent.enableCuriosity(True)
            
            for world_step in range(20):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(world_step)
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print learning statistics
        print(f"  Final spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print()

def memory_example():
    """Example 5: Using memory systems"""
    print("=== Example 5: Memory Systems ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Initial memory states:")
    print(f"  Working memory capacity: {working_memory.getCapacity()}")
    print(f"  Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"  Working memory active neurons: {len(working_memory.getMemoryNeurons())}")
    
    # Simulate and create memories
    for step in range(100):
        brain.step(step)
        
        # Simulate working memory activity
        if step % 10 == 0:  # Periodically create memory events
            working_memory.storeToNeuron(brain.getRegionIds()[0], 1.0)
        
        # Periodically create episodic memory
        if step % 50 == 0 and step > 0:
            # Create simple episode
            import random
            episode = episodic_memory.createEpisode()
            episode.positionX = random.uniform(0, 20)
            episode.positionY = random.uniform(0, 20)
            episode.action = pynlm.ActionType.Interact
            episode.reward = agent.getNeuromodulationLevel()
            episode.novelty = agent.getNoveltyLevel()
            episodic_memory.storeEpisode(episode)
    
    # Print memory statistics
    print("\nFinal memory statistics:")
    print(f"  Working memory active: {len(working_memory.getMemoryNeurons())} neurons")
    print(f"  Episodic memory episodes: {len(episodic_memory.getEpisodes())}")
    print(f"  Associative memory patterns: {len(associative_memory.getPatternAssociations())}")
    
    # Show recent episodic memories
    if episodic_memory.getEpisodes().size() > 0:
        print("\nRecent episodes:")
        for i, episode in enumerate(episodicMemory.getEpisodes().limit(5)):
            print(f"  Episode {i}: pos({episode.positionX:.1f},{episode.positionY:.1f}), reward={episode.reward:.2f}")
    
    print()

def development_example():
    """Example 6: Development and maturation"""
    print("=== Example 6: Development and Maturation ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    print("Development progression over time:")
    for stage_num in range(1, 6):  # 5 development stages
        # Update development
        agent.updateDevelopment(1000.0)  # Update over 1000 simulation steps
        
        # Print developmental statistics
        print(f"Stage {stage_num} (developmental time {stage_num * 1000}):")
        print(f"  Stage: {agent.getDevelopmentalStage()}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        
        # Simulate a few steps
        for step in range(100):
            brain.step(step)
    
    print()

def curiosity_driven_example():
    """Example 7: Curiosity-driven exploration"""
    print("=== Example 7: Curiosity-Driven Exploration ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=7, visionHeight=7)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent.initialize(world)
    
    print("Running curiosity-driven exploration...")
    exploration_steps = 0
    novel_events = 0
    
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Check curiosity level
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        if curiosity > 0.5:
            exploration_steps += 1
            print(f"Step {step}: High curiosity ({curiosity:.3f}), exploring...")
        
        # Novel event detected
        if novelty > 0.3:
            novel_events += 1
            print(f"Step {step}: Novel event detected (novelty={novelty:.3f})")
        
        # Run brain
        brain.step(step)
        
        # Generate motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    print(f"\nExploration results:")
    print(f"  Total steps: {step + 1}")
    print(f"  Curiosity-driven steps: {exploration_steps}")
    print(f"  Novel events detected: {novel_events}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print()

def checkpoint_example():
    """Example 8: Checkpoint save/load functionality"""
    print("=== Example 8: Checkpoint Save/Load ===")
    
    # Create and train first brain
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    # Run some simulation
    for step in range(200):
        brain1.step(step)
    
    print(f"Initial brain trained: {brain1.getTotalSpikeCount()} spikes")
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint.test"
    save_success = brain1.save(checkpoint_file)
    print(f"Saved checkpoint: {'Success' if save_success else 'Failed'}")
    
    if save_success:
        # Create new brain and load checkpoint
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        load_success = brain2.load(checkpoint_file)
        print(f"Loaded checkpoint: {'Success' if load_success else 'Failed'}")
        
        if load_success:
            print(f"Loaded brain has: {brain2.getTotalSpikeCount()} spikes")
            print(f"Spike count preserved: {'Yes' if brain2.getTotalSpikeCount() == brain1.getTotalSpikeCount() else 'No'}")
    
    print()

if __name__ == "__main__":
    print("NLM Python Examples")
    print("===================")
    print()
    
    try:
        create_simple_brain_example()
        basic_simulation_example()
        agent_world_example()
        learning_example()
        memory_example()
        development_example()
        curiosity_driven_example()
        checkpoint_example()
        
        print("All examples completed successfully!")
        print("\nKey observations:")
        print("1. Brains can be created and initialized with custom configurations")
        print("2. Simulation runs event-driven with spike propagation")
        print("3. Agents can interact with environments through sensory input/output")
        print("4. Learning occurs through neuromodulation and plasticity")
        print("5. Memory systems store and retrieve experience")
        print("6. Development stages affect behavior and learning capabilities")
        print("7. Curiosity drives exploration of novel environments")
        print("8. Checkpointing preserves learned behaviors and states")
        
    except Exception as e:
        print(f"Example failed with error: {e}")
        import traceback
        traceback.print_exc()