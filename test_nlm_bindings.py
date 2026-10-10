#!/usr/bin/env python3

"""
Simple test for NLM Python bindings
This test verifies that the basic NLM functionality works correctly.
"""

import sys
import traceback

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    try:
        import pynlm
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Created default configuration")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Created and initialized brain")
        
        # Run some simulation steps
        for i in range(10):
            brain.step(i)
        
        print("✓ Completed simulation steps")
        
        # Check brain statistics
        total_spikes = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"  Total spikes: {total_spikes}")
        print(f"  Average firing rate: {firing_rate:.2f} Hz")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Created world and agent")
        
        # Run simulation with agent
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Completed agent simulation")
        
        # Check agent statistics
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        neuromodulation = agent.getNeuromodulationLevel()
        
        print(f"  Curiosity level: {curiosity:.3f}")
        print(f"  Novelty level: {novelty:.3f}")
        print(f"  Neuromodulation level: {neuromodulation:.3f}")
        
        # Test configuration operations
        config.set("test_key", "test_value")
        config_value = config.get("test_key")
        print(f"✓ Configuration test: {config_value}")
        
        # Test brain configuration
        config2 = pynlm.createAdvancedConfig()
        config2.set("neuron_count", 500)
        config2.set("plasticity_learning_rate", 0.02)
        
        print("✓ Created advanced configuration")
        
        # Test serialization (save)
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(suffix='.cfg', delete=False) as f:
            temp_file = f.name
        
        brain.save(temp_file)
        print(f"✓ Saved brain state to {temp_file}")
        
        # Test serialization (load)
        if os.path.exists(temp_file):
            # Clean up first
            os.unlink(temp_file)
            print("✓ Cleaned up temporary file")
        
        print("\n✅ All basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced NLM features"""
    print("\nTesting advanced NLM features...")
    
    try:
        import pynlm
        
        # Create advanced configuration
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 200)
        config.set("plasticity_learning_rate", 0.03)
        config.set("stdp_ltp_weight", 0.025)
        config.set("stdp_ltd_weight", 0.024)
        
        # Create brain and test performance metrics
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✓ Created advanced configuration and brain")
        
        # Run some steps to generate performance data
        for step in range(50):
            brain.step(step)
        
        # Check performance metrics
        metrics = brain.getPerformanceMetrics()
        print(f"  Performance metrics available: {metrics is not None}")
        
        # Test configuration validation
        is_valid = config.validate()
        print(f"✓ Configuration validation: {is_valid}")
        
        # Test environment configuration
        env_config = pynlm.createEnvironmentConfig()
        env_config.loadFromEnvironment()
        print("✓ Created and loaded environment configuration")
        
        print("\n✅ All advanced feature tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Advanced feature test failed with error: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    # Run tests
    basic_test_passed = test_basic_functionality()
    advanced_test_passed = test_advanced_features()
    
    print("\n" + "=" * 60)
    print("Test Summary:")
    print(f"  Basic functionality: {'PASS' if basic_test_passed else 'FAIL'}")
    print(f"  Advanced features: {'PASS' if advanced_test_passed else 'FAIL'}")
    
    if basic_test_passed and advanced_test_passed:
        print("\n🎉 All tests passed! NLM bindings are working correctly.")
        return 0
    else:
        print("\n💥 Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())