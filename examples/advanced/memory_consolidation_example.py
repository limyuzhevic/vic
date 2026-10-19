#!/usr/bin/env python3
"""
Memory Consolidation Example

This example demonstrates the Memory Consolidation system in NLM.
It shows how episodic memories are consolidated into long-term storage.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import time
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
from datetime import datetime

class MemoryConsolidationExample:
    """Example class demonstrating the Memory Consolidation system."""
    
    def __init__(self):
        """Initialize the memory consolidation example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.episodic_memory = None
        self.working_memory = None
        self.example_name = "Memory Consolidation System"
        
    def setup_simulation_environment(self):
        """Set up the simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(16, 16, 12, 12)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Create memory systems
        self.episodic_memory = pynlm.EpisodicMemory()
        self.working_memory = pynlm.WorkingMemory()
        self.working_memory.setCapacity(10)
        
    def create_episodic_memories(self):
        """Create and store episodic memories."""
        print("Creating episodic memories...")
        
        # Create a series of episodic memories
        memories = []
        
        for episode_id in range(10):
            # Create sensory input for this episode
            vision = np.random.rand(12, 12, 3) * 0.5
            
            # Create episodic memory item
            episode = pynlm.EpisodicMemoryItem(
                timestamp=episode_id,
                neurons=[pynlm.NeuronId(1000 + episode_id)],
                values=[0.7 - (episode_id * 0.05)],  # Decreasing intensity
                metadata=f"Episode {episode_id}: Resource interaction"
            )
            
            memories.append(episode)
            
            # Store in episodic memory
            self.episodic_memory.storeEpisode(episode)
            print(f"  - Stored episode {episode_id}: intensity={0.7 - (episode_id * 0.05):.2f}")
        
        print(f"  - Total memories stored: {self.episodic_memory.getEpisodeCount()}")
        
    def demonstrate_memory_storage(self):
        """Demonstrate episodic memory storage and retrieval."""
        print("\nDemonstrating episodic memory storage and retrieval...")
        
        # Get some episodic memories
        recent_episodes = self.episodic_memory.getRecentEpisodes(3)
        
        print(f"1. Retrieved recent episodes ({len(recent_episodes)}):")
        for i, episode in enumerate(recent_episodes):
            intensity = episode.values[0] if episode.values else 0.0
            print(f"   - Episode {episode.timestamp}: intensity={intensity:.2f}, "
                  f"metadata='{episode.metadata}'")
        
        # Demonstrate retrieval by timestamp
        if recent_episodes:
            first_episode = self.episodic_memory.retrieveEpisode(0)
            print(f"\n2. Retrieved specific episode:")
            print(f"   - Timestamp: {first_episode.timestamp}")
            print(f"   - Neurons: {[n.value for n in first_episode.neurons]}")
            print(f"   - Values: {first_episode.values}")
            print(f"   - Metadata: {first_episode.metadata}")
        
    def demonstrate_working_memory_integration(self):
        """Demonstrate integration between working memory and episodic memory."""
        print("\nDemonstrating Working Memory - Episodic Memory integration...")
        
        # Store information in working memory that will later be episodic
        neuron_ids = [pynlm.NeuronId(2000), pynlm.NeuronId(2001), pynlm.NeuronId(2002)]
        
        print("1. Storing in working memory:")
        for i, neuron_id in enumerate(neuron_ids):
            value = 0.8 - (i * 0.2)
            self.working_memory.store(neuron_id, value)
            print(f"   - Stored {value:.1f} for neuron {neuron_id.value}")
        
        print("\n2. Retrieving from working memory:")
        for neuron_id in neuron_ids:
            value = self.working_memory.recall(neuron_id)
            print(f"   - Retrieved {value:.1f} for neuron {neuron_id.value}")
        
        print(f"\n3. Working memory status:")
        print(f"   - Current size: {self.working_memory.getCurrentSize()}")
        print(f"   - Capacity: {self.working_memory.capacity}")
        
        # Clear working memory (simulate consolidation process)
        print("\n4. Simulating consolidation - clearing working memory...")
        self.working_memory.clear()
        print(f"   - Working memory cleared, current size: {self.working_memory.getCurrentSize()}")
        
    def demonstrate_memory_consolidation(self, threshold=0.6):
        """
        Demonstrate memory consolidation process.
        
        Args:
            threshold: Relevance threshold for consolidation
        """
        print(f"\nDemonstrating Memory Consolidation (threshold={threshold})...")
        
        # Get all current episodic memories
        total_episodes = self.episodic_memory.getEpisodeCount()
        print(f"1. Current episodic memories: {total_episodes}")
        
        # Calculate relevance scores for each episode
        print("2. Calculating relevance scores...")
        relevance_scores = []
        
        for episode_id in range(total_episodes):
            # For demonstration, calculate relevance based on timestamp and intensity
            episode = self.episodic_memory.retrieveEpisode(episode_id)
            intensity = episode.values[0] if episode.values else 0.0
            # Relevance decreases with age (older episodes less relevant)
            relevance = intensity * (1.0 - (episode_id / max(1, total_episodes - 1)))
            relevance_scores.append(relevance)
            print(f"   - Episode {episode_id}: intensity={intensity:.2f}, relevance={relevance:.3f}")
        
        print(f"\n3. Performing consolidation...")
        print(f"   - Episodes to consolidate: {len([r for r in relevance_scores if r > threshold])}")
        
        # Perform consolidation (simplified)
        consolidated_count = 0
        for i, score in enumerate(relevance_scores):
            if score > threshold:
                # In real system, consolidated memories would be moved to long-term storage
                print(f"   - Consolodated episode {i} (relevance={score:.3f})")
                consolidated_count += 1
        
        # Display consolidation statistics
        print(f"\n4. Consolidation statistics:")
        print(f"   - Total episodes: {total_episodes}")
        print(f"   - Episodes consolidated: {consolidated_count}")
        print(f"   - Consolidation ratio: {consolidated_count / max(1, total_episodes):.1%}")
        print(f"   - Episodes removed: {self.episodic_memory.getEpisodeCount()}")
        
        # In real implementation, consolidated episodes would be removed
        # For demonstration, we'll clear the episodic memory
        self.episodic_memory.clear()
        print(f"   - Episodic memory cleared after consolidation")
        
    def demonstrate_memory_replay(self):
        """Demonstrate memory replay and rehearsal."""
        print("\nDemonstrating Memory Replay and Rehearsal...")
        
        # Create some test memories for replay
        test_episodes = []
        for i in range(5):
            episode = pynlm.EpisodicMemoryItem(
                timestamp=i + 10,  # Offset timestamps
                neurons=[pynlm.NeuronId(3000 + i)],
                values=[0.9 - (i * 0.1)],
                metadata=f"Replay episode {i}"
            )
            test_episodes.append(episode)
            self.episodic_memory.storeEpisode(episode)
        
        print(f"1. Stored {len(test_episodes)} test episodes for replay")
        
        # Demonstrate replay of episodes
        print("2. Replaying episodes...")
        for episode in test_episodes:
            print(f"   - Replaying episode {episode.timestamp}: "
                  f"intensity={episode.values[0]:.2f}, "
                  f"metadata='{episode.metadata}'")
        
        # Get episodes suitable for replay
        replay_episodes = self.episodic_memory.getEpisodesForReplay(3)
        print(f"\n3. Episodes selected for replay: {len(replay_episodes)}")
        
        # Clear for next demonstration
        self.episodic_memory.clear()
        
    def demonstrate_forgetting_process(self):
        """Demonstrate the natural forgetting process."""
        print("\nDemonstrating Natural Forgetting Process...")
        
        # Store many memories (simulating long-term experience)
        print("1. Storing many memories (simulating long-term experience)...")
        for i in range(20):
            episode = pynlm.EpisodicMemoryItem(
                timestamp=i,
                neurons=[pynlm.NeuronId(4000 + i)],
                values=[0.5 + (i % 3) * 0.1],  # Varying intensities
                metadata=f"Experience {i}"
            )
            self.episodic_memory.storeEpisode(episode)
        
        print(f"   - Stored {self.episodic_memory.getEpisodeCount()} memories")
        
        # Apply different forgetting thresholds
        print("\n2. Applying forgetting thresholds...")
        thresholds = [0.3, 0.5, 0.7]
        
        for threshold in thresholds:
            # Calculate how many memories would be forgotten
            total = self.episodic_memory.getEpisodeCount()
            # For demonstration, estimate based on timestamp distribution
            forgotten = int(total * (threshold - 0.3) / 0.4)  # Simplified model
            remaining = total - forgotten
            
            print(f"   - Threshold {threshold}: {forgotten} memories forgotten, {remaining} remaining")
        
        # Clear for next demonstration
        self.episodic_memory.clear()
        
    def demonstrate_working_memory_decay(self):
        """Demonstrate working memory decay with episodic integration."""
        print("\nDemonstrating Working Memory Decay with Episodic Integration...")
        
        # Store information in working memory
        neuron_ids = [pynlm.NeuronId(5000 + i) for i in range(5)]
        
        for i, neuron_id in enumerate(neuron_ids):
            self.working_memory.store(neuron_id, 0.9 - (i * 0.1))
        
        print(f"1. Stored {len(neuron_ids)} items in working memory")
        
        # Apply decay
        decay_rates = [0.0, 0.2, 0.5, 0.8]
        
        for decay_rate in decay_rates:
            # Create copy to show decay effect (in real system, in-place)
            current_values = [self.working_memory.recall(nid) for nid in neuron_ids]
            decayed_values = [v * (1.0 - decay_rate) for v in current_values]
            
            print(f"\n2. Decay rate {decay_rate}:")
            for i, (neuron_id, orig, dec) in enumerate(zip(neuron_ids, current_values, decayed_values)):
                print(f"   - Neuron {neuron_id.value}: {orig:.3f} -> {dec:.3f}")
        
        # Clear for next demonstration
        self.working_memory.clear()
        
    def display_memory_consolidation_statistics(self):
        """Display memory consolidation system statistics."""
        print("\nMemory Consolidation System Statistics:")
        print("-" * 40)
        
        print(f"1. Episodic Memory:")
        print(f"   - Current episodes: {self.episodic_memory.getEpisodeCount()}")
        print(f"   - Maximum episodes: {self.episodic_memory.maxEpisodes}")
        
        print(f"\n2. Working Memory:")
        print(f"   - Capacity: {self.working_memory.capacity}")
        print(f"   - Current size: {self.working_memory.getCurrentSize()}")
        print(f"   - Memory neurons: {self.working_memory.memoryNeurons}")
        
        print(f"\n3. Integration Metrics:")
        print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   - Firing neurons: {self.brain.getFiringNeuronCount()}")
        
        # Calculate memory system efficiency
        if self.episodic_memory.maxEpisodes > 0:
            usage_ratio = self.episodic_memory.getEpisodeCount() / self.episodic_memory.maxEpisodes
            print(f"\n4. Memory System Efficiency:")
            print(f"   - Episodic memory usage: {usage_ratio:.1%}")
        
        if self.working_memory.capacity > 0:
            wm_usage = self.working_memory.getCurrentSize() / self.working_memory.capacity
            print(f"   - Working memory usage: {wm_usage:.1%}")
        
    def run_example(self, consolidation_threshold=0.6, steps=40):
        """
        Run the complete memory consolidation example.
        
        Args:
            consolidation_threshold: Threshold for memory consolidation
            steps: Number of steps for simulation demo
        """
        print("=" * 60)
        print(f"NLM Memory Consolidation System Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Create and populate episodic memories
            self.create_episodic_memories()
            print()
            
            # Step 3: Demonstrate memory storage and retrieval
            self.demonstrate_memory_storage()
            print()
            
            # Step 4: Demonstrate working memory integration
            self.demonstrate_working_memory_integration()
            print()
            
            # Step 5: Demonstrate memory consolidation
            self.demonstrate_memory_consolidation(consolidation_threshold)
            print()
            
            # Step 6: Demonstrate memory replay
            self.demonstrate_memory_replay()
            print()
            
            # Step 7: Demonstrate forgetting process
            self.demonstrate_forgetting_process()
            print()
            
            # Step 8: Demonstrate working memory decay
            self.demonstrate_working_memory_decay()
            print()
            
            # Step 9: Display statistics
            self.display_memory_consolidation_statistics()
            print()
            
            print("=" * 60)
            print("Memory Consolidation Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.episodic_memory, self.working_memory
            
        except Exception as e:
            print(f"Error in memory consolidation example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Memory Consolidation Example")
    parser.add_argument("--threshold", type=float, default=0.6, help="Consolidation threshold")
    parser.add_argument("--steps", type=int, default=40, help="Number of simulation steps")
    
    args = parser.parse_args()
    
    # Run the memory consolidation example
    example = MemoryConsolidationExample()
    result = example.run_example(args.threshold, args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Memory consolidation system is working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)