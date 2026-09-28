#!/usr/bin/env python3
"""
Example script demonstrating the NLM Python bindings.

This script shows how to use the NLM Python bindings to create and simulate
neural networks with integrated memory, prediction, cognition, and neuromodulation systems.
"""

import sys
import os
import time

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("NLM Python bindings loaded successfully!")
except ImportError as e:
    print(f"Error importing NLM Python bindings: {e}")
    print("Please ensure the NLM Python bindings are properly installed.")
    sys.exit(1)
def main():
    print("=" * 60)
    print("NLM Python Bindings - Example Script")
    print("=" * 60)
    
    print("\n1. Creating a default brain configuration...")
    config = pynlm.create_default_config()
    print(f"   - Neuron count: {config.get('neuron_count')}")
    print(f"   - Region count: {config.get('region_count')}")
    print(f"   - Connection probability: {config.get('connection_probability')}")
    
    print("\n2. Creating and initializing brain...")
    brain = pynlm.Brain(config)
    
    if not brain.initialize():
        print("   ERROR: Failed to initialize brain")
        return 1
    
    print("   ✓ Brain initialized successfully")
    print(f"   - Total neurons: {brain.get_total_neuron_count()}")
    print(f"   - Total synapses: {brain.get_total_synapse_count()}")
    
    print("\n3. Running simulation...")
    num_steps = 50
    
    for step in range(num_steps):
        brain.step(step, step * 0.001)
        
        if step % 10 == 0:
            print(f"   Step {step:2d}: Firing rate = {brain.get_average_firing_rate():.3f}")
    
    print(f"   ✓ Simulation completed ({num_steps} steps)")
    
    print("\n4. Testing memory systems...")
    working_memory = brain.get_working_memory()
    if working_memory:
        print(f"   - Working memory traces: {working_memory.get_active_traces()}")
    else:
        print("   - Working memory: Not available")
    
    episodic_memory = brain.get_episodic_memory()
    if episodic_memory:
        print(f"   - Episodic memory episodes: {episodic_memory.get_episode_count()}")
    else:
        print("   - Episodic memory: Not available")
    
    print("\n5. Testing neuromodulation systems...")
    dopamine = brain.get_dopamine()
    if dopamine:
        print(f"   - Dopamine level: {dopamine.get_level():.3f}")
    else:
        print("   - Dopamine: Not available")
    
    curiosity = brain.get_curiosity()
    if curiosity:
        print(f"   - Curiosity level: {curiosity.get_level():.3f}")
    else:
        print("   - Curiosity: Not available")
    
    novelty = brain.get_novelty()
    if novelty:
        print(f"   - Novelty level: {novelty.get_level():.3f}")
    else:
        print("   - Novelty: Not available")
    
    print("\n6. Testing prediction system...")
    prediction_system = brain.get_prediction_system()
    if prediction_system:
        print(f"   - Prediction system available")
    else:
        print("   - Prediction system: Not available")
    
    print("\n7. Testing action selection...")
    action = brain.produce_action()
    if action:
        print(f"   - Selected action: {action.get_type()}")
    else:
        print("   - Action selection: Not available")
    
    print("\n8. Testing checkpointing...")
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    if brain.save(checkpoint_path):
        print(f"   ✓ Brain state saved to {checkpoint_path}")
        
        # Create new brain and load checkpoint
        brain2 = pynlm.Brain(config)
        brain2.initialize()
        
        if brain2.load(checkpoint_path):
            print(f"   ✓ Brain state loaded successfully")
            print(f"   - Loaded neurons: {brain2.get_total_neuron_count()}")
        else:
            print(f"   ERROR: Failed to load checkpoint")
        
        # Clean up
        os.unlink(checkpoint_path)
    else:
        print("   ERROR: Failed to save checkpoint")
    
    print("\n9. Running integration experiment...")
    experiment = pynlm.Phase6IntegratedExperiment()
    
    # Quick integration test
    if experiment.verifyIntegration():
        print("   ✓ Integration verification passed")
    else:
        print("   ERROR: Integration verification failed")
    
    print("\n10. Running memory integration test...")
    if experiment.testMemoryIntegration():
        print("   ✓ Memory integration test passed")
    else:
        print("   ERROR: Memory integration test failed")
    
    print("\n11. Running neuromodulation integration test...")
    if experiment.testNeuromodulationIntegration():
        print("   ✓ Neuromodulation integration test passed")
    else:
        print("   ERROR: Neuromodulation integration test failed")
    
    print("\n12. Running checkpoint test...")
    if experiment.testCheckpointing():
        print("   ✓ Checkpoint test passed")
    else:
        print("   ERROR: Checkpoint test failed")
    
    print("\n13. Running replay test...")
    if experiment.testReplay():
        print("   ✓ Replay test passed")
    else:
        print("   ERROR: Replay test failed")
    
    print("\n" + "=" * 60)
    print("Example script completed successfully!")
    print("=" * 60)
    
    print("\nNext steps:")
    print("1. Install the C++ extensions with: pip install -e .")
    print("2. Run the Phase 6 integration experiment with:")
    print("   python -c \"import pynlm; result = pynlm.run_integration_experiment(500, 2000); print(result)\"")
    print("3. Explore the full API documentation")
    print("4. Run the NLM examples and tutorials")
    
    return 0
if __name__ == "__main__":
    exit(main())
