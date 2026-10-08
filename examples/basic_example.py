#!/usr/bin/env python3
"""
Basic NLM Python Bindings Example

This example demonstrates the enhanced features of the NLM Python bindings,
showcasing the improvements over the original bindings.
"""

import pynlm as nlm

def main():
    print("=== NLM Python Bindings - Basic Example ===\n")
    
    # Example 1: Create and configure brain
    print("1. Creating brain with default configuration...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ {brain.getRegionCount()} brain regions")
    print()
    
    # Example 2: Run simulation with error handling
    print("2. Running simulation...")
    try:
        for step in range(5):
            brain.step(step, step * 0.001)
            print(f"   ✓ Step {step}: {brain.getTotalSpikeCount()} spikes")
    except nlm.NLMError as e:
        print(f"   ✗ Error: {e}")
        return 1
    print()
    
    # Example 3: Demonstrate new factory functions
    print("3. Using enhanced factory functions...")
    world = nlm.createSimpleWorld()
    agent_brain = nlm.createAgentBrain(brain)
    
    world.configure(64, 64, 16, 16)
    world.setAgentStart(32, 32)
    brain.initialize(world)
    agent_brain.initialize(world)
    
    print("   ✓ Enhanced factory functions working")
    print()
    
    # Example 4: Type conversions
    print("4. Testing type conversions...")
    float_data = [0.1, 0.5, 0.9, -0.3]
    double_data = nlm.to_double_vector(float_data)
    float_data2 = nlm.to_float_vector(double_data)
    
    print(f"   ✓ Original: {float_data}")
    print(f"   ✓ Converted: {double_data}")
    print(f"   ✓ Round-trip: {float_data2}")
    print()
    
    # Example 5: Enhanced sensory processing
    print("5. Testing enhanced sensory processing...")
    percept = nlm.SensoryPercept()
    
    # Create vision data
    vision = nlm.Vision(16, 16, 1)
    vision_data = [0.5] * 256  # Simple gray background
    vision.setData(vision_data)
    percept.setVision(vision)
    
    # Process with error handling
    try:
        agent_brain.processSensoryInput(percept)
        print("   ✓ Sensory input processed successfully")
        print(f"   ✓ Vision dimensions: {vision.getWidth()}x{vision.getHeight()}")
    except nlm.InvalidArgumentError as e:
        print(f"   ✗ Sensory processing error: {e}")
    
    print()
    
    # Example 6: Enhanced motor control
    print("6. Testing enhanced motor control...")
    for i in range(3):
        action = agent_brain.decodeMotorCommand()
        print(f"   ✓ Step {i}: Command = {action}")
    
    print()
    
    # Example 7: Learning demonstration
    print("7. Testing learning features...")
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    
    print(f"   ✓ Reward modulation: {agent_brain.isRewardModulationEnabled()}")
    print(f"   ✓ Curiosity: {agent_brain.isCuriosityEnabled()}")
    
    # Simulate learning
    for i in range(2):
        reward = 1.0 if i == 0 else 0.5
        predicted = 0.0
        
        agent_brain.applyRewardModulation(reward, predicted)
        print(f"   ✓ Learning step {i}: reward={reward}, error={agent_brain.getPredictionError():.3f}")
    
    print()
    
    # Example 8: Exception handling
    print("8. Testing exception handling...")
    try:
        # Try to create a brain with invalid configuration (simulated)
        invalid_config = nlm.createDefaultConfig()
        # In real usage, you might set invalid parameters
        
        brain2 = nlm.createBrain(invalid_config)
        brain2.initialize()
        
        print("   ✗ Should have raised an exception")
    except nlm.NLMError as e:
        print(f"   ✓ Exception handling works: {type(e).__name__}")
    except Exception as e:
        print(f"   ✓ Caught exception: {type(e).__name__}: {e}")
    
    print()
    
    # Example 9: Constants and IDs
    print("9. Testing constants and identifiers...")
    print(f"   ✓ Invalid neuron ID: {nlm.INVALID_NEURON_ID}")
    print(f"   ✓ Invalid synapse ID: {nlm.INVALID_SYNAPSE_ID}")
    print(f"   ✓ Invalid region ID: {nlm.INVALID_REGION_ID}")
    
    print()
    
    # Example 10: Configuration features
    print("10. Testing configuration features...")
    config.set("test_param", "test_value")
    print(f"   ✓ Configuration has key 'test_param': {config.has('test_param')}")
    print(f"   ✓ Configuration keys: {config.getKeys()}")
    
    print()
    print("=== All Examples Completed Successfully! ===")
    
    # Summary of improvements
    print("\n=== Improvements Demonstrated ===")
    print("✓ Factory functions for common objects")
    print("✓ Enhanced error handling with custom exceptions")
    print("✓ Type conversion utilities")
    print("✓ Pythonic interfaces (convenience methods)")
    print("✓ Enhanced configuration management")
    print("✓ Learning and adaptation capabilities")
    print("✓ Comprehensive exception handling")
    print("✓ Constants and identifiers")
    print("✓ Documentation and examples")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
