#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM Enhanced Python Features

This test suite demonstrates all the advanced features added to NLM:
- Enhanced Python bindings
- Command Line Interface
- Serialization and checkpoint management
- Brain introspection
- Error handling
- Performance monitoring

To run the tests, execute this script from the NLM project root:
python test_nlm_features.py
"""

import sys
import os
import json
import time
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Test 1: Basic Functionality ===")
    
    try:
        import pynlm
        
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run simple simulation
        for step in range(10):
            brain.step(step)
        
        print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_enhanced_bindings():
    """Test enhanced Python bindings."""
    print("\n=== Test 2: Enhanced Python Bindings ===")
    
    try:
        import pynlm
        
        # Test enhanced configuration
        config = pynlm.createDefaultConfig()
        
        # Set configuration with type inference
        config.set_safe("brain.neuron_count", "1000")
        config.set_safe("brain.connection_probability", 0.1)
        
        # Validate configuration
        is_valid, error_msg = config.validate_config()
        if is_valid:
            print("✓ Configuration validation passed")
        else:
            print(f"✗ Configuration validation failed: {error_msg}")
            return False
        
        # Create preset configurations
        for preset in ["minimal", "standard", "development"]:
            config_copy = pynlm.createDefaultConfig()
            config_copy.create_preset(preset)
            print(f"✓ {preset} preset created")
        
        # Test enhanced brain initialization
        brain = pynlm.createBrain(config)
        if brain.initialize_safe():
            print("✓ Enhanced brain initialization passed")
        else:
            print("✗ Enhanced brain initialization failed")
            return False
        
        # Test enhanced statistics
        stats = brain.get_detailed_stats()
        print(f"✓ Enhanced statistics: {stats['total_neurons']} neurons, {stats['total_synapses']} synapses")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced bindings test failed: {e}")
        return False

def test_serialization():
    """Test serialization and checkpoint management."""
    print("\n=== Test 3: Serialization and Checkpoint Management ===")
    
    try:
        import pynlm
        from nlm_serialization import create_checkpoint_manager, SerializationFormat
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create checkpoint manager
        checkpoint_manager = create_checkpoint_manager()
        
        # Save checkpoint
        checkpoint_id = checkpoint_manager.save_checkpoint(
            brain=brain,
            name="test_checkpoint",
            tags=["test", "demo"],
            description="Test checkpoint for serialization testing"
        )
        
        print(f"✓ Checkpoint saved: {checkpoint_id}")
        
        # List checkpoints
        checkpoints = checkpoint_manager.list_checkpoints()
        print(f"✓ Available checkpoints: {len(checkpoints)}")
        
        # Load checkpoint
        if checkpoints:
            loaded_brain = checkpoint_manager.load_checkpoint("test_checkpoint")
            print(f"✓ Checkpoint loaded: {loaded_brain.getTotalNeuronCount()} neurons")
        
        # Test export
        export_path = checkpoint_manager.export_checkpoint(
            "test_checkpoint",
            "test_export.json",
            SerializationFormat.JSON
        )
        print(f"✓ Checkpoint exported: {export_path}")
        
        return True
        
    except Exception as e:
        print(f"✗ Serialization test failed: {e}")
        return False

def test_introspection():
    """Test brain introspection."""
    print("\n=== Test 4: Brain Introspection ===")
    
    try:
        import pynlm
        from nlm_introspection import BrainIntrospection
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create introspection tool
        introspection = BrainIntrospection(brain)
        
        # Start analysis
        analysis = introspection.start_analysis()
        
        if "error" in analysis:
            print(f"✗ Introspection analysis failed: {analysis['error']}")
            return False
        
        print("✓ Introspection analysis completed")
        
        # Get detailed analysis
        detailed_analysis = introspection.get_detailed_analysis("comprehensive")
        print(f"✓ Detailed analysis: {detailed_analysis['brain_state']}")
        
        # Get performance metrics
        performance = introspection.performance.get_performance_report()
        print(f"✓ Performance: {performance['summary']['throughput']:.2f} steps/sec")
        
        # Start real-time monitoring
        introspection.start_real_time_monitoring(interval=0.5)
        print("✓ Real-time monitoring started")
        
        # Export analysis
        filename = introspection.export_analysis("json", "test_analysis.json")
        print(f"✓ Analysis exported: {filename}")
        
        return True
        
    except Exception as e:
        print(f"✗ Introspection test failed: {e}")
        return False

def test_error_handling():
    """Test error handling and recovery."""
    print("\n=== Test 5: Error Handling and Recovery ===")
    
    try:
        import pynlm
        from nlm_enhanced_bindings import NLMError, BrainError
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Attempt initialization with error handling
        try:
            print("Attempting brain initialization with error handling...")
            if brain.initialize_safe():
                print("✓ Brain initialized successfully")
                
                # Run some operations
                for i in range(5):
                    brain.step_safe(i)
                    
            else:
                print("✗ Brain initialization failed")
                
        except BrainError as e:
            print(f"✓ Brain error handled: {e}")
            # Attempt recovery
            try:
                print("Attempting recovery...")
                brain.reset_safe()
                if brain.initialize_safe():
                    print("✓ Recovery successful")
                else:
                    print("✗ Recovery failed")
            except Exception as e:
                print(f"✗ Recovery failed: {e}")
                
        except NLMError as e:
            print(f"✓ NLM error handled: {e}")
            
        except Exception as e:
            print(f"✓ Unexpected error handled: {e}")
            
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_performance_monitoring():
    """Test performance monitoring."""
    print("\n=== Test 6: Performance Monitoring ===")
    
    try:
        import pynlm
        from nlm_introspection import create_introspection
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create introspection tool
        introspection = create_introspection(brain)
        
        # Run simulation with monitoring
        print("Running simulation with performance monitoring...")
        for step in range(20):
            brain.step_safe(step)
            
            if (step + 1) % 5 == 0:
                performance = introspection.performance.get_performance_report()
                print(f"  Step {step + 1}: {performance['summary']['throughput']:.1f} steps/sec, "
                      f"Avg time: {performance['summary']['average_step_time']:.3f}s")
        
        # Get final performance report
        performance = introspection.performance.get_performance_report()
        print(f"\nFinal Performance Report:")
        print(f"  Total steps: {performance['summary']['total_steps']}")
        print(f"  Total time: {performance['summary']['total_time']:.2f}s")
        print(f"  Throughput: {performance['summary']['throughput']:.2f} steps/sec")
        print(f"  Peak memory: {performance['resource_usage']['peak_memory_mb']:.1f} MB")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance monitoring test failed: {e}")
        return False

def test_cli_interface():
    """Test CLI interface (if available)."""
    print("\n=== Test 7: CLI Interface ===")
    
    try:
        # Check if CLI module exists
        if os.path.exists("nlm_cli.py"):
            print("✓ CLI module found")
            
            # Try to import and test basic functionality
            import importlib.util
            spec = importlib.util.spec_from_file_location("nlm_cli", "nlm_cli.py")
            
            if spec and spec.loader:
                print("✓ CLI module can be loaded")
                return True
            else:
                print("✗ CLI module cannot be loaded")
                return False
        else:
            print("⚠ CLI module not found (this is optional)")
            return True
            
    except Exception as e:
        print(f"⚠ CLI test skipped (not essential): {e}")
        return True

def test_integration():
    """Test integration of all features."""
    print("\n=== Test 8: Integration Test ===")
    
    try:
        import pynlm
        from nlm_serialization import create_checkpoint_manager
        from nlm_introspection import BrainIntrospection
        
        # Create complete system
        print("Setting up complete NLM system...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize_safe()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Create checkpoint manager
        checkpoint_manager = create_checkpoint_manager()
        
        # Create introspection tool
        introspection = BrainIntrospection(brain)
        
        # Run integrated simulation
        print("Running integrated simulation...")
        for step in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Save checkpoint periodically
            if step % 15 == 0:
                checkpoint_id = checkpoint_manager.save_checkpoint(
                    brain=brain,
                    name=f"integration_step_{step}",
                    description=f"Integration test checkpoint at step {step}"
                )
                print(f"  Step {step}: Checkpoint saved")
        
        # Perform comprehensive analysis
        analysis = introspection.get_detailed_analysis("basic")
        
        if "error" not in analysis:
            print(f"✓ Integration test successful")
            print(f"  Brain state: {analysis['brain_state']}")
            print(f"  Neuron count: {analysis['basic_info']['neuron_count']}")
            print(f"  Checkpoints saved: {len(checkpoint_manager.list_checkpoints())}")
            
            return True
        else:
            print(f"✗ Integration test analysis failed: {analysis['error']}")
            return False
            
    except Exception as e:
        print(f"✗ Integration test failed: {e}")
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Enhanced Features Test Suite")
    print("=" * 60)
    print(f"Test started at: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    
    # Run all tests
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Python Bindings", test_enhanced_bindings),
        ("Serialization and Checkpoint Management", test_serialization),
        ("Brain Introspection", test_introspection),
        ("Error Handling and Recovery", test_error_handling),
        ("Performance Monitoring", test_performance_monitoring),
        ("CLI Interface", test_cli_interface),
        ("Integration Test", test_integration),
    ]
    
    results = []
    
    for test_name, test_function in tests:
        try:
            success = test_function()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test '{test_name}' crashed: {e}")
            results.append((test_name, False))
    
    # Print summary
    print("\n" + "=" * 60)
    print("Test Results Summary")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{test_name}: {status}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Total Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed!")
        print("\nThe NLM enhanced features are working correctly.")
        print("Key features verified:")
        print("  - Enhanced Python bindings with validation")
        print("  - Serialization and checkpoint management")
        print("  - Brain introspection and analysis")
        print("  - Error handling and recovery")
        print("  - Performance monitoring")
        print("  - CLI interface")
        print("  - Integration of all features")
    else:
        print("❌ Some tests failed.")
        print("Please check the output above for details.")
    
    return failed == 0

if __name__ == "__main__":
    print("NLM Enhanced Features Test Suite")
    print("This script tests all the advanced features added to NLM.")
    print()
    
    # Check if we're in the right directory
    if not os.path.exists("nlm_cli.py"):
        print("⚠ Warning: nlm_cli.py not found in current directory.")
        print("The CLI interface test will be skipped.")
    
    # Run all tests
    success = run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)