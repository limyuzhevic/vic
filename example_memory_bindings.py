#!/usr/bin/env python3
"""
Example usage of NLM Python bindings for memory systems.

This file demonstrates how to use the Python bindings for the three neural memory systems:
1. NeuralWorkingMemory - persistent activity patterns
2. NeuralEpisodicMemory - experience storage
3. NeuralAssociativeMemory - pattern associations
"""

import pynlm as nlm

def main():
    print("=" * 70)
    print("NLM Memory Systems Python Bindings - Example Usage")
    print("=" * 70)
    
    # Create configuration and brain
    print("\n1. Creating configuration and brain...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    print("   Brain initialized with", brain.getRegionCount(), "regions")
    print("   Total neurons:", brain.getTotalNeuronCount())
    
    # ===== NEURAL WORKING MEMORY EXAMPLES =====
    print("\n2. Neural Working Memory Examples")
    print("-" * 50)
    
    # Get working memory reference
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print("   Working memory initialized")
        print("   - Capacity:", working_memory.getCapacity())
        print("   - Decay rate:", working_memory.getDecayRate())
        
        # Create and store a pattern
        print("\n   Storing pattern...")
        pattern = [0.1, 0.3, 0.7, 0.9, 0.5]
        working_memory.store(pattern, strength=1.0)
        print("   Pattern stored")
        
        # Store neuron-specific activation
        print("\n   Storing neuron activation...")
        test_neuron_id = nlm.INVALID_NEURON_ID
        if test_neuron_id != 0:
            working_memory.storeToNeuron(test_neuron_id, 0.8)
            print("   Neuron activation stored")
        
        # Retrieve and examine memory
        print("\n   Retrieving memory...")
        retrieved = working_memory.retrieve()
        print(f"   Retrieved {len(retrieved)} activation values")
        if retrieved:
            print(f"   Average activation: {sum(retrieved)/len(retrieved):.3f}")
        
        # Check specific neuron
        if test_neuron_id != 0:
            activation = working_memory.getNeuronActivation(test_neuron_id)
            print(f"   Neuron {test_neuron_id} activation: {activation:.3f}")
        
        # Run competition and check winners
        print("\n   Running competition...")
        working_memory.runCompetition()
        winners = working_memory.getMemoryNeurons()
        print(f"   Number of winners: {len(winners)}")
        
        # Strengthen memory
        print("\n   Strengthening memory...")
        working_memory.strengthenMemory(1.2)
        print("   Memory strengthened by 20%")
        
        # Update memory dynamics
        print("\n   Updating memory dynamics...")
        working_memory.update(0.01)  # 10ms timestep
        print("   Memory updated")
        
        # Get statistics
        print("\n   Working memory statistics:")
        print(f"   - Active traces: {working_memory.getActiveTraces()}")
        print(f"   - Memory activity: {working_memory.getMemoryActivity():.3f}")
        print(f"   - Memory neurons: {len(working_memory.getMemoryNeurons())}")
        
    # ===== NEURAL EPISODIC MEMORY EXAMPLES =====
    print("\n3. Neural Episodic Memory Examples")
    print("-" * 50)
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print("   Episodic memory initialized")
        print("   - Max episodes:", episodic_memory.getEpisodeCount())
        print("   - Replay enabled:", episodic_memory.isReplayEnabled())
        
        # Create and store an episode
        print("\n   Creating episode...")
        episode = nlm.EpisodicMemoryItem()
        episode.sensoryState = [0.5, 0.2, 0.8]
        episode.positionX = 10.0
        episode.positionY = 5.0
        episode.orientation = 0.0
        episode.action = nlm.ActionType.MoveForward
        episode.reward = 1.0
        episode.energy = 0.8
        episode.novelty = 0.3
        episode.resultingSensoryState = [0.6, 0.3, 0.7]
        episode.resultingReward = 1.2
        episode.activeNeurons = [1001, 1002, 1003]
        episode.neuronActivations = [0.7, 0.8, 0.6]
        episode.timestamp = 100
        
        episodic_memory.storeEpisode(episode)
        print("   Episode stored")
        
        # Retrieve similar episodes
        print("\n   Retrieving similar episodes...")
        query_pattern = [0.4, 0.2, 0.7]
        similar_episodes = episodic_memory.retrieveSimilar(query_pattern, maxResults=3)
        print(f"   Found {len(similar_episodes)} similar episodes")
        
        # Retrieve by time window
        print("\n   Retrieving episodes by time window...")
        temporal_episodes = episodic_memory.retrieveTemporal(50, 150, maxResults=5)
        print(f"   Episodes between steps 50-150: {len(temporal_episodes)}")
        
        # Retrieve by location
        print("\n   Retrieving episodes by location...")
        location_episodes = episodic_memory.retrieveByLocation(10.0, 5.0, 2.0, maxResults=3)
        print(f"   Episodes near (10, 5) radius 2.0: {len(location_episodes)}")
        
        # Retrieve after action
        print("\n   Retrieving episodes after action...")
        action_episodes = episodic_memory.retrieveAfterAction(nlm.ActionType.MoveForward, maxResults=3)
        print(f"   Episodes after MoveForward: {len(action_episodes)}")
        
        # Get recent episodes
        print("\n   Getting recent episodes...")
        recent_episodes = episodic_memory.getRecentEpisodes(5)
        print(f"   Recent episodes (last 5): {len(recent_episodes)}")
        
        # Get average reward
        avg_reward = episodic_memory.getAverageReward()
        print(f"   Average reward: {avg_reward:.3f}")
        
        # Get episodes for replay
        print("\n   Getting episodes for replay...")
        replay_episodes = episodic_memory.getEpisodesForReplay(3)
        print(f"   Episodes selected for replay: {len(replay_episodes)}")
        
        # Replay an episode
        print("\n   Replaying episode...")
        if recent_episodes:
            episodic_memory.replayEpisode(recent_episodes[0])
            print("   Episode replayed (neural patterns reactivated)")
        
        # Get statistics
        print("\n   Episodic memory statistics:")
        print(f"   - Total episodes: {episodic_memory.getEpisodeCount()}")
        print(f"   - Average reward: {episodic_memory.getAverageReward():.3f}")
        
    # ===== NEURAL ASSOCIATIVE MEMORY EXAMPLES =====
    print("\n4. Neural Associative Memory Examples")
    print("-" * 50)
    
    associative_memory = brain.getAssociativeMemory()
    if associative_memory:
        print("   Associative memory initialized")
        print("   - Association count:", associative_memory.getAssociationCount())
        
        # Create associations between patterns
        print("\n   Creating associations...")
        pattern_a = [0.1, 0.2, 0.3]
        pattern_b = [0.8, 0.9, 0.7]
        pattern_c = [0.4, 0.5, 0.6]
        
        associative_memory.associate(pattern_a, pattern_b, strength=0.9)
        associative_memory.associate(pattern_b, pattern_c, strength=0.7)
        associative_memory.associate(pattern_a, pattern_c, strength=0.5)
        print("   Associations created")
        
        # Retrieve associated patterns
        print("\n   Retrieving associations...")
        query_pattern = pattern_a
        results = associative_memory.retrieve(query_pattern, maxResults=3)
        print(f"   Patterns associated with query: {len(results)}")
        
        # Get association strength
        strength_ab = associative_memory.getAssociationStrength(pattern_a, pattern_b)
        strength_bc = associative_memory.getAssociationStrength(pattern_b, pattern_c)
        print(f"   Association strength A-B: {strength_ab:.3f}")
        print(f"   Association strength B-C: {strength_bc:.3f}")
        
        # Update association
        print("\n   Updating association...")
        associative_memory.updateAssociation(pattern_a, pattern_b, 0.1)
        new_strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
        print(f"   Updated strength A-B: {new_strength:.3f}")
        
        # Spread activation
        print("\n   Spreading activation...")
        activated_patterns = associative_memory.spreadActivation(pattern_a, steps=2)
        print(f"   Patterns activated through association: {len(activated_patterns)}")
        
        # Get statistics
        print("\n   Associative memory statistics:")
        print(f"   - Total associations: {associative_memory.getAssociationCount()}")
        
        # Create experience-based association
        print("\n   Creating experience-based association...")
        if 'episode' in locals():
            associative_memory.associateFromExperience(episode)
            print("   Association created from episode")
        
    # ===== INTEGRATION DEMONSTRATION =====
    print("\n5. Integration Demonstration")
    print("-" * 50)
    
    print("   Simulating working memory usage...")
    if working_memory:
        working_memory.store([0.2, 0.4, 0.6, 0.8], 0.5)
        print("   Pattern stored in working memory")
    
    print("   Creating new episode...")
    if episodic_memory:
        new_episode = nlm.EpisodicMemoryItem()
        new_episode.sensoryState = [0.9, 0.3, 0.5]
        new_episode.positionX = 15.0
        new_episode.positionY = 8.0
        new_episode.action = nlm.ActionType.Interact
        new_episode.reward = 0.5
        new_episode.activeNeurons = [2001, 2002]
        episodic_memory.storeEpisode(new_episode)
        print("   Episode stored in episodic memory")
    
    print("   Creating association from episode...")
    if associative_memory and 'episode' in locals():
        associative_memory.associateFromExperience(new_episode)
        print("   Association created from sensory patterns")
    
    # ===== FINAL STATISTICS =====
    print("\n6. Final Brain Statistics")
    print("-" * 50)
    brain.logStatus()
    
    print("\n" + "=" * 70)
    print("Example completed successfully!")
    print("=" * 70)

if __name__ == "__main__":
    main()
