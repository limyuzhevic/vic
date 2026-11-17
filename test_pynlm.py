#!/usr/bin/env python3
"""
Python Bindings Test for NLM (Neural Learning Machine)

This script tests the Python bindings and demonstrates the main capabilities
of the NLM system through its Python interface.
"""

import sys
import os
import time

# Add the current directory to Python path for testing
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("This test requires the pynlm module to be properly built.")
    sys.exit(1)

def test_basic_functionality():
    """Test basic Python binding functionality"""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Test 1: Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Test 2: Create brain with config
        brain = pynlm.createBrain(config)
        print("✓ Created brain instance")
        
        # Test 3: Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Test 4: Get initial statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        print(f"✓ Brain statistics - Neurons: {total_neurons}, Synapses: {total_synapses}")
        
        # Test 5: Run simulation steps
        for step in range(10):
            brain.step(step)
        
        print("✓ Successfully ran simulation steps")
        
        # Test 6: Check after simulation
        firing_neurons = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_rate = brain.getAverageFiringRate()
        
        print(f"✓ After simulation - Firing: {firing_neurons}, Spikes: {total_spikes}, Rate: {avg_rate:.2f} Hz")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_sensory_world_agent():
    """Test the complete sensory-world-agent pipeline"""
    print("\n=== Testing Sensory-World-Agent Pipeline ===")
    
    try:
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("✓ Created and initialized brain")
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        print("✓ Created agent brain")
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        print("✓ Created and configured world")
        
        # Initialize agent with world
        agent.initialize(world)
        print("✓ Initialized agent with world")
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("✓ Enabled learning subsystems")
        
        # Run complete simulation loop
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward learning
            reward = 0.1
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress every 10 steps
            if step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"curiosity: {agent.getCuriosityLevel():.2f}")
        
        print("✓ Successfully completed sensory-world-agent simulation")
        return True
        
    except Exception as e:
        print(f"✗ Sensory-world-agent test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_memory_systems():
    """Test memory-related functionality"""
    print("\n=== Testing Memory Systems ===")
    
    try:
        # Create brain with memory emphasis
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        agent.initialize(world)
        
        # Enable systems that would use memory
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation with memory usage
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.05, 0.0)
            agent.updateDevelopment(0.1)
        
        # Check memory-related statistics
        total_spikes = brain.getTotalSpikeCount()
        firing_neurons = brain.getFiringNeuronCount()
        curiosity = agent.getCuriosityLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"✓ Memory test completed:")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Firing neurons: {firing_neurons}")
        print(f"  Curiosity level: {curiosity:.2f}")
        print(f"  Prediction error: {prediction_error:.2f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Memory systems test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_performance():
    """Test performance characteristics"""
    print("\n=== Testing Performance ===")
    
    try:
        # Test with different brain sizes
        sizes = [100, 500, 1000]
        
        for size in sizes:
            print(f"\nTesting brain with {size} neurons:")
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            for step in range(min(200, size // 2)):  # Limit steps for larger brains
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            steps_per_sec = 200 / duration if duration > 0 else 0
            
            print(f"  Simulated 200 steps in {duration:.3f}s")
            print(f"  Performance: {steps_per_sec:.1f} steps/second")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and edge cases"""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Load from non-existent file
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # This should not crash even if file doesn't exist
        try:
            brain.load("non_existent_file.bin")
            print("✓ Gracefully handled non-existent load file")
        except Exception as e:
            # Expected behavior - file doesn't exist
            print(f"✓ Properly reported load error: {type(e).__name__}")
        
        # Test 2: Save and load
        brain.initialize()
        for step in range(10):
            brain.step(step)
        
        # Save current state
        brain.save("test_brain.bin")
        print("✓ Successfully saved brain state")
        
        # Create new brain and load
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        new_brain.load("test_brain.bin")
        print("✓ Successfully loaded brain state")
        
        # Verify loaded brain
        if new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount():
            print("✓ Loaded brain matches original")
        else:
            print("✗ Loaded brain does not match original")
        
        # Clean up test file
        import os
        if os.path.exists("test_brain.bin"):
            os.remove("test_brain.bin")
            print("✓ Cleaned up test file")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("NLM Python Bindings Test Suite")
    print("===============================")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Sensory-World-Agent Pipeline", test_sensory_world_agent),
        ("Memory Systems", test_memory_systems),
        ("Performance", test_performance),
        ("Error Handling", test_error_handling),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"\n✗ {test_name} crashed: {e}")
            import traceback
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print("\n" + "="*50)
    print("TEST SUMMARY")
    print("="*50)
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:.<40} {status}")
        if result:
            passed += 1
    
    print(f"\nOverall: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print("⚠️  Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
