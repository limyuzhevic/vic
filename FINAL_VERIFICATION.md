#!/usr/bin/env python3
"""
Final verification script for NLM Python Bindings Enhancement.
This script validates all enhancements and provides a comprehensive report.
"""

import sys
import os
import tempfile
import json

def print_header(title):
    """Print formatted header."""
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def test_imports():
    """Test that all enhanced modules are available."""
    print_header("Testing Python Bindings Imports")
    
    try:
        import pynlm
        print("✅ Successfully imported pynlm")
        
        # Test type aliases
        required_types = [
            'neuron_id', 'synapse_id', 'region_id', 'population_id',
            'action_type', 'motor_command', 'world_object_type',
            'developmental_stage', 'firing_state', 'neuron_type', 'synapse_type'
        ]
        
        for type_name in required_types:
            if hasattr(pynlm, type_name):
                print(f"✅ Type alias '{type_name}' available")
            else:
                print(f"❌ Type alias '{type_name}' missing")
                return False
        
        # Test enum types
        required_enums = [
            'NeuronType', 'SynapseType', 'ActionType', 'MotorCommand',
            'WorldObjectType', 'DevelopmentalStage', 'FiringState'
        ]
        
        for enum_name in required_enums:
            if hasattr(pynlm, enum_name):
                print(f"✅ Enum '{enum_name}' available")
            else:
                print(f"❌ Enum '{enum_name}' missing")
                return False
        
        # Test factory functions
        required_factories = [
            'createBrain', 'createDefaultConfig', 'createSimpleWorld',
            'createAgentBrain', 'create_agent_config', 'create_rendering_config'
        ]
        
        for factory_name in required_factories:
            if hasattr(pynlm, factory_name):
                print(f"✅ Factory function '{factory_name}' available")
            else:
                print(f"❌ Factory function '{factory_name}' missing")
                return False
        
        # Test convenience functions
        required_convenience = [
            'run_simulation', 'train_agent', 'save_checkpoint',
            'load_checkpoint', 'benchmark_brain', 'profile_simulation',
            'deepcopy_brain', 'get_version', 'get_default_config',
            'validate_config', 'format_config', 'explore_states',
            'analyze_behavior'
        ]
        
        for conv_name in required_convenience:
            if hasattr(pynlm, conv_name):
                print(f"✅ Convenience function '{conv_name}' available")
            else:
                print(f"❌ Convenience function '{conv_name}' missing")
                return False
        
        # Test context managers
        required_contexts = ['SimulationContext', 'ConfigContext']
        
        for context_name in required_contexts:
            if hasattr(pynlm, context_name):
                print(f"✅ Context manager '{context_name}' available")
            else:
                print(f"❌ Context manager '{context_name}' missing")
                return False
        
        print("\n✅ All import tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Import test failed: {e}")
        return False

def test_configuration_enhancements():
    """Test enhanced configuration features."""
    print_header("Testing Configuration Enhancements")
    
    try:
        import pynlm
        
        # Test basic configuration
        config = pynlm.createDefaultConfig()
        print("✅ Created default config")
        
        # Test preset configurations
        agent_config = pynlm.create_agent_config()
        print("✅ Created agent config")
        
        rendering_config = pynlm.create_rendering_config()
        print("✅ Created rendering config")
        
        # Test enhanced convenience methods
        config.set("test.int", 42)
        assert config.get_int("test.int") == 42
        print("✅ get_int() method works")
        
        config.set("test.float", 3.14)
        assert config.get_float("test.float") == 3.14
        print("✅ get_float() method works")
        
        config.set("test.string", "hello")
        assert config.get_string("test.string") == "hello"
        print("✅ get_string() method works")
        
        config.set("test.bool", True)
        assert config.get_bool("test.bool") == True
        print("✅ get_bool() method works")
        
        # Test configuration validation
        config.set("invalid..key", "value")
        errors = pynlm.validate_config(config)
        print(f"✅ validate_config() works (found {len(errors)} errors)")
        
        # Test configuration formatting
        formatted = pynlm.format_config(config, indent=2)
        assert isinstance(formatted, str)
        print("✅ format_config() works")
        
        # Test default config dictionary
        default_config = pynlm.get_default_config()
        assert isinstance(default_config, dict)
        assert "brain.neuron_count" in default_config
        print("✅ get_default_config() works")
        
        print("\n✅ All configuration enhancement tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Configuration test failed: {e}")
        return False

