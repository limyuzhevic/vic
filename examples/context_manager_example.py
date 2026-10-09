import pynlm
import os

print("\n=== Advanced Context Manager Example ===")

# Create multiple brains with different configurations
configs = [
    ("Fast", pynlm.EnhancedConfig.createForSpeed()),
    ("Balanced", pynlm.EnhancedConfig.createDefault()),
    ("Precise", pynlm.EnhancedConfig.createForAccuracy()),
]

brains = []

try:
    # Create and train multiple brains
    for name, config in configs:
        print(f"\nTraining {name} brain...")
        
        brain = pynlm.createBrain(config)
        brains.append((name, brain))
        
        with brain:
            # Train with different patterns
            if name == "Fast":
                brain.train(20, "Quick test")
            elif name == "Balanced":
                brain.train(50, "Moderate training")
            else:  # Precise
                brain.train(100, "Detailed training")
            
            print(f"  Completed: {brain.getSpikeCount()} spikes")
            
            # Save intermediate state
            brain.save(f"{name.lower()}_brain.nlm")
    
    print("\nAll brains trained successfully!")
    
    # Load and compare brains
    print("\nComparing brain states:")
    for name, brain in brains:
        brain.load(f"{name.lower()}_brain.nlm")
        print(f"  {name}: {brain.getSpikeCount()} spikes, {brain.getFiringRate():.2f} Hz avg")
        
finally:
    # Clean up files
    import os
    for name, _ in brains:
        filename = f"{name.lower()}_brain.nlm"
        if os.path.exists(filename):
            os.remove(filename)
            print(f"Cleaned up {filename}")
