#!/usr/bin/env python3
"""
Example 3: Memory System Usage

This example demonstrates how to use NLM's memory systems (working memory, 
episodic memory, associative memory) to store and retrieve information.
"""

import pynlm
import numpy as np

def memory_systems_usage():
    """Demonstrate memory system functionality."""
    
    print("=== Example 3: Memory System Usage ===")
    
    # Create brain with memory
    config = pynlm.createDefaultConfig()
    config.set("working.memory.capacity", 200)
    config.set("working.memory.decay_rate", 0.02)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable development and curiosity for learning
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Memory system configuration:")
    print(f"  Working memory capacity: {config.get('working.memory.capacity')}")
    print(f"  Working memory decay rate: {config.get('working.memory.decay_rate')}")
    
    # Store and retrieve patterns in working memory
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print("\n--- Working Memory Operations ---")
        
        # Store several patterns
        patterns = [
            np.random.randn(10),
            np.random.randn(10),
            np.random.randn(10)
        ]
        
        for i, pattern in enumerate(patterns):
            working_memory.store(pattern, strength=1.0 + i*0.2)
            print(f"  Stored pattern {i+1} with strength {1.0 + i*0.2:.1f}")
        
        # Retrieve patterns
        print("\nRetrieving patterns:")
        for i in range(3):
            retrieved = working_memory.retrieve()
            if retrieved:
                print(f"  Retrieved pattern {i+1}")
                
                # Get activation information
                activation = working_memory.getNeuronActivation()
                if activation:
                    print(f"    Max activation: {max(activation.values()):.3f}")
        
        # Test competition
        print("\n--- Memory Competition ---")
        working_memory.runCompetition()
        active_traces = working_memory.getActiveTraces()
        print(f"Active memory traces: {active_traces}")
        
        # Test pattern strengthening
        print("\n--- Memory Strengthening ---")
        working_memory.strengthenMemory(2.0)
        print("Memory strengthened by factor 2.0")
    
    # Episodic memory operations
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print("\n--- Episodic Memory Operations ---")
        
        # Store episodes
        episodes = []
        for i in range(5):
            episode_data = {
                'step': i * 20,
                'sensory_pattern': np.random.randn(8),
                'reward': np.random.random() * 0.5,
                'action': i % 3
            }
            episodic_memory.storeEpisode(episode_data)
            episodes.append(episode_data)
            print(f"  Stored episode {i+1}: step {i*20}, reward {episode_data['reward']:.2f}")
        
        # Retrieve similar episodes
        query_pattern = np.random.randn(8)
        similar_episodes = episodic_memory.retrieveSimilar(query_pattern, 2)
        print(f"\nRetrieved {len(similar_episodes)} similar episodes")
        
        # Get recent episodes
        recent = episodic_memory.getRecentEpisodes(2)
        print(f"Last 2 episodes: steps {[e['step'] for e in recent]}")
    
    # Associative memory operations
    associative_memory = brain.getAssociativeMemory()
    if associative_memory:
        print("\n--- Associative Memory Operations ---")
        
        # Create associations between patterns
        pattern_a = np.random.randn(5)
        pattern_b = np.random.randn(5)
        
        associative_memory.associate(pattern_a, pattern_b, 1.0)
        print(f"Created association between pattern A and B")
        
        # Test retrieval
        test_pattern = pattern_a.copy()
        retrieved = associative_memory.retrieve(test_pattern, 1)
        print(f"Retrieved patterns for pattern A: {len(retrieved)} patterns")
        
        # Get association strength
        strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
        print(f"Association strength between A and B: {strength:.3f}")
    
    # Run simulation to see memory in action
    print("\n--- Simulation with Memory ---")
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 50 == 0:
            wm_active = working_memory.getActiveTraces() if working_memory else 0
            em_count = episodic_memory.getEpisodeCount() if episodic_memory else 0
            print(f"  Step {step}: {wm_active} working memory traces, {em_count} episodic memories")
    
    print("\nMemory systems demonstration complete!")
    return brain, world, agent

if __name__ == "__main__":
    memory_systems_usage()