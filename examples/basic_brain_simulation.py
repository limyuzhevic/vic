"""
Example: Basic Brain Simulation

This example demonstrates the basic usage of the enhanced NLM Python API,
showing how to create a brain, configure it, and run a simple simulation.
"""

from nlm import (
    Brain, Config, create_brain, create_default_config,
    brain_context, NLMError
)

def main():
    """Run the basic brain simulation example."""
    print("=== Basic Brain Simulation Example ===")
    
    # Create a default configuration
    print("1. Creating default configuration...")
    config = create_default_config()
    print(f"   Config: {config}")
    
    # Create a brain
    print("2. Creating brain...")
    brain = create_brain(config)
    print(f"   Brain: {brain}")
    
    # Initialize the brain
    print("3. Initializing brain...")
    if not brain.initialize():
        raise NLMError("Failed to initialize brain")
    print("   Brain initialized successfully")
    
    # Add a region
    print("4. Adding neural region...")
    region_id = brain.add_region("Cortex")
    print(f"   Added region: {region_id}")
    
    # Display brain statistics
    print("5. Brain statistics:")
    print(f"   Neuron count: {brain.neuron_count}")
    print(f"   Synapse count: {brain.synapse_count}")
    print(f"   Region count: {brain.region_count}")
    print(f"   Active neurons: {brain.active_neuron_count}")
    print(f"   Firing neurons: {brain.firing_neuron_count}")
    print(f"   Average firing rate: {brain.average_firing_rate:.2f}")
    print(f"   Developmental stage: {brain.developmental_stage}")
    
    # Run a simulation step
    print("6. Running simulation step...")
    brain.step(0)
    print("   Simulation step completed")
    
    # Update developmental stage
    print("7. Updating developmental stage...")
    from nlm import DevelopmentalStage
    brain.developmental_stage = DevelopmentalStage.Adult
    print(f"   Developmental stage updated to: {brain.developmental_stage}")
    
    # Save brain state
    print("8. Saving brain state...")
    brain.save("basic_brain.nlm")
    print("   Brain state saved to 'basic_brain.nlm'")
    
    # Reset brain
    print("9. Resetting brain...")
    brain.reset()
    print("   Brain reset successfully")
    
    # Load brain state
    print("10. Loading brain state...")
    brain.load("basic_brain.nlm")
    print("   Brain state loaded successfully")
    
    print("\n=== Basic Brain Simulation Example Complete ===")
    print("\nKey points demonstrated:")
    print("- Creating configuration with sensible defaults")
    print("- Creating and initializing a brain")
    print("- Adding neural regions")
    print("- Accessing brain statistics via properties")
    print("- Running simulation steps")
    print("- Managing developmental stages")
    print("- Saving and loading brain states")
    print("- Using enhanced Python API features")

if __name__ == "__main__":
    main()
