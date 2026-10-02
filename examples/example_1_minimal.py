import pynlm

def main():
    """Minimal Brain Example - Basic usage and setup."""
    print("NLM Minimal Brain Example")
    print("=" * 50)
    
    # Create and initialize a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation for 100 steps
    for i in range(100):
        brain.step(i)
    
    print(f"Simulation complete!")
    print(f"Brain produced {brain.getTotalSpikeCount()} spikes")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")

if __name__ == "__main__":
    main()