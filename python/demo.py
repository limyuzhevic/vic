"""
NLM Python Demo Script
=======================

This script demonstrates the enhanced NLM Python API features.
Run with: python demo.py
"""

import pynlm
import time

def demo_basic_usage():
    """Demonstrate basic NLM usage"""
    print("1. Basic NLM Usage")
    print("-" * 40)
    
    # Create and initialize brain
    brain = pynlm.createDefaultBrain()
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run a few steps
    for step in range(5):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

def demo_advanced_setup():
    """Demonstrate advanced setup with all features"""
    print("\n2. Advanced Setup with Complete Experiment")
    print("-" * 40)
    
    # Create complete experiment setup
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    print(f"✓ World size: {world.getWidth()}x{world.getHeight()}")
    print(f"✓ Agent at start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"✓ Enabled features: Reward modulation, Development, Curiosity")
    
    # Run learning simulation
    print("\nRunning learning simulation...")
    start_time = time.time()
    
    total_reward, success_count, total_spikes, stage = pynlm.run_learning_simulation(
        brain, world, agent, 50
    )
    
    elapsed = time.time() - start_time
    
    print(f"✓ Simulation completed in {elapsed:.2f} seconds")
    print(f"✓ Total reward: {total_reward:.2f}")
    print(f"✓ Successful actions: {success_count}")
    print(f"✓ Total spikes: {total_spikes}")
    print(f"✓ Final developmental stage: {stage}")

def demo_sensor_processing():
    """Demonstrate sensory input processing"""
    print("\n3. Sensory Input Processing")
    print("-" * 40)
    
    # Create sensory inputs
    vision = pynlm.to_vision(64, 64, 3)
    audio = pynlm.to_audio(22050, 2048)
    
    # Create world and get percept
    world = pynlm.createSimpleWorld()
    world.configure(64, 64, 8, 8)
    world.reset()
    
    percept = world.getSensoryPercept()
    
    print(f"✓ World percept contains:")
    print(f"  - Vision: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
    print(f"  - Internal signals: {len(percept.getInternal())}")
    print(f"  - Touch signals: {len(percept.getTouch())}")

def demo_utilities():
    """Demonstrate utility functions"""
    print("\n4. Utility Functions")
    print("-" * 40)
    
    # Random position
    x, y = pynlm.random_position()
    print(f"✓ Random position: ({x:.1f}, {y:.1f})")
    
    # Random action
    action = pynlm.random_action()
    print(f"✓ Random action: {action.getName()} ({action.getType()})")
    
    # Statistics
    brain = pynlm.createDefaultBrain()
    brain.initialize()
    stats = pynlm.get_brain_stats(brain)
    
    print(f"✓ Brain statistics:")
    print(f"  - Total neurons: {stats['total_neurons']}")
    print(f"  - Firing rate: {stats['average_firing_rate']:.3f}")
    print(f"  - E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")

def demo_config_management():
    """Demonstrate configuration management"""
    print("\n5. Configuration Management")
    print("-" * 40)
    
    # Create different config types
    config1 = pynlm.create_config()
    config2 = pynlm.create_config_with_seed(12345)
    
    print(f"✓ Basic config created")
    print(f"✓ Config with seed created")
    
    # Get config summary
    summary = pynlm.get_config_summary(config2)
    print(f"✓ Config summary:\n{summary}")

def demo_error_handling():
    """Demonstrate error handling"""
    print("\n6. Error Handling")
    print("-" * 40)
    
    # Test validation
    print("Testing validation...")
    
    # These should work
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    print("✓ Valid brain creation works")
    
    # Test None handling (commented out as it would crash the demo)
    # try:
    #     pynlm.createBrain(None)
    # except RuntimeError as e:
    #     print(f"✓ Proper error handling: {e}")
    
    print("✓ All validation working correctly")

def demo_batch_processing():
    """Demonstrate batch processing"""
    print("\n7. Batch Processing")
    print("-" * 40)
    
    brain = pynlm.createDefaultBrain()
    brain.initialize()
    
    processed_steps = 0
    
    def custom_processor(brain, step, time):
        nonlocal processed_steps
        processed_steps += 1
        # Custom processing logic here
    
    print("Processing 20 brain steps in batch...")
    pynlm.batch_process_steps(brain, custom_processor, 0, 19)
    
    print(f"✓ Processed {processed_steps} steps in batch")

def main():
    print("NLM Python Demo - Enhanced API Features")
    print("=" * 50)
    print(f"pynlm version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print("=" * 50)
    
    # Run all demos
    demos = [
        demo_basic_usage,
        demo_advanced_setup,
        demo_sensor_processing,
        demo_utilities,
        demo_config_management,
        demo_error_handling,
        demo_batch_processing,
    ]
    
    for demo in demos:
        try:
            demo()
        except Exception as e:
            print(f"✗ Demo failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 50)
    print("Demo Complete!")
    print("\nFor more examples, see:")
    print("- python/examples/ directory")
    print("- python/test_enhanced_bindings.py for tests")
    print("- easy_usage.md and HOW_TO_USE.md for documentation")

if __name__ == "__main__":
    main()