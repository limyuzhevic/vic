def main():
    """Console script entry point for NLM."""
    import pynlm
    import sys
    
    print("NLM - Neural Learning Machine Python Bindings")
    print("Version:", pynlm.__version__)
    print("\nAvailable commands:")
    print("  --demo        Run a demonstration simulation")
    print("  --brain [n]  Create a brain with n neurons")
    print("  --help        Show this help message")
    
    # Parse simple command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] == "--demo":
            run_demo()
        elif sys.argv[1] == "--brain":
            neuron_count = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
            create_brain_demo(neuron_count)
        elif sys.argv[1] == "--help":
            print("\n" + __doc__)
        else:
            print(f"Unknown command: {sys.argv[1]}")
            sys.exit(1)
    else:
        # Default: show help
        print("\nRun 'nlm --help' for more information")
    
def run_demo():
    """Run a demonstration simulation."""
    import pynlm
    
    print("\nRunning NLM demonstration...")
    
    # Create a simple brain
    config = pynlm.createConfigFromTemplate("explorer")
    brain = pynlm.createValidatedBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    # Create agent
    agent = pynlm.createAgentSystem(config, world)
    
    # Run simulation
    print("Running simulation with", brain.getTotalNeuronCount(), "neurons")
    actions = pynlm.runSimulation(brain, world, 50)
    
    print(f"Simulation completed with {len(actions)} actions")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")

def create_brain_demo(neuron_count=1000):
    """Create a brain with specified neuron count."""
    import pynlm
    
    print(f"\nCreating brain with {neuron_count} neurons...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", neuron_count)
    config.set("brain.simulationSteps", 100)
    
    brain = pynlm.createValidatedBrain(config)
    brain.initialize()
    
    print(f"Brain created successfully!")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    
    # Step through simulation
    for step in range(10):
        brain.step(step)
    
    print(f"  Total spikes after 10 steps: {brain.getTotalSpikeCount()}")

if __name__ == "__main__":
    main()