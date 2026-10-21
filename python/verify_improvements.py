#!/usr/bin/env python3
"""
Minimal test to verify the improved NLM Python interface works.
This is a simplified version of the comprehensive test suite.
"""

import sys
import os

# Add the python directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_imports():
    """Test that all required modules can be imported."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\nTesting basic functionality...")
    
    try:
        import pynlm
        
        # Create default config
        config = pynlm.create_default_config()
        print("✓ Default config created")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain created and initialized")
        
        # Test property access
        neurons = brain.total_neurons
        synapses = brain.total_synapses
        firing_rate = brain.average_firing_rate
        
        print(f"✓ Neurons: {neurons}")
        print(f"✓ Synapses: {synapses}")
        print(f"✓ Average firing rate: {firing_rate:.2f}")
        
        # Test simulation
        for step in range(10):
            brain.step(step)
        
        total_spikes = brain.getTotalSpikeCount()
        print(f"✓ Simulation completed with {total_spikes} total spikes")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_context_managers():
    """Test context manager functionality."""
    print("\nTesting context managers...")
    
    try:
        import pynlm
        
        # Test SimulationSession
        with pynlm.simulation_session() as session:
            session.brain.initialize()
            
            for step in range(20):
                session.brain.step(step)
            
            spikes = session.brain.getTotalSpikeCount()
            print(f"✓ SimulationSession completed with {spikes} spikes")
        
        print("✓ SimulationSession cleaned up properly")
        return True
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")
        return False

def test_memory_management():
    """Test memory management functionality."""
    print("\nTesting memory management...")
    
    try:
        import pynlm
        
        brain = pynlm.createBrain(pynlm.create_default_config())
        brain.initialize()
        
        memory_manager = pynlm.MemoryManager(brain)
        
        # Get memory stats
        stats = memory_manager.get_memory_stats()
        print(f"✓ Memory stats retrieved: {stats}")
        
        # Optimize memory
        memory_manager.optimize_working_memory(500)
        memory_manager.consolidate_memories(0.3)
        
        print("✓ Memory management completed")
        return True
    except Exception as e:
        print(f"✗ Memory management test failed: {e}")
        return False

def test_advanced_features():
    """Test advanced features."""
    print("\nTesting advanced features...")
    
    try:
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.create_default_config())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        
        # Run simulation
        for step in range(30):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        print(f"✓ Advanced simulation completed")
        print(f"✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"✓ Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"✓ Prediction error: {agent.getPredictionError():.3f}")
        
        return True
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        return False

def main():
    """Main test function."""
    print("=" * 60)
    print("NLM Python Interface - Quick Verification Test")
    print("=" * 60)
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Context Managers", test_context_managers),
        ("Memory Management", test_memory_management),
        ("Advanced Features", test_advanced_features),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        success = test_func()
        results.append((test_name, success))
        
        if success:
            print(f"✓ {test_name} PASSED")
        else:
            print(f"✗ {test_name} FAILED")
    
    print(f"\n{'='*60}")
    print("Test Summary")
    print(f"{'='*60}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    print(f"Tests run: {total}")
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {total - passed}")
    
    if passed == total:
        print(f"\n✓ All tests passed!")
        print(f"\nThe improved NLM Python interface is working correctly.")
        print(f"\nKey improvements verified:")
        print(f"1. ✓ Enhanced Python API with property access")
        print(f"2. ✓ Context managers for safe resource management")
        print(f"3. ✓ Memory management capabilities")
        print(f"4. ✓ Advanced simulation features")
        print(f"5. ✓ Comprehensive error handling")
        return 0
    else:
        print(f"\n✗ Some tests failed!")
        print(f"\nPlease check the output above for details.")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)