#!/usr/bin/env python3
"""
Test script for enhanced Python bindings
"""

import sys
import os

def test_imports():
    """Test that all enhanced bindings can be imported"""
    print("Testing imports...")
    
    try:
        import pynlm as enhanced
        print("  ✓ pynlm (enhanced) imported successfully")
        
        # Check if Config class exists
        if hasattr(enhanced, 'Config'):
            print("  ✓ Config class available")
        else:
            print("  ✗ Config class not found")
            return False
            
        # Check if Brain class exists
        if hasattr(enhanced, 'Brain'):
            print("  ✓ Brain class available")
        else:
            print("  ✗ Brain class not found")
            return False
            
        # Check if create_brain factory function exists
        if hasattr(enhanced, 'create_brain'):
            print("  ✓ create_brain factory available")
        else:
            print("  ✗ create_brain factory not found")
            return False
            
    except ImportError as e:
        print(f"  ✗ Failed to import pynlm: {e}")
        return False
    
    try:
        import pynlm_original as original
        print("  ✓ pynlm_original (original) imported successfully")
        
        # Check if Config class exists
        if hasattr(original, 'Config'):
            print("  ✓ Original Config class available")
        else:
            print("  ✗ Original Config class not found")
            return False
            
    except ImportError as e:
        print(f"  ✗ Failed to import pynlm_original: {e}")
        return False
    
    return True

def test_config():
    """Test enhanced Config functionality"""
    print("\nTesting enhanced Config...")
    
    try:
        import pynlm as nlm
        
        # Test basic creation
        config = nlm.Config()
        print("  ✓ Config created successfully")
        
        # Test dictionary-like interface
        config["neuron_count"] = 1000
        config["region_count"] = 2
        config["random_seed"] = 42
        print("  ✓ Dictionary-like interface working")
        
        # Test get_or method
        neurons = config.get_or("neuron_count", 500)
        if neurons == 1000:
            print("  ✓ get_or method working")
        else:
            print(f"  ✗ get_or method failed: expected 1000, got {neurons}")
            return False
        
        # Test has method
        if config.has("neuron_count"):
            print("  ✓ has method working")
        else:
            print("  ✗ has method failed")
            return False
        
        # Test keys method
        keys = config.get_keys()
        if "neuron_count" in keys and "region_count" in keys:
            print("  ✓ get_keys method working")
        else:
            print(f"  ✗ get_keys method failed: {keys}")
            return False
        
        # Test update method
        config.update({"new_param": "test_value"})
        if config.get_or("new_param") == "test_value":
            print("  ✓ update method working")
        else:
            print("  ✗ update method failed")
            return False
        
        # Test summary
        summary = config.summary()
        if "Configuration" in summary:
            print("  ✓ summary method working")
        else:
            print("  ✗ summary method failed")
            return False
        
    except Exception as e:
        print(f"  ✗ Config test failed: {e}")
        return False
    
    return True

def test_brain_creation():
    """Test enhanced Brain creation"""
    print("\nTesting enhanced Brain creation...")
    
    try:
        import pynlm as nlm
        
        # Create config
        config = nlm.Config()
        config.update({
            "neuron_count": 200,
            "region_count": 1,
            "random_seed": 123
        })
        
        # Test with context manager
        with nlm.Brain(config) as brain:
            print("  ✓ Brain context manager working")
            
            # Test basic properties
            if brain.get_total_neuron_count() > 0:
                print("  ✓ Total neuron count accessible")
            else:
                print("  ✗ Total neuron count not accessible")
                return False
                
            if brain.get_total_synapse_count() >= 0:
                print("  ✓ Total synapse count accessible")
            else:
                print("  ✗ Total synapse count not accessible")
                return False
                
            # Test step functionality
            brain.step(0)
            print("  ✓ Brain step working")
        
        print("  ✓ Brain automatically reset after context")
        
    except Exception as e:
        print(f"  ✗ Brain test failed: {e}")
        return False
    
    return True

def test_agent_brain():
    """Test enhanced AgentBrain"""
    print("\nTesting enhanced AgentBrain...")
    
    try:
        import pynlm as nlm
        
        # Create components
        brain = nlm.create_brain(nlm.create_config())
        agent = nlm.create_agent_brain(brain)
        
        # Test basic methods
        if agent.get_sensory_input_size() > 0:
            print("  ✓ Sensory input size accessible")
        else:
            print("  ✗ Sensory input size not accessible")
            return False
            
        if agent.get_motor_output_size() > 0:
            print("  ✓ Motor output size accessible")
        else:
            print("  ✗ Motor output size not accessible")
            return False
        
        # Test enable/disable methods
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        print("  ✓ Enable methods working")
        
        # Test is_* methods
        if agent.is_reward_modulation_enabled():
            print("  ✓ is_reward_modulation_enabled working")
        else:
            print("  ✗ is_reward_modulation_enabled failed")
            return False
            
        if agent.is_curiosity_enabled():
            print("  ✓ is_curiosity_enabled working")
        else:
            print("  ✗ is_curiosity_enabled failed")
            return False
        
        # Test developmental stage
        stage = agent.get_developmental_stage()
        print(f"  ✓ Developmental stage: {stage}")
        
    except Exception as e:
        print(f"  ✗ AgentBrain test failed: {e}")
        return False
    
    return True

def run_examples():
    """Run some basic examples"""
    print("\nRunning basic examples...")
    
    try:
        import pynlm as nlm
        import time
        
        # Example 1: Basic brain simulation
        print("  Example 1: Basic brain simulation")
        config = nlm.Config()
        config.update({"neuron_count": 100, "region_count": 1})
        
        with nlm.Brain(config) as brain:
            start_time = time.time()
            brain.step(0)
            end_time = time.time()
            
            print(f"    ✓ Brain step completed in {end_time - start_time:.4f}s")
            print(f"    ✓ Neurons: {brain.get_total_neuron_count()}")
        
        # Example 2: Config dictionary interface
        print("  Example 2: Config dictionary interface")
        config2 = nlm.Config()
        config2["test_param"] = "value"
        config2["another_param"] = 123
        
        if config2.get_or("test_param") == "value" and config2.get_or("another_param") == 123:
            print("    ✓ Dictionary interface working")
        else:
            print("    ✗ Dictionary interface failed")
            return False
        
        # Example 3: Create with factory functions
        print("  Example 3: Factory functions")
        config3 = nlm.create_default_config()
        if config3:
            print("    ✓ create_default_config working")
        else:
            print("    ✗ create_default_config failed")
            return False
        
    except Exception as e:
        print(f"  ✗ Examples failed: {e}")
        return False
    
    return True

def main():
    """Run all tests"""
    print("Enhanced NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Import Test", test_imports),
        ("Config Test", test_config),
        ("Brain Creation Test", test_brain_creation),
        ("AgentBrain Test", test_agent_brain),
        ("Examples Test", run_examples),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} FAILED with exception: {e}")
    
    print(f"\n{'='*50}")
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
