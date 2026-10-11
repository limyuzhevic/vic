#!/usr/bin/env python3
"""
Test script for NLM Python bindings improvements
"""

import sys
import traceback

def test_basic_import():
    """Test basic import of pynlm"""
    print("Testing basic import...")
    try:
        import pynlm
        print(f"✓ Imported pynlm v{pynlm.__version__}")
        return pynlm
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return None

def test_basic_usage(pynlm):
    """Test basic Python usage"""
    print("\nTesting basic usage...")
    try:
        # Create a default config
        config = pynlm.createDefaultConfig()
        print(f"✓ Created default config: {config.summary()}")
        
        # Create a brain
        brain = pynlm.createDefaultBrain()
        print(f"✓ Created brain: {brain}")
        
        # Test some basic methods
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        print(f"✓ Brain has {neurons} neurons and {synapses} synapses")
        
        # Test step
        brain.step(0)
        print("✓ Brain step completed")
        
        return brain
    except Exception as e:
        print(f"✗ Basic usage test failed: {e}")
        traceback.print_exc()
        return None

def test_convenience_methods(pynlm):
    """Test new convenience methods"""
    print("\nTesting convenience methods...")
    try:
        # Create a brain
        brain = pynlm.createDefaultBrain()
        
        # Test getConnectionsInfo
        connections = brain.getConnectionsInfo()
        print(f"✓ Connections info: {len(connections)} regions found")
        
        # Test getNeuralActivityStats
        stats = brain.getNeuralActivityStats()
        print(f"✓ Neural activity stats: {len(stats)} metrics")
        
        # Test simulateStepAsync (should work like regular step)
        brain.simulateStepAsync(1)
        print("✓ Simulate step async completed")
        
        # Test configureLearningRates
        config = pynlm.createDefaultConfig()
        brain.configureLearningRates(config)
        print("✓ Configure learning rates completed")
        
        return brain
    except Exception as e:
        print(f"✗ Convenience methods test failed: {e}")
        traceback.print_exc()
        return None

def test_agent_features(pynlm):
    """Test agent-specific features"""
    print("\nTesting agent features...")
    try:
        # Create a brain and agent
        brain = pynlm.createDefaultBrain()
        agent = pynlm.createAgentBrain(brain)
        print(f"✓ Created agent: {agent}")
        
        # Test enableAllFeatures
        agent.enableAllFeatures()
        print("✓ Enabled all features")
        
        # Test getPerformanceMetrics
        metrics = agent.getPerformanceMetrics()
        print(f"✓ Performance metrics: {len(metrics)} items")
        
        # Test configureAgent
        config = pynlm.createDefaultConfig()
        agent.configureAgent(config)
        print("✓ Configured agent")
        
        return agent
    except Exception as e:
        print(f"✗ Agent features test failed: {e}")
        traceback.print_exc()
        return None

def test_world_features(pynlm):
    """Test world-specific features"""
    print("\nTesting world features...")
    try:
        # Create a world
        world = pynlm.createSimpleWorld()
        print(f"✓ Created world: {world}")
        
        # Test createDefaultEnvironment
        world.createDefaultEnvironment()
        print("✓ Created default environment")
        
        # Test createMaze
        world.createMaze(20, 20)
        print("✓ Created maze environment")
        
        # Test world methods
        width = world.getWidth()
        height = world.getHeight()
        print(f"✓ World dimensions: {width}x{height}")
        
        return world
    except Exception as e:
        print(f"✗ World features test failed: {e}")
        traceback.print_exc()
        return None

def test_factory_functions(pynlm):
    """Test factory functions"""
    print("\nTesting factory functions...")
    try:
        # Test create_learning_config
        learning_config = pynlm.create_learning_config()
        print(f"✓ Created learning config: {learning_config.summary()}")
        
        # Test create_exploration_config
        exploration_config = pynlm.create_exploration_config()
        print(f"✓ Created exploration config")
        
        # Test create_defense_config
        defense_config = pynlm.create_defense_config()
        print(f"✓ Created defense config")
        
        # Test run_simulation
        brain = pynlm.createDefaultBrain()
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        print("✓ Running short simulation (10 steps)...")
        pynlm.run_simulation(brain, world, agent, 10)
        print("✓ Simulation completed")
        
        return True
    except Exception as e:
        print(f"✗ Factory functions test failed: {e}")
        traceback.print_exc()
        return None

def test_error_handling(pynlm):
    """Test error handling"""
    print("\nTesting error handling...")
    try:
        # Test invalid operations
        try:
            # This should create an error
            invalid_config = pynlm.createDefaultConfig()
            invalid_config.set("invalid.key", "invalid_value")
            print("✓ Invalid configuration handled")
        except Exception as e:
            print(f"✓ Configuration error caught: {type(e).__name__}")
        
        # Test brain initialization error (simulate by using invalid config)
        try:
            # Create brain and then try to step without initializing
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.step(0)
            print("✓ Brain initialization handled")
        except Exception as e:
            print(f"✓ Brain error caught: {type(e).__name__}")
        
        return True
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return None

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Python Bindings Improvement Tests")
    print("=" * 60)
    
    # Test basic import
    pynlm = test_basic_import()
    if not pynlm:
        print("✗ Failed to import pynlm. Exiting.")
        return 1
    
    # Run all tests
    tests = [
        test_basic_usage,
        test_convenience_methods,
        test_agent_features,
        test_world_features,
        test_factory_functions,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test(pynlm):
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())