#!/usr/bin/env python3
# Memory Brain Example
# Demonstrates memory formation and retrieval in NLM

import sys
sys.path.insert(0, '../')

import pynlm as nlm

def main():
    print("=== NLM Memory Brain Example ===")
    print("This example demonstrates memory formation and retrieval.\n")
    
    # Create brain
    print("1. Creating brain...")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Get memory systems
    print("2. Accessing memory systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory systems available:")
    print(f"   Working memory capacity: {working_memory.getCapacity()}")
    print(f"   Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    
    # Demonstrate working memory
    print("\n3. Demonstrating working memory...")
    print("   Storing information in working memory...")
    
    # Store some information (simulating learning)
    for i in range(5):
        working_memory.storeToNeuron(nlm.NeuronId(i), float(i) / 10.0)
        working_memory.update(0.001)
    
    # Get current memory traces
    memory_neuons = working_memory.getMemoryNeurons()
    print(f"   Current memory traces: {len(memory_neuons)} neurons")
    print(f"   Memory neuron IDs: {[str(nid) for nid in memory_neuons]}")
    
    # Demonstrate episodic memory
    print("\n4. Demonstrating episodic memory...")
    print("   Creating episodic memory (event)...")
    
    # Create an episode (simulating an experience)
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = 100
    episode.reward = 0.8
    
    # Add some active neurons and their activation levels
    for i in range(10):
        episode.activeNeurons.append(nlm.NeuronId(i))
        episode.neuronActivations.append(float(i) / 20.0)
    
    # Store the episode
    episodic_memory.storeEpisode(episode)
    
    print(f"   Episode created:")
    print(f"     Timestamp: {episode.timestamp}")
    print(f"     Reward: {episode.reward}")
    print(f"     Active neurons: {len(episode.activeNeurons)}")
    print(f"     Neuron activations: {[f'{act:.2f}' for act in episode.neuronActivations]}")
    
    # Get all episodes
    all_episodes = episodic_memory.getEpisodes()
    print(f"\n   Total episodes in memory: {len(all_episodes)}")
    
    # Demonstrate episodic memory retrieval
    print("\n5. Demonstrating episodic memory retrieval...")
    episodes_for_replay = episodic_memory.getEpisodesForReplay(2)
    print(f"   Episodes available for replay: {len(episodes_for_replay)}")
    
    for i, episode_ptr in enumerate(episodes_for_replay):
        if episode_ptr:
            print(f"   Episode {i+1}:")
            print(f"     Timestamp: {episode_ptr->timestamp}")
            print(f"     Reward: {episode_ptr->reward}")
            print(f"     Number of active neurons: {len(episode_ptr->activeNeurons)}")
    
    # Demonstrate associative memory
    print("\n6. Demonstrating associative memory...")
    print("   Associative memory would handle pattern completion here...")
    print("   (Note: Full implementation would show how partial patterns")
    print("    trigger complete memory recall)")
    
    # Show memory system statistics
    print("\n7. Memory system statistics...")
    print(f"   Working memory active traces: {working_memory.getActiveTraces()}")
    print(f"   Episodic memory episode count: {episodic_memory.getEpisodeCount()}")
    print(f"   Associative memory pattern count: {associative_memory.getPatternCount()}")
    
    print("\nMemory example completed successfully!")
    
    return brain

if __name__ == "__main__":
    main()
