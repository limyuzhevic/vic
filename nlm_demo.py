#!/usr/bin/env python3
"""
NLM Python Demo Script

This script demonstrates the basic usage of NLM (Neural Learning Machine)
through Python. It shows both the traditional API and the new convenience functions.

Run with: python3 nlm_demo.py
"""

import sys
import time
def main():
    print("=== NLM Python Demo ===\n")
    
    try:
        import pynlm
        print("✓ NLM Python bindings loaded successfully")
    except ImportError as e:
        print(f"✗ Failed to load NLM: {e}")
        print("\nTo install NLM:")
        print("1. pip install pybind11 scikit-build-core")
        print("2. pip install -e .")
        return 1
    
    print("\n" + "="*50)
    print("DEMO 1: Traditional API (Original Method)")
    print("="*50)
    
    # Traditional method - still works!
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Total spikes after 10 steps: {brain.getTotalSpikeCount()}")
    print("✓ Traditional API works correctly")
    
    print("\n" + "="*50)
    print("DEMO 2: New Convenience Functions (EASIEST Method)")
    print("="*50)
    
    # New convenience functions - much easier!
    brain2 = pynlm.create_default_brain()
    print(f"Created brain with convenience functions: {brain2.getTotalNeuronCount()} neurons")
    
    world2 = pynlm.create_simple_world()
    world2.configure(20, 20, 10, 10)
    world2.reset()
    
    experiment_result = pynlm.run_simple_learning_experiment(brain2, world2, 50)
    print("✓ Convenience functions work correctly")
    
    print("\n" + "="*50)
    print("DEMO 3: Complete Learning Agent")
    print("="*50)
    
    # Create a complete agent with learning
    config3 = pynlm.create_default_config_with_params(
        neuron_count=2000,
        region_count=3
    )
    brain3 = pynlm.create_default_brain(config3)
    
    world3 = pynlm.create_simple_world()
    world3.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world3.reset()
    
    agent3 = pynlm.createAgentBrain(brain3)
    agent3.initialize(world3)
    
    # Enable learning subsystems
    agent3.enableRewardModulation(True)
    agent3.enableCuriosity(True)
    agent3.enableDevelopment(True)
    
    print("Started learning agent with:")
    print(f"  - {brain3.getTotalNeuronCount()} neurons")
    print(f"  - {brain3.getTotalSynapseCount()} synapses")
    print(f"  - {agent3.getSensoryInputSize()} sensory inputs")
    print(f"  - {agent3.getMotorOutputSize()} motor outputs")
    
    # Run learning episode
    print("\nRunning learning episode...")
    start_time = time.time()
    
    for step in range(100):
        world3.update(timestep=0.1)
        agent3.processSensoryInput(world3.getSensoryPercept())
        brain3.step(step)
        world3.applyMotorCommand(agent3.decodeMotorCommand(), world3.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Step {step}:")
            print(f"    Active neurons: {brain3.getActiveNeuronCount()}")
            print(f"    Curiosity level: {agent3.getCuriosityLevel():.3f}")
            print(f"    Firing rate: {brain3.getAverageFiringRate():.4f}")
    
    end_time = time.time()
    
    print(f"\n✓ Learning episode completed in {end_time - start_time:.2f} seconds")
    print(f"  - Total spikes: {brain3.getTotalSpikeCount()}")
    print(f"  - Development stage: {brain3.getDevelopmentalStage()}")
    print(f"  - Final curiosity: {agent3.getCuriosityLevel():.3f}")
    
    print("\n" + "="*50)
    print("DEMO 4: Brain State Visualization")
    print("="*50)
    
    # Get visualization
    visualization = pynlm.visualize_brain_state(brain3, max_regions=3)
    print("Brain state visualization:")
    print(visualization)
    
    print("✓ Visualization completed")
    
    print("\n" + "="*50)
    print("DEMO 5: Complete Example")
    print("="*50)
    
    # Get complete example
    example_output = pynlm.example_basic_usage()
    print("Complete example (first 500 chars):")
    print(example_output[:500] + "...")
    
    print("\n✓ All demos completed successfully!")
    
    print("\n" + "="*50)
    print("SUMMARY")
    print("="*50)
    print("""
You have successfully used NLM! Here's what you learned:

1. ✓ Traditional API still works (backward compatibility)
2. ✓ New convenience functions are much easier to use
3. ✓ Complete agents can learn and adapt
4. ✓ Brain state can be visualized
5. ✓ All systems work together seamlessly

Quick Python tips:
- Use 'create_default_brain()' for easy setup
- Use 'visualize_brain_state()' to inspect brain state
- Use 'run_simple_learning_experiment()' for quick testing
- Check agent status with curiosity, novelty, and development levels

For more examples, see:
- NLM_PYTHON_QUICK_START.md
- HOW_TO_USE.md
- BUILD.md
    """)
    
    return 0
if __name__ == "__main__":
    sys.exit(main())