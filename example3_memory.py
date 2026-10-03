import pynlm

print("=== NLM Memory Systems Example ===")
print("Demonstrating working and episodic memory systems...")

# Create brain with memory systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)

brain = pynlm.createBrain(config)
brain.initialize()

# Check initial memory state
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

print(f"Initial working memory capacity: {working_memory.getCapacity()}")
print(f"Initial working memory traces: {working_memory.getActiveTraces()}")
print(f"Initial episodic memory episodes: {episodic_memory.getEpisodeCount()}")

# Run simulation to build memories
print("\nBuilding memories...")
for step in range(150):
    brain.step(step)
    
    # Inject sensory input to build working memory
    if step % 15 == 0:
        # Create simple sensory input (simulating visual input)
        vision_data = [0.3, 0.7, 0.5, 0.9, 0.2, 0.6, 0.8, 0.4] * 8  # 64 elements
        from pynlm import Vision
        sensory_input = Vision(8, 8, 1, vision_data)
        brain.receiveSensoryInput(sensory_input)
        print(f"  Step {step}: Injected sensory input, working memory traces: {working_memory.getActiveTraces()}")
    
    # Store episode in episodic memory every 40 steps
    if step % 40 == 39:
        # Create an episodic memory item
        from pynlm import EpisodicMemoryItem
        episode = EpisodicMemoryItem()
        episode.timestamp = step
        episode.reward = 0.5
        
        # Add active neurons to episode
        for region in brain.getRegions():
            for pop in region.getPopulations():
                for neuron in pop.getNeurons():
                    if neuron.isFiring():
                        episode.activeNeurons.append(neuron.getId())
        
        episodic_memory.storeEpisode(episode)
        print(f"  Step {step}: Stored episodic memory, total episodes: {episodic_memory.getEpisodeCount()}")
    
    # Show memory statistics periodically
    if step % 30 == 29:
        print(f"  Step {step}: Firing rate: {brain.getAverageFiringRate():.3f}, "
              f"Working traces: {working_memory.getActiveTraces()}, "
              f"Episodic episodes: {episodic_memory.getEpisodeCount()}")

print(f"\n=== Memory Systems Final Statistics ===")
print(f"Working memory capacity: {working_memory.getCapacity()}")
print(f"Working memory active traces: {working_memory.getActiveTraces()}")
print(f"Working memory utilization: {working_memory.getActiveTraces()/working_memory.getCapacity()*100:.1f}%")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
print(f"Episodic memory capacity: {episodic_memory.getMaxEpisodes()}")
print(f"Episodic memory utilization: {episodic_memory.getEpisodeCount()/episodic_memory.getMaxEpisodes()*100:.1f}%")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Demonstrate memory replay if available
if episodic_memory.getEpisodeCount() > 0:
    print(f"\n=== Memory Replay Demo ===")
    # Get episodes for replay
    episodes = episodic_memory.getEpisodesForReplay(3)
    print(f"Retrieved {len(episodes)} episodes for replay")
    for i, episode in enumerate(episodes[:2]):
        print(f"  Episode {i+1}: {len(episode.activeNeurons)} active neurons, "
              f"reward: {episode.reward:.2f}, timestamp: {episode.timestamp}")

print("\n=== Memory Example Complete ===")