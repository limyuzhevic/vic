# NLM Enhanced - Example 2: SmartBrain Context Manager

import pynlm

print("=== SmartBrain Context Manager Example ===")

# Create SmartBrain with learning-optimized configuration
with pynlm.createBrain(pynlm.EnhancedConfig.createForLearning()) as brain:
    print(f"Learning brain created with {brain.getNeuronCount()} neurons")
    
    # Run multiple episodes
    for episode in range(3):
        print(f"\n--- Episode {episode + 1} ---")
        
        # Start new episode
        brain.runEpisode(f"Training episode {episode + 1}")
        
        # Train for 50 steps
        brain.train(50, f"Learning episode {episode + 1}")
        
        # Save progress
        brain.save(f"brain_episode_{episode + 1}.nlm")
        
        print(f"Spikes this episode: {brain.getSpikeCount() - (episode * 50)}")