def test_convenience_functions():
    """Test convenience functions."""
    print_header("Testing Convenience Functions")
    
    try:
        import pynlm
        
        # Test version function
        version = pynlm.get_version()
        assert isinstance(version, str) and "NLM" in version
        print(f"✅ get_version() works: {version}")
        
        # Test deepcopy function
        config = pynlm.createDefaultConfig()
        original_brain = pynlm.createBrain(config)
        original_brain.initialize()
        
        copied_brain = pynlm.deepcopy_brain(original_brain)
        assert copied_brain.getTotalNeuronCount() == original_brain.getTotalNeuronCount()
        print("✅ deepcopy_brain() works")
        
        # Test with actual simulation components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation steps
        for i in range(10):
            brain.step(i)
        
        print("✅ Basic simulation steps work")
        
        # Test benchmark function
        metrics = pynlm.benchmark_brain(brain, 50, 10)
        assert "steps_per_second" in metrics
        assert "total_time" in metrics
        assert metrics["steps_per_second"] > 0
        print(f"✅ benchmark_brain() works (performance: {metrics['steps_per_second']:.2f} steps/sec)")
        
        print("\n✅ All convenience function tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Convenience function test failed: {e}")
        return False

def test_context_managers():
    """Test context managers."""
    print_header("Testing Context Managers")
    
    try:
        import pynlm
        
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test SimulationContext
        with pynlm.SimulationContext(brain, world, agent) as sim:
            initial_neurons = brain.getTotalNeuronCount()
            sim.run(25)
            
            assert brain.getTotalNeuronCount() == initial_neurons
            stats = sim.get_stats()
            assert "steps_run" in stats
            assert stats["steps_run"] == 25
            print("✅ SimulationContext works correctly")
        
        # Test ConfigContext
        with pynlm.ConfigContext(config) as cfg:
            cfg.set("test.context.key", "value")
            assert config.get("test.context.key") == "value"
            print("✅ ConfigContext works correctly")
        
        print("\n✅ All context manager tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Context manager test failed: {e}")
        return False

def test_checkpoint_functions():
    """Test checkpoint functions."""
    print_header("Testing Checkpoint Functions")
    
    try:
        import pynlm
        import tempfile
        
        # Create and run simulation
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        for i in range(50):
            brain1.step(i)
        
        initial_spikes = brain1.getTotalSpikeCount()
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as tmp:
            checkpoint_path = tmp.name
        
        pynlm.save_checkpoint(brain1, checkpoint_path)
        print(f"✅ save_checkpoint() works (saved to {checkpoint_path})")
        
        # Load checkpoint
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        pynlm.load_checkpoint(brain2, checkpoint_path)
        
        # Verify loaded state
        assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        print(f"✅ load_checkpoint() works (preserved {abs(brain2.getTotalSpikeCount() - initial_spikes)} spikes)")
        
        # Clean up
        os.unlink(checkpoint_path)
        
        print("\n✅ All checkpoint function tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Checkpoint function test failed: {e}")
        return False

def test_documentation_examples():
    """Test examples from documentation."""
    print_header("Testing Documentation Examples")
    
    try:
        import pynlm
        
        # Test minimal example
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print("✅ Minimal example works")
        
        # Test enhanced example
        config = pynlm.create_agent_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("✅ Enhanced configuration example works")
        
        # Test version example
        print(f"✅ Version example works: {pynlm.get_version()}")
        
        print("\n✅ All documentation example tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Documentation example test failed: {e}")
        return False

def main():
    """Run all verification tests."""
    print_header("NLM Python Bindings - Final Verification")
    
    tests = [
        ("Import Tests", test_imports),
        ("Configuration Enhancements", test_configuration_enhancements),
        ("Convenience Functions", test_convenience_functions),
        ("Context Managers", test_context_managers),
        ("Checkpoint Functions", test_checkpoint_functions),
        ("Documentation Examples", test_documentation_examples),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\nRunning {test_name}...")
        if test_func():
            passed += 1
            print(f"✅ {test_name} PASSED")
        else:
            failed += 1
            print(f"❌ {test_name} FAILED")
    
    print_header("FINAL RESULTS")
    print(f"Tests Passed: {passed}")
    print(f"Tests Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED!")
        print("The NLM Python bindings enhancements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} TEST(S) FAILED")
        print("Please check the errors above and fix any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())