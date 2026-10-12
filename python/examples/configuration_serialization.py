from pynlm import *
import json

print("NLM Python Bindings Example: Configuration and Serialization")
print("=" * 60)

# Create and configure a brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)
config.set("brain.synapse_density", 0.08)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.enabled", True)
config.set("brain.name", "research_simulation")

print("Created and configured brain")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Initialized brain with {brain.getTotalNeuronCount()} neurons")

# Save brain state to file
import tempfile
import os

# Create temporary files
with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as config_file:
    config_filename = config_file.name

with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as brain_file:
    brain_filename = brain_file.name

try:
    # Save configuration to file
    config.saveToFile(config_filename)
    print(f"Saved configuration to {config_filename}")
    
    # Load and verify configuration
    config2 = pynlm.Config()
    config2.loadFromFile(config_filename)
    
    # Compare key settings
    assert config2.has("brain.neuron_count")
    assert config2.has("brain.synapse_density")
    assert config2.has("plasticity.enabled")
    print("Configuration save/load test passed")
    
    # Run some simulation
    for i in range(20):
        brain.step(i)
    
    # Save brain state
    brain.save(brain_filename)
    print(f"Saved brain state to {brain_filename}")
    
    # Load brain state into new brain
    new_config = pynlm.Config()
    new_config.loadFromFile(config_filename)
    new_brain = pynlm.createBrain(new_config)
    new_brain.initialize()
    new_brain.load(brain_filename)
    
    # Verify state match
    assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
    assert new_brain.getFiringNeuronCount() == brain.getFiringNeuronCount()
    print("Brain state save/load test passed")
    
    # Display configuration details
    print(f"\nConfiguration Details:")
    print(f"  Brain neuron count: {config.get('brain.neuron_count')}")
    print(f"  Brain synapse density: {config.get('brain.synapse_density')}")
    print(f"  Plasticity enabled: {config.get('plasticity.enabled')}")
    
    # Display brain statistics
    print(f"\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Brain size: {brain.getBrainSize()}")
    
    print("\nConfiguration and serialization example completed successfully!")
    
finally:
    # Cleanup temporary files
    if os.path.exists(config_filename):
        os.unlink(config_filename)
    if os.path.exists(brain_filename):
        os.unlink(brain_filename)
    print("Cleaned up temporary files")