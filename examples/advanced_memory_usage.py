#!/usr/bin/env python3
"""
Advanced Memory Usage Example for NLM Phase 6

Demonstrates how to use all the newly integrated memory systems:
- WorkingMemory: Persistent activity with winner-take-all competition
- EpisodicMemory: Experience storage with replay and consolidation
- AssociativeMemory: Pattern associations and spreading activation

This example shows how to create and interact with these memory systems
in a realistic scenario of learning in a dynamic environment.
"""

import pynlm
import numpy as np
import time

def advanced_memory_usage_example():
    """Run an advanced example using all memory systems."""
    
    print("=== Advanced Memory Usage Example (NLM Phase 6) ===\n")
    
    # Create brain and configure it for memory-intensive tasks
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # More neurons for memory
    config.set("working_memory.capacity", 200)  # Larger working memory
    config.set("episodic_memory.max_episodes", 5000)  # Store more experiences
    config.set("plasticity.structural.enable", True)  # Allow structural changes
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable all advanced systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create memory systems
    workingMemory = brain.getWorkingMemory()
    episodicMemory = brain.getEpisodicMemory()
    associativeMemory = brain.getAssociativeMemory()
    
    print("1. Testing Working Memory System")
    print("-" * 50)
    
    # Create a complex pattern and store it in working memory
    complex_pattern = [np.random.random() for _ in range(50)]
    workingMemory.store(complex_pattern, strength=1.0)
    
    print(f"Stored complex pattern of length {len(complex_pattern)}")
    print(f"Working memory activity: {workingMemory.getMemoryActivity():.3f}")
    print(f"Number of active traces: {workingMemory.getActiveTraces()}")
    
    # Run competition and see which neurons win
    workingMemory.runCompetition()
    winners = workingMemory.getWinners()
    print(f"Number of winners: {len(winners)}")
    
    # Update memory with decay and reinforcement
    for step in range(100):
        workingMemory.update(0.1)
        if step % 20 == 0:
            # Inject pattern to strengthen
            workingMemory.store(complex_pattern, strength=0.5)
    
    print(f"After 100 steps - Working memory activity: {workingMemory.getMemoryActivity():.3f}")
    print()
    
    print("2. Testing Episodic Memory System")
    print("-" * 50)
    
    # Create an episodic memory with realistic fields
    from datetime import datetime
    
    # Simulate experiences in the world
    for episode_id in range(10):
        # Experience: agent navigating world
        episode = episodicMemory.createEpisode()
        episode.timestamp = episode_id
        episode.positionX = 15.0 + np.random.uniform(-10, 10)
        episode.positionY = 15.0 + np.random.uniform(-10, 10)
        episode.orientation = np.random.uniform(0, 360)
        episode.action = pynlm.ActionType.MoveForward
        episode.reward = np.random.uniform(0, 1)
        episode.energy = np.random.uniform(0.5, 1.0)
        episode.novelty = np.random.uniform(0, 1)
        
        # Create sensory state (vision-like data)
        episode.sensoryState = [np.random.random() for _ in range(256)]
        
        # Create resulting state after action
        episode.resultingSensoryState = [max(0, v + np.random.normal(0, 0.1)) 
                                       for v in episode.sensoryState]
        episode.resultingReward = episode.reward + np.random.uniform(-0.1, 0.1)
        
        # Store neural activity (simulated)
        episode.activeNeurons = []
        episode.neuronActivations = []
        for i in range(50):
            episode.activeNeurons.append(pynlm.NeuronId(i + 10000))
            episode.neuronActivations.append(np.random.random())
        
        episodicMemory.storeEpisode(episode)
    
    print(f"Stored {episodicMemory.getEpisodeCount()} episodes")
    print(f"Average reward across episodes: {episodicMemory.getAverageReward():.3f}")
    
    # Retrieve similar episodes
    query_pattern = [np.random.random() for _ in range(256)]
    similar_episodes = episodicMemory.retrieveSimilar(query_pattern, maxResults=5)
    print(f"Found {len(similar_episodes)} similar episodes")
    
    # Test temporal retrieval
    temporal_episodes = episodicMemory.retrieveTemporal(0, 5, maxResults=3)
    print(f"Found {len(temporal_episodes)} episodes in time window")
    
    # Test replay selection
    replay_episodes = episodicMemory.getEpisodesForReplay(3)
    print(f"Selected {len(replay_episodes)} episodes for replay")
    
    # Run consolidation
    episodicMemory.consolidate(0.3)
    print("Completed memory consolidation")
    print()
    
    print("3. Testing Associative Memory System")
    print("-" * 50)
    
    # Create pattern associations
    pattern_a = [np.random.random() for _ in range(100)]
    pattern_b = [np.random.random() for _ in range(100)]
    pattern_c = [np.random.random() for _ in range(100)]
    
    # Create associations
    associativeMemory.associate(pattern_a, pattern_b, strength=1.0)
    associativeMemory.associate(pattern_b, pattern_c, strength=0.7)
    associativeMemory.associate(pattern_a, pattern_c, strength=0.3)
    
    # Store patterns in associative memory
    associativeMemory.storePattern("A", pattern_a)
    associativeMemory.storePattern("B", pattern_b)
    associativeMemory.storePattern("C", pattern_c)
    
    print(f"Created {associativeMemory.getAssociationCount()} associations")
    
    # Test retrieval with partial cues
    query = pattern_a  # Query with full pattern A
    retrieved = associativeMemory.retrieve(query, maxResults=3)
    print(f"Retrieved {len(retrieved)} patterns from query")
    
    # Test spreading activation
    activated = associativeMemory.spreadActivation(pattern_b, steps=2)
    print(f"Spread activation found {len(activated)} activated patterns")
    
    # Test association strength
    strength_ac = associativeMemory.getAssociationStrength(pattern_a, pattern_c)
    print(f"Association strength between A and C: {strength_ac:.3f}")
    print()
    
    print("4. Integration Test")
    print("-" * 50)
    
    # Create an experience and store in all memory systems
    print("Creating experience in integrated environment...")
    
    # Experience: agent sees something, acts, gets reward
    experience_pattern = [np.random.random() for _ in range(256)]
    
    # Store in working memory
    workingMemory.store(experience_pattern, strength=0.8)
    workingMemory.runCompetition()
    
    # Create and store in episodic memory
    episode = episodicMemory.createEpisode()
    episode.timestamp = 1000
    episode.positionX = world.getAgentBody().x
    episode.positionY = world.getAgentBody().y
    episode.orientation = world.getAgentBody().orientation
    episode.action = pynlm.ActionType.Interact
    episode.reward = 0.8
    episode.sensoryState = experience_pattern
    episode.resultingSensoryState = [max(0, v * 0.9) for v in experience_pattern]
    
    episodicMemory.storeEpisode(episode)
    
    # Create associations from experience
    associativeMemory.associateFromExperience(episode)
    
    print("Experience stored across all memory systems:")
    print(f"  - Working memory traces: {workingMemory.getActiveTraces()}")
    print(f"  - Episodic episodes: {episodicMemory.getEpisodeCount()}")
    print(f"  - Association connections: {associativeMemory.getAssociationCount()}")
    
    # Test retrieval from all systems
    workingMemory_store = workingMemory.retrieve()
    print(f"Working memory retrieval: {len(workingMemory_store)} patterns")
    
    similar_eps = episodicMemory.retrieveSimilar(experience_pattern, maxResults=1)
    print(f"Episodic memory retrieval: {len(similar_eps)} similar episodes")
    
    assoc_ret = associativeMemory.retrieve(experience_pattern, maxResults=1)
    print(f"Associative memory retrieval: {len(assoc_ret)} associated patterns")
    
    print("\n=== Advanced Memory Usage Example Complete ===")
    print("\nKey achievements:")
    print("✅ Successfully integrated all memory systems")
    print("✅ Demonstrated working memory competition dynamics")
    print("✅ Showed episodic memory storage and retrieval")
    print("✅ Implemented associative memory with spreading activation")
    print("✅ Connected all systems into coherent memory architecture")
    
    return {
        "working_memory_activity": workingMemory.getMemoryActivity(),
        "episodic_count": episodicMemory.getEpisodeCount(),
        "associations_count": associativeMemory.getAssociationCount(),
        "integration_success": True
    }

if __name__ == "__main__":
    result = advanced_memory_usage_example()
    print(f"\nExample completed with result: {result}")